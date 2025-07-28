//+------------------------------------------------------------------+
//|                                                  BPSocketDll.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict



#ifndef __BP_TESTUI__
#define __BP_TESTUI__


//#include "BPUtils.mqh"
//#include "BPCommon.mqh"
//#include "mt4gui2.mqh"

#import "MFCLibrary1.dll"
  
   void LoadUI(); 
#import "Kernel32.dll"
   int LoadLibraryA(char& lpLibFileName[]);
   int LoadLibraryW(char& lpLibFileName[]);
   int GetModuleHandleA(char& lpLibFileName[]);
   int GetModuleHandleW(char& lpLibFileName[]);
   bool FreeLibrary(int h);
   void FreeLibraryAndExitThread(
  int hLibModule,
  int   dwExitCode
);
#import


void loadUI()
{
   LoadUI();
}



#endif

