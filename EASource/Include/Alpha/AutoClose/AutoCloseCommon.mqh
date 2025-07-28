#ifndef __AUTOCLOSE_COMMON_H__
#define __AUTOCLOSE_COMMON_H__


#include "../_IncCommon.mqh"
#include "../Utils.mqh"
#include "../CSearchStringArray.mqh"
#include "../CSearchIntegerArray.mqh"

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
   Daily=0
   ,Custom
};



enum  EN_CLOSE_TP
{
   ALL=0
   ,MARKET_ORDER
   ,PENDING_ORDER
};
/////////////////////////////////////////////////////////////////////////////////


input string   Setting1 = "---------- EA SETTING ----------";
input EN_USE_NOUSE      ActivateEA = Enabled; // Activate EA

input string   Setting2 = "---------- Time Settings for one-time close ----------";
input EN_USE_NOUSE      OneTimeClose=Enabled;               //Enable one-time close
input string            OneTimeCloseDate="yyyy.mm.dd";  //Close once on this day 
input string            OneTimeCloseTime="hh:mm";      //Close once on this time

input string   Setting3 = "---------- Time Settings for repeat close ----------";
input EN_USE_NOUSE      RepeatClose=Disabled;         //Enable repeat close
input EN_REPEAT_TP      RepeatType=Daily;             //Repeat close type
input string            CloseTimeOfDaily="hh:mm";     //Close daily at this time
input string            CloseTimeOnMonday="hh:mm";    //Close on Monday at this time
input string            CloseTimeOnTuesday="hh:mm";   //Close on Tuesday at this time
input string            CloseTimeOnWedneday="hh:mm";  //Close on Wednesday at this time
input string            CloseTimeOnThusday="hh:mm";   //Close on Thursday at this time
input string            CloseTimeOnFriday="hh:mm";    //Close on Friday at this time

input string   Setting4 = "---------- Order Settings ----------";
input EN_CLOSE_TP       CloseTp=ALL;                  //Order to close/delete
input EN_USE_NOUSE      CloseMagicNoOnly=Disabled;    //Close orders by Magic Number(s) only
input string            MagicNumbers="";              //Magic Number(s) to close
input EN_USE_NOUSE      CloseSymbolsOnly=Disabled;    //Close orders by symbol(s) only
input string            SymbolsToClose="";            //Symbol(s) to close

input string   Setting5 = "---------- Other Settings ----------";
input string            OrderComments=""; //Custom Order Comment



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
   if( RepeatClose==Enabled && RepeatType==Daily )
      return true;
      
   return false;
}


bool IsActive_Repeat_Custom()
{
   if( RepeatClose==Enabled && RepeatType==Custom )
      return true;
      
   return false;
}

bool Validate_InputData()
{

   ////////////////////////////////////////////////////
   // Setting2
   if( IsActive_OneTimeClose() )
   {
      if( StringLen(OneTimeCloseDate)!=10 ||
          StringSubstr(OneTimeCloseDate,4,1) != "." ||
          StringSubstr(OneTimeCloseDate,7,1) != "." 
         )
      {
         showErr("Input the right format for [Close once on this day]", true, true);
         return false;
      }
      
      if( StringToInteger(StringSubstr(OneTimeCloseDate,0,4))==0 ||
          StringToInteger(StringSubstr(OneTimeCloseDate,5,2))==0 ||
          StringToInteger(StringSubstr(OneTimeCloseDate,8,2))==0 
        )  
      {
         showErr("Input the right format for [Close once on this day]", true, true);
         return false;
      }
      
      if( StringLen(OneTimeCloseTime)!=5 ||
          StringSubstr(OneTimeCloseTime,2,1) != ":" 
         )
      {
         showErr("Input the right format for [Close once on this time]", true, true);
         return false;
      }
      if( StringToInteger(StringSubstr(OneTimeCloseTime,0,2))==0 ||
          StringToInteger(StringSubstr(OneTimeCloseTime,3,2))==0 
        )
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
      
      if( StringLen(CloseTimeOfDaily)!=5 ||
       StringSubstr(CloseTimeOfDaily,2,1) != ":" 
      )
      {
         showErr("Input the right format for [Close daily at this time]", true, true);
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
      
      if( StringSubstr(CloseTimeOnMonday,2,1) != ":" &&
          StringSubstr(CloseTimeOnTuesday,2,1) != ":" &&
          StringSubstr(CloseTimeOnWedneday,2,1) != ":" &&
          StringSubstr(CloseTimeOnThusday,2,1) != ":" &&
          StringSubstr(CloseTimeOnFriday,2,1) != ":" )
      {
         showErr("Input the right format for close time", true, true);
         return false;
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