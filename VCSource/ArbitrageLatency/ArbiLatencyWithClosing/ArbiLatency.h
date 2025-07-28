#pragma once

#include <windows.h>
#include <string>
#include "../../CommonAnsi/BaseThread.h"
#include "../../CommonAnsi/LogCsv.h"
#include "CManagePos.h"
#include <map>
#include <vector>
using namespace std;

#define DEF_BEST    "BEST"
#define DEF_WORST   "WORST"
#define DEF_BROKER_CNT  2

enum EN_BIDASK { E_NONE, E_BID, E_ASK };
struct TCurrMD
{
    string sBroker;
    string sMDTime;
    double dBid;
    double dAsk;
    double dSpread;

    TCurrMD() { dBid = 0; dAsk = 0; dSpread = 0; }
};

struct TTriggered
{
    EN_BIDASK   enBidAsk;
    string      sBidAsk;

    char        zLocalTime[64];

    string  sBest_Broker;
    string  sBest_Time;
    double  dBestPrc;
    double  dBestSpread;

    string  sWorst_Broker;
    string  sWorst_Time;
    double  dWorstPrc;
    double  dWorstSpread;

    double  dOppPrc;

    int     nGapPts;
    int     nNetPLPts;

    TTriggered() { Reset(); }
    BOOL    Is_BidTriggered() { return (enBidAsk == E_BID); }
    VOID    Reset() { 
        enBidAsk = E_NONE; sBidAsk = "";
        sBest_Broker = sBest_Time = ""; dBestPrc = dBestSpread = 0;
        sWorst_Broker = sWorst_Time = ""; dOppPrc = dWorstSpread = 0;
        dWorstPrc = 0;
        nNetPLPts = nGapPts = 0; ZeroMemory(zLocalTime, sizeof(zLocalTime));
    }
    VOID    Set_BidAskStr() { 
        if (enBidAsk == E_BID) sBidAsk = "BID";
        else if (enBidAsk == E_ASK) sBidAsk = "ASK";
        else sBidAsk = "";
    }
 };



class CArbiLatency : public CBaseThread
{
public:
    CArbiLatency();
    ~CArbiLatency();
    
    BOOL Initialize(_In_ TSymbolInfo* pInfo, const UINT unThreadOrderBest, const UINT unThreadOrderWorst);
    VOID Execute(char* pCode, char* pzRecvData);
    VOID Update_Config();
private:
    void    ThreadFunc();
    void    MarketDataProc(const string *pData);
    TCurrMD* Update_CurrMD(const char* pzBroker, const double dBid, const double dAsk, const double dSpread, const char* pzMT4Time);
    BOOL    Check_Triggering();

    VOID    Send_OpenOrder();
    VOID    Send_CloseOrder(TPosInfo *l, TPosInfo *s);
    void    Print_Triggered();

    void    PosDataProc(const string* pData);

    //BOOL    Set_FirstData(const char* pzLocalTime, const char* pzBroker, const double dNewBid, const double dNewAsk,
    //    const double dSpread, const char* pzMT4Time);

    //VOID    Update_BestWorst(const char* pzLocalTime, const char* pzBroker, const double dBid, const double dAsk,
    //    const double dSpread, const char* pzMT4Time, _Out_ BOOL* pChanged);
   // VOID    Update_Gap_PL(_In_ BOOL bBid, _In_ TBestWorstMD* p);
    



    bool Is_SameBroker(string sBroker1, LPCSTR pzBroker2)
    {
        return (sBroker1.compare(pzBroker2) == 0);
    }
    VOID Remove_CurrMDList()
    {
        for (int i = 0; i < (int)m_vecMD.size(); i++)
            delete m_vecMD.at(i);
        m_vecMD.clear();
    }




    VOID Calc_Profits(string sBrokerKey, double dBid, double dAsk);

private:
    //string  m_sSymbol;
    //double  m_dPointSize;
    //int     m_nTargetPLOpen;
    //int     m_nTargetPLClose;
    //int     m_nSlippagePts;
    //int     m_nMaxTraceCnt;
    //double  m_dVol;
    string  m_sRawData;
    //char    m_zTradeTimeFrom[32], m_zTradeTimeTo[32];
    
    UINT    m_unThreadOrderBest, m_unThreadOrderWorst;
    
    vector<TCurrMD*>    m_vecMD;
    TTriggered          m_triggered;    
    TTriggered          m_tBid, m_tAsk;
    TSymbolInfo         m_symbolInfo;
    CManagePos* m_pManagePos;
};

