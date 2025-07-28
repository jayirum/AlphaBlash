#include "Main.h"
#include "Dispatch.h"
#include <process.h>
#include "../Common/Util.h"
//#include "MemPool.h"
#include "../Common/LogMsg.h"
#include "../Common/IRExcept.h"
#include "../Common/IRUM_Common.h"
#include "../Common/CommonFunc.h"
#include <string.h>
#include "../Common/MemPool.h"

extern CMemPool	g_memPool;
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
	m_lIocpThreadIdx = 0;
	m_bSendCntrInit = FALSE;
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


void CDispatch::FormatMasterId(wchar_t* pwzMasterID, _Out_ string* pID)
{
	char zMasterID[32];
	U2A(pwzMasterID, zMasterID);
	FormatMasterId(zMasterID, pID);
}

void CDispatch::FormatMasterId(char* pzMasterID, _Out_ string* pID)
{
	CUtil::TrimAll(pzMasterID, strlen(pzMasterID));

	char zID[128] = { 0 };
	for (int i = 0; i < strlen(pzMasterID); i++)
	{
		if (islower(pzMasterID[i]))
			zID[i] = _toupper(pzMasterID[i]);
		else
			zID[i] = pzMasterID[i];

	}

	*pID = string(zID);
	
}

BOOL CDispatch::Create_MasterInstances()
{
	wchar_t wzRslt[128] = { 0, };
	wchar_t wzKey[128];

	m_nMastersCnt = 0;
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), TEXT("CNT"), wzRslt);
	m_nMastersCnt = _ttoi(wzRslt);

	if (m_nMastersCnt <= 0)
	{
		g_log.log(ERR, "Master 정보(CNT)가 CONFIG 파일에 없습니다.");
		return FALSE;
	}
	
	for (int i = 1; i < m_nMastersCnt+1; i++)
	{
		wsprintf(wzKey, TEXT("MASTER_%d"), i);
		CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), wzKey, wzRslt);
		
		string sID;
		FormatMasterId(wzRslt, &sID);

		CMaster* p = new CMaster;
		if (!p->Initialize(i, m_dSendToAllThread))
		{
			return FALSE;
		}
		m_mapMaster[sID] = p;
		g_log.log(INFO, "[Read Master](%s)", sID.c_str());
	}
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

BOOL CDispatch::Initialize( )
{
	InitializeCriticalSection(&m_csCK);
	InitializeCriticalSection(&m_csMasters);

	ReadIPPOrt();

	if (!ReadCnfg_IocpCnt())
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


	// send data thread
	m_hSendToAllThread = (HANDLE)_beginthreadex(NULL, 0, &SendToAllThread, this, 0, &m_dSendToAllThread);

	Create_MasterInstances();

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
	for( int i=0; i<m_nIocpThreadCnt; i++)
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
	map<STR_MASTER_ID, CMaster*>::iterator it;
	lockMasters();
	for (it = m_mapMaster.begin(); it != m_mapMaster.end(); it++)
	{
		delete (*it).second;
	}
	m_mapMaster.clear();
	unlockMasters();
}

void CDispatch::Finalize()
{
	m_bRun = FALSE;

	for (int i = 0; i < m_nIocpThreadCnt; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			, 0
			, NULL
			, NULL
		);
	}

	RemoveAllMaster();


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
	DeleteCriticalSection(&m_csMasters);

	//SAFE_DELETE(m_pack);
	WSACleanup();

	//delete m_pDBPool;
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
	_XAlpha::TRET_MSG* p = (_XAlpha::TRET_MSG*)zMsgBuf;
	memcpy(p->Code, CODE_MSG, strlen(CODE_MSG));
	p->Enter[0] = DEF_ENTER;


	_XAlpha::TCL_PWD* pPwd = (_XAlpha::TCL_PWD*)pRecvBuf;
	if (strncmp(zPwd, pPwd->Pwd, sizeof(pPwd->Pwd)) != 0)
	{
		memcpy(p->RetCode, RETCODE_PWD_WRONG, strlen(RETCODE_PWD_WRONG));
		RequestSendIO(pCK, zMsgBuf, strlen(zMsgBuf));
		g_log.log(ERR, "[Wrong Password](%s)", zPwd);
		return;
	}
	
	// Pwd ok
	memcpy(p->RetCode, RETCODE_PWD_OK, strlen(RETCODE_PWD_OK));
	RequestSendIO(pCK, zMsgBuf, strlen(zMsgBuf));
	g_log.log(INFO, "[Password OK](%s)", zPwd);


	BOOL bCopierCall = TRUE;

	//	로그인 정보를 전달한다.
	lockMasters();
	map<STR_MASTER_ID, CMaster*>::iterator it;
	for (it=m_mapMaster.begin(); it!= m_mapMaster.end(); it++)
	{
		(*it).second->CopierRqst_LogOnOff();
	}
	unlockMasters();
}



void CDispatch::Copier_Rqst_CntrHist(COMPLETION_KEY* pCK, char* pRecvBuf)
{
	_XAlpha::TCL_RQST_CNTR_HIST* pRqst = (_XAlpha::TCL_RQST_CNTR_HIST*)pRecvBuf;
	char zMasterID[32];
	sprintf(zMasterID, "%.*s", sizeof(pRqst->MasterID), pRqst->MasterID);

	BOOL bHistory = TRUE;

	string sMasterID;
	FormatMasterId(zMasterID, &sMasterID);
	lockMasters();
	map<STR_MASTER_ID, CMaster*>::iterator it = m_mapMaster.find(zMasterID);
	if (it == m_mapMaster.end())
	{
		g_log.log(ERR, "Faile to find Master ID class(Copier_Rqst_CntrHist)(%s)", zMasterID);
		return;
	}

	(*it).second->CopierRqst_CntrHist(pCK);
	unlockMasters();
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
		pThis->Exist_SameIP(sockClient, zClientIp);
		//if (pThis->Exist_SameIP(sockClient, zClientIp))
		//{
		//	g_log.logW(NOTIFY, TEXT("같은 IP 중복접속시도. 거부.(%s)"), zClientIp);
		//	closesocket(sockClient);
		//	continue;
		//}

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
	SOCKET sock = pCK->sock;

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

unsigned WINAPI CDispatch::SendToAllThread(LPVOID lp)
{
	CDispatch* pThis = (CDispatch*)lp;
	MSG msg;
	while (pThis->m_bRun)
	{
		Sleep(10);

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_SENDALL_DATA:
				pThis->SendToAll((char*)msg.lParam, (int)msg.wParam);				
				break;
			}
			g_memPool.release((char*)msg.lParam);
		}
	}
	return 0;
}
