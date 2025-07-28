#ifndef __ORDER_HANDLER_H__
#define __ORDER_HANDLER_H__

#include "../stdlib.mqh"
#include "_IncCommon.mqh"

#define TIMEOUTMS_TRADECONTEXT  5

struct TMT4OrdInfo
{
   int            nTicket;
   EN_ORD_STATUS  enStatus;
   datetime       dtOpenTime;
   int            nCmd;
   double         dLots;
   double         dOpenPrc;
   double         dSLPrc;
   double         dTPPrc;
   datetime       dtCloseTime;
   double         dClosePrc;
   double         dCmsn;
   double         dSwap;
   double         dProfit;
   int            nMagicNo;   
};


class COrderHandler
{
public:
   COrderHandler(){};
   ~COrderHandler(){};
   
   
   
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
public: 
   bool  PlaceMarketOrder_Open(_Out_ int& oTicket,  string sSymbol, int nCmd, double dVol, int nSlippage,double dSL, double dTP, string sComment=NULL, int nMagic=0, datetime dtExpiration=0, int nWaitSec=1, int nRetryCnt=3);
   bool  PlacePendingOrder_Open(){return true;};
   
   bool  Place_CloseDeleteOrder(_In_ int nTicket, double dLots, int nSlippage, bool bCloseEntireLots=true, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3);
   bool  Place_CloseDeleteOrder_withSelectedInfo(_In_ int nTicket, _In_ string sSymbol, _In_ int cmd, _In_ double dLots, _In_ int nSlippage, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3);
   
   
   
private: 
   bool   PlaceMarketOrder_Open_MT4(_Out_ int& oTicket,  string sSymbol, int nCmd, double dVol, int nSlippage, double dSL, double dTP, string sComment=NULL, int nMagic=0, datetime dtExpiration=0, int nWaitSec=1, int nRetryCnt=3);
   bool   PlaceMarketOrder_Open_MT5(_Out_ int& oTicket,  string sSymbol, int nCmd, double dVol, int nSlippage, double dSL, double dTP, string sComment=NULL, int nMagic=0, datetime dtExpiration=0, int nWaitSec=1, int nRetryCnt=3);
   
   bool  Place_CloseDeleteOrder_MT4(_In_ int nTicket, double dLots, int nSlippage, bool bCloseEntireLots=true, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3);
   bool  Place_CloseDeleteOrder_MT5(_In_ int nTicket, double dLots, int nSlippage, bool bCloseEntireLots=true, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3){return true;};
   
   bool  Place_CloseDeleteOrder_withSelectedInfo_MT4(_In_ int nTicket, _In_ string sSymbol, _In_ int cmd, _In_ double dLots, _In_ int nSlippage, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3);
   bool  Place_CloseDeleteOrder_withSelectedInfo_MT5(_In_ int nTicket, _In_ string sSymbol, _In_ int cmd, _In_ double dLots, _In_ int nSlippage, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3){return true;}
   
   bool  PlacePendingOrder_Open_MT4(){return true;};
   bool  PlacePendingOrder_Open_MT5(){return true;};
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
public:
   int  CloseAllOrders(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL);
   
private:
   int  CloseAllOrders_MT4(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL);     
   int  CloseAllOrders_MT5(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL);
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
public:
   int   Get_Tickets_LiveOrders(_In_ string sSymbol, _In_ EN_ORD_STATUS status, _Out_ int& arrTicket[]);
   int   Get_Tickets_ClosedOrders(_In_ string sSymbol, _Out_ int& arrTicket[]);
private:
   int   Get_Tickets_LiveOrders_MT4(_In_ string sSymbol, _In_ EN_ORD_STATUS status, _Out_ int& arrTicket[]);
   int   Get_Tickets_LiveOrders_MT5(_In_ string sSymbol, _In_ EN_ORD_STATUS status, _Out_ int& arrTicket[]){return 0;};
   
   int   Get_Tickets_ClosedOrders_MT4(_In_ string sSymbol, _Out_ int& arrTicket[]);
   int   Get_Tickets_ClosedOrders_MT5(_In_ string sSymbol, _Out_ int& arrTicket[]){return 0;};
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   
public:
   bool   Get_LiveOrderInfo_ByTicket(_In_ int nTicket, _In_ EN_ORD_STATUS status, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dPrc, _Out_ double& dLots, _Out_ datetime& dtOpenTm);
   bool   Get_ClosedOrderInfo_ByTicket(_In_ int nTicket, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dOpenPrc, _Out_ double& dClosePrc, 
                                          _Out_ double& dLots, _Out_ double& dCmsn,  _Out_ double& dPL, _Out_ datetime& dtCloseTm, _Out_ double& dSwap);
private:
   bool   Get_LiveOrderInfo_ByTicket_MT4(_In_ int nTicket, _In_ EN_ORD_STATUS status, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dPrc, _Out_ double& dLots, _Out_ datetime& dtOpenTm);
   bool   Get_LiveOrderInfo_ByTicket_MT5(_In_ int nTicket, _In_ EN_ORD_STATUS status, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dPrc, _Out_ double& dLots, _Out_ datetime& dtOpenTm){return true;};
   
   bool   Get_ClosedOrderInfo_ByTicket_MT4(_In_ int nTicket, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dOpenPrc, 
                                             _Out_ double& dClosePrc, _Out_ double& dLots, _Out_ double& dCmsn, _Out_ double& dPL, _Out_ datetime& dtCloseTm, _Out_ double& dSwap);
   bool   Get_ClosedOrderInfo_ByTicket_MT5(_In_ int nTicket, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dOpenPrc, 
                                             _Out_ double& dClosePrc, _Out_ double& dLots, _Out_ double& dCmsn, _Out_ double& dPL, _Out_ datetime& dtCloseTm, _Out_ double& dSwap){return true;};
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
public:
   int   Read_AllOpenOrders(_In_ string sSymbol, _In_ EN_ORD_STATUS status, TMT4OrdInfo& arrOrdInfo[]);
   
public:
   bool   Open_NumsVols_Symbol(_In_ string sSymbol, _In_ EN_POS_ORD_TP posordTp, _In_ string sSideDef, _Out_ int& o_nNums, _Out_ double& o_dVols);
   bool   Closed_NumsVols_Symbol(_In_ string sSymbol, _In_ EN_POS_ORD_TP posordTp, _In_ string sSideDef, _Out_ int& o_nNums, _Out_ double& o_dVols) { return true;};
private:
   bool   Open_NumsVols_Symbol_MT4(_In_ string sSymbol, _In_ EN_POS_ORD_TP posordTp, _In_ string sSideDef, _Out_ int& o_nNums, _Out_ double& o_dVols);
   bool   Open_NumsVols_Symbol_MT5(_In_ string sSymbol, _In_ EN_POS_ORD_TP posordTp, _In_ string sSideDef, _Out_ int& o_nNums, _Out_ double& o_dVols);
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   
   
private:
   int  Read_AllOpenOrders_MT4(_In_ string sSymbol, _In_ EN_ORD_STATUS status, TMT4OrdInfo& arrOrdInfo[]);
   int  Read_AllOpenOrders_MT5(_In_ string sSymbol, _In_ EN_ORD_STATUS status, TMT4OrdInfo& arrOrdInfo[]){return true;};
   
public:      
   string   GetMsg(){ return m_sMsg;}
private:
   string   m_sMsg;   
}
;

bool COrderHandler::Open_NumsVols_Symbol(_In_ string sSymbol, _In_ EN_POS_ORD_TP posordTp, _In_ string sSideDef, _Out_ int& o_nNums, _Out_ double& o_dVols)
{
   if( __IsMT4() )
      return Open_NumsVols_Symbol_MT4(sSymbol, posordTp, sSideDef, o_nNums, o_dVols);
   
   return Open_NumsVols_Symbol_MT5(sSymbol, posordTp, sSideDef, o_nNums, o_dVols);
}


bool COrderHandler::Open_NumsVols_Symbol_MT4(string sSymbol,EN_POS_ORD_TP posordTp,string sSideDef,int &o_nNums,double &o_dVols)
{
   return true;
//   o_nNums  = 0;
//   o_dVols  = 0;
//   
//   bool bRes;
//   for(int idx = 0; idx<OrdersTotal(); idx++) 
//   {
//      Sleep(1);
//      
//      if(  
//      
//      if( status==ORDSTATUS_CLOSED )
//         bRes = OrderSelect(idx, SELECT_BY_POS, MODE_HISTORY );
//      else
//         bRes = OrderSelect(idx, SELECT_BY_POS, MODE_TRADES );
//      if(!bRes)  
//      {
//         // if one order is delete while this loop is running, the index is over the real index
//         if( idx>=OrdersTotal())
//            m_sMsg = "Failed to run through Trade tab(out ot index)";
//         else
//            m_sMsg = StringFormat("Read Order Error(%s)", __FormatErrMsg(GetLastError()));
//         
//         return nRsltCnt;
//      }
//      
//      // pending 
//      if( status == ORDSTATUS_PENDING && !__IsPendingOrder(OrderType()) )
//         continue;
//      
//      if( status == ORDSTATUS_OPENED && __IsPendingOrder(OrderType()) )
//         continue;
//      
//      
//      arrOrdInfo[nRsltCnt].nTicket     = OrderTicket();
//      arrOrdInfo[nRsltCnt].enStatus    = status;
//      arrOrdInfo[nRsltCnt].dtOpenTime  = OrderOpenTime();
//      arrOrdInfo[nRsltCnt].nCmd        = OrderType();
//      arrOrdInfo[nRsltCnt].dLots       = OrderLots();
//      arrOrdInfo[nRsltCnt].dOpenPrc    = OrderOpenPrice();
//      arrOrdInfo[nRsltCnt].dSLPrc      = OrderStopLoss();
//      arrOrdInfo[nRsltCnt].dTPPrc      = OrderTakeProfit();
//      
//      arrOrdInfo[nRsltCnt].dClosePrc   = OrderClosePrice();
//      arrOrdInfo[nRsltCnt].dCmsn       = OrderCommission();
//      arrOrdInfo[nRsltCnt].dSwap       = OrderSwap();
//      arrOrdInfo[nRsltCnt].dProfit     = OrderProfit();
//      arrOrdInfo[nRsltCnt].nMagicNo    = OrderMagicNumber();
//      
//      nRsltCnt++;
//   }
//   return nRsltCnt;

}



int COrderHandler::CloseAllOrders(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL)
{
   if( __IsMT4() )
      return CloseAllOrders_MT4(ordStatus);
   
   return CloseAllOrders_MT5(ordStatus);
}


int COrderHandler::CloseAllOrders_MT4(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL)
{
   int nRsltCnt = 0;
   bool bRes;
   
   int      nSlippage   = 5;
   int      nRetryCnt   = 3;
   
   for(int idx = OrdersTotal()-1; idx>-1; idx--) 
   {
      Sleep(1);
      bRes = OrderSelect(idx, SELECT_BY_POS, MODE_TRADES );
      if(!bRes)  
      {
         PrintFormat("[CloseAllOrders_MT4]Read Order Error(%s)", __FormatErrMsg(GetLastError()));
         continue;
      }
      
      int nTicket = OrderTicket();
      
      if( Place_CloseDeleteOrder_withSelectedInfo_MT4(nTicket, OrderSymbol(), OrderType(), OrderLots(), nSlippage, ordStatus, nRetryCnt) )
         nRsltCnt++;
   }
   return nRsltCnt;
}

int COrderHandler::CloseAllOrders_MT5(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL)
{
   return 0;
}

int COrderHandler::Read_AllOpenOrders(_In_ string sSymbol, _In_ EN_ORD_STATUS status, TMT4OrdInfo& arrOrdInfo[])
{
   if( __IsMT4() )
      return Read_AllOpenOrders_MT4(sSymbol, status, arrOrdInfo);
   
   return Read_AllOpenOrders_MT5(sSymbol, status, arrOrdInfo);
}


// enum EN_ORD_STATUS { ORDSTATUS_ALL=0, ORDSTATUS_PENDING, ORDSTATUS_OPENED, ORDSTATUS_CLOSED};
int COrderHandler::Read_AllOpenOrders_MT4(_In_ string sSymbol, _In_ EN_ORD_STATUS status, TMT4OrdInfo& arrOrdInfo[])
{
   int nRsltCnt = 0;
   bool bRes;
   for(int idx = 0; idx<OrdersTotal(); idx++) 
   {
      Sleep(1);
      if( status==ORDSTATUS_CLOSED )
         bRes = OrderSelect(idx, SELECT_BY_POS, MODE_HISTORY );
      else
         bRes = OrderSelect(idx, SELECT_BY_POS, MODE_TRADES );
      if(!bRes)  
      {
         // if one order is delete while this loop is running, the index is over the real index
         if( idx>=OrdersTotal())
            m_sMsg = "Failed to run through Trade tab(out ot index)";
         else
            m_sMsg = StringFormat("Read Order Error(%s)", __FormatErrMsg(GetLastError()));
         
         return nRsltCnt;
      }
      
      // pending 
      if( status == ORDSTATUS_PENDING && !__IsPendingOrder(OrderType()) )
         continue;
      
      if( status == ORDSTATUS_OPENED && __IsPendingOrder(OrderType()) )
         continue;
      
      
      arrOrdInfo[nRsltCnt].nTicket     = OrderTicket();
      arrOrdInfo[nRsltCnt].enStatus    = status;
      arrOrdInfo[nRsltCnt].dtOpenTime  = OrderOpenTime();
      arrOrdInfo[nRsltCnt].nCmd        = OrderType();
      arrOrdInfo[nRsltCnt].dLots       = OrderLots();
      arrOrdInfo[nRsltCnt].dOpenPrc    = OrderOpenPrice();
      arrOrdInfo[nRsltCnt].dSLPrc      = OrderStopLoss();
      arrOrdInfo[nRsltCnt].dTPPrc      = OrderTakeProfit();
      
      arrOrdInfo[nRsltCnt].dClosePrc   = OrderClosePrice();
      arrOrdInfo[nRsltCnt].dCmsn       = OrderCommission();
      arrOrdInfo[nRsltCnt].dSwap       = OrderSwap();
      arrOrdInfo[nRsltCnt].dProfit     = OrderProfit();
      arrOrdInfo[nRsltCnt].nMagicNo    = OrderMagicNumber();
      
      nRsltCnt++;
   }
   return nRsltCnt;
}


int COrderHandler::Get_Tickets_LiveOrders(_In_ string sSymbol, _In_ EN_ORD_STATUS status, _Out_ int& arrTicket[])
{
   if(__IsMT4())
      return Get_Tickets_LiveOrders_MT4(sSymbol, status, arrTicket);

   return Get_Tickets_LiveOrders_MT5(sSymbol, status, arrTicket);
}

int COrderHandler::Get_Tickets_LiveOrders_MT4(_In_ string sSymbol, _In_ EN_ORD_STATUS status, _Out_ int& arrTicket[])
{
   int idxTicket = 0;
   for(int idx = 0; idx<OrdersTotal(); idx++) 
   {
      Sleep(1);
      if(!OrderSelect(idx, SELECT_BY_POS, MODE_TRADES ))  
      {
         // if one order is delete while this loop is running, the index is over the real index
         if( idx>=OrdersTotal())
         {
            m_sMsg = "Failed to run through Trade tab(out ot index)";
            return idxTicket;
         }
         Sleep(3000);
         continue;         
      }
      
      string s = OrderSymbol();
      int nMatched = 0;
      if( s==sSymbol )
      {
         if( status == ORDSTATUS_ALL || status==ORDSTATUS_OPENED )
         {
            if( __IsMarketOrder(OrderType()))
               nMatched++;
         }
         if( status == ORDSTATUS_ALL || status==ORDSTATUS_PENDING )
         {
            if( !__IsMarketOrder(OrderType()))
               nMatched++;
         }
         if( nMatched>0 )
         ArrayResize(arrTicket, ArraySize(arrTicket)+1);
         arrTicket[idxTicket] = OrderTicket();
         idxTicket++;
      }
   }
   
   return idxTicket;
}



int COrderHandler::Get_Tickets_ClosedOrders(_In_ string sSymbol, _Out_ int& arrTicket[])
{   
   if(__IsMT4())
      return Get_Tickets_ClosedOrders_MT4(sSymbol, arrTicket);

   return Get_Tickets_ClosedOrders_MT5(sSymbol, arrTicket);

}



int COrderHandler::Get_Tickets_ClosedOrders_MT4(_In_ string sSymbol, _Out_ int& arrTicket[])
{
   int idxTicket = 0;
   for(int idx = 0; idx<OrdersTotal(); idx++) 
   {
      Sleep(1);
      if(!OrderSelect(idx, SELECT_BY_POS, MODE_HISTORY ))  
      {
         // if one order is delete while this loop is running, the index is over the real index
         if( idx>=OrdersTotal())
         {
            return idxTicket;
         }
         Sleep(3000);
         continue;         
      }
      
      string s = OrderSymbol();
      if( s==sSymbol )
      {
         ArrayResize(arrTicket, ArraySize(arrTicket)+1);
         arrTicket[idxTicket] = OrderTicket();
         idxTicket++;
      }
   }
   
   return idxTicket;
}





bool COrderHandler::Get_LiveOrderInfo_ByTicket(_In_ int nTicket, _In_ EN_ORD_STATUS status, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dPrc, _Out_ double& dLots, _Out_ datetime& dtOpenTm)
{
   if(__IsMT4())
      return Get_LiveOrderInfo_ByTicket_MT4(_In_ nTicket, _In_ status, _Out_ sSymbol, _Out_ nCmd, _Out_ dPrc, _Out_ dLots, _Out_ dtOpenTm);

   return Get_LiveOrderInfo_ByTicket_MT5(_In_ nTicket, _In_ status, _Out_ sSymbol, _Out_ nCmd, _Out_ dPrc, _Out_ dLots, _Out_ dtOpenTm);
}



bool COrderHandler::Get_LiveOrderInfo_ByTicket_MT4(_In_ int nTicket, _In_ EN_ORD_STATUS status, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dPrc, _Out_ double& dLots, _Out_ datetime& dtOpenTm)
{
   if(!OrderSelect(nTicket, SELECT_BY_TICKET , MODE_TRADES ))  
   {
      return false;
   }
   
   int nMatched = 0;
   if( status == ORDSTATUS_ALL || status==ORDSTATUS_OPENED )
   {
      if( __IsMarketOrder(OrderType()))
         nMatched++;
   }
   if( status == ORDSTATUS_ALL || status==ORDSTATUS_PENDING )
   {
      if( !__IsMarketOrder(OrderType()))
         nMatched++;
   }
   if(nMatched==0)
   {
      m_sMsg = StringFormat("Failed to find order(Ticket:%d)", nTicket);
      return false;
   }
   
   sSymbol  = OrderSymbol();
   nCmd     = OrderType();
   dPrc     = OrderOpenPrice();
   dLots    = OrderLots();
   dtOpenTm = OrderOpenTime();
   
   return true;
}


bool COrderHandler::Get_ClosedOrderInfo_ByTicket(_In_ int nTicket, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dOpenPrc, 
                                                _Out_ double& dClosePrc, _Out_ double& dLots, _Out_ double& dCmsn,  _Out_ double& dPL, _Out_ datetime& dtCloseTm, _Out_ double& dSwap)
{
   if(__IsMT4())
      return Get_ClosedOrderInfo_ByTicket_MT4(_In_ nTicket, _Out_ sSymbol, _Out_ nCmd, _Out_ dOpenPrc, dClosePrc, _Out_ dLots, dCmsn, _Out_ dPL, dtCloseTm, dSwap);

   return Get_ClosedOrderInfo_ByTicket_MT5(_In_ nTicket, _Out_ sSymbol, _Out_ nCmd, _Out_ dOpenPrc, dClosePrc, _Out_ dLots, dCmsn, _Out_ dPL, dtCloseTm, dSwap);
}



bool COrderHandler::Get_ClosedOrderInfo_ByTicket_MT4(_In_ int nTicket, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dOpenPrc, _Out_ double& dClosePrc, 
                                                      _Out_ double& dLots, _Out_ double& dCmsn,  _Out_ double& dPL, _Out_ datetime& dtCloseTm, _Out_ double& dSwap)
{
   if(!OrderSelect(nTicket, SELECT_BY_TICKET , MODE_HISTORY ))  
   {
      return false;
   }
      
   sSymbol  = OrderSymbol();
   nCmd     = OrderType();
   dOpenPrc = OrderOpenPrice();
   dClosePrc   = OrderClosePrice();
   dLots       = OrderLots();
   dCmsn       = OrderCommission();
   dPL         = OrderProfit();
   dtCloseTm   = OrderCloseTime();
   dSwap       = OrderSwap();
   
   return true;
}


bool COrderHandler::PlaceMarketOrder_Open(_Out_ int& oTicket,  string sSymbol, int nCmd, double dVol, int nSlippage, 
                                          double dSL, double dTP, string sComment, int nMagic, datetime dtExpiration, int nWaitSec, int nRetryCnt)
{
   if(__IsMT4())
      return PlaceMarketOrder_Open_MT4(oTicket, sSymbol,nCmd, dVol, nSlippage, dSL, dTP, sComment, nMagic, dtExpiration, nWaitSec, nRetryCnt);

   return PlaceMarketOrder_Open_MT5(oTicket, sSymbol,nCmd, dVol, nSlippage, dSL, dTP, sComment, nMagic, dtExpiration, nWaitSec, nRetryCnt);
}
   
bool COrderHandler::PlaceMarketOrder_Open_MT4(_Out_ int& oTicket,  string sSymbol, int nCmd, double dVol, int nSlippage, 
                                          double dSL, double dTP, string sComment, int nMagic, datetime dtExpiration, int nWaitSec, int nRetryCnt)
{
   
   int nCnt = (nWaitSec*1000) / TIMEOUTMS_TRADECONTEXT;
   int nLoop = 0;
   
   while( !IsTradeAllowed() )
   {
      Sleep(TIMEOUTMS_TRADECONTEXT);
      if(++nLoop >= nCnt)
         break;
   }
   if( nLoop>=nCnt )
   {
      m_sMsg = StringFormat("IsTradeAllowed not allowed or Trade Context is busy now(Cnt:%d)(Loop:%d)", nCnt, nLoop);
      return false;
   }
   
   
    
   string side = __GetSide(nCmd);
   double dPrc = __GetCurrPrc(sSymbol, side);

   for( nLoop=0; nLoop<nRetryCnt; nLoop++)
   {
      oTicket = OrderSend(
      sSymbol,       // symbol
      nCmd,          // operation
      dVol,          // volume
      dPrc,          // price
      nSlippage,     // slippage pip
      dSL,           // stop loss
      dTP,           // take profit
      sComment,      // comment
      nMagic,        // magic number
      dtExpiration   // pending order expiration
      //arrow_color=clrNONE  // color
      );
      
      if( oTicket < 0 )
      {
         m_sMsg = StringFormat("[OpenError](%s)(side:%s)(OrdPrc:%f)(%s)", sSymbol, side, dPrc,__FormatErrMsg(GetLastError()));
         Print(m_sMsg);
         Sleep(100);
         continue;
      }
      else
         break;
   }
   if(oTicket<0)
      return false;
      
   m_sMsg = StringFormat("[OPENORD_MARKET](%s)(%s)(Vol:%.2f)(CurrPrc:%.5f)(SL:%f)(T.Profit:%f)(Magic:%d)",
                                          sSymbol, side, dVol, dPrc, dSL, dTP, nMagic);

   return true;
}


bool COrderHandler::PlaceMarketOrder_Open_MT5(_Out_ int& oTicket,  string sSymbol, int nCmd, double dVol, int nSlippage, 
                                          double dSL, double dTP, string sComment, int nMagic, datetime dtExpiration, int nWaitSec, int nRetryCnt)
{
   return true;
}


bool COrderHandler::Place_CloseDeleteOrder(_In_ int nTicket, double dLots, int nSlippage, bool bCloseEntireLots=true, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3)
{
   if(__IsMT4() )
      return Place_CloseDeleteOrder_MT4(_In_ nTicket, dLots, nSlippage, bCloseEntireLots, ordStatus, nRetryCnt);
      
   return Place_CloseDeleteOrder_MT5(_In_ nTicket, dLots, nSlippage, bCloseEntireLots, ordStatus, nRetryCnt);
}



bool COrderHandler::Place_CloseDeleteOrder_MT4(_In_ int nTicket, double dLots, int nSlippage, bool bCloseEntireLots=true, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3)
{
   string   sSymbol;
   int      nCmd     = 0;
   double   dLotsPrev= 0;
   double   dPrc     = 0;
   datetime dtTm;
   if( !Get_LiveOrderInfo_ByTicket_MT4(nTicket, ordStatus, sSymbol, nCmd, dPrc, dLotsPrev, dtTm) )
   {
      PrintFormat("No position to be closed(%d)", nTicket);     
      return false;
   }
   
   if( bCloseEntireLots )
      dLots = dLotsPrev;
   

   return Place_CloseDeleteOrder_withSelectedInfo_MT4(nTicket, sSymbol, nCmd, dLots, nSlippage, ordStatus, nRetryCnt);
}



bool  COrderHandler::Place_CloseDeleteOrder_withSelectedInfo(_In_ int nTicket,_In_ string sSymbol, _In_ int cmd, _In_ double dLots, _In_ int nSlippage, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3)
{
   if(__IsMT4() )
      return Place_CloseDeleteOrder_withSelectedInfo_MT4(nTicket, sSymbol, cmd, dLots, nSlippage, ordStatus, nRetryCnt);
      
      
   return Place_CloseDeleteOrder_withSelectedInfo_MT5(nTicket, sSymbol, cmd, dLots, nSlippage, ordStatus, nRetryCnt);
}

/*
   close or delete orders.
   before calling this function, OrderSelect must be called to get some information.
   
   if you want to close the position only : ORDSTATUS_OPENED
   if you want to delete the pending only : ORDSTATUS_PENDING
   if you want to close/delete all        : ORDSTATUS_ALL
*/
bool  COrderHandler::Place_CloseDeleteOrder_withSelectedInfo_MT4(_In_ int nTicket,_In_ string sSymbol, _In_ int cmd, _In_ double dLots, _In_ int nSlippage, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3)
{
   double dClosePrc = __GetCurrPrcForClose(sSymbol, __getSideFromCmd(cmd));
   bool bRes = true;
   for( int nLoop=0; nLoop<nRetryCnt; nLoop++)
   {
      if( __IsMarketOrder(cmd) )
      {
         if( ordStatus==ORDSTATUS_ALL || ordStatus==ORDSTATUS_OPENED  )
         {
            bRes = OrderClose(nTicket, dLots, dClosePrc, nSlippage);
         }
      }
      else
      {
         if( ordStatus==ORDSTATUS_ALL || ordStatus==ORDSTATUS_PENDING )
         {
            bRes = OrderDelete(nTicket);
         }
      }
      if( !bRes )
      {
         m_sMsg = StringFormat("Failed to close(%d)(%s)(%s)", nTicket, sSymbol, __FormatErrMsg(GetLastError()));
         Print(m_sMsg);
         Sleep(100);
      }
      else
         break;
   } 

   if(bRes)
      m_sMsg = StringFormat("[CLOSEORD_MARKET](%s)(%d)(Lots:%.2f)(Prc:%.5f)",sSymbol, nTicket, dLots, dClosePrc);

   return bRes;
}









#endif