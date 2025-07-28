#pragma once

#pragma warning( disable : 26495)
#include "../../Common/AlphaInc.h"
#include "../../Common/AlphaProtocol.h"
#include "../../Common/LogMsg.h"
#include "CConfig.h"

#define TYPE_USER_ID	std::string
#define TYPE_APP_ID		std::string

#define DEF_CLIENT_SOCKTP_RECV 'R'
#define DEF_CLIENT_SOCKTP_SEND 'S'

#define DEF_APPTP_MANAGER   "M"
#define DEF_APPTP_EA        "E"

#define DEF_SVRTP_AUTH  "A"
#define DEF_SVRTP_RELAY "R"
#define DEF_SVRTP_DATA  "D"

enum EN_APP_TP { APPTP_EA, APPTP_MANAGER};
enum EN_LOGONOUT { EN_LOGON, EN_LOGOUT};

struct TRecvData
{
    char data[__ALPHA::LEN_BUF];
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



void LOGGING(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...);
void LOG_DATA(const char* pMsg, ...);
char* AppTp_S(EN_APP_TP appTp);
