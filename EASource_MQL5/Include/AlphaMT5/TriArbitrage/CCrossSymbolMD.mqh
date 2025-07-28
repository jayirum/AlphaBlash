#ifndef _TRI_ARBITRAGE_MARKET_DATA_HANDLER_H
#define _TRI_ARBITRAGE_MARKET_DATA_HANDLER_H


#include "../Utils.mqh"

struct TMD
{
   string   symbol;
   double   bid;
   double   ask;
   datetime time;
};

enum {IDX_SYMBOL_1=0, IDX_SYMBOL_2, IDX_CROSS, IDX_CROSS_ARTIFICIAL};

class CCrossSymbolMD
{
public:
   CCrossSymbolMD();
   ~CCrossSymbolMD();
   
   void     InitData(string sSymbol1, string sSymbol2, string sCrossSymbol);
   bool     UpdateLastMD(int idx, double i_bid, double i_ask, datetime i_time);
   void     ResetData();
   
   bool     IsNewPrice(int idx, datetime time);
   string   InTheRangeYN();
   
   double   Arti_Price()   { return m_md[IDX_CROSS_ARTIFICIAL].bid; };
   double   Cross_Bid()    { return m_md[IDX_CROSS].bid; };
   double   Cross_Ask()    { return m_md[IDX_CROSS].ask; };
   double   Symbol1_Bid()    { return m_md[IDX_SYMBOL_1].bid; };
   double   Symbol2_Ask()    { return m_md[IDX_SYMBOL_2].ask; };
   
   //bool     IsSymbolIncluded(string sSymbol);
   string   SymbolName_1() { return m_md[IDX_SYMBOL_1].symbol; };
   string   SymbolName_2() { return m_md[IDX_SYMBOL_2].symbol; };
   string   SymbolName_Cross() { return m_md[IDX_CROSS].symbol; };
//private:
   //double   bid,ask,high,low,spread;
   //datetime time;
   
   
   
   int      fd;
   string   fileName;
   
private:
   int   IdxBySymbol(string sSymbol);
   void  Compute_ArtiPrice();
   
private:   
   int      m_nDigits;

   
   TMD      m_md[4];
private:
   
};

CCrossSymbolMD::CCrossSymbolMD()
{
}
CCrossSymbolMD::~CCrossSymbolMD()
{
   
}

//bool CCrossSymbolMD::IsSymbolIncluded(string sSymbol)
//{
//   if( m_sSymbol1       == sSymbol ||
//       m_sSymbol2       == sSymbol ||
//       m_sCrossSymbol   == sSymbol )
//   {
//      return true;
//   }
//   
//   return false;
//}

void CCrossSymbolMD::ResetData(void)
{
   for( int i=0; i<3; i++ )
   {
      m_md[i].bid = m_md[i].ask = 0;
      m_md[i].time = 0;
   }
}

void CCrossSymbolMD::InitData(string sSymbol1, string sSymbol2, string sCrossSymbol)
{
   for( int i=0; i<4; i++ )
   {
      m_md[i].bid = m_md[i].ask = 0;
      m_md[i].time = 0;
   }
   
   m_md[IDX_SYMBOL_1].symbol           = sSymbol1;
   m_md[IDX_SYMBOL_2].symbol           = sSymbol2;
   m_md[IDX_CROSS].symbol              = sCrossSymbol;   
   m_md[IDX_CROSS_ARTIFICIAL].symbol   = sCrossSymbol;
   
   m_nDigits = __GetDigits(m_md[IDX_SYMBOL_1].symbol);

   fd=0;   
}

int CCrossSymbolMD::IdxBySymbol(string sSymbol)
{
   int ret = -1;
   
   if( m_md[IDX_SYMBOL_1].symbol == sSymbol )
      ret = IDX_SYMBOL_1;
   else if( m_md[IDX_SYMBOL_2].symbol == sSymbol )
      ret = IDX_SYMBOL_2;
   else if( m_md[IDX_CROSS].symbol == sSymbol )
      ret = IDX_CROSS;
   
   return ret;
}


bool  CCrossSymbolMD::IsNewPrice(int idx, datetime time)
{
   bool isNew = ( m_md[idx].time == time )? false : true;
   return isNew;
}

bool  CCrossSymbolMD::UpdateLastMD(int idx, double i_bid, double i_ask, datetime i_time)
{
   //int idx = IdxBySymbol(sSymbol);
   //if(idx<0)
   //   return false;
      
   //if( m_md[idx].time == i_time )
   //   return false;
      
   m_md[idx].bid   = i_bid;
   m_md[idx].ask   = i_ask;
   m_md[idx].time  = i_time;
  
   if( idx == IDX_SYMBOL_1 || idx == IDX_SYMBOL_2 )
      Compute_ArtiPrice();
   
   return true;
}


void  CCrossSymbolMD::Compute_ArtiPrice()
{
   m_md[IDX_CROSS_ARTIFICIAL].bid = NormalizeDouble( m_md[IDX_SYMBOL_1].bid * m_md[IDX_SYMBOL_2].ask, m_nDigits );
}


string  CCrossSymbolMD::InTheRangeYN()
{
   if( m_md[IDX_CROSS_ARTIFICIAL].bid > m_md[IDX_CROSS].bid &&
       m_md[IDX_CROSS_ARTIFICIAL].bid < m_md[IDX_CROSS].ask )
   {
      return "Y";
   }
   
   return "N";
}

#endif