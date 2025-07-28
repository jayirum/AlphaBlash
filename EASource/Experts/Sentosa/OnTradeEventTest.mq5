//+------------------------------------------------------------------+
//|                                             OnTradeEventTest.mq5 |
//|                                  Copyright 2022, MetaQuotes Ltd. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2022, MetaQuotes Ltd."
#property link      "https://www.mql5.com"
#property version   "1.00"

int _pos_prev = 0;
ulong _lastticket = 0;

int _ord_prev = 0;
ulong _lastordticket = 0;

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
//--- create timer
   PrintFormat("ORDER_STATE_STARTED:%d, ORDER_STATE_PLACED:%d, ORDER_STATE_FILLED:%d",
               ORDER_STATE_STARTED, ORDER_STATE_PLACED, ORDER_STATE_FILLED);
//EventSetMillisecondTimer (5000);

   _pos_prev = PositionsTotal();
   _lastticket = PositionGetTicket(_pos_prev-1);

   _ord_prev = OrdersTotal();
   _lastordticket = OrderGetTicket(_ord_prev-1);


   PrintFormat("OnInit : pos count;%d, pos last ticket:%d", _pos_prev, _lastticket);
   PrintFormat("OnInit : ord count;%d, ord last ticket:%d", _ord_prev, _lastordticket);
   
   DecideTheStartTime();
//---
   return(INIT_SUCCEEDED);
  }
  
void DecideTheStartTime()
{
   _pos_prev = 0;
   for ( int i=0; i<PositionsTotal(); i++ )
   {
      ulong ticket = PositionGetTicket(i);
      long time = PositionGetInteger(POSITION_TIME_MSC);
      
      PrintFormat("[%d] time:%d", ticket, time);
      if(_pos_prev==0){
         _pos_prev = time;
         continue;
      }
      if( time < _pos_prev ){
         PrintFormat("update _pos_prev with %d", time);
         _pos_prev = time;   
      } 
   }
   PrintFormat("_pos_prev is %d", _pos_prev);
   
   _ord_prev = 0;
   for ( int i=0; i<OrdersTotal(); i++ )
   {
      ulong ticket = OrderGetTicket(i);
      long time = OrderGetInteger(ORDER_TIME_SETUP_MSC);
      PrintFormat("[%d] time:%d", ticket, time);
      
      if(_ord_prev==0){
         _ord_prev = time;
         continue;
      }
      
      
      if( time < _ord_prev ){
         PrintFormat("update _ord_prev with %d", time);
         _ord_prev = time;   
      } 
   }
   PrintFormat("_ord_prev is %d", _ord_prev);
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
   long ticket = PositionGetTicket(PositionsTotal()-1);

   PrintFormat("\tOnTimer Event Ok.COUNT:%d, TICKET:%d, status:%d", PositionsTotal(), ticket, PositionGetTicket(ORDER_STATE));

  }
//+------------------------------------------------------------------+
//| Trade function                                                   |
//+------------------------------------------------------------------+
void OnTrade()
  {

//   checkPos();
//
//   checkOrd();

      long ticket = PositionGetTicket(PositionsTotal()-1);
      PrintFormat("count:%d, ticket:%d, stat:%d, time:%s, type:%d", 
      PositionsTotal(), ticket, OrderGetInteger(ORDER_STATE), TimeToString(OrderGetInteger(ORDER_TIME_SETUP), TIME_SECONDS)
      ,OrderGetInteger(ORDER_TYPE)
      );
  }



//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void checkOrd()
  {
   if(_ord_prev == OrdersTotal())
      return;

   if(_ord_prev < OrdersTotal())
   {
      long ticket = OrderGetTicket(OrdersTotal()-1);

      if(OrderGetInteger(ORDER_TYPE)< 2)
        {
         PrintFormat("Order added but market order:%d", ticket);
         return;
        }

      PrintFormat("[ORD]Trade Event-Open: COUNT:%d, TICKET:%d, status:%d, type:%d",
                  OrdersTotal(), ticket, OrderGetInteger(ORDER_STATE),
                  OrderGetInteger(ORDER_TYPE)
                 );

      _lastordticket = ticket;

   }
   else if(_ord_prev > OrdersTotal())
  {
         HistorySelect(D'2023.02.15 00:00:00',TimeCurrent());
   
         for( int k=HistoryOrdersTotal()-1; k>-1; k--)
         {
   
            ulong ticket1 = HistoryOrderGetTicket(HistoryOrdersTotal()-1);
            if( HistoryOrderGetInteger(ticket1, ORDER_TYPE)<2 ) 
               continue;
      
            PrintFormat("[ORD]Trade Event-DEL : count:%d,"
                        " hist order ticket:%d, order hist status:%d, type:%d"
                        "last ticket:%d, last ticket status:%d" ,
                        OrdersTotal(),
                        ticket1, HistoryOrderGetInteger(ticket1,ORDER_STATE),
                        HistoryOrderGetInteger(ticket1, ORDER_TYPE)
                        );
            break;
         }


  }

   _ord_prev = OrdersTotal();
  }


//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void checkPos()
  {
   if(_pos_prev == PositionsTotal())
      return;

   if(_pos_prev < PositionsTotal())
     {
      long ticket = PositionGetTicket(PositionsTotal()-1);

      PrintFormat("Trade Event-Open : COUNT:%d, TICKET:%d, status:%d",
                  PositionsTotal(), ticket, OrderGetInteger(ORDER_STATE));

      _lastticket = ticket;

     }
   else
      if(_pos_prev > PositionsTotal())
        {
         HistorySelect(D'2023.02.15 00:00:00',TimeCurrent());

         PrintFormat("HistoryOrdersTotal:%d, HistoryDealsTotal:%d", HistoryOrdersTotal(), HistoryDealsTotal());

         ulong ticket1 = HistoryOrderGetTicket(HistoryOrdersTotal()-1);
         ulong dealticket = HistoryDealGetTicket(HistoryDealsTotal()-1);

         PrintFormat("Trade Event-CLOSE : count:%d,"
                     " hist order ticket:%d, order hist status:%d,"
                     " hist deal ticket:%d, order deal status:%d,"
                     "last ticket:%d, last ticket status:%d",
                     PositionsTotal(),
                     ticket1, HistoryOrderGetInteger(ticket1,ORDER_STATE),
                     dealticket, HistoryOrderGetInteger(dealticket,ORDER_STATE),
                     _lastticket,HistoryOrderGetInteger(_lastticket,ORDER_STATE)
                    );

        }

   _pos_prev = PositionsTotal();
  }

//+------------------------------------------------------------------+
