//+------------------------------------------------------------------+
//|                                                      Version.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict


#ifndef __PLATFORMVER_H__
#define __PLATFORMVER_H__

/*****************************************************************************************
*
*
*
*
*****************************************************************************************/
#define __MT5__
//#define __MT4__

//////////////////////////////////////////////////////////////////////////////////////

bool __IsMT4()
{
   if (TerminalInfoInteger(TERMINAL_X64))
      return false;
   return true;
}


bool __IsMT5()
{
   return (!__IsMT4());
}


string __Platform()
{
   string s;
   if (TerminalInfoInteger(TERMINAL_X64))
      s = "MT5";
   else
      s = "MT4";

   return s;      
}


//////////////////////////////////////////////////////////////////////////////////////




#endif