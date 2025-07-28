#ifndef __MASTER_HEADER__
#define __MASTER_HEADER__


//--- input parameters
input string   InputMyID         = "JAYKIM_M";
input string   InputMyPwd        = "111111";
    

//+----------------------------------------------------------------------------------------
// Global Variables

uint     SleepMS ;

string   EA_NAME = "Alpha-Blash Master";

#define  MAX_DELETED_ORDERS  30
int     _DeletedTicketArray [MAX_DELETED_ORDERS];
int     _DeletedGroupKeyArray [MAX_DELETED_ORDERS];
double  _DeletedOrgLotsArray [MAX_DELETED_ORDERS];

char     _zRecvBuff[BUF_LEN];
char     _zMsg[BUF_LEN];
string   _sMsg;
bool     _IsTradableStatus;
//+----------------------------------------------------------------------------------------


//+----------------------------------------------------------------------------------------
//  Classes
CSleep         _sleep();   // 0.01 sec
CConfigSymbol  _configSymbol;
//+----------------------------------------------------------------------------------------


void SetTradableStatus()   { _IsTradableStatus=true; }
void UnsetTradableStatus() { _IsTradableStatus = false; }
bool IsTradableStatus()    { return _IsTradableStatus;}
#endif