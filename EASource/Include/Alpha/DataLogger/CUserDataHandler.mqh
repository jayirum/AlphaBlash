#ifndef __USER_DATA_HANDLER_H__
#define __USER_DATA_HANDLER_H__


#include "../_PlatformVer.mqh"
#include "../_IncCommon.mqh"
#include "../UtilDateTime.mqh"

class CUserDataHandler
{
public:
   CUserDataHandler() { ResetData(); };
   ~CUserDataHandler(){};
   
   void ResetData();
   void Read_AllData();
   void Read_Balance();
   void Read_OpenPositions();
   void Read_ClosedPositions();
   
private:
   double Real_PnL_Cmsn_Tax_Swap();   
   
public:
   double   AccBalance;
   double   AccEquity;
   double   FreeMargin;
   double   AccPnL;
   double   AccPnLPts;
   double   AccCmsn;
   //double   AccTax;
   double   AccSwap;
   
   double   ActiveLongLots;
   double   ActiveShortLots;
   double   ClosedLongLots;
   double   ClosedShortLots;
   int      NumOfActiveLongOrders;
   int      NumOfActiveShortOrders;
   int      NumOfClosedLongOrders;
   int      NumOfClosedShortOrders;
   
};

void CUserDataHandler::ResetData()
{
   AccBalance = AccEquity = FreeMargin = AccPnL = AccPnLPts = AccCmsn = AccSwap = ActiveLongLots = ActiveShortLots = ClosedLongLots = ClosedShortLots = 0;
   NumOfActiveLongOrders = NumOfActiveShortOrders = NumOfClosedLongOrders = NumOfClosedShortOrders = 0;
}

void CUserDataHandler::Read_AllData()
{
   ResetData();
   
   Read_Balance();
   Read_OpenPositions();
   Read_ClosedPositions();
}

double CUserDataHandler::Real_PnL_Cmsn_Tax_Swap()
{
   int nTotal = OrdersTotal();
   double dPnLPts = 0;
   AccCmsn = AccSwap = 0;
   
   for(int idx = 0; idx<nTotal; idx++) 
   {
      Sleep(1);
      if(OrderSelect(idx, SELECT_BY_POS, MODE_TRADES ))
      {
         double dPlPts = 0;
         string sSymbol = OrderSymbol();
         
         if( __GetSide(OrderType())==DEF_SIDE_BUY) 
            dPlPts = (__GetCurrPrcForClose(sSymbol, DEF_SIDE_BUY ) - OrderOpenPrice() ) / __GetPointUnit(sSymbol);
         else
            dPlPts = (OrderOpenPrice() - __GetCurrPrcForClose(sSymbol, DEF_SIDE_SELL ) ) / __GetPointUnit(sSymbol);
            
         dPnLPts += dPlPts;
         
         AccCmsn  += OrderCommission();
         AccSwap  += OrderSwap();
      }
   }
   
   return dPnLPts;
}

void CUserDataHandler::Read_Balance()
{
   if(__IsMT4())
   {
      AccBalance  = AccountBalance();
      AccEquity   = AccountEquity();
      FreeMargin  = AccountFreeMargin();
      AccPnL      = AccountProfit();
      AccPnLPts   = Real_PnL_Cmsn_Tax_Swap();
   }
   else
   {
      AccBalance  = AccountInfoDouble(ACCOUNT_BALANCE);
      AccEquity   = AccountInfoDouble(ACCOUNT_EQUITY);
      FreeMargin  = AccountInfoDouble(ACCOUNT_MARGIN_FREE);
      AccPnL      = AccountInfoDouble(ACCOUNT_PROFIT);
      AccPnLPts   = Real_PnL_Cmsn_Tax_Swap();
   }
}


void CUserDataHandler::Read_OpenPositions()
{
   int nTotal = OrdersTotal();
   for(int idx = 0; idx<nTotal; idx++) 
   {
      Sleep(1);
      if(OrderSelect(idx, SELECT_BY_POS))  
      {        
         int      cmd   = OrderType();
         double   dLots = OrderLots();
         
         if( !__IsMarketOrder(cmd) )
            continue;
         
         if( __GetSide(cmd)==DEF_SIDE_SELL )
         {
            ActiveShortLots += dLots;
            NumOfActiveShortOrders++;
         }
         else
         {
            ActiveLongLots += dLots;
            NumOfActiveLongOrders++;
         }
      }
   }
}


void CUserDataHandler::Read_ClosedPositions()
{
   string sToday = __YYYYMMDDToStr_withDot(TimeCurrent());
   
   for( int i=0; i<OrdersHistoryTotal(); i++ )
   {
      Sleep(1);
      if(OrderSelect(i,SELECT_BY_POS,MODE_HISTORY)==true)
      {
         int      cmd   = OrderType();
         double   dLots = OrderLots();

         if( !__IsMarketOrder(cmd) )
            continue;
                   
         string sClosedDate = __YYYYMMDDToStr_withDot(OrderCloseTime());
         if( sToday!=sClosedDate )
            continue;
         
         if( __GetSide(cmd)==DEF_SIDE_SELL )
         {
            ClosedShortLots += dLots;
            NumOfClosedShortOrders++;
         }
         else
         {
            ClosedLongLots += dLots;
            NumOfClosedLongOrders++;
         }
      }
   }
}


#endif