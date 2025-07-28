#ifndef __AUTOMACD_INC_H__
#define __AUTOMACD_INC_H__


#include "../_IncCommon.mqh"
#include "../Utils.mqh"
#include "../ValidateUtils.mqh"
#include "../UtilDateTime.mqh"
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


enum EN_TIMEFRAME
{
   M1 = 0, M5, M15, M30, H1, H4, D1
};

enum EN_APPLYED_PRICE
{
   OpenPrice=0, ClosePrice
};

enum EN_OPEN_TP
{
   OpenBuy=0, OpenSell
};

enum EN_CLOSE_TP
{
   CloseBuy=0, CloseSell, Ignore
};

#define DEF_MIN_PERIOD 2
/////////////////////////////////////////////////////////////////////////////////


input string   Setting1 = "---------- EA Settings ----------";
input EN_USE_NOUSE      ActivateEA = Disabled; // Activate EA

input string   Setting2 = "---------- Trading Time Settings ----------";
input string   StartTime;  // Start Time [hh:mm]
input string   StopTime;   // Stop Time [hh:mm]

input string   Setting3 = "---------- Trading Day Settings ----------";
input EN_USE_NOUSE      TradeMonday=Disabled;        //Trade on Monday [select]
input EN_USE_NOUSE      TradeTuesday=Disabled;       //Trade on Tuesday [select]
input EN_USE_NOUSE      TradeWednesday=Disabled;     //Trade on Wednesday [select]
input EN_USE_NOUSE      TradeThursday=Disabled;      //Trade on Thursday [select]
input EN_USE_NOUSE      TradeFriday=Disabled;        //Trade on Friday [select]

input string   Setting4 = "---------- Fetch MACD Indicator values ----------";
input EN_TIMEFRAME      TimeFrame=M1;                 // Timeframe [select]
input string            Symbols;                      // Symbols [enter symbol code]

input string   Setting5 = "---------- MACD Indicator Settings ----------";
input int               FastEmaPeriod;                // Fast EMA Period [>=2]
input int               SlowEmaPeriod;                // Slow EMA Period [>=2]
input int               SignalSmaPeriod;              // Signal SMA Period [>=2]
input EN_APPLYED_PRICE  AppliedPrice;                 // Apply to Price [select]


input string   Setting6 = "---------- Trading Logic (Open orders) Settings ----------";
input EN_OPEN_TP        OpenTp_OverZero;              // if Histogram value is > 0 [select]
input EN_OPEN_TP        OpenTp_BelowZero;             // if Histogram value is < 0 [select]
input EN_OPEN_TP        OpenTp_CrossOver;             // if Histogram value is crossover 0 [select]        
input EN_OPEN_TP        OpenTp_CrossUnder;            // if Histogram value is crossunder 0 [select]


input string   Setting7 = "---------- Trading Logic (Close orders) Settings ----------";
input EN_CLOSE_TP       CloseTp_OverZero;             // if Histogram value is > 0 [select]
input EN_CLOSE_TP       CloseTp_BelowZero;            // if Histogram value is < 0 [select]
input EN_CLOSE_TP       CloseTp_CrossOver;            // if Histogram value is crossover 0 [select]
input EN_CLOSE_TP       CloseTp_CrossUnder;           // if Histogram value is crossunder 0 [select]


input string   Setting8 = "---------- Risk / Order Management Settings ----------";
input double            LotSize;                      // Lot Size
input double            MaxSpread;                    // Max Spread allowed (points) [0:Off]
input double            TakingProfit;
input double            StopLoss;
input double            TrailingStop;
input double            CloseTargetPL;
input int               MaxNumActiveOrder;
input string            Comments;
input string            MagicNumbers;

input string   Setting9 = "---------- Notification Settings ----------";
input EN_USE_NOUSE      NotiOfTrading=Disabled;       // Send email when trading Starts / Stops
input EN_USE_NOUSE      NotiOfPLReached=Disabled;     // Send email if P&L Target is reached
input EN_USE_NOUSE      NotiOfMarginLevel=Disabled;   // Send email if Margin Level is < 100%


bool IsActiveEA()
{
   return (ActivateEA==Enabled);
}

bool Validate_InputData()
{

   ////////////////////////////////////////////////////
   if( !validate_time(StartTime) || validate_time(StopTime))
   {
      showErr("Input the right format for time", true, true);
      return false;
   }

  
   ////////////////////////////////////////////////////
   if( StringLen(Symbols)==0 )
   {
      showErr("Input the symbols", true, true);
      return false;
   }

   if(FastEmaPeriod < DEF_MIN_PERIOD || 
      SlowEmaPeriod < DEF_MIN_PERIOD ||
      SignalSmaPeriod < DEF_MIN_PERIOD 
      )
   {
      string msg = StringFormat("Fast EMA Period / Slow EMA Period, Signal SMA Period must be bigger than %d", DEF_MIN_PERIOD);
      showErr(msg, true, true);
      return false;
   }

   if(FastEmaPeriod < DEF_MIN_PERIOD || 
      SlowEmaPeriod < DEF_MIN_PERIOD ||
      SignalSmaPeriod < DEF_MIN_PERIOD 
      )
   {
      string msg = StringFormat("Fast EMA Period / Slow EMA Period, Signal SMA Period must be bigger than %d", DEF_MIN_PERIOD);
      showErr(msg, true, true);
      return false;
   }

   return true;
}



bool RetrieveSymbols_fromInput()
{
   string arrSymbolsName[];
   
   ushort deli = StringGetChar(",",0);
   int nCnt = StringSplit(Symbols, deli, arrSymbolsName);
   
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


bool Is_TradeDayOfWeek()
{
   string sTodayOfWeek = __DayOfWeek();
   
   if( sTodayOfWeek==DEF_MONDAY && TradeMonday==Enabled )
      return true;

   if( sTodayOfWeek==DEF_TUESDAY && TradeTuesday==Enabled )
      return true;

   if( sTodayOfWeek==DEF_WEDNESDAY && TradeWednesdayday==Enabled )
      return true;

   if( sTodayOfWeek==DEF_THURSDAY && TradeThursday=Enabled )
      return true;

   if( sTodayOfWeek==DEF_FRIDAY && TradeFriday==Enabled )
      return true;

   return false;
}

#endif