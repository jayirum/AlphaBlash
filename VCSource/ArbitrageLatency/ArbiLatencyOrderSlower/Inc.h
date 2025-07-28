#pragma once

#pragma warning( disable : 26495)
#include "../../Common/AlphaInc.h"
#include "../../CommonAnsi/LogMsg.h"
#include "CConfig.h"

struct TRecvData
{
    char data[BUF_LEN];
    TRecvData() { ZeroMemory(data, sizeof(data)); }
    TRecvData(char* p) { strcpy(data, p); }
};

struct TSymbolSpec
{
    int     nDecimalCnt;
    double  dPipSize;
    int     nActiveSpreadPt;
    int     nThresholdOpenPt;     //  exclude nCost 
    int     nThresholdClosePt;    //  exclude nCost 
    double  dOrderLots;
    int     nNoMoreOpenRoundCnt;  //0514-2

};


struct TPos
{
    bool bRejected;
    char zBrokerKey         [32];
    char zOpenPrc           [__ALPHA::LEN_PRC + 1];
    char zOpenPrc_Triggered [__ALPHA::LEN_PRC + 1];
    char zMDTime_OpenPrcTriggered[32]; // opening 할때 bestprice  에 사용한 current price의 Market Time
    char zOpenOppPrc        [__ALPHA::LEN_PRC + 1];
    char zOpenTmMT4         [32];
    int nOpenSlippage;      // zOpenPrc_Triggered - zOpenPrc

    char zClosePrc          [__ALPHA::LEN_PRC + 1];
    char zClosePrc_Triggered[__ALPHA::LEN_PRC + 1];
    char zMDTime_ClosePrcTriggered[32]; // closing 할때 손익계산시 사용한 current price의 Market Time
    char zTicket            [32];
    char zCloseTmMT4        [32];
    int nCloseSlippage;     // zClosePrc_Triggered - zClosePrc

    double dLots;
    int nPLPt;              // MT4 PL
    double dPL;             // MT4 PL / pip size
    double dCmsn;
    double dSwap;

    
};



#define UPDATE_BESTPRC_ENOUGH_CNT   3
struct TBestPrcForOpen
{
    //int bidUpdateCnt;
    char    bidPrc      [__ALPHA::LEN_PRC+1];
    char    bidBrokerKey[32];
    char    bidOpposite [__ALPHA::LEN_PRC + 1];
    
    char    askPrc      [__ALPHA::LEN_PRC+1];
    char    askBrokerKey[32];
    char    askOpposite [__ALPHA::LEN_PRC + 1];
    
    int     nGapBidAsk;


    //int     nGapMax_Open;  // Once gap goes over the open condition(comm*2),
                        // update this value whenever new max value occurrs.
    //int     nGapMax_Close;  // Once gap hits the target pt,
                        // update this value whenever new max value occurrs.
};

/*
* ORDSTATUS_OPEN_TRIGGERED  - Sending Open order to MT4
* ORDSTATUS_OPEN_MT4        - receive response of open order from MT4
* ORDSTATUS_CLOSE_TRIGGERED - Sending Close order to MT4
* ORDSTATUS_CLOSE_MT4       - receive response of close order from MT4
*/
enum EN_ORD_STATUS { ORDSTATUS_NONE = 0, ORDSTATUS_OPEN_TRIGGERED, ORDSTATUS_OPEN_MT4_1, ORDSTATUS_OPEN_MT4_2, ORDSTATUS_CLOSE_TRIGGERED, ORDSTATUS_CLOSE_MT4_1, ORDSTATUS_CLOSE_MT4_2};


struct TData
{
    char        symbol[32];    // __ALPHA::LEN_SYMBOL + 1];
    int         nOrdStatus;
    char        zDBSerial[32];          // Serial which is made when save data in DB. YYYYMMDD(8)+SEQNO(10)
    int         nCloseNetPLTriggered;   // calculated value
    int         nProfitCnt;
    int         nLossCnt;
    
    int         nRoundCnt;              // round(Open and Close) count
    long        lMagicNo;
    TSymbolSpec Spec;
    TBestPrcForOpen    BestPrcForOpen;
    TPos        Long;
    TPos        Short;
};

//struct TPosition
//{
//    char symbol[__ALPHA::LEN_SYMBOL + 1];
//    bool bOpen;
//    TPos Long;
//    TPos Short;
//};


struct TBroker
{
    char brokerKey[32];
    char BrokerName[64];
    double Balance;
    double Equity;
    double FreeMgn;
    int Lvg;
    bool bStartMD;
    char zBid[32];
    char zAsk[32];
};


struct TSendOrder
{
    char brokerKey[32];
    char zSendBuf[MAX_BUF];
};

void LOGGING(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...);
void LOG_DATA(const char* pMsg, ...);