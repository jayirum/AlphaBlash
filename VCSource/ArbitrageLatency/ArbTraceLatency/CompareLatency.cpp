#include "CompareLatency.h"
#include "../../CommonAnsi/LogMsg.h"
#include "../../CommonAnsi/Util.h"
#include "../../Common/AlphaInc.h"
#include "../../Common/AlphaProtocolUni.h"
#include "Inc.h"

extern  CLogMsg g_log;
extern  TCHAR	g_zConfig[_MAX_PATH];
extern  TCHAR	g_zLogDir[_MAX_PATH];
extern  CLogCsv g_csv;

CCompareLatency::CCompareLatency()
{
    m_dPointSize = 0;
    m_nNetProfitThresholdPts = 0;
}

CCompareLatency::~CCompareLatency()
{
    StopThread();
}

BOOL CCompareLatency::Initialize(const string sSymbol, const string sPointSize, 
    const string sNetProfitPts, const string sTraceCnt, const string sSlippagePts)
{
    m_sSymbol = sSymbol;
    m_dPointSize = atof(sPointSize.c_str());
    m_nNetProfitThresholdPts = atoi(sNetProfitPts.c_str());
    m_nSlippagePts = atoi(sSlippagePts.c_str());
    m_nMaxTraceCnt = atoi(sTraceCnt.c_str());

    CBaseThread::ResumeThread();

    return TRUE;
}

VOID CCompareLatency::Execute(char* pzRecvData)
{
    string* pData = new string(pzRecvData);

    PostThreadMessage(m_dwThreadID, WM_RECEIVE_DATA, (WPARAM)pData->size(), (LPARAM)pData);
}



void CCompareLatency::ThreadFunc()
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

void CCompareLatency::MainProc(const string* pData)
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
    dBid    = get.GetValD(FDD_BID);
    dAsk    = get.GetValD(FDD_ASK);
    dSpread = get.GetValD(FDD_SPREAD);


    char zLocalTime[32]; SYSTEMTIME st; GetLocalTime(&st);
    sprintf(zLocalTime, "%04d%02d%02d_%02d:%02d:%02d:%03d",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    UpdateCurrMD(zBrokerKey, dBid, dAsk, dSpread, zMDTime);

    if (SetFirstData(zLocalTime, zBrokerKey, dBid, dAsk, dSpread, zMDTime))
        return;

    BOOL bBid = TRUE;
    BOOL bChanged = FALSE;
    INT nTriggered = 0;

    UpdateBestWorst_NonTriggered(bBid, zLocalTime, zBrokerKey, dBid, dSpread, zMDTime, _Out_ &bChanged);
    if (bChanged){
        if (CheckTriggering(bBid)) nTriggered++;
    }

    UpdateBestWorst_NonTriggered(!bBid, zLocalTime, zBrokerKey, dAsk, dSpread, zMDTime, _Out_ &bChanged);
    if (bChanged) {
        if (CheckTriggering(!bBid)) nTriggered++;
    }
    if (nTriggered > 0)
    {
        LOGGING(INFO, TRUE, "[%s]Triggered", m_sSymbol.c_str());
        WriteDataOnFile();
        //
        //TODO.ORDER
        //
        return;
    }

    INT nChanged = 0;
    UpdateBestWorst_Triggered(bBid, zLocalTime, zBrokerKey, dBid, dSpread, zMDTime, _Out_ &bChanged);
    if (bChanged) nChanged++;

    UpdateBestWorst_Triggered(!bBid, zLocalTime, zBrokerKey, dAsk, dSpread, zMDTime, _Out_ & bChanged);
    if (bChanged) nChanged++;

    if (nChanged>0)
    {
        m_md.nTraceCnt++;
        WriteDataOnFile();
        if (m_md.nTraceCnt >= m_nMaxTraceCnt)
        {
            m_md.ResetTriggeredFlag();
            LOGGING(INFO, TRUE, "[%s]Reset Tracing", m_sSymbol.c_str());
        }
    }

}



VOID CCompareLatency::UpdateCurrMD(const char* pzBroker, const double dBid, const double dAsk, const double dSpread, const char* pzMT4Time)
{
    TCurrMD* p;
    map<string, TCurrMD* >::iterator it = m_mapCurrMD.find(pzBroker);
    if (it == m_mapCurrMD.end())
        p = new TCurrMD;
    else
        p = (*it).second;

    p->sBroker = pzBroker;
    p->sMDTime = pzMT4Time;
    p->dBid = dBid;
    p->dAsk = dAsk;
    p->dSpread = dSpread;

    m_mapCurrMD[p->sBroker] = p;
}


VOID CCompareLatency::UpdateBestWorst_NonTriggered(const bool bBid, const char* pzLocalTime, const char* pzBroker, const double dNewPrc,
    const double dSpread, const char* pzMT4Time, _Out_ BOOL* pChanged)
{
    if (m_md.bid.IsTriggered() || m_md.ask.IsTriggered() )
        return;

    TMarketData* pCurr = (bBid) ? &m_md.bid : &m_md.ask;
    *pChanged = FALSE;


    if (
        (bBid && (pCurr->dBest < dNewPrc)) ||
        (!bBid && (pCurr->dBest > dNewPrc))
        )
    {
        *pChanged = TRUE;

        pCurr->dBest = dNewPrc;
        pCurr->dBestSpread = dSpread;
        pCurr->sBest_Time = pzMT4Time;
        pCurr->sBest_Broker = pzBroker;
    }

    // check worst
    if (
        (bBid && (pCurr->dWorst > dNewPrc)) ||
        (!bBid && (pCurr->dWorst < dNewPrc))
        )
    {
        *pChanged = FALSE;

        pCurr->dWorst = dNewPrc;
        pCurr->dWorstSpread = dSpread;
        pCurr->sWorst_Time = pzMT4Time;
        pCurr->sWorst_Broker = pzBroker;
    }
    
    if (m_md.bid.Is_NotFilledYet() || m_md.ask.Is_NotFilledYet())
        return;

    if (*pChanged)
    {
        m_md.sLocalTime = pzLocalTime;
        Calc_Gap_NetPL(bBid, pCurr);
    }
}



void CCompareLatency::UpdateBestWorst_Triggered(const bool bBid, const char* pzLocalTime, const char* pzBroker, const double dNewPrc,
    const double dSpread, const char* pzMT4Time, _Out_ BOOL* pChanged)
{
    TMarketData* pCurr = (bBid) ? &m_md.bid : &m_md.ask;

    *pChanged = FALSE;
    if (pCurr->IsNotTriggered())
        return;

    if (IsSameBroker(pCurr->sBest_Broker, pzBroker))
    {
        *pChanged = TRUE;
        pCurr->dBest = dNewPrc;
        pCurr->dBestSpread = dSpread;
        pCurr->sBest_Time = pzMT4Time;
    }
    else if (IsSameBroker(pCurr->sWorst_Broker, pzBroker))
    {
        *pChanged = TRUE;
        pCurr->dWorst = dNewPrc;
        pCurr->dWorstSpread = dSpread;
        pCurr->sWorst_Time = pzMT4Time;
    }

    if (*pChanged)
    {
        m_md.sLocalTime = pzLocalTime;
        Calc_Gap_NetPL(bBid, pCurr);
    }
}


BOOL CCompareLatency::Calc_Gap_NetPL(_In_ BOOL bBid, _In_ TMarketData* p)
{
    map<string, TCurrMD* >::iterator it = m_mapCurrMD.find(p->sWorst_Broker);
    if (it == m_mapCurrMD.end())
    {
        LOGGING(ERR, TRUE, "Failed to find broker (%s)", p->sWorst_Broker.c_str());
        return FALSE;
    }


    double dGapPts = (bBid) ? (p->dBest - p->dWorst) : (p->dWorst - p->dBest);
    double dRealGapPts = (bBid) ? (p->dBest - (*it).second->dAsk) : ((*it).second->dBid - p->dBest);

    p->nGapPts  = (int)(dGapPts / m_dPointSize);
    p->nRealGapPts = (int)(dRealGapPts / m_dPointSize);
    p->nNetPL = p->nRealGapPts - m_nSlippagePts;

    return TRUE;
}

BOOL CCompareLatency::CheckTriggering(BOOL bBid)
{
    TMarketData* pCurr = (bBid) ? &m_md.bid : &m_md.ask;

    if (pCurr->sBest_Broker == pCurr->sWorst_Broker)    
        return FALSE;

    if (pCurr->IsTriggered())
        return FALSE;

    //if (pCurr->nGapPts >= (m_nNetProfitThresholdPts + pCurr->dBestSpread + pCurr->dWorstSpread))
    if (pCurr->nNetPL >= m_nNetProfitThresholdPts )
    {
        pCurr->cTriggeredYN[0] = 'Y';
        PrintCurrData();
        return TRUE;
    }
    return FALSE;
}


VOID CCompareLatency::WriteDataOnFile()
{
    g_csv.BeginTx();

    g_csv.Data_Add(m_sSymbol, false);
    g_csv.Data_Add(m_md.sLocalTime, false);

    g_csv.Data_Add(m_md.bid.sBest_Broker, false);
    g_csv.Data_Add(m_md.bid.sBest_Time, false);
    g_csv.Data_Add(m_md.bid.dBest, false);
    g_csv.Data_Add(m_md.bid.dWorst, false);
    g_csv.Data_Add(GetCurrMD(m_md.bid.sWorst_Broker)->dAsk, false); //opp
    g_csv.Data_Add(m_md.bid.dBestSpread, false);
    g_csv.Data_Add(m_md.bid.dWorstSpread, false);
    g_csv.Data_Add(m_md.bid.sWorst_Time, false);
    g_csv.Data_Add(m_md.bid.sWorst_Broker, false);
    g_csv.Data_Add(m_md.bid.nGapPts, false);
    g_csv.Data_Add(m_md.bid.nRealGapPts, false);
    g_csv.Data_Add(m_nSlippagePts, false);
    g_csv.Data_Add(m_md.bid.dBestSpread + m_md.bid.dWorstSpread, false);
    g_csv.Data_Add(m_md.bid.nNetPL, false);
    g_csv.Data_Add(m_md.bid.cTriggeredYN[0], false);

    g_csv.Data_Add(m_md.ask.sBest_Broker, false);
    g_csv.Data_Add(m_md.ask.sBest_Time, false);
    g_csv.Data_Add(m_md.ask.dBest, false);
    g_csv.Data_Add(m_md.ask.dWorst, false);
    g_csv.Data_Add(GetCurrMD(m_md.ask.sWorst_Broker)->dBid, false);
    g_csv.Data_Add(m_md.ask.dBestSpread, false);
    g_csv.Data_Add(m_md.ask.dWorstSpread, false);
    g_csv.Data_Add(m_md.ask.sWorst_Time, false);
    g_csv.Data_Add(m_md.ask.sWorst_Broker, false);
    g_csv.Data_Add(m_md.ask.nGapPts, false);
    g_csv.Data_Add(m_md.ask.nRealGapPts, false);
    g_csv.Data_Add(m_nSlippagePts, false);
    g_csv.Data_Add(m_md.ask.dBestSpread + m_md.ask.dWorstSpread, false);
    g_csv.Data_Add(m_md.ask.nNetPL, false);
    g_csv.Data_Add(m_md.ask.cTriggeredYN[0], false);

    g_csv.Data_Add(m_nNetProfitThresholdPts, false);
    g_csv.Data_Add(m_md.nTraceCnt, true);

    g_csv.EndTx();
}


BOOL CCompareLatency::SetFirstData(const char* pzLocalTime, const char* pzBroker, const double dBid, const double dAsk,
    const double dSpread, const char* pzMT4Time)
{
    if (m_md.bid.dBest == 0)
    {
        m_md.sLocalTime = pzLocalTime;

        m_md.bid.dBest = dBid;
        m_md.bid.sBest_Time = pzMT4Time;
        m_md.bid.sBest_Broker = pzBroker;
        m_md.bid.dWorst = dBid;
        m_md.bid.sWorst_Time = pzMT4Time;
        m_md.bid.sWorst_Broker = pzBroker;

        m_md.ask.dBest = dAsk;
        m_md.ask.sBest_Time = pzMT4Time;
        m_md.ask.sBest_Broker = pzBroker;
        m_md.ask.dWorst = dAsk;
        m_md.ask.sWorst_Time = pzMT4Time;
        m_md.ask.sWorst_Broker = pzBroker;

        return TRUE;
    }

    return FALSE;
}

void CCompareLatency::PrintCurrData()
{
    char buf[4096];
    sprintf(buf, "[%s](%s)(%s)(B:%.5f)(W:%.5f)(G:%d)(R.G:%d)(OppPrc:%.5f)(PL:%d)(%s)(%s)(Triggered:%c)\n"
        "(%s)(%s)(B:%.5f)(W:%.5f)(G:%d)(R.G:%d)(OppPrc:%.5f)(PL:%d)(%s)(%s)(Triggered:%c)"
        , m_md.sLocalTime.c_str()
        , m_md.bid.sBest_Broker.c_str()
        , m_md.bid.sBest_Time.c_str()
        , m_md.bid.dBest
        , m_md.bid.dWorst
        , m_md.bid.nGapPts
        , m_md.bid.nRealGapPts
        , GetCurrMD(m_md.bid.sWorst_Broker)->dAsk
        , m_md.bid.nNetPL
        , m_md.bid.sWorst_Time.c_str()
        , m_md.bid.sWorst_Broker.c_str()
        , m_md.bid.cTriggeredYN[0]
        , m_md.ask.sBest_Broker.c_str()
        , m_md.ask.sBest_Time.c_str()
        , m_md.ask.dBest
        , m_md.ask.dWorst
        , m_md.ask.nGapPts
        , m_md.ask.nRealGapPts
        , GetCurrMD(m_md.ask.sWorst_Broker)->dBid
        , m_md.ask.nNetPL
        , m_md.ask.sWorst_Time.c_str()
        , m_md.ask.sWorst_Broker.c_str()
        , m_md.ask.cTriggeredYN[0]
    );
    LOGGING(INFO, FALSE, buf);
}