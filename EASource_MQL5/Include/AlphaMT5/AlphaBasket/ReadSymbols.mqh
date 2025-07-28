#ifndef __READ_SYMBOLS_H__
#define __READ_SYMBOLS_H__
//
#include "AlphaBasketCommon.mqh"
#include "../_IncCommon.mqh"

bool ReadSymbols_from_IniFile(_In_ string sIniFileName, _Out_ string& sMsg, _Out_ TMD& arrMD[])
{
   string sSymbols;
   if(__Ini_GetVal(sIniFileName, "SYMBOLS", BROKER_KEY, sSymbols)==false)
   {
      sMsg = "Input same BrokerKey in the ini file or verify ini file";
      return false;
   }
      
   string arrSymbols[];
   ushort deli = StringGetChar(",",0);
   int nCnt = StringSplit(sSymbols, deli, arrSymbols);

   ArrayResize(arrMD, nCnt);
   for( int i=0; i<nCnt; i++ )
   {
      arrMD[i].sSymbol = arrSymbols[i];
   }

   PrintFormat("[%d](%s)", ArraySize(arrMD), sSymbols);

   return true;
}



#endif