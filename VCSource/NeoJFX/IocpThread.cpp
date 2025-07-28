
#include "IocpThread.h"
#include <process.h>
#include "../CommonAnsi/Util.h"
#include "../CommonAnsi/LogMsg.h"
#include "../CommonAnsi/IRExcept.h"
#include "../Common/AlphaInc.h"
#include "../Common/TimeInterval.h"
//#include "Compose_DBSave_String.h"


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

	m_sockets.main = m_sockets.hedge = INVALID_SOCKET;


	m_hThread_Listen	= m_hThread_Parsing		= m_hThread_Dispatch	= NULL;
	m_hThread_OrderSend = m_hThread_MarketTime	= m_hThread_Db			= NULL;

	m_unThread_Listen		= m_unThread_Parsing	= m_unThread_Dispatch	= 0;
	m_unThread_OrderSend	= m_unThread_MarketTime = m_unThread_Db			= 0;

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




BOOL CIocp::Initialize( )
{
	ReadIPPOrt();

	InitializeCriticalSection(&m_csCK);
	InitializeCriticalSection(&m_csStrategy);
	InitializeCriticalSection(&m_csPacket);

	m_hThread_MarketTime	= (HANDLE)_beginthreadex(NULL, 0, &Thread_MarketTime, this, 0, &m_unThread_MarketTime);
	m_hThread_Parsing		= (HANDLE)_beginthreadex(NULL, 0, &Thread_Parsing, this, 0, &m_unThread_Parsing);
	m_hThread_Dispatch		= (HANDLE)_beginthreadex(NULL, 0, &Thread_Dispatch, this, 0, &m_unThread_Dispatch);

	m_hThread_OrderSend		= (HANDLE)_beginthreadex(NULL, 0, &Thread_OrderSend, this, 0, &m_unThread_OrderSend);
	m_hThread_Db			= (HANDLE)_beginthreadex(NULL, 0, &Thread_DBSave, this, 0, &m_unThread_Db);

	if (!Load_TradingTime())
		return FALSE;

	if (!CreateStrategies_BySymbol())
		return FALSE;

	//DB OPEN
	if (!DBOpen())
		return FALSE;

	if (!RecoverOpenPositions())
		return FALSE;

	if (!InitListen()) {
		return FALSE;
	}
	m_hThread_Listen = (HANDLE)_beginthreadex(NULL, 0, &Thread_Listen, this, 0, &m_unThread_Listen);


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
	unsigned int dwID;
	for (unsigned int n = 0; n < m_dwThreadCount; n++)
	{
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_Iocp, this, 0, &dwID);
		CloseHandle(h);
	}

	return TRUE;
}


BOOL CIocp::Load_TradingTime()
{
	char zTimeTradeStart[32] = { 0 }, zTimeTradeEnd[32] = { 0 }, zTimeMarketClr[32] = { 0 };
	char zMarketCloseUseYN[32] = { 0 };

	
	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("TRADING_START"), zTimeTradeStart);
	if (zTimeTradeStart[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get TRADING_START count from config file");
		return FALSE;
	}
	
	
	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("TRADING_END"), zTimeTradeEnd);
	if (zTimeTradeEnd[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get TRADING_END count from config file");
		return FALSE;
	}

	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("MARKET_CLOSE_CLR_USE_YN"), zMarketCloseUseYN);
	if (zMarketCloseUseYN[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get MARKET_CLOSE_CLR_USE_YN count from config file");
		return FALSE;
	}

	CUtil::GetConfig(g_zConfig, TEXT("MARKET_TIME"), TEXT("MARKET_CLOSE_CLR_TIME"), zTimeMarketClr);
	if (zTimeMarketClr[0] == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get MARKET_CLOSE_CLR_TIME count from config file");
		return FALSE;
	}

	m_marketTimeHandler.Set_MarketTime(zTimeTradeStart, zTimeTradeEnd, zMarketCloseUseYN, zTimeMarketClr);

	return TRUE;
}

BOOL CIocp::CreateStrategies_BySymbol()
{
	char buffer[128] = { 0 };
	char zIdx[32]; 
	
	CUtil::GetConfig(g_zConfig, TEXT("SYMBOLS"), TEXT("COUNT"), buffer);
	if (buffer[0] == NULL)
	{
		LOGGING(ERR, TRUE, FALSE, "Failed to get symbols count from config file");
		return FALSE;
	}
	int nCnt = atoi(buffer);

	
	// NeoJFX.ini
	// # symbol / pip size / decimal cnt / allowed spread 

	for (int i = 0; i < nCnt; i++)
	{
		char zLine[512] = { 0 };
		sprintf(zIdx, "%d", i);
		CUtil::GetConfig(g_zConfig, TEXT("SYMBOLS"), zIdx, zLine);

		vector<string> vecSplit;
		CUtil::SplitData(zLine, '/', &vecSplit);

		CStrategyProc* p = new CStrategyProc(
			i,
			vecSplit.at(0).c_str(),
			atof(vecSplit.at(1).c_str()),
			atoi(vecSplit.at(2).c_str()),
			atoi(vecSplit.at(3).c_str()),
			m_unThread_OrderSend,
			m_unThread_Db,
			&m_marketTimeHandler
		);

		LOGGING(INFO, TRUE, TRUE, "Create Strategy[%d](%s)(Pipsize:%.5f)(DecimalCnt:%d)(AllowedSpread:%d)",
			i,
			vecSplit.at(0).c_str(),
			atof(vecSplit.at(1).c_str()),
			atoi(vecSplit.at(2).c_str()),
			atoi(vecSplit.at(3).c_str())
		);

		m_mapStrategy[vecSplit.at(0)] = p;
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
	if ( WSAEventSelect(m_sockListen, m_hListenEvent, FD_ACCEPT)== SOCKET_ERROR) 
	{

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



void CIocp::Finalize()
{
	m_bRun = FALSE;


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
	DeleteCriticalSection(&m_csStrategy);
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
			//TODO
			//if (pThis->m_bWeekendStartAlready)
			//{
			//	delete pIoContext;
			//	continue;
			//}

			pThis->m_parser.AddPacket(pIoContext->buf, dwIoSize);
			
			PostThreadMessage(pThis->m_unThread_Parsing, WM_RECEIVE_DATA, (WPARAM)0, (LPARAM)pCK);
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

unsigned WINAPI CIocp::Thread_Dispatch(LPVOID lp)
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
CODE_MARKET_DATA
CODE_CANDLE_DATA
CODE_ORDER_OPEN
CODE_ORDER_CLOSE
*/
void CIocp::DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen)
{
	char zCode[32] = { 0 };

	CProtoUtils util;
	if (util.PacketCode((char*)pRecvData, zCode) == NULL)
	{
		LOGGING(ERR, TRUE, TRUE, "No packet code(%.128s)", pRecvData);
		return;
	}

	if (strcmp(zCode, __ALPHA::CODE_MARKET_DATA) != 0)
	{
		LOGGING(INFO, FALSE, FALSE, "[RECV](%s)", pRecvData);
		Sleep(1);	//TODO FOR TESING
	}


	if (strcmp(zCode, __ALPHA::CODE_LOGON) == 0)
	{
		Logon_Process(pCK->sock, pRecvData, nRecvLen);
	}


	if (//strcmp(zCode, __ALPHA::CODE_MARKET_DATA) == 0 || 
		strcmp(zCode, __ALPHA::CODE_HISTORY_CANDLES) == 0 ||
		strcmp(zCode, __ALPHA::CODE_CANDLE_DATA) == 0 ||
		strcmp(zCode, __ALPHA::CODE_ORDER_OPEN) == 0  || 
		strcmp(zCode, __ALPHA::CODE_ORDER_CLOSE) == 0
		)
	{
		char zSymbol[32] = { 0 };
		CProtoUtils util;
		if ( !util.GetValue((char*)pRecvData, FDS_SYMBOL, zSymbol) )
		{
			LOGGING(ERR, TRUE, TRUE, "No symbol in the packet(%.128s)", pRecvData);
			return;
		}
		string sSymbol = string(zSymbol);
		IT_MAP_STRATEGY it;
		if (!FindStrategyMap(sSymbol, it))
		{
			LOGGING(ERR, TRUE, TRUE, "Failed to find (%s)strategy", sSymbol.c_str());
			return;
		}

		(*it).second->ReceiveProcess(pRecvData, nRecvLen);
	}
	
	
}



void CIocp::SendOpenOrder(int iSymbol)
{
	char zTemp[128] = { 0 };
	char zSendBuff[MAX_BUF] = { 0 };

	//TODO


	//TData* pData = m_dataHandler.Data(iSymbol);

	//CProtoSet set;
	//set.Begin();
	//set.SetVal(FDS_CODE,		__ALPHA::CODE_ORDER_OPEN);
	//set.SetVal(FDN_SYMBOL_IDX,	iSymbol);
	//set.SetVal(FDS_SYMBOL,		pData->symbol);
	//set.SetVal(FDN_ORDER_CMD,	__ALPHA::getMT4Cmd_MarketBuy());
	//set.SetVal(FDD_LOTS,		pData->Spec.dOrderLots);
	//set.SetVal(FDS_MT4_TICKET,	0);
	//set.SetVal(FDN_MAGIC_NO,	pData->lMagicNo);	

	//TSendOrder* pSendBuy = new TSendOrder;
	//ZeroMemory(pSendBuy->zSendBuf, sizeof(pSendBuy->zSendBuf));
	//strcpy(pSendBuy->brokerKey, pData->Long.zBrokerKey);

	//int nLen = set.Complete(pSendBuy->zSendBuf);
	//PostThreadMessage(m_unThread_OrderSend, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pSendBuy);

	//sprintf(m_zMsg, "[SendBuy_Open](%5.5s)(%s)(IDX:%d)(%s)",
	//	pData->Long.zBrokerKey, pData->symbol, iSymbol, __ALPHA::getMT4CmdDesc(__ALPHA::getMT4Cmd_MarketBuy(), zTemp));
	//LOGGING(INFO, TRUE, TRUE, m_zMsg);

	////
	////
	////

	//CProtoSet set2;
	//set2.Begin();
	//set2.SetVal(FDS_CODE,		__ALPHA::CODE_ORDER_OPEN);
	//set2.SetVal(FDN_SYMBOL_IDX, iSymbol);
	//set2.SetVal(FDS_SYMBOL,		pData->symbol);
	//set2.SetVal(FDN_ORDER_CMD,	__ALPHA::getMT4Cmd_MarketSell());
	//set2.SetVal(FDD_LOTS,		pData->Spec.dOrderLots);
	//set2.SetVal(FDS_MT4_TICKET, 0);

	//TSendOrder* pSendSell = new TSendOrder;
	//ZeroMemory(pSendSell->zSendBuf, sizeof(pSendSell->zSendBuf));
	//strcpy(pSendSell->brokerKey, pData->Short.zBrokerKey);

	//nLen = set2.Complete(pSendSell->zSendBuf);
	//PostThreadMessage(m_unThread_OrderSend, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pSendSell);

	//sprintf(m_zMsg, "[SendSell_Open](%5.5s)(%s)(IDX:%d)(%s)",
	//	pData->Short.zBrokerKey, pData->symbol, iSymbol, __ALPHA::getMT4CmdDesc(__ALPHA::getMT4Cmd_MarketSell(), zTemp));
	//LOGGING(INFO, TRUE, TRUE, m_zMsg);
}



void CIocp::SendCloseOrder(int iSymbol, char cOrderSide)
{
	char zTemp[128] = { 0 };
	char zHead[128] = { 0 };
	char zSendBuff[MAX_BUF] = { 0 };

	//TODO

	//TData* pData = m_dataHandler.Data(iSymbol);
	//TSendOrder* pSendOrder = new TSendOrder;
	//ZeroMemory(pSendOrder->zSendBuf, sizeof(pSendOrder->zSendBuf));

	//int nLen = 0;

	//CProtoSet set;
	//set.Begin();
	//set.SetVal(FDS_CODE, __ALPHA::CODE_ORDER_CLOSE);
	//set.SetVal(FDN_SYMBOL_IDX,	iSymbol);
	//set.SetVal(FDS_SYMBOL,		pData->symbol);

	//// Buy Order - close short position
	//if (cOrderSide == __ALPHA::BUY_SIDE)
	//{
	//	set.SetVal(FDN_ORDER_CMD, __ALPHA::getMT4Cmd_MarketBuy());

	//	if (pData->Short.dLots <= 0)
	//		set.SetVal(FDD_LOTS, pData->Spec.dOrderLots);
	//	else
	//		set.SetVal(FDD_LOTS, pData->Short.dLots);

	//	set.SetVal(FDS_MT4_TICKET, pData->Short.zTicket);

	//	strcpy(pSendOrder->brokerKey, pData->Short.zBrokerKey);

	//	nLen = set.Complete(pSendOrder->zSendBuf);

	//	sprintf(m_zMsg, "[SendBuy_CloseShort](%5.5s)(%s)(IDX:%d)(%s)(Ticket:%s)",
	//		pData->Short.zBrokerKey, pData->symbol, iSymbol, __ALPHA::getMT4CmdDesc(__ALPHA::getMT4Cmd_MarketBuy(), zTemp), pData->Short.zTicket);
	//}
	//else
	//{
	//	set.SetVal(FDN_ORDER_CMD, __ALPHA::getMT4Cmd_MarketSell());

	//	if (pData->Long.dLots <= 0)
	//		set.SetVal(FDD_LOTS, pData->Spec.dOrderLots);
	//	else
	//		set.SetVal(FDD_LOTS, pData->Long.dLots);

	//	set.SetVal(FDS_MT4_TICKET, pData->Long.zTicket);

	//	strcpy(pSendOrder->brokerKey, pData->Long.zBrokerKey);

	//	nLen = set.Complete(pSendOrder->zSendBuf);

	//	sprintf(m_zMsg, "[SendSell_CloseLong](%5.5s)(%s)(IDX:%d)(%s)(Ticket:%s)",
	//		pData->Long.zBrokerKey, pData->symbol, iSymbol, __ALPHA::getMT4CmdDesc(__ALPHA::getMT4Cmd_MarketSell(), zTemp), pData->Long.zTicket);

	//}

	//LOGGING(INFO, TRUE, TRUE, m_zMsg);
	//LOGGING(INFO, TRUE, FALSE, pSendOrder->zSendBuf);

	//PostThreadMessage(m_unThread_OrderSend, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pSendOrder);
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
		if (sockClient == INVALID_SOCKET) 
		{
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
				
				p->RequestSendIO(p->m_sockets.main, pOrd->zSendBuf, (int)msg.wParam);

				LOGGING(INFO, TRUE, TRUE, "[SEND ORD](%s)", pOrd->zSendBuf);

				delete (TSendOrder*)(msg.lParam);
			}
		}

	}
	return 0;
}

unsigned WINAPI CIocp::Thread_DBSave(LPVOID lp)
{
	return 0;
}

unsigned WINAPI CIocp::Thread_MarketTime(LPVOID lp)
{
	CIocp* p = (CIocp*)lp;
	while (p->m_bRun)
	{
		Sleep(500);
		
		p->Load_TradingTime();

		if (p->m_marketTimeHandler.IsToday_Weekend())
			continue;

		BOOL bTimeToMarketClr = FALSE;
		p->m_marketTimeHandler.Check_NowTime(&bTimeToMarketClr);

		if (bTimeToMarketClr)
		{
			p->CloseAllOpenPositions("MarketClose");
		}
	}
	
	return 0;
}



void CIocp::CloseAllOpenPositions(char* pzCloseType)
{
	//TODO

	//for (int iSymbol = 0; iSymbol < m_dataHandler.Get_SymbolCntCurrent(); iSymbol++)
	//{
	//	m_bMarketCloseClearedAlready = TRUE;

	//	if (m_dataHandler.Data(iSymbol)->nOrdStatus >= ORDSTATUS_CLOSE_TRIGGERED)
	//		continue;

	//	BOOL bMustSendOrd = FALSE;
	//	m_dataHandler.MarketClose(iSymbol, &bMustSendOrd);
	//	if (bMustSendOrd)
	//	{
	//		SendCloseOrder(iSymbol, __ALPHA::BUY_SIDE);
	//		SendCloseOrder(iSymbol, __ALPHA::SELL_SIDE);
	//		m_dataHandler.Update_OrdStatus_CloseTriggered(iSymbol);
	//		SaveToDB(iSymbol, 0, "", TRUE);
	//		LOGGING(INFO, TRUE, TRUE, "[%s-Buy-CloseShort](%s)(%5.5s)(Ticket:%s)"
	//			, pzCloseType
	//			, m_dataHandler.Data(iSymbol)->symbol
	//			, m_dataHandler.Data(iSymbol)->Long.zBrokerKey
	//			, m_dataHandler.Data(iSymbol)->Long.zTicket
	//		);
	//		LOGGING(INFO, TRUE, TRUE, "[%s-Sell-CloseLong](%s)(%5.5s)(Ticket:%s)"
	//			, pzCloseType
	//			, m_dataHandler.Data(iSymbol)->symbol
	//			, m_dataHandler.Data(iSymbol)->Short.zBrokerKey
	//			, m_dataHandler.Data(iSymbol)->Short.zTicket
	//		);
	//	}
	//}
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
	//TODO

	//char zMsg[1024];
	//char zQ[1024];
	//char zStatus[32];
	//sprintf(zQ, "AlphaBasket_Recovery ");

	//CDBHandlerAdo db(m_pDBPool->Get());
	//if (!db->ExecQuery(zQ))
	//{
	//	LOGGING(ERR, TRUE, TRUE, "Recover Failed:%s",db->GetError());
	//	return FALSE;
	//}

	//while (db->IsNextRow())
	//{
	//	int iSymbol = db->GetLong("ISymbol");
	//	if (iSymbol < 0)
	//	{
	//		LOGGING(ERR, TRUE, TRUE, "Recover Failed:Wrong iSymbol");
	//		return FALSE;
	//	}

	//	if (iSymbol >= m_dataHandler.SymbolCount())
	//		continue;

	//	char z[256];
	//	strcpy(m_dataHandler.Data(iSymbol)->symbol, db->GetStr("Symbol", z));
	//	m_dataHandler.Data(iSymbol)->nOrdStatus = db->GetLong("ORD_STATUS");
	//	db->GetStr("ORD_STATUS_DESC", zStatus);
	//	strcpy(m_dataHandler.Data(iSymbol)->zDBSerial, db->GetStr("SERIAL_NO", z));

	//	m_dataHandler.Data(iSymbol)->nCloseNetPLTriggered = 0;
	//	m_dataHandler.Data(iSymbol)->nProfitCnt = 0;
	//	m_dataHandler.Data(iSymbol)->nLossCnt = 0;
	//	0514-2 m_dataHandler.Data(iSymbol)->bNoMoreOpenByLossCnt = FALSE;
	//	m_dataHandler.Data(iSymbol)->lMagicNo = db->GetLong("MagicNo");

	//	m_dataHandler.Data(iSymbol)->Long.bRejected = FALSE;
	//	ZeroMemory(z, sizeof(z)); db->GetStr("B_BrokerKey", z); if(strlen(z)>0) strcpy(m_dataHandler.Data(iSymbol)->Long.zBrokerKey, z);
	//	ZeroMemory(z, sizeof(z)); db->GetStr("B_OpenPrc", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zOpenPrc, z);
	//	ZeroMemory(z, sizeof(z)); db->GetStr("B_OpenPrcTriggered", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zOpenPrc_Triggered, z);
	//	ZeroMemory(z, sizeof(z)); db->GetStr("B_OpenOppPrc", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zOpenOppPrc, z);

	//	ZeroMemory(z, sizeof(z)); db->GetStr("B_OpenTimeMT4)", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zOpenTmMT4, z);
	//	ZeroMemory(z, sizeof(z)); db->GetStr("B_Ticket", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Long.zTicket, z);
	//	m_dataHandler.Data(iSymbol)->Long.nOpenSlippage = db->GetLong("B_OpenSlippage");
	//	m_dataHandler.Data(iSymbol)->Long.dLots = db->GetDbl("B_Lots");

	//	m_dataHandler.Data(iSymbol)->Short.bRejected = FALSE;
	//	ZeroMemory(z, sizeof(z)); db->GetStr("S_BrokerKey", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zBrokerKey, z);
	//	ZeroMemory(z, sizeof(z)); db->GetStr("S_OpenPrc", z); if (strlen(z) > 0)  strcpy(m_dataHandler.Data(iSymbol)->Short.zOpenPrc,z );
	//	ZeroMemory(z, sizeof(z)); db->GetStr("S_OpenPrcTriggered", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zOpenPrc_Triggered,z );
	//	ZeroMemory(z, sizeof(z)); db->GetStr("S_OpenOppPrc", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zOpenOppPrc, z);
	//	ZeroMemory(z, sizeof(z)); db->GetStr("S_OpenTimeMT4", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zOpenTmMT4,z );
	//	ZeroMemory(z, sizeof(z)); db->GetStr("S_Ticket", z); if (strlen(z) > 0) strcpy(m_dataHandler.Data(iSymbol)->Short.zTicket, z);
	//	m_dataHandler.Data(iSymbol)->Short.nOpenSlippage = db->GetLong("S_OpenSlippage");
	//	m_dataHandler.Data(iSymbol)->Short.dLots = db->GetDbl("S_Lots");

	//	sprintf(zMsg, "[RECOVER](DBSerial:%s)(iSymbo:%d)(%s)(%s)(%d).[LONG]-(%5.5s)(Ticket:%s)(OpenPrc:%s) [SHORT]-(%5.5s)(Ticket:%s)(OpenPrc:%s)",
	//		m_dataHandler.Data(iSymbol)->zDBSerial
	//		, iSymbol
	//		, m_dataHandler.Data(iSymbol)->symbol
	//		, zStatus
	//		, m_dataHandler.Data(iSymbol)->lMagicNo

	//		, m_dataHandler.Data(iSymbol)->Long.zBrokerKey
	//		, m_dataHandler.Data(iSymbol)->Long.zTicket
	//		, m_dataHandler.Data(iSymbol)->Long.zOpenPrc

	//		, m_dataHandler.Data(iSymbol)->Short.zBrokerKey
	//		, m_dataHandler.Data(iSymbol)->Short.zTicket
	//		, m_dataHandler.Data(iSymbol)->Short.zOpenPrc
	//	);
	//		
	//	LOGGING(INFO, TRUE, TRUE, zMsg);

	//	db->Next();
	//}
	return TRUE;
}


// enum EN_ORD_STATUS { ORDSTATUS_NONE = 0, ORDSTATUS_OPEN_TRIGGERED, ORDSTATUS_OPEN_MT4, ORDSTATUS_CLOSE_TRIGGERED, ORDSTATUS_CLOSE_MT4 };
void CIocp::SaveToDB(int iSymbol, BOOL bBuy, char* pzErrMsg, BOOL bSucc /*= TRUE*/, BOOL bMarketClose /*= FALSE*/)
{
	//TODO

	//char zBuffer[1024] = { 0 };
	//char zSerial[32] = { 0 };
	//EN_ORD_STATUS ordStatus = (EN_ORD_STATUS)(m_dataHandler.Data(iSymbol)->nOrdStatus);

	//if (bSucc)
	//{
	//	if (ordStatus == ORDSTATUS_OPEN_TRIGGERED)
	//	{
	//		AlphaBasket_Save_OpenTriggered(iSymbol, m_dataHandler.Data(iSymbol), zBuffer);
	//	}
	//	if (ordStatus == ORDSTATUS_OPEN_MT4_1 || ordStatus == ORDSTATUS_OPEN_MT4_2)
	//	{
	//		AlphaBasket_Save_OpenMT4(iSymbol, m_dataHandler.Data(iSymbol), zBuffer);
	//	}
	//	if (ordStatus == ORDSTATUS_CLOSE_TRIGGERED)
	//	{
	//		AlphaBasket_Save_CloseTriggered(iSymbol, m_dataHandler.Data(iSymbol), bMarketClose, zBuffer);
	//	}
	//	if (ordStatus == ORDSTATUS_CLOSE_MT4_1 || ordStatus == ORDSTATUS_CLOSE_MT4_2)
	//	{
	//		AlphaBasket_Save_CloseMT4(iSymbol, m_dataHandler.Data(iSymbol), zBuffer);
	//	}
	//}
	//else
	//{
	//	char cBuySell = (bBuy)? 'B':'S';
	//	AlphaBasket_Error(iSymbol, cBuySell, m_dataHandler.Data(iSymbol), pzErrMsg, zBuffer);
	//}

	//LOGGING(INFO, TRUE, TRUE, "DB SAVE(%s)", zBuffer);

	//CDBHandlerAdo db(m_pDBPool->Get());
	//if (!db->ExecQuery(zBuffer))
	//{
	//	LOGGING(ERR, TRUE, TRUE, db->GetError());
	//}
	//else
	//{
	//	db->GetStr("SERIAL_NO", zSerial);
	//	//LOGGING(INFO, FALSE, TRUE, "DB SERIAL_NO(%s)", zSerial);
	//	if (ordStatus == ORDSTATUS_OPEN_TRIGGERED)
	//		strcpy(m_dataHandler.Data(iSymbol)->zDBSerial, zSerial);
	//	if (ordStatus == ORDSTATUS_CLOSE_MT4_2)
	//		m_dataHandler.Data(iSymbol)->nRoundCnt = db->GetLong("ROUNDCNT");
	//}
}



BOOL CIocp::FindStrategyMap(string sSymbol, IT_MAP_STRATEGY& it)
{
	it = m_mapStrategy.find(sSymbol);
	return (it != m_mapStrategy.end());
}

BOOL CIocp::Logon_Process(SOCKET sock, const char* pLoginData, int nDataLen)
{
	CProtoGet get;
	if (!get.ParsingWithHeader(pLoginData, nDataLen))
	{
		LOGGING(ERR, TRUE, TRUE, "(%s)(%s)", get.GetMsg(), pLoginData);
		return FALSE;
	}
	int res = 0;
	char zMasterCopierTp[128] = { 0 };
	char zClientSockTp[32] = { 0 };

	res = get.GetVal(FDS_MASTERCOPIER_TP, zMasterCopierTp);
	res += get.GetVal(FDS_CLIENT_SOCKET_TP, zClientSockTp);

	LOGGING(INFO, TRUE, TRUE, "[LOGIN](%5.5s)(%c)(Socket:%d)", zMasterCopierTp, zClientSockTp[0], sock);

	if (res < 2)
	{
		LOGGING(ERR, TRUE, TRUE, "Failed to get FDS_MASTERCOPIER_TP from Logon Packet");
		return FALSE;
	}

	// Only client receiving socket is added ==> To transfer data to client
	if (zClientSockTp[0] == 'R')
	{
		if (zMasterCopierTp[0] == MC_TP_MASTER)
			m_sockets.main = sock;
		else
			m_sockets.hedge = sock;

		char zSendBuff[MAX_BUF] = { 0 };
		string sSymbolArray;
		char zSymbolArray[512] = { 0 };
		CProtoSet set;
		set.Begin();
		set.SetVal(FDS_CODE,		__ALPHA::CODE_LOGON);
		set.SetVal(FDN_DATA_CNT,	MAX_CANDLE);
		set.SetVal(FDN_ARRAY_SIZE,	(int)m_mapStrategy.size());

		int k = 0;
		map<string, CStrategyProc*>::iterator it;
		for( it=m_mapStrategy.begin(), k=0; it!=m_mapStrategy.end(); ++it, k++)
		{
			sprintf(zSymbolArray, "%d=%s%c%d=%d%c",
				FDS_SYMBOL, (*it).first.c_str(), DEF_DELI_COLUMN
				, FDN_SYMBOL_IDX, k, DEF_DELI_RECORD
			);
			sSymbolArray += zSymbolArray;

			ZeroMemory(zSymbolArray, sizeof(zSymbolArray));
		}
		set.SetVal(FDS_ARRAY_DATA, sSymbolArray);
		int nLen = set.Complete(zSendBuff);
		RequestSendIO(sock, zSendBuff, nLen);
	}
	return TRUE;
}

//[RECV](134=0044101=1007521=10506=1175=107=GBPUSD514=0)
