#ifndef __COPIER_HEADER__
#define __COPIER_HEADER__


#include "CopierConfig_Symbols.mqh"
#include "CopierConfig_General.mqh"
#include "MasterOrders.mqh"
#include "CopierOrdFactor.mqh"
//#include "PlaceMT4Order.mqh"


//--- input parameters
input string    InputMyID  = "JAYKIM_C";
input string   InputMyPwd  = "111111";
    

//+----------------------------------------------------------------------------------------
// Global Variables

uint     SleepMS;;
string   EA_NAME = "Alpha-Blash Copier";

#define  MAX_DELETED_ORDERS  30
int     _DeletedTicketArray [MAX_DELETED_ORDERS];

char     _zRecvBuff[BUF_LEN];
char     _zMsg[BUF_LEN];
string   _sMsg;
bool     _bMasterLogon;
bool     _bMeLogon;
//bool     _IsTradableStatus;
//+----------------------------------------------------------------------------------------


//+----------------------------------------------------------------------------------------
//  Classes
CSleep         _sleep;   // 0.01 sec
CMasterOrders  _masterOrd;
CConfigGeneral _config;
CSymbolMapping _symbolMapping;
CCopierOrdFactor _copierOrdFactor;
//+----------------------------------------------------------------------------------------


void SetMasterLogon()   { _bMasterLogon=true; }
void SetMasterLogoff()  { _bMasterLogon=false; }
void SetMeLogon()       { _bMeLogon = true; }
void SetMeLogoff()      { _bMeLogon = false; }
bool IsTradableStatus()
{
   if( _bMasterLogon && _bMeLogon )
      return true;
   
   return false;
}
void UnsetTradable()
{
   _bMasterLogon = _bMeLogon = false;
}

#endif