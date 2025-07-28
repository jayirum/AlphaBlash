#include "ArbiLatency.h"
#include "../../Common/AlphaInc.h"
#include "../../CommonAnsi/LogMsg.h"
#include "../../CommonAnsi/Util.h"
#include "../../CommonAnsi/TimeUtils.h"
#include "../../Common/AlphaInc.h"
#include "../../Common/AlphaProtocolUni.h"
#include "Inc.h"



extern  CConfig g_config;
extern  TCHAR	g_zConfig[_MAX_PATH];
extern  TCHAR	g_zLogDir[_MAX_PATH];


CArbiLatency::CArbiLatency()
{
 /*   m_symbolInfo.dPtsSize = 0;
    m_nTargetPLOpen = 0;
    m_nMaxTraceCnt = 1;
    m_dVol = 0;*/
}

CArbiLatency::~CArbiLatency()
{
    StopThread();

    Remove_CurrMDList();

    delete m_pManagePos;
}

BOOL CArbiLatency::Initialize(_In_ TSymbolInfo* pInfo, const UINT unThreadOrderBest, const UINT unThreadOrderWorst)
{
    CopyMemory(&m_symbolInfo, pInfo, sizeof(TSymbolInfo));
    //m_symbolInfo.dPtsSize                = atof(sPointSize.c_str());
    //m_nTargetPLOpen             = atoi(sNetProfitPtsOpen.c_str());
    //m_nTargetPLClose            = atoi(sNetProfitPtsClose.c_str());
    //m_nMaxTraceCnt              = atoi(sTraceCnt.c_str());
    //m_dVol                      = atof(sVol.c_str());
    //m_nSlippagePts              = atoi(sSlippagePts.c_str());
    //strcpy(m_zTradeTimeFrom, pzTimeFrom);
    //strcpy(m_zTradeTimeTo, pzTimeTo);
    m_unThreadOrderBest         = unThreadOrderBest;
    m_unThreadOrderWorst        = unThreadOrderWorst;

    m_pManagePos = new CManagePos();
    m_pManagePos->UpdateSymbolInfo(pInfo);

    CBaseThread::ResumeThread();

    return TRUE;
}

VOID CArbiLatency::Execute(char* pCode, char* pzRecvData)
{
    //TRecvData* pData = new TRecvData(pzRecvData);
    string* pData = new string(pzRecvData);

    if( strcmp(pCode, __ALPHA::CODE_POSITION)==0)
        PostThreadMessage(m_dwThreadID, WM_POSITION_DATA, (WPARAM)pData->size(), (LPARAM)pData);
    if (strcmp(pCode, __ALPHA::CODE_MARKET_DATA)==0)
        PostThreadMessage(m_dwThreadID, WM_MARKET_DATA, (WPARAM)pData->size(), (LPARAM)pData);

}



void CArbiLatency::ThreadFunc()
{
    while (m_bContinue)
    {
        Sleep(1);
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            BOOL bDelete = TRUE;
            if (msg.message == WM_MARKET_DATA)
            {
                MarketDataProc((string*)msg.lParam);
            }
            else if (msg.message == WM_POSITION_DATA)
            {
                PosDataProc((string*)msg.lParam);
            }
            else if (msg.message == WM_CONFIG_UPDATED)
            {
                Update_Config();
            }
            delete (string*)msg.lParam;
        } // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    } // while(pThis->m_bRun)
    
    return ;
}

void CArbiLatency::Update_Config()
{
    string symbol = m_symbolInfo.sSymbol;
    g_config.getSymbolInfo(symbol, &m_symbolInfo);

    m_pManagePos->UpdateSymbolInfo(&m_symbolInfo);
}

void CArbiLatency::PosDataProc(const string* pData)
{
    m_sRawData = *pData;

    CProtoGet get;
    if (!get.ParsingWithHeader(pData->c_str(), pData->size()))
    {
        LOGGING(ERR, TRUE, "[%s]PosDataProc ParsingWithHeader Error(%s)(%s)", m_symbolInfo.sSymbol.c_str(), get.GetMsg(), pData->c_str());
        return;
    }


    char zBrokerKey[128] = { 0 };// , zSymbol[128] = { 0 };
    char zOrderSide[32] = { 0 };
    double dOpenPrc = 0, dLots = 0;
    int nOrderCmd = 0;
    int nTicket = 0;

    get.GetVal(FDS_KEY, zBrokerKey);
    get.GetVal(FDS_ORD_SIDE, zOrderSide);
    nTicket = get.GetValN(FDN_TICKET);
    dOpenPrc = get.GetValD(FDD_OPEN_PRC);
    dLots = get.GetValD(FDD_LOTS);
    get.GetValN(nOrderCmd);

    if (nTicket <= 0 || dOpenPrc<=0 || dLots<=0 )
    {
        LOGGING(ERR, TRUE, "Pos Info is NOT correct(%s)(Ticket:%d)(OpenPRc:%.5f)(Lots:%d)",
            m_symbolInfo.sSymbol.c_str(), nTicket, dOpenPrc, dLots);
        return;
    }


    if (m_pManagePos->Is_PositionOpened())
    {
        LOGGING(ERR, TRUE, "(%s)(%s) position exists already", zBrokerKey, m_symbolInfo.sSymbol.c_str());
    }
    else
    {
        m_pManagePos->AddOpenPos(zBrokerKey, nTicket, zOrderSide[0], dOpenPrc, dLots);
    }
}

void CArbiLatency::MarketDataProc(const string* pData)
{
    m_sRawData = *pData;

    CProtoGet get;
    if (!get.ParsingWithHeader(pData->c_str(), pData->size()))
    {
        LOGGING(ERR, TRUE, "[%s]ParsingWithHeader Error(%s)(%s)", m_symbolInfo.sSymbol.c_str(), get.GetMsg(), pData->c_str());
        return;
    }

    // 2022.11.04 05:10:31.641	ArbiLatencyWithClosing (GBPUSD,H1)	After Place OpenOrder(NZDCHF)(S)(Ticket:14602606)(Magic:15296)(RecvPrc:0.58519)(OrdPrc:0.000000)(Bid:0.584520)(Ask:0.585410)(MDTime:2022.11.04 00:10:32)

    char zBrokerKey[128] = { 0 };// , zSymbol[128] = { 0 };
    char zMDTime[32] = { 0 };
    double dBid, dAsk, dSpread;

    get.GetVal(FDS_KEY, zBrokerKey);
    get.GetVal(FDS_MARKETDATA_TIME, zMDTime);  // MDTime:2022.11.04 00:10:32
    dBid = get.GetValD(FDD_BID);
    dAsk = get.GetValD(FDD_ASK);
    dSpread = get.GetValD(FDD_SPREAD);

    if (dBid <= 0 || dAsk <= 0)
    {
        LOGGING(ERR, TRUE, "MD is NOT correct(%s)(Symbol:%s)(Time:%s)(Bid:%.5f)(Ask:%.5f)",
            zBrokerKey, m_symbolInfo.sSymbol.c_str(), zMDTime, dBid, dAsk);
        return;
    }

    char zMT4Time[32];
    sprintf(zMT4Time, "%.8s", zMDTime + 11);
    if (strcmp(g_config.getTradeTimeFrom(), zMT4Time) > 0 || strcmp(zMT4Time, g_config.getTradeTimeTo()) > 0)    //02:00~23:30
        return;


    if (m_pManagePos->Is_PositionOpened())
    {
        Calc_Profits(zBrokerKey, dBid, dAsk);
        
        //
        return;
        //
    }

    Update_CurrMD(zBrokerKey, dBid, dAsk, dSpread, zMDTime);

    if (m_vecMD.size() < DEF_BROKER_CNT)
    {
        return;
    }


    if (Check_Triggering())
    {
        Send_OpenOrder();

        Print_Triggered();

        // RESET
        Remove_CurrMDList();
        return;
    }
    else
    {

    }
}


BOOL CArbiLatency::Check_Triggering()
{
    m_triggered.Reset();

    int nBestIdx = -1, nWorstIdx = -1;
    int nGapBid = 0, nGapAsk = 0;
    BOOL bTriggeredBid = FALSE, bTriggeredAsk = FALSE;

    TTriggered* pBid = &m_tBid; pBid->Reset();
    TTriggered* pAsk = &m_tAsk; pAsk->Reset();

    //
    // BID for SELL (Higher is best)
    //
    double dGap = m_vecMD.at(0)->dBid - m_vecMD.at(1)->dBid;    
    if (dGap != 0)
    {
        if (dGap > 0)   // 100 - 90
        {
            nBestIdx = 0;
            nWorstIdx = 1;
        }
        else            // 90 - 100
        {
            nBestIdx = 1;
            nWorstIdx = 0;
        }

        pBid->dBestPrc  = m_vecMD.at(nBestIdx)->dBid;
        pBid->dWorstPrc = m_vecMD.at(nWorstIdx)->dBid;
        pBid->dOppPrc   = m_vecMD.at(nWorstIdx)->dAsk;

        pBid->nGapPts   = (int)((pBid->dBestPrc - pBid->dOppPrc) / m_symbolInfo.dPtsSize);
        pBid->nNetPLPts = pBid->nGapPts - m_symbolInfo.nSlippagePts;   

        if (pBid->nNetPLPts >= m_symbolInfo.nTargetPtsOpening)
        {
            pBid->enBidAsk = E_BID;

            pBid->sBest_Broker  = m_vecMD.at(nBestIdx)->sBroker;
            pBid->sBest_Time    = m_vecMD.at(nBestIdx)->sMDTime;
            pBid->dBestSpread   = m_vecMD.at(nBestIdx)->dSpread;

            pBid->sWorst_Broker = m_vecMD.at(nWorstIdx)->sBroker;
            pBid->sWorst_Time   = m_vecMD.at(nWorstIdx)->sMDTime;
            pBid->dWorstSpread  = m_vecMD.at(nWorstIdx)->dSpread;

            bTriggeredBid = TRUE;
        }
    }

    //
    // ASK for Buy (Lower is best)
    //
    dGap = m_vecMD.at(0)->dAsk - m_vecMD.at(1)->dAsk;
    if (dGap != 0)
    {
        if (dGap < 0)           // 90 - 100
        {
            nBestIdx = 0;
            nWorstIdx = 1;
        }
        else                    // 100 - 90
        {
            nBestIdx = 1;
            nWorstIdx = 0;
        }

        pAsk->dBestPrc  = m_vecMD.at(nBestIdx)->dAsk;
        pAsk->dWorstPrc = m_vecMD.at(nWorstIdx)->dAsk;
        pAsk->dOppPrc   = m_vecMD.at(nWorstIdx)->dBid;

        pAsk->nGapPts   = (int)((pAsk->dOppPrc - pAsk->dBestPrc) / m_symbolInfo.dPtsSize);
        pAsk->nNetPLPts = pAsk->nGapPts - m_symbolInfo.nSlippagePts;

        if (pAsk->nNetPLPts >= m_symbolInfo.nTargetPtsOpening)
        {
            pAsk->enBidAsk = E_ASK;

            pAsk->sBest_Broker  = m_vecMD.at(nBestIdx)->sBroker;
            pAsk->sBest_Time    = m_vecMD.at(nBestIdx)->sMDTime;
            pAsk->dBestSpread   = m_vecMD.at(nBestIdx)->dSpread;

            pAsk->sWorst_Broker = m_vecMD.at(nWorstIdx)->sBroker;
            pAsk->sWorst_Time   = m_vecMD.at(nWorstIdx)->sMDTime;
            pAsk->dWorstSpread  = m_vecMD.at(nWorstIdx)->dSpread;

            bTriggeredAsk = TRUE;
        }
    }

    if (!bTriggeredBid && !bTriggeredAsk)
        return FALSE;

    if (bTriggeredBid && bTriggeredAsk)
    {
        if (pBid->nNetPLPts > pAsk->nNetPLPts)
            bTriggeredAsk = FALSE;
        else
            bTriggeredBid = FALSE; 
    }

    if (bTriggeredBid)   memcpy(&m_triggered, pBid, sizeof(TTriggered));
    if (bTriggeredAsk)   memcpy(&m_triggered, pAsk, sizeof(TTriggered));

    m_triggered.Set_BidAskStr();
    CTimeUtils::LocalTime_Full_WithDot(m_triggered.zLocalTime);


    return TRUE;
}

TCurrMD* CArbiLatency::Update_CurrMD(const char* pzBroker, const double dBid, const double dAsk, const double dSpread, const char* pzMT4Time)
{
    string sNewBroker = pzBroker;
    TCurrMD* p = NULL;
    if (m_vecMD.size() > 0)
    {
        for (int i = 0; i < (int)m_vecMD.size(); i++)
        {
            if (m_vecMD.at(i)->sBroker == sNewBroker)
            {
                p = m_vecMD.at(i);
                p->sBroker = sNewBroker;
                p->sMDTime = pzMT4Time;
                p->dBid = dBid;
                p->dAsk = dAsk;
                p->dSpread = dSpread;
                break;
            }
        }
    }
    if(p==NULL)
    {
        TCurrMD* p = new TCurrMD;
        p->sBroker = sNewBroker;
        p->sMDTime = pzMT4Time;
        p->dBid = dBid;
        p->dAsk = dAsk;
        p->dSpread = dSpread;

        m_vecMD.push_back(p);
    }
    return p;
}


VOID CArbiLatency::Send_OpenOrder()
{
    CProtoSet setBest;
    setBest.Begin();
    setBest.SetVal(FDS_CODE,        __ALPHA::CODE_ORDER_OPEN);
    setBest.SetVal(FDS_SYMBOL,      m_symbolInfo.sSymbol);
    setBest.SetVal(FDS_ORD_SIDE,    (m_triggered.Is_BidTriggered())? __ALPHA::DEF_SELL : __ALPHA::DEF_BUY) ;
    setBest.SetVal(FDD_LOTS,        m_symbolInfo.dVolume);
    setBest.SetVal(FDD_OPEN_PRC,    m_triggered.dBestPrc);
    setBest.SetVal(FDN_MAGIC_NO,    g_config.getMagicNo());

    TSendOrder* pSendBest = new TSendOrder;    
    strcpy(pSendBest->brokerKey, m_triggered.sBest_Broker.c_str());
    int nLen = setBest.Complete(pSendBest->zSendBuf);
    PostThreadMessage(m_unThreadOrderBest, WM_ORDER_OPEN, (WPARAM)nLen, (LPARAM)pSendBest);


    CProtoSet setWorst;
    setWorst.Begin();
    setWorst.SetVal(FDS_CODE,       __ALPHA::CODE_ORDER_OPEN);
    setWorst.SetVal(FDS_SYMBOL,     m_symbolInfo.sSymbol);
    setWorst.SetVal(FDS_ORD_SIDE,   (m_triggered.Is_BidTriggered()) ? __ALPHA::DEF_BUY : __ALPHA::DEF_SELL);
    setWorst.SetVal(FDD_LOTS,       m_symbolInfo.dVolume);
    setWorst.SetVal(FDD_OPEN_PRC,   m_triggered.dOppPrc);
    setWorst.SetVal(FDN_MAGIC_NO, g_config.getMagicNo());

    TSendOrder* pSendWorst = new TSendOrder;
    strcpy(pSendWorst->brokerKey, m_triggered.sWorst_Broker.c_str());
    nLen = setWorst.Complete(pSendWorst->zSendBuf);
    PostThreadMessage(m_unThreadOrderWorst, WM_ORDER_OPEN, (WPARAM)nLen, (LPARAM)pSendWorst);

    
}

void CArbiLatency::Print_Triggered()
{
    char buf[4096];

    sprintf(buf, "[%s][%s][%s]"
                "BEST->(%s)(%s)(Prc:%.5f)(Spread:%.0f)  WORST->(%s)(%s)(Prc:%.5f)(Opp:%.5f)(Spread:%.0f)"
                "(GapPts:%d)(PLPts:%d)(SlippagePtr:%d)(Target:%d)\n"
        
        ,m_symbolInfo.sSymbol.c_str()
        , m_triggered.sBidAsk.c_str()
        , m_triggered.zLocalTime

        , m_triggered.sBest_Broker.c_str()
        , m_triggered.sBest_Time.c_str()
        , m_triggered.dBestPrc
        , m_triggered.dBestSpread

        , m_triggered.sWorst_Broker.c_str()
        , m_triggered.sWorst_Time.c_str()
        , m_triggered.dWorstPrc
        , m_triggered.dOppPrc
        , m_triggered.dWorstSpread

        , m_triggered.nGapPts
        , m_triggered.nNetPLPts
        , m_symbolInfo.nSlippagePts
        , m_symbolInfo.nTargetPtsOpening
    );
    LOGGING(INFO, FALSE, buf);

    sprintf(buf, "[%s][%s]"
        "(%s)(%s)(B:%.5f)(A:%.5f)(S:%.0f)__(%s)(%s)(B:%.5f)(A:%.5f)(S:%.0f)\n"

        , m_symbolInfo.sSymbol.c_str()
        , m_triggered.zLocalTime

        , m_vecMD.at(0)->sBroker.c_str()
        , m_vecMD.at(0)->sMDTime.c_str()
        , m_vecMD.at(0)->dBid
        , m_vecMD.at(0)->dAsk
        , m_vecMD.at(0)->dSpread

        , m_vecMD.at(1)->sBroker.c_str()
        , m_vecMD.at(1)->sMDTime.c_str()
        , m_vecMD.at(1)->dBid
        , m_vecMD.at(1)->dAsk
        , m_vecMD.at(1)->dSpread
    );
    LOGGING(INFO, FALSE, buf);

    int nGapBestWorst = 0, nGapBestOpp = 0;
    if (m_triggered.sBidAsk.c_str() == "BID")
    {
        nGapBestWorst = (INT) ((m_triggered.dBestPrc - m_triggered.dWorstPrc) / m_symbolInfo.dPtsSize);
        nGapBestOpp = (INT)((m_triggered.dBestPrc - m_triggered.dOppPrc) / m_symbolInfo.dPtsSize);
    }
    else
    {
        nGapBestWorst = (INT)((m_triggered.dWorstPrc - m_triggered.dBestPrc) / m_symbolInfo.dPtsSize);
        nGapBestOpp = (INT)((m_triggered.dOppPrc - m_triggered.dBestPrc) / m_symbolInfo.dPtsSize);
    }

    sprintf(buf, "<%s><%s><MT4:%s>[B](%s)(%.5f)<-->[W](%s)(%.5f)(Opp:%.5f){%d Pts}{%d Pts}"
        , m_symbolInfo.sSymbol.c_str()
        , m_triggered.sBidAsk.c_str()
        , m_triggered.sBest_Time.c_str()
        , m_triggered.sBest_Broker.c_str()
        , m_triggered.dBestPrc

        , m_triggered.sWorst_Broker.c_str()
        , m_triggered.dWorstPrc
        , m_triggered.dOppPrc

        , nGapBestWorst
        , nGapBestOpp
    );
    LOG_DATA(buf);
}


VOID CArbiLatency::Calc_Profits(string sBrokerKey, double dBid, double dAsk)
{

    if (m_pManagePos->CheckProfit(sBrokerKey, dBid, dAsk) == FALSE)
        return;

    TPosInfo l, s;
    m_pManagePos->GetPosInfo(_Out_ &l, _Out_ &s);

    Send_CloseOrder(_In_ & l, _In_ & s);

    m_pManagePos->ResetPosInfo();
}


VOID CArbiLatency::Send_CloseOrder(_In_ TPosInfo* l, _In_ TPosInfo* s)
{
    char z[128] = {};
    CProtoSet setLong;
    setLong.Begin();
    setLong.SetVal(FDS_CODE, __ALPHA::CODE_ORDER_CLOSE);
    setLong.SetVal(FDN_TICKET, l->nTicket);
    setLong.SetVal(FDS_SYMBOL, m_symbolInfo.sSymbol.c_str());

    TSendOrder* pLongSend = new TSendOrder;
    strcpy(pLongSend->brokerKey, l->zBrokerKey);
    int nLen = setLong.Complete(pLongSend->zSendBuf);
    PostThreadMessage(m_unThreadOrderBest, WM_ORDER_CLOSE, (WPARAM)nLen, (LPARAM)pLongSend);


    CProtoSet setShort; 
    setShort.Begin();
    setShort.SetVal(FDS_CODE, __ALPHA::CODE_ORDER_CLOSE);
    setShort.SetVal(FDN_TICKET, s->nTicket);
    setShort.SetVal(FDS_SYMBOL, m_symbolInfo.sSymbol.c_str());

    TSendOrder* pShortSend = new TSendOrder;
    strcpy(pShortSend->brokerKey, s->zBrokerKey);
    nLen = setShort.Complete(pShortSend->zSendBuf);
    PostThreadMessage(m_unThreadOrderWorst, WM_ORDER_CLOSE, (WPARAM)nLen, (LPARAM)pShortSend);

}

//void CArbiLatency::UpdateBestWorst_Triggered(const bool bBid, const char* pzLocalTime, const char* pzBroker, const double dNewPrc,
//    const double dSpread, const char* pzMT4Time, _Out_ BOOL* pChanged)
//{
//    TBestWorstMD* pCurr = (bBid) ? &m_md.BID : &m_md.ASK;
//
//    *pChanged = FALSE;
//    if (pCurr->IsNotTriggered())
//        return;
//
//    if (IsSameBroker(pCurr->sBest_Broker, pzBroker))
//    {
//        *pChanged = TRUE;
//        pCurr->dBest = dNewPrc;
//        pCurr->dBestSpread = dSpread;
//        pCurr->sBest_Time = pzMT4Time;
//    }
//    else if (IsSameBroker(pCurr->sWorst_Broker, pzBroker))
//    {
//        *pChanged = TRUE;
//        pCurr->dWorst = dNewPrc;
//        pCurr->dWorstSpread = dSpread;
//        pCurr->sWorst_Time = pzMT4Time;
//    }
//
//    if (*pChanged)
//    {
//        m_md.sLocalTime = pzLocalTime;
//        Calc_Gap_NetPL(bBid, pCurr);
//    }
//}
