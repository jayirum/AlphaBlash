
#include "IocpThread.h"
#include <process.h>
#include "../CommonAnsi/Util.h"
#include "../CommonAnsi/LogMsg.h"
#include "../CommonAnsi/IRExcept.h"
#include "../Common/AlphaInc.h"
#include "../Common/TimeInterval.h"
#include "Compose_DBSave_String.h"


#include <assert.h>

extern CLogMsg	g_log, g_debug;
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
	m_pDBPool			= NULL;
	m_lIocpThreadIdx = 0;

	m_hThread_Listen = m_hThread_OrderSend = m_hNoMoreOpenThread =  m_hWorkerThread = NULL;
	m_unThread_Listen = m_unThread_OrderSend = m_unNoMoreOpenThread = m_unWorkerThread = 0;

	m_bUse_NoMoreOpenByTime = FALSE;
	m_bUse_MarketCloseClear = FALSE;;
	m_bMarketCloseClearedAlready = FALSE;
	m_bWeekendStartAlready = FALSE;
	m_bTradeClose = FALSE;
	ZeroMemory(m_zLastSnapshotHour, sizeof(m_zLastSnapshotHour));
	ZeroMemory(m_zMarketCloseClearTime, sizeof(m_zMarketCloseClearTime));
}
CIocp::~CIocp()
{
	Finalize();
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

BOOL CIocp::ReadBrokerCount()
{
	char zTemp[1024] = { 0, };
	if (!CUtil::GetConfig(g_zConfig, TEXT("BROKER_COUNT"), TEXT("COUNT"), zTemp))
		return FALSE;
	m_dataHandler.Set_BrokerCntTodo(atoi(zTemp));
	return TRUE;
}

BOOL CIocp::ReadTSApplyYN()
{
	char zTemp[1024] = { 0, };
	if (!CUtil::GetConfig(g_zConfig, TEXT("TS_APPLY"), TEXT("TS_APPLY"), zTemp))
		return FALSE;

	//m_dataHandler.Set_TSAppliedYN(zTemp[0]);
	return TRUE;
}

BOOL CIocp::ReadProfitCutThreshold()
{
	char zTemp[1024] = { 0, };
	if (!CUtil::GetConfig(g_zConfig, TEXT("PROFIT_CUT"), TEXT("THRESHOLD"), zTemp))
		return FALSE;

	m_dataHandler.Set_ProfitCutThreshold(atoi(zTemp));
	return TRUE;
}

BOOL CIocp::ReadTradeCloseTime()
{
	char zTemp[1024] = { 0, };
	if (!CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("TRADE_CLOSE_TIME"), m_zTradeCloseTime))
		return FALSE;

	
	return TRUE;
}

BOOL CIocp::Initialize( )
{
	ReadIPPOrt();

	InitializeCriticalSection(&m_csCK);
	InitializeCriticalSection(&m_csToSend);
	InitializeCriticalSection(&m_csPacket);

	if (!ReadBrokerCount() || !ReadTSApplyYN() || !ReadProfitCutThreshold())
		return FALSE;

	if (!Load_TradingTime())
		return FALSE;

	if (!LoadSymbols())
		return FALSE;

	//DB OPEN
	if (!DBOpen())
		return FALSE;

	if (!RecoverOpenPositions())
		return FALSE;

	if (!InitListen()) {
		return FALSE;
	}

	int nWorkerCnt = 0;
	if (!ReadWorkerThreadCnt(&nWorkerCnt))
		return FALSE;

	unsigned int dwID;
	for (int i = 0; i < nWorkerCnt; i++)
	{
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_Worker, this, 0, &dwID);
		CloseHandle(h);
	}

	m_hThread_Listen = (HANDLE)_beginthreadex(NULL, 0, &Thread_Listen, this, 0, &m_unThread_Listen);
	m_hParsing = (HANDLE)_beginthreadex(NULL, 0, &Thread_Parsing, this, 0, &m_unParsing);
	m_hThread_OrderSend = (HANDLE)_beginthreadex(NULL, 0, &Thread_OrderSend, this, 0, &m_unThread_OrderSend);
	m_hNoMoreOpenThread = (HANDLE)_beginthreadex(NULL, 0, &Thread_MarketTime, this, 0, &m_unNoMoreOpenThread);

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
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_Iocp, this, 0, &dwID);
		CloseHandle(h);
	}

	return TRUE;
}


BOOL CIocp::Load_TradingTime()
{
	char zLossCnt[32] = { 0 };
	char zNoMoreOpenTimeUseYN[32] = { 0 }, zBegin[32] = { 0 }, zEnd[32] = { 0 };
	char zMarketCloseUseYN[32] = { 0 };

	
	//0514-2
	//CUtil::GetConfig(g_zConfig, TEXT("MAX_TRADE_COUNT"), TEXT("LOSS_COUNT"), zLossCnt);
	//if (zLossCnt[0] == NULL)
	//{
	//	LOGGING(ERR, TRUE, TRUE, "Failed to get MAX_TRADE_COUNT count from config file");
	//	return FALSE;
	//}

	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("NO_MORE_OPEN_USE_YN"), zNoMoreOpenTimeUseYN);
	if (zNoMoreOpenTimeUseYN[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get NO_MORE_OPEN_USE_YN count from config file");
		return FALSE;
	}
	m_bUse_NoMoreOpenByTime = (zNoMoreOpenTimeUseYN[0] == 'Y');
	
	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("NO_MORE_OPEN_BEGIN"), zBegin);
	if (zBegin[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get NO_MORE_OPEN_BEGIN count from config file");
		return FALSE;
	}

	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("NO_MORE_OPEN_END"), zEnd);
	if (zEnd[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get NO_MORE_OPEN_END count from config file");
		return FALSE;
	}

	////0524-2
	m_dataHandler.Set_NoMoreOpen_Time(zBegin, zEnd);


	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("MARKET_CLOSE_CLR_USE_YN"), zMarketCloseUseYN);
	if (zMarketCloseUseYN[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get MARKET_CLOSE_CLR_USE_YN count from config file");
		return FALSE;
	}
	m_bUse_MarketCloseClear = (zMarketCloseUseYN[0] == 'Y');
	
	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("MARKET_CLOSE_CLR_TIME"), m_zMarketCloseClearTime);
	if (m_zMarketCloseClearTime[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get MARKET_CLOSE_CLR_TIME count from config file");
		return FALSE;
	}


	// Week close time 
	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("WEEKEND_START_TIME"), m_zWeekendStartTime);
	if (m_zWeekendStartTime[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get WEEKEND_MARKET_CLOSE_TIME count from config file");
		return FALSE;
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

	//vector<string>vec;
	//VOID CUtil::SplitData(_In_ char* psData, _In_ char cDelimeter, _Out_ std::list<std::string> * pListResult)

	
	char zIdx[32];
	int nCnt = atoi(buffer);

	// AlphaBasketFX.ini
	// # symbol / pip size / decimal cnt / active spread / Cost(brokers-spread+comm) / OpenThreshold(exclude nCost) / CloseThreshold(exclude nCost) / TSpt_Open / TSpt_Close / OrderLots 

	for (int i = 0; i < nCnt; i++)
	{
		char zLine[512] = { 0 };
		sprintf(zIdx, "%d", i);
		CUtil::GetConfig(g_zConfig, TEXT("SYMBOLS"), zIdx, zLine);
		vector<string> vec;
		CUtil::SplitData(zLine, '/', &vec);
		m_dataHandler.Add_Symbol(i,
			vec.at(0).c_str(),
			atof(vec.at(1).c_str()),
			atoi(vec.at(2).c_str()),
			atoi(vec.at(3).c_str()),
			atoi(vec.at(4).c_str()),
			atoi(vec.at(5).c_str()),
			atof(vec.at(6).c_str()),
			atoi(vec.at(7).c_str())  // 0514-2
		);
		LOGGING(INFO, TRUE, TRUE, "AddSymbol[%d](%s)(Pipsize:%.5f)(DecimalCnt:%d)(ActiveSpread:%d)"
			"(OpenThreshold:%d)(CloseThreshold:%d)(Lots:%.2f)(NoMoreOpenRoundCnt:%d)",
			i,
			vec.at(0).c_str(),
			atof(vec.at(1).c_str()),
			atoi(vec.at(2).c_str()),
			atoi(vec.at(3).c_str()),
			atoi(vec.at(4).c_str()),
			atoi(vec.at(5).c_str()),
			atof(vec.at(6).c_str()),
			atoi(vec.at(7).c_str())
		);
	}
	return TRUE;
}


BOOL CIocp::DBOpen()
{
	CHAR ip[32], id[32], pwd[32], cnt[32], name[32];
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_IP"), ip);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_ID"), id);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_PWD"), pwd);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_NAME"), name);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_POOL_CNT"), cnt);

	if (!m_pDBPool)
	{
		m_pDBPool = new CDBPoolAdo(ip, id, pwd, name);
		if (!m_pDBPool->Init(atoi(cnt)))
		{
			g_log.log(ERR, TEXT("DB OPEN FAILED.(%s)(%s)(%s)"), ip, id, pwd);
			return 0;
		}
	}
	return TRUE;
}

BOOL CIocp::InitListen()
{
	LOGGING(INFO, TRUE, TRUE, TEXT("CIocp::InitListen() starts.........."));
	CloseListenSock();

	WORD	wVersionRequired;
	WSADATA	wsaData;

	//// WSAStartup
	wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		LOGGING(LOGTP_ERR, TRUE, TRUE, TEXT("WSAStartup Error:%d"), GetLastError());
		return FALSE;
	}

	//DumpWsaData(&wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		LOGGING(LOGTP_ERR, TRUE, TRUE, TEXT("RequiredVersion not Usable"));
		return FALSE;
	}


	// Create a listening socket 
	if ((m_sockListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		LOGGING(ERR,TRUE, TRUE, TEXT("create socket error: %d"), WSAGetLastError());
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
		LOGGING(ERR, TRUE, TRUE, TEXT("bind error (ip:%s) (port:%d) (err:%d)"), m_zListenIP, m_nListenPort, WSAGetLastError());
		return FALSE;
	}
	// Prepare socket for listening 
	if (listen(m_sockListen, 5) == SOCKET_ERROR)
	{
		LOGGING(ERR, TRUE, TRUE, TEXT("listen error: %d"), WSAGetLastError());
		return FALSE;
	}

	m_hListenEvent = WSACreateEvent();
	if (WSAEventSelect(m_sockListen, m_hListenEvent, FD_ACCEPT)) {

		LOGGING(ERR, TRUE, TRUE, TEXT("WSAEventSelect for accept error: %d"), WSAGetLastError());
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
	DeleteCriticalSection(&m_csToSend);
	DeleteCriticalSection(&m_csPacket);

	//SAFE_DELETE(m_pack);
	WSACleanup();

	//delete m_pDBPool;
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

	LOGGING(LOGTP_SUCC, TRUE, FALSE, TEXT("[%d][%d]IOCPThread Start....."), iocpIdx, GetCurrentThreadId());

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
				LOGGING(INFO, TRUE, TRUE, "[Close Client-1]GetQueuedCompletionStatus failed(%d)", pCK->sock);
				pThis->DeleteSocket(pCK);
			}
			Sleep(3000);
			continue;
		}
		
		if (dwIoSize == 0)
		{
			LOGGING(INFO, TRUE, TRUE, "[Close Client-1]dwIoSize == 0 (%d)", pCK->sock);
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

			// Do nothing in the weekend
			if (pThis->m_bWeekendStartAlready)
			{
				delete pIoContext;
				continue;
			}

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
	//char zLogBuff[BUF_LEN];
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

			
			COMPLETION_KEY* pCK = (COMPLETION_KEY*)msg.lParam;

			BOOL bContinue;
			do
			{
				ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
				nRet = 0;
				bContinue = pThis->m_parser.GetOnePacketWithHeaderWithLock(&nRet, zRecvBuff);
				if (nRet > 0)
				{
					InterlockedIncrement(&pCK->refcnt);

					TDeliveryItem* pItem = new TDeliveryItem;
					pItem->ppCK = &pCK;
					strcpy(pItem->packet, zRecvBuff);

					EnterCriticalSection(&pThis->m_csPacket);
					pThis->m_lstPacket.push_back(pItem);
					LeaveCriticalSection(&pThis->m_csPacket);

					InterlockedDecrement(&pCK->refcnt);
				}
				Sleep(0);
			} while (bContinue);
		} // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	} // while(pThis->m_bRun)
	
	
	return 0;
}

unsigned WINAPI CIocp::Thread_Worker(LPVOID lp)
{
	CIocp* p = (CIocp*)lp;

	//CTimeInterval interval;
	//p->m_intevalLoggingCnt = 0;

	//SYSTEMTIME st; char time[32];

	while (p->m_bRun)
	{
		Sleep(1);

		//interval.start();

		EnterCriticalSection(&p->m_csPacket);
		if (!p->m_lstPacket.empty())
		{
			TDeliveryItem* pItem = *p->m_lstPacket.begin();
			p->m_lstPacket.pop_front();
			LeaveCriticalSection(&p->m_csPacket);
			
			p->DispatchData(*pItem->ppCK, pItem->packet, strlen(pItem->packet));

			delete pItem;
		}
		else
		{
			LeaveCriticalSection(&p->m_csPacket);
		}
		
		//interval.lapse();
		//if (++p->m_intevalLoggingCnt > 1000000) {
		//	LOGGING(INFO, FALSE, FALSE, "[TIME LAPSE]%d ms", interval.interval_ms());
		//	p->m_intevalLoggingCnt = 0;
		//}
	}
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
	char zCode[32] = { 0 };

	CProtoGet protoGet;
	if (CProtoUtils::PacketCode((char*)pRecvData, zCode) == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "No packet code(%.128s)", pRecvData);
		return;
	}

	if (strcmp(zCode, __ALPHA::CODE_MARKET_DATA) != 0)
		LOGGING(INFO, FALSE, FALSE, "[RECV](%s)", pRecvData);

	if (strcmp(zCode, __ALPHA::CODE_LOGON) == 0)
	{
		Logon_Process(pCK->sock, pRecvData, nRecvLen);
	}
	if (strcmp(zCode, __ALPHA::CODE_MARKET_DATA) == 0)
	{
		if (!m_bTradeClose)
		{
			if (!m_dataHandler.Is_ProfitCut_AlreadyTriggered())
				MD_Process(pRecvData, nRecvLen);
		}
	}
	if (strcmp(zCode, __ALPHA::CODE_ORDER_OPEN) == 0)
	{
		OrderReceive_Open(pRecvData, nRecvLen);
	}
	if (strcmp(zCode, __ALPHA::CODE_ORDER_CLOSE) == 0)
	{
		OrderReceive_Close( pRecvData, nRecvLen);
	}
	//if (strcmp(zCode, __ALPHA::CODE_OPEN_ORDERS) == 0)
	//{
	//	OpenPositions_Process(pCK, pRecvData, nRecvLen);
	//}

}


BOOL CIocp::TradeCloseTimeCheck()
{
	if (!m_bTradeClose)
	{
		char zNowTime[32] = { 0 };
		NOWTIME_MIN(zNowTime);
		if (strcmp(m_zTradeCloseTime, zNowTime) == 0)
		{
			m_bTradeClose = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

//
///*
//FDS_KEY
//FDS_ARRAY_DATA
//>> FDS_SYMBOL
//>> FDS_SYMBOL_IDX
//>> FDS_MT4_TICKET
//>> FDD_OPEN_PRC
//>>FDD_LOTS
//>>FDN_MAGIC_NO
//>>FDS_OPEN_TM
//*/
//VOID CIocp::OpenPositions_Process(COMPLETION_KEY* pCK, const char* pRecvData, int nDataLen)
//{
//	CProtoOpenPositions get;
//	TOpenPos* pPos;
//	char zBrokerKey[32] = { 0 };
//	int nCnt = get.GetPosInfo((char*)pRecvData, nDataLen, zBrokerKey, pPos);
//
//	for (int i = 0; i < nCnt; i++)
//	{
//
//	}
//}

VOID CIocp::OrderReceive_Close(const char* pRecvData, int nDataLen)
{
	CProtoGet get;
	if (!get.ParsingWithHeader(pRecvData, nDataLen))
	{
		LOGGING(ERR, TRUE, TRUE, "(%s)(%s)", get.GetMsg(), pRecvData);
		return;
	}


	char zErrYN		[32] = { 0 };
	char zErrMsg	[512] = { 0 };
	char zBrokerKey	[128] = { 0 };// , zSymbol[128] = { 0 };
	char zTicketNo	[128] = { 0 };
	char zMT4Time	[128] = { 0 };
	int iSymbol = 0, nCmd = 0;
	double dClosePrc = 0, dLots = 0, dPL = 0, dCmsn=0, dSwap=0;

	get.GetVal(FDS_ERR_YN, zErrYN); 
	get.GetVal(FDS_KEY, zBrokerKey);
	get.GetVal(FDS_MT4_TICKET, zTicketNo);
	get.GetVal(FDS_CLOSE_TM, zMT4Time);
	
	dClosePrc = get.GetValD(FDD_CLOSE_PRC);
	dLots	= get.GetValD(FDD_LOTS);
	dCmsn	= get.GetValD(FDD_CMSN);
	dPL		= get.GetValD(FDD_PROFIT);
	iSymbol	= get.GetValN(FDN_SYMBOL_IDX);
	nCmd	= get.GetValN(FDN_ORDER_CMD);
	dSwap	= get.GetValD(FDD_SWAP);

	m_dataHandler.LockData(iSymbol);

	if (CProtoUtils::IsSuccess(zErrYN[0]))
	{
		char zMsg[256] = { 0 };
		m_dataHandler.Update_ResponseOfCloseOrder(iSymbol, zTicketNo, dClosePrc, dLots, nCmd, zMT4Time, dCmsn, dPL, dSwap, zMsg);
		SaveToDB(iSymbol, !__ALPHA::IsBuyOrder(nCmd), "");

		if (m_dataHandler.Is_OrdStatus_BothClosed(iSymbol))
		{
			// reset data to continue trading
			m_dataHandler.ResetData(iSymbol);

			//0514-2
			//// Set No More Open 
			//if (m_dataHandler.Check_Set_SymbolNoMoreOpen_by_LossCnt(iSymbol, zErrMsg))
			//{
			//	SaveLogToDB("NoMoreOpen_ByLossCnt", zErrMsg);
			//}
			//else
			//{
			//	// reset data to continue trading
			//	m_dataHandler.ResetData(iSymbol);
			//}
		}
	}
	else
	{
		get.GetVal(FDS_ERR_MSG, zErrMsg);
		SaveToDB(iSymbol, !__ALPHA::IsBuyOrder(nCmd), zErrMsg, FALSE);
		sprintf(m_zMsg, "(%s) rejected the close order(Ticket:%s)", zBrokerKey, zTicketNo);
		g_log.SendAlermBoth(m_zMsg);
	}
	m_dataHandler.UnlockData(iSymbol);
}

VOID CIocp::OrderReceive_Open(const char* pRecvData, int nDataLen)
{
	CProtoGet get;
	if (!get.ParsingWithHeader(pRecvData, nDataLen))
	{
		LOGGING(ERR, TRUE, TRUE, "(%s)(%s)", get.GetMsg(), pRecvData);
		return;
	}


	char zErrYN		[32] = { 0 };
	char zErrMsg	[512] = { 0 };
	char zBrokerKey	[128] = { 0 };// , zSymbol[128] = { 0 };
	char zTicketNo	[128] = { 0 };
	char zMT4Time	[128] = { 0 };
	int iSymbol=0, nCmd=0;
	double dOpenPrc = 0, dLots = 0;
	
	get.GetVal(FDS_ERR_YN, zErrYN);
	get.GetVal(FDS_KEY, zBrokerKey); 
	get.GetVal(FDS_ERR_MSG, zErrMsg);
	get.GetVal(FDS_MT4_TICKET, zTicketNo);
	get.GetVal(FDS_OPEN_TM, zMT4Time);

	nCmd		= get.GetValN(FDN_ORDER_CMD);
	iSymbol		= get.GetValN(FDN_SYMBOL_IDX);
	dOpenPrc	= get.GetValD(FDD_OPEN_PRC);
	dLots		= get.GetValD(FDD_LOTS);

	m_dataHandler.LockData(iSymbol);

	if (CProtoUtils::IsSuccess(zErrYN[0]))
	{
		// Hedge position order was rejected. Close this position immediately.
		if (m_dataHandler.Is_HedgePositionRejected(iSymbol, nCmd))
		{
			char zRejectBuySell[32];
			char zRejectBroker[32];
			char zMyBuySell[32];

			// change status
			m_dataHandler.Data(iSymbol)->nOrdStatus = ORDSTATUS_CLOSE_MT4_1;

			// if my cmd is buy
			if (__ALPHA::IsBuyOrder(nCmd)) {
				strcpy(zMyBuySell, "BUY");
				strcpy(zRejectBuySell, "SELL");
				strcpy(zRejectBroker, m_dataHandler.Data(iSymbol)->Short.zBrokerKey);

				// close my position
				SendCloseOrder(iSymbol, __ALPHA::SELL_SIDE);
			}
			else {
				strcpy(zMyBuySell, "SELL");
				strcpy(zRejectBuySell, "BUY");
				strcpy(zRejectBroker, m_dataHandler.Data(iSymbol)->Long.zBrokerKey);
				
				// close my LONG position
				if (m_dataHandler.Data(iSymbol)->Long.zTicket[0] != NULL)
					SendCloseOrder(iSymbol, __ALPHA::BUY_SIDE);
			}
			
			//LOGGING
			sprintf(m_zMsg, "[REJECTED-1.1]OpenOrder Response OK.But HedgeOrder already rejected.So close this order.(iSymbol:%d)", iSymbol);
			LOGGING(INFO, TRUE, TRUE, m_zMsg);

			sprintf(m_zMsg, "[REJECTED-1.2](MyBroker:%5.5s)(MySide:%s)(MyTicket:%s)(RejectBroker:%5.5s)(RejectSide:%s)",
				zBrokerKey, zMyBuySell, zTicketNo, zRejectBroker, zRejectBuySell);
			LOGGING(INFO, TRUE, TRUE, m_zMsg);

		}
		else
		{
			m_dataHandler.Update_ResponseOfOpenOrder(iSymbol, zTicketNo, dOpenPrc, dLots, nCmd, zMT4Time);
			SaveToDB(iSymbol, __ALPHA::IsBuyOrder(nCmd), "");
		}
	}
	else // if (CProtoUtils::IsSuccess(zErrYN[0]))
	{
		m_dataHandler.Mark_Rejected(iSymbol, nCmd);
		SaveToDB(iSymbol, __ALPHA::IsBuyOrder(nCmd), zErrMsg, FALSE);

		char zRejectBuySell[32], zHedgeBuySell[32];
		char zHedgeBroker[32];
		char zHedgeTicket[64];

		if (__ALPHA::IsBuyOrder(nCmd)) {
			strcpy(zRejectBuySell, "BUY");
			strcpy(zHedgeBuySell, "SELL");
			strcpy(zHedgeBroker, m_dataHandler.Data(iSymbol)->Short.zBrokerKey);
			strcpy(zHedgeTicket, m_dataHandler.Data(iSymbol)->Short.zTicket);

			// close hedge position if it exists
			if (m_dataHandler.Data(iSymbol)->Short.zTicket[0] != NULL) {
				// change status
				m_dataHandler.Data(iSymbol)->nOrdStatus = ORDSTATUS_CLOSE_MT4_1;
				SendCloseOrder(iSymbol, __ALPHA::BUY_SIDE);
			}
		}
		else {
			strcpy(zRejectBuySell, "SELL");
			strcpy(zHedgeBuySell, "BUY");
			strcpy(zHedgeBroker, m_dataHandler.Data(iSymbol)->Long.zBrokerKey);
			strcpy(zHedgeTicket, m_dataHandler.Data(iSymbol)->Long.zTicket);

			// close hedge position if it exists
			if (m_dataHandler.Data(iSymbol)->Long.zTicket[0] != NULL) {// change status
				m_dataHandler.Data(iSymbol)->nOrdStatus = ORDSTATUS_CLOSE_MT4_1;
				SendCloseOrder(iSymbol, __ALPHA::SELL_SIDE);
			}
		}

		//LOGGING
		sprintf(m_zMsg, "[REJECTED-2.1]Receive OpenOrder Reject.So must close HedgeOrder if exist.(iSymbol:%d)(%s)",iSymbol, zErrMsg);
		LOGGING(INFO, TRUE, TRUE, m_zMsg);

		sprintf(m_zMsg, "[REJECTED-2.2](Reject:%5.5s)(Reject:%s)(Hedge:%5.5s)(Hedge:%s)(Hedge Ticket:%s)",
			zBrokerKey, zRejectBuySell, zHedgeBroker, zHedgeBuySell, zHedgeTicket);
		LOGGING(INFO, TRUE, TRUE, m_zMsg);

		sprintf(m_zMsg, "(%s) rejected the open order(Ticket:%s)", zBrokerKey, zTicketNo);
		g_log.SendAlermBoth(m_zMsg);

	}

	m_dataHandler.UnlockData(iSymbol);

}

VOID CIocp::MD_Process(const char* pRecvData, int nDataLen)
{
	
	CProtoGet get;
	if (!get.ParsingWithHeader(pRecvData, nDataLen))
	{
		LOGGING(ERR, TRUE, TRUE, "ParsingWithHeader Error(%s)(size:%d)(%s)", get.GetMsg(), nDataLen, pRecvData);
		//return;
	}

	char zBrokerKey[128] = { 0 };// , zSymbol[128] = { 0 };
	char zMDTime[32] = { 0 };
	double dBid, dAsk, dSpread;
	int iSymbol;

	get.GetVal(FDS_KEY, zBrokerKey);
	//get.GetVal(FDS_SYMBOL, zSymbol);
	dBid = get.GetValD(FDD_BID);
	dAsk = get.GetValD(FDD_ASK);
	dSpread = get.GetValD(FDD_SPREAD);
	iSymbol = get.GetValN(FDN_SYMBOL_IDX);
	get.GetVal(FDS_MARKETDATA_TIME, zMDTime);

	if (dBid <= 0 || dAsk <= 0)
		return;

	if (m_dataHandler.Get_SymbolCntTodo() <= iSymbol)
	{
		return;
	}

	//m_dataHandler.LockData(iSymbol);

	//if (strcmp(zBrokerKey, "BLUEBERRY") == 0)
	//	int a = 0;

	//
	m_dataHandler.Update_LatestMarketData(iSymbol, zBrokerKey, dBid, dAsk, dSpread, zMDTime);
	//

	m_dataHandler.LockData(iSymbol);

	// Best price & Check_Send Open Order
	if (!m_dataHandler.Is_AlreadOpened(iSymbol))
	{	
		//if (!m_dataHandler.Is_NoMoreOpenByTime() && !m_dataHandler.Is_NoMoreOpenByRoundCnt(iSymbol))
		//{
		//	BOOL bMustSendOrder = FALSE;
		//	m_dataHandler.Calc_BestPrc_OpenOrd(iSymbol, /*zSymbol,*/ dBid, dAsk, dSpread, zBrokerKey, "", &bMustSendOrder);
		//	if (bMustSendOrder)
		//	{
		//		SendOpenOrder(iSymbol);
		//		m_dataHandler.Update_OrdStatus_OpenTriggered(iSymbol);
		//		SaveToDB(iSymbol, 0, "");
		//	}
		//}

		if (!m_dataHandler.Is_NoMoreOpenByTime() )
		{
			BOOL bMustSendOrder = FALSE;
			m_dataHandler.Calc_BestPrc_OpenOrd(iSymbol, /*zSymbol,*/ dBid, dAsk, dSpread, zBrokerKey, "", &bMustSendOrder);
			if (bMustSendOrder)
			{
				if (!m_dataHandler.Is_NoMoreOpenByRoundCnt(iSymbol))
				{
					LOGGING(INFO, TRUE, FALSE, "[OPEN MD TIME](iSymbol:%d)(%s)(%s)(bid:%.5f)(ask:%.5f)", iSymbol, zBrokerKey, zMDTime, dBid, dAsk);
					SendOpenOrder(iSymbol);
					m_dataHandler.Update_OrdStatus_OpenTriggered(iSymbol);
					SaveToDB(iSymbol, 0, "");
				}
				else
				{
					LOGGING(INFO, TRUE, TRUE, "Do NOT Open even BestPrice. NoMoreOpenRoundCnt(%d), CurrRoundCnt(%d)",
						m_dataHandler.Data(iSymbol)->Spec.nNoMoreOpenRoundCnt, m_dataHandler.Data(iSymbol)->nRoundCnt);
				}
			}
		}
	}
	// Has opened positions for this symbol?
	else //if (m_dataHandler.Is_AlreadOpened(iSymbol))
	{
		//calculate P&L and send close order if condition is met
		BOOL bMustSendOrder = FALSE;
		m_dataHandler.Calc_EstmPL_CloseOrder(iSymbol, /*zSymbol,*/ dBid, dAsk, zBrokerKey, FALSE, &bMustSendOrder);
		if (bMustSendOrder) 
		{
			LOGGING(INFO, TRUE, FALSE, "[CLOSE MD TIME](iSymbol:%d)(%s)(%s)(bid:%.5f)(ask:%.5f)", iSymbol, zBrokerKey, zMDTime, dBid, dAsk);
			SendCloseOrder(iSymbol, __ALPHA::BUY_SIDE);
			SendCloseOrder(iSymbol, __ALPHA::SELL_SIDE);
			m_dataHandler.Update_OrdStatus_CloseTriggered(iSymbol);
			SaveToDB(iSymbol, 0, "");
		}
	}
	m_dataHandler.UnlockData(iSymbol);
}


void CIocp::SendOpenOrder(int iSymbol)
{
	char zTemp[128] = { 0 };
	char zSendBuff[MAX_BUF] = { 0 };

	TData* pData = m_dataHandler.Data(iSymbol);

	CProtoSet set;
	set.Begin();
	set.SetVal(FDS_CODE,		__ALPHA::CODE_ORDER_OPEN);
	set.SetVal(FDN_SYMBOL_IDX,	iSymbol);
	set.SetVal(FDS_SYMBOL,		pData->symbol);
	set.SetVal(FDN_ORDER_CMD,	__ALPHA::getMT4Cmd_MarketBuy());
	set.SetVal(FDD_LOTS,		pData->Spec.dOrderLots);
	set.SetVal(FDS_MT4_TICKET,	0);
	set.SetVal(FDN_MAGIC_NO,	pData->lMagicNo);	

	TSendOrder* pSendBuy = new TSendOrder;
	ZeroMemory(pSendBuy->zSendBuf, sizeof(pSendBuy->zSendBuf));
	strcpy(pSendBuy->brokerKey, pData->Long.zBrokerKey);

	int nLen = set.Complete(pSendBuy->zSendBuf);
	PostThreadMessage(m_unThread_OrderSend, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pSendBuy);

	sprintf(m_zMsg, "[SendBuy_Open](%5.5s)(%s)(IDX:%d)(%s)",
		pData->Long.zBrokerKey, pData->symbol, iSymbol, __ALPHA::getMT4CmdDesc(__ALPHA::getMT4Cmd_MarketBuy(), zTemp));
	LOGGING(INFO, TRUE, TRUE, m_zMsg);

	//
	//
	//

	CProtoSet set2;
	set2.Begin();
	set2.SetVal(FDS_CODE,		__ALPHA::CODE_ORDER_OPEN);
	set2.SetVal(FDN_SYMBOL_IDX, iSymbol);
	set2.SetVal(FDS_SYMBOL,		pData->symbol);
	set2.SetVal(FDN_ORDER_CMD,	__ALPHA::getMT4Cmd_MarketSell());
	set2.SetVal(FDD_LOTS,		pData->Spec.dOrderLots);
	set2.SetVal(FDS_MT4_TICKET, 0);

	TSendOrder* pSendSell = new TSendOrder;
	ZeroMemory(pSendSell->zSendBuf, sizeof(pSendSell->zSendBuf));
	strcpy(pSendSell->brokerKey, pData->Short.zBrokerKey);

	nLen = set2.Complete(pSendSell->zSendBuf);
	PostThreadMessage(m_unThread_OrderSend, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pSendSell);

	sprintf(m_zMsg, "[SendSell_Open](%5.5s)(%s)(IDX:%d)(%s)",
		pData->Short.zBrokerKey, pData->symbol, iSymbol, __ALPHA::getMT4CmdDesc(__ALPHA::getMT4Cmd_MarketSell(), zTemp));
	LOGGING(INFO, TRUE, TRUE, m_zMsg);
}



void CIocp::SendCloseOrder(int iSymbol, char cOrderSide)
{
	char zTemp[128] = { 0 };
	char zHead[128] = { 0 };
	char zSendBuff[MAX_BUF] = { 0 };

	TData* pData = m_dataHandler.Data(iSymbol);
	TSendOrder* pSendOrder = new TSendOrder;
	ZeroMemory(pSendOrder->zSendBuf, sizeof(pSendOrder->zSendBuf));

	int nLen = 0;

	CProtoSet set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_ORDER_CLOSE);
	set.SetVal(FDN_SYMBOL_IDX,	iSymbol);
	set.SetVal(FDS_SYMBOL,		pData->symbol);

	// Buy Order - close short position
	if (cOrderSide == __ALPHA::BUY_SIDE)
	{
		set.SetVal(FDN_ORDER_CMD, __ALPHA::getMT4Cmd_MarketBuy());

		if (pData->Short.dLots <= 0)
			set.SetVal(FDD_LOTS, pData->Spec.dOrderLots);
		else
			set.SetVal(FDD_LOTS, pData->Short.dLots);

		set.SetVal(FDS_MT4_TICKET, pData->Short.zTicket);

		strcpy(pSendOrder->brokerKey, pData->Short.zBrokerKey);

		nLen = set.Complete(pSendOrder->zSendBuf);

		sprintf(m_zMsg, "[SendBuy_CloseShort](%5.5s)(%s)(IDX:%d)(%s)(Ticket:%s)",
			pData->Short.zBrokerKey, pData->symbol, iSymbol, __ALPHA::getMT4CmdDesc(__ALPHA::getMT4Cmd_MarketBuy(), zTemp), pData->Short.zTicket);
	}
	else
	{
		set.SetVal(FDN_ORDER_CMD, __ALPHA::getMT4Cmd_MarketSell());

		if (pData->Long.dLots <= 0)
			set.SetVal(FDD_LOTS, pData->Spec.dOrderLots);
		else
			set.SetVal(FDD_LOTS, pData->Long.dLots);

		set.SetVal(FDS_MT4_TICKET, pData->Long.zTicket);

		strcpy(pSendOrder->brokerKey, pData->Long.zBrokerKey);

		nLen = set.Complete(pSendOrder->zSendBuf);

		sprintf(m_zMsg, "[SendSell_CloseLong](%5.5s)(%s)(IDX:%d)(%s)(Ticket:%s)",
			pData->Long.zBrokerKey, pData->symbol, iSymbol, __ALPHA::getMT4CmdDesc(__ALPHA::getMT4Cmd_MarketSell(), zTemp), pData->Long.zTicket);

	}

	LOGGING(INFO, TRUE, TRUE, m_zMsg);
	LOGGING(INFO, TRUE, FALSE, pSendOrder->zSendBuf);

	PostThreadMessage(m_unThread_OrderSend, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pSendOrder);
}


void CIocp::AddList_ClientRecvSock(char* pzBrokerKey, SOCKET sock)
{
	string sBroker = string(pzBrokerKey);
	EnterCriticalSection(&m_csToSend);
	m_mapToSend[sBroker] = sock;
	LeaveCriticalSection(&m_csToSend);
}

/*
	BrokerKey
	BrokerName
*/
BOOL CIocp::Logon_Process(SOCKET sock, const char* pLoginData, int nDataLen)
{
	CProtoGet get;
	if (!get.ParsingWithHeader(pLoginData, nDataLen))
	{
		LOGGING(ERR, TRUE, TRUE, "(%s)(%s)", get.GetMsg(), pLoginData);
		return FALSE;
	}
	int res = 0;
	char zBrokerKey[128] = { 0 }, zBrokerName[128] = { 0 };
	char zClientSockTp[32] = { 0 };

	res = get.GetVal(FDS_KEY, zBrokerKey);
	res += get.GetVal(FDS_BROKER, zBrokerName);
	res += get.GetVal(FDS_CLIENT_SOCKET_TP, zClientSockTp);

	LOGGING(INFO, TRUE, TRUE, "[LOGIN](%5.5s)(%c)(Socket:%d)", zBrokerKey, zClientSockTp[0], sock);

	if (res < 3)
	{
		LOGGING(ERR,TRUE, TRUE, "Failed to get BrokerKey, BrokerName from Logon Packet");
		return FALSE;
	}

	// Only client receiving socket is added ==> To transfer data to client
	if (zClientSockTp[0] == 'R')
	{
		AddList_ClientRecvSock(zBrokerKey, sock);
		m_dataHandler.Add_BrokerWhenLogin(zBrokerKey, zBrokerName);


		char zSendBuff[MAX_BUF] = { 0 };
		string sSymbolArray;
		char zSymbolArray[512] = { 0 };
		CProtoSet set;
		set.Begin();
		set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON);
		set.SetVal(FDN_ARRAY_SIZE, m_dataHandler.Get_SymbolCntCurrent());

		for (int k = 0; k < m_dataHandler.Get_SymbolCntCurrent(); k++)
		{
			sprintf(zSymbolArray, "%d=%s%c%d=%d%c",
				FDS_SYMBOL, m_dataHandler.Data(k)->symbol, DEF_DELI_COLUMN
				, FDN_SYMBOL_IDX, k, DEF_DELI_ARRAY
			);
			sSymbolArray += zSymbolArray;

			ZeroMemory(zSymbolArray, sizeof(zSymbolArray));
		}
		set.SetVal(FDS_ARRAY_DATA, sSymbolArray);
		int nLen = set.Complete(zSendBuff);
		RequestSendIO(sock, zSendBuff, nLen);

		nDebug = 0;
	}
	return TRUE;
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

	LOGGING(INFO, TRUE, TRUE, "[Return Error to Client](%s)", zSendBuff);
	RequestSendIO(pCK->sock, zSendBuff, nLen);
}





unsigned WINAPI CIocp::Thread_Listen(LPVOID lp)
{
	CIocp *pThis = (CIocp*)lp;
	LOGGING(INFO, TRUE, FALSE, TEXT("Thread_Listen starts.....[%s][%d]"), pThis->m_zListenIP, pThis->m_nListenPort);

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
			LOGGING(ERR, TRUE, TRUE, TEXT("accept error:%d"), nErr);
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
			LOGGING(ERR, TRUE, TRUE, TEXT("setsockopt error : %d"), WSAGetLastError);
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

		LOGGING(INFO, FALSE, FALSE, TEXT("Accept & Add IOCP.[socket:%d][CK:%x]"), sockClient, pCK);

		//	최초 RECV IO 요청
		pThis->RequestRecvIO(pCK);

		//pThis->SendMessageToIocpThread(CTX_MT4PING);

	}//while

	return 0;
}




unsigned WINAPI CIocp::Thread_OrderSend(LPVOID lp)
{
	CIocp* p = (CIocp*)lp;

	while (p->m_bRun)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			BOOL bDelete = TRUE;
			if (msg.message == WM_ORDER_SEND)
			{
				TSendOrder* pOrd = (TSendOrder*)msg.lParam;
				string sBrokerKey = pOrd->brokerKey;
				EnterCriticalSection(&p->m_csToSend);
				map<string, SOCKET>::iterator it = p->m_mapToSend.find(sBrokerKey);
				SOCKET sock;
				if (it != p->m_mapToSend.end())
				{
					sock = (*it).second;
				}
				LeaveCriticalSection(&p->m_csToSend);
				
				p->RequestSendIO(sock, pOrd->zSendBuf, (int)msg.wParam);

				delete (TSendOrder*)(msg.lParam);
			}
		}

	}
	return 0;
}


BOOL CIocp::Weekend_Check_Set()
{
	int nDayOfWeek = CUtil::Get_WeekDay();

	if (nDayOfWeek == EN_SUNDAY)
	{
		m_bWeekendStartAlready = TRUE;
	}
	else if (nDayOfWeek == EN_SATURDAY )
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		char zNow[32];
		sprintf(zNow, "%02d:%02d", st.wHour, st.wMinute);
		if (strcmp(m_zWeekendStartTime, zNow) <= 0)
		{
			//
			m_bWeekendStartAlready = TRUE;
			//
		}
	}
	else
	{
		m_bWeekendStartAlready = FALSE;
	}

	return m_bWeekendStartAlready;
}

unsigned WINAPI CIocp::Thread_MarketTime(LPVOID lp)
{
	CIocp* p = (CIocp*)lp;
	char zMsg[1024];
	while (p->m_bRun)
	{
		Sleep(500);
		ZeroMemory(zMsg, sizeof(zMsg));

		p->Load_TradingTime();

		if (p->Weekend_Check_Set())
			continue;

		if (p->m_bUse_NoMoreOpenByTime)
		{
			// Set No More Open
			if (p->m_dataHandler.Check_Set_NoMoreOpen_by_Time(zMsg))
			{
				p->SaveLogToDB("NoMoreOpen_ByTime", zMsg);
			}

		}
		
		if (p->m_dataHandler.Check_ResumeTrade_by_Time(zMsg))
		{
			p->m_bMarketCloseClearedAlready = FALSE;
			p->m_bTradeClose = FALSE;
			p->SaveLogToDB("ResumeTrade", zMsg);
		}

		if (p->m_bUse_MarketCloseClear) 
		{
			p->MarketClose_Check_Order();
		}

		//p->Snapshot();
		if (!p->m_dataHandler.Is_ProfitCut_AlreadyTriggered())
		{
			if (p->m_dataHandler.Is_Condition_ProfitCut())
			{
				p->CloseAllOpenPositions("ProfitCut");
				g_log.SendAlermTelegram("ProfitCut done");
			}
		}

		p->TradeCloseTimeCheck();	
	}
	
	return 0;
}


/*
create PROCEDURE AlphaBasket_CatchSnapshot
	@I_SnapshotHour	char(5)
	,@I_iSymbol	int
	,@I_B_bidPrc	varchar(20)
	,@I_B_pipSize	float
	,@I_S_askPrc	varchar(20)
	,@I_S_pipSize	float
*/
void CIocp::Snapshot()
{
	SYSTEMTIME st;
	char zNow[6];
	GetLocalTime(&st);
	sprintf(zNow, "%02d:%02d", st.wHour, st.wMinute);	// hh:mm
	if (strcmp(zNow+3, "00") != 0)	// mm must be 00
		return;

	BOOL bRun = FALSE;
	if (m_zLastSnapshotHour[0] == 0x00)		
		bRun = TRUE;
	else
	{
		if (strcmp(zNow, m_zLastSnapshotHour) != 0)
			bRun = TRUE;
	}
	if (!bRun)
		return;

	//
	strcpy(m_zLastSnapshotHour, zNow);
	//

	char zQ[128];
	for (int iSymbol = 0; iSymbol < m_dataHandler.Get_SymbolCntCurrent(); iSymbol++)
	{
		char zB_Bid[32] = { 0 }, zB_Ask[32] = { 0 };
		char zS_Bid[32] = { 0 }, zS_Ask[32] = { 0 };
		char zLongLastMDTime[32] = { 0 }, zShortLastMDTime[32] = { 0 };

		m_dataHandler.LockData(iSymbol);
		m_dataHandler.Calc(iSymbol)->GetLastBidAsk(m_dataHandler.Data(iSymbol)->Long.zBrokerKey, zB_Bid, zB_Ask, zLongLastMDTime);
		m_dataHandler.Calc(iSymbol)->GetLastBidAsk(m_dataHandler.Data(iSymbol)->Short.zBrokerKey, zS_Bid, zS_Ask, zShortLastMDTime);
		
		
		sprintf(zQ,
			"AlphaBasket_CatchSnapshot "
			" '%s'"	//@I_SnapshotHour	char(5)
			",%d"	//, @I_iSymbol	int
			",'%s'"	//, @I_B_bidPrc	varchar(20)
			",'%s'"	//, @I_S_askPrc	varchar(20)
			",%f"	//, @I_pipSize	float
			,
			m_zLastSnapshotHour
			, iSymbol
			, zB_Bid
			, zS_Ask
			, m_dataHandler.Data(iSymbol)->Spec.dPipSize
		);
			
		CDBHandlerAdo db(m_pDBPool->Get());
		if (!db->ExecQuery(zQ))
		{
			LOGGING(ERR, TRUE, TRUE, "(%s)(%s)", zQ, db->GetError());
		}
		else
			LOGGING(INFO, TRUE, TRUE, "(%s)", zQ);

		m_dataHandler.UnlockData(iSymbol);
	}

}

void	CIocp::MarketClose_Check_Order()
{
	if (m_bMarketCloseClearedAlready)
		return;

	SYSTEMTIME st; char zNow[32];
	GetLocalTime(&st);
	sprintf(zNow, "%02d:%02d", st.wHour, st.wMinute);
	if (strcmp(zNow, m_zMarketCloseClearTime) != 0)
		return;

	CloseAllOpenPositions("MarketClose");

	if (m_bMarketCloseClearedAlready) {
		char zMsg[1024];
		SaveLogToDB("MarketClose", zMsg);
	}
}

void CIocp::CloseAllOpenPositions(char* pzCloseType)
{
	for (int iSymbol = 0; iSymbol < m_dataHandler.Get_SymbolCntCurrent(); iSymbol++)
	{
		m_bMarketCloseClearedAlready = TRUE;

		if (m_dataHandler.Data(iSymbol)->nOrdStatus >= ORDSTATUS_CLOSE_TRIGGERED)
			continue;

		BOOL bMustSendOrd = FALSE;
		m_dataHandler.MarketClose(iSymbol, &bMustSendOrd);
		if (bMustSendOrd)
		{
			SendCloseOrder(iSymbol, __ALPHA::BUY_SIDE);
			SendCloseOrder(iSymbol, __ALPHA::SELL_SIDE);
			m_dataHandler.Update_OrdStatus_CloseTriggered(iSymbol);
			SaveToDB(iSymbol, 0, "", TRUE);
			LOGGING(INFO, TRUE, TRUE, "[%s-Buy-CloseShort](%s)(%5.5s)(Ticket:%s)"
				, pzCloseType
				, m_dataHandler.Data(iSymbol)->symbol
				, m_dataHandler.Data(iSymbol)->Long.zBrokerKey
				, m_dataHandler.Data(iSymbol)->Long.zTicket
			);
			LOGGING(INFO, TRUE, TRUE, "[%s-Sell-CloseLong](%s)(%5.5s)(Ticket:%s)"
				, pzCloseType
				, m_dataHandler.Data(iSymbol)->symbol
				, m_dataHandler.Data(iSymbol)->Short.zBrokerKey
				, m_dataHandler.Data(iSymbol)->Short.zTicket
			);
		}
	}
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
				LOGGING(LOGTP_ERR, TRUE, TRUE, TEXT("WSARecv error : %d"), WSAGetLastError());
				bRet = FALSE;
			}
		}
	}
	catch (...) {
		LOGGING(LOGTP_ERR, TRUE, TRUE, TEXT("WSASend TRY CATCH"));
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
				LOGGING(LOGTP_ERR, TRUE, TRUE, TEXT("WSASend error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		LOGGING(ERR, TRUE, TRUE, TEXT("WSASend try catch error [CIocp]"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;
	else
		LOGGING(INFO, FALSE, FALSE, "[SEND](sock:%d)(%s)",sock, pSendBuf);
	return;
}



BOOL CIocp::RecoverOpenPositions()
{
	char zMsg[1024];
	char zQ[1024];
	char zStatus[32];
	sprintf(zQ, "AlphaBasket_Recovery ");

	CDBHandlerAdo db(m_pDBPool->Get());
	if (!db->ExecQuery(zQ))
	{
		LOGGING(ERR, TRUE, TRUE, "Recover Failed:%s",db->GetError());
		return FALSE;
	}

	while (db->IsNextRow())
	{
		int iSymbol = db->GetLong("ISymbol");
		if (iSymbol < 0)
		{
			LOGGING(ERR, TRUE, TRUE, "Recover Failed:Wrong iSymbol");
			return FALSE;
		}

		if (iSymbol >= m_dataHandler.SymbolCount())
			continue;

		char z[256];
		strcpy(m_dataHandler.Data(iSymbol)->symbol, db->GetStr("Symbol", z));
		m_dataHandler.Data(iSymbol)->nOrdStatus = db->GetLong("ORD_STATUS");
		db->GetStr("ORD_STATUS_DESC", zStatus);
		strcpy(m_dataHandler.Data(iSymbol)->zDBSerial, db->GetStr("SERIAL_NO", z));

		m_dataHandler.Data(iSymbol)->nCloseNetPLTriggered = 0;
		m_dataHandler.Data(iSymbol)->nProfitCnt = 0;
		m_dataHandler.Data(iSymbol)->nLossCnt = 0;
		//0514-2 m_dataHandler.Data(iSymbol)->bNoMoreOpenByLossCnt = FALSE;
		m_dataHandler.Data(iSymbol)->lMagicNo = db->GetLong("MagicNo");

		m_dataHandler.Data(iSymbol)->Long.bRejected = FALSE;
		ZeroMemory(z, sizeof(z)); db->GetStr("B_BrokerKey", z); if(strlen(z)>0) strcpy(m_dataHandler.Data(iSymbol)->Long.zBrokerKey, z);
		ZeroMemory(z, sizeof(z)); db->GetStr("B_OpenPrc", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zOpenPrc, z);
		ZeroMemory(z, sizeof(z)); db->GetStr("B_OpenPrcTriggered", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zOpenPrc_Triggered, z);
		ZeroMemory(z, sizeof(z)); db->GetStr("B_OpenOppPrc", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zOpenOppPrc, z);

		ZeroMemory(z, sizeof(z)); db->GetStr("B_OpenTimeMT4)", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zOpenTmMT4, z);
		ZeroMemory(z, sizeof(z)); db->GetStr("B_Ticket", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zTicket, z);
		m_dataHandler.Data(iSymbol)->Long.nOpenSlippage = db->GetLong("B_OpenSlippage");
		m_dataHandler.Data(iSymbol)->Long.dLots = db->GetDbl("B_Lots");

		m_dataHandler.Data(iSymbol)->Short.bRejected = FALSE;
		ZeroMemory(z, sizeof(z)); db->GetStr("S_BrokerKey", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zBrokerKey, z);
		ZeroMemory(z, sizeof(z)); db->GetStr("S_OpenPrc", z); if (strlen(z) > 0)  strcpy(m_dataHandler.Data(iSymbol)->Short.zOpenPrc,z );
		ZeroMemory(z, sizeof(z)); db->GetStr("S_OpenPrcTriggered", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zOpenPrc_Triggered,z );
		ZeroMemory(z, sizeof(z)); db->GetStr("S_OpenOppPrc", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zOpenOppPrc, z);
		ZeroMemory(z, sizeof(z)); db->GetStr("S_OpenTimeMT4", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zOpenTmMT4,z );
		ZeroMemory(z, sizeof(z)); db->GetStr("S_Ticket", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zTicket, z);
		m_dataHandler.Data(iSymbol)->Short.nOpenSlippage = db->GetLong("S_OpenSlippage");
		m_dataHandler.Data(iSymbol)->Short.dLots = db->GetDbl("S_Lots");

		sprintf(zMsg, "[RECOVER](DBSerial:%s)(iSymbo:%d)(%s)(%s)(%d).[LONG]-(%5.5s)(Ticket:%s)(OpenPrc:%s) [SHORT]-(%5.5s)(Ticket:%s)(OpenPrc:%s)",
			m_dataHandler.Data(iSymbol)->zDBSerial
			, iSymbol
			, m_dataHandler.Data(iSymbol)->symbol
			, zStatus
			, m_dataHandler.Data(iSymbol)->lMagicNo

			, m_dataHandler.Data(iSymbol)->Long.zBrokerKey
			, m_dataHandler.Data(iSymbol)->Long.zTicket
			, m_dataHandler.Data(iSymbol)->Long.zOpenPrc

			, m_dataHandler.Data(iSymbol)->Short.zBrokerKey
			, m_dataHandler.Data(iSymbol)->Short.zTicket
			, m_dataHandler.Data(iSymbol)->Short.zOpenPrc
		);
			
		LOGGING(INFO, TRUE, TRUE, zMsg);

		db->Next();
	}
	return TRUE;
}


// enum EN_ORD_STATUS { ORDSTATUS_NONE = 0, ORDSTATUS_OPEN_TRIGGERED, ORDSTATUS_OPEN_MT4, ORDSTATUS_CLOSE_TRIGGERED, ORDSTATUS_CLOSE_MT4 };
void CIocp::SaveToDB(int iSymbol, BOOL bBuy, char* pzErrMsg, BOOL bSucc /*= TRUE*/, BOOL bMarketClose /*= FALSE*/)
{
	char zBuffer[1024] = { 0 };
	char zSerial[32] = { 0 };
	EN_ORD_STATUS ordStatus = (EN_ORD_STATUS)(m_dataHandler.Data(iSymbol)->nOrdStatus);

	if (bSucc)
	{
		if (ordStatus == ORDSTATUS_OPEN_TRIGGERED)
		{
			AlphaBasket_Save_OpenTriggered(iSymbol, m_dataHandler.Data(iSymbol), zBuffer);
		}
		if (ordStatus == ORDSTATUS_OPEN_MT4_1 || ordStatus == ORDSTATUS_OPEN_MT4_2)
		{
			AlphaBasket_Save_OpenMT4(iSymbol, m_dataHandler.Data(iSymbol), zBuffer);
		}
		if (ordStatus == ORDSTATUS_CLOSE_TRIGGERED)
		{
			AlphaBasket_Save_CloseTriggered(iSymbol, m_dataHandler.Data(iSymbol), bMarketClose, zBuffer);
		}
		if (ordStatus == ORDSTATUS_CLOSE_MT4_1 || ordStatus == ORDSTATUS_CLOSE_MT4_2)
		{
			AlphaBasket_Save_CloseMT4(iSymbol, m_dataHandler.Data(iSymbol), zBuffer);
		}
	}
	else
	{
		char cBuySell = (bBuy)? 'B':'S';
		AlphaBasket_Error(iSymbol, cBuySell, m_dataHandler.Data(iSymbol), pzErrMsg, zBuffer);
	}

	LOGGING(INFO, TRUE, TRUE, "DB SAVE(%s)", zBuffer);

	CDBHandlerAdo db(m_pDBPool->Get());
	if (!db->ExecQuery(zBuffer))
	{
		LOGGING(ERR, TRUE, TRUE, db->GetError());
	}
	else
	{
		db->GetStr("SERIAL_NO", zSerial);
		//LOGGING(INFO, FALSE, TRUE, "DB SERIAL_NO(%s)", zSerial);
		if (ordStatus == ORDSTATUS_OPEN_TRIGGERED)
			strcpy(m_dataHandler.Data(iSymbol)->zDBSerial, zSerial);
		if (ordStatus == ORDSTATUS_CLOSE_MT4_2)
			m_dataHandler.Data(iSymbol)->nRoundCnt = db->GetLong("ROUNDCNT");
	}
}


void CIocp::SaveLogToDB(char* pzTitle, char* pzMsg)
{
	char zBuffer[1024] = { 0 };
	

	sprintf(zBuffer,
		"AlphaBasket_Log "
		"'%s'"
		",'%s'"

		, pzTitle
		, pzMsg
	);

	LOGGING(INFO, FALSE, TRUE, "DB LOG(%s)", zBuffer);

	CDBHandlerAdo db(m_pDBPool->Get());
	if (!db->ExecQuery(zBuffer))
	{
		LOGGING(ERR, TRUE, TRUE, db->GetError());
	}
	else
	{
		
	}
}
