#ifndef __TIMEDCLOSE_INC_H__
#define __TIMEDCLOSE_INC_H__


#include "../_IncCommon.mqh"
#include "../Utils.mqh"
#include "../CSearchStringArray.mqh"
#include "../CSearchIntegerArray.mqh"
#include "../UtilDateTime.mqh"

CSearchStringArray   _arrSymbols;
CSearchIntegerArray  _arrMaginNo;

//#define __FREE_VERSION__

/////////////////////////////////////////////////////////////////////////////////
enum EN_USE_NOUSE
{
#ifndef __FREE_VERSION__   
   Disabled=0,
#endif   
   Enabled=1
};

enum  EN_REPEAT_TYPE
{
   DAILY_TIME = 0
   ,WEEKDAY_TIME 
};



enum  EN_CLOSE_TP
{
   ALL_ORDERS=0
   ,MARKET_ORDERS
   ,PENDING_ORDERS
};
/////////////////////////////////////////////////////////////////////////////////


input string   Setting1 = "---------- EA Settings ----------";
input EN_USE_NOUSE      I_ActivateEA = Disabled; // Activate EA

input string   Setting2 = "---------- Time Settings for one-time close ----------";
input EN_USE_NOUSE      I_OneTimeClose=Disabled;           //Enable One-Time close
input string            I_OneTimeCloseDate="yyyy.mm.dd";   //One-Time Close orders on this day
input string            I_OneTimeCloseTime="hh:mm";        //One-Time Close orders at this time

input string   Setting3 = "---------- Time Settings for repeat close ----------";
input EN_USE_NOUSE      I_RepeatClose=Disabled;         //Enable Repeat Close [select]
input EN_REPEAT_TYPE    I_RepeatType=DAILY_TIME;        //Repeat Close Type [select]
input string            I_CloseTimeOfDaily="hh:mm";     //Close on All Weekdays at this Daily time
input string            I_CloseTimeOnMonday="hh:mm";    //Close on Monday at this Custom time
input string            I_CloseTimeOnTuesday="hh:mm";   //Close on Tuesday at this Custom time
input string            I_CloseTimeOnWedneday="hh:mm";  //Close on Wednesday at this Custom time
input string            I_CloseTimeOnThusday="hh:mm";   //Close on Thursday at this Custom time
input string            I_CloseTimeOnFriday="hh:mm";    //Close on Friday at this Custom time

input string   Setting4 = "---------- Order Settings ----------";
input EN_CLOSE_TP       I_CloseTp=ALL_ORDERS;           //Order Type to Close / Delete [select]
input EN_USE_NOUSE      I_CloseMagicNoOnly=Disabled;    //Close by Magic Numbers only [select]
input string            I_MagicNumbers="";              //Magic Numbers to close
input EN_USE_NOUSE      I_CloseSymbolsOnly=Disabled;    //Close by Symbols only [select]
input string            I_SymbolsToClose="";            //Symbols to close




bool IsEnabled_ActivateEA()
{
   return (I_ActivateEA==Enabled);
}

bool IsEnabled_OnetimeClose()
{
   return (I_OneTimeClose==Enabled);
}

bool IsEnabled_RepeatClose()
{
   return (I_RepeatClose==Enabled);
}

bool IsEnabled_Repeat_Everyday()
{
   if( I_RepeatClose==Enabled && I_RepeatType==DAILY_TIME )
      return true;
      
   return false;
}


bool IsActive_Repeat_Weekday()
{
   if( I_RepeatClose==Enabled && I_RepeatType==WEEKDAY_TIME )
      return true;
      
   return false;
}

bool RetrieveSymbols_fromInput()
{
   string arrSymbolsName[];
   
   ushort deli = StringGetChar(",",0);
   int nCnt = StringSplit(I_SymbolsToClose, deli, arrSymbolsName);
   
   if( nCnt==0 )
   {
      Alert("Please input symbols to close");
      return false;
   }
   
   for( int i=0; i<nCnt; i++ )
   {
      if( arrSymbolsName[i]!="" )
      {
         if( !SymbolSelect(arrSymbolsName[0], true) )
         {
            Alert("Please input the exact symbol code as shown in Market Watch");
            return false;
         }
         _arrSymbols.AddNewValue(arrSymbolsName[i]);
         
         PrintFormat("[RetrieveSymbols_fromInput]%s", arrSymbolsName[i]);
      }
   }
   return true;
}



bool RetrieveMagicNos_fromInput()
{
   string arrMagicNo[];
   
   ushort deli = StringGetChar(",",0);
   int nCnt = StringSplit(I_MagicNumbers, deli, arrMagicNo);
   
   if( nCnt==0 )
   {
      Alert("Please input Magic numbers to close");
      return false;
   }
   
   for( int i=0; i<nCnt; i++ )
   {
      if( arrMagicNo[i]!="" )
      {
         int nMagic = (int)StringToInteger(arrMagicNo[i]);
         
         _arrMaginNo.AddNewValue(nMagic);
         
         PrintFormat("[RetrieveMagicNos_fromInput]%d", nMagic);
      }
   }
   
   return true;
}


#endif