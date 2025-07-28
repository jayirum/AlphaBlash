#ifndef __AUTO_MACD_PRO_INC__
#define __AUTO_MACD_PRO_INC__


#include "../_IncCommon.mqh"
#include "../Utils.mqh"
#include "../UtilDateTime.mqh"
#include "../CTimeFrameFreq.mqh"


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                     defines
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

const string START_TIME                = "00:05";  
const string STOP_TIME                 = "23:55";
const string TERMINATE_TIME_ON_FRIDAY  = "23:55";

const double   ORD_LOT           = 0.01;
const int      ALLOWD_SLIPPAGE   = 10;


#define __VER_1__
//#define __MD_LOGGER__
//#define __USER_LOGGER__


enum EN_USE_NOUSE
{
   Disabled=0,
   Enabled=1
};


//enum EN_DISABLE_FORFREE
//{
//   Pro_version_only=0,
//};
//
//
//enum  EN_DURATION
//{
//   Once=0
//   
//   #ifndef __FREE_VERSION__   
//   ,Daily
//   #endif   
//};


   

enum EN_OUTPUT_FOLDER
{
   MQL4_Files = 0,
  Terminal_Common_Files
   
};

#define FILE_SUB_DIR             "AutoMacd"
//#define FILE_SUB_DIR_USERSTAT    "UserStatLogger"/
//enum EN_LOGGER_TP { LOGGER_TP_MD, LOGGER_TP_USER } ;



///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                     input
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __VER_1__
input string         I_StartTime     = START_TIME;   //Trading start time
input string         I_StopTime      = STOP_TIME;    //Trading stop time
input EN_USE_NOUSE   I_MarketClose   = Disabled;     //Close all orders at trading stop time

input EN_USE_NOUSE   I_TradeOnMon    = Disabled;    //Trade on Monday 
input EN_USE_NOUSE   I_TradeOnTue    = Disabled;    //Trade on Tuesday
input EN_USE_NOUSE   I_TradeOnWed    = Disabled;    //Trade on Wednesday
input EN_USE_NOUSE   I_TradeOnThur   = Disabled;    //Trade on Thursday
input EN_USE_NOUSE   I_TradeOnFri    = Disabled;    //Trade on Friday
#endif

input EN_TIME_FRAME  I_TimeFrame     = TFRAME_M1; //Get indicator values for this Timeframe
input string         I_Symbols       = "EURUSD";    //Get indicator values for these Symbols
input int            I_FastEma       = 12;          //Fast EMA Period
input int            I_SlowEma       = 26;          //Slow EMA Period
input int            I_SignalSma     = 9;           //Signal SMA Period

input EN_USE_NOUSE   I_OpenB_HisOverZero   = Enabled; //Open Buy  when Histogram value is > 0
input EN_USE_NOUSE   I_OpenB_HisBelowZero  = Disabled; //Open Buy  when Histogram value is < 0
input EN_USE_NOUSE   I_OpenS_HisOverZero   = Disabled; //Open Sell when Histogram value is > 0
input EN_USE_NOUSE   I_OpenS_HisBelowZero  = Enabled; //Open Sell when Histogram value is < 0



///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                     input
//
///////////////////////////////////////////////////////////////////////////////////////////////////////


bool Validate_InputTime(string sStartLogging, string sStopLogging)
{
  
   // hh:mm
   if( StringLen(sStartLogging)!=5 || StringLen(sStopLogging)!=5 )
   {
      Alert("WriteTime format is hh:mm");
      return false;
   }
   
   if( StringSubstr(sStartLogging,2,1)!=":" || StringSubstr(sStopLogging,2,1)!=":" )
   {
      Alert("WriteTime format is hh:mm");
      return false;
   }
   
   // are they number?  ascii code 48 - 0, 57 - 9
   if( 
      !__IsNumberAscii(StringGetChar(sStartLogging,0)) ||
      !__IsNumberAscii(StringGetChar(sStartLogging,1)) ||  
      !__IsNumberAscii(StringGetChar(sStartLogging,3)) ||
      !__IsNumberAscii(StringGetChar(sStartLogging,4)) ||

      !__IsNumberAscii(StringGetChar(sStopLogging,0)) ||
      !__IsNumberAscii(StringGetChar(sStopLogging,1)) ||
      !__IsNumberAscii(StringGetChar(sStopLogging,3)) ||
      !__IsNumberAscii(StringGetChar(sStopLogging,4))
      )
   {
      Alert("Please input the valid time");
      return false;
   }
   
   if( 
       !__IsValidHour( StringSubstr(sStartLogging, 0, 2) ) ||
       !__IsValidMin( StringSubstr(sStartLogging, 3, 2) )  ||
       !__IsValidHour( StringSubstr(sStopLogging, 0, 2) )  ||
       !__IsValidMin( StringSubstr(sStopLogging, 3, 2) ) 
    )
    {
      Alert("Please input the valid time");
      return false;
    }
   
   
   //int comp = StringCompare(sStartLogging, sStopLogging);
   //if( comp >= 0 )
   //{
   //   string msg = StringFormat("[%d]StartLogging(%s) must be earlier than StopLogging(%s)",comp, sStartLogging, sStopLogging);
   //   Alert(msg);
   //   return false;
   //}
   return true;
}

#endif