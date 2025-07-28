#include "CStrategyProc.h"
#include "Inc.h"
#include "../Common/AlphaInc.h"
#include "../Common/AlphaProtocolUni.h"
#include <process.h>
#include "../CommonAnsi/Util.h"


CStrategyProc::CStrategyProc(int nSymbolIdx, string sSymbol, double dPipSize, int nDecimalCnt, int nAllowedSpreadPoint, 
	unsigned int unOrderSendThreadId, unsigned int unDBThreadId, CMarketTimeHandler* marketTimeHandler)
{
	m_spec.nSymbolIdx			= nSymbolIdx;
	m_spec.sSymbol				= sSymbol; 
	m_spec.dPipSize				= dPipSize;
	m_spec.nDecimalCnt			= nDecimalCnt;
	m_spec.nAllowedSpreadPoint	= nAllowedSpreadPoint;
	m_unOrderSendThreadId	= unOrderSendThreadId;
	m_unDBThreadId			= unDBThreadId;
	m_bThreadRun			= TRUE;

	m_ordStatus				= ORDSTATUS_NONE;

	m_candle			= new CCandles();
	m_candle->SetSymbol(sSymbol);

	m_ema				= new CEma(sSymbol, m_candle);
	m_computeSignal		= new CComputeSignal(m_candle);
	m_marketTimeHandler = marketTimeHandler;


	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &Thread_Proc, this, 0, &m_unThread);
}


CStrategyProc::~CStrategyProc()
{
	m_bThreadRun = FALSE;
	delete m_computeSignal;
	delete m_ema;
	delete m_candle;
}


void CStrategyProc::ReceiveProcess(const char* pRecvData, int nDataLen)
{
	char* pData = new char[nDataLen];
	memcpy(pData, pRecvData, nDataLen);
	PostThreadMessage(m_unThread, WM_RECEIVE_DATA, (WPARAM)nDataLen, (LPARAM)pData);
}

unsigned WINAPI CStrategyProc::Thread_Proc(LPVOID lp)
{
	CStrategyProc* p = (CStrategyProc*)lp;
	while (p->m_bThreadRun)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message != WM_RECEIVE_DATA)
				continue;

			int nRecvLen = (int)msg.wParam;
			char* pRecvData = (char*)msg.lParam;
			
			char zCode[32] = { 0 };
			CProtoGet protoGet;
			CProtoUtils util;
			if (util.PacketCode((char*)pRecvData, zCode) == NULL)
			{
				LOGGING(ERR, TRUE, TRUE, "No packet code(%.128s)", pRecvData);
				continue;
			}

			// whenever new market data happens in the MT4, the latest candel data will be sent.
			// So, there is no packet of MARKET_DATA
			//if (strcmp(zCode, __ALPHA::CODE_MARKET_DATA) == 0)
			//{
			//	p->MD_Process(pRecvData, nRecvLen);
			//}
			if (strcmp(zCode, __ALPHA::CODE_HISTORY_CANDLES) == 0)
			{
				p->CandleHistory_Process(pRecvData, nRecvLen);
			}
			if (strcmp(zCode, __ALPHA::CODE_CANDLE_DATA) == 0)
			{
				p->Candle_Process(pRecvData, nRecvLen);
			}
			else if (strcmp(zCode, __ALPHA::CODE_ORDER_OPEN) == 0 )
			{
				p->RecvOrder_Open(pRecvData, nRecvLen);
			}
			else if (strcmp(zCode, __ALPHA::CODE_ORDER_CLOSE) == 0)
			{
				p->RecvOrder_Close(pRecvData, nRecvLen);
			}
		}

	}
	return 0;
}

//void CStrategyProc::MD_Process(char* pRecvData, int nDataLen)
//{
//	CProtoGet get;
//	if (!get.ParsingWithHeader(pRecvData, nDataLen))
//	{
//		LOGGING(ERR, TRUE, TRUE, "[MD_Process]parsing error(%s)", get.GetMsg());
//		return;
//	}
//
//	
//	double dBid = 0, dAsk = 0, dSpreadPt = 0;
//
//	dBid = get.GetValD(FDD_BID);
//	dAsk = get.GetValD(FDD_ASK);
//	dSpreadPt = get.GetValD(FDD_SPREAD);
//
//	sprintf(m_tick.zBid, "%.*f", m_spec.nDecimalCnt, dBid);
//	sprintf(m_tick.zAsk, "%.*f", m_spec.nDecimalCnt, dAsk);
//	m_tick.nSpreadPt = (int)dSpreadPt;
//}

void CStrategyProc::Candle_Process(char* pRecvData, int nDataLen)
{
	if (!m_candle->Is_EnoughCandles())
	{
		LOGGING(ERR, TRUE, TRUE, "[%s] Candles are not enough", __FUNCTION__);
		return;
	}

	CProtoGet get;
	if (!get.ParsingWithHeader(pRecvData, nDataLen))
	{
		LOGGING(ERR, TRUE, TRUE, "[%s]parsing error(%s)", __FUNCTION__, get.GetMsg());
		return;
	}

	char zSymbol[128] = { 0 }, zTimeFrame[32] = { 0 }, zMDTime[32] = { 0 }, zCandleTime[32] = { 0 };
	int iSymbol;
	double dClose;

	get.GetVal(FDS_SYMBOL,			zSymbol);
	get.GetVal(FDS_TIMEFRAME,		zTimeFrame);
	get.GetVal(FDS_MARKETDATA_TIME, zMDTime);
	get.GetVal(FDS_CANDLE_TIME,		zCandleTime);

	iSymbol = get.GetValN(FDN_SYMBOL_IDX);
	dClose	= get.GetValD(FDD_CLOSE_PRC);

	if ( dClose <= 0)
		return;

	char zC[32];
	sprintf(zC, "%.*f", m_spec.nDecimalCnt, dClose);


	//2021.10.04 07:22:00
	//char zTime1[32]; sprintf(zTime1, "%.8s", zCandleTime + 11);
	//char zTime2[32]; sprintf(zTime2, "%.8s", zMDTime + 11);
	//LOGGING(INFO, TRUE, TRUE, "[Recv Candle(%s)](C.T:%s)(M.T:%s)(C:%s)", zTimeFrame, zTime1, zTime2, zC);

	BOOL bNewCandle = FALSE;
	m_candle->Calc_Save_CandleData(zSymbol, zTimeFrame, zCandleTime, zMDTime,zC, &bNewCandle, FALSE);
	//if(bNewCandle)	m_candle->ShowwAllCandles();
	
	m_ema->Calc_Save_Ema(zCandleTime, zMDTime);	
	//if (bNewCandle)	m_ema->ShowAll();


	m_computeSignal->Calc_Save_Rsi(zCandleTime);
	//if (bNewCandle)	m_computeSignal->showRsi();
	
	m_computeSignal->Calc_Save_MabValues(zCandleTime, FALSE, 0);
	//if (bNewCandle)	m_computeSignal->showMabValues(); 
	
	if (!m_candle->Is_RecvFirstNewCandle())
		return;

	//MARKET TIME CHECK//
	if (!m_marketTimeHandler->Is_InTradingTime())
		return;

	EN_SIGNALS signal = m_computeSignal->ComputeSignal();
	if (signal ==SIGNAL_BUY)
	{
		SendOrder(iSymbol, zSymbol, __ALPHA::DEF_BUY);
		//TODO. Check correlations

		//TODO. check CRAM
	}
	if (signal == SIGNAL_SELL)
	{
		SendOrder(iSymbol, zSymbol, __ALPHA::DEF_SELL);
		//TODO. Check correlations

		//TODO. check CRAM
	}

	//TODO ORDER
}


void CStrategyProc::SendOrder(int iSymbol, char* pzSymbol, char cBuySell)
{
	int cmd = (cBuySell == __ALPHA::DEF_BUY) ? __ALPHA::getMT4Cmd_MarketBuy() : __ALPHA::getMT4Cmd_MarketSell();

	CProtoSet set;
	set.Begin();
	set.SetVal(FDS_CODE,		__ALPHA::CODE_ORDER_OPEN);
	set.SetVal(FDS_SYMBOL,		pzSymbol);
	set.SetVal(FDN_SYMBOL_IDX,	iSymbol);
	set.SetVal(FDN_ORDER_CMD,	cmd);
	set.SetVal(FDD_LOTS,		0.01);	// pData->Spec.dOrderLots);
	set.SetVal(FDS_MT4_TICKET,	0);
	set.SetVal(FDN_MAGIC_NO, MAGIC_NO);

	TSendOrder* pOrder = new TSendOrder;
	ZeroMemory(pOrder, sizeof(TSendOrder));
	pOrder->cBuySell[0] = cBuySell;

	int nLen = set.Complete(pOrder->zSendBuf);
	PostThreadMessage(m_unOrderSendThreadId, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pOrder);
}


void CStrategyProc::CandleHistory_Process(char* pRecvData, int nDataLen)
{
	CProtoGet get;
	if (!get.ParsingWithHeader(pRecvData, nDataLen))
	{
		LOGGING(ERR, TRUE, TRUE, "[CandleHistory_Process]parsing error(%s)", get.GetMsg());
		return;
	}

	char zSymbol[128] = { 0 }, zTimeFrame[32];
	smartptr zInnerArray(MAX_BUF);
	int nRecordCnt = 0;

	get.GetVal(FDS_SYMBOL,		zSymbol);
	get.GetVal(FDS_TIMEFRAME,	zTimeFrame);
	get.GetVal(FDN_RECORD_CNT,	&nRecordCnt);
	get.GetVal(FDS_ARRAY_DATA,	zInnerArray.get());

	if (nRecordCnt == 0)
	{
		LOGGING(ERR, TRUE, TRUE, "No candle History(%s)", zSymbol);
		return;
	}

	m_candle->ResetData();


	// 1=10[0x05]2=20[0x05]3=abc<0x06>1=100[0x05]2=200[0x05]3=def<0x06>
	// 1=10 ==> column : divided by [0x05]
	//          record : divided by [0x06]
	// >> FDS_CANDLE_TIME
	// >> FDD_CLOSE_PRC
	vector<string> vecRecord;
	CSplit split; 
	int nCnt = split.Split(zInnerArray.get(), DEF_DELI_RECORD, vecRecord);
	LOGGING(INFO, TRUE, FALSE, "[HISTORY CHART CNT](%d)(%s)", nCnt, zInnerArray.get());
	for (int k = 0; k < nCnt; k++)
	{
		vector<string> vecColumn;
		int nColumnCnt = split.Split(vecRecord[k], DEF_DELI_COLUMN, vecColumn);

		char tmp[32] = { 0 }, zCandleTime[32] = { 0 }, zClose[32] = { 0 };
		for (int j = 0; j < nColumnCnt; j++)
		{	
			CProtoUtils utils;			
			if (utils.GetValue((char*)vecColumn[j].c_str(), FDS_CANDLE_TIME, tmp))
				strcpy(zCandleTime, tmp);
			if (utils.GetValue((char*)vecColumn[j].c_str(), FDD_CLOSE_PRC, tmp))
				strcpy(zClose, tmp);
		}

		BOOL bNewCandle;
		m_candle->Calc_Save_CandleData(zSymbol, zTimeFrame, zCandleTime, zCandleTime, zClose, &bNewCandle, TRUE);
	}
	m_candle->ShowwAllCandles();
	m_computeSignal->Calc_History_SigFactors();
	m_computeSignal->showRsi();
}


void CStrategyProc::StrategyProcess()
{


	//TODO. Send to orderthread
}

void CStrategyProc::RecvOrder_Open(char* pRecvData, int nDataLen)
{
	CProtoGet get;
	if (!get.ParsingWithHeader(pRecvData, nDataLen))
	{
		LOGGING(ERR, TRUE, TRUE, "[Rev OpenOrd](%s)(%s)", get.GetMsg(), pRecvData);
		return;
	}

	char zErrYN[32] = { 0 };
	char zErrMsg[512] = { 0 };
	char zBrokerKey[128] = { 0 };// , zSymbol[128] = { 0 };
	char zTicketNo[128] = { 0 };
	char zMT4Time[128] = { 0 };
	int iSymbol = 0, nCmd = 0;
	double dOpenPrc = 0, dLots = 0;

	get.GetVal(FDS_ERR_YN, zErrYN);
	get.GetVal(FDS_KEY, zBrokerKey);
	get.GetVal(FDS_ERR_MSG, zErrMsg);
	get.GetVal(FDS_MT4_TICKET, zTicketNo);
	get.GetVal(FDS_OPEN_TM, zMT4Time);

	nCmd = get.GetValN(FDN_ORDER_CMD);
	iSymbol = get.GetValN(FDN_SYMBOL_IDX);
	dOpenPrc = get.GetValD(FDD_OPEN_PRC);
	dLots = get.GetValD(FDD_LOTS);

	CProtoUtils util;
	if (util.IsSuccess(zErrYN[0]))
	{
		m_ordStatus = ORDSTATUS_OPEN;
	}
	else // reject 
	{
		m_ordStatus = ORDSTATUS_NONE;
	}

	//TODO. SAVE ON DB SaveToDB(iSymbol, __ALPHA::IsBuyOrder(nCmd), "");

}


void CStrategyProc::RecvOrder_Close(char* pRecvData, int nDataLen)
{
	CProtoGet get;
	if (!get.ParsingWithHeader(pRecvData, nDataLen))
	{
		//TODO LOGGING(ERR, TRUE, TRUE, "(%s)(%s)", get.GetMsg(), pRecvData);
		return;
	}


	char zErrYN[32]			= { 0 };
	char zErrMsg[512]		= { 0 };
	char zBrokerKey[128]	= { 0 };// , zSymbol[128] = { 0 };
	char zTicketNo[128]		= { 0 };
	char zMT4Time[128]		= { 0 };
	int iSymbol = 0, nCmd = 0;
	double dClosePrc = 0, dLots = 0, dPL = 0, dCmsn = 0, dSwap = 0;

	get.GetVal(FDS_ERR_YN, zErrYN);
	get.GetVal(FDS_KEY, zBrokerKey);
	get.GetVal(FDS_MT4_TICKET, zTicketNo);
	get.GetVal(FDS_CLOSE_TM, zMT4Time);

	dClosePrc	= get.GetValD(FDD_CLOSE_PRC);
	dLots		= get.GetValD(FDD_LOTS);
	dCmsn		= get.GetValD(FDD_CMSN);
	dPL			= get.GetValD(FDD_PROFIT);	
	nCmd		= get.GetValN(FDN_ORDER_CMD);
	dSwap		= get.GetValD(FDD_SWAP);

	CProtoUtils util;
	if (util.IsSuccess(zErrYN[0]))
	{
		m_ordStatus = ORDSTATUS_CLOSE;
	}
	else // reject
	{
		m_ordStatus = ORDSTATUS_OPEN;
		//TODO LOGGING
	}

	//TODO. SAVE ON DB SaveToDB(iSymbol, __ALPHA::IsBuyOrder(nCmd), "");

}
