//+------------------------------------------------------------------+
//|                                          MT5_Order_Pos_Funcs.mqh |
//|                        Copyright 2020, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2020, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"

#ifndef __MT4_SEL_ORDPOS__
#define __MT4_SEL_ORDPOS__


#include "Common.mqh"

//enum ENUM_CURR_HIST { EN_DEFAULT=-1, EN_CURR, EN_HIST };
//enum ENUM_ORD_POS   { EN_DEFAULT=-1, EN_ORD, EN_POS };



class CMT5OrdPos
{
public:
   CMT5OrdPos(){};
   ~CMT5OrdPos(){};

public:   
   static int     GetTotalCnt(bool bPosition);
   static ulong   GetTicketNo(bool bPosition, int idx);
   static void    GetOrdInfo(_In_ bool bPosition, _In_ ulong nTicket, _Out_ ORD_INFO& ord);
   static bool    GetPosRemainLots_OfPartialed(_In_ ulong nTicket, _Out_ double& dRemainLots);
   
   static string getComments(bool bPosition);
   static string getSymbol(bool bPosition);
   
   static bool FindAndReadOrdInfo(bool bPosition, int nTicket, _Out_ ORD_INFO& ord);
   static bool FindAndReadOrdInfo_WithMasterTicket(bool bPosition, int nMasterTicket, _Out_ ORD_INFO& ord);
private:
};


bool CMT5OrdPos::GetPosRemainLots_OfPartialed(_In_ ulong nTicket, _Out_ double& dRemainLots)
{
   ORD_INFO ord;
   if( FindAndReadOrdInfo(true, nTicket, ord)==false ){
      dRemainLots = 0;
      return false;
   }
   
   dRemainLots = ord.dLots;
   return true;
}


// Get position info in trade tab using order ticket
// call this function to get order info for sending to server
// after success of placing order on MT5
bool CMT5OrdPos::FindAndReadOrdInfo(bool bPosition, int nTicket, _Out_ ORD_INFO& ord)
{
   int total = 0;
   
   if( bPosition==true )   total = PositionsTotal(); 
   else                    total = OrdersTotal();
   
   // search from the latest order
   bool bFind = false;
   for(int i=total-1; i>=0; i--)
   {
      ulong nCurrTicket;
      if( bPosition )   nCurrTicket = PositionGetTicket(i);
      else              nCurrTicket = OrderGetTicket(i);
      
      if( nCurrTicket == nTicket )
      {
         bFind = true;
         break;
      }
   }
   
   if(bFind==false)
      return false;
      
   GetOrdInfo(bPosition, nTicket, ord);
      
   return true;
}

// Get position info in trade tab using order ticket
// call this function to get order info for sending to server
// after success of placing order on MT5
bool CMT5OrdPos::FindAndReadOrdInfo_WithMasterTicket(bool bPosition,int nMasterTicket,ORD_INFO &ord)
{
   int total = 0;
   
   if( bPosition==true )   total = PositionsTotal(); 
   else                    total = OrdersTotal();
   
   // search from the latest order
   bool bFind = false;
   for(int i=total-1; i>=0; i--)
   {
      ZeroMemory(ord);
      GetOrdInfo(bPosition, GetTicketNo(bPosition, i), ord);

      if( ord.nMagic == nMasterTicket )
      {
         bFind = true;
         break;
      }
   }
   
   if(bFind==false)
      return false;
      
   return true;
}


int CMT5OrdPos::GetTotalCnt(bool bPosition)
{
   if(bPosition)  return PositionsTotal();
   return OrdersTotal();
}

ulong CMT5OrdPos::GetTicketNo(bool bPosition, int idx)
{
   ulong ticket;
   if(bPosition)  ticket = PositionGetTicket(idx);
   else           ticket = OrderGetTicket(idx);
   return ticket;
}

string CMT5OrdPos::getComments(bool bPosition)
{
   if(bPosition) return PositionGetString(POSITION_COMMENT);
   return OrderGetString(ORDER_COMMENT);
}


string CMT5OrdPos::getSymbol(bool bPosition)
{
   if(bPosition) return PositionGetString(POSITION_SYMBOL);
   return OrderGetString(ORDER_SYMBOL);
}


void CMT5OrdPos::GetOrdInfo(_In_ bool bPosition, _In_ ulong nTicket, _Out_ ORD_INFO& ord)
{
   ZeroMemory(ord);
   ord.nTicket = nTicket;
   ord.nClosingTicket = 0;
   ord.bPosition = bPosition;
   ord.sOrdPosTp = (bPosition==true)? ORDPOS_P : ORDPOS_O;

   ord.sCloseTime = "";
   
   if(ord.bPosition)
   {
      ord.sOpenTime     = DatetimeToStr(PositionGetInteger(POSITION_TIME));
      ord.nType         = (int)PositionGetInteger(POSITION_TYPE);
      ord.dLots         = PositionGetDouble(POSITION_VOLUME);
      ord.sSymbol       = PositionGetString(POSITION_SYMBOL);
      ord.dOpenPrc      = PositionGetDouble(POSITION_PRICE_OPEN);
      ord.dClosePrc     = PositionGetDouble(POSITION_PRICE_CURRENT);
      ord.dStopLoss     = PositionGetDouble(POSITION_SL);
      ord.dTakeProfit   = PositionGetDouble(POSITION_TP);
      ord.dCmsn         = 0;
      ord.dSwap         = PositionGetDouble(POSITION_SWAP);
      ord.dProfit       = PositionGetDouble(POSITION_PROFIT);
      ord.sComments     = PositionGetString(POSITION_COMMENT);
      ord.sExpiry       = "";
      ord.nExpiry       = 0;
      ord.nMagic        = (int)PositionGetInteger(POSITION_MAGIC);
      ord.sOrdPosTp     = ORDPOS_P;
   }
   
   else
   {
      ord.sOpenTime     = DatetimeToStr(OrderGetInteger(ORDER_TIME_SETUP));
      ord.nType         = (int)OrderGetInteger(ORDER_TYPE);
      ord.dLots         = OrderGetDouble(ORDER_VOLUME_CURRENT);
      ord.sSymbol       = OrderGetString(ORDER_SYMBOL);
      ord.dOpenPrc      = OrderGetDouble(ORDER_PRICE_OPEN);
      ord.dClosePrc     = OrderGetDouble(ORDER_PRICE_CURRENT);
      ord.dStopLoss     = OrderGetDouble(ORDER_SL);
      ord.dTakeProfit   = OrderGetDouble(ORDER_TP);
      ord.dCmsn         = 0;
      ord.dSwap         = 0;
      ord.dProfit       = 0;
      ord.sComments     = OrderGetString(ORDER_COMMENT);
      ord.nExpiry       = (int)OrderGetInteger(ORDER_TIME_EXPIRATION);
      ord.sExpiry       = DatetimeToStr(ord.nExpiry);
      ord.nMagic        = (int)OrderGetInteger(ORDER_MAGIC);
      ord.sOrdPosTp     = ORDPOS_O;
   }
   
   ord.sOrdPosTp = __OrdTypeString((int)ord.nType);
}




#endif