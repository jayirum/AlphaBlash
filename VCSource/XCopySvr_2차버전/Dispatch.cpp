
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
	m_bSendCntrInit = FALSE;

	ZeroMemory(&m_arrMasters, sizeof(m_arrMasters));
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

BOOL CDispatch::ReadCnfg_MasterID()
{
	wchar_t wzRslt[128] = { 0, };
	wchar_t wzKey[128];
	
	for (int i = 0; i < MAX_MASTERS_CNT; i++)
	{
		wsprintf(wzKey, TEXT("ID%d"), i + 1);

		CUtil::GetConfig(g_wzConfig, TEXT("MASTER"), wzKey, wzRslt);
		if (wcslen(wzRslt) > 0)
		{
			CUtil::TrimAll(wzRslt, wcslen(wzRslt));		
			wcscpy(m_arrMasters[i].wzID, wzRslt);
			m_arrMasters[i].logInOff[0] = 0x00;
			m_arrMasters[i].nLastCntrNo = -1;

			g_log.logW(INFO, TEXT("[Read Master ID] (%s)"), wzRslt);
		}
	}
	return TRUE;
}



BOOL CDispatch::ReadCnfg_DBReadTimeOut()
{
	wchar_t wzRslt[128] = { 0, };
	char zRslt[128] = { 0 };

	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"), TEXT("DB_SEL_TIMEOUT"), wzRslt);
	if (wcslen(wzRslt) == 0)
		return FALSE;

	U2A(wzRslt, zRslt);
	m_nTimeoutDB = atoi(zRslt) * 1000;
	return TRUE;
}


BOOL CDispatch::ReadCnfg_IocpCnt()
{
	wchar_t wzRslt[128] = { 0, };
	char zRslt[128] = { 0 };

	CUtil::GetConfig(g_wzConfig, TEXT("NETWORK"), TEXT("IOCP_CNT"), wzRslt);
	if (wcslen(wzRslt) == 0)
		return FALSE;

	U2A(wzRslt, zRslt);
	m_nIocpThreadCnt = atoi(zRslt) ;
	return TRUE;
}

BOOL CDispatch::DBOpen()
{
	wchar_t ip[128] = { 0, }, id[128] = { 0, }, pwd[128] = { 0, }, cnt[128] = { 0, }, name[128] = { 0, };
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"), TEXT("DB_IP"), ip);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"), TEXT("DB_ID"), id);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"), TEXT("DB_PWD"), pwd);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"), TEXT("DB_NAME"), name);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO"), TEXT("DB_POOL_CNT"), cnt);

	if (!m_pDBPool)
	{
		m_pDBPool = new CDBPoolAdo(ip, id, pwd, name);
	}
	if (!m_pDBPool->Init(_ttoi(cnt)))
	{
		g_log.logW(NOTIFY, TEXT("DB OPEN FAILED.(%s)(%s)(%s)"), ip, id, pwd);
		return FALSE;
	}

	return TRUE;
}

BOOL CDispatch::Initialize( )
{
	InitializeCriticalSection(&m_csCK);
	InitializeCriticalSection(&m_csMasters);

	ReadIPPOrt();

	if (!ReadCnfg_MasterID() ||
		!ReadCnfg_DBReadTimeOut() ||
		!ReadCnfg_IocpCnt()
		)
		return FALSE;

	if (!DBOpen())
		return FALSE;

	if(!Get_LastCntrNo())
		return FALSE;

	if (!InitListen()) {
		g_log.logW(ERR, TEXT("Init Listen Failed"));
		return FALSE;
	}
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, &ListenThread, this, 0, &m_dListenThread);

	// CPU의 수를 알기 위해서....IOCP에서 사용할 쓰레드의 수는 cpu개수 또는 cpu*2
	SYSTEM_INFO         systemInfo;
	int nThreadCnt;
	GetSystemInfo(&systemInfo);
	nThreadCnt = m_nIocpThreadCnt;	// systemInfo.dwNumberOfProcessors;
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


	// QTable Read
	m_hQThread = (HANDLE)_beginthreadex(NULL, 0, &DBReadThread, this, 0, &m_dQThread);

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

void	CDispatch::RemoveAllMaster()
{
	//lockMasters();
	//list<MASTER_INFO*>::iterator it;
	//for (it = m_lstMasters.begin(); it != m_lstMasters.end(); it++)
	//	delete (*it);

	//m_lstMasters.clear();
	//unlockMasters();
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

	RemoveAllMaster();

	SAFE_CLOSEHANDLE(m_hCompletionPort);

	DeleteCriticalSection(&m_csCK);
	DeleteCriticalSection(&m_csMasters);

	//SAFE_DELETE(m_pack);
	WSACleanup();

	delete m_pDBPool;
}


void CDispatch::Recv_CloseEvent_FromEA(COMPLETION_KEY *pCompletionKey)
{
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
	char zCode[3];
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
			pThis->RequestRecvIO(pCompletionKey);

			g_log.log(INFO, "[RECV](%s)", pIoContext->buf);
			sprintf(zCode, "%.2s", pIoContext->buf);

			if( strcmp(zCode, CODE_PWD)==0)
				pThis->Copier_CheckPwd_SendInitData(pCompletionKey, pIoContext->buf);
			if (strcmp(zCode, CODE_CNTR_HIST)==0)
				pThis->Copier_Rqst_CntrHist(pCompletionKey, pIoContext->buf);
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



void CDispatch::Copier_CheckPwd_SendInitData(COMPLETION_KEY* pCK, char* pRecvBuf)
{
	wchar_t wzPwd[129];
	char zPwd[128] = { 0 };
	CUtil::GetConfig(g_wzConfig, TEXT("PASSWORD"), TEXT("PWD"), wzPwd);
	U2A(wzPwd, zPwd);

	char zMsgBuf[1024] = { 0 };
	_XAlphaGT::TRET_MSG* p = (_XAlphaGT::TRET_MSG*)zMsgBuf;
	memcpy(p->Code, CODE_MSG, strlen(CODE_MSG));
	p->Enter[0] = DEF_ENTER;


	_XAlphaGT::TCL_PWD* pPwd = (_XAlphaGT::TCL_PWD*)pRecvBuf;
	if (strncmp(zPwd, pPwd->Pwd, sizeof(pPwd->Pwd)) != 0)
	{
		memcpy(p->RetCode, RETCODE_PWD_WRONG, strlen(RETCODE_PWD_WRONG));
		RequestSendIO(pCK, zMsgBuf, strlen(zMsgBuf));
		return;
	}
	
	// Pwd ok
	memcpy(p->RetCode, RETCODE_PWD_OK, strlen(RETCODE_PWD_OK));
	RequestSendIO(pCK, zMsgBuf, strlen(zMsgBuf));


	BOOL bCopierCall = TRUE;

	//	로그인 정보와 체결정보를 전달한다.
	for (int i = 0; i < MAX_MASTERS_CNT; i++)
	{
		_M_Conn_MainProc(&m_arrMasters[i], bCopierCall);
	}
}



void CDispatch::Copier_Rqst_CntrHist(COMPLETION_KEY* pCK, char* pRecvBuf)
{
	_XAlphaGT::TCL_RQST_CNTR_HIST* pRqst = (_XAlphaGT::TCL_RQST_CNTR_HIST*)pRecvBuf;
	char zMasterID[32];
	sprintf(zMasterID, "%.*s", sizeof(pRqst->MasterID), pRqst->MasterID);

	BOOL bHistory = TRUE;
	int idx = MasterIdx(zMasterID);
	if (idx < 0)
	{
		g_log.log(ERR, "Master Idx Error(ID:%s)", zMasterID);
		return;
	}


	_M_Cntr_MainProc(&m_arrMasters[idx], bHistory, pCK);
}

int	CDispatch::MasterIdx(char* pzMasterId)
{
	wchar_t wzMasterId[32] = { 0 };
	A2U(pzMasterId, wzMasterId);

	BOOL bFound = FALSE;
	int i = 0;
	for (i = 0; i < MAX_MASTERS_CNT; i++)
	{
		if (wcsncmp(wzMasterId, m_arrMasters[i].wzID, wcslen(wzMasterId)) == 0)
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
		return -1;

	return i;
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

		char zClientIp[32] = { 0 };
		if (pThis->Exist_SameIP(sockClient, zClientIp))
		{
			g_log.logW(NOTIFY, TEXT("같은 IP 중복접속시도. 거부.(%s)"), zClientIp);
			closesocket(sockClient);
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
		pCK->sIp = string(zClientIp);

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

		char zSocket[128]; 
		CVT_SOCKET(sockClient, zSocket);

		pThis->lockCK();
		pThis->m_mapCK[string(zSocket)] = pCK;
		pThis->unlockCK();

		g_log.logW(LOGTP_SUCC, TEXT("Accept & Add IOCP.[socket:%d][CK:%x]"), sockClient, pCK);

		//	최초 RECV IO 요청
		pThis->RequestRecvIO(pCK);

	}//while

	return 0;
}



BOOL CDispatch::Exist_SameIP(SOCKET sock, _Out_ char* pzClientIp)
{
	//	client public ip 추출
	SOCKADDR_IN peer_addr;
	int			peer_addr_len = sizeof(peer_addr);

	BOOL bExist = FALSE;

	if (getpeername(sock, (sockaddr*)&peer_addr, &peer_addr_len) == 0)
	{
		strcpy(pzClientIp, inet_ntoa(peer_addr.sin_addr));

		lockCK();
		map<STR_SOCKET, COMPLETION_KEY*>::iterator it;
		for (it = m_mapCK.begin(); it != m_mapCK.end(); it++)
		{
			COMPLETION_KEY* p = (*it).second;
			if (p->sIp.compare(pzClientIp) == 0)
			{
				bExist = TRUE;
				break;
			}
		}
		unlockCK();
	}
	return bExist;
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
			if (WSAGetLastError() != WSA_IO_PENDING) 
			{
				g_log.logW(LOGTP_ERR, TEXT("WSARecv error : %d. Close client"), WSAGetLastError);
				Recv_CloseEvent_FromEA(pCK);
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


BOOL CDispatch::_M_Conn_MainProc(_InOut_ MASTER_INFO* p, BOOL bCopierCall)
{

	wchar_t wzMasterId[32] = { 0 };
	wchar_t wzUserNm[32] = { 0 };
	wchar_t wzDt[32];
	wchar_t wzTm[32] = { 0 };
	wchar_t wzLoginTp[32] = { 0 };
	int		nGapSec;

	wchar_t wzDtTm[128];

	wchar_t wzQ[1024] = { 0, };

	CDBHandlerAdo db(m_pDBPool->Get());

	wsprintf(wzQ, TEXT(
		"SELECT TOP 1 A.USER_ID, A.LOGIN_DT, A.LOGIN_TM, A.LOGIN_TP, B.USER_NM "
		" ,DATEDIFF(SS, (CONVERT(CHAR(10), CONVERT(DATETIME, LOGIN_DT),121)+' '+LOGIN_TM), CONVERT(CHAR(22),getdate(), 121))  AS TIMEGAP_SEC "
		" FROM LOGIN_HIS A, USER_MST B "
		" WHERE A.USER_ID = '%s' AND LOGIN_DT >= DBO.FP_TRADE_DT() AND A.USER_ID=B.USER_ID  ORDER BY LOGIN_DT DESC,  LOGIN_TM DESC"
	)
		, p->wzID
	);
	if (FALSE == db->ExecQuery(wzQ))
	{
		g_log.logW(NOTIFY, TEXT("LogonStatus Get Error(%s)(%s)"), db->GetError(), wzQ);
		return FALSE;
	}
	if (db->IsNextRow() == FALSE)
	{
		// NO RECORDSET
		return TRUE;
	}


	if (db->IsNextRow())
	{
		ZeroMemory(wzMasterId, sizeof(wzMasterId));
		ZeroMemory(wzUserNm, sizeof(wzUserNm));
		ZeroMemory(wzDt, sizeof(wzDt));
		ZeroMemory(wzTm, sizeof(wzTm));
		ZeroMemory(wzLoginTp, sizeof(wzLoginTp));
		ZeroMemory(wzDtTm, sizeof(wzDtTm));
		nGapSec = 0;

		db->GetStrWithLen(TEXT("USER_ID"), sizeof(wzMasterId), wzMasterId);
		db->GetStrWithLen(TEXT("USER_NM"), sizeof(wzUserNm), wzUserNm);
		db->GetStrWithLen(TEXT("LOGIN_DT"), sizeof(wzDt), wzDt);
		db->GetStrWithLen(TEXT("LOGIN_TM"), sizeof(wzTm), wzTm);
		db->GetStrWithLen(TEXT("LOGIN_TP"), sizeof(wzLoginTp), wzLoginTp);
		nGapSec = db->GetLong(TEXT("TIMEGAP_SEC"));

		if ((bCopierCall == FALSE) && nGapSec > OLDDATA_GAP_SEC)
		{
			db->Close();
			return TRUE;
		}

		if (bCopierCall == FALSE)
		{
			wsprintf(wzDtTm, TEXT("%s%s"), wzDt, wzTm);
			if (wcsncmp(wzDtTm, p->wzLastLogOnOffTm, lstrlenW(wzDtTm)) <= 0)
			{
				db->Close();
				return TRUE;
			}
		}

		_M_Conn_Publish_LoginStatus(wzMasterId, wzUserNm, wzTm, wzLoginTp, nGapSec);

		char z[32] = { 0 }; U2A(wzLoginTp, z);

		// update 
		p->logInOff[0] = z[0];
		lstrcpyW(p->wzLastLogOnOffTm, wzDtTm);
	}
	db->Close();

	return TRUE;
}

/*
struct TLOGON
	{
		char	STX[1];
		char	Len[4];
		char	Code[10];
		char	userId[20];
		char	userNm[20];
		char	Tm[12];
		char	loginTp[1];	// I/O
		char	ETX[1];
	};
*/
BOOL CDispatch::_M_Conn_Publish_LoginStatus(wchar_t* masterId, wchar_t* userNm, wchar_t* Tm, wchar_t* loginTp, int nGapSec)
{
	char z[128];
	char zSendBuf[1024] = { 0 };
	_XAlphaGT::TLOGON* pSend = (_XAlphaGT::TLOGON*)zSendBuf;
	int len = sizeof(_XAlphaGT::TLOGON); 
	memset(pSend, 0x20, len);
	
	memcpy(pSend->Code, CODE_LOGONOFF, strlen(CODE_LOGONOFF));

	BOOL bOldData = (nGapSec > OLDDATA_GAP_SEC);
	if (bOldData)	pSend->oldDataYN[0] = 'Y';
	else			pSend->oldDataYN[0] = 'N';

	U2A(masterId, z);
	memcpy(pSend->masterId, z, strlen(z));
	
	U2A(userNm, z);
	memcpy(pSend->masterNm, z, strlen(z));

	U2A(Tm, z);
	memcpy(pSend->Tm, z, strlen(z));

	U2A(loginTp, z);
	memcpy(pSend->loginTp, z, 1);

	//p->ETX[0] = DEF_ETX;
	pSend->Enter[0] = DEF_ENTER;


	g_log.log(INFO, "[LOGONOFF](%.20s)<%c>(%.12s)(%c)(%s)",
		pSend->masterId, pSend->oldDataYN[0], pSend->Tm, pSend->loginTp[0], zSendBuf);

	SendToAll(zSendBuf, len);
	
	return TRUE;
}


BOOL CDispatch::Get_LastCntrNo()
{
	wchar_t wzQ[1024] = { 0, };
	CDBHandlerAdo db(m_pDBPool->Get());

	for (int i = 0; i < MAX_MASTERS_CNT; i++)
	{
		if (m_arrMasters[i].wzID[0] != 0x00)
		{
			m_arrMasters[i].nLastCntrNo = 0;

			wsprintf(wzQ, TEXT("SELECT  TOP 1 CNTR_NO FROM CNTR WHERE USER_ID = ")
				TEXT("'%s'")
				TEXT(" ORDER BY CNTR_NO DESC ")
				, m_arrMasters[i].wzID
			);

			if (db->ExecQuery(wzQ) == FALSE)
			{
				g_log.logW(NOTIFY, TEXT("Get CNTR Last No Error(%s)"), wzQ);
				return FALSE;
			}

			if (db->IsNextRow())
			{
				m_arrMasters[i].nLastCntrNo = db->GetLong(TEXT("CNTR_NO"));
				g_log.logW(INFO, TEXT("[%s]Last Cntr No(%d)"), m_arrMasters[i].wzID, m_arrMasters[i].nLastCntrNo);
			}
		}
	}

	return TRUE;
}

BOOL CDispatch::_M_Cntr_MainProc(_InOut_ MASTER_INFO* p, BOOL bHistory, COMPLETION_KEY* pCK)
{
	wchar_t wzQ[1024] = { 0, };
	CDBHandlerAdo db(m_pDBPool->Get());
	//BOOL bInit = (m_Master[idx].nLastCntrNo <= 0);
	if(bHistory)
	{
		wsprintf(wzQ, TEXT("SELECT  * FROM CNTR WHERE USER_ID = ")
			TEXT("'%s'")
			TEXT(" ORDER BY CNTR_NO ")
			, p->wzID
		);
	}
	else
	{
		wsprintf(wzQ, TEXT("SELECT TOP 1 * FROM CNTR WHERE USER_ID = ")
			TEXT("'%s'")
			TEXT(" AND CNTR_NO > %d ORDER BY CNTR_NO")
			, p->wzID
			, p->nLastCntrNo
		);
	}
	if (FALSE == db->ExecQuery(wzQ))
	{
		g_log.logW(NOTIFY, TEXT("CNTR Get Error(%s)(%s)"), db->GetError(), wzQ);
		return FALSE;
	}
	if (db->IsNextRow() == FALSE)
	{
		if (bHistory)
		{
			char returnBuf[128] = { 0 };
			_XAlphaGT::TRET_MSG* p = (_XAlphaGT::TRET_MSG*)returnBuf;
			memcpy(p->Code, CODE_MSG, sizeof(p->Code));
			memcpy(p->RetCode, RETCODE_CNTR_NODATA, sizeof(p->RetCode));
			p->Enter[0] = DEF_ENTER;
			RequestSendIO(pCK, returnBuf, strlen(returnBuf));
		}
		return TRUE;
	}

	int nCntrNo;
	wchar_t wzMasterId[32];
	wchar_t wzStkCd[32];
	wchar_t wzBsTp[32];
	int		nCntrQty;
	double	dCntrPrc;
	double	dClrPl;
	double	dCmsn;
	wchar_t wzClrTp[32];
	int		nBf_Pos;
	int		nAf_Pos;
	double	dBf_Avg;
	double	dAf_Avg;
	double	dBf_Amt; 
	double	dAf_Amt;
	wchar_t wzOrdTp[32];
	wchar_t wzCntrTm[32];
	int		nLvg;

	while (db->IsNextRow())
	{
		nCntrNo = 0; 
		nBf_Pos = 0;
		nAf_Pos = 0;
		nLvg = 0;
		nCntrQty = 0;

		dCntrPrc = 0;
		dClrPl = 0;
		dCmsn = 0;		
		dBf_Avg=0;
		dAf_Avg=0;
		dBf_Amt=0;
		dAf_Amt=0;

		
		ZeroMemory(wzStkCd, sizeof(wzStkCd));
		ZeroMemory(wzMasterId, sizeof(wzMasterId));
		ZeroMemory(wzBsTp, sizeof(wzBsTp));
		ZeroMemory(wzOrdTp, sizeof(wzOrdTp));
		ZeroMemory(wzCntrTm, sizeof(wzCntrTm));
		ZeroMemory(wzClrTp, sizeof(wzClrTp));

		nCntrNo = db->GetLong(TEXT("CNTR_NO"));
		nCntrQty = db->GetLong(TEXT("CNTR_QTY"));
		nBf_Pos = db->GetLong(TEXT("BF_NCLR_POS_QTY"));
		nAf_Pos = db->GetLong(TEXT("AF_NCLR_POS_QTY"));
		nLvg = db->GetLong(TEXT("LEVERAGE"));

		dCntrPrc = db->GetDbl(TEXT("CNTR_PRC"));
		dClrPl = db->GetDbl(TEXT("CLR_PL"));
		dCmsn = db->GetDbl(TEXT("CMSN_AMT"));
		dBf_Avg = db->GetDbl(TEXT("BF_AVG_PRC"));
		dAf_Avg = db->GetDbl(TEXT("AF_AVG_PRC"));
		dBf_Amt = db->GetDbl(TEXT("BF_NET_ACNT_AMT"));
		dAf_Amt = db->GetDbl(TEXT("AF_NET_ACNT_AMT"));

		db->GetStrWithLen(TEXT("USER_ID"), sizeof(wzMasterId), wzMasterId);
		db->GetStrWithLen(TEXT("STK_CD"), sizeof(wzStkCd), wzStkCd);
		db->GetStrWithLen(TEXT("BS_TP"), sizeof(wzBsTp), wzBsTp);
		db->GetStrWithLen(TEXT("ORD_TP"), sizeof(wzOrdTp), wzOrdTp);
		db->GetStrWithLen(TEXT("CNTR_TM"), sizeof(wzCntrTm), wzCntrTm);
		db->GetStrWithLen(TEXT("CLR_TP"), sizeof(wzClrTp), wzClrTp);

		if ( (bHistory==FALSE) && (p->nLastCntrNo >= nCntrNo) )
		{
			db->Next();
			continue;
		}

		_M_Cntr_Publish(bHistory, nCntrNo, wzMasterId, wzStkCd, wzBsTp, nCntrQty, dCntrPrc,
					dClrPl, dCmsn, wzClrTp, nBf_Pos, nAf_Pos, dBf_Avg, dAf_Avg, dBf_Amt, 
					dAf_Amt, wzOrdTp, wzCntrTm, nLvg, pCK);
		
		if( bHistory==FALSE )
			p->nLastCntrNo = nCntrNo;


		db->Next();
	}
	db->Close();

	return TRUE;
}

BOOL CDispatch::_M_Cntr_Publish( 
						BOOL bHistory
						, int cntrNo
						, wchar_t* userId
						, wchar_t* stkCd
						, wchar_t* bsTp
						, int cntrQty
						, double cntrPrc
						, double clrPl
						, double cmsn
						, wchar_t* clrTp
						, int bf_nclrQty
						, int af_nclrQty
						, double bf_avgPrc
						, double af_avgPrc
						, double bf_amt
						, double af_amt
						, wchar_t* ordTp
						, wchar_t* tradeTm
						, int lvg
						, COMPLETION_KEY* pCK
					)
{
	char zSendBuf[1024] = { 0 };
	_XAlphaGT::TCNTR* p = (_XAlphaGT::TCNTR*)zSendBuf;
	
	char z[128];
	int nTotLen = sizeof(_XAlphaGT::TCNTR);
	memset(zSendBuf, 0x20, nTotLen);

	if(bHistory)
		memcpy(p->Code, CODE_CNTR_HIST, strlen(CODE_CNTR));
	else
		memcpy(p->Code, CODE_CNTR, strlen(CODE_CNTR));


	sprintf(z, "%d", cntrNo);
	memcpy(p->cntrNo, z, strlen(z));

	U2A(userId, z);
	memcpy(p->masterId, z, strlen(z));

	U2A(stkCd, z);
	memcpy(p->stkCd, z, strlen(z));

	U2A(bsTp, z);
	memcpy(p->bsTp, z, strlen(z));

	sprintf(z, "%d", cntrQty);
	memcpy(p->cntrQty, z, strlen(z));

	sprintf(z, "%.5f", cntrPrc);
	memcpy(p->cntrPrc, z, strlen(z));

	sprintf(z, "%.0f", clrPl);
	memcpy(p->clrPl, z, strlen(z));
	
	sprintf(z, "%.0f", cmsn);
	memcpy(p->cmsn, z, strlen(z));

	U2A(clrTp, z);
	memcpy(p->clrTp, z, 1);

	sprintf(z, "%d", bf_nclrQty);
	memcpy(p->bf_nclrQty, z, strlen(z));
	
	sprintf(z, "%d", af_nclrQty);
	memcpy(p->af_nclrQty, z, strlen(z));
	
	sprintf(z, "%.5f", bf_avgPrc);
	memcpy(p->bf_avgPrc, z, strlen(z));

	sprintf(z, "%.5f", af_avgPrc);
	memcpy(p->af_avgPrc, z, strlen(z));

	sprintf(z, "%.0f", bf_amt);
	memcpy(p->bf_amt, z, strlen(z));

	sprintf(z, "%.0f", af_amt);
	memcpy(p->af_amt, z, strlen(z));
	
	U2A(ordTp, z);
	memcpy(p->ordTp, z, strlen(z));

	U2A(tradeTm, z);
	memcpy(p->tradeTm, z, strlen(z));

	sprintf(z, "%d", lvg);
	memcpy(p->lvg, z, strlen(z));

	p->Enter[0] = DEF_ENTER;

	g_log.log(INFO, "[CNTR](%.2s)(%d)(%.12s)(%.5s)(%.2s)(%c)(%.5f)(%d)",
			p->Code, cntrNo, p->masterId, p->stkCd, p->ordTp, p->bsTp[0], cntrPrc, cntrQty);

	if (bHistory)
		RequestSendIO(pCK, zSendBuf, nTotLen);
	else
		SendToAll(zSendBuf, nTotLen);

	return TRUE;
}


void CDispatch::SendToAll(char* psData, int nDataLen)
{
	lockCK();
	map<STR_SOCKET, COMPLETION_KEY*>::iterator it;
	for (it = m_mapCK.begin(); it != m_mapCK.end(); it++)
	{
		COMPLETION_KEY* p = (*it).second;

		RequestSendIO(p, psData, nDataLen);
	}
	unlockCK();
}

unsigned WINAPI CDispatch::DBReadThread(LPVOID lp)
{
	CDispatch* pThis = (CDispatch*)lp;
	BOOL bCopierCall = FALSE;
	BOOL bNotHistory = FALSE;
	while (pThis->m_bRun)
	{
		Sleep(pThis->m_nTimeoutDB);

		pThis->lockMasters();
		for (int i = 0; i < MAX_MASTERS_CNT; i++)
		{
			if(pThis->m_arrMasters[i].wzID[0]!=0x00)
				pThis->_M_Conn_MainProc(&pThis->m_arrMasters[i], bCopierCall);
		}
		pThis->unlockMasters();

		pThis->lockMasters();
		for (int i = 0; i < MAX_MASTERS_CNT; i++)
		{
			if (pThis->m_arrMasters[i].wzID[0] != 0x00)
				pThis->_M_Cntr_MainProc(&pThis->m_arrMasters[i], bNotHistory, NULL);
		}
		pThis->unlockMasters();
	}
	return 0;
}
