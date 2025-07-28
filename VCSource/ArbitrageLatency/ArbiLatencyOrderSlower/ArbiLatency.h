#pragma once

#include <windows.h>
#include <string>
#include "../../CommonAnsi/BaseThread.h"
#include "../../CommonAnsi/LogCsv.h"
#include "CManagePos.h"
#include <map>
#include <vector>
using namespace std;

#define DEF_FASTER   'F'
#define DEF_SLOWER   'S'
#define DEF_BOTH     'B'
#define DEF_BROKER_CNT  2

#define DIRECTION_UP    "U"
#define DIRECTION_DN    "D"
#define DIRECTION_FLAT  "0"

enum EN_BIDASK { E_NONE, E_BID, E_ASK };
class CCurrMD
{
public:
    CCurrMD() { Reset(); }
    ~CCurrMD(){}

public:
    void Reset() { sMDTime_prev = ""; dBid_prev = 0; dAsk_prev = 0; dSpread_prev = 0; sMDTime_curr = ""; dBid_curr = 0; dAsk_curr = 0; dSpread_curr = 0; }
    void Swap()
    {
        sMDTime_prev = sMDTime_curr; dBid_prev = dBid_curr; dAsk_prev = dAsk_curr; dSpread_prev = dSpread_curr;
    }

    BOOL Is_Empty() { return (sMDTime_prev == ""); }
    double GapBid() { return (dBid_curr - dBid_prev); }
    double GapAsk() { return (dAsk_curr - dAsk_prev); }

public:
    string sBroker;
    string sClientTp;   // DEF_FAST OR DEF_SLOW

    string sMDTime_prev;
    double dBid_prev;
    double dAsk_prev;
    double dSpread_prev;

    string sMDTime_curr;
    double dBid_curr;
    double dAsk_curr;
    double dSpread_curr;
};

struct TTriggered
{
    //EN_BIDASK   enBidAsk;
    //string      sBidAsk;


    char        zLocalTime[64];

    string  sBrokerFaster;
    string  sBrokerSlower;
    string  sBrokerGap;
    string  sGapDirection;

    string  MDTimeF;
    double  PrevF;
    double  CurrF;
    int     GapF;

    string  MDTimeS;
    double  PrevS;
    double  CurrS;
    int     GapS;

    int     Threshold;

    TTriggered() { Reset(); }
    BOOL    Is_BidTriggered() { return (sGapDirection !=""); }
    VOID    Reset() { 
        //enBidAsk = E_NONE; sBidAsk = "";
        zLocalTime[0] = 0;
        sBrokerFaster = sBrokerSlower = sBrokerGap = sGapDirection = ""; 
        MDTimeF = ""; PrevF = CurrF = 0; GapF = 0;
        MDTimeS = ""; PrevS = CurrS = 0; GapS = 0;
    }
 /*   VOID    Set_BidAskStr() { 
        if (enBidAsk == E_BID) sBidAsk = "BID";
        else if (enBidAsk == E_ASK) sBidAsk = "ASK";
        else sBidAsk = "";
    }*/
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
    VOID Update_CurrMD(const char* pzBroker, LPCSTR pzClientTp, const double dBid, const double dAsk, const double dSpread, const char* pzMT4Time);
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
    
    BOOL Is_Faster(LPCSTR pzClientTp) { return (pzClientTp[0] == DEF_FASTER); }


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
    
    //vector<TCurrMD*>    m_vecMD;
    CCurrMD             m_mdFaster, m_mdSlower;
    TTriggered          m_triggered;    
    TTriggered          m_tBid, m_tAsk;
    TSymbolInfo         m_symbolInfo;
    CManagePos* m_pManagePos;
};

