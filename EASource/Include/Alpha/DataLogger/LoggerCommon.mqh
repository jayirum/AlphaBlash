#ifndef __MARKET_DATA_LOGGER_PRO__
#define __MARKET_DATA_LOGGER_PRO__


#include "../_IncCommon.mqh"
#include "../Utils.mqh"
#include "../UtilDateTime.mqh"


//#define __FREE_VERSION__
//#define __MD_LOGGER__
#define __USER_LOGGER__




enum EN_USE_NOUSE
{

#ifndef __FREE_VERSION__   
   Disabled=0,
#endif   
   Enabled=1
};


enum EN_DISABLE_FORFREE
{
   Pro_version_only=0,
};


enum  EN_DURATION
{
   Once=0
   
   #ifndef __FREE_VERSION__   
   ,Daily
   #endif   
};


#ifdef __MD_LOGGER__
   enum EN_MDLOGGER_FREQ
   {
      #ifndef __FREE_VERSION__      
      Tick = 0,
      #endif      
      
      M1=1
      
      #ifndef __FREE_VERSION__   
      ,M5=2
      ,M15=3
      ,M30=4
      ,H1=5
      ,H4=6
      #endif
   };
#endif

#ifdef __USER_LOGGER__
   
      enum EN_USERLOGGER_FREQ
      {
      #ifndef __FREE_VERSION__
         Sec = 0
         ,
      #endif
         M1
      #ifndef __FREE_VERSION__
         ,M5
         ,M15
         ,M30
         ,H1
         ,H4
      #endif
      };
#endif

enum EN_OUTPUT_FOLDER
{
   MQL4_Files = 0,
#ifndef __FREE_VERSION__
  Terminal_Common_Files
#endif  
   
};

#define FILE_SUB_DIR             "MarketDataLogger"
#define FILE_SUB_DIR_USERSTAT    "UserStatLogger"

enum EN_LOGGER_TP { LOGGER_TP_MD, LOGGER_TP_USER } ;

//0804
bool IsTerminateTime(datetime dtNow, string sTerminateTime)
{
   string sDayOfWeek = __DayOfWeek(dtNow);
   if( sDayOfWeek==DEF_FRIDAY )
   {
      string sNowTime = __HHMMToStr(dtNow); // hh:mm
      if( StringCompare(sNowTime, sTerminateTime) >=0 )
         return true;
   }
   else if ( sDayOfWeek==DEF_SATURDAY )
      return true;
      
   return false;

}

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
//   if( 
//      (StringGetChar(sStartLogging,0) < 48 || StringGetChar(sStartLogging,0) > 57) ||
//      (StringGetChar(sStartLogging,1) < 48 || StringGetChar(sStartLogging,1) > 57) ||  
//      (StringGetChar(sStartLogging,3) < 48 || StringGetChar(sStartLogging,3) > 57) ||
//      (StringGetChar(sStartLogging,4) < 48 || StringGetChar(sStartLogging,4) > 57) ||
//
//      (StringGetChar(sStopLogging,0) < 48 || StringGetChar(sStopLogging,0) > 57) ||
//      (StringGetChar(sStopLogging,1) < 48 || StringGetChar(sStopLogging,1) > 57) ||  
//      (StringGetChar(sStopLogging,3) < 48 || StringGetChar(sStopLogging,3) > 57) ||
//      (StringGetChar(sStopLogging,4) < 48 || StringGetChar(sStopLogging,4) > 57)
//      )
//   {
//      Alert("Please input the valid time");
//      return false;
//   }

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
       StringToInteger( StringSubstr(sStartLogging, 0, 2) ) > 23 ||
       StringToInteger( StringSubstr(sStartLogging, 3, 2) ) > 59 ||
       StringToInteger( StringSubstr(sStopLogging, 0, 2) ) > 23 ||
       StringToInteger( StringSubstr(sStopLogging, 3, 2) ) > 59 
    )
    {
      Alert("Please input the valid time");
      return false;
    }
   
   
   int comp = StringCompare(sStartLogging, sStopLogging);
   if( comp >= 0 )
   {
      string msg = StringFormat("[%d]StartLogging(%s) must be earlier than StopLogging(%s)",comp, sStartLogging, sStopLogging);
      Alert(msg);
      return false;
   }
   return true;
}

#endif