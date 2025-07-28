#ifndef __COPIER_LOCAL_HEADER__
#define __COPIER_LOCAL_HEADER__


#include "C_CfgSymbols.mqh"
#include "C_CfgGeneral.mqh"
#include "../Copier/MasterOrders.mqh"
#include "../Copier/CopierOrdFactor.mqh"
#include "../Copier/PlaceMT4Order.mqh"
    

//+----------------------------------------------------------------------------------------
// Global Variables

string   EA_NAME = "AlphaCopierL";
uint     SleepMS;


#define  MAX_DELETED_ORDERS  30
int     _DeletedTicketArray [MAX_DELETED_ORDERS];

string   _sRecvBuff;
char     _zRecvBuff[BUF_LEN];
char     _zMsg[BUF_LEN];
string   _sMsg;
bool     _IsTradableStatus;
//bool     _bMasterLogon;
//bool     _bMeLogon;

string   _sSvrIp, _sSvrPort, _sSendTimout, _sRecvTimeout;

//+----------------------------------------------------------------------------------------


//+----------------------------------------------------------------------------------------
//  Classes
CSleep            _sleep;   // 0.01 sec
CCfgSymbols       _symbolMapping;
CLog              _log;
CMasterOrders     _masterOrd;
CCfgGeneral       _config;
CCopierOrdFactor  _copierOrdFactor;

//+----------------------------------------------------------------------------------------


//void SetMasterLogon()   { _bMasterLogon=true; }
//void SetMasterLogoff()  { _bMasterLogon=false; }
//void SetMeLogon()       { _bMeLogon = true; }
//void SetMeLogoff()      { _bMeLogon = false; }


void SetTradableStatus()   { _IsTradableStatus=true; }
void UnsetTradableStatus() { _IsTradableStatus = false; }
bool IsTradableStatus()    { return _IsTradableStatus;}

//bool IsTradableStatus()
//{
//   if( _bMasterLogon && _bMeLogon )
//      return true;
//   
//   return false;
//}
//void UnsetTradable()
//{
//   _bMasterLogon = _bMeLogon = false;
//}

#endif