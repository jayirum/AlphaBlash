#ifndef _ALPHA_PLACE_MT5_ORDER_H_
#define _ALPHA_PLACE_MT5_ORDER_H_


#include "../Utils.mqh"
#include "../MT5_OrdPos.mqh"

/*
struct MqlTradeRequest
  {
   ENUM_TRADE_REQUEST_ACTIONS    action;           // Trade operation type
   ulong                         magic;            // Expert Advisor ID (magic number)
   ulong                         order;            // Order ticket
   string                        symbol;           // Trade symbol
   double                        volume;           // Requested volume for a deal in lots
   double                        price;            // Price
   double                        stoplimit;        // StopLimit level of the order
   double                        sl;               // Stop Loss level of the order
   double                        tp;               // Take Profit level of the order
   ulong                         deviation;        // Maximal possible deviation from the requested price
   ENUM_ORDER_TYPE               type;             // Order type
   ENUM_ORDER_TYPE_FILLING       type_filling;     // Order execution type
   ENUM_ORDER_TYPE_TIME          type_time;        // Order expiration type
   datetime                      expiration;       // Order expiration time (for the orders of ORDER_TIME_SPECIFIED type)
   string                        comment;          // Order comment
   ulong                         position;         // Position ticket
   ulong                         position_by;      // The ticket of an opposite position
  };
  
  
*/

class CPlaceMT5Order
{
public:
    CPlaceMT5Order(){};
    ~CPlaceMT5Order(){};
    
    static bool   Check_BeforePlaceOrder(MqlTradeRequest& req);
    
    static bool   Place_OpenOrder( string sSymbol
                                       ,int nOrdType
                                       ,double dOrdLots
                                       ,double dOrdPrc
                                       ,double dStopLoss
                                       ,double dTakeProfit
                                       ,int    nMasterTicket
                                       ,string sComments
                                       ,_Out_ int& nNewTicket
                                       ,_Out_ MqlTradeResult& ordResult
                                       );
    static bool Place_ModifyOrder( 
                                       _In_  int nTicket
                                       ,_In_ bool bPosition
                                       ,_In_ bool bPrcChange
                                       ,_In_ bool bSLChange
                                       ,_In_ bool bTPChange
                                       ,_In_ bool bExpiryChange
                                       ,_In_ double dNewPrc
                                       ,_In_ double dNewSL
                                       ,_In_ double dNewTP
                                       ,_In_ datetime dtNewExpiry
                                       );
                                       
    static bool Place_Delete_Close_Partial( EN_ORD_ACTION enAction, int nTicket, double dClosePrc, double dPartialLots);
    
    static string  GetMsg(){return m_sMsg;}
    
    
    //bool    LinkFilter(CConfigUI* config);

private:
    //CConfigUI*    m_pConfig;
    static string        m_sMsg;   
};

string CPlaceMT5Order::m_sMsg;


/*
struct MqlTradeCheckResult
  {
   uint         retcode;             // Reply code
   double       balance;             // Balance after the execution of the deal
   double       equity;              // Equity after the execution of the deal
   double       profit;              // Floating profit
   double       margin;              // Margin requirements
   double       margin_free;         // Free margin
   double       margin_level;        // Margin level
   string       comment;             // Comment to the reply code (description of the error)
  };
*/
bool CPlaceMT5Order::Check_BeforePlaceOrder(MqlTradeRequest& req)
{
   MqlTradeCheckResult result = {0};
   
   ResetLastError();
   bool bSuccess = OrderCheck(req, result);
   if( bSuccess==false )
   {
      m_sMsg = StringFormat("[OrderCheck Error]%s",__FormatResult(m_sMsg, result.retcode));
      return false;
    }
   
   return true;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Place Open Order via MT4 API
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CPlaceMT5Order::Place_OpenOrder( string sSymbol
                                       ,int nOrdType
                                       ,double dOrdLots
                                       ,double dOrdPrc
                                       ,double dStopLoss
                                       ,double dTakeProfit
                                       ,int    nMasterTicket
                                       ,string sComments
                                       ,_Out_ int& nNewTicket
                                       ,_Out_ MqlTradeResult& ordResult
                                       )
{
   

   ZeroMemory(ordResult);
   MqlTradeRequest req     = {0};
   
   bool bMarketOrd = __IsMarketOrder(nOrdType);

   req.type    = nOrdType;
   req.magic   = nMasterTicket;       
   req.symbol  = sSymbol; 
   req.volume  = dOrdLots;
   req.price   = dOrdPrc;
   req.sl      = dStopLoss;
   req.tp      = dTakeProfit;
   req.deviation  = ULONG_MAX;
   req.comment    = sComments;
   req.type_filling = ORDER_FILLING_IOC;
   
   if(bMarketOrd)
   {
      req.action  = TRADE_ACTION_DEAL;
   }
   else
   {
      req.action  = TRADE_ACTION_PENDING;
   }
   
    
   if(Check_BeforePlaceOrder(req)==false )
      return false;
    
   if( OrderSend(req, ordResult)==false)
   {
      m_sMsg = StringFormat("[Place_OpenOrder Error-1](%s)", __FormatResult(m_sMsg, ordResult.retcode));
      return false;
   }
    
   if(ordResult.retcode!=TRADE_RETCODE_DONE)
   {
      m_sMsg = StringFormat("[Place_OpenOrder Error-2](%s)", __FormatResult(m_sMsg, ordResult.retcode));
      return false;
   }

   nNewTicket = (int)ordResult.order;
   debug(StringFormat("[Place_OpenOrder Ok]ticket:%d, vol:%f, prc:%f, bid:%f, ask:%f, magic:%d"
                     ,ordResult.order, ordResult.volume, ordResult.price, ordResult.bid, ordResult.ask, nMasterTicket));
                  
    return true;
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// For position - modify SL or TP
// For order    - modify SL or TP or Price
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CPlaceMT5Order::Place_ModifyOrder( 
                                       _In_  int nTicket
                                       ,_In_ bool bPosition
                                       ,_In_ bool bPrcChange
                                       ,_In_ bool bSLChange
                                       ,_In_ bool bTPChange
                                       ,_In_ bool bExpiryChange
                                       ,_In_ double dNewPrc
                                       ,_In_ double dNewSL
                                       ,_In_ double dNewTP
                                       ,_In_ datetime dtNewExpiry
                                       )
{
   ORD_INFO ordInfo = {0};
   CMT5OrdPos::FindAndReadOrdInfo(bPosition, nTicket, ordInfo);
   
   MqlTradeResult  resultOrd  = {0};
   MqlTradeRequest req        = {0};
   
   if( bPosition )
   {
      req.action     = TRADE_ACTION_SLTP;
      req.position   = ordInfo.nTicket;
   }
   else
   {
      req.action  = TRADE_ACTION_MODIFY;
      req.order   = ordInfo.nTicket;
   }
   
   if( bPrcChange )  req.price   = dNewPrc;
   if( bSLChange  )  req.sl      = dNewSL;
   if( bTPChange  )  req.tp      = dNewTP;
   if( bExpiryChange)   req.expiration = dtNewExpiry;
   
   bool bSuccess = OrderSend(req, resultOrd);
   if(bSuccess==false)
   {
      m_sMsg = StringFormat("[Place_ModifyOrder Error-1][ticket:%d](%s)", ordInfo.nTicket, __FormatResult(m_sMsg, resultOrd.retcode));
      return false;
   }
 
   if(resultOrd.retcode!=TRADE_RETCODE_DONE)
      m_sMsg = StringFormat("[Place_ModifyOrder Error-2][ticket:%d](%s)", ordInfo.nTicket, __FormatResult(m_sMsg, resultOrd.retcode));
   else
      debug(StringFormat("[Place_ModifyOrder Ok]ticket:%d, magic:%d", resultOrd.order, ordInfo.nMagic));
                  
    return true;
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Delete pending order / close / partial
// - sAction : D/C/P
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CPlaceMT5Order::Place_Delete_Close_Partial( EN_ORD_ACTION enAction, int nTicket, double dClosePrc, double dPartialLots)
{
   bool bPosition = (enAction==ORD_ACTION_DELETE)? false:true;
   ORD_INFO ordInfo = {0};
   CMT5OrdPos::FindAndReadOrdInfo(bPosition, nTicket, ordInfo);
   string sAction;
   MqlTradeResult  resultOrd  = {0};
   MqlTradeRequest req        = {0};
   
   if( enAction==ORD_ACTION_DELETE)
   {
      req.action  = TRADE_ACTION_REMOVE;
      req.order   = ordInfo.nTicket;
   }
   else
   {
      req.action     = TRADE_ACTION_DEAL;
      req.position   = nTicket;
      req.symbol     = PositionGetString(POSITION_SYMBOL);
      req.price      = dClosePrc;
      req.deviation  = ULONG_MAX;
      req.type_filling = ORDER_FILLING_IOC;
      
      ENUM_POSITION_TYPE type=(ENUM_POSITION_TYPE)PositionGetInteger(POSITION_TYPE);
      if(type==POSITION_TYPE_BUY)
         req.type =ORDER_TYPE_SELL;
      else
         req.type =ORDER_TYPE_BUY;
      
      if ( enAction==ORD_ACTION_CLOSE_FULL ){
         req.volume  = PositionGetDouble(POSITION_VOLUME);
         sAction = "CLOSE";
      }
      else if ( enAction==ORD_ACTION_CLOSE_PARTIAL ){
         req.volume  = dPartialLots;
         sAction = "PARTIAL";
      }
   }
      
   bool bSuccess = OrderSend(req, resultOrd);
   if(bSuccess==false)
   {
      m_sMsg = StringFormat("[Place_Delete_Close_Partial Error-1][%s][%d](%s)", sAction, ordInfo.nTicket, __FormatResult(m_sMsg, resultOrd.retcode));
      return false;
   }
 
   if(resultOrd.retcode!=TRADE_RETCODE_DONE)
      m_sMsg = StringFormat("[Place_Delete_Close_Partial Error-2][%s][%d](%s)", sAction, ordInfo.nTicket,__FormatResult(m_sMsg, resultOrd.retcode));
   else
      debug(StringFormat("[Place_Delete_Close_Partial Ok][%s]ticket:%d, magic:%d", sAction, nTicket, ordInfo.nMagic));
                  
    return true;
}




//bool CPlaceMT5Order::CheckOrderResult(bool bOpen, int nSlaveTicket)
//{
//    string   sOpenClose;
//    bool    bReturn;
//    if(bOpen){
//        sOpenClose = "[OPEN ]";
//        bReturn = OrderSelect(nSlaveTicket, SELECT_BY_TICKET);
//    }
//    else{
//        sOpenClose = "[CLOSE]";
//        bReturn = OrderSelect(nSlaveTicket, SELECT_BY_TICKET, MODE_HISTORY);
//    }
//    if(!bReturn) 
//    {
//        int err = GetLastError();
//        m_sMsg = StringFormat("Fail to OrderSelect after Place Order[%d]%s",err, ErrorDescription(err));
//        return false;
//    }
//    
////    m_sMsg = StringFormat("%s[%s](MasterAcc:%d)(Masterticket:%d)(MasterOrdType:%d)(MasterLots:%f)(MasterPrc:%f)(MasterSlippage:%d)"
////                        "(Acc:%d)(SlaveTicket:%d)(OrdType:%d)(Lots:%f)(OpenTM:%d)(OpenPrc:%f)(CloseTM:%d)(ClosePrc:%f)(Prfit:%f)(Magic:%d)"
////                        , sOpenClose, sSymbol
////                        , nMasterAcc, nMasterTicket, nMasterOrdType, dMasterLots, dMasterPrc, nMasterSlippage
////                        , AccountNumber(), OrderTicket(), OrderType(), OrderLots(), OrderOpenTime(), OrderOpenPrice()
////                        , OrderCloseTime(), OrderClosePrice(), OrderProfit(),OrderMagicNumber()
////                        );
////   
////    printlog(m_sMsg);
//
//    return true;
//}








#endif //_ALPHA_PLACE_MT5_ORDER_H_