#ifndef __MASTER_HEADER_LOCAL__
#define __MASTER_HEADER_LOCAL__


//--- input parameters
//input string   InputMyID         = "JAYKIM_M";
//input string   InputMyPwd        = "111111";
    

//+----------------------------------------------------------------------------------------
// Global Variables

string   EA_NAME = "AlphaMasterL";

uint     SleepMS ;

#define  MAX_DELETED_ORDERS  30
int     _DeletedTicketArray [MAX_DELETED_ORDERS];
int     _DeletedGroupKeyArray [MAX_DELETED_ORDERS];
double  _DeletedOrgLotsArray [MAX_DELETED_ORDERS];

string   _sRecvBuff;
char     _zRecvBuff[BUF_LEN];
char     _zMsg[BUF_LEN];
string   _sMsg;
bool     _IsTradableStatus;

string   _DB_M_ORD   = "AlphaDB_M.ini";  

string   _sSvrIp, _sSvrPort, _sSendTimout, _sRecvTimeout;
//+----------------------------------------------------------------------------------------


//+----------------------------------------------------------------------------------------
//  Classes
CSleep         _sleep();   // 0.01 sec
//CConfigSymbol  _configSymbol;
CLog           _log;
//+----------------------------------------------------------------------------------------


void SetTradableStatus()   { _IsTradableStatus=true; }
void UnsetTradableStatus() { _IsTradableStatus = false; }
bool IsTradableStatus()    { return _IsTradableStatus;}
#endif