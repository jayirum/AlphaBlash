
#include "IocpThread.h"
#include <process.h>
#include "../../CommonAnsi/Util.h"
#include "../../CommonAnsi/LogMsg.h"
#include "../../CommonAnsi/IRExcept.h"
#include "../../Common/AlphaInc.h"
#include "../../Common/TimeInterval.h"
//#include "Compose_DBSave_String.h"
#include "Inc.h"

#include <assert.h>

//extern CLogMsg	g_log;
extern TCHAR	g_zConfig[_MAX_PATH];
extern BOOL		g_bDebugLog;
CIocp::CIocp()
{
	m_hCompletionPort	= NULL;
	m_hListenEvent		= NULL;
	m_sockListen		= INVALID_SOCKET;
	m_nListenPort		= 0;
	m_hListenEvent		= NULL;
	m_bRun				= TRUE;
	//m_pDBPool			= NULL;
	m_lIocpThreadIdx = 0;

	m_hThread_Listen = m_hParsing = NULL;	// m_hThread_OrderSend = m_hNoMoreOpenThread = m_hWorkerThread = NULL;
	m_unThread_Listen = m_unParsing = 0;	// m_unThread_OrderSend = m_unNoMoreOpenThread = m_unWorkerThread = 0;

	//m_bUse_NoMoreOpenByTime = FALSE;
	//m_bUse_MarketCloseClear = FALSE;;
	//m_bMarketCloseClearedAlready = FALSE;
	//m_bWeekendStartAlready = FALSE;
	//m_bTradeClose = FALSE;
	//ZeroMemory(m_zLastSnapshotHour, sizeof(m_zLastSnapshotHour));
	//ZeroMemory(m_zMarketCloseClearTime, sizeof(m_zMarketCloseClearTime));
}
CIocp::~CIocp()
{
	Finalize();
}


void CIocp::Finalize()
{
	m_bRun = FALSE;

	lockSymbol();
	IT_MAP_SYMBOL itSymbol;
	for (itSymbol = m_mapSymbol.begin(); itSymbol != m_mapSymbol.end(); itSymbol++)
	{
		delete (*itSymbol).second;
	}
	m_mapSymbol.clear();
	unlockSymbol();

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
	DeleteCriticalSection(&m_csSymbol);
	//DeleteCriticalSection(&m_csToSend);
	//DeleteCriticalSection(&m_csPacket);

	//SAFE_DELETE(m_pack);
	WSACleanup();

	//delete m_pDBPool;
}


BOOL CIocp::ReadIPPOrt()
{
	char zTemp[1024] = { 0, };
	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("LISTEN_IP"), m_zListenIP);
	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("LISTEN_PORT"), zTemp);
	m_nListenPort = atoi(zTemp);
	
	return TRUE;
}


BOOL CIocp::ReadWorkerThreadCnt(int* pnCnt)
{
	char zTemp[1024] = { 0, };
	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("WORKER_THREAD_CNT"), zTemp);
	*pnCnt = atoi(zTemp);

	return (zTemp[0]!=NULL);
}

BOOL CIocp::Initialize( )
{
	ReadIPPOrt();

	InitializeCriticalSection(&m_csCK);
	InitializeCriticalSection(&m_csSymbol);

	if (!LoadSymbols())
		return FALSE;


	if (!InitListen()) {
		return FALSE;
	}
	LOGGING(INFO, TRUE, "Init Listen(%s)(%d)", m_zListenIP, m_nListenPort);

	m_hThread_Listen = (HANDLE)_beginthreadex(NULL, 0, &Thread_Listen, this, 0, &m_unThread_Listen);
	m_hParsing = (HANDLE)_beginthreadex(NULL, 0, &Thread_Parsing, this, 0, &m_unParsing);

	// CPU의 수를 알기 위해서....IOCP에서 사용할 쓰레드의 수는 cpu개수 또는 cpu*2
	SYSTEM_INFO         systemInfo;
	GetSystemInfo(&systemInfo);
	m_dwThreadCount = 1;	// systemInfo.dwNumberOfProcessors;

	
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_dwThreadCount);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		LOGGING(LOGTP_ERR, TRUE, FALSE, TEXT("IOCP Create Error:%d"), GetLastError());
		return FALSE;
	}

	// 실제로 recv와 send를 담당할 스레드를 생성한다.
	
	for (unsigned int n = 0; n < m_dwThreadCount; n++)
	{
		UINT dwID;
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_Iocp, this, 0, &dwID);
		CloseHandle(h);
	}

	return TRUE;
}


BOOL CIocp::LoadSymbols()
{
	char buffer[128] = { 0 };
	CUtil::GetConfig(g_zConfig, TEXT("SYMBOLS"), TEXT("COUNT"), buffer);
	if (buffer[0] == NULL)
	{
		LOGGING(ERR, TRUE, FALSE, "Failed to get symbols count from config file");
		return FALSE;
	}

	char zIdx[32];
	int nCnt = atoi(buffer);

	// ArbTraceLatency.ini
	// # symbol / point size / NetProfitPts / TraceCounts / SlippagePts

	for (int i = 0; i < nCnt; i++)
	{
		char zLine[512] = { 0 };
		sprintf(zIdx, "%d", i);
		CUtil::GetConfig(g_zConfig, TEXT("SYMBOLS"), zIdx, zLine);
		vector<string> vec;
		CUtil::SplitData(zLine, '/', &vec);

		CCompareLatency* p = new CCompareLatency();
		if ( !p->Initialize(vec.at(0), vec.at(1), vec.at(2), vec.at(3), vec.at(4)) )
		{
			//TODO LOGGING
			return FALSE;
		}
		
		m_mapSymbol[vec.at(0)] = p;

		LOGGING(INFO, TRUE, "Load Symbol[%s](Pointsize:%.5f)(NetProfitPts:%d)(TraceCounts:%d)(SlippagePts:%d)",
			vec.at(0).c_str(),
			atof(vec.at(1).c_str()),
			atoi(vec.at(2).c_str()),
			atoi(vec.at(3).c_str()),
			atoi(vec.at(4).c_str())
		);
	}
	return TRUE;
}

BOOL CIocp::InitListen()
{
	LOGGING(INFO, TRUE, TEXT("CIocp::InitListen() starts.........."));
	CloseListenSock();

	WORD	wVersionRequired;
	WSADATA	wsaData;

	//// WSAStartup
	wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("WSAStartup Error:%d"), GetLastError());
		return FALSE;
	}

	//DumpWsaData(&wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("RequiredVersion not Usable"));
		return FALSE;
	}


	// Create a listening socket 
	if ((m_sockListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		LOGGING(ERR,TRUE, TEXT("create socket error: %d"), WSAGetLastError());
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
		LOGGING(ERR, TRUE, TEXT("bind error (ip:%s) (port:%d) (err:%d)"), m_zListenIP, m_nListenPort, WSAGetLastError());
		return FALSE;
	}
	// Prepare socket for listening 
	if (listen(m_sockListen, 5) == SOCKET_ERROR)
	{
		LOGGING(ERR, TRUE, TEXT("listen error: %d"), WSAGetLastError());
		return FALSE;
	}

	m_hListenEvent = WSACreateEvent();
	if (WSAEventSelect(m_sockListen, m_hListenEvent, FD_ACCEPT)) {

		LOGGING(ERR, TRUE, TEXT("WSAEventSelect for accept error: %d"), WSAGetLastError());
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
unsigned WINAPI CIocp::Thread_Iocp(LPVOID lp)
{
	CIocp* pThis = (CIocp*)lp;

	COMPLETION_KEY	*pCK = NULL;
	IO_CONTEXT		*pIoContext = NULL;
	DWORD			dwBytesTransferred = 0;
	DWORD           dwIoSize = 0;
	DWORD           dwRecvNumBytes = 0;
	DWORD           dwSendNumBytes = 0;
	DWORD           dwFlags = 0;
	LPOVERLAPPED	pOverlap = NULL;
	BOOL bRet;
	
	long iocpIdx = InterlockedIncrement(&pThis->m_lIocpThreadIdx)-1;

	LOGGING(INFO, FALSE, TEXT("[%d][%d]IOCPThread Start....."), iocpIdx, GetCurrentThreadId());

	while (pThis->m_bRun)
	{
		bRet = GetQueuedCompletionStatus(pThis->m_hCompletionPort,
			&dwIoSize,
			(LPDWORD)&pCK, 	//여기로는 실제 CK 는 던지지 않는다. 무조건 new, delete 하므로. 
			(LPOVERLAPPED *)&pOverlap,
			INFINITE);

		// Finalize 에서 PostQueuedCompletionStatus 에 NULL 입력
		if (pCK == NULL) // 종료
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
				LOGGING(INFO, TRUE, "[Close Client-1]GetQueuedCompletionStatus failed(%d)", pCK->sock);
				pThis->DeleteSocket(pCK);
			}
			Sleep(3000);
			continue;
		}
		
		if (dwIoSize == 0)
		{
			LOGGING(INFO, TRUE, "[Close Client-1]dwIoSize == 0 (%d)", pCK->sock);
			if (pIoContext->context == CTX_RQST_RECV)
				pThis->DeleteSocket(pCK);
			continue;
		}
		
		if (pIoContext->context == CTX_DIE)
		{
			break;
		}

		// 데이터 수신
		if (pIoContext->context == CTX_RQST_RECV)
		{
			pThis->RequestRecvIO(pCK);

			pThis->m_parser.AddPacket(pIoContext->buf, dwIoSize);
			
			PostThreadMessage(pThis->m_unParsing, WM_RECEIVE_DATA, (WPARAM)0, (LPARAM)pCK);
		}

		if (pIoContext->context == CTX_RQST_SEND)
		{
			
		}
		delete pIoContext;

	} // while

	//mt4helper.disconnect();

	return 0;
}


unsigned WINAPI CIocp::Thread_Parsing(LPVOID lp)
{
	CIocp* pThis = (CIocp*)lp;

	char zRecvBuff[BUF_LEN];
	string sSymbol;
	int nRet;
	while (pThis->m_bRun)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			BOOL bDelete = TRUE;
			if (msg.message != WM_RECEIVE_DATA)
				continue;

			
			COMPLETION_KEY* pCK = (COMPLETION_KEY*)msg.lParam;	//DO NOT DELETE pCK

			BOOL bContinue;
			do
			{
				ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
				nRet = 0;
				bContinue = pThis->m_parser.GetOnePacketWithHeaderWithLock(&nRet, zRecvBuff);
				if (nRet > 0)
				{	
					CProtoUtils util;
					if (!util.GetSymbol(zRecvBuff,_Out_ sSymbol))
					{
						//TODO LOGGING
						continue;
					}

					InterlockedIncrement(&pCK->refcnt);

					pThis->lockSymbol();
					IT_MAP_SYMBOL itMap = pThis->m_mapSymbol.find(sSymbol);
					if (itMap != pThis->m_mapSymbol.end())
					{
						(*itMap).second->Execute(zRecvBuff);
					}
					pThis->unlockSymbol();

					InterlockedDecrement(&pCK->refcnt);
				}
				Sleep(0);
			} while (bContinue);
		} // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	} // while(pThis->m_bRun)
	
	
	return 0;
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

	LOGGING(INFO, TRUE, "[Return Error to Client](%s)", zSendBuff);
	RequestSendIO(pCK->sock, zSendBuff, nLen);
}





unsigned WINAPI CIocp::Thread_Listen(LPVOID lp)
{
	CIocp *pThis = (CIocp*)lp;
	LOGGING(INFO, FALSE, TEXT("Thread_Listen starts.....[%s][%d]"), pThis->m_zListenIP, pThis->m_nListenPort);

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
			LOGGING(ERR, TRUE, TEXT("accept error:%d"), nErr);
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
			LOGGING(ERR, TRUE, TEXT("setsockopt error : %d"), WSAGetLastError);
			continue;;
		}


		//	CK 와 IOCP 연결
		COMPLETION_KEY* pCK = new COMPLETION_KEY;
		pCK->sock		= sockClient;
		pCK->refcnt = 0;

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

		LOGGING(INFO, TRUE, TEXT("Accept & Add IOCP.[socket:%d][CK:%x]"), sockClient, pCK);

		//	최초 RECV IO 요청
		pThis->RequestRecvIO(pCK);

		//pThis->SendMessageToIocpThread(CTX_MT4PING);

	}//while

	return 0;
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
				LOGGING(LOGTP_ERR, TRUE, TEXT("WSARecv error : %d"), WSAGetLastError());
				bRet = FALSE;
			}
		}
	}
	catch (...) {
		LOGGING(LOGTP_ERR, TRUE, TEXT("WSASend TRY CATCH"));
		bRet = FALSE;
	}

	if (!bRet)
		delete pRecv;

	//printf("RequestRecvIO ok\n");
	return;
}


VOID CIocp::RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen)
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

		int nRet = WSASend(sock
			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
			, 1					// wsaBuf 포인터 갯수
			, &dwOutBytes		// 전송된 바이트 수
			, dwFlags
			, &pSend->overLapped	// overlapped 포인터
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				LOGGING(LOGTP_ERR, TRUE,  TEXT("WSASend error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		LOGGING(ERR, TRUE, TEXT("WSASend try catch error [CIocp]"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;
	else
		LOGGING(INFO, FALSE, "[SEND](sock:%d)(%s)",sock, pSendBuf);
	return;
}
