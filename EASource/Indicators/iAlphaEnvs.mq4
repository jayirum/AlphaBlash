//+------------------------------------------------------------------+
//|                                                     IND_3in1.mq4 |
//|                             Copyright 2021, Giorgi Gachechiladze |
//|           https://www.upwork.com/freelancers/~01e6281fb21a4a3862 |
//+------------------------------------------------------------------+
#property copyright "Copyright 2021, Giorgi Gachechiladze"
#property link      "https://www.upwork.com/freelancers/~01e6281fb21a4a3862"
#property version   "1.00"
#property strict
#property indicator_chart_window

#property indicator_buffers 6

#property indicator_color1 clrWhite
#property indicator_style1 STYLE_DOT
#property indicator_width1 1
#property indicator_label1 "ATRUP(0)"

#property indicator_color2 clrWhite
#property indicator_style2 STYLE_DOT
#property indicator_width2 1
#property indicator_label2 "ATRDOWN(1)"

#property indicator_color3 clrRed
#property indicator_style3 STYLE_SOLID
#property indicator_width3 2
#property indicator_label3 "EnvelopeUPRed(2)"

#property indicator_color4 clrRed
#property indicator_style4 STYLE_SOLID
#property indicator_width4 2
#property indicator_label4 "EnvelopeDNRed(3)"

#property indicator_color5 clrYellow
#property indicator_style5 STYLE_SOLID
#property indicator_width5 2
#property indicator_label5 "BandUPYellow(4)"

//#property indicator_color6 clrYellow
//#property indicator_style6 STYLE_DOT
//#property indicator_width6 1
//#property indicator_label6 "BANDMID(5)"

#property indicator_color6 clrYellow
#property indicator_style6 STYLE_SOLID
#property indicator_width6 2
#property indicator_label6 "BandDNYellow(5)"


input int ENVPeriod  = 14;
input int ATRPeriod  = 14;
input int BBPeriod   = 14;

input ENUM_MA_METHOD ENVMethod = MODE_SMA;
input ENUM_APPLIED_PRICE ENVAppliedPrice  = PRICE_CLOSE;
input ENUM_APPLIED_PRICE BBAppliedPrice   = PRICE_CLOSE;

input int      ENVShift       = 0;
input double   ENVDeviation   = 0.25;
input int      BBShift        = 1;
input double   BBDeviation    = 2;


double ATRUP[],ATRDOWN[];
double ENVUP[],ENVDOWN[];
double BUP[],
       //BMID[],
       BDOWN[];

//+------------------------------------------------------------------+
//| Custom indicator initialization function                         |
//+------------------------------------------------------------------+
int OnInit()
  {
//--- indicator buffers mapping
   IndicatorBuffers(7);
   
   SetIndexBuffer(0,ATRUP,INDICATOR_DATA);
   SetIndexBuffer(1,ATRDOWN,INDICATOR_DATA);
   SetIndexBuffer(2,ENVUP,INDICATOR_DATA);
   SetIndexBuffer(3,ENVDOWN,INDICATOR_DATA);
   SetIndexBuffer(4,BUP,INDICATOR_DATA);
   //SetIndexBuffer(5,BMID,INDICATOR_DATA);
   SetIndexBuffer(5,BDOWN,INDICATOR_DATA);
   
   ArraySetAsSeries(ATRUP,true);
   ArraySetAsSeries(ATRDOWN,true);
   ArraySetAsSeries(ENVUP,true);
   ArraySetAsSeries(ENVDOWN,true);
   ArraySetAsSeries(BUP,true);
   //ArraySetAsSeries(BMID,true);
   ArraySetAsSeries(BDOWN,true);
//---
   return(INIT_SUCCEEDED);
  }
  

void OnDeinit(const int reason)
{
   ArrayFree(ATRUP);
   ArrayFree(ATRDOWN);
   ArrayFree(ENVUP);
   ArrayFree(ENVDOWN);
   ArrayFree(BUP);
   //ArrayFree(BMID);
   ArrayFree(BDOWN);
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
   int bars = rates_total-prev_calculated;
   for(int i=0;i<MathMax(bars,1);i++)
   {
      ATRUP[i]    = iClose(Symbol(),0,i)+ATR(i);
      ATRDOWN[i]  = iClose(Symbol(),0,i)-ATR(i);
      ENVUP[i]    = ENV(1,i);
      ENVDOWN[i]  = ENV(2,i);
      BUP[i]      = BANDS(1,i);
      BDOWN[i]    = BANDS(2,i);
      //BMID[i] = BANDS(0,i);
   }
//--- return value of prev_calculated for next call
   return(rates_total);
  }
//+------------------------------------------------------------------+

double ATR(int index)
{
   return iATR(Symbol(),0,ATRPeriod,index);
}
double ENV(int buf,int index)
{
   return iEnvelopes(Symbol(),0,ENVPeriod,ENVMethod,ENVShift,ENVAppliedPrice,ENVDeviation,buf,index);
}
double BANDS(int buf,int index)
{
   return iBands(Symbol(),0,BBPeriod,BBDeviation,BBShift,BBAppliedPrice,buf,index);
}