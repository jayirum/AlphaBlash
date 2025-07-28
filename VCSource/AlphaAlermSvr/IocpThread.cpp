
#include "IocpThread.h"
#include <process.h>
#include "../CommonAnsi/Util.h"
#include "../CommonAnsi/LogMsg.h"
#include "../CommonAnsi/IRExcept.h"


extern CLogMsg	g_log;
extern char		g_zConfig[_MAX_PATH];

CIocp::CIocp()
{
	m_hCompletionPort	= NULL;
	m_hListenEvent		= NULL;
	m_sockListen		= INVALID_SOCKET;
	m_hThread_Listen		= NULL;
	m_dThread_Listen		= 0;
	m_nListenPort		= 0;
	m_hListenEvent		= NULL;
	m_bRun				= TRUE;
	m_lIocpThreadIdx = 0;
}
CIocp::~CIocp()
{
	Finalize();
}



BOOL CIocp::ReadIPPOrt()
{
	return __ReadAlermSvr_IpPort(m_zListenIP, &m_nListenPort);
}

BOOL CIocp::Initialize( )
{
	if (InitEmail() == FALSE) {
		return FALSE;
	}

	if (InitTelegram() == FALSE)
		return FALSE;

	InitializeCriticalSection(&m_csCK);

	if (!InitListen()) {
		return FALSE;
	}
	m_hThread_Listen = (HANDLE)_beginthreadex(NULL, 0, &Thread_Listen, this, 0, &m_dThread_Listen);

	// CPU의 수를 알기 위해서....IOCP에서 사용할 쓰레드의 수는 cpu개수 또는 cpu*2
	SYSTEM_INFO         systemInfo;
	GetSystemInfo(&systemInfo);
	m_dwThreadCount = MAX_IOCPTHREAD_CNT;	// systemInfo.dwNumberOfProcessors;

	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		g_log.log(LOGTP_ERR, "IOCP Create Error:%d", GetLastError());
		return FALSE;
	}

	// 실제로 recv와 send를 담당할 스레드를 생성한다.
	unsigned int dwID;
	for (unsigned int n = 0; n < m_dwThreadCount; n++)
	{
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_IocpWork, this, 0, &dwID);
		CloseHandle(h);
	}

	return TRUE;
}


BOOL CIocp::InitEmail()
{
	char zFile[128], zFnName[128], zSender[128], zSenderDevicePwd[128], zReceiver[128];
	CUtil::GetConfig(g_zConfig, "EMAIL_INFO", "PYTHON_FILE", zFile);
	CUtil::GetConfig(g_zConfig, "EMAIL_INFO", "FUNCTION_NAME", zFnName);
	CUtil::GetConfig(g_zConfig, "EMAIL_INFO", "SENDER_ACC", zSender);
	CUtil::GetConfig(g_zConfig, "EMAIL_INFO", "SENDER_DEVICE_PWD", zSenderDevicePwd);
	CUtil::GetConfig(g_zConfig, "EMAIL_INFO", "RECEIVER_ACC", zReceiver);


	g_log.log(INFO, "Email Init(Python File:%s, Sender:%s, Pwd:%s, Receiver:%s\n", zFile, zSender, zSenderDevicePwd, zReceiver);
	m_emailSender.SetEmailInfo(zFile, zFnName, zSender, zSenderDevicePwd, zReceiver);
	return TRUE;
}


BOOL CIocp::InitTelegram()
{
	char zUrl[1024] = { 0 }, zToken[256] = { 0 }, zChatID[256] = { 0 };
	CUtil::GetConfig(g_zConfig, "TELEGRAM_INFO", "URL", zUrl);
	CUtil::GetConfig(g_zConfig, "TELEGRAM_INFO", "TOKEN", zToken);
	CUtil::GetConfig(g_zConfig, "TELEGRAM_INFO", "CHAT_ID", zChatID);

	m_teleSender.Initialize();
	m_teleSender.SetTelegramInfo(zUrl, zToken, zChatID);
	g_log.log(INFO, "Telegram Init(Url:%s)(Token:%s)(ChatID:%s)", zUrl, zToken, zChatID);
	return TRUE;
}


BOOL CIocp::InitListen()
{
	ReadIPPOrt();

	g_log.log(INFO, "CIocp::InitListen() starts..........");
	CloseListenSock();

	WORD	wVersionRequired;
	WSADATA	wsaData;

	//// WSAStartup
	wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		g_log.log(ERR, "WSAStartup Error:%d", GetLastError());
		return FALSE;
	}

	//DumpWsaData(&wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		g_log.log(ERR, "RequiredVersion not Usable");
		return FALSE;
	}


	// Create a listening socket 
	if ((m_sockListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		g_log.log(ERR, "create socket error: %d", WSAGetLastError());
		return FALSE;
	}

	ReadIPPOrt();

	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = inet_addr(m_zListenIP);
	InternetAddr.sin_port = htons(m_nListenPort);

	BOOL opt = TRUE;
	int optlen = sizeof(opt);
	setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (const char far *)&opt, optlen);


	if (::bind(m_sockListen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		g_log.log(ERR, "bind error (ip:%s) (port:%d) (err:%d)", m_zListenIP, m_nListenPort, WSAGetLastError());
		return FALSE;
	}
	// Prepare socket for listening 
	if (listen(m_sockListen, 5) == SOCKET_ERROR)
	{
		g_log.log(ERR, "listen error: %d", WSAGetLastError());
		return FALSE;
	}

	m_hListenEvent = WSACreateEvent();
	if (WSAEventSelect(m_sockListen, m_hListenEvent, FD_ACCEPT)) {

		g_log.log(ERR, "WSAEventSelect for accept error: %d", WSAGetLastError());
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
		structLinger.l_onoff = TRUE;
		structLinger.l_linger = 0;

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
unsigned WINAPI CIocp::Thread_IocpWork(LPVOID lp)
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
	int nLoop = 0;
	int nRet = 0;
	
	long iocpIdx = InterlockedIncrement(&pThis->m_lIocpThreadIdx)-1;

	g_log.log(LOGTP_SUCC, "[%d][%d]IOCPThread Start.....", iocpIdx, GetCurrentThreadId());


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
			g_log.log(INFO, "[IOCP](pOverlap == NULL)");
			break;
		}


		pIoContext = (IO_CONTEXT*)pOverlap;

		if (FALSE == bRet)
		{
			if (pIoContext->context == CTX_RQST_RECV)
			{				
				//pThis->RecvLogOffAndClose(pCompletionKey);
				pThis->DeleteSocket(pCompletionKey);
				g_log.log(LOGTP_ERR, "[IOCP](if (FALSE == bRet)) call DeleteSocket()");
			}
			Sleep(3000);
			continue;
		}
		
		if (dwIoSize == 0)
		{
			g_log.log(INFO, "[IOCP]if (dwIoSize == 0)");
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
			printf("[RECV](%s)", zRecvBuff);
			g_log.log(INFO, "[RECV](%s)", zRecvBuff);

			nRet = pThis->m_buffering.AddPacket(pIoContext->buf);
			if (nRet < 0) {
				g_log.log(ERR, "[%d][%d]AddPacket Error(%s)", iocpIdx, GetCurrentThreadId(), pThis->m_buffering.GetErrMsg());
			}
			pThis->RequestRecvIO(pCompletionKey);

			BOOL bContinue, bCopied;
			nLoop = 0;
			do
			{
				ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
				nRet = 0;
				bCopied = pThis->m_buffering.GetOnePacket(zRecvBuff, &bContinue);
				if (bCopied == TRUE)
				{
					//g_log.log(INFO, "Buffering[%d](%s)\n", ++nLoop, zRecvBuff);
					InterlockedIncrement(&pCompletionKey->refcnt);
					pThis->SendAlermData( zRecvBuff);
					InterlockedDecrement(&pCompletionKey->refcnt);
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


/*

struct NOTI_LOG
{
	char STX[1];
	int AlermTp;					//ALERM_EMAIL, ALERM_TELEGRAM, ALERM_BOTH
	char zAppName[LEN_APP_NAME];
	char zBody[LEN_BODY_NAME];
	char ETX[1];
};

*/
void CIocp::SendAlermData(const char* pRecvData)
{
	NOTI_LOG* pNoti = (NOTI_LOG*)pRecvData;
	char zAppName[128], zTitle[128], zBody[512];

	int len = sizeof(pNoti->zAppName);
	sprintf(zAppName, "%.*s", len, pNoti->zAppName);
	CUtil::RTrim(zAppName, len);

	len = sizeof(pNoti->zBody);
	sprintf(zBody, "%.*s", len, pNoti->zBody);
	CUtil::RTrim(zBody, len);

	char zTp[3];
	sprintf(zTp, "%.2s", pNoti->AlermTp);
	int nTp = atoi(zTp);
	if (nTp == ALERM_EMAIL || nTp == ALERM_BOTH)
	{
		sprintf(zTitle, "[%s]Sent Alerm", zAppName);
		if (m_emailSender.SendEmail(zTitle, zBody))
		{
			sprintf(m_zMsg, "[SEND_EMAIL OK to [%s](%s)(%s)", m_emailSender.Receiver(), zTitle, zBody);
			g_log.Log(INFO, m_zMsg);
		}
	}
	if (nTp == ALERM_TELEGRAM || nTp == ALERM_BOTH)
	{
		if(!m_teleSender.SendTelegram(zBody))
			g_log.log(INFO, "[SEND_TELEGRAM Error](%s)", m_teleSender.GetMsg());
		else
		g_log.log(INFO, "[SEND_TELEGRAM OK](%s)", zBody);
	}
}



unsigned WINAPI CIocp::Thread_Listen(LPVOID lp)
{
	CIocp *pThis = (CIocp*)lp;
	g_log.log(INFO, "Thread_Listen starts.....[%s][%d]", pThis->m_zListenIP, pThis->m_nListenPort);

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
				//pThis->SendMessageToIocpThread(CTX_MT4PING);
				//if (++nHearbeatCnt > 30)
				//{
				//	pThis->api_hearbeat();
				//	printf("heartbeat.........\n");
				//	nHearbeatCnt = 0;
				//}
				continue;
			}
		}

		WSAResetEvent(pThis->m_hListenEvent);		
		
		SOCKET sockClient = accept(pThis->m_sockListen, (LPSOCKADDR)&sinClient, &sinSize);
		if (sockClient == INVALID_SOCKET) {
			int nErr = WSAGetLastError();
			g_log.log(LOGTP_ERR, "accept error:%d", nErr);
			//if (nErr==10038) 
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
			g_log.log(LOGTP_ERR, "setsockopt error : %d", WSAGetLastError);
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

		g_log.log(LOGTP_SUCC, "Accept & Add IOCP.[socket:%d][CK:%x]", sockClient, pCK);

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
				g_log.log(LOGTP_ERR, "WSASend error : %d", WSAGetLastError);
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		g_log.log(ERR, "WSASend try catch error [CIocp]");
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
		pRecv->wsaBuf.len = BUF_LEN;
		pRecv->context = CTX_RQST_RECV;


		int nRet = WSARecv(pCK->sock
			, &(pRecv->wsaBuf)
			, 1, &dwNumberOfBytesRecvd, &dwFlags
			, &(pRecv->overLapped)
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				g_log.log(LOGTP_ERR, "WSARecv error : %d", WSAGetLastError);
				bRet = FALSE;
			}
		}
	}
	catch (...) {
		g_log.log(LOGTP_ERR, "WSASend TRY CATCH");
		bRet = FALSE;
	}

	if (!bRet)
		delete pRecv;

	return;
}