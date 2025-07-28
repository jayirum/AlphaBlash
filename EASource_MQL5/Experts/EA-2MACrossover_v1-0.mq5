#property copyright     "EarnForex.com"
#property link          "https://www.earnforex.com/metatrader-expert-advisors/2-ma-crossover/"
#property version       "1.00"
#property strict
#property description   "This Expert Advisor open orders at the crossover of two simple moving average (MA) indicators"
#property description   " "
#property description   "DISCLAIMER: This code comes with no guarantee, you can use it at your own risk"
#property description   "We recommend to test it first on a Demo Account"

/*
ENTRY BUY: when the fast MA crosses the slow from the bottom, both MA are going up
ENTRY SELL: when the fast MA crosses the slow from the top, both MA are going down
EXIT: Can be fixed pips (Stop Loss and Take Profit) or the entry signal for the next trade
Only 1 order at a time
*/


extern double LotSize=0.1;             //Position size

extern bool UseEntryToExit=true;       //Use next entry to close the trade (if false uses take profit)
extern double StopLoss=20;             //Stop loss in pips
extern double TakeProfit=50;           //Take profit in pips

extern int Slippage=2;                 //Slippage in pips

extern bool TradeEnabled=true;         //Enable trade

extern int MAFastPeriod=10;            //Fast moving average period
extern int MASlowPeriod=25;            //Slow moving average period

//Functional variables
double ePoint;                         //Point normalized

bool CanOrder;                         //Check for risk management
bool CanOpenBuy;                       //Flag if there are buy orders open
bool CanOpenSell;                      //Flag if there are sell orders open

int OrderOpRetry=10;                   //Number of attempts to perform a trade operation
int SleepSecs=3;                       //Seconds to sleep if can't order
int MinBars=60;                        //Minimum bars in the graph to enable trading

//Functional variables to determine prices
double MinSL;
double MaxSL;
double TP;
double SL;
double Spread;
int Slip; 


//Variable initialization function
void Initialize(){          
   // RefreshRates(); no longer in mql4
   ePoint=Point();
   Slip=Slippage;
   if (MathMod(Digits(),2)==1)
   {
      ePoint*=10;
      Slip*=10;
   }
   TP=TakeProfit*ePoint;
   SL=StopLoss*ePoint;
   CanOrder=TradeEnabled;
   CanOpenBuy=true;
   CanOpenSell=true;
}


//Check if orders can be submitted
void CheckCanOrder()
{            
   //if( Bars<MinBars )
   if( Bars(Symbol(), PERIOD_CURRENT))  
   {
      Print("INFO - Not enough Bars to trade");
      CanOrder=false;
   }
   OrdersOpen();
   return;
}


//Check if there are open orders and what type
void OrdersOpen()
{
   for( int i = 0 ; i < PositionsTotal() ; i++ ) 
   {
      if( PositionGetSymbol(i)==Symbol())
      {
         if(PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_BUY) 
            CanOpenBuy=false;
         if(PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_SELL) 
            CanOpenSell=false;
      }
   }
   return;
}
// MT4 Codes
//void OrdersOpen()
//{
//   for( int i = 0 ; i < OrdersTotal() ; i++ ) 
//   {
//      if( OrderSelect( i, SELECT_BY_POS, MODE_TRADES ) == false ) {
//         Print("ERROR - Unable to select the order - ",GetLastError());
//         break;
//      } 
//      if( OrderSymbol()==Symbol() && OrderType() == ORDER_TYPE_BUY) CanOpenBuy=false;
//      if( OrderSymbol()==Symbol() && OrderType() == ORDER_TYPE_SELL) CanOpenSell=false;
//   }
//   return;
//}


//Close all the orders of a specific type and current symbol
void CloseAll(int Command)
{
   double ClosePrice=0;
   for( int i = PositionsTotal() ; i >=0  ; i-- ) 
   {
      if( PositionGetSymbol(i)==Symbol() && PositionGetInteger(POSITION_TYPE)==Command) 
      {
         if(Command==POSITION_TYPE_BUY)   ClosePrice = SymbolInfoDouble(Symbol(),SYMBOL_BID);
         if(Command==POSITION_TYPE_SELL)  ClosePrice = SymbolInfoDouble(Symbol(),SYMBOL_ASK);
         
         ulong Ticket = PositionGetTicket(i);
         
         MqlTradeResult  resultOrd  = {};
         MqlTradeRequest req        = {};

         req.action        = TRADE_ACTION_DEAL;
         req.position      = Ticket;
         req.symbol        = PositionGetString(POSITION_SYMBOL);
         req.volume        = PositionGetDouble(POSITION_VOLUME);
         req.price         = ClosePrice;
         req.deviation     = ULONG_MAX;
         req.type_filling  = ORDER_FILLING_IOC;
      
         for(int j=1; j<OrderOpRetry; j++)
         {
            bool res = OrderSend(req, resultOrd);
            if(res)
            {
               Print("TRADE - CLOSE - Order ",Ticket," closed at price ",ClosePrice);
               break;
            }
            else Print("ERROR - CLOSE - error closing order ",Ticket," return error: ",GetLastError());
         }
      }
   }
   return;
}

//void CloseAll(int Command)
//{
//   double ClosePrice=0;
//   for( int i = 0 ; i < OrdersTotal() ; i++ ) 
//   {
//      if( OrderSelect( i, SELECT_BY_POS, MODE_TRADES ) == false ) 
//      {
//         Print("ERROR - Unable to select the order - ",GetLastError());
//         break;
//      }
//      if( OrderSymbol()==Symbol() && OrderType()==Command) 
//      {
//         if(Command==ORDER_TYPE_BUY) ClosePrice=Bid;
//         if(Command==ORDER_TYPE_SELL) ClosePrice=Ask;
//         double Lots=OrderLots();
//         int Ticket=OrderTicket();
//         for(int j=1; j<OrderOpRetry; j++)
//         {
//            bool res=OrderClose(Ticket,Lots,ClosePrice,Slip,Red);
//            if(res)
//            {
//               Print("TRADE - CLOSE - Order ",Ticket," closed at price ",ClosePrice);
//               break;
//            }
//            else Print("ERROR - CLOSE - error closing order ",Ticket," return error: ",GetLastError());
//         }
//      }
//   }
//   return;
//}


//Open new order of a given type
void OpenNew(int Command)
{
   // RefreshRates();
   double OpenPrice=0;
   double SLPrice=0;
   double TPPrice=0;
   if(Command==ORDER_TYPE_BUY)
   {
      OpenPrice = SymbolInfoDouble(Symbol(),SYMBOL_ASK);
      if(!UseEntryToExit)
      {
         SLPrice=OpenPrice-SL;
         TPPrice=OpenPrice+TP;
      }
   }
   if(Command==ORDER_TYPE_SELL)
   {
      OpenPrice = SymbolInfoDouble(Symbol(),SYMBOL_BID);
      if(!UseEntryToExit)
      {
         SLPrice=OpenPrice+SL;
         TPPrice=OpenPrice-TP;
      }
   }
   
   MqlTradeResult  resultOrd  = {};
   MqlTradeRequest req        = {};

   req.action  = TRADE_ACTION_DEAL;
   //req.magic   = ;       
   req.order   = 0;        // Order ticket
   req.symbol  = Symbol(); 
   req.volume  = LotSize;
   req.price   = OpenPrice;
   //req.stoplimit  = 0;   // for Limit pending order
   req.sl         = NormalizeDouble(SLPrice,Digits());
   req.tp         = NormalizeDouble(TPPrice,Digits());
   req.deviation  = Slip;
   req.type       = Command;
   req.type_filling = ORDER_FILLING_IOC;
   //req.expiration
   req.comment    = "";
   //req.position
   //req.position_by
   
   for(int i=1; i<OrderOpRetry; i++)
   {
      bool res = OrderSend(req, resultOrd);
      if(res==true)
      {
         Print("TRADE - NEW - Order ",resultOrd.order," submitted: Command ",Command," Volume ",LotSize," Open ",OpenPrice," Slippage ",Slip," Stop ",SLPrice," Take ",TPPrice);
         break;
      }
      else Print("ERROR - NEW - error sending order, return error: ",GetLastError());
   }
   return;
}


//Technical analysis of the indicators
bool CrossToBuy=false;
bool CrossToSell=false;

void CheckMACross(){
   CrossToBuy=false;
   CrossToSell=false;
   double MASlowCurr=iMA(Symbol(), PERIOD_CURRENT, MASlowPeriod, 0, MODE_SMA, PRICE_CLOSE);
   double MASlowPrev=iMA(Symbol(), PERIOD_CURRENT, MASlowPeriod, 0, MODE_SMA, PRICE_CLOSE);
   double MAFastCurr=iMA(Symbol(), PERIOD_CURRENT, MAFastPeriod, 0, MODE_SMA, PRICE_CLOSE);
   double MAFastPrev=iMA(Symbol(), PERIOD_CURRENT, MAFastPeriod, 0, MODE_SMA, PRICE_CLOSE);
   if(MASlowPrev>MAFastPrev && MAFastCurr>MASlowCurr){
      CrossToBuy=true;
   }
   if(MASlowPrev<MAFastPrev && MAFastCurr<MASlowCurr){
      CrossToSell=true;
   }
}





//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
//---
   
//---
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//---
   
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
//---
   //Calling initialization, checks and technical analysis
   Initialize();
   CheckCanOrder();
   CheckMACross();
   //Check of Entry/Exit signal with operations to perform
   if(CrossToBuy)
   {
      if(UseEntryToExit) CloseAll(ORDER_TYPE_SELL);
      if(CanOpenBuy && CanOpenSell && CanOrder) OpenNew(ORDER_TYPE_BUY);
   }
   if(CrossToSell)
   {
      if(UseEntryToExit) CloseAll(ORDER_TYPE_BUY);
      if(CanOpenSell && CanOpenBuy && CanOrder) OpenNew(ORDER_TYPE_SELL);
   }
  }
//+------------------------------------------------------------------+
