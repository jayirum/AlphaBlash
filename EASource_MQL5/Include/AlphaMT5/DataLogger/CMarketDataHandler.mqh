#ifndef _MARKET_DATA_HANDLER_H
#define _MARKET_DATA_HANDLER_H

#include "../hash.mqh"
#include "../Utils.mqh"


class CSpread: public HashValue
{
public:
   CSpread(){m_cnt=0;};
   ~CSpread(){};
   
   uint  m_cnt; 
};

class CMDHandler
{
public:
   CMDHandler();
   ~CMDHandler();
   
   void  InitData();
   void  UpdateLastMD(double i_bid, double i_ask, double i_high, double i_low, double i_spread, datetime i_time);
   void  ResetData();
   void  ClearHash();
   
//private:
   string   symbol;
   double   bid,ask,high,low,spread;
   datetime time;
   
   double   meanSpread;
   double   modeSpread;
   double   minSpread;
   double   maxSpread;
   
   // for mean
   double   dAcmlSpread;
   double   dMDCnt;
   
   int      fd;
   string   fileName;
   
private:   
   double   pipSize;
   
   Hash*       m_pHashSpread;
   HashLoop*   m_pHashSpreadLoop;
   
   uint        nMostFreqSpreadCnt;
   string      sMostFreqSpread;
   
private:
   void     UpdateMinMaxSpread();
   void     UpdateModeSpread();
   void     ReCreateHash();
};

CMDHandler::CMDHandler()
{
}
CMDHandler::~CMDHandler()
{
   ClearHash();
}


void CMDHandler::ClearHash()
{
   if(m_pHashSpreadLoop)
   {
      for( ; m_pHashSpreadLoop.hasNext() ; m_pHashSpreadLoop.next())
      {
         CSpread* pVal = m_pHashSpreadLoop.val();
         delete pVal;
      }
      delete m_pHashSpreadLoop;
      delete m_pHashSpread;
      
      m_pHashSpread = NULL;
      m_pHashSpreadLoop = NULL;
      
   }
}

void CMDHandler::ResetData(void)
{
   bid=ask=high=low=spread=0;
   meanSpread = 0;
   modeSpread = 0;
   minSpread = 0;
   maxSpread = 0;
   dAcmlSpread = 0;
   dMDCnt = 0;
   
   ClearHash();
}

void CMDHandler::InitData()
{
   bid=ask=high=low=spread=meanSpread=modeSpread=0;
   time=0;
   fd=0;
   
   // for mean
   dAcmlSpread = 0;
   dMDCnt      = 0;
   
   minSpread   = 0;
   maxSpread   = 0;   
   
   pipSize     = __GetPointUnit(symbol);
   m_pHashSpread     = new Hash();
   m_pHashSpreadLoop = new HashLoop(m_pHashSpread);
   
   nMostFreqSpreadCnt = 0;
}


void CMDHandler::ReCreateHash()
{
   if( m_pHashSpread==NULL )
   {
      m_pHashSpread = new Hash();
      
      if(m_pHashSpreadLoop==NULL)
         m_pHashSpreadLoop = new HashLoop(m_pHashSpread);
      else
         m_pHashSpreadLoop.setHash(m_pHashSpread);               
   }

   if(m_pHashSpreadLoop==NULL)
      m_pHashSpreadLoop = new HashLoop(m_pHashSpread);  
}


void  CMDHandler::UpdateLastMD(double i_bid, double i_ask, double i_high, double i_low, double i_spread, datetime i_time)
{
   bid   = i_bid;
   ask   = i_ask;
   high  = i_high;
   low   = i_low;
   spread   = i_spread;
   time     = i_time;
   
   dAcmlSpread += spread;
   dMDCnt++;
   
   meanSpread = dAcmlSpread / dMDCnt;

   UpdateMinMaxSpread();

   UpdateModeSpread();

}

void  CMDHandler::UpdateModeSpread(void)
{
   string sSpread = StringFormat("%.1f", spread); 

   ReCreateHash();

   CSpread* pVal = m_pHashSpread.hGet(sSpread);
   if( pVal==NULL )
   {
      pVal = new CSpread;
      pVal.m_cnt=1;
   }
   else
   {
      pVal.m_cnt++;
   }

   m_pHashSpread.hPut(sSpread, pVal);
   m_pHashSpreadLoop.setHash(m_pHashSpread);
   // most frequent 
   if( nMostFreqSpreadCnt < pVal.m_cnt )
   {
      nMostFreqSpreadCnt   = pVal.m_cnt;
      modeSpread           = spread;
   }
}

void CMDHandler::UpdateMinMaxSpread()
{
   if( minSpread==0 )   
   {
      minSpread = spread;
   }
   else
   {
      minSpread = (minSpread < spread) ? minSpread : spread;
   }
      
   if( maxSpread==0 )   
   {
      maxSpread = spread;
   }
   else
   {
      maxSpread = (maxSpread > spread) ? maxSpread : spread;
   }
   
}


#endif