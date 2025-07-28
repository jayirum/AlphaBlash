#ifndef __MASTER_CFG_SYMBOLS_LOCAL__
#define __MASTER_CFG_SYMBOLS_LOCAL__

#include "_AlphaCopyLCommon.mqh"
#include "../Master/MasterOMS.mqh"
#include "../IniFile.mqh"

bool __Load_TradableSymbols()
{
   string sKey;
   string sVal;
   
   if(!__Ini_GetVal(__CNFG_FILE, SEC_SYMBOL_M, "CNT", sVal))
   {
      //TODO LOG
      return false;
   }
   
   __Symbol_ClearAll();
   
   int cnt = (int)StringToInteger(sVal);
   
   for( int i=1; i<=cnt; i++)
   {
      sKey = "SYMBOL"+IntegerToString(i);
      sVal = "";
      if(!__Ini_GetVal(__CNFG_FILE, SEC_SYMBOL_M, sKey, sVal))
      {
         //TODO.
         return false;
      }
      
      if( !__Symbol_Add(sVal) )
      {
         //TODO.
         return false;
      }
      //Print(data);
      //_log.log(INFO, data); 
   }
   
   return true;
}

void __Symbol_ClearAll()
{
   AlphaOMS_TradableSymbols_Reset();
}

bool __Symbol_Add(string sSymbol)
{
   char zSymbol[32]; StringToCharArray(sSymbol, zSymbol);
   int ret = AlphaOMS_TradableSymbols_Set(zSymbol);
   
   return (ret==E_OK);
}

bool __Symbol_IsTradable(string sSymbol)
{
    char zSymbol[32]; StringToCharArray(sSymbol, zSymbol);
    int ret = AlphaOMS_IsTradableSymbol(zSymbol);
    
    return (ret==E_OK);
}



#endif