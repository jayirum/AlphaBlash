#pragma once

#include <windows.h>
#include <string>
#include "../../CommonAnsi/BaseThread.h"
#include "../../CommonAnsi/LogCsv.h"

#include <map>
using namespace std;


#define DEF_BEST    "BEST"
#define DEF_WORST   "WORST"


struct TCurrMD
{
    string sBroker;
    string sMDTime;
    double dBid;
    double dAsk;
    double dSpread;

    TCurrMD() { dBid = 0; dAsk = 0; dSpread = 0; }
};

struct TMarketData
{    
    string  sBest_Broker;
    string  sBest_Time;
    double  dBest;
    double  dWorst;
    double  dBestSpread;
    double  dWorstSpread;
    int     nGapPts;
    int     nRealGapPts;
    int     nNetPL;
    string  sWorst_Time;
    string  sWorst_Broker;
    //string  sWhoIsTriggered;    //Best? Worst?
    char    cTriggeredYN[1];
    //int     nTraceCnt;
    
    TMarketData() { dBest = 0; dWorst = 0; nGapPts = 0; dBestSpread = 0; dWorstSpread = 0; cTriggeredYN[0] = 'N'; }
    BOOL    IsTriggered() { return (cTriggeredYN[0] == 'Y'); }
    BOOL    IsNotTriggered() { return (cTriggeredYN[0] != 'Y'); }
    BOOL    Is_NotFilledYet() { return (dBest == 0 || dWorst == 0); }
};

struct TFullData
{
    TMarketData bid;
    TMarketData ask;
    string  sLocalTime;
    double  dSpread;
    //BOOL    bTriggered;
    int     nTraceCnt;


    TFullData() { dSpread = 0;  nTraceCnt = 0; }
    VOID ResetTriggeredFlag() {
        bid.dBest = bid.dWorst = 0;
        ask.dBest = ask.dWorst = 0;
        bid.cTriggeredYN[0] = 'N'; ask.cTriggeredYN[0] = 'N'; nTraceCnt = 0; 
    }
};

class CCompareLatency : public CBaseThread
{
public:
    CCompareLatency();
    ~CCompareLatency();
    
    BOOL Initialize(const string sSymbol, const string sPointSize, const string sNetProfitPts, 
        const string sTraceCnt, const string sSlippagePts);
    VOID Execute(char* pzRecvData);

private:
    void    ThreadFunc();
    void    MainProc(const string *pData);

    BOOL    SetFirstData(const char* pzLocalTime, const char* pzBroker, const double dBid, const double dAsk,
        const double dSpread, const char* pzMT4Time);


    VOID    UpdateCurrMD(const char* pzBroker, const double dBid, const double dAsk, const double dSpread, const char* pzMT4Time);
    VOID    UpdateBestWorst_NonTriggered(const bool bBid, const char* pzLocalTime, const char* pzBroker, const double dNewPrc,
        const double dSpread, const char* pzMT4Time, _Out_ BOOL* pChanged);

    VOID    UpdateBestWorst_Triggered(const bool bBid, const char* pzLocalTime, const char* pzBroker, const double dNewPrc,
        const double dSpread, const char* pzMT4Time, _Out_ BOOL* pChanged);

    BOOL     Calc_Gap_NetPL(_In_ BOOL bBid, _In_ TMarketData* p);
    BOOL    CheckTriggering(BOOL bBid);

    void    PrintCurrData();

    VOID    WriteDataOnFile();

    bool IsSameBroker(string sBroker1, LPCSTR pzBroker2)
    {
        return (sBroker1.compare(pzBroker2) == 0);
    }

    TCurrMD* GetCurrMD(string sBroker) { return (*(m_mapCurrMD.find(sBroker))).second; }

private:
    string  m_sSymbol;
    double  m_dPointSize;
    int     m_nNetProfitThresholdPts;
    int     m_nSlippagePts;
    int     m_nMaxTraceCnt;
    string  m_sRawData;
 

    TFullData     m_md;
    map<string, TCurrMD* > m_mapCurrMD;

};

