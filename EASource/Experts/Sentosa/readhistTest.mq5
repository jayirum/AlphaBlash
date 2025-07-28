//+------------------------------------------------------------------+
//|                                                     readhist.mq5 |
//|                                  Copyright 2022, MetaQuotes Ltd. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2022, MetaQuotes Ltd."
#property link      "https://www.mql5.com"
#property version   "1.00"
//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+


void readhist()
{

   Print("-----------------------------------");
   HistorySelect(D'2023.02.15 08:00:00',TimeCurrent());

   PrintFormat("HistoryOrdersTotal:%d, HistoryDealsTotal:%d", HistoryOrdersTotal(), HistoryDealsTotal());

   int cnt = 0;
   for( int i=HistoryDealsTotal()-1; i>-1; i--)
   {
      ulong dealticket = HistoryDealGetTicket(i);
      ulong ordticket = HistoryDealGetInteger(dealticket,DEAL_ORDER);
      
      PrintFormat("Dealticket:%d, DealOrder:%d", dealticket,  ordticket);
      PrintFormat("-->Dealtime:%s, DealType:%d", 
                     TimeToString(HistoryDealGetInteger(dealticket, DEAL_TIME), TIME_DATE|TIME_SECONDS),
                     HistoryDealGetInteger(dealticket,DEAL_TYPE)
                  );
      PrintFormat("-->DealordTime:%s, DealOrdType:%d, DealOrdState:%d", 
                     TimeToString(HistoryOrderGetInteger(ordticket, ORDER_TIME_DONE), TIME_DATE |TIME_SECONDS),
                     HistoryOrderGetInteger(ordticket,ORDER_TYPE),
                     HistoryOrderGetInteger(ordticket,ORDER_STATE)
                     );
                     
      //if(++cnt>5)
      //   break;
   }
}
        

int OnInit()
  {
//--- create timer
   EventSetTimer(1);
   
//---
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//--- destroy timer
   EventKillTimer();
   
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
//---
   
  }
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
  {
//---
      EventKillTimer();
      readhist();
   
  }
//+------------------------------------------------------------------+
