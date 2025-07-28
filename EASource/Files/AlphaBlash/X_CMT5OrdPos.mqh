//+------------------------------------------------------------------+
//|                                          MT5_Order_Pos_Funcs.mqh |
//|                        Copyright 2020, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2020, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"

#ifndef __MT4_ORDPOS_FUNCS__
#define __MT4_ORDPOS_FUNCS__


#include "_PlatformVer.mqh"

enum ENUM_CURR_HIST { EN_DEFAULT=-1, EN_CURR, EN_HIST };
enum ENUM_ORD_POS   { EN_DEFAULT=-1, EN_ORD, EN_POS };

//
//struct ORD_INFO
//{
//   int nTicket;
//   bool bPosition;
//   string sOrdPosTp;
//   string sOpenTime;
//   string sCloseTime;
//   int nType;
//   double dLots;
//   string sSymbol;
//   double dOpenPrc;
//   double dClosePrc;
//   double dStopLoss;
//   double dTakeProfit;
//   double dCmsn;
//   double dSwap;
//   double dProfit;
//   string sComments;
//   string sExpiry;
//}

class CMT5OrdPos
{
private:
   int               m_nTicket;
   //ENUM_CURR_HIST    m_CurrHist;
   ENUM_ORD_POS      m_OrdPos;

public:
   CMT5OrdPos(){m_nTicket=0, m_CurrHist=DEFAULT, m_OrdPos=DEFAULT};
   ~CMT5OrdPos(){m_nTicket=0, m_CurrHist=DEFAULT, m_OrdPos=DEFAULT};
   
private:
   void SetOrdPos(bool bPosition) { if(bPosition) m_OrdPos = EN_ORD; else m_OrdPos = EN_POS;};
   //void SetCurrHis(bool bHis)     { if(bHis) m_CurrHist = EN_HIST; else m_CurrHist = EN_CURR;}
   void assertPreCall()           { if(m_OrdPos==EN_DEFAULT || m_nTicket==0) Alert("Call GetTicketNo or BeginSelect"); };

public:   
   static int GetTotalCnt(bool bPosition);
   static int GetTicketNo(bool bPosition, int idx);
   
   static bool BeginSelect(int nTicket, bool bHist, bool bPosition);
   
   static int     getTicket();
   static string  getComments();
   static string  getSymbol();
   static string  getOpenTime();
   static int     getType();
   static string  getTypeS();
   static double  getLots();
   static double  getOpenPrc();
   static double  getStopLoss();
   static double  getTakeProfit();
   static double  getSwap();
   static double  getProfit();
   static string  getExpiration();
   
};

int CMT5OrdPos::GetTotalCnt(bool bPosition)
{
   SetOrdPos(bPosition);
   //SetCurrHis(false);
   if(bPosition)  return PositionsTotal();
   return OrdersTotal();
}

int CMT5OrdPos::GetTicketNo(bool bPosition, int idx)
{
   int ticket;
   if(bPosition)  ticket = PositionGetTicket(idx);
   else           ticket = OrderGetTicket(idx);
   m_nTicket = ticket;
   //SetCurrHis(false);
   SetOrdPos( bPosition);
   return m_nTicket;
}


int CMT5OrdPos::getTicket()
{
   assertPreCall();
   //if(m_CurrHist==EN_HIST)
   //{
   //   return HistoryOrderGetString(m_nTicket, ORDER_COMMENT);
   //}
   
   if(m_OrdPos==EN_POS) return PositionGetInteger(POSITION_TICKET);
   else                 return OrderGetInteger(ORDER_TICKET);
}


string CMT5OrdPos::getComments()
{
   assertPreCall();
   //if(m_CurrHist==EN_HIST)
   //{
   //   return HistoryOrderGetString(m_nTicket, ORDER_COMMENT);
   //}
   
   if(m_OrdPos==EN_POS) return ;
   else                 return OrderGetString(ORDER_COMMENT);
}


string CMT5OrdPos::getOpenTime()
{
   assertPreCall();
  
   if(m_OrdPos==EN_POS) return DatetimeToStr(PositionGetInteger(POSITION_TIME));
   else                 return DatetimeToStr(OrderGetInteger(ORDER_TIME_SETUP));
}

string CMT5OrdPos::getSymbol()
{
   assertPreCall();
   if(m_OrdPos==EN_POS) return OrderGetString(POSITION_SYMBOL);
   else                 return OrderGetString(ORDER_SYMBOL);
}


int CMT5OrdPos::getType()
{
   assertPreCall();
   if(m_OrdPos==EN_POS) return PositionGetInteger(POSITION_TYPE);
   else                 return OrderGetInteger(ORDER_TYPE);
}


string CMT5OrdPos::getTypeS()
{
   assertPreCall();
   string type;
    
   if(m_OrdPos==EN_POS)
   {
      if(PositionGetInteger(POSITION_TYPE)==POSITION_TYPE_BUY) type="BUY";
      if(PositionGetInteger(POSITION_TYPE)==POSITION_TYPE_SELL) type="SELL";
   }
   else
   {
      int n = OrderGetInteger(ORDER_TYPE);
      switch(n)
      {
      case ORDER_TYPE_BUY; type="BUY"; break;
      case ORDER_TYPE_SELL; type="SELL"; break;
      case ORDER_TYPE_BUY_LIMIT; type="BUY_LIMIT"; break;
      case ORDER_TYPE_SELL_LIMIT; type="SELL_LIMIT"; break;
      case ORDER_TYPE_BUY_STOP; type="BUY_STOP"; break;
      case ORDER_TYPE_SELL_STOP; type="SELL_STOP"; break;
      case ORDER_TYPE_BUY_STOP_LIMIT; type="BUY_STOPLIMIT"; break;
      case ORDER_TYPE_SELL_STOP_LIMIT; type="SELL_STOPLIMIT"; break;
      }
   }
   return type;
}

double  CMT5OrdPos::getLots();
{
   assertPreCall();
   if(m_OrdPos==EN_POS) return PositionGetDouble(POSITION_VOLUME);
   else                 return OrderGetDouble(ORDER_VOLUME_CURRENT);
}

double  CMT5OrdPos::getOpenPrc()
{
   assertPreCall();
   if(m_OrdPos==EN_POS) return PositionGetDouble(POSITION_PRICE_OPEN);
   else                 return OrderGetDouble(ORDER_PRICE_OPEN);
}

double  CMT5OrdPos::getStopLoss()
{
   assertPreCall();
   if(m_OrdPos==EN_POS) return PositionGetDouble(POSITION_SL);
   else                 return OrderGetDouble(ORDER_SL);
}

double  CMT5OrdPos::getTakeProfit()
{
   assertPreCall();
   if(m_OrdPos==EN_POS) return PositionGetDouble(POSITION_TP);
   else                 return OrderGetDouble(ORDER_TP);
}

double  CMT5OrdPos::getSwap()
{
   assertPreCall();
   if(m_OrdPos==EN_POS) return PositionGetDouble(POSITION_SWAP);
   else                 return 0.;
}

double  CMT5OrdPos::getProfit()
{
   assertPreCall();
   if(m_OrdPos==EN_POS) return PositionGetDouble(POSITION_PROFIT);
   else                 return 0.;
}

string CMT5OrdPos::getExpiration()
{
   if(m_OrdPos==EN_POS) return "";
   else                 return );
}


int mt5Ticket(bool bPosition)
{
   if(bPosition)   return PositionGetInteger(POSITION_TICKET);  
   return OrderGetInteger(ORDER_TICKET);
}

string mt5OpenTime(bool bPosition)
{
   if(bPosition)
      return TimeToString(PositionGetInteger(POSITION_TIME));
      
   return TimeToString(OrderGetInteger(ORDER_TIME_SETUP));
}

int mt5Type(bool bPosition)
{
   // POSITION_TYPE_BUY(0), POSITION_TYPE_SELL(1)
   if(bPosition)  return PositionGetInteger(POSITION_TYPE); 
   return OrderGetInteger(ORDER_TYPE);
}

double mt5Lots(bool bPosition)
{   
   if(bPosition)  return PositionGetDouble(POSITION_VOLUME);
   return OrderGetDouble(ORDER_VOLUME_CURRENT);
}
 
string mt5Symbol(bool bPosition)
{
   if(bPosition)  return PositionGetString(POSITION_SYMBOL);
   return OrderGetString(ORDER_SYMBOL);
}



#endif