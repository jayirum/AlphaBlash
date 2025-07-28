
#include "IocpThread.h"
#include <process.h>
#include "../Common/Util.h"
#include "../Common/LogMsg.h"
#include "../Common/IRExcept.h"
#include "../Common/IRUM_Common.h"
#include "../Common/CommonFunc.h"
#include "../Common/AlphaProtoSetEx.h"
#include <assert.h>

extern CLogMsg	g_log;
extern wchar_t	g_zConfig[_MAX_PATH];
extern BOOL		g_bDebugLog;
CIocp::CIocp()
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
CIocp::~CIocp()
{
	Finalize();
}

BOOL CIocp::ReadIPPOrt()
{
	wchar_t zTemp[1024] = { 0, };
	CUtil::GetConfig(g_zConfig, TEXT("NETWORK"), TEXT("LISTEN_IP"), m_wzListenIP);
	CUtil::GetConfig(g_zConfig, TEXT("NETWORK"), TEXT("LISTEN_PORT"), zTemp);
	m_nListenPort = _ttoi(zTemp);
	
	return TRUE;
}

BOOL CIocp::Initialize( )
{
	ReadIPPOrt();

	InitializeCriticalSection(&m_csCK);

	//DB OPEN
	if (!DBOpen())
		return FALSE;

	if (!InitListen()) {
		return FALSE;
	}
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, &ListenThread, this, 0, &m_dListenThread);

	// CPU의 수를 알기 위해서....IOCP에서 사용할 쓰레드의 수는 cpu개수 또는 cpu*2
	SYSTEM_INFO         systemInfo;
	GetSystemInfo(&systemInfo);
	m_dwThreadCount = systemInfo.dwNumberOfProcessors;

	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		g_log.logW(LOGTP_ERR, TEXT("IOCP Create Error:%d"), GetLastError());
		return FALSE;
	}

	// 실제로 recv와 send를 담당할 스레드를 생성한다.
	unsigned int dwID;
	for (unsigned int n = 0; n < m_dwThreadCount; n++)
	{
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &IocpWorkerThread, this, 0, &dwID);
		CloseHandle(h);
	}

	return TRUE;
}


BOOL CIocp::DBOpen()
{
	wchar_t ip[32], id[32], pwd[32], cnt[32], name[32];
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_IP"), ip);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_ID"), id);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_PWD"), pwd);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_NAME"), name);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_POOL_CNT"), cnt);


	if (!m_pDBPool)
	{
		m_pDBPool = new CDBPoolAdo(ip, id, pwd, name);
		if (!m_pDBPool->Init(_ttoi(cnt)))
		{
			g_log.logW(ERR, TEXT("DB OPEN FAILED.(%s)(%s)(%s)"), ip, id, pwd);
			return 0;
		}
	}
	return TRUE;
}

BOOL CIocp::InitListen()
{
	g_log.logW(INFO, TEXT("CIocp::InitListen() starts.........."));
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


void CIocp::CloseListenSock()
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

VOID CIocp::SendMessageToIocpThread(int Message)
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

void CIocp::Finalize()
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

	SAFE_CLOSEHANDLE(m_hCompletionPort);

	DeleteCriticalSection(&m_csCK);
	//DeleteCriticalSection(&m_csEA);
	//SAFE_DELETE(m_pack);
	WSACleanup();

	delete m_pDBPool;
}

void CIocp::DeleteSocket(COMPLETION_KEY *pCompletionKey)
{
	if (pCompletionKey->refcnt > 0)
		return;

	lockCK();

	char zSock[32];
	CVT_SOCKET(pCompletionKey->sock, zSock);

	std::map<std::string, COMPLETION_KEY*>::iterator it = m_mapCK.find(std::string(zSock));
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
unsigned WINAPI CIocp::IocpWorkerThread(LPVOID lp)
{
	CIocp* pThis = (CIocp*)lp;

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
	char			zLogBuff[BUF_LEN];
	int nLoop = 0;
	int nRet = 0;
	
	long iocpIdx = InterlockedIncrement(&pThis->m_lIocpThreadIdx)-1;

	g_log.logW(LOGTP_SUCC, TEXT("[%d][%d]IOCPThread Start....."), iocpIdx, GetCurrentThreadId());

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
				//pThis->RecvLogOffAndClose(pCompletionKey);
				pThis->DeleteSocket(pCompletionKey);
				//g_log.log(LOGTP_ERR, "[IOCP](if (FALSE == bRet)) call DeleteSocket()");
			}
			Sleep(3000);
			continue;
		}
		
		if (dwIoSize == 0)
		{
			//g_log.log(INFO, "[IOCP]if (dwIoSize == 0)");
			if (pIoContext->context == CTX_RQST_RECV)
				pThis->DeleteSocket(pCompletionKey);
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
			sprintf(zLogBuff, "[%d][%d][RECV](%.*s)", iocpIdx, GetCurrentThreadId(), dwIoSize, zRecvBuff);
			g_log.log(INFO, zLogBuff);
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
					g_log.log(INFO, "[%d][%d]Buffering[%d](%s)\n", iocpIdx, GetCurrentThreadId(), ++nLoop, zRecvBuff);
					InterlockedIncrement(&pCompletionKey->refcnt);
					pThis->DispatchData(pCompletionKey, zRecvBuff, nRet);
					InterlockedDecrement(&pCompletionKey->refcnt);
				}

			} while (bContinue);

			//pThis->RequestRecvIO(pCompletionKey);

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


/*
CODE_USER_LOG
CODE_COPIER_ORDER
CODE_LOGON
CODE_LOGOFF
*/
void CIocp::DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen)
{
	CProtoGet protoGet;
	string sCode;
	string sMasterSlaveTp;
	char zMCTp[32] = { 0, };
	wchar_t wzID[128] = { 0, };
	char zID[128] = { 0, };
	protoGet.Parsing(pRecvData, nRecvLen); 

	try
	{
		ASSERT_BOOL2(protoGet.GetCode(sCode), E_NOCODE, TEXT("Receive data but there is no Code"));

		CopierList(pCK, pRecvData, nRecvLen);
	}
	catch (CIRExcept& e)
	{
		g_log.log(ERR, e.GetMsg());	
		g_log.log(NOTIFY, "[DispatchData Exception](%s)(OrgPacket:%s)",e.GetMsg(), pRecvData);
		ReturnError(pCK, e.GetCode());
		return;
	}

}


BOOL CIocp::CopierList(COMPLETION_KEY* pCK, const char* pLogData, int nDataLen)
{
	////	client public ip 추출
	//SOCKADDR_IN peer_addr;
	//int			peer_addr_len = sizeof(peer_addr);
	//char zClientIp[128] = { 0, };
	//wchar_t wzClientIp[128] = { 0, };
	//if (getpeername(pCK->sock, (sockaddr*)&peer_addr, &peer_addr_len) == 0)
	//{
	//	strcpy(zClientIp, inet_ntoa(peer_addr.sin_addr));
	//	A2U(zClientIp, wzClientIp);
	//}

	BOOL bRet;

	CDBHandlerAdo db(m_pDBPool->Get());
	wchar_t wzMasterID[32] = { 0 };
	wchar_t wzQ[1024];
	
	CProtoGet protoGet;
	protoGet.Parsing(pLogData, nDataLen, FALSE);
	protoGet.GetValS(FDS_USERID_MASTER, wzMasterID);
	protoGet.GetValS(FDS_DATA, wzQ);

	_stprintf(wzQ, "EXEC WEB_MasterCopier_Link_GET "
		TEXT("'%s'")	//@I_USER_ID
		, wzMasterID
	);
	if (g_bDebugLog) g_log.logW(INFO, TEXT("[Master Login SP](%s)"), wzQ);

	//
	// RETURN 
	//

	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };

	int nRetCode;
	CProtoSetEx	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_COPIER_LIST);
	
	wchar_t wzVal[128]	= { 0 };

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);

	if (FALSE == db->ExecQuery(wzQ))
	{
		g_log.logW(NOTIFY, TEXT("Logon_Master Error(%s)(%s)"), db->GetError(), wzQ);
		set.SetVal(FDN_RSLT_CODE, E_SYS_DB_EXCEPTION);
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
		int nLoop = 0;
		while (db->IsNextRow())
		{
			nRetCode = db->GetLong(TEXT("RET_CODE"));
			set.SetVal(FDN_RSLT_CODE, nRetCode);
			if (nRetCode != ERR_OK)
			{
				bRet = FALSE;
				g_log.logW(ERR, TEXT("[%s]DB Login Error(%d)"), wzMasterID, nRetCode);
				break;
			}
			
			if(nLoop==0)
				set.RecordStart();
			
			set.RowStart();

			db->GetStrWithLen(TEXT("COPIER_ID"), 32, wzVal);
			set.SetRecordVal(TEXT("COPIER_ID"), wzVal);
			
			long nSubsStatus = db->GetLong(TEXT("SUBS_STATUS"));
			set.SetRecordVal(TEXT("SUBS_STATUS"), nSubsStatus);

			db->GetStrWithLen(TEXT("RQST_DT"), 32, wzVal);
			set.SetRecordVal(TEXT("RQST_DT"), wzVal);

			db->GetStrWithLen(TEXT("UPUPDATE_DTDATE_DT"), 32, wzVal);
			set.SetRecordVal(TEXT("RQST_DT"), wzVal);

			db->GetStrWithLen(TEXT("COPIER_MT4_ACC"), 32, wzVal);
			set.SetRecordVal(TEXT("COPIER_MT4_ACC"), wzVal);

			set.RowEnd();

			nLoop++;
			db->Next();
		}
		set.SetRecordEnd();
	}
	db->Close();

	int nLen = set.Complete(zSendBuff);
	RequestSendIO(pCK, zSendBuff, nLen);

	return bRet;
}




void CIocp::ReturnError(COMPLETION_KEY* pCK, int nErrCode)
{
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };

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





unsigned WINAPI CIocp::ListenThread(LPVOID lp)
{
	CIocp *pThis = (CIocp*)lp;
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
		if (sockClient == INVALID_SOCKET) {
			int nErr = WSAGetLastError();
			g_log.logW(NOTIFY, TEXT("accept error:%d"), nErr);
			{ // Socket operation on nonsocket.
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

		char zSocket[32]; CVT_SOCKET(sockClient, zSocket);
		pThis->lockCK();
		pThis->m_mapCK[string(zSocket)] = pCK;
		pThis->unlockCK();

		g_log.logW(INFO, TEXT("Accept & Add IOCP.[socket:%d][CK:%x]"), sockClient, pCK);

		//	최초 RECV IO 요청
		pThis->RequestRecvIO(pCK);

		//pThis->SendMessageToIocpThread(CTX_MT4PING);

	}//while

	return 0;
}


VOID CIocp::RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen)
{

	BOOL  bRet = TRUE;
	DWORD dwOutBytes = 0;
	DWORD dwFlags = 0;
	IO_CONTEXT* pSend = NULL;

	try {
		pSend = new IO_CONTEXT;

		ZeroMemory(pSend, sizeof(IO_CONTEXT));
		CopyMemory(pSend->buf, pSendBuf, nSendLen);
		//	pSend->sock	= sock;
		pSend->wsaBuf.buf = pSend->buf;
		pSend->wsaBuf.len = nSendLen;
		pSend->context = CTX_RQST_SEND;

		int nRet = WSASend(pCK->sock
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
		g_log.logW(ERR, TEXT("WSASend try catch error [CIocp]"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;

	return;
}



void  CIocp::RequestRecvIO(COMPLETION_KEY* pCK)
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
				g_log.logW(LOGTP_ERR, TEXT("WSARecv error : %d"), WSAGetLastError());
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

	printf("RequestRecvIO ok\n");
	return;
}