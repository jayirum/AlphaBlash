
#include "IocpThread.h"
#include <process.h>
#include "../../Common/Util.h"
#include "../../Common/LogMsg.h"
#include "../../Common/IRExcept.h"
#include "../../Common/AlphaInc.h"
#include "../../Common/TimeInterval.h"
//#include "Compose_DBSave_String.h"
#include "Inc.h"

#include <assert.h>

//extern CLogMsg	g_log;
extern CConfig	g_config;
extern BOOL		g_bDebugLog;
CIocp::CIocp()
{
	m_hCompletionPort	= NULL;
	m_hListenEvent		= NULL;
	m_sockListen		= INVALID_SOCKET;
	m_hListenEvent		= NULL;
	m_bRun				= TRUE;
	//m_pDBPool			= NULL;
	m_lIocpThreadIdx = 0;

	m_hThread_Listen = m_hParsing = m_hThread_Config = NULL;
	m_unThread_Listen = m_unParsing = m_unThread_Config = 0;

}
CIocp::~CIocp()
{
	Finalize();
}


void CIocp::Finalize()
{
	m_bRun = FALSE;

	//LockUser();
	//IT_MAP_USER itSymbol;
	//for (itSymbol = m_mapUser.begin(); itSymbol != m_mapUser.end(); itSymbol++)
	//{
	//	delete (*itSymbol).second;
	//}
	//m_mapUser.clear();
	//UnlockUser();

	EnterCriticalSection(&m_csAuth);
	for (unsigned int k = 0; k < m_vecAuth.size(); k++)
	{
		delete m_vecAuth[k];
	}
	m_vecAuth.clear();
	LeaveCriticalSection(&m_csAuth);

	for (UINT i = 0; i < WORKTHREAD_CNT; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			, 0
			, NULL
			, NULL
		);
	}



	CloseListenSock();

	SAFE_CLOSEHANDLE(m_hCompletionPort);

	//DeleteCriticalSection(&m_csUser);

	//SAFE_DELETE(m_pack);
	WSACleanup();

	//delete m_pDBPool;
}

//
//BOOL CIocp::ReadIPPOrt()
//{
//	char zTemp[1024] = { 0, };
//	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("LISTEN_IP"), m_zListenIP);
//	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("LISTEN_PORT"), zTemp);
//	m_nListenPort = atoi(zTemp);
//	
//	return TRUE;
//}

//
//BOOL CIocp::ReadWorkerThreadCnt(int* pnCnt)
//{
//	char zTemp[1024] = { 0, };
//	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("WORKER_THREAD_CNT"), zTemp);
//	*pnCnt = atoi(zTemp);
//
//	return (zTemp[0]!=NULL);
//}

BOOL CIocp::Initialize( )
{
	InitializeCriticalSection(&m_csAuth);

	if (g_config.ReLoad_ConfigInfo(TRUE)!= CNFG_SUCC)
		return FALSE;

	if (!InitListen()) {
		return FALSE;
	}

	char zIP[32], zPort[32];
	g_config.getVal("APP_CONFIG", "LISTEN_IP", zIP);
	g_config.getVal("APP_CONFIG", "LISTEN_PORT", zPort);
	LOGGING(INFO, TRUE, "Init Listen(%s)(%s)", zIP, zPort);

	m_hThread_Listen	= (HANDLE)_beginthreadex(NULL, 0, &Thread_Listen, this, 0, &m_unThread_Listen);
	m_hParsing			= (HANDLE)_beginthreadex(NULL, 0, &Thread_Parsing, this, 0, &m_unParsing);
	m_hThread_Config	= (HANDLE)_beginthreadex(NULL, 0, &Thread_Config, this, 0, &m_unThread_Config);

	// CPU의 수를 알기 위해서....IOCP에서 사용할 쓰레드의 수는 cpu개수 또는 cpu*2
	SYSTEM_INFO         systemInfo;
	GetSystemInfo(&systemInfo);
	m_dwThreadCount = 1;	// systemInfo.dwNumberOfProcessors;

	
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_dwThreadCount);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("IOCP Create Error:%d"), GetLastError());
		return FALSE;
	}

	// 실제로 recv와 send를 담당할 스레드를 생성한다.
	
	for (unsigned int n = 0; n < m_dwThreadCount; n++)
	{
		UINT dwID;
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_Iocp, this, 0, &dwID);
		CloseHandle(h);
	}

	//if (!CreateSymbolThread())
	//	return FALSE;



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


	char data[1024] = { 0 };
	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = inet_addr(g_config.getListenIP());
	InternetAddr.sin_port = htons(g_config.getListenPort());

	BOOL opt = TRUE;
	int optlen = sizeof(opt);
	setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (const char far *)&opt, optlen);


	if (::bind(m_sockListen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		LOGGING(ERR, TRUE, TEXT("bind error (ip:%s) (port:%s) (err:%d)"), g_config.getVal("APP_CONFIG", "LISTEN_IP", data), g_config.getVal("APP_CONFIG", "LISTEN_PORT", data), WSAGetLastError());
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
	for (int i = 0; i < 100; i++)
	{
		if (!pCompletionKey->Is_BeingUsed())
			break;
		Sleep(10);
	}

	shutdown(pCompletionKey->sock, SD_BOTH);

	// TIME-WAIT 없도록
	struct linger structLinger;
	structLinger.l_onoff = TRUE;
	structLinger.l_linger = 0;
	setsockopt(pCompletionKey->sock, SOL_SOCKET, SO_LINGER, (LPSTR)& structLinger, sizeof(structLinger));
	closesocket(pCompletionKey->sock);
	delete pCompletionKey;
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
			LOGGING(INFO, TRUE, "[Close Client-2]dwIoSize == 0 (%d)", pCK->sock);
			if (pIoContext->context == CTX_RQST_RECV)
			{
				LOGGING(INFO,TRUE, "Delete Client(%d)(%x)", pCK->sock, pCK);
				pThis->DeleteSocket(pCK);
			}
				
			continue;
		}
		
		if (pIoContext->context == CTX_DIE)
		{
			break;
		}

		// 데이터 수신
		if (pIoContext->context == CTX_RQST_RECV)
		{
			RequestRecvIO(pCK);

			LOGGING(INFO, TRUE, "[RECV-1](%s)", pIoContext->buf);
			pThis->m_parser.AddPacket( pCK->sock, pIoContext->buf, dwIoSize);
			
			pCK->TurnOn_BeingUsed();

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

	char zRecvBuff[__ALPHA::BUF_LEN];
	string sUserId;
	int nLen;
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
			SOCKET sock			= pCK->sock;
			string sUserID		= pCK->sUserID;
			string sClientIp	= pCK->sClientIp;
			pCK->TurnOff_BeingUsed();

			BOOL bContinue;
			do
			{
				ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
				nLen = 0;
				bContinue = pThis->m_parser.GetOnePacket(sock, &nLen, zRecvBuff);
				if (nLen > 0)
				{
					LOGGING(INFO, FALSE, "[RECV-2](%s)", zRecvBuff);

					TPacket* pPacket = new TPacket;
					pPacket->sock = sock;
					pPacket->sUserID = sUserID;
					pPacket->sClientIp = sClientIp;
					pPacket->packet = string(zRecvBuff, nLen);

					char zCode[32] = { 0 };

					CProtoUtils util;
					if ( util.PacketCode((char*)zRecvBuff, zCode) == NULL)
					{
						LOGGING(ERR, TRUE, "No packet code(%.128s)", zRecvBuff);
						delete pPacket;
						continue;
					}

					if (strcmp(zCode, __ALPHA::CODE_LOGON_AUTH) == 0)
					{
						LOGGING(INFO, FALSE, "[LOGINAUTH-1]%d", pPacket->sock);
						pPacket->packet = string(zRecvBuff);

						CLogonAuth* p = new CLogonAuth();
						EnterCriticalSection(&pThis->m_csAuth);
						pThis->m_vecAuth.push_back(p);
						LeaveCriticalSection(&pThis->m_csAuth);
						Sleep(100);
						p->Execute(pPacket);
						
						continue;
					}
				}
				Sleep(0);
			} while (bContinue);
		} // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	} // while(pThis->m_bRun)
	
	
	return 0;
}



unsigned WINAPI CIocp::Thread_Listen(LPVOID lp)
{
	CIocp *pThis = (CIocp*)lp;
	//LOGGING(INFO, FALSE, TEXT("Thread_Listen starts.....[%s][%d]"), g_config.getListenIP(), g_config.getListenPort());

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
				pThis->Delete_AuthCompleted();
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

		char zIp[32];
		strcpy(zIp, inet_ntoa(sinClient.sin_addr));

		//	CK 와 IOCP 연결
		COMPLETION_KEY* pCK = new COMPLETION_KEY;
		pCK->sock		= sockClient;
		pCK->sClientIp = zIp;
		

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

		// PacketParser
		pThis->m_parser.AddSocket(sockClient);

		//char zSocket[32]; CVT_SOCKET(sockClient, zSocket);
		//pThis->lockCK();
		//pThis->m_mapCK[string(zSocket)] = pCK;
		//pThis->unlockCK();

		LOGGING(INFO, TRUE, TEXT("Accept & Add IOCP.[socket:%d][CK:%x]"), sockClient, pCK);

		//	최초 RECV IO 요청
		RequestRecvIO(pCK);

		//pThis->SendMessageToIocpThread(CTX_MT4PING);

	}//while

	return 0;
}


unsigned WINAPI CIocp::Thread_Config(LPVOID lp)
{
	//TODO
	return 0;


	CIocp* p = (CIocp*)lp;

	while (p->m_bRun)
	{
		EN_CNFG_RET ret = g_config.ReLoad_ConfigInfo();
		if (ret == CNFG_ERR)
		{
			LOGGING(ERR, TRUE, "Read cnfg error!!!");
		}
		else if (ret == CNFG_SUCC)
		{
			LOGGING(INFO, TRUE, "Cnfg Updated !");
		}

		Sleep(g_config.getTimeoutReadCnfg() * 1000);

		
	}
	return 0;
}

VOID CIocp::Delete_AuthCompleted()
{
	for (vector<CLogonAuth*>::iterator it=m_vecAuth.begin(); it!=m_vecAuth.end(); ++it )
	{
		if ((*it)->Is_Finished())
		{
			it = m_vecAuth.erase(it);
			continue;
		}
	}
}
