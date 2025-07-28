#include "DataHandler.h"
#include "../CommonAnsi/LogMsg.h"
#include "../CommonAnsi/Util.h"

extern CLogMsg g_log, g_debug;

INT g_LogCntBestPrc = 0;
int g_LogCntEstm = 0;
#define LOG_CNT 50

CDataHandler::CDataHandler()
{
    m_nSymbolCntTodo = 0;
    m_nBrokerCntTodo = 0;
    //m_bTradeStart = FALSE;
    m_bNoMoreOpen = FALSE;
    m_bProfitCutTriggered = FALSE;
}

CDataHandler::~CDataHandler()
{
    for (UINT i = 0; i < m_vecDataPerSymbol.size(); i++)
    {
        DeleteCriticalSection(CS(i));
        delete m_vecDataPerSymbol.at(i);
    }
    m_vecDataPerSymbol.clear();

    for (UINT i = 0; i < m_vecBroker.size(); i++)
        delete m_vecBroker.at(i);
    m_vecBroker.clear();

}


// Trade can be started after all brokers start sending market data
//BOOL CDataHandler::Is_ReadyTrade()
//{
//    //if (m_vecBroker.size() < (UINT)m_nBrokerCntTodo )
//    //    return FALSE;
//
//    return m_bTradeStart;
//}


BOOL CDataHandler::Add_Symbol(
    int iSymbol, const char* pzSymbol, double dPipSize
    , int nDecimalCnt, int nActiveSpreadPt
    , int nThresholdOpenPt, int nThresholdClosePt, double dOrderLots
    , int nNoMoreOpenRoundCnt // 0514-2
)
{
    TDataPerSymbol* perSymbol = new TDataPerSymbol;

    perSymbol->data = new TData;
    perSymbol->cs = new CRITICAL_SECTION;
    perSymbol->calc = new CCalcBestPrc;

    ZeroMemory(perSymbol->data->symbol, sizeof(perSymbol->data->symbol));
    ZeroMemory(perSymbol->data->zDBSerial, sizeof(perSymbol->data->zDBSerial));
    perSymbol->data->nOrdStatus             = 0;
    perSymbol->data->nCloseNetPLTriggered   = 0;
    perSymbol->data->nProfitCnt             = 0;
    perSymbol->data->nLossCnt               = 0;
    perSymbol->data->nRoundCnt              = 0;
    perSymbol->data->lMagicNo               = 0;
    //perSymbol->data->bNoMoreOpenByLossCnt = FALSE;

    ZeroMemory(&perSymbol->data->Spec, sizeof(perSymbol->data->Spec));
    ZeroMemory(&perSymbol->data->BestPrcForOpen, sizeof(perSymbol->data->BestPrcForOpen));
    ZeroMemory(&perSymbol->data->Long, sizeof(perSymbol->data->Long));
    ZeroMemory(&perSymbol->data->Short, sizeof(perSymbol->data->Short));

    strcpy(perSymbol->data->symbol, pzSymbol);
    perSymbol->data->nOrdStatus             = ORDSTATUS_NONE;
    perSymbol->data->nCloseNetPLTriggered   = 0;
    perSymbol->data->nLossCnt               = 0;
    perSymbol->data->nProfitCnt             = 0;
    //perSymbol->data->bNoMoreOpenByLossCnt = FALSE;

    perSymbol->data->Spec.nDecimalCnt           = nDecimalCnt;
    perSymbol->data->Spec.dPipSize              = dPipSize;
    perSymbol->data->Spec.nActiveSpreadPt       = nActiveSpreadPt;
    perSymbol->data->Spec.nThresholdOpenPt      = nThresholdOpenPt;
    perSymbol->data->Spec.nThresholdClosePt     = nThresholdClosePt;
    perSymbol->data->Spec.dOrderLots            = dOrderLots;
    perSymbol->data->Spec.nNoMoreOpenRoundCnt   = nNoMoreOpenRoundCnt;

    InitializeCriticalSection(perSymbol->cs);

    perSymbol->calc->Set_ISymbol(iSymbol);
    perSymbol->calc->Set_BroketCntTodo(m_nBrokerCntTodo);

    //
    m_vecDataPerSymbol.push_back(perSymbol);
    //

    m_nSymbolCntTodo++;

    return TRUE;
}


BOOL CDataHandler::Add_BrokerWhenLogin(char* pzBrokerKey, char* pzBrokerName)
{
    // Prevent adding more brokers than TO DO Count
    if (m_vecBroker.size() == m_nBrokerCntTodo)
        return FALSE;

    for (UINT i = 0; i < (INT) m_vecBroker.size(); i++)
    {
        // Already this broker has been added
        if (strcmp((m_vecBroker.at(i))->brokerKey, pzBrokerKey) == 0)
            return TRUE;
    }

    TBroker* p = new TBroker;
    ZeroMemory(p, sizeof(TBroker));
    
    strcpy(p->brokerKey, pzBrokerKey);
    strcpy(p->BrokerName, pzBrokerName);
    p->bStartMD = FALSE;
    
    m_vecBroker.push_back(p);
    LOGGING(INFO, TRUE, TRUE, "[%s]Logon.", pzBrokerKey);

    // CalCBestPrice 
    for (UINT iSymbol = 0; iSymbol < m_vecDataPerSymbol.size(); iSymbol++)
    {
        Calc(iSymbol)->AddBroker(pzBrokerKey);
    }

    return TRUE;
}

//// When brokers start sending MD, mark it.
//VOID CDataHandler::Mark_BrokerStartMD(char* pzBrokerKey)
//{
//    if (m_bTradeStart == TRUE)
//        return;
//
//    // All brokers must be added first by Add_BrokerWhenLogin
//    if (Get_BrokerCntCurrent() <  m_nBrokerCntTodo)
//        return;
//
//    int cnt1 = m_vecBroker.size();
//    int cnt2 = Calc(0)->GetBrokerCnt();
//    if ( cnt1 == cnt2 )
//    {
//        m_bTradeStart = TRUE;
//        for (int i = 0; i < (int)m_vecDataPerSymbol.size(); i++)
//            Calc(i)->AddBrokerDone();
//        
//        LOGGING(INFO, TRUE, TRUE, "[MARK_STARTMD] All Brokers have sent 1st Market Data. Start Trading.");
//    }
//    
//
//    //BOOL bFound = FALSE;
//
//    //// If this broker is already in the list, just exit
//    //int nMarkedCnt = 0;
//    //for (UINT i = 0; i < m_vecBroker.size(); i++)
//    //{
//    //    if (strcmp(m_vecBroker.at(i)->brokerKey, pzBrokerKey) == 0)
//    //    {
//    //        m_vecBroker.at(i)->bStartMD = TRUE;
//    //        bFound = TRUE;
//    //        nMarkedCnt++;
//    //    }
//    //    else
//    //    {
//    //        if (m_vecBroker.at(i)->bStartMD == TRUE)
//    //            nMarkedCnt++;
//    //    }
//    //}
//
//    //if (bFound)
//    //    LOGGING(-1,INFO, FALSE, FALSE, "[MARK_STARTMD] (BrokerKey:%s)", pzBrokerKey);
//    //else
//    //    LOGGING(INFO, FALSE, TRUE, "[MARK_STARTMD] Failed to find Broker (BrokerKey:%s)", pzBrokerKey);
//
//
//    //if (nMarkedCnt == Get_BrokerCntTodo())
//    //{
//    //    m_bTradeStart = TRUE;
//    //    LOGGING(INFO, FALSE, FALSE, "[MARK_STARTMD] All Brokers have sent 1st Market Data. Start Trading.");
//
//    //    //TODO
//    //    //for (int i = 0; i < (int)m_vecDataPerSymbol.size(); i++)
//    //    //    m_vecCalcBestPrc.at(i)->AddBroker(m_vecBroker);
//    //    //
//    //}
//}


/*
* char symbol[__ALPHA::LEN_SYMBOL+1];
    int         nOrdStatus;
    char        zDBSerial[32];          // Serial which is made when save data in DB. YYYYMMDD(8)+SEQNO(10)
    int         nCloseNetPLTriggered;   // calculated value
    int         nProfitCnt;
    int         nLossCnt;
    BOOL        bNoMoreOpen;
    TSymbolSpec Spec;
    TBestPrc    BestPrcForOpen;
    TPos        Long;
    TPos        Short;
*/
void CDataHandler::ResetData(int iSymbol)
{
    Data(iSymbol)->nOrdStatus = 0;
    ZeroMemory(Data(iSymbol)->zDBSerial, sizeof(Data(iSymbol)->zDBSerial));
    Data(iSymbol)->nCloseNetPLTriggered = 0;
    Data(iSymbol)->nProfitCnt = 0;
    Data(iSymbol)->nLossCnt = 0;
    //Data(iSymbol)->bNoMoreOpenByLossCnt = FALSE;
    Data(iSymbol)->lMagicNo = 0;

    ZeroMemory(&Data(iSymbol)->Spec, sizeof(Data(iSymbol)->Spec));
    ZeroMemory(&Data(iSymbol)->BestPrcForOpen, sizeof(Data(iSymbol)->BestPrcForOpen));
    ZeroMemory(&Data(iSymbol)->Long, sizeof(Data(iSymbol)->Long));
    ZeroMemory(&Data(iSymbol)->Short, sizeof(Data(iSymbol)->Short));

}


// Has opened positions for this symbol?
BOOL CDataHandler::Is_AlreadOpened(int iSymbol)
{
    return (Data(iSymbol)->nOrdStatus == ORDSTATUS_OPEN_MT4_2);
}


// To prevent caluclate and update the Best Price
BOOL CDataHandler::Should_CalcBestPrc(int iSymbol)
{
    if (Data(iSymbol)->nOrdStatus == ORDSTATUS_NONE)
        return TRUE;

    return FALSE;
}

//

int CDataHandler::CalcGap(int iSymbol, char* pzBidPrc, char* pzAskPrc)
{
    int nNewGap = 0;
    double dGap = atof(pzBidPrc) - atof(pzAskPrc);

    if (dGap != 0)
        nNewGap = (int)(dGap / Data(iSymbol)->Spec.dPipSize);

    return nNewGap;
}


//BOOL CDataHandler::BP_IsSame_PrevBroker_NewBroker(int iSymbol, char side, char* pzNewBrokerKey)
//{
//    BOOL bSame = FALSE;
//    if (side == __ALPHA::DEF_BUY)
//    {
//        if (strcmp(m_vecDataPerSymbol.at(iSymbol)->BestPrcForOpen.bidBrokerKey, pzNewBrokerKey) == 0)
//            bSame = TRUE;
//    }
//    else
//    {
//        if (strcmp(m_vecDataPerSymbol.at(iSymbol)->BestPrcForOpen.askBrokerKey, pzNewBrokerKey) == 0)
//            bSame = TRUE;
//    }
//    return bSame;
//
//}

/*
*
*   Update Position Info when receives reponse of open order from EA
* 
*/
VOID CDataHandler::Update_ResponseOfOpenOrder(int iSymbol, char* pzTicketNo, double dOpenPrc, double dLots, int nMT4Cmd, char* pzOpenTmMT4)
{
    TPos* p = NULL, *pOppPos = NULL;
    char zHead[128];
    BOOL bLong = FALSE;
    if (__ALPHA::IsBuyOrder(nMT4Cmd)) 
    {
        p       = &Data(iSymbol)->Long;
        pOppPos = &Data(iSymbol)->Short;
        strcpy(zHead, "LONG_RECV");
        bLong = TRUE;
    }
    else 
    {
        p       = &Data(iSymbol)->Short;
        pOppPos = &Data(iSymbol)->Long;
        strcpy(zHead, "SHORT_RECV");
    }
    
    char zPrc[32] = { 0 };
    FORMAT_PRC(dOpenPrc, Data(iSymbol)->Spec.nDecimalCnt, zPrc); // 000000012.12

    strcpy(p->zTicket, pzTicketNo);
    strcpy(p->zOpenPrc, zPrc);
    strcpy(p->zOpenTmMT4, pzOpenTmMT4);
    p->dLots = dLots;

    // calculate slippage
    double dPrcTriggered = atof(p->zOpenPrc_Triggered);
    if (bLong)
    {
        p->nOpenSlippage = (int)((dPrcTriggered- dOpenPrc) / Data(iSymbol)->Spec.dPipSize);
    }
    else
    {
        p->nOpenSlippage = (int)((dOpenPrc - dPrcTriggered) / Data(iSymbol)->Spec.dPipSize);
    }

    // UPDATE ORD STATUS
    Data(iSymbol)->nOrdStatus++;

    sprintf(m_zMsg, "[%s][%s](%5.5s)(iSymbol:%d)(OpenPrc_Triggered:%s,MT4:%s,Slippage:%d )(Lots:%.2f)(Ticket:%s)",
        zHead, Data(iSymbol)->symbol, p->zBrokerKey, iSymbol, p->zOpenPrc_Triggered, p->zOpenPrc, p->nOpenSlippage, p->dLots, pzTicketNo);
    LOGGING(INFO, TRUE, TRUE, m_zMsg);
}

VOID CDataHandler::Update_ResponseOfCloseOrder(
    int iSymbol, 
    char* pzTicketNo, 
    double dClosePrc, 
    double dLots, 
    int nMT4Cmd, 
    char* pzCloseTmMT4, 
    double dCmsn, 
    double dPL,
    double dSwap,
    char* pzDBLog
    )
{
    *pzDBLog = NULL;

    
    TPos* p = NULL;
    char zHead[128];
    BOOL bLong = FALSE;
    if (__ALPHA::IsBuyOrder(nMT4Cmd)) 
    {
        p = &Data(iSymbol)->Long;
        strcpy(zHead, "CLOSELONG_RECV");
        bLong = TRUE;
    }
    else 
    {
        p = &Data(iSymbol)->Short;
        strcpy(zHead, "CLOSESHORT_RECV");
    }

    char zClosePrc[32] = { 0 };
    FORMAT_PRC(dClosePrc, Data(iSymbol)->Spec.nDecimalCnt, zClosePrc); // 000000012.12
    strcpy(p->zClosePrc, zClosePrc);

    strcpy(p->zCloseTmMT4, pzCloseTmMT4);
    p->dPL = dPL;
    p->dCmsn = dCmsn;
    p->dSwap = dSwap;
    
    Data(iSymbol)->nRoundCnt++;

    // calculate slippage
    double dPrcTriggered = atof(p->zClosePrc_Triggered);
    if (bLong)
    {
        double dPLPt = (atof(zClosePrc) - atof(p->zOpenPrc)) / Data(iSymbol)->Spec.dPipSize;
        p->nPLPt = (int)dPLPt;
        p->nCloseSlippage = (int)((dClosePrc - dPrcTriggered ) / Data(iSymbol)->Spec.dPipSize);
    }
    else
    {
        double dPLPt = (atof(p->zOpenPrc) - atof(zClosePrc)) / Data(iSymbol)->Spec.dPipSize;
        p->nPLPt = (int)dPLPt;
        p->nCloseSlippage = (int)((dPrcTriggered - dClosePrc) / Data(iSymbol)->Spec.dPipSize);
    }

    sprintf(m_zMsg, "[%s][%s](%5.5s)(iSymbol:%d)(ClosePrc_Triggered:%s,MT4:%s)(Profit:%.2f)",
        zHead, Data(iSymbol)->symbol, p->zBrokerKey, iSymbol, p->zClosePrc_Triggered, zClosePrc, dPL);
    LOGGING(INFO, TRUE, TRUE, m_zMsg);


    // UPDATE ORD STATUS
    Data(iSymbol)->nOrdStatus++;

    // Increase ProiftCnt or LossCnt
    if (Data(iSymbol)->nOrdStatus == ORDSTATUS_CLOSE_MT4_2)
    {
        if (Data(iSymbol)->Long.nPLPt + Data(iSymbol)->Short.nPLPt > 0)
            Data(iSymbol)->nProfitCnt++;
        if (Data(iSymbol)->Long.nPLPt + Data(iSymbol)->Short.nPLPt < 0)
            Data(iSymbol)->nLossCnt++;
    }

}

BOOL CDataHandler::Is_NoMoreOpenByRoundCnt(int iSymbol)
{
    //0514-2
    BOOL bNoMoreOpen = (Data(iSymbol)->nRoundCnt == Data(iSymbol)->Spec.nNoMoreOpenRoundCnt);
    return bNoMoreOpen;
}


//0514-2
//BOOL CDataHandler::Check_Set_SymbolNoMoreOpen_by_LossCnt(int iSymbol, char* pzMsg)
//{
//    BOOL bSetNoMoreOpen = FALSE;
//
//    if (!Is_NoMoreOpenByRoundCnt(iSymbol))
//    {
//        if (Data(iSymbol)->nLossCnt == m_nMaxLossCnt)
//        {
//            Data(iSymbol)->bNoMoreOpenByLossCnt = TRUE;
//            sprintf(pzMsg, "[%s] NoMoreOpen - LossCnt(%d) = MaxLossCnt(%d)", Data(iSymbol)->symbol, Data(iSymbol)->nLossCnt, m_nMaxLossCnt);
//            bSetNoMoreOpen = TRUE;
//        }
//    }
//    return bSetNoMoreOpen;
//}


// Begin called from outer thread (Iocp::CheckNoMoreTimeThread)
// return TRUE : NO MORE OPEN
BOOL CDataHandler::Check_Set_NoMoreOpen_by_Time(char* pzMsg)
{
    BOOL bSetNoMoreOpen = FALSE;

    if (!m_bNoMoreOpen)
    {
        SYSTEMTIME st;
        char zTm[32];
        GetLocalTime(&st);
        sprintf(zTm, "%02d:%02d", st.wHour, st.wMinute);

        if (strcmp(zTm, m_zNoMoreOpenTimeBegin) >= 0 &&
            strcmp(zTm, m_zNoMoreOpenTimeEnd) <= 0
            )
        {
            m_bNoMoreOpen = TRUE;
            bSetNoMoreOpen = TRUE;
            sprintf(pzMsg, "NoMoreOpen Now(%s) is NoMoreOpenTime(%s - %s)", zTm, m_zNoMoreOpenTimeBegin, m_zNoMoreOpenTimeEnd);
        }
    }
    return bSetNoMoreOpen;
}



// Begin called from outer thread (Iocp::CheckNoMoreTimeThread)
// return TRUE : Resume
BOOL CDataHandler::Check_ResumeTrade_by_Time(char* pzMsg)
{
    BOOL bResume = FALSE;
    if (m_bNoMoreOpen)
    {
        SYSTEMTIME st;
        char zTm[32];
        GetLocalTime(&st);
        sprintf(zTm, "%02d:%02d", st.wHour, st.wMinute);

        if (strcmp(zTm, m_zNoMoreOpenTimeBegin) < 0 ||
            strcmp(zTm, m_zNoMoreOpenTimeEnd) > 0
            )
        {
            m_bNoMoreOpen = FALSE;
            bResume = TRUE;
            //for (UINT i = 0; i < m_vecDataPerSymbol.size(); i++)
            //{
            //    LockMD(i);
            //    ResetData(i);
            //    UnlockMD(i);
            //}
            sprintf(pzMsg, "ResumeByTime.Now(%s) is before NoMoreOpenBegin(%s) OR after NoMoreOpenEnd(%s)",
                zTm, m_zNoMoreOpenTimeBegin, m_zNoMoreOpenTimeEnd);
        }
    }
    return bResume;
}

//0524-2
VOID CDataHandler::Set_NoMoreOpen_Time(char* pzNoMoreTimeBegin, char* pzNoMoreTimeEnd)
{
    //0524-2 m_nMaxLossCnt = nMaxLossCnt;
    strcpy(m_zNoMoreOpenTimeBegin, pzNoMoreTimeBegin);
    strcpy(m_zNoMoreOpenTimeEnd, pzNoMoreTimeEnd);
    
}


BOOL CDataHandler::Is_HedgePositionRejected(int iSymbol, int nMyMT4Cmd)
{
    if (__ALPHA::IsBuyOrder(nMyMT4Cmd))
        return (Data(iSymbol)->Short.bRejected == true);

    return (Data(iSymbol)->Long.bRejected == true);
}

void CDataHandler::Mark_Rejected(int iSymbol, int nMT4Cmd)
{
    //0514-3
    //if (__ALPHA::IsBuyOrder(nMT4Cmd))
    //    Data(iSymbol)->Short.bRejected = true;
    //else
    //    Data(iSymbol)->Long.bRejected = true;

    if (__ALPHA::IsBuyOrder(nMT4Cmd))
        Data(iSymbol)->Long.bRejected = true;
    else
        Data(iSymbol)->Short.bRejected = true;
}


void CDataHandler::Update_LatestMarketData(int iSymbol, char* pzBrokerKey, double dNewBid, double dNewAsk, double dSpread, char* pzMDTime)
{
    char zNewBid[32], zNewAsk[32];
    
    FORMAT_PRC(dNewBid, Data(iSymbol)->Spec.nDecimalCnt, zNewBid); // 000000012.12
    FORMAT_PRC(dNewAsk, Data(iSymbol)->Spec.nDecimalCnt, zNewAsk); // 000000012.12

    if (__ALPHA::RET_ERR == Calc(iSymbol)->Update_LatestMarketData(pzBrokerKey, zNewBid, zNewAsk, dSpread, pzMDTime))
    {
        LOGGING(ERR, TRUE, TRUE, Calc(iSymbol)->GetMsg());
    }
}


void CDataHandler::Calc_BestPrc_OpenOrd(int iSymbol, /*char* pzSymbol,*/ double dNewBid, double dNewAsk, double dSpread, char* pzNewBrokerKey, char* pzNewBrokerName, _Out_ BOOL* o_pbMustSendOrder)
{
    *o_pbMustSendOrder = FALSE;
    
    // Don't update after positions have already opened.
    if (Is_AlreadOpened(iSymbol))
        return;
    
    if (FALSE == Should_CalcBestPrc(iSymbol))
        return;

    if (FALSE == Calc(iSymbol)->Check_AllBrokers_SendFirstMD())
        return;

    BOOL bBidUpdate = FALSE, bAskUpdate = FALSE;

    const BOOL INCREASED = TRUE, REDUCED = FALSE;;


    // price formating
    char zSymbol[32];
    //char zNewBid[32], zNewAsk[32];
    
    strcpy(zSymbol, Data(iSymbol)->symbol);

    // Check if all brokers have sent MD at least one time.
    //if (FALSE == Is_ReadyTrade())
    //    return;

    char zBestBid[32] = { 0 };
    char zBestAsk[32] = { 0 };
    char zBestBidBroker[32] = { 0 };
    char zBestAskBroker[32] = { 0 };
    char zBestBidOpposite[32] = { 0 };
    char zBestAskOpposite[32] = { 0 };
    char zLastBidMDTime[32] = { 0 };
    char zLastAskMDTime[32] = { 0 };
    double dBidderSpread = 0, dAskerSpread = 0;

    ////////////////////////////////////////////////////////////////////////////
    // Compute Best Price from all brokers' last market data
    __ALPHA::EN_RET_VAL retVal = Calc(iSymbol)->CalcBestPrc(
        zBestBid
        , zBestBidBroker
        , zBestBidOpposite
        , &dBidderSpread
        , zLastBidMDTime
        , zBestAsk
        , zBestAskBroker
        , zBestAskOpposite
        , &dAskerSpread
        , zLastAskMDTime
    );
    if (retVal == __ALPHA::RET_ERR)
    {
        LOGGING(ERR, TRUE, TRUE, Calc(iSymbol)->GetMsg());
        return;
    }
    else if (retVal == __ALPHA::RET_SKIP)
        return;
    //
    ////////////////////////////////////////////////////////////////////////////

    
    // Save Best Prices and Gap
    strcpy(Data(iSymbol)->BestPrcForOpen.bidBrokerKey, zBestBidBroker);
    strcpy(Data(iSymbol)->BestPrcForOpen.bidPrc,       zBestBid);
    strcpy(Data(iSymbol)->BestPrcForOpen.bidOpposite,  zBestBidOpposite);

    strcpy(Data(iSymbol)->BestPrcForOpen.askBrokerKey, zBestAskBroker);
    strcpy(Data(iSymbol)->BestPrcForOpen.askPrc,       zBestAsk);
    strcpy(Data(iSymbol)->BestPrcForOpen.askOpposite,  zBestAskOpposite);
    
    Data(iSymbol)->BestPrcForOpen.nGapBidAsk = CalcGap(iSymbol, Data(iSymbol)->BestPrcForOpen.bidPrc, Data(iSymbol)->BestPrcForOpen.askPrc);

    //if (Data(iSymbol)->BestPrcForOpen.nGapBidAsk > 20)
    if(++g_LogCntEstm>LOG_CNT)
    {
        g_LogCntEstm = 0;
        LOGGING(INFO, FALSE, TRUE,
            "[%s](Gap:%d)(Threshold:%d)(BestBid:%s,%5.5s)(BestAsk:%s,%5.5s)"
            "(BestBidOpp(Ask):%s)(BestAskOpp(Bid):%s)(Bidder Spread:%.2f)(Asker Spread:%.2f)"
            , Data(iSymbol)->symbol
            , Data(iSymbol)->BestPrcForOpen.nGapBidAsk
            , Data(iSymbol)->Spec.nThresholdOpenPt
            , Data(iSymbol)->BestPrcForOpen.bidPrc
            , Data(iSymbol)->BestPrcForOpen.bidBrokerKey
            , Data(iSymbol)->BestPrcForOpen.askPrc
            , Data(iSymbol)->BestPrcForOpen.askBrokerKey
            , zBestBidOpposite
            , zBestAskOpposite
            , dBidderSpread
            , dAskerSpread
        );
    }



    //
    // if condition is enough, PLACE OPEN ORDER !!!
    //

    BOOL bRes = Is_UnderActiveSpread(
        iSymbol,
        Data(iSymbol)->BestPrcForOpen.bidBrokerKey,
        dBidderSpread,
        Data(iSymbol)->BestPrcForOpen.askBrokerKey,
        dAskerSpread
    );
    if (bRes)
        *o_pbMustSendOrder = OpenOrder_Check_Place(iSymbol, zSymbol, zLastBidMDTime, zLastAskMDTime);  // , pzNewBrokerKey, zNewBid, zNewAsk);
}


/*
*   BestBid, BestAsk 둘 중 하나가 갱신되어 GAP 이 커져서, COST 를 넘어서는 경우 주문한다.
* 
*   return TRUE : Must send order
*/
BOOL CDataHandler::OpenOrder_Check_Place(int iSymbol, char* pzSymbol, char* pzLastBidTime, char* pzLastAskTime)    //, char* pzNewBrokerKey, char* pzNewBid, char* pzNewAsk)
{
    // if MaxGap not exists
    if (Data(iSymbol)->BestPrcForOpen.nGapBidAsk == 0)
        return FALSE;

    //
    // Gap is less than cost
    //
    //if (m_vecDataPerSymbol.at(iSymbol)->BestPrcForOpen.nGapBidAsk < m_vecDataPerSymbol.at(iSymbol)->Spec.nCostPt + m_vecDataPerSymbol.at(iSymbol)->Spec.nThresholdOpenPt)
    if (!IsGap_BiggerThan_OpenThreshold(iSymbol))
        return FALSE;


    Data(iSymbol)->lMagicNo = m_magicNo.getMagicNo();

    strcpy(Data(iSymbol)->Long.zBrokerKey,          Data(iSymbol)->BestPrcForOpen.askBrokerKey);
    strcpy(Data(iSymbol)->Long.zOpenPrc_Triggered,  Data(iSymbol)->BestPrcForOpen.askPrc);
    strcpy(Data(iSymbol)->Long.zMDTime_OpenPrcTriggered, pzLastAskTime);
    strcpy(Data(iSymbol)->Long.zOpenPrc,            Data(iSymbol)->BestPrcForOpen.askPrc);
    strcpy(Data(iSymbol)->Long.zOpenOppPrc,         Data(iSymbol)->BestPrcForOpen.askOpposite);
    Data(iSymbol)->Long.dLots = Data(iSymbol)->Spec.dOrderLots;
    Data(iSymbol)->Long.nPLPt = 0;

    strcpy(Data(iSymbol)->Short.zBrokerKey,         Data(iSymbol)->BestPrcForOpen.bidBrokerKey);
    strcpy(Data(iSymbol)->Short.zOpenPrc_Triggered, Data(iSymbol)->BestPrcForOpen.bidPrc);
    strcpy(Data(iSymbol)->Short.zMDTime_OpenPrcTriggered, pzLastBidTime);
    strcpy(Data(iSymbol)->Short.zOpenPrc,           Data(iSymbol)->BestPrcForOpen.bidPrc);
    strcpy(Data(iSymbol)->Short.zOpenOppPrc,        Data(iSymbol)->BestPrcForOpen.bidOpposite);
    Data(iSymbol)->Short.dLots = Data(iSymbol)->Spec.dOrderLots;
    Data(iSymbol)->Short.nPLPt = 0;


    //
    // Place Open Order - Buy, Sell both.
    //
    LOGGING(INFO, TRUE, TRUE,
        "[OPEN ORDER-1][%s][Gap:%d] >= [ThresholdPL=%d]",
        pzSymbol,
        Data(iSymbol)->BestPrcForOpen.nGapBidAsk,
        Data(iSymbol)->Spec.nThresholdOpenPt
    );
    LOGGING(INFO, TRUE, TRUE,
        "[OPEN ORDER-2][BUY ](%5.5s) OpenPrc(ASK):%s, OppPrc(BID):%s, MDTime:%s",
        Data(iSymbol)->Long.zBrokerKey,
        Data(iSymbol)->Long.zOpenPrc,
        Data(iSymbol)->Long.zOpenOppPrc,
        Data(iSymbol)->Long.zMDTime_OpenPrcTriggered
    );
    LOGGING(INFO, TRUE, TRUE,
        "[OPEN ORDER-3][SELL](%5.5s) OpenPrc(BID):%s, OppPrc(ASK):%s, MDTime:%s",
        Data(iSymbol)->Short.zBrokerKey,
        Data(iSymbol)->Short.zOpenPrc,
        Data(iSymbol)->Short.zOpenOppPrc,
        Data(iSymbol)->Short.zMDTime_OpenPrcTriggered
    );

    return TRUE;
}

int CDataHandler::Get_SymbolCntTodo()
{
    return m_nSymbolCntTodo;
}


void CDataHandler::MarketClose(int iSymbol, BOOL* bMustSendOrder)
{
    *bMustSendOrder = FALSE;
    if(!Is_AlreadOpened(iSymbol))
        return;


    Calc_EstmPL_CloseOrder(iSymbol, /*dNewBid*/0, /*dNewAsk*/0, /*pzNewBrokerKey*/NULL, /*bBatchClose*/TRUE, bMustSendOrder);
}

void CDataHandler::Calc_EstmPL_CloseOrder(int iSymbol, /*char* pzSymbol,*/ double dNewBid, double dNewAsk, char* pzNewBrokerKey, BOOL bBatchClose, _Out_ BOOL* pbMustSendOrder)
{
    char zSymbol[32];
    BOOL bSameBroker = FALSE;
    
    strcpy(zSymbol, Data(iSymbol)->symbol);
    *pbMustSendOrder = FALSE;
    
    char zLongCurrBid[32] = { 0 }, zLongCurrAsk[32] = { 0 };
    char zShortCurrBid[32] = { 0 }, zShortCurrAsk[32] = { 0 };
    char zLongLastMDTime[32] = { 0 }, zShortLastMDTime[32] = { 0 };

    double dLongCurrBid = 0;
    double dLongCurAsk = 0;
    
    double dShortCurrBid = 0;
    double dShortCurrAsk = 0;
    
    double dLongOpenPrc = 0;
    double dShortOpenPrc = 0;
    
    double dLongPL = 0;
    double dShortPL = 0;

    if (!bBatchClose)
    {
        if (dNewBid <= 0 || dNewAsk <= 0)
            return;

        if (strcmp(Data(iSymbol)->Long.zBrokerKey, pzNewBrokerKey) != 0 &&
            strcmp(Data(iSymbol)->Short.zBrokerKey, pzNewBrokerKey) != 0)
        {
            return;
        }
    }


    if (!bBatchClose)
    {
        if (!Calc(iSymbol)->GetLastBidAsk(Data(iSymbol)->Long.zBrokerKey, zLongCurrBid, zLongCurrAsk, zLongLastMDTime))
            return;

        dLongCurrBid = atof(zLongCurrBid);
        dLongCurAsk = atof(zLongCurrAsk);

        if (!Calc(iSymbol)->GetLastBidAsk(Data(iSymbol)->Short.zBrokerKey, zShortCurrBid, zShortCurrAsk, zShortLastMDTime))
            return;
        dShortCurrBid = atof(zShortCurrBid);
        dShortCurrAsk = atof(zShortCurrAsk);


        // Long 과 Short 손익을 계산한다.
        if (dLongCurrBid == 0 || dLongCurAsk == 0 ||
            dShortCurrBid == 0 || dShortCurrAsk == 0)
        {
            return;
        }
    }

    if ( !Calc(iSymbol)->Is_LaterThanCutOffTime(zLongLastMDTime) && !Calc(iSymbol)->Is_LaterThanCutOffTime(zShortLastMDTime))
        return;


    dLongOpenPrc = atof(Data(iSymbol)->Long.zOpenPrc);
    dLongPL = (dLongCurrBid - dLongOpenPrc) / Data(iSymbol)->Spec.dPipSize;

    dShortOpenPrc = atof(Data(iSymbol)->Short.zOpenPrc);
    dShortPL = (dShortOpenPrc - dShortCurrAsk) / Data(iSymbol)->Spec.dPipSize;

    if (!bBatchClose)
    {
        if (dLongPL + dShortPL <= 0)
            return;
    }
    int nLongPL = (int)dLongPL;
    int nShortPL = (int)dShortPL;
    int nNetPL = (int)(dLongPL + dShortPL);
    
    BOOL bClose = IsNetPL_BiggerThan_CloseThreshold(iSymbol, nNetPL);
    
    if (bBatchClose || (!bBatchClose && bClose))
    {
        char zHead[128];
        Data(iSymbol)->nCloseNetPLTriggered = nNetPL;

        strcpy(Data(iSymbol)->Long.zClosePrc, zLongCurrBid);
        strcpy(Data(iSymbol)->Long.zClosePrc_Triggered, zLongCurrBid);
        strcpy(Data(iSymbol)->Long.zMDTime_ClosePrcTriggered, zLongLastMDTime);

        strcpy(Data(iSymbol)->Short.zClosePrc, zShortCurrAsk);
        strcpy(Data(iSymbol)->Short.zClosePrc_Triggered, zShortCurrAsk);
        strcpy(Data(iSymbol)->Short.zMDTime_ClosePrcTriggered, zShortLastMDTime);

        *pbMustSendOrder = TRUE;    // To Send Order to EAs


        if (bBatchClose)    strcpy(zHead, "CLOSE ORDER-1(BatchClose)");
        else                 strcpy(zHead, "CLOSE ORDER-1");
        LOGGING(INFO, TRUE, TRUE, "[%s][%s][NET:%d]>=[ThresholdPL=%d]"
            , zHead
            , zSymbol
            , nNetPL
            , Data(iSymbol)->Spec.nThresholdClosePt
        );


        if (bBatchClose)    strcpy(zHead, "CLOSE ORDER-2(BatchClose)");
        else                 strcpy(zHead, "CLOSE ORDER-2");
        LOGGING(INFO, TRUE, TRUE, "[%s][LONG-SELL ORDER](%5.5s) CURR(BID):%s - OPEN:%s = (PLPt:%d)(Time:%s)"
            , zHead
            , Data(iSymbol)->Long.zBrokerKey
            ,zLongCurrBid
            , Data(iSymbol)->Long.zOpenPrc
            ,nLongPL
            , zLongLastMDTime
        );


        if (bBatchClose)    strcpy(zHead, "CLOSE ORDER-3(BatchClose)");
        else                 strcpy(zHead, "CLOSE ORDER-3");
        LOGGING(INFO, TRUE, TRUE, "[%s][SHORT-BUY ORDER](%5.5s) OPEN:%s - CURR(ASK):%s = (PLPt:%d)(Time:%s)"
            , zHead
            ,Data(iSymbol)->Short.zBrokerKey
            , Data(iSymbol)->Short.zOpenPrc
            , zShortCurrAsk
            , nShortPL
            , zShortLastMDTime
        );
        
    }
    
    if(!bBatchClose && !bClose)
    {
        if (++g_LogCntEstm > LOG_CNT) {
            LOGGING(INFO, FALSE, TRUE, "[ESTM_PL][%s][ThresholdPL=%d][NET:%d]"
                "[LONG-%5.5s.(CURR:%s)-(OPEN:%s)=(PLPt:%d)]"
                "[SHORT-%5.5s.(OPEN:%s)-(CURR:%s)=(PLPt:%d)]"
                ,
                zSymbol,
                Data(iSymbol)->Spec.nThresholdClosePt,
                nNetPL,
                Data(iSymbol)->Long.zBrokerKey,
                zLongCurrBid,
                Data(iSymbol)->Long.zOpenPrc,
                nLongPL,
                Data(iSymbol)->Short.zBrokerKey,
                Data(iSymbol)->Short.zOpenPrc,
                zShortCurrAsk,
                nShortPL
            );
            g_LogCntEstm = 0;
        }
    }
}

BOOL CDataHandler::IsNetPL_BiggerThan_CloseThreshold(int iSymbol, int nNetPLPt)
{
    int nThreshold = Data(iSymbol)->Spec.nThresholdClosePt;
    return (nNetPLPt >= nThreshold);
}

BOOL CDataHandler::IsGap_BiggerThan_OpenThreshold(int iSymbol)
{
    return (Data(iSymbol)->BestPrcForOpen.nGapBidAsk >= Data(iSymbol)->Spec.nThresholdOpenPt);
}



BOOL CDataHandler::Is_UnderActiveSpread(int iSymbol, char* pzBidBrokerKey, double dBidderSpread, char* pzAskBrokerKey, double dAskerSpread)
{
    if (Data(iSymbol)->Spec.nActiveSpreadPt < (int)dBidderSpread)
        return FALSE;

    if (Data(iSymbol)->Spec.nActiveSpreadPt < (int)dAskerSpread)
        return FALSE;

    return TRUE;
}


BOOL CDataHandler::Is_Condition_ProfitCut()
{
    int nTotlPL = 0;
    for (UINT i = 0; i < m_vecDataPerSymbol.size(); i++)
    {
        nTotlPL += Is_Condition_ProfitCut_Inner(i);
    }

    if (nTotlPL >= m_nProfitCutThreshold)
    {
        m_bProfitCutTriggered = TRUE;
        sprintf(m_zMsg, "[ProfitCut] Current Total Estimated PL (%d) >= ProfitCut Threshold(%d)", nTotlPL, m_nProfitCutThreshold);
        LOGGING(INFO, TRUE, TRUE, m_zMsg);
        g_log.SendAlermBoth(m_zMsg);
    }
    return  (nTotlPL >= m_nProfitCutThreshold);
}

int CDataHandler::Is_Condition_ProfitCut_Inner(int iSymbol)
{
    char zSymbol[32];
    strcpy(zSymbol, Data(iSymbol)->symbol);
    

    char zLongCurrBid[32] = { 0 }, zLongCurrAsk[32] = { 0 };
    char zShortCurrBid[32] = { 0 }, zShortCurrAsk[32] = { 0 };
    char zLastBidTime[32] = { 0 }, zLastAskTime[32] = { 0 };

    double dLongCurrBid = 0;
    double dShortCurrAsk = 0;

    double dLongOpenPrc = 0;
    double dShortOpenPrc = 0;

    double dLongPL = 0;
    double dShortPL = 0;

    if (!Calc(iSymbol)->GetLastBidAsk(Data(iSymbol)->Long.zBrokerKey, zLongCurrBid, zLongCurrAsk, zLastBidTime))
        return 0;

    if (!Calc(iSymbol)->GetLastBidAsk(Data(iSymbol)->Short.zBrokerKey, zShortCurrBid, zShortCurrAsk, zLastAskTime))
        return 0;

    dLongCurrBid = atof(zLongCurrBid);
    dShortCurrAsk = atof(zShortCurrAsk);


    if (dLongCurrBid == 0 || dShortCurrAsk == 0)
    {
        return 0;
    }

    dLongOpenPrc = atof(Data(iSymbol)->Long.zOpenPrc);
    dLongPL = (dLongCurrBid - dLongOpenPrc) / Data(iSymbol)->Spec.dPipSize;

    dShortOpenPrc = atof(Data(iSymbol)->Short.zOpenPrc);
    dShortPL = (dShortOpenPrc - dShortCurrAsk) / Data(iSymbol)->Spec.dPipSize;

    int nNetPL = (int)(dLongPL + dShortPL);

    return nNetPL;
}
