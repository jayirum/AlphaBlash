#ifndef __ALPHA_SWING_H__
#define __ALPHA_SWING_H__



#include "../include/Alpha/_IncCommon.mqh"
#include "../include/Alpha/COrderMT4.mqh"
#include "../include/Alpha/UtilDateTime.mqh"
#include "../include/Alpha/AlphaBasket/AlphaBasketCommon.mqh"
#include "../include/Alpha/AlphaSwing/GUIButton.mqh"

#include "../include/alpha/CExpertEnabled.mqh"
#include "../include/alpha/CIniFile.mqh"
///////////////////////////////////////////////////////////////////////

input string   I_SYMBOLS         = "";//SYMBOLS (Use "," for multiple symbols)
input int      I_TP_PIPS         = 30;//TP_PIPS (Must be 3 times bigger than SL_PIPS)
input int      I_SL_PIPS         = 10;//SL_PIPS 
input double   I_START_LOTS      = 0.1;
input double   I_LOTS_MULTIPLIER = 1.33;

input int      I_TIMEOUT_MDFETCH = 1;
input int      I_TIMEOUT_RECONN  = 3000;
input int      I_SLIPPAGE_PIP    = 3;
input int      I_RETRYCNT_ORD    = 3;

string   _Symbols;

// order related
CorderMT4  _ordHandler;

/*
   STEP_READY : Before Entry
   STEP_1ST_ENTRY : First Entry
   STEP_1ST_RVS : First Reverse
   STEP_2ND_ENTRY : 2nd Entry
   STEP_2ND_RVS : 2nd Reverse
   STEP_2ND_ENTRY : 3rd Entry
   STEP_2ND_RVS : 3rd Reverse
   STEP_4TH_ENTRY : 4th Entry
   STEP_4TH_ENTRY : 4th Reverse
   
*/
#define MAX_STEP  9
enum SWING_STEP 
{ 
   STEP_READY=0, 
   STEP_1ST_ENTRY, 
   STEP_1ST_RVS, 
   STEP_2ND_ENTRY, 
   STEP_2ND_RVS, 
   STEP_3RD_ENTRY, 
   STEP_3RD_RVS, 
   STEP_4TH_ENTRY, 
   STEP_4TH_RVS,
   STEP_LAST_CLOSE
};
SWING_STEP     _CurrSteps;

string _StartDirection;   //DEF_BUY or DEF_SELL

struct TPrices
{
   double   entryPrc;
   double   tpEntry;
   double   sl;
   double   tpReverse;
};

#define MAX_SYMBOLS  2
struct TSwingInfo
{
   string      sSymbol;
   SWING_STEP  NowStep;
   string      startDirection;
   TPrices     Prices;
   double      Lots;
   datetime    dtLastMDtime;
};
TSwingInfo  _Swings[];


// 
CExpertEnabled *_oExpertEnabled;
CIniFile       *_oIni;
CPanelDialog   _GuiDlg;

string   _sMsg;



#endif