//+------------------------------------------------------------------+
//|                                               iAlphaThreeEma.mq4 |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict
#property indicator_chart_window

#property indicator_buffers   3



enum EN_TIMEFRAME
{
   //M0    = 0,
   M1    = 1,
   M5    = 5,
   M15   = 15,
   M30   = 30,
   H1    = 60,
   H4    = 240,
   D1    = 1440,
   W1    = 10080,
   MN1   = 43200
};

input string   I_Symbol;
input int      I_TimeFrame;
input int      I_FastEmaPeriod = 25;
input int      I_MiddleEmaPeriod = 50;
input int      I_SlowEmaPeriod = 100;

double BuffFast[];
double BuffMiddle[];
double BuffSlow[];

enum { IDX_FAST=0, IDX_MIDDLLE, IDX_SLOW};

string   _SYMBOL;
int      _TIMEFRAME;

//+------------------------------------------------------------------+
//| Custom indicator initialization function                         |
//+------------------------------------------------------------------+
int OnInit()
{
   _SYMBOL = I_Symbol;
   if(I_Symbol=="")
      _SYMBOL = Symbol();
      
   _TIMEFRAME = (int)I_TimeFrame;   
   if(I_TimeFrame==0)
      _TIMEFRAME = Period();
      
//--- indicator buffers mapping
   SetIndexStyle(IDX_FAST, DRAW_LINE, STYLE_SOLID, 2, clrAqua);
   SetIndexStyle(IDX_MIDDLLE, DRAW_LINE, STYLE_SOLID, 2, clrYellow);
   SetIndexStyle(IDX_SLOW, DRAW_LINE, STYLE_SOLID, 2, clrRed);
   
   SetIndexBuffer(IDX_FAST, BuffFast);
   SetIndexBuffer(IDX_MIDDLLE, BuffMiddle);
   SetIndexBuffer(IDX_SLOW, BuffSlow);
   
   SetIndexLabel(IDX_FAST, "Fast");
   SetIndexLabel(IDX_MIDDLLE, "Middle");
   SetIndexLabel(IDX_SLOW, "Yellow");
   
//---
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Custom indicator iteration function                              |
//+------------------------------------------------------------------+
int OnCalculate(const int rates_total,
                const int prev_calculated,
                const datetime &time[],
                const double &open[],
                const double &high[],
                const double &low[],
                const double &close[],
                const long &tick_volume[],
                const long &volume[],
                const int &spread[])
  {
//---

   int limit;
   
   
   if( rates_total <= I_FastEmaPeriod )
      return 0;
      
   limit = rates_total - prev_calculated;

   for( int i=limit-1; i>-1; i-- )
   {
      BuffFast[i] = iMA(_SYMBOL, _TIMEFRAME, I_FastEmaPeriod, 0, MODE_EMA, PRICE_CLOSE, i);
      BuffMiddle[i] = iMA(_SYMBOL, _TIMEFRAME, I_MiddleEmaPeriod, 0, MODE_EMA, PRICE_CLOSE, i);
      BuffSlow[i] = iMA(_SYMBOL, _TIMEFRAME, I_SlowEmaPeriod, 0, MODE_EMA, PRICE_CLOSE, i);      
   }
   
   
//--- return value of prev_calculated for next call
   return(rates_total);
  }
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
  {
//---
   
  }
//+------------------------------------------------------------------+
//| ChartEvent function                                              |
//+------------------------------------------------------------------+
void OnChartEvent(const int id,
                  const long &lparam,
                  const double &dparam,
                  const string &sparam)
  {
//---
   
  }
//+------------------------------------------------------------------+
