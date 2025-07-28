#pragma once

#include "CCalcBestPrc.h"
#include "CMagicNo.h"


struct TDataPerSymbol
{
    TData           *data;
    CCalcBestPrc    *calc;
    CRITICAL_SECTION    *cs;
};

class CDataHandler
{
public:
    CDataHandler();
    ~CDataHandler();

public:

    void Calc_BestPrc_OpenOrd(int iSymbol, /*char* pzSymbol,*/ double dNewBid, double dNewAsk,
        double dSpread, char* pzNewBrokerKey, char* pzNewBrokerName, _Out_ BOOL* o_pbMustSendOrder);

    void Calc_EstmPL_CloseOrder(int iSymbol, /*char* pzSymbol*/ double dNewBid, double dNewAsk, char* pzNewBrokerKey, 
                                BOOL bBatchClose, _Out_ BOOL* pbMustSendOrder);


public:
    BOOL Add_Symbol(int iSymbol, const char* pzSymbol, double dPipSize
        , int nDecimalCnt, int nActiveSpreadPt
        , int nThresholdOpenPt, int nThresholdClosePt, double dOrderLots
        , int nNoMoreOpenRoundCnt //0514-2
    );
    BOOL Add_BrokerWhenLogin(char* pzBrokerKey, char* pzBrokerName);


    VOID Update_ResponseOfOpenOrder(int iSymbol, char* pzTicketNo, double dOpenPrc, double dLots, int nMT4Cmd, char* pzOpenTmMT4);
    VOID Update_ResponseOfCloseOrder(int iSymbol, char* pzTicketNo, double dClosePrc, double dLots, int nMT4Cmd, char* pzCloseTmMT4, double dCmsn, double dPL, double dSwap, char* pzDBLog);
    void Update_LatestMarketData(int iSymbol, char* pzBrokerKey, double dNewBid, double pzNewAsk, double dSpread, char* pzMDTime);
    void ResetData(int iSymbol);
    void MarketClose(int iSymbol, BOOL* bMustSendOrder);

    //0514-2 BOOL Check_Set_SymbolNoMoreOpen_by_LossCnt(int iSymbol, char* pzMsg);
    BOOL Is_NoMoreOpenByRoundCnt(int iSymbol);
    
    // Begin called from outer thread (Iocp::CheckNoMoreTimeThread)
    BOOL Check_Set_NoMoreOpen_by_Time(char* pzMsg);
    BOOL Check_ResumeTrade_by_Time(char* pzMsg);
    
    BOOL Is_NoMoreOpenByTime() { return m_bNoMoreOpen; }

    BOOL Is_HedgePositionRejected(int iSymbol, int nMyMT4Cmd);
    void Mark_Rejected(int iSymbol, int nMT4Cmd);

    VOID Set_NoMoreOpen_Time(char* pzNoMoreTimeBegin, char* pzNoMoreTimeEnd);
    BOOL Is_UnderActiveSpread(int iSymbol, char* pzBidBrokerKey, double dBidderSpread, char* pzAskBrokerKey, double dAskerSpread);
    
    BOOL Is_AlreadOpened(int iSymbol);
    BOOL Should_CalcBestPrc(int iSymbol);

    BOOL Is_Condition_ProfitCut();
    BOOL Is_ProfitCut_AlreadyTriggered() { return m_bProfitCutTriggered; }

    VOID Update_OrdStatus_OpenTriggered(int iSymbol)    { Data(iSymbol)->nOrdStatus = ORDSTATUS_OPEN_TRIGGERED; }
    VOID Update_OrdStatus_CloseTriggered(int iSymbol)   { Data(iSymbol)->nOrdStatus = ORDSTATUS_CLOSE_TRIGGERED; }
    BOOL Is_OrdStatus_BothClosed(int iSymbol)           { return (Data(iSymbol)->nOrdStatus == ORDSTATUS_CLOSE_MT4_2); }
    int Get_SymbolCntTodo();// { return m_nSymbolCntTodo; }
    int Get_SymbolCntCurrent()                          { return m_vecDataPerSymbol.size(); }
    int Get_BrokerCntTodo()                             { return m_nBrokerCntTodo; }
    int Get_BrokerCntCurrent()                          { return m_vecBroker.size(); }
    VOID Set_BrokerCntTodo(int nCnt)                    { m_nBrokerCntTodo = nCnt; }
    VOID Set_ProfitCutThreshold(int nPlPoints)          { m_nProfitCutThreshold = nPlPoints; }
    VOID LockData(int iSymbol)                            {EnterCriticalSection(CS(iSymbol));}
    VOID UnlockData(int iSymbol)                          { LeaveCriticalSection(CS(iSymbol)); }


    //BOOL Is_ReadyTrade();


private:
    //BOOL TSApplied() { return m_bTSApplied; }

    
    int CalcGap(int iSymbol, char* pzBidPrc, char* pzAskPrc);
    BOOL OpenOrder_Check_Place(int iSymbol, char* pzSymbol, char* pzLastBidTime, char* pzLastAskTime); // , char* pzNewBrokerKey, char* pzNewBid, char* pzNewAsk);
    
    BOOL IsGap_BiggerThan_OpenThreshold(int iSymbol);
    BOOL IsNetPL_BiggerThan_CloseThreshold(int iSymbol, int nNetPLPt);

    int Is_Condition_ProfitCut_Inner(int iSymbol);
    
public:
    TData* Data(int iSymbol)            { return m_vecDataPerSymbol.at(iSymbol)->data; }
    CCalcBestPrc* Calc(int iSymbol)     { return m_vecDataPerSymbol.at(iSymbol)->calc; }
    CRITICAL_SECTION* CS(int iSymbol)   { return m_vecDataPerSymbol.at(iSymbol)->cs; }
    int SymbolCount() { return m_vecDataPerSymbol.size(); }
    
private:
    int     m_nSymbolCntTodo;
    int     m_nBrokerCntTodo;
    //0524-2 int     m_nMaxLossCnt;
    char    m_zNoMoreOpenTimeBegin[32], m_zNoMoreOpenTimeEnd[32];
    char    m_zMsg[1024];
    //BOOL    m_bTradeStart;
    //BOOL    m_bTSApplied;

    int     m_nProfitCutThreshold;
    BOOL    m_bProfitCutTriggered;
private:
    // symbol º°
    vector< TDataPerSymbol*>        m_vecDataPerSymbol;
    vector<TBroker*>                m_vecBroker;

    BOOL        m_bNoMoreOpen;
    CMagicNo    m_magicNo;

};

