//+------------------------------------------------------------------+
//|                                          MT5_Order_Pos_Funcs.mqh |
//|                        Copyright 2020, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2020, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"

#ifndef __MT4_SEL_ORDPOS_HIST__
#define __MT4_SEL_ORDPOS_HIST__


#include "Common.mqh"

class CMT5OrdPosHist
{
public:
   CMT5OrdPosHist(){};
   ~CMT5OrdPosHist(){};

public:   
   static bool Check_TicketExists(int nOrdTicket );
   
   //static bool GetOrdInfo()(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, int nPartialCnt);
   static bool GetOrdInfoEx(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, double dRemainLotsOfPartialed);
   
   static string GetMsg(){ return CMT5OrdPosHist::m_sMsg; };
private:
   static bool GetPendingInfo(_In_ ulong nTargetOrdTicket, _Out_ ORD_INFO& ord);
   static bool GetPosInfo(_In_ ulong nTargetOrdTicket, _Out_ ORD_INFO& ord, int nPartialCnt);
   static bool GetPosInfoInner(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, int nPartialCnt);
   
   static bool GetPosInfoEx(_In_ ulong nTargetOrdTicket, _Out_ ORD_INFO& ord, double dRemainLotsOfPartialed);
   static bool GetPosInfoInnerEx(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, double dRemainLotsOfPartialed);
   
   
   static string m_sMsg;
};

string CMT5OrdPosHist::m_sMsg;

bool CMT5OrdPosHist::Check_TicketExists(int nOrdTicket )
{
   return HistoryOrderSelect(nOrdTicket);
}

bool CMT5OrdPosHist::GetOrdInfoEx(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, double dRemainLotsOfPartialed)
{
   if(HistoryOrderSelect(nOrgOrdTicket)==false)
      return false;
   
   ZeroMemory(ord);

   // find out whether this order is pending order or position   
   ord.nType = (int)HistoryOrderGetInteger(nOrgOrdTicket, ORDER_TYPE);
   
   int nPosId = (int)HistoryOrderGetInteger(nOrgOrdTicket, ORDER_POSITION_ID);
   ord.bPosition = (nPosId==0) ? false:true;
   ord.sOrdPosTp = (ord.bPosition==true)? ORDPOS_P:ORDPOS_O;
   
   // if pending order, use the nOrgOrdTicket
   if(ord.bPosition==false)
      return GetPendingInfo(nOrgOrdTicket, ord);

   return GetPosInfoEx(nOrgOrdTicket, ord, dRemainLotsOfPartialed);
}

//bool CMT5OrdPosHist::GetOrdInfo(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, int nPartialCnt)
//{
//   if(HistoryOrderSelect(nOrgOrdTicket)==false)
//      return false;
//   
//   ZeroMemory(ord);
//
//   // find out whether this order is pending order or position   
//   ord.nType = (int)HistoryOrderGetInteger(nOrgOrdTicket, ORDER_TYPE);
//   
//   int nPosId = (int)HistoryOrderGetInteger(nOrgOrdTicket, ORDER_POSITION_ID);
//   ord.bPosition = (nPosId==0) ? false:true;
//   ord.sOrdPosTp = (ord.bPosition==true)? ORDPOS_P:ORDPOS_O;
//   
//   // if pending order, use the nOrgOrdTicket
//   if(ord.bPosition==false)
//      return GetPendingInfo(nOrgOrdTicket, ord);
//
//   return GetPosInfo(nOrgOrdTicket, ord, nPartialCnt);
//}


bool CMT5OrdPosHist::GetPosInfo(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, int nPartialCnt)
{
   bool bOk = false;
   for( int i=0; i<10; i++)
   {
      bOk = GetPosInfoInner(nOrgOrdTicket, ord, nPartialCnt);
      if(bOk)
         return true;
         
      Sleep(100);
      continue;
   }
   return bOk;
}

bool CMT5OrdPosHist::GetPosInfoInner(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, int nPartialCnt)
{
   //
   // Get Order Ticket of the closing order - key is Pos ID
   //
   if( HistoryOrderSelect(nOrgOrdTicket)==false )
   {
      m_sMsg = StringFormat("[GetPosInfo]HistoryOrderSelect Error(Ticket:%d)", nOrgOrdTicket);
      return false;
   }
   long nPosID = HistoryOrderGetInteger(nOrgOrdTicket, ORDER_POSITION_ID);
   
   if(HistorySelectByPosition(nPosID)==false)
   {
      m_sMsg = StringFormat("[GetPosInfo]HistorySelectByPosition Error(Ticket:%d)(PosID:%d)", nOrgOrdTicket, nPosID);
      return false;
   }
   
   // Total number of positions which has the same PosID
   int nTotal = HistoryOrdersTotal();
   int nClosingOrderTicket = 0;
   int nDealTicket         = 0;
   
   //for ( int nIdx= nTotal-1; nIdx>=0; nIdx-- )
   //{
   //   // Get the latest order
   //   nClosingOrderTicket = (int)HistoryOrderGetTicket(nIdx);
   //   nDealTicket         = (int)HistoryDealGetTicket(nIdx);
   //   PrintFormat("[HIST_SELECT][%d](Ticket:%d)(Deal:%d)",nIdx, nClosingOrderTicket, nDealTicket);
   //}
   

   if(nTotal<nPartialCnt+1)
   {
      PrintFormat("[HIST_SELECT]Total(%d) < Partial+1(%d)", nTotal, nPartialCnt+1);
      return false;
   }

   
   nClosingOrderTicket = (int)HistoryOrderGetTicket(nTotal-1);
   nDealTicket         = (int)HistoryDealGetTicket(nTotal-1);

   // Let copier closes the open order(orignal order)
   ord.nTicket       = nOrgOrdTicket;
   ord.nClosingTicket= nClosingOrderTicket;
   ord.nType         = (int)HistoryOrderGetInteger(nClosingOrderTicket, ORDER_TYPE);
   ord.sOrdPosTp     = ORDPOS_P;

   ord.sOpenTime     = DatetimeToStr((datetime)HistoryOrderGetInteger(nClosingOrderTicket, ORDER_TIME_SETUP));
   ord.sSymbol       = HistoryOrderGetString(nClosingOrderTicket, ORDER_SYMBOL);
   ord.dOpenPrc      = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_PRICE_OPEN);
   ord.dStopLoss     = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_SL);
   ord.dTakeProfit   = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_TP);
   ord.dLots         = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_VOLUME_INITIAL);
         
   ord.sComments     = HistoryOrderGetString(nClosingOrderTicket, ORDER_COMMENT);
   ord.nExpiry       = (int)HistoryOrderGetInteger(nClosingOrderTicket, ORDER_TIME_EXPIRATION);
   ord.sExpiry       = DatetimeToStr(ord.nExpiry);
   ord.sCloseTime    = DatetimeToStr((datetime)HistoryOrderGetInteger(nClosingOrderTicket, ORDER_TIME_DONE));
   ord.dClosePrc     = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_PRICE_CURRENT);
   ord.dCmsn         = HistoryDealGetDouble(nDealTicket, DEAL_COMMISSION);
   ord.dSwap         = HistoryDealGetDouble(nDealTicket, DEAL_SWAP);
   ord.dProfit       = HistoryDealGetDouble(nDealTicket, DEAL_PROFIT);
   
   return true;
}



bool CMT5OrdPosHist::GetPosInfoEx(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, double dRemainLotsOfPartialed)
{
   bool bOk = false;
   for( int i=0; i<10; i++)
   {
      bOk = GetPosInfoInnerEx(nOrgOrdTicket, ord, dRemainLotsOfPartialed);
      if(bOk)
         return true;
         
      Sleep(100);
      continue;
   }
   return bOk;
}

bool CMT5OrdPosHist::GetPosInfoInnerEx(_In_ ulong nOrgOrdTicket, _Out_ ORD_INFO& ord, double dRemainLotsOfPartialed)
{
   //
   // Get Order Ticket of the closing order - key is Pos ID
   //
   if( HistoryOrderSelect(nOrgOrdTicket)==false )
   {
      m_sMsg = StringFormat("[GetPosInfo]HistoryOrderSelect Error(Ticket:%d)", nOrgOrdTicket);
      return false;
   }
   long nPosID = HistoryOrderGetInteger(nOrgOrdTicket, ORDER_POSITION_ID);
   
   if(HistorySelectByPosition(nPosID)==false)
   {
      m_sMsg = StringFormat("[GetPosInfo]HistorySelectByPosition Error(Ticket:%d)(PosID:%d)", nOrgOrdTicket, nPosID);
      return false;
   }
   
   // Total number of positions which has the same PosID
   int nTotal = HistoryOrdersTotal();
   int nClosingOrderTicket = 0;
   int nDealTicket         = 0;
   double dLots, dOrgLots;
   double dHistoryLotsSum = dRemainLotsOfPartialed;


#ifdef __DEBUG__
   for ( int nIdx= nTotal-1; nIdx>=0; nIdx-- )
   {
      // Get the latest order
      nClosingOrderTicket = (int)HistoryOrderGetTicket(nIdx);
      nDealTicket         = (int)HistoryDealGetTicket(nIdx);
      dLots               = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_VOLUME_INITIAL);
      dHistoryLotsSum     += dLots;
      if( nIdx==0 )
         dOrgLots = dLots;
      PrintFormat("[HIST_SELECT][%d](Ticket:%d)(Deal:%d)(Lots:%f)",nIdx, nClosingOrderTicket, nDealTicket, dLots);
   }
#endif

   if(dOrgLots > dHistoryLotsSum)
   {
      PrintFormat("[HIST_SELECT]OrgLots(%f) < HistorySum(%f)", dOrgLots, dHistoryLotsSum);
      return false;
   }

   
   nClosingOrderTicket = (int)HistoryOrderGetTicket(nTotal-1);
   nDealTicket         = (int)HistoryDealGetTicket(nTotal-1);

   // Let copier closes the open order(orignal order)
   ord.nTicket       = nOrgOrdTicket;
   ord.nClosingTicket= nClosingOrderTicket;
   ord.nType         = (int)HistoryOrderGetInteger(nClosingOrderTicket, ORDER_TYPE);
   ord.sOrdPosTp     = ORDPOS_P;

   ord.sOpenTime     = DatetimeToStr((datetime)HistoryOrderGetInteger(nClosingOrderTicket, ORDER_TIME_SETUP));
   ord.sSymbol       = HistoryOrderGetString(nClosingOrderTicket, ORDER_SYMBOL);
   ord.dOpenPrc      = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_PRICE_OPEN);
   ord.dStopLoss     = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_SL);
   ord.dTakeProfit   = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_TP);
   ord.dLots         = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_VOLUME_INITIAL);
         
   ord.sComments     = HistoryOrderGetString(nClosingOrderTicket, ORDER_COMMENT);
   ord.nExpiry       = (int)HistoryOrderGetInteger(nClosingOrderTicket, ORDER_TIME_EXPIRATION);
   ord.sExpiry       = DatetimeToStr(ord.nExpiry);
   ord.sCloseTime    = DatetimeToStr((datetime)HistoryOrderGetInteger(nClosingOrderTicket, ORDER_TIME_DONE));
   ord.dClosePrc     = HistoryOrderGetDouble(nClosingOrderTicket, ORDER_PRICE_CURRENT);
   ord.dCmsn         = HistoryDealGetDouble(nDealTicket, DEAL_COMMISSION);
   ord.dSwap         = HistoryDealGetDouble(nDealTicket, DEAL_SWAP);
   ord.dProfit       = HistoryDealGetDouble(nDealTicket, DEAL_PROFIT);
   
   return true;
}

bool CMT5OrdPosHist::GetPendingInfo(_In_ ulong nTargetOrdTicket, _Out_ ORD_INFO& ord)
{
   ord.nTicket       = nTargetOrdTicket;
   ord.nClosingTicket= nTargetOrdTicket;
   ord.sOpenTime     = DatetimeToStr((datetime)HistoryOrderGetInteger(nTargetOrdTicket, ORDER_TIME_SETUP));
   ord.sSymbol       = HistoryOrderGetString(nTargetOrdTicket, ORDER_SYMBOL);
   ord.dOpenPrc      = HistoryOrderGetDouble(nTargetOrdTicket, ORDER_PRICE_OPEN);
   ord.dStopLoss     = HistoryOrderGetDouble(nTargetOrdTicket, ORDER_SL);
   ord.dTakeProfit   = HistoryOrderGetDouble(nTargetOrdTicket, ORDER_TP);
   
   ord.dLots         = HistoryOrderGetDouble(nTargetOrdTicket, ORDER_VOLUME_CURRENT);
   if(ord.dLots==0)
      ord.dLots      = HistoryOrderGetDouble(nTargetOrdTicket, ORDER_VOLUME_INITIAL);
         
   ord.dCmsn         = 0;
   ord.dSwap         = 0;
   ord.dProfit       = 0;
   ord.sComments     = HistoryOrderGetString(nTargetOrdTicket, ORDER_COMMENT);
   ord.nExpiry       = (int)HistoryOrderGetInteger(nTargetOrdTicket, ORDER_TIME_EXPIRATION);
   ord.sExpiry       = DatetimeToStr(ord.nExpiry);
   ord.sCloseTime    = DatetimeToStr((datetime)HistoryOrderGetInteger(nTargetOrdTicket, ORDER_TIME_DONE));
   ord.dClosePrc     = HistoryOrderGetDouble(nTargetOrdTicket, ORDER_PRICE_CURRENT);
   ord.sOrdPosTp     = ORDPOS_O;
   return true;
}


#endif