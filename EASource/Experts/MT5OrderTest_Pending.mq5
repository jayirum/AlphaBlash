//+------------------------------------------------------------------+
//|                                                    OrderTest.mq5 |
//|                        Copyright 2020, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2020, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"

int g_Cnt = 0;

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
   EventSetTimer(3);
   
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


string sSymbol = "EURUSD";
string sMsg;
int g_posOrdTicket;

void OnTimer()
{
   if( g_Cnt==0 )
   {
      PendingOrder_New(ORDER_TYPE_BUY_LIMIT);      
   }
   if( g_Cnt==1 )
   {
      PendingOrder_New(ORDER_TYPE_SELL_STOP);      
   }
   
   if( g_Cnt == 5 )
   {
      //PendingOrder_ViewOrders();
      PendingOrder_Modify();
   }

   if( g_Cnt==5 )
   {
      PendingOrder_Delete();
      PendingOrder_ViewHistory();
   }
      
   g_Cnt++;
}


void PendingOrder_ViewHistory()
{
   if(HistoryOrderSelect(g_posOrdTicket)==false)
   {
      PrintFormat("HistoryOrderSelect error(%d)", g_posOrdTicket);
      return;
   }
   
   int pos_id = (int)HistoryOrderGetInteger(g_posOrdTicket, ORDER_POSITION_ID);
   PrintFormat("position id:%d", pos_id);
}


void PendingOrder_New(int nOrdTp)
{
   MqlTradeCheckResult result = {0};
   MqlTradeResult  resultOrd  = {0};
   MqlTradeRequest req     = {0};

   req.action  = TRADE_ACTION_PENDING;
   req.magic   = 100;       
   req.order   = 0;
   req.symbol  = sSymbol; 
   req.volume  = 0.1;
   
   int offset = 50;                                                    // offset from the current price to place the order, in points
   double price;                                                       // order triggering price
   double point=SymbolInfoDouble(_Symbol,SYMBOL_POINT);                // value of point
   int digits=SymbolInfoInteger(_Symbol,SYMBOL_DIGITS);                // number of decimal places (precision)
   //--- checking the type of operation
   if(nOrdTp==ORDER_TYPE_BUY_LIMIT)
   {
      req.type     =ORDER_TYPE_BUY_LIMIT;                          // order type
      price=SymbolInfoDouble(Symbol(),SYMBOL_ASK)-offset*point;        // price for opening 
      req.price    =NormalizeDouble(price,digits);                 // normalized opening price 
      req.tp = NormalizeDouble(price+offset*point,digits);
      req.sl = NormalizeDouble(price-offset*point,digits);
   }
   
   else if(nOrdTp==ORDER_TYPE_SELL_STOP)
   {
      req.type     =ORDER_TYPE_SELL_STOP;                           // order type
      price=SymbolInfoDouble(Symbol(),SYMBOL_BID)-offset*point;         // price for opening 
      req.price    =NormalizeDouble(price,digits);                  // normalized opening price 
      req.tp = NormalizeDouble(price-offset*point,digits);
      req.sl = NormalizeDouble(price+offset*point,digits);
   }
   req.stoplimit  = 0;   
   req.deviation  = ULONG_MAX;
   req.comment    = "Pending Order";
   //req.type_filling = ORDER_FILLING_IOC;
         
   ResetLastError();
   bool bSuccess = OrderCheck(req, result);
   if( bSuccess==false )
   {
      PrintFormat("[OrderCheck](ret:%d)(lasterr:%d)(retcode:%d)(%s)(prc:%f)(symbol:%s)", 
         bSuccess, GetLastError(), result.retcode, FormatResult(sMsg, result.retcode), req.price, req.symbol);
      return;
    }
    
    bSuccess = OrderSend(req, resultOrd);
    if(bSuccess==false)
    {
      PrintFormat("[OrderSend-New](ret:%d)(lasterr:%d)(retcode:%d)(%s)(prc:%f)(symbol:%s)", 
         bSuccess, GetLastError(), result.retcode, FormatResult(sMsg, resultOrd.retcode), req.price, req.symbol);
      return;
    }
    
    if(resultOrd.retcode!=TRADE_RETCODE_DONE){
      PrintFormat("result error:%d", resultOrd.retcode);
      return;
    }
    
    if( g_Cnt==0 )
      g_posOrdTicket = resultOrd.order;
      
    PrintFormat("[Order-New Ok]retcode:%d, deal:%d, order:%d, vol:%f, prc:%f, bid:%f, ask:%f, req_id:%d, comments:%s, point:%f, digits:%d",
                  resultOrd.retcode, resultOrd.deal, resultOrd.order, resultOrd.volume, resultOrd.price, resultOrd.bid
                  ,resultOrd.ask, resultOrd.request_id, resultOrd.comment, point, digits);
}


void PendingOrder_ViewOrders()
{
   MqlTradeRequest request;
   MqlTradeResult  result;
   int total=OrdersTotal(); // number of open positions   
   for(int i=total-1; i>=0; i--)
   {
      //--- parameters of the order
      ulong  ticket=OrderGetTicket(i);
      PrintFormat("[idx:%d]OrderGetTicket:%d",i, ticket);
   }
}

void PendingOrder_Delete()
{
   MqlTradeCheckResult result = {0};
   MqlTradeResult  resultOrd  = {0};
   MqlTradeRequest req     = {0};

   int total=OrdersTotal(); // number of open positions   
   for(int i=total-1; i>=0; i--)
   {
      //--- parameters of the order
      ulong  ticket=OrderGetTicket(i);
      PrintFormat("[idx:%d]OrderGetTicket:%d",i, ticket);
      
      if( ticket!=g_posOrdTicket )
         continue;
      
      req.action  = TRADE_ACTION_REMOVE;
      req.order   = ticket;
      
      ResetLastError();
  
      bool bSuccess = OrderSend(req, resultOrd);
      if(bSuccess==false)
      {
         PrintFormat("[OrderSend-Delete error](ret:%d)(lasterr:%d)(retcode:%d)(%s)(prc:%f)(symbol:%s)", 
            bSuccess, GetLastError(), result.retcode, FormatResult(sMsg, resultOrd.retcode), req.price, req.symbol);
         return;
      }
    
      if(resultOrd.retcode!=TRADE_RETCODE_DONE)
         PrintFormat("result error:%d", resultOrd.retcode);
      else
         PrintFormat("[Delete Ok]retcode:%d, deal:%d, order:%d, vol:%f, prc:%f, bid:%f, ask:%f, req_id:%d, comments:%s",
                  resultOrd.retcode, resultOrd.deal, resultOrd.order, resultOrd.volume, resultOrd.price, resultOrd.bid
                  ,resultOrd.ask, resultOrd.request_id, resultOrd.comment);
   }
}



void PendingOrder_Modify()
{
   MqlTradeCheckResult result = {0};
   MqlTradeResult  resultOrd  = {0};
   MqlTradeRequest req     = {0};

   int offset = 50;
   double price;
   int total=OrdersTotal(); // number of open positions   
   for(int i=total-1; i>=0; i--)
   //for(int i=total-1; i>0; i--)
   {
      //--- parameters of the order
      ulong  ticket=OrderGetTicket(i);
      PrintFormat("[idx:%d]OrderGetTicket:%d, globlticket:%d",i, ticket, g_posOrdTicket);
    
      req.action  = TRADE_ACTION_MODIFY;
      req.order   = ticket;
      req.symbol  = OrderGetString(ORDER_SYMBOL);
      req.sl      = 0;  //OrderGetDouble(ORDER_SL);
      req.tp      = 0;  //OrderGetDouble(ORDER_TP);
      
      ENUM_ORDER_TYPE type=(ENUM_ORDER_TYPE)OrderGetInteger(ORDER_TYPE);
      double point=SymbolInfoDouble(req.symbol,SYMBOL_POINT); 
      int    digits=(int)SymbolInfoInteger(req.symbol,SYMBOL_DIGITS);
      
      if(type==ORDER_TYPE_BUY_LIMIT)
      {
         price=SymbolInfoDouble(Symbol(),SYMBOL_ASK)-offset*point*5;        // price for opening 
         req.price    =NormalizeDouble(price,digits);                 // normalized opening price 
         if(ticket!=g_posOrdTicket)
         {
            req.tp = NormalizeDouble(price+offset*point,digits);
            req.sl = NormalizeDouble(price-offset*point,digits);
         }
      }
      else if(type==ORDER_TYPE_SELL_STOP)
      {
         price=SymbolInfoDouble(Symbol(),SYMBOL_BID)-offset*point*5;        // price for opening 
         req.price    =NormalizeDouble(price,digits);                 // normalized opening price 
         if(ticket!=g_posOrdTicket)
         {
            req.tp = NormalizeDouble(price-offset*point,digits);
            req.sl = NormalizeDouble(price+offset*point,digits);
         }
      }
      
      
      req.comment    = "MODIFY";
      //req.type_filling = ORDER_FILLING_IOC;
         
      ResetLastError();
  
      bool bSuccess = OrderSend(req, resultOrd);
      if(bSuccess==false)
      {
         PrintFormat("[OrderSend-Ord Modify error](ret:%d)(lasterr:%d)(retcode:%d)(%s)(prc:%f)(symbol:%s)", 
            bSuccess, GetLastError(), result.retcode, FormatResult(sMsg, resultOrd.retcode), req.price, req.symbol);
         return;
      }
    
      if(resultOrd.retcode!=TRADE_RETCODE_DONE)
         PrintFormat("result error:%d", resultOrd.retcode);
      else
         PrintFormat("[Modify Ok]retcode:%d, order:%d, vol:%f,prc:%f, point:%f, digits:%d, req_id:%d, comments:%s",
                  resultOrd.retcode, resultOrd.order, resultOrd.volume, resultOrd.price, point, digits, resultOrd.request_id, resultOrd.comment);
                  
                  
   }
}


string FormatResult(string &str, const int retcode) 
{
   
//--- clean
   str="";

//--- see the response code
   switch(retcode)
     {
      case 0:
         str = "Success";
         break;
      case TRADE_RETCODE_REQUOTE:
         str=StringFormat("requote %s", "");  // (bid:%f/ask:%f)", result.bid, result.ask);
      break;

      case TRADE_RETCODE_DONE:
            str=StringFormat("done %s", ""); // at %f", result.price);
      break;

      case TRADE_RETCODE_DONE_PARTIAL:
            str=StringFormat("done partially %s", "");   // %f at %f", result.volume, result.price);
      break;

      case TRADE_RETCODE_REJECT            : str="rejected";                        break;
      case TRADE_RETCODE_CANCEL            : str="canceled";                        break;
      case TRADE_RETCODE_PLACED            : str="placed";                          break;
      case TRADE_RETCODE_ERROR             : str="common error";                    break;
      case TRADE_RETCODE_TIMEOUT           : str="timeout";                         break;
      case TRADE_RETCODE_INVALID           : str="invalid request";                 break;
      case TRADE_RETCODE_INVALID_VOLUME    : str="invalid volume";                  break;
      case TRADE_RETCODE_INVALID_PRICE     : str="invalid price";                   break;
      case TRADE_RETCODE_INVALID_STOPS     : str="invalid stops";                   break;
      case TRADE_RETCODE_TRADE_DISABLED    : str="trade disabled";                  break;
      case TRADE_RETCODE_MARKET_CLOSED     : str="market closed";                   break;
      case TRADE_RETCODE_NO_MONEY          : str="not enough money";                break;
      case TRADE_RETCODE_PRICE_CHANGED     : str="price changed";                   break;
      case TRADE_RETCODE_PRICE_OFF         : str="off quotes";                      break;
      case TRADE_RETCODE_INVALID_EXPIRATION: str="invalid expiration";              break;
      case TRADE_RETCODE_ORDER_CHANGED     : str="order changed";                   break;
      case TRADE_RETCODE_TOO_MANY_REQUESTS : str="too many requests";               break;
      case TRADE_RETCODE_NO_CHANGES        : str="no changes";                      break;
      case TRADE_RETCODE_SERVER_DISABLES_AT: str="auto trading disabled by server"; break;
      case TRADE_RETCODE_CLIENT_DISABLES_AT: str="auto trading disabled by client"; break;
      case TRADE_RETCODE_LOCKED            : str="locked";                          break;
      case TRADE_RETCODE_FROZEN            : str="frozen";                          break;
      case TRADE_RETCODE_INVALID_FILL      : str="invalid type_filling. choose another option";                    break;
      case TRADE_RETCODE_CONNECTION        : str="no connection";                   break;
      case TRADE_RETCODE_ONLY_REAL         : str="only real";                       break;
      case TRADE_RETCODE_LIMIT_ORDERS      : str="limit orders";                    break;
      case TRADE_RETCODE_LIMIT_VOLUME      : str="limit volume";                    break;
      case TRADE_RETCODE_POSITION_CLOSED   : str="position closed";                 break;
      case TRADE_RETCODE_INVALID_ORDER     : str="invalid order";                   break;
      case TRADE_RETCODE_CLOSE_ORDER_EXIST : str="close order already exists";      break;
      case TRADE_RETCODE_LIMIT_POSITIONS   : str="limit positions";                 break;
      default:
         str="unknown retcode "+(string)retcode;
         break;
     }
//--- return the result
   return(str);
  }
  
  
//+------------------------------------------------------------------+
//| Trade function                                                   |
//+------------------------------------------------------------------+
void OnTrade()
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
