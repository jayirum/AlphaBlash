
#include "Dispatch.h"
#include <process.h>
#include "../Common/Util.h"
//#include "MemPool.h"
#include "../Common/LogMsg.h"
#include "../Common/IRExcept.h"
#include "../Common/IRUM_Common.h"
#include "../Common/CommonFunc.h"

extern CLogMsg	g_log;
extern wchar_t	g_wzConfig[_MAX_PATH];
extern BOOL		g_bDebugLog;

CDispatch::CDispatch()
{
	m_hCompletionPort	= NULL;
	m_hListenEvent		= NULL;
	m_sockListen		= INVALID_SOCKET;
	m_hListenThread		= NULL;
	m_dListenThread		= 0;
	m_nListenPort		= 0;
	m_hListenEvent		= NULL;
	m_bRun				= TRUE;
	m_pDBPool			= NULL;
	m_lIocpThreadIdx = 0;
}
CDispatch::~CDispatch()
{
	Finalize();
}

BOOL CDispatch::ReadIPPOrt()
{
	wchar_t wzTemp[128] = { 0, };
	CUtil::GetConfig(g_wzConfig, TEXT("NETWORK"), TEXT("LISTEN_IP"), m_wzListenIP);
	CUtil::GetConfig(g_wzConfig, TEXT("NETWORK"), TEXT("LISTEN_PORT"), wzTemp);
	m_nListenPort = _ttoi(wzTemp);
	
	return TRUE;
}

BOOL CDispatch::Initialize( )
{
	ReadIPPOrt();

	InitializeCriticalSection(&m_csCK);
	InitializeCriticalSection(&m_csEA);
	InitializeCriticalSection(&m_csQ);
	

	if (!InitListen()) {
		g_log.logW(ERR, TEXT("Init Listen Failed"));
		return FALSE;
	}
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, &ListenThread, this, 0, &m_dListenThread);

	// CPU의 수를 알기 위해서....IOCP에서 사용할 쓰레드의 수는 cpu개수 또는 cpu*2
	SYSTEM_INFO         systemInfo;
	int nThreadCnt;
	GetSystemInfo(&systemInfo);
	nThreadCnt = systemInfo.dwNumberOfProcessors;
	//if (nThreadCnt >= m_nMaxThreadCnt)
	//	m_dwThreadCount = m_nMaxThreadCnt;
	//// = 2;

	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		g_log.logW(ERR, TEXT("IOCP Create Error:%d"), GetLastError());
		return FALSE;
	}

	// 실제로 recv와 send를 담당할 스레드를 생성한다.
	unsigned int dwID;
	for (int n = 0; n < nThreadCnt; n++)
	{
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &IocpWorkerThread, this, 0, &dwID);
		CloseHandle(h);
	}

	// DB SAVE
	m_hSaveThread = (HANDLE)_beginthreadex(NULL, 0, &DBSaveThread, this, 0, &m_dSaveThread);

	// QTable Read
	m_hQThread = (HANDLE)_beginthreadex(NULL, 0, &QReadThread, this, 0, &m_dQThread);

	return TRUE;
}

BOOL CDispatch::InitListen()
{
	g_log.logW(INFO, TEXT("CDispatch::InitListen() starts.........."));
	CloseListenSock();

	WORD	wVersionRequired;
	WSADATA	wsaData;

	//// WSAStartup
	wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		g_log.logW(LOGTP_ERR, TEXT("WSAStartup Error:%d"), GetLastError());
		return FALSE;
	}

	//DumpWsaData(&wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		g_log.logW(LOGTP_ERR, TEXT("RequiredVersion not Usable"));
		return FALSE;
	}


	// Create a listening socket 
	if ((m_sockListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		g_log.logW(ERR, TEXT("create socket error: %d"), WSAGetLastError());
		return FALSE;
	}

	ReadIPPOrt();

	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = _tinet_addr(m_wzListenIP);
	InternetAddr.sin_port = htons(m_nListenPort);

	BOOL opt = TRUE;
	int optlen = sizeof(opt);
	setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (const char far *)&opt, optlen);

	if (::bind(m_sockListen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		g_log.logW(ERR, TEXT("bind error (ip:%s) (port:%d) (err:%d)"), m_wzListenIP, m_nListenPort, WSAGetLastError());
		return FALSE;
	}
	// Prepare socket for listening 
	if (listen(m_sockListen, 5) == SOCKET_ERROR)
	{
		g_log.logW(ERR, TEXT("listen error: %d"), WSAGetLastError());
		return FALSE;
	}

	m_hListenEvent = WSACreateEvent();
	if (WSAEventSelect(m_sockListen, m_hListenEvent, FD_ACCEPT)) {

		g_log.logW(ERR, TEXT("WSAEventSelect for accept error: %d"), WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

/*
BOOL WINAPI dQueuedCompletionStatus(
_In_     HANDLE       CompletionPort,
_In_     DWORD        dwNumberOfBytesTransferred,
_In_     ULONG_PTR    dwCompletionKey,
_In_opt_ LPOVERLAPPED lpOverlapped
);
*/


void CDispatch::CloseListenSock()
{
	SAFE_CLOSEHANDLE(m_hListenEvent);
	if (m_sockListen != INVALID_SOCKET) {
		struct linger ling;
		ling.l_onoff = 1;   // 0 ? use default, 1 ? use new value
		ling.l_linger = 0;  // close session in this time
		setsockopt(m_sockListen, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));
		//-We can avoid TIME_WAIT on both of client and server side as we code above.
		closesocket(m_sockListen);
	}
	SAFE_CLOSESOCKET(m_sockListen);
}

VOID CDispatch::SendMessageToIocpThread(int Message)
{
	for( int i=0; i<WORKTHREAD_CNT; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			,0
			, (ULONG_PTR)Message
			, NULL
		);
	}
}

void CDispatch::Finalize()
{
	m_bRun = FALSE;

	for (UINT i = 0; i < WORKTHREAD_CNT; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			, 0
			, NULL
			, NULL
		);
	}

	lockCK();
	std::map<std::string, COMPLETION_KEY*>::iterator it;
	for (it = m_mapCK.begin(); it != m_mapCK.end(); ++it)
	{
		COMPLETION_KEY* pCK = (*it).second;
		shutdown(pCK->sock, SD_BOTH);

		// TIME-WAIT 없도록
		struct linger structLinger;
		structLinger.l_onoff	= TRUE;
		structLinger.l_linger	= 0;

		setsockopt(pCK->sock, SOL_SOCKET, SO_LINGER, (LPSTR)&structLinger, sizeof(structLinger));
		closesocket(pCK->sock);
		
		delete pCK;
	}
	m_mapCK.clear();
	unlockCK();

	CloseListenSock();

	//lockRecvData();
	//m_lstRecvData.clear();
	//unlockRecvData();
	SAFE_CLOSEHANDLE(m_hCompletionPort);

	DeleteCriticalSection(&m_csCK);
	DeleteCriticalSection(&m_csEA);
	DeleteCriticalSection(&m_csQ);

	//SAFE_DELETE(m_pack);
	WSACleanup();

	delete m_pDBPool;
}

// client 의 소켓이 close 되는 경우 처리
//void CDispatch::RecvLogOffAndClose(COMPLETION_KEY *pCompletionKey)
//{
//	if (pCompletionKey->sUserID.size() == 0)
//		return;
//
//	if (pCompletionKey->sUserID.size() == 0)
//	{
//		g_log.log(INFO, "[RecvLogOffAndClose]ID is empty");
//		return;
//	}
//
//	EnterCriticalSection(&m_csEA);
//	map<string, CMasterChannel*>::iterator it = m_mapEA.find(pCompletionKey->sUserID);
//
//	// MASTER 인 경우
//	if (it != m_mapEA.end())
//	{
//		delete (*it).second;
//		m_mapEA.erase(it);
//	}
//	// SLAVE 인 경우
//	else
//	{
//		for (it = m_mapEA.begin(); it != m_mapEA.end(); it++)
//		{
//			BOOL bAlreadClosed	= TRUE;
//			BOOL bMaster		= FALSE;
//			((*it).second)->ForceCloseOneID(pCompletionKey->sUserID, bMaster, bAlreadClosed);
//		}
//	}
//
//	LeaveCriticalSection(&m_csEA);
//}


void CDispatch::Recv_CloseEvent_FromEA(COMPLETION_KEY *pCompletionKey)
{
	BOOL bMaster;

	if (pCompletionKey->sUserID.size() > 0)
	{
		EnterCriticalSection(&m_csEA);
		map<STR_MASTER_ID, CMasterChannel*>::iterator it = m_mapEA.find(pCompletionKey->sUserID);
		//TODO
		g_log.log(DEBUG_, "Dup LogOff-1(%s)", pCompletionKey->sUserID.c_str());
		bMaster = (it != m_mapEA.end());
		// MASTER 인 경우
		if (bMaster)
		{
			CMasterChannel* p = (*it).second;
			p->Remove_Master(pCompletionKey->sUserID, pCompletionKey->sock);
			g_log.log(DEBUG_, "Dup LogOff-2(%s)", pCompletionKey->sUserID.c_str());
		}
		// Copier 인 경우
		else if (bMaster==FALSE)
		{
			// 전체 EA map 중 해당 COPIER 가 있는 map 에서 제거
			for (it = m_mapEA.begin(); it != m_mapEA.end(); it++)
			{
				((*it).second)->Remove_Copier(pCompletionKey->sUserID, pCompletionKey->sock);
			}
		}
		LeaveCriticalSection(&m_csEA);
	}


	lockCK();

	char zSock[128];
	CVT_SOCKET(pCompletionKey->sock, zSock);

	std::map<STR_SOCKET, COMPLETION_KEY*>::iterator it = m_mapCK.find(std::string(zSock));
	if (it != m_mapCK.end())
	{
		shutdown(pCompletionKey->sock, SD_BOTH);

		// TIME-WAIT 없도록
		struct linger structLinger;
		structLinger.l_onoff = TRUE;
		structLinger.l_linger = 0;
		setsockopt(pCompletionKey->sock, SOL_SOCKET, SO_LINGER, (LPSTR)& structLinger, sizeof(structLinger));
		closesocket(pCompletionKey->sock);
		delete pCompletionKey;
		m_mapCK.erase(it);
	}
	unlockCK();
}


/*
1.	WSASend / WSARecv 를 호출한 경우는 사용한 socket 과 CK 가 연결되어 있으므로
pCompletionKey 를 통해서 CK 의 포인터가 나온다.

2.	PostQueuedCompletionStatus 를 호출한 경우는 socket 이 사용되지 않으므로
이때는 WM_MSG 를 보내도록 한다.

3.	확장된 OVERLAPPED 에 context 필드가 있으므로 여기에 CTX_DIE, CTX_RQST_SEND, CTX_RQST_RECV 를 채워서 보낸다.

*/
unsigned WINAPI CDispatch::IocpWorkerThread(LPVOID lp)
{
	CDispatch* pThis = (CDispatch*)lp;

	COMPLETION_KEY	*pCompletionKey = NULL;
	IO_CONTEXT		*pIoContext = NULL;
	DWORD			dwBytesTransferred = 0;
	DWORD           dwIoSize = 0;
	DWORD           dwRecvNumBytes = 0;
	DWORD           dwSendNumBytes = 0;
	DWORD           dwFlags = 0;
	LPOVERLAPPED	pOverlap = NULL;
	BOOL bRet;
	char			zRecvBuff[BUF_LEN];
	int nLoop = 0;
	int nRet = 0;
	
	long iocpIdx = InterlockedIncrement(&pThis->m_lIocpThreadIdx)-1;

	g_log.logW(INFO, TEXT("[%d][%d]IOCPThread Start....."), iocpIdx, GetCurrentThreadId());

	while (pThis->m_bRun)
	{
		bRet = GetQueuedCompletionStatus(pThis->m_hCompletionPort,
			&dwIoSize,
			(LPDWORD)&pCompletionKey, 	//여기로는 실제 CK 는 던지지 않는다. 무조건 new, delete 하므로. 
			(LPOVERLAPPED *)&pOverlap,
			INFINITE);

		// Finalize 에서 PostQueuedCompletionStatus 에 NULL 입력
		if (pCompletionKey == NULL) // 종료
		{
			break;
		}
		
		// Finalize 에서 PostQueuedCompletionStatus 에 NULL 입력
		if (pOverlap == NULL)
		{
			//g_log.log(INFO, "[IOCP](pOverlap == NULL)");
			break;
		}


		pIoContext = (IO_CONTEXT*)pOverlap;

		if (FALSE == bRet)
		{
			if (pIoContext->context == CTX_RQST_RECV)
			{
				g_log.log(INFO, "if (FALSE == bRet)");
				pThis->Recv_CloseEvent_FromEA(pCompletionKey);
			}
			Sleep(3000);
			continue;
		}
		
		if (dwIoSize == 0)
		{
			if (pIoContext->context == CTX_RQST_RECV)
			{
				g_log.log(INFO, "if (dwIoSize == 0)");
				pThis->Recv_CloseEvent_FromEA(pCompletionKey);
			}
			continue;
		}
		
		if (pIoContext->context == CTX_DIE)
		{
			break;
		}

		// Master / Slave 로 부터 데이터 수신
		if (pIoContext->context == CTX_RQST_RECV)
		{
			sprintf(zRecvBuff, "%.*s", pIoContext->wsaBuf.len, pIoContext->buf);
			g_log.log(INFO, "[RECV][SOCK:%d][%s](%s)", pCompletionKey->sock, pCompletionKey->sUserID.c_str(), zRecvBuff);
			nRet = pThis->m_buffering[iocpIdx].AddPacket(pIoContext->buf, dwIoSize);
			if (nRet < 0) {
				g_log.log(NOTIFY, "[%d][%d]AddPacket Error(%s)", iocpIdx, GetCurrentThreadId(), pThis->m_buffering[iocpIdx].GetErrMsg());
			}

			pThis->RequestRecvIO(pCompletionKey);

			BOOL bContinue ;
			nLoop = 0;
			do
			{
				ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
				nRet = 0;
				bContinue = pThis->m_buffering[iocpIdx].GetOnePacket(&nRet, zRecvBuff);
				if (bRet == FALSE)
				{
					g_log.log(NOTIFY, "[%d][%d][Buffering Err](%s)", iocpIdx, GetCurrentThreadId(), pThis->m_buffering[iocpIdx].GetErrMsg());
					break;
				}
				
				if (nRet > 0)
				{
					g_log.log(DEBUG_, "Buffering[%d](%s)\n", ++nLoop, zRecvBuff);
					pThis->DispatchData(pCompletionKey, zRecvBuff, nRet);
				}

			} while (bContinue);
		}

		if (pIoContext->context == CTX_RQST_SEND)
		{
			//printf("RequestSendIO returned\n");
		}
		delete pIoContext;

	} // while

	//mt4helper.disconnect();

	return 0;
}



void CDispatch::DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen)
{
	CProtoGet protoGet;
	string sCode;
	string sMasterSlaveTp;
	char zUserID[128] = { 0, };
	char zAccNo[128] = { 0, };
	protoGet.Parsing(pRecvData, nRecvLen); 

	try
	{
		ASSERT_BOOL2(protoGet.GetCode(sCode), E_NO_CODE, TEXT("Receive data but there is no Code"));

		if (sCode.compare(__ALPHA::CODE_PING) != 0) {
			//g_log.log(INFO, "=============================================");
			//g_log.log(INFO, "[RECV](len:%d)(%s)", nRecvLen, pRecvData);
		}

		ASSERT_BOOL2(protoGet.GetVal(FDS_MASTERCOPIER_TP, &sMasterSlaveTp), E_INVALIDE_MASTERCOPIER, 
										TEXT("FDS_MASTERSLAVE_TP is not in the packet"));

		// ID 저장
		if (pCK->sUserID.size() == 0)
		{
			ASSERT_BOOL2(protoGet.GetVal(FDS_USERID_MINE, zUserID), E_NO_USERID, TEXT("FDS_USERID_MINE is not in the packet"));
			ASSERT_BOOL2(protoGet.GetVal(FDS_ACCNO_MINE, zAccNo), E_NO_ACNTNO, TEXT("FDS_ACCNO_MINE is not in the packet"));
			char zKey[128] = { 0, };
			//__ALPHA::enMasterKey(zUserID, zAccNo, zKey);
			pCK->sUserID = zUserID;
		}
	}
	catch (CIRExcept e)
	{
		g_log.log(ERR, e.GetMsg());	
		g_log.log(NOTIFY, "[DispatchData Exception](OrgPacket:%s)", pRecvData);
		ReturnError(pCK, e.GetCode());
		return;
	}

	if (!Dispatch_To_MasterChannel(__ALPHA::IsMaster(sMasterSlaveTp), pCK->sUserID, pCK, pRecvData, nRecvLen, &protoGet))
	{
		g_log.logW(NOTIFY, TEXT("Dispatch_To_MasterChannel failed"));
	}
}


void CDispatch::ReturnError(COMPLETION_KEY* pCK, int nErrCode)
{
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[128] = { 0, };

	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_RETURN_ERROR);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pCK->sUserID.c_str());
	set.SetVal(FDN_ERR_CODE, nErrCode);
	
	int nLen = set.Complete(zSendBuff);

	g_log.log(INFO, "[Return Error to Client](%s)", zSendBuff);
	RequestSendIO(pCK, zSendBuff, nLen);
}

BOOL CDispatch::Dispatch_To_MasterChannel(BOOL bIsMastr, string sUserID, COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen, CProtoGet* pProtoGet)
{
	string sMasterID;
	if (!pProtoGet->GetVal(FDS_USERID_MASTER, &sMasterID) )
	{
		ReturnError(pCK, E_NO_MASTER_ID);
		g_log.logW(ERR, TEXT("[Dispatch_To_MasterChannel]FDS_USERID_MASTER is not in the packet"));
		return FALSE;
	}

	char zMasterAccNo[128] = { 0, };
	if (!pProtoGet->GetVal(FDS_ACCNO_MASTER, zMasterAccNo))
	{
		ReturnError(pCK, E_NO_MASTER_ACCNO);
		g_log.logW(ERR, TEXT("[Dispatch_To_MasterChannel]Master Account is not in the packet"));
		return FALSE;
	}

	//char zKey[128];
	//__ALPHA::enMasterKey(sMasterID.c_str(), zMasterAccNo, zKey);
	//if (g_bDebugLog)	g_log.log(INFO, "[enMasterKey](%s)", zKey);
	string sKey = sMasterID;
	CMasterChannel* p;
	map<string, CMasterChannel*>::iterator it = m_mapEA.find(sKey);
	if (IsMasterMapExist(it)==FALSE)
	{
		p = new CMasterChannel();
		p->Initialize(sMasterID, m_dSaveThread);
		m_mapEA[sKey] = p;
		g_log.log(INFO, "MasterClass Initialize.MasterID(%s)", sMasterID.c_str());
		Sleep(500);
	}
	else
	{
		p = (CMasterChannel*)(*it).second;
	}

	if (nRecvLen == 0) {
		g_log.log(INFO, "[Dispatch_To_MasterChannel]wrong receive packet(%s)", pRecvData);
	}
	else
		p->PassData(pCK->sock, pRecvData, nRecvLen);

	return TRUE;
}


unsigned WINAPI CDispatch::ListenThread(LPVOID lp)
{
	CDispatch *pThis = (CDispatch*)lp;
	g_log.logW(INFO, TEXT("ListenThread starts.....[%s][%d]"), pThis->m_wzListenIP, pThis->m_nListenPort);

	SOCKADDR_IN			sinClient;
	int	sinSize			= sizeof(sinClient);
	long nLoop			= 0;
	int nHearbeatCnt	= 0;

	while (pThis->m_bRun)
	{
		//DWORD dw = WSAWaitForMultipleEvents(1, &pThis->m_hListenEvent, TRUE, HEARTBEAT_TIMEOUT, FALSE);
		DWORD dw = WSAWaitForMultipleEvents(1, &pThis->m_hListenEvent, TRUE, 10, FALSE);
		if (dw != WSA_WAIT_EVENT_0) 
		{
			if (dw == WSA_WAIT_TIMEOUT)
			{
				continue;
			}
		}

		WSAResetEvent(pThis->m_hListenEvent);		
		
		SOCKET sockClient = accept(pThis->m_sockListen, (LPSOCKADDR)&sinClient, &sinSize);
		if (sockClient == INVALID_SOCKET) 
		{
			int nErr = WSAGetLastError();
			g_log.logW(NOTIFY, TEXT("accept error:%d"), nErr);
			{
				pThis->InitListen();
				Sleep(3000);
			}
			continue;
		}

		int nZero = 0;
		if (SOCKET_ERROR == setsockopt(sockClient, SOL_SOCKET, SO_SNDBUF, (const char*)&nZero, sizeof(int)))
		{
			shutdown(sockClient, SD_SEND);
			closesocket(sockClient);
			g_log.logW(NOTIFY, TEXT("setsockopt error : %d"), WSAGetLastError);
			continue;;
		}


		//	CK 와 IOCP 연결
		COMPLETION_KEY* pCK = new COMPLETION_KEY;
		pCK->sock		= sockClient;

		HANDLE h = CreateIoCompletionPort((HANDLE)pCK->sock,
			pThis->m_hCompletionPort,
			(DWORD)pCK,
			0);
		if (h == NULL)
		{
			delete pCK;
			closesocket(sockClient);
			continue;
		}

		char zSocket[128]; CVT_SOCKET(sockClient, zSocket);
		pThis->lockCK();
		pThis->m_mapCK[string(zSocket)] = pCK;
		pThis->unlockCK();

		g_log.logW(LOGTP_SUCC, TEXT("Accept & Add IOCP.[socket:%d][CK:%x]"), sockClient, pCK);

		//	최초 RECV IO 요청
		pThis->RequestRecvIO(pCK);

	}//while

	return 0;
}


VOID CDispatch::RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen)
{
	RequestSendIO(pCK->sock, pSendBuf, nSendLen);
}

VOID CDispatch::RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen)
{

	BOOL  bRet = TRUE;
	DWORD dwOutBytes = 0;
	DWORD dwFlags = 0;
	IO_CONTEXT* pSend = NULL;

	try {
		pSend = new IO_CONTEXT;

		ZeroMemory(pSend, sizeof(IO_CONTEXT));
		CopyMemory(pSend->buf, pSendBuf, nSendLen);
		pSend->wsaBuf.buf = pSend->buf;
		pSend->wsaBuf.len = nSendLen;
		pSend->context = CTX_RQST_SEND;

		int nRet = WSASend(sock
			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
			, 1					// wsaBuf 포인터 갯수
			, &dwOutBytes		// 전송된 바이트 수
			, dwFlags
			, &pSend->overLapped	// overlapped 포인터
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				g_log.logW(LOGTP_ERR, TEXT("WSASend error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		g_log.logW(ERR, TEXT("WSASend try catch error [CDispatch]"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;

	return;
}


void  CDispatch::RequestRecvIO(COMPLETION_KEY* pCK)
{
	IO_CONTEXT* pRecv = NULL;
	DWORD dwNumberOfBytesRecvd = 0;
	DWORD dwFlags = 0;

	BOOL bRet = TRUE;
	try {
		pRecv = new IO_CONTEXT;
		ZeroMemory(pRecv, CONTEXT_SIZE);
		//ZeroMemory( &(pRecv->overLapped), sizeof(WSAOVERLAPPED));
		pRecv->wsaBuf.buf = pRecv->buf;
		pRecv->wsaBuf.len = MAX_BUF;
		pRecv->context = CTX_RQST_RECV;


		int nRet = WSARecv(pCK->sock
			, &(pRecv->wsaBuf)
			, 1, &dwNumberOfBytesRecvd, &dwFlags
			, &(pRecv->overLapped)
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				g_log.logW(LOGTP_ERR, TEXT("WSARecv error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
		}
	}
	catch (...) {
		g_log.logW(LOGTP_ERR, TEXT("WSASend TRY CATCH"));
		bRet = FALSE;
	}

	if (!bRet)
		delete pRecv;

	return;
}


unsigned WINAPI CDispatch::QReadThread(LPVOID lp)
{
	CDispatch* pThis = (CDispatch*)lp;
	wchar_t wzQ[1024] = { 0, };
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[128] = { 0, };

	int nSeqNo, nRsltLen;
	wchar_t wzCode[128] = { 0, };
	wchar_t wzAllYN[128] = { 0, };
	wchar_t wzUserID[128] = { 0, };
	wchar_t wzMT4Acc[128] = { 0, };
	wchar_t wzMCTp[128] = { 0, };
	int nQDataTp = 0;
	wchar_t wzData[512] = { 0, };

	char zCode[128] = { 0, };

	while (pThis->m_bRun)
	{
		Sleep(500);

		CDBHandlerAdo db(pThis->m_pDBPool->Get());

		_stprintf(wzQ, TEXT("EXEC QFORWEB_READ "));
		if (FALSE == db->ExecQuery(wzQ))
		{
			g_log.logW(NOTIFY, TEXT("QFORWEB_READ Error(%s)(%s)"), db->GetError(), wzQ);
			Sleep(3000);
			continue;
		}
		if (db->IsNextRow()==FALSE)
			continue;

		nSeqNo = 0;
		nQDataTp = 0;

		// 12	1015	N	TRADER-1	123123	C	1	EURUSD=EURUSD___GBPUSD=GBPUSD___EURJPY=EURJPY___USDJPY=USDJPY	20191205_23:57:51:000	C
		if (db->IsNextRow())
		{
			nSeqNo = db->GetLong(TEXT("SEQNO"));
			db->GetStrWithLen(TEXT("TR_CODE"), sizeof(wzCode), wzCode);
			U2A(wzCode, zCode);

			db->GetStrWithLen(TEXT("ALL_YN"), sizeof(wzAllYN), wzAllYN);
			db->GetStrWithLen(TEXT("USER_ID"), sizeof(wzUserID), wzUserID);

			CUtil::TrimAll(wzUserID, _tcslen(wzUserID));
			_tcsupr(wzUserID);

			db->GetStrWithLen(TEXT("MT4_ACC"), sizeof(wzMT4Acc), wzMT4Acc);
			db->GetStrWithLen(TEXT("MC_TP"), sizeof(wzMCTp), wzMCTp);
			nQDataTp = db->GetLong(TEXT("QDATA_TP"));
			db->GetStrWithLen(TEXT("DATA"), sizeof(wzData), wzData);
		}
		db->Close();

		if (nQDataTp == QDATA_TP_NOTI)
		{
			CProtoSet	set;
			set.Begin();
			set.SetVal(FDS_CODE, wzCode);
			set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

			__ALPHA::Now(zTime);
			set.SetVal(FDS_TM_HEADER, zTime);
			set.SetVal(FDS_USERID_MINE, wzUserID);
			set.SetVal(FDS_ACCNO_MINE, wzMT4Acc);
			set.SetVal(FDS_MASTERCOPIER_TP, wzMCTp);
			set.SetVal(FDS_NOTI_MSG, wzData);

			nRsltLen = set.Complete(zSendBuff);

			g_log.logW(INFO, TEXT("[NOTI_MSG](ID:%s)(SEQ:%d)(DATA:%s)"), wzUserID, nSeqNo, wzData);

			pThis->QSendData(wzAllYN[0], wzUserID, zSendBuff, nRsltLen);

			pThis->CompleteRead(nSeqNo);
		}
		else if (nQDataTp == QDATA_TP_CONFIG)
		{
			ZeroMemory(zSendBuff, sizeof(zSendBuff));
			BOOL bRet;
			if (strncmp(zCode, __ALPHA::CODE_CONFIG_SYMBOL, strlen(__ALPHA::CODE_CONFIG_SYMBOL)) == 0)
			{
				if (wzMCTp[0] == 'M')	
					bRet = Comm_Compose_ConfigSymbol_MasterW((void*)pThis->m_pDBPool, wzUserID, wzMT4Acc, zSendBuff, &nRsltLen, pThis->m_wzMsg);
				if (wzMCTp[0] == 'C')
					bRet = Comm_Compose_ConfigSymbol_CopierW((void*)pThis->m_pDBPool, wzUserID, wzMT4Acc, zSendBuff, &nRsltLen, pThis->m_wzMsg);
			}
			if (strncmp(zCode, __ALPHA::CODE_CONFIG_GENERAL, strlen(__ALPHA::CODE_CONFIG_GENERAL)) == 0)
			{
				if (wzMCTp[0] == 'M')
					bRet = Comm_Compose_ConfigGeneral_MasterW((void*)pThis->m_pDBPool, wzUserID, wzMT4Acc, zSendBuff, &nRsltLen, pThis->m_wzMsg);
				if (wzMCTp[0] == 'C')
					bRet = Comm_Compose_ConfigGeneral_CopierW((void*)pThis->m_pDBPool, wzUserID, wzMT4Acc, zSendBuff, &nRsltLen, pThis->m_wzMsg);
			}

			g_log.logW(INFO, TEXT("[CONFIG](ID:%s)(SEQ:%d)(DATA:%s)(Send:%s)"), wzUserID, nSeqNo, wzData, zSendBuff);

			pThis->QSendData(wzAllYN[0], wzUserID, zSendBuff, nRsltLen);

			pThis->CompleteRead(nSeqNo);
		}
		
	} // while

	return 0;
}

void	CDispatch::CompleteRead(int nSeq)
{
	wchar_t wzQ[1024] = { 0, };
	CDBHandlerAdo db(m_pDBPool->Get());

	_stprintf(wzQ, TEXT("EXEC QFORWEB_COMPLETE %d "), nSeq);
	db->ExecQuery(wzQ);
}

void	CDispatch::QSendData(wchar_t cAllYN, wchar_t* pwzUserID, char* pzData, int nDataLen)
{
	if (cAllYN == 'Y ')
	{
		EnterCriticalSection(&m_csQ);
		map<STR_USER_ID, SOCKET>::iterator it;
		for (it = m_mapQ.begin(); it != m_mapQ.end(); it++)
		{
			RequestSendIO((*it).second, pzData, nDataLen);
		}
		LeaveCriticalSection(&m_csQ);
	}
	else
	{
		char zUserId[128] = { 0, };
		U2A(pwzUserID, zUserId);
		string sUserID = zUserId;

		EnterCriticalSection(&m_csQ);
		map<STR_USER_ID, SOCKET>::iterator it = m_mapQ.find(sUserID);
		if (it != m_mapQ.end())
		{
			RequestSendIO((*it).second, pzData, nDataLen);
		}
		LeaveCriticalSection(&m_csQ);
	}
}


unsigned WINAPI CDispatch::DBSaveThread(LPVOID lp)
{
	int nRet;
	CDispatch* pThis = (CDispatch*)lp;
	__try
	{
		nRet = pThis->DBSaveThreadFn();
	}
	__except (ReportException(GetExceptionCode(), TEXT("DBSaveThread"), pThis->m_wzMsg))
	{
		g_log.logW(NOTIFY, pThis->m_wzMsg);
		nRet = 0;
	}
	return nRet;
}

unsigned CDispatch::DBSaveThreadFn()
{
	wchar_t ip[128] = { 0, }, id[128] = { 0, }, pwd[128] = { 0, }, cnt[128] = { 0, }, name[128] = { 0, };
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"),TEXT( "DB_IP"), ip);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"),TEXT( "DB_ID"), id);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"),TEXT( "DB_PWD"), pwd);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"),TEXT( "DB_NAME"), name);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"),TEXT( "DB_POOL_CNT"), cnt);


	if (!m_pDBPool)
	{
		m_pDBPool = new CDBPoolAdo(ip, id, pwd, name);
	}
	if (!m_pDBPool->Init(_ttoi(cnt)))
		{
			g_log.logW(NOTIFY, TEXT("DB OPEN FAILED.(%s)(%s)(%s)"), ip, id, pwd);
			return 0;
		}
		
	while (m_bRun)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			BOOL bDelete = TRUE;
			if (msg.message == WM_SAVE_ORDER)
			{
				//g_log.log(INFO, "DBSave_Order");
				DBSave_Order(((TAG_BUF*)msg.lParam)->buf, msg.wParam);
			}
			else if (msg.message == WM_LOGON)
			{
				DBSave_LogOnOff((char*)msg.lParam, msg.wParam, TRUE);
			}
			else if (msg.message == WM_LOGOUT)
			{
				DBSave_LogOnOff((char*)msg.lParam, msg.wParam, FALSE);
			}
			else
				bDelete = FALSE;
			
			if(bDelete)
				delete (TAG_BUF*)msg.lParam;
		}
	}
	return 0;
}


BOOL CDispatch::DBSave_LogOnOff(char* psUserId, SOCKET sock, BOOL bLogon)
{
	BOOL bRet;
	TAG_BUF* p = (TAG_BUF*)psUserId;

	CDBHandlerAdo db(m_pDBPool->Get());
	wchar_t wzQ[1024] = { 0, };
	wchar_t wzLogOnYN[2] = { 0, };
	wchar_t wzUserID[128] = { 0, };
	char zUserID[128] = { 0, };

	strcpy(zUserID, p->buf);
	A2U(zUserID, wzUserID);
	wzLogOnYN[0] = (bLogon) ? 'Y' : 'N';

	_stprintf(wzQ, TEXT("EXEC EA_RELAY_LOG_ONOFF ")
		TEXT("'%s'")		//@I_USER_ID	varchar(20)
		TEXT(",'%s'")
		, wzUserID
		, wzLogOnYN
	);

	if (FALSE == db->ExecQuery(wzQ))
	{
		g_log.logW(NOTIFY, TEXT("EA_RELAY_LOG_ONOFF Error(%s)(%s)"), db->GetError(), wzQ);
		bRet = FALSE;
	}
	else
	{
		//g_log.logW(DEBUG_, TEXT("DB LogOnOff(%s)(%s)"), wzUserID, wzLogOnYN);
		bRet = TRUE;
	}
	db->Close();

	if (bLogon)
	{
		AddQMap(zUserID,sock);
	}
	else
	{
		RemoveQMap(zUserID);
	}

	return bRet;
}

void CDispatch::AddQMap(string sUserID, SOCKET sock)
{
	EnterCriticalSection(&m_csQ);
	m_mapQ[sUserID] = sock;
	LeaveCriticalSection(&m_csQ);
}
void CDispatch::RemoveQMap(string sUserID)
{
	EnterCriticalSection(&m_csQ);
	map<STR_USER_ID, SOCKET>::iterator it = m_mapQ.find(sUserID);
	if (it != m_mapQ.end())
		m_mapQ.erase(it);
	LeaveCriticalSection(&m_csQ);
}


BOOL CDispatch::DBSave_Order(char* pOrdData, int nDataLen)
{
	BOOL bRet = Comm_DBSave_TraceOrder((void*)m_pDBPool, pOrdData, nDataLen, m_wzMsg);
	if (bRet == FALSE)
	{
		g_log.logW(NOTIFY, TEXT("Comm_DBSave_TraceOrder Error(%s)"), m_wzMsg);
	}
	else {
		if(g_bDebugLog) g_log.logW(DEBUG_, TEXT("[Save Order](%s)"), m_wzMsg);
	}
	return bRet;
}
