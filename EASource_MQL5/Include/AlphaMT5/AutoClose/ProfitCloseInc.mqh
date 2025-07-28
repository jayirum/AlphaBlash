#ifndef __PROFITCLOSE_INC_H__
#define __PROFITCLOSE_INC_H__


#include "../_IncCommon.mqh"
#include "../Utils.mqh"
#include "../CSearchStringArray.mqh"
#include "../CSearchIntegerArray.mqh"
#include "../ValidateUtils.mqh"

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

enum  EN_REPEAT_TP
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

enum EN_PL_METRIC
{
   enCurrency = 0
   ,enPoints 
};
/////////////////////////////////////////////////////////////////////////////////


input string   Setting1 = "---------- EA Settings ----------";
input EN_USE_NOUSE      ActivateEA = Disabled; // Activate EA

input string   Setting2 = "---------- Close by P&L Settings ----------";
input EN_USE_NOUSE      OneTimeClose=Disabled;              //Enable One-Time Close [select]
input EN_USE_NOUSE      StopEA_AfterOneTimeclose=Disable;   //Disable EA after One-Time Close [select]
input EN_USE_NOUSE      RepeatClose=Disabled;               //Enable Repeat Close [select]
input string   Setting3 = "---------- Time Settings for repeat close ----------";
input EN_USE_NOUSE      RepeatClose=Disabled;         //Enable Repeat Close [select]
input EN_REPEAT_TP      RepeatType=DAILY_TIME;        //Repeat Close Type [select]
input string            CloseTimeOfDaily="hh:mm";     //Close on All Weekdays at this Daily time
input string            CloseTimeOnMonday="hh:mm";    //Close on Monday at this Custom time
input string            CloseTimeOnTuesday="hh:mm";   //Close on Tuesday at this Custom time
input string            CloseTimeOnWedneday="hh:mm";  //Close on Wednesday at this Custom time
input string            CloseTimeOnThusday="hh:mm";   //Close on Thursday at this Custom time
input string            CloseTimeOnFriday="hh:mm";    //Close on Friday at this Custom time

input string   Setting4 = "---------- Order Settings ----------";
input EN_CLOSE_TP       CloseTp=ALL_ORDERS;           //Order Type to Close / Delete [select]
input EN_USE_NOUSE      CloseMagicNoOnly=Disabled;    //Close by Magic Numbers only [select]
input string            MagicNumbers="";              //Magic Numbers to close
input EN_USE_NOUSE      CloseSymbolsOnly=Disabled;    //Close by Symbols only [select]
input string            SymbolsToClose="";            //Symbols to close



bool IsActiveEA()
{
   return (ActivateEA==Enabled);
}

bool IsActive_OneTimeClose()
{
   return (OneTimeClose==Enabled);
}

bool IsActive_Repeat_Daily()
{
   if( RepeatClose==Enabled && RepeatType==DAILY_TIME )
      return true;
      
   return false;
}


bool IsActive_Repeat_Custom()
{
   if( RepeatClose==Enabled && RepeatType==WEEKDAY_TIME )
      return true;
      
   return false;
}

bool Validate_InputData()
{

   ////////////////////////////////////////////////////
   // Setting2
   if( IsActive_OneTimeClose() )
   {
      if( !validate_date(OneTimeCloseDate) )
      {
         showErr("Input the right format for [Close once on this day]", true, true);
         return false;
      }
      
      if( !validate_time(OneTimeCloseTime))
      {
         showErr("Input the right format for [Close once on this time]", true, true);
         return false;
      }
   }
   // Setting2
   ////////////////////////////////////////////////////

  
   ////////////////////////////////////////////////////
   // Setting3
   if(IsActive_Repeat_Daily())
   {
      if(IsActive_OneTimeClose() )
      {
         showErr("Disable [OneTimeClose] first", true, true);
         return false;
      }
      
      if( !validate_time(CloseTimeOfDaily)!=5)
      {
         showErr("Input the right format for [Close DAILY_TIME at this time]", true, true);
         return false;
      }
   }
   
   if(IsActive_Repeat_Custom())
   {
      if( StringTrimRight(CloseTimeOnMonday) == "" &&
          StringTrimRight(CloseTimeOnTuesday) == "" &&
          StringTrimRight(CloseTimeOnWedneday) == "" &&
          StringTrimRight(CloseTimeOnThusday) == "" &&
          StringTrimRight(CloseTimeOnFriday) == "" )
      {
         showErr("Custom type needs at least one close time from Monday to Friday",true, true);
         return false;
      }
      
      //if( StringSubstr(CloseTimeOnMonday,2,1) != ":" &&
      //    StringSubstr(CloseTimeOnTuesday,2,1) != ":" &&
      //    StringSubstr(CloseTimeOnWedneday,2,1) != ":" &&
      //    StringSubstr(CloseTimeOnThusday,2,1) != ":" &&
      //    StringSubstr(CloseTimeOnFriday,2,1) != ":" )
      //{
      //   showErr("Input the right format for close time", true, true);
      //   return false;
      //}
      if( CloseTimeOnMonday != "hh:mm" )
      {
         if(!validate_time(CloseTimeOnMonday) ) return false;            
      }
      if( CloseTimeOnTuesday != "hh:mm" )
      {
         if(!validate_time(CloseTimeOnTuesday) ) return false;            
      }
      if( CloseTimeOnWedneday != "hh:mm" )
      {
         if(!validate_time(CloseTimeOnWedneday) ) return false;            
      }
      if( CloseTimeOnThusday != "hh:mm" )
      {
         if(!validate_time(CloseTimeOnThusday) ) return false;            
      }
      if( CloseTimeOnFriday != "hh:mm" )
      {
         if(!validate_time(CloseTimeOnFriday) ) return false;            
      }
   }
   // Setting3
   ////////////////////////////////////////////////////


   ////////////////////////////////////////////////////
   // Setting4
   if( CloseMagicNoOnly==Enabled )
   {
      if( StringTrimRight(MagicNumbers)=="" )
      {
         showErr("Input magic number(s) to close", true, true);
         return false;
      }
      if(!RetrieveMagicNos_fromInput())
         return false;
   }
   if( CloseSymbolsOnly==Enabled )
   {
      if( StringTrimRight(SymbolsToClose)=="" )
      {
         showErr("Input symbol(s) to close",true,true);
         return false;
      }
      if(!RetrieveSymbols_fromInput())
         return false;
   }

   return true;
}



bool RetrieveSymbols_fromInput()
{
   string arrSymbolsName[];
   
   ushort deli = StringGetChar(",",0);
   int nCnt = StringSplit(SymbolsToClose, deli, arrSymbolsName);
   
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
   int nCnt = StringSplit(MagicNumbers, deli, arrMagicNo);
   
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