#include "ArbiLatency.h"
#include "../../Common/AlphaInc.h"
#include "../../CommonAnsi/LogMsg.h"
#include "../../CommonAnsi/Util.h"
#include "../../CommonAnsi/TimeUtils.h"
#include "../../Common/AlphaInc.h"
#include "../../Common/AlphaProtocolUni.h"
#include "Inc.h"



extern  CLogMsg g_log;
extern  TCHAR	g_zConfig[_MAX_PATH];
extern  TCHAR	g_zLogDir[_MAX_PATH];


CArbiLatency::CArbiLatency()
{
    m_dPointSize = 0;
    m_nTargetProfitPts = 0;
    m_nMaxTraceCnt = 1;
    m_dVol = 0;;
}

CArbiLatency::~CArbiLatency()
{
    StopThread();

    Remove_CurrMDList();
}

BOOL CArbiLatency::Initialize(const string sSymbol, const string sPointSize, const string sNetProfitPts, 
    const string sTraceCnt, const string sVol, const string sSlippagePts,
    const UINT unThreadOrderBest, const UINT unThreadOrderWorst)
{
    m_sSymbol                   = sSymbol;
    m_dPointSize                = atof(sPointSize.c_str());
    m_nTargetProfitPts    = atoi(sNetProfitPts.c_str());
    m_nMaxTraceCnt              = atoi(sTraceCnt.c_str());
    m_dVol                      = atof(sVol.c_str());
    m_nSlippagePts              = atoi(sSlippagePts.c_str());
    m_unThreadOrderBest         = unThreadOrderBest;
    m_unThreadOrderWorst        = unThreadOrderWorst;

    CBaseThread::ResumeThread();

    return TRUE;
}

VOID CArbiLatency::Execute(char* pzRecvData)
{
    //TRecvData* pData = new TRecvData(pzRecvData);
    string* pData = new string(pzRecvData);

    PostThreadMessage(m_dwThreadID, WM_RECEIVE_DATA, (WPARAM)pData->size(), (LPARAM)pData);
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
            if (msg.message != WM_RECEIVE_DATA)
                continue;

            MainProc((string*)msg.lParam);

            delete (string*)msg.lParam;
        } // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    } // while(pThis->m_bRun)
    
    return ;
}

void CArbiLatency::MainProc(const string* pData)
{
    m_sRawData = *pData;

    CProtoGet get;
    if (!get.ParsingWithHeader(pData->c_str(), pData->size()))
    {
        LOGGING(ERR, TRUE, "[%s]ParsingWithHeader Error(%s)(%s)", m_sSymbol.c_str(), get.GetMsg(), pData->c_str());
        return;
    }


    char zBrokerKey[128] = { 0 };// , zSymbol[128] = { 0 };
    char zMDTime[32] = { 0 };
    double dBid, dAsk, dSpread;

    get.GetVal(FDS_KEY, zBrokerKey);
    get.GetVal(FDS_MARKETDATA_TIME, zMDTime);
    dBid = get.GetValD(FDD_BID);
    dAsk = get.GetValD(FDD_ASK);
    dSpread = get.GetValD(FDD_SPREAD);

    if (dBid <= 0 || dAsk <= 0)
    {
        LOGGING(ERR, TRUE, "MD is NOT correct(Symbol:%s)(Time:%s)(Bid:5f)(Ask:%.5f)",
            m_sSymbol.c_str(), zMDTime, dBid, dAsk);
        return;
    }

    Update_CurrMD(zBrokerKey, dBid, dAsk, dSpread, zMDTime);

    if (m_vecMD.size() < DEF_BROKER_CNT)
    {
        return;
    }


    if (Check_Triggering())
    {
        Send_Order();

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

        pBid->nGapPts   = (int)((pBid->dBestPrc - pBid->dOppPrc) / m_dPointSize);
        pBid->nNetPLPts = pBid->nGapPts - m_nSlippagePts;   

        if (pBid->nNetPLPts >= m_nTargetProfitPts)
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

        pAsk->nGapPts   = (int)((pAsk->dOppPrc - pAsk->dBestPrc) / m_dPointSize);
        pAsk->nNetPLPts = pAsk->nGapPts - m_nSlippagePts;

        if (pAsk->nNetPLPts >= m_nTargetProfitPts)
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


VOID CArbiLatency::Send_Order()
{
    CProtoSet setBest;
    setBest.Begin();
    setBest.SetVal(FDS_CODE,        __ALPHA::CODE_ORDER_OPEN);
    setBest.SetVal(FDS_SYMBOL,      m_sSymbol);
    setBest.SetVal(FDS_ORD_SIDE,    (m_triggered.Is_BidTriggered())? __ALPHA::DEF_SELL : __ALPHA::DEF_BUY) ;
    setBest.SetVal(FDD_LOTS,        m_dVol);
    setBest.SetVal(FDD_OPEN_PRC,    m_triggered.dBestPrc);

    TSendOrder* pSendBest = new TSendOrder;    
    strcpy(pSendBest->brokerKey, m_triggered.sBest_Broker.c_str());
    int nLen = setBest.Complete(pSendBest->zSendBuf);
    PostThreadMessage(m_unThreadOrderBest, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pSendBest);


    CProtoSet setWorst;
    setWorst.Begin();
    setWorst.SetVal(FDS_CODE,       __ALPHA::CODE_ORDER_OPEN);
    setWorst.SetVal(FDS_SYMBOL,     m_sSymbol);
    setWorst.SetVal(FDS_ORD_SIDE,   (m_triggered.Is_BidTriggered()) ? __ALPHA::DEF_BUY : __ALPHA::DEF_SELL);
    setWorst.SetVal(FDD_LOTS,       m_dVol);
    setWorst.SetVal(FDD_OPEN_PRC,   m_triggered.dOppPrc);

    TSendOrder* pSendWorst = new TSendOrder;
    strcpy(pSendWorst->brokerKey, m_triggered.sWorst_Broker.c_str());
    nLen = setWorst.Complete(pSendWorst->zSendBuf);
    PostThreadMessage(m_unThreadOrderWorst, WM_ORDER_SEND, (WPARAM)nLen, (LPARAM)pSendWorst);

    
}

void CArbiLatency::Print_Triggered()
{
    char buf[4096];
    sprintf(buf, "[%s][%s][%s]"
                "BEST->(%s)(%s)(Prc:%.5f)(Spread:%.0f)  WORST->(%s)(%s)(Prc:%.5f)(Opp:%.5f)(Spread:%.0f)"
                "(GapPts:%d)(PLPts:%d)(SlippagePtr:%d)(Target:%d)\n"
        
        ,m_sSymbol.c_str()
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
        , m_nSlippagePts
        , m_nTargetProfitPts
    );
    LOGGING(INFO, FALSE, buf);

    sprintf(buf, "[%s][%s]"
        "(%s)(%s)(B:%.5f)(A:%.5f)(S:%.0f)__(%s)(%s)(B:%.5f)(A:%.5f)(S:%.0f)\n"

        , m_sSymbol.c_str()
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
