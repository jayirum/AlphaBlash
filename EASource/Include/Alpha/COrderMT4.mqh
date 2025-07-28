#ifndef __ORDER_HANDLER_MT4_H__
#define __ORDER_HANDLER_MT4_H__

#include "../stdlib.mqh"
#include "_IncCommon.mqh"

#define TIMEOUTMS_TRADECONTEXT  5


class COrderMT4
{
public:
   COrderMT4(){};
   ~COrderMT4(){};
   
   
   bool  Open_MarketOrder(_Out_ int& oTicket, _Out_ double& oExcutedPrc, string sSymbol, int nCmd, double dVol, int nSlippage,
                           double dSL, double dTP, string sComment=NULL, int nMagic=0, datetime dtExpiration=0, 
                           int nWaitSec=1, int nRetryCnt=3);
   
   bool  Open_PendingOrder(_Out_ int& oTicket, double dPrc, string sSymbol, int nCmd, double dVol, int nSlippage,
                           double dSL, double dTP, string sComment=NULL, int nMagic=0, datetime dtExpiration=0, 
                           int nWaitSec=1, int nRetryCnt=3);
                           
   bool  ModifyOrder(int Ticket, double dNewSL, double dNewTP, int nRetryCnt=3);

   bool  Close_OneSymbol(string sSymbol, int nSlippage, bool bDeletePending=false);
   bool  Close_OneTicket(string sSymbol, int Ticket, int nSlippage, int nRetryCnt=3);
   
   bool  Is_Position_Alive(int Ticket);
   bool  Is_PendingOrder_Executed(int Ticket, double& o_dOpenPrc);
   bool  Is_PositionClosed(int Ticket, double& o_dProfit);
   bool  Is_PositionClosed(int Ticket, double& o_dProfit, double& o_dClosePrc);
   //bool  Is_PositionClosed(int Ticket);
   
   int   GetGount_LivePosition(string sSymbol, bool bAllSymbol=true);
   
   double   Sum_LiveOrderProfit(string sSymbol, bool bAllSymbol=false);
   double   Sum_LiveOrderProfit(_In_ string sSymbol, _Out_ double& o_dNetLots, bool bAllSymbol=false);
   
   string   GetMsg(){ return m_sMsg;}
   int      GetMT4ErrCode()  {return m_nMT4ErrCode;}
private:
   string   m_sMsg;
   int      m_nMT4ErrCode;
}
;



double COrderMT4::Sum_LiveOrderProfit(string sSymbol, bool bAllSymbol/*=true*/)
{
   double dProfit = 0;
   int nTotCnt = OrdersTotal();
   int nPosCnt = 0;
   for( int i=0; i<nTotCnt; i++ )
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES ) )
      {
         if( !bAllSymbol )
         {
            if( sSymbol != OrderSymbol() )
               continue;
         }
         dProfit += OrderProfit();
         //PrintFormat("[PL][%d](%f)", i, OrderProfit());
      }
   }
   
   //PrintFormat("[PL-total](%f)", dProfit);
   
   return dProfit;
}


// NetLots : long + , short -
double COrderMT4::Sum_LiveOrderProfit(_In_ string sSymbol, _Out_ double& o_dNetLots, bool bAllSymbol=false)
{
   o_dNetLots     = 0;
   double dProfit = 0;
   int nTotCnt = OrdersTotal();
   int nPosCnt = 0;
   for( int i=0; i<nTotCnt; i++ )
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES ) )
      {
         if( !bAllSymbol )
         {
            if( sSymbol != OrderSymbol() )
               continue;
         }
         dProfit += (OrderProfit() - OrderCommission() + OrderSwap());
         
         if( __GetSide(OrderType())==DEF_BUY )
            o_dNetLots += OrderLots();
         else
            o_dNetLots -= OrderLots();
      }
   }
   
   //PrintFormat("[PL-total](%f)", dProfit);
   
   return dProfit;
}

int COrderMT4::GetGount_LivePosition(string sSymbol, bool bAll=true)
{
   int nTotCnt = OrdersTotal();
   int nPosCnt = 0;
   for( int i=0; i<nTotCnt; i++ )
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES ) )
      {
         if( !bAll )
         {
            if( sSymbol != OrderSymbol() )
               continue;
         }
         if( __IsMarketOrder(OrderType()) )
            nPosCnt++;
      }
   }
   
   return nPosCnt;
}



bool COrderMT4::Open_MarketOrder(
                           _Out_ int& oTicket, 
                           _Out_ double& oExcutedPrc, 
                           string   sSymbol, 
                           int      nCmd, 
                           double   dVol, 
                           int      nSlippage,
                           double   dSL, 
                           double   dTP, 
                           string   sComment    =NULL, 
                           int      nMagic      =0, 
                           datetime dtExpiration=0, 
                           int      nWaitSec    =1, 
                           int      nRetryCnt   =3
                           )
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
   double dPrc = 0;
   for( nLoop=0; nLoop<nRetryCnt; nLoop++)
   {
      dPrc = __GetCurrPrc(sSymbol, side);
      
      int slippage = (nLoop==0)? 0 : nSlippage;
      
      oTicket = OrderSend(
                        sSymbol,       // symbol
                        nCmd,          // operation
                        dVol,          // volume
                        dPrc,          // price
                        slippage,      // slippage pip
                        dSL,           // stop loss
                        dTP,           // take profit
                        sComment,      // comment
                        nMagic,        // magic number
                        dtExpiration   // pending order expiration
                        //arrow_color=clrNONE  // color
                        );
      
      if( oTicket <= 0 )
      {
         int nErrNo = GetLastError();
         m_sMsg = StringFormat("[OpenError](%s)(%s)(OrdPrc:%f)(Slpg:%d)(%.5f)(%.5f)(%s)", 
                  sSymbol, side, dPrc, slippage, dSL, dTP, __FormatErrMsg(GetLastError()));
         Print(m_sMsg);
         
         if( nErrNo==136 ) // off quotes
         {
            nSlippage += 5;   // add 5 points
         }
         
         if(nLoop==0)   Sleep(10);
         else           Sleep(1000);
         continue;
      }
      else
         break;
   }
   if(oTicket<0)
      return false;
      
   if(!OrderSelect(oTicket, SELECT_BY_TICKET , MODE_TRADES ))  
   {
      m_sMsg = StringFormat("[%s]OrderSelect failed (%d)", __FUNCTION__, oTicket);
      return false;
   }

   m_sMsg = StringFormat("[%s](%s)(%s)(Vol:%.2f)(CurrPrc:%.5f)(SL:%f)(T.Profit:%f)(Magic:%d)",
                           __FUNCTION__,sSymbol, side, dVol, dPrc, dSL, dTP, nMagic);

   oExcutedPrc = OrderOpenPrice();
   
   return true;
}





bool COrderMT4::Open_PendingOrder(
                           _Out_ int& oTicket, 
                           double   dPrc,
                           string   sSymbol, 
                           int      nCmd, 
                           double   dVol, 
                           int      nSlippage,
                           double   dSL, 
                           double   dTP, 
                           string   sComment    =NULL, 
                           int      nMagic      =0, 
                           datetime dtExpiration=0, 
                           int      nWaitSec    =1, 
                           int      nRetryCnt   =3
                           )
{
   m_nMT4ErrCode = 0;
   
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
   
   for( nLoop=0; nLoop<nRetryCnt; nLoop++)
   {
      int slippage = (nLoop==0)? 0:nSlippage;
      oTicket = OrderSend(
                        sSymbol,       // symbol
                        nCmd,          // operation
                        dVol,          // volume
                        dPrc,          // price
                        slippage,      // slippage pip
                        dSL,           // stop loss
                        dTP,           // take profit
                        sComment,      // comment
                        nMagic,        // magic number
                        dtExpiration   // pending order expiration
                        //arrow_color=clrNONE  // color
                        );
      
      if( oTicket <= 0 )
      {
         m_nMT4ErrCode = GetLastError();
         m_sMsg = StringFormat("[PendingOrd](%s)(Ord:%s)(OrdPrc:%f)(%s)(%s)", 
                  sSymbol, side, dPrc,__GetCmdStr(nCmd), __FormatErrMsg(m_nMT4ErrCode));
         Print(m_sMsg);
         
         if(nLoop==0)   Sleep(10);
         else           Sleep(100);
         continue;
      }
      else
         break;
   }
   if(oTicket<0)
      return false;
      
   //if(!OrderSelect(oTicket, SELECT_BY_TICKET , MODE_TRADES ))  
   //{
   //   m_sMsg = StringFormat("[%s]OrderSelect failed (%d)", __FUNCTION__, oTicket);
   //   return false;
   //}

   m_sMsg = StringFormat("[%s](%s)(%s)(Vol:%.2f)(CurrPrc:%.5f)(SL:%f)(T.Profit:%f)(Magic:%d)",
                           __FUNCTION__,sSymbol, side, dVol, dPrc, dSL, dTP, nMagic);
  
   return true;
}

bool  COrderMT4::ModifyOrder(int Ticket, double dNewSL, double dNewTP, int nRetryCnt/*=3*/)
{
   if( !OrderSelect(Ticket, SELECT_BY_TICKET) )
   {
      m_sMsg = StringFormat("Can't ModifyOrder - No Ticket(%d)", Ticket);
      return false;
   }
   
   bool bRes = false;
   for( int nLoop=0; nLoop<nRetryCnt; nLoop++)
   {
      bRes = OrderModify( Ticket, 0, NormalizeDouble(dNewSL,Digits), NormalizeDouble(dNewTP,Digits),0,0);
      if( !bRes )
      {
         m_nMT4ErrCode = GetLastError();
         m_sMsg = StringFormat("[OrderModify](Ticket:%d)(%s)", Ticket, __FormatErrMsg(m_nMT4ErrCode));
         Print(m_sMsg);
         
         if(nLoop==0)   Sleep(10);
         else           Sleep(100);
         continue;
      }
      else
         break;
   }
   return bRes;
}


bool  COrderMT4::Is_Position_Alive(int Ticket)
{
   if( OrderSelect(Ticket, SELECT_BY_TICKET ) )
   {
      if( OrderCloseTime()==0 )
         return true;
   }
   
   return false;
}

//bool  COrderMT4::Is_Position_Alive(int Ticket)
//{
//   for(int idx = 0; idx < OrdersTotal(); idx++) 
//   {
//      if( OrderSelect(idx, SELECT_BY_POS, MODE_TRADES ) )
//      {
//         if( OrderTicket()==Ticket )
//            return true;
//      }
//   }
//   
//   return false;
//}


//bool  COrderMT4::Is_PositionClosed(int Ticket)
//{
//   if( OrderSelect(Ticket, SELECT_BY_TICKET, MODE_TRADES ) )
//   {
//      if( OrderCloseTime()>0 )
//         return true;
//   }
//   return false;
//}

bool  COrderMT4::Is_PositionClosed(int Ticket, double& o_dProfit, double& o_dClosePrc)
{
   if( Is_PositionClosed(Ticket, _Out_ o_dProfit) )
   {
      o_dClosePrc = OrderClosePrice();
      return true;
   }
   return false;
}

bool  COrderMT4::Is_PositionClosed(int Ticket, double& o_dProfit)
{
   if( OrderSelect(Ticket, SELECT_BY_TICKET ) )
   {
      if( OrderCloseTime()!=0 )
      {
         o_dProfit   = OrderProfit() - OrderCommission() + OrderSwap();
         return true;
      }
   }
   return false;
   //for( int i=OrdersHistoryTotal()-1; i>-1; i++ )
   //{
   //   if( OrderSelect(i,SELECT_BY_POS,MODE_HISTORY) )
   //   {
   //      if( OrderTicket()==Ticket )
   //      {
   //         o_dProfit   = OrderProfit() - OrderCommission() + OrderSwap()
   //         return true;
   //      }
   //   }
   //}
   //return false;
}

bool  COrderMT4::Is_PendingOrder_Executed(int Ticket, double& o_dOpenPrc)
{
   if( OrderSelect(Ticket, SELECT_BY_TICKET, MODE_TRADES ) )
   {
      if( __IsMarketOrder(OrderType()) )
      {
         o_dOpenPrc  = OrderOpenPrice();
         return true;
      }
   }
   return false;
   //for(int idx = 0; idx < OrdersTotal(); idx++) 
   //{
   //   if( OrderSelect(idx, SELECT_BY_POS, MODE_TRADES ) )
   //   {
   //      if( OrderTicket()==Ticket )
   //      {
   //         if( __IsMarketOrder(OrderType()) )
   //         {
   //            o_dOpenPrc  = OrderOpenPrice();
   //            return true;
   //         }
   //         else 
   //            return false;
   //      }
   //   }
   //}
   //return false;
}

bool  COrderMT4::Close_OneSymbol(string sSymbol, int nSlippage, bool bDeletePending/*=false*/)
{
   for(int idx = OrdersTotal()-1; idx>-1; idx--) 
   {
      bool bRes = OrderSelect(idx, SELECT_BY_POS, MODE_TRADES );
      if(!bRes)  
      {
         PrintFormat("[%s]OrderSelect Error(%s)", __FUNCTION__, __FormatErrMsg(GetLastError()));
         continue;
      }
      
      if( OrderSymbol()!=sSymbol )
         continue;
      
      int nTicket = OrderTicket();
      if( __IsMarketOrder(OrderType()) )
      {      
         Close_OneTicket(sSymbol, nTicket, nSlippage);
      }
      else
      {
         if( bDeletePending )
         {
            bool b = OrderDelete(nTicket);
         }
      }
   }
   
   return true;
}


bool  COrderMT4::Close_OneTicket(string sSymbol, int Ticket, int nSlippage, int nRetryCnt)
{
   if( OrderSymbol()!=sSymbol )
      return false;
      
   int cmd = OrderType();
   if( !__IsMarketOrder(cmd) )
      return false;
      
   
   double   dOrdLots    = OrderLots();
   bool bRes = false;
   for(int i=0; i<nRetryCnt; i++)
   {
      double   dClosePrc   = __GetCurrPrcForClose(sSymbol, cmd);
      int slippage = (i==0)? 0:nSlippage;
      bRes = OrderClose(Ticket, dOrdLots, dClosePrc, slippage);
      if( bRes )
      {
         break;
      }
      else
      {
         int nErrNo = GetLastError();
         PrintFormat("Failed to close[%s][%d](%s)", sSymbol, Ticket, __FormatErrMsg(nErrNo));
         
         if( nErrNo==136 ) // off quotes
         {
            nSlippage += 5;   // add 5 points
         }
      }
      
      if(i==0) Sleep(10);
      else     Sleep(100);
   }
   return bRes;
}

//
//bool COrderMT4::Get_LivePosition_ByTicket(_In_ int nTicket, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dPrc, _Out_ double& dLots, _Out_ datetime& dtOpenTm)
//{
//   if(!OrderSelect(nTicket, SELECT_BY_TICKET , MODE_TRADES ))  
//   {
//      return false;
//   }
//   
//   int nMatched = 0;
//   if( status == ORDSTATUS_ALL || status==ORDSTATUS_OPENED )
//   {
//      if( __IsMarketOrder(OrderType()))
//         nMatched++;
//   }
//   if( status == ORDSTATUS_ALL || status==ORDSTATUS_PENDING )
//   {
//      if( !__IsMarketOrder(OrderType()))
//         nMatched++;
//   }
//   if(nMatched==0)
//   {
//      m_sMsg = StringFormat("Failed to find order(Ticket:%d)", nTicket);
//      return false;
//   }
//   
//   sSymbol  = OrderSymbol();
//   nCmd     = OrderType();
//   dPrc     = OrderOpenPrice();
//   dLots    = OrderLots();
//   dtOpenTm = OrderOpenTime();
//   
//   return true;
//}

//int COrderMT4::CloseAllOrders(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL)
//{
//   if( __IsMT4() )
//      return CloseAllOrders_MT4(ordStatus);
//   
//   return CloseAllOrders_MT5(ordStatus);
//}
//
//
//int COrderMT4::CloseAllOrders_MT4(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL)
//{
//   int nRsltCnt = 0;
//   bool bRes;
//   
//   int      nSlippage   = 5;
//   int      nRetryCnt   = 3;
//   
//   for(int idx = OrdersTotal()-1; idx>-1; idx--) 
//   {
//      Sleep(1);
//      bRes = OrderSelect(idx, SELECT_BY_POS, MODE_TRADES );
//      if(!bRes)  
//      {
//         PrintFormat("[CloseAllOrders_MT4]Read Order Error(%s)", __FormatErrMsg(GetLastError()));
//         continue;
//      }
//      
//      int nTicket = OrderTicket();
//      
//      if( Place_CloseDeleteOrder_withSelectedInfo_MT4(nTicket, OrderSymbol(), OrderType(), OrderLots(), nSlippage, ordStatus, nRetryCnt) )
//         nRsltCnt++;
//   }
//   return nRsltCnt;
//}
//
//int COrderMT4::CloseAllOrders_MT5(EN_ORD_STATUS ordStatus=ORDSTATUS_ALL)
//{
//   return 0;
//}
//
//int COrderMT4::Read_AllOpenOrders(_In_ string sSymbol, _In_ EN_ORD_STATUS status, TMT4OrdInfo& arrOrdInfo[])
//{
//   if( __IsMT4() )
//      return Read_AllOpenOrders_MT4(sSymbol, status, arrOrdInfo);
//   
//   return Read_AllOpenOrders_MT5(sSymbol, status, arrOrdInfo);
//}
//
//
//// enum EN_ORD_STATUS { ORDSTATUS_ALL=0, ORDSTATUS_PENDING, ORDSTATUS_OPENED, ORDSTATUS_CLOSED};
//int COrderMT4::Read_AllOpenOrders_MT4(_In_ string sSymbol, _In_ EN_ORD_STATUS status, TMT4OrdInfo& arrOrdInfo[])
//{
//   int nRsltCnt = 0;
//   bool bRes;
//   for(int idx = 0; idx<OrdersTotal(); idx++) 
//   {
//      Sleep(1);
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
//}
//
//
//int COrderMT4::Get_Tickets_LiveOrders(_In_ string sSymbol, _In_ EN_ORD_STATUS status, _Out_ int& arrTicket[])
//{
//   if(__IsMT4())
//      return Get_Tickets_LiveOrders_MT4(sSymbol, status, arrTicket);
//
//   return Get_Tickets_LiveOrders_MT5(sSymbol, status, arrTicket);
//}
//
//int COrderMT4::Get_Tickets_LiveOrders_MT4(_In_ string sSymbol, _In_ EN_ORD_STATUS status, _Out_ int& arrTicket[])
//{
//   int idxTicket = 0;
//   for(int idx = 0; idx<OrdersTotal(); idx++) 
//   {
//      Sleep(1);
//      if(!OrderSelect(idx, SELECT_BY_POS, MODE_TRADES ))  
//      {
//         // if one order is delete while this loop is running, the index is over the real index
//         if( idx>=OrdersTotal())
//         {
//            m_sMsg = "Failed to run through Trade tab(out ot index)";
//            return idxTicket;
//         }
//         Sleep(3000);
//         continue;         
//      }
//      
//      string s = OrderSymbol();
//      int nMatched = 0;
//      if( s==sSymbol )
//      {
//         if( status == ORDSTATUS_ALL || status==ORDSTATUS_OPENED )
//         {
//            if( __IsMarketOrder(OrderType()))
//               nMatched++;
//         }
//         if( status == ORDSTATUS_ALL || status==ORDSTATUS_PENDING )
//         {
//            if( !__IsMarketOrder(OrderType()))
//               nMatched++;
//         }
//         if( nMatched>0 )
//         ArrayResize(arrTicket, ArraySize(arrTicket)+1);
//         arrTicket[idxTicket] = OrderTicket();
//         idxTicket++;
//      }
//   }
//   
//   return idxTicket;
//}
//
//
//
//int COrderMT4::Get_Tickets_ClosedOrders(_In_ string sSymbol, _Out_ int& arrTicket[])
//{   
//   if(__IsMT4())
//      return Get_Tickets_ClosedOrders_MT4(sSymbol, arrTicket);
//
//   return Get_Tickets_ClosedOrders_MT5(sSymbol, arrTicket);
//
//}
//
//
//
//int COrderMT4::Get_Tickets_ClosedOrders_MT4(_In_ string sSymbol, _Out_ int& arrTicket[])
//{
//   int idxTicket = 0;
//   for(int idx = 0; idx<OrdersTotal(); idx++) 
//   {
//      Sleep(1);
//      if(!OrderSelect(idx, SELECT_BY_POS, MODE_HISTORY ))  
//      {
//         // if one order is delete while this loop is running, the index is over the real index
//         if( idx>=OrdersTotal())
//         {
//            return idxTicket;
//         }
//         Sleep(3000);
//         continue;         
//      }
//      
//      string s = OrderSymbol();
//      if( s==sSymbol )
//      {
//         ArrayResize(arrTicket, ArraySize(arrTicket)+1);
//         arrTicket[idxTicket] = OrderTicket();
//         idxTicket++;
//      }
//   }
//   
//   return idxTicket;
//}
//
//
//
//
//
//bool COrderMT4::Get_LiveOrderInfo_ByTicket(_In_ int nTicket, _In_ EN_ORD_STATUS status, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dPrc, _Out_ double& dLots, _Out_ datetime& dtOpenTm)
//{
//   if(__IsMT4())
//      return Get_LiveOrderInfo_ByTicket_MT4(_In_ nTicket, _In_ status, _Out_ sSymbol, _Out_ nCmd, _Out_ dPrc, _Out_ dLots, _Out_ dtOpenTm);
//
//   return Get_LiveOrderInfo_ByTicket_MT5(_In_ nTicket, _In_ status, _Out_ sSymbol, _Out_ nCmd, _Out_ dPrc, _Out_ dLots, _Out_ dtOpenTm);
//}
//
//
//
//bool COrderMT4::Get_LiveOrderInfo_ByTicket_MT4(_In_ int nTicket, _In_ EN_ORD_STATUS status, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dPrc, _Out_ double& dLots, _Out_ datetime& dtOpenTm)
//{
//   if(!OrderSelect(nTicket, SELECT_BY_TICKET , MODE_TRADES ))  
//   {
//      return false;
//   }
//   
//   int nMatched = 0;
//   if( status == ORDSTATUS_ALL || status==ORDSTATUS_OPENED )
//   {
//      if( __IsMarketOrder(OrderType()))
//         nMatched++;
//   }
//   if( status == ORDSTATUS_ALL || status==ORDSTATUS_PENDING )
//   {
//      if( !__IsMarketOrder(OrderType()))
//         nMatched++;
//   }
//   if(nMatched==0)
//   {
//      m_sMsg = StringFormat("Failed to find order(Ticket:%d)", nTicket);
//      return false;
//   }
//   
//   sSymbol  = OrderSymbol();
//   nCmd     = OrderType();
//   dPrc     = OrderOpenPrice();
//   dLots    = OrderLots();
//   dtOpenTm = OrderOpenTime();
//   
//   return true;
//}
//
//
//bool COrderMT4::Get_ClosedOrderInfo_ByTicket(_In_ int nTicket, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dOpenPrc, 
//                                                _Out_ double& dClosePrc, _Out_ double& dLots, _Out_ double& dCmsn,  _Out_ double& dPL, _Out_ datetime& dtCloseTm, _Out_ double& dSwap)
//{
//   if(__IsMT4())
//      return Get_ClosedOrderInfo_ByTicket_MT4(_In_ nTicket, _Out_ sSymbol, _Out_ nCmd, _Out_ dOpenPrc, dClosePrc, _Out_ dLots, dCmsn, _Out_ dPL, dtCloseTm, dSwap);
//
//   return Get_ClosedOrderInfo_ByTicket_MT5(_In_ nTicket, _Out_ sSymbol, _Out_ nCmd, _Out_ dOpenPrc, dClosePrc, _Out_ dLots, dCmsn, _Out_ dPL, dtCloseTm, dSwap);
//}
//
//
//
//bool COrderMT4::Get_ClosedOrderInfo_ByTicket_MT4(_In_ int nTicket, _Out_ string& sSymbol, _Out_ int& nCmd, _Out_ double& dOpenPrc, _Out_ double& dClosePrc, 
//                                                      _Out_ double& dLots, _Out_ double& dCmsn,  _Out_ double& dPL, _Out_ datetime& dtCloseTm, _Out_ double& dSwap)
//{
//   if(!OrderSelect(nTicket, SELECT_BY_TICKET , MODE_HISTORY ))  
//   {
//      return false;
//   }
//      
//   sSymbol  = OrderSymbol();
//   nCmd     = OrderType();
//   dOpenPrc = OrderOpenPrice();
//   dClosePrc   = OrderClosePrice();
//   dLots       = OrderLots();
//   dCmsn       = OrderCommission();
//   dPL         = OrderProfit();
//   dtCloseTm   = OrderCloseTime();
//   dSwap       = OrderSwap();
//   
//   return true;
//}
//
//
//bool COrderMT4::PlaceMarketOrder_Open(_Out_ int& oTicket,  string sSymbol, int nCmd, double dVol, int nSlippage, 
//                                          double dSL, double dTP, string sComment, int nMagic, datetime dtExpiration, 
//                                          int nWaitSec, int nRetryCnt, double dPrc)
//{
//   if(__IsMT4())
//      return PlaceMarketOrder_Open_MT4(oTicket, sSymbol,nCmd, dVol, nSlippage, dSL, dTP, sComment, nMagic, dtExpiration, nWaitSec, nRetryCnt, dPrc);
//
//   return PlaceMarketOrder_Open_MT5(oTicket, sSymbol,nCmd, dVol, nSlippage, dSL, dTP, sComment, nMagic, dtExpiration, nWaitSec, nRetryCnt, dPrc);
//}
//   

//
//bool COrderMT4::Place_CloseDeleteOrder(_In_ int nTicket, double dLots, int nSlippage, bool bCloseEntireLots=true, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3)
//{
//   if(__IsMT4() )
//      return Place_CloseDeleteOrder_MT4(_In_ nTicket, dLots, nSlippage, bCloseEntireLots, ordStatus, nRetryCnt);
//      
//   return Place_CloseDeleteOrder_MT5(_In_ nTicket, dLots, nSlippage, bCloseEntireLots, ordStatus, nRetryCnt);
//}
//
//
//
//bool COrderMT4::Place_CloseDeleteOrder_MT4(_In_ int nTicket, double dLots, int nSlippage, bool bCloseEntireLots=true, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3)
//{
//   string   sSymbol;
//   int      nCmd     = 0;
//   double   dLotsPrev= 0;
//   double   dPrc     = 0;
//   datetime dtTm;
//   if( !Get_LiveOrderInfo_ByTicket_MT4(nTicket, ordStatus, sSymbol, nCmd, dPrc, dLotsPrev, dtTm) )
//   {
//      PrintFormat("No position to be closed(%d)", nTicket);     
//      return false;
//   }
//   
//   if( bCloseEntireLots )
//      dLots = dLotsPrev;
//   
//
//   return Place_CloseDeleteOrder_withSelectedInfo_MT4(nTicket, sSymbol, nCmd, dLots, nSlippage, ordStatus, nRetryCnt);
//}
//
//
//
//bool  COrderMT4::Place_CloseDeleteOrder_withSelectedInfo(_In_ int nTicket,_In_ string sSymbol, _In_ int cmd, _In_ double dLots, _In_ int nSlippage, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3)
//{
//   if(__IsMT4() )
//      return Place_CloseDeleteOrder_withSelectedInfo_MT4(nTicket, sSymbol, cmd, dLots, nSlippage, ordStatus, nRetryCnt);
//      
//      
//   return Place_CloseDeleteOrder_withSelectedInfo_MT5(nTicket, sSymbol, cmd, dLots, nSlippage, ordStatus, nRetryCnt);
//}
//
///*
//   close or delete orders.
//   before calling this function, OrderSelect must be called to get some information.
//   
//   if you want to close the position only : ORDSTATUS_OPENED
//   if you want to delete the pending only : ORDSTATUS_PENDING
//   if you want to close/delete all        : ORDSTATUS_ALL
//*/
//bool  COrderMT4::Place_CloseDeleteOrder_withSelectedInfo_MT4(_In_ int nTicket,_In_ string sSymbol, _In_ int cmd, _In_ double dLots, _In_ int nSlippage, EN_ORD_STATUS ordStatus=ORDSTATUS_ALL, int nRetryCnt=3)
//{
//   double dClosePrc = __GetCurrPrcForClose(sSymbol, __getSideFromCmd(cmd));
//   bool bRes = true;
//   for( int nLoop=0; nLoop<nRetryCnt; nLoop++)
//   {
//      if( __IsMarketOrder(cmd) )
//      {
//         if( ordStatus==ORDSTATUS_ALL || ordStatus==ORDSTATUS_OPENED  )
//         {
//            bRes = OrderClose(nTicket, dLots, dClosePrc, nSlippage);
//         }
//      }
//      else
//      {
//         if( ordStatus==ORDSTATUS_ALL || ordStatus==ORDSTATUS_PENDING )
//         {
//            bRes = OrderDelete(nTicket);
//         }
//      }
//      if( !bRes )
//      {
//         m_sMsg = StringFormat("Failed to close(%d)(%s)(%s)", nTicket, sSymbol, __FormatErrMsg(GetLastError()));
//         Print(m_sMsg);
//         Sleep(100);
//      }
//      else
//         break;
//   } 
//
//   if(bRes)
//      m_sMsg = StringFormat("[CLOSEORD_MARKET](%s)(%d)(Lots:%.2f)(Prc:%.5f)",sSymbol, nTicket, dLots, dClosePrc);
//
//   return bRes;
//}
//
//
//
//





#endif