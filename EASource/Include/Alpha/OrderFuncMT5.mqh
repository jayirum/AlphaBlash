#ifndef __ORDER_FUNC_MT5_H__
#define __ORDER_FUNC_MT5_H__


#include "_IncCommon.mqh"



bool __PlaceOrderMT5(_In_ MqlTradeRequest& req, _Out_ MqlTradeResult& result,  _In_ long nRetryCnt, _Out_ string& sMsg)
{
   MqlTradeCheckResult  checkRslt={};
   
   bool bRet = OrderCheck(req,checkRslt);
   if(!bRet)
   {
      sMsg = StringFormat("OrderCheck returns false.DO check the request structure or balance:%d", GetLastError());
      Print(sMsg);
      return false;
   }
   if( checkRslt.retcode!=0 )
   {
      sMsg = StringFormat("(%d)%s",checkRslt.retcode, checkRslt.comment);
      Print(sMsg);
      return false;
   }   
   
   bool bOk = false;
   for( int i=0; i<nRetryCnt; i++ )
   {
      if( (bOk=PlaceOrderInner(req, result, sMsg))==true ){
         PrintFormat("[ORDER OK](%s)(%d)(%.5f)", req.symbol, result.order, result.price);
         break;
      }
      
      PrintFormat("[E][TRY:%d]Order Fail:%s", i, sMsg );
      if(result.retcode==10031)  // Request rejected due to absence of network connection
         Sleep(500);
      else         
         Sleep(10);
   }
   return bOk;
}


bool PlaceOrderInner(_In_ MqlTradeRequest& req, _Out_ MqlTradeResult& result, _Out_ string& sMsg)
{
   bool bRet = OrderSend(req, result);   
   if(!bRet)
   {
      sMsg = StringFormat("Server rejected the Order Request.(%d)%s", result.retcode,result.comment);
      return false;
   }
   if( result.retcode!=10009 ) // Request completed
   {
      sMsg = StringFormat("(%d)%s", result.retcode,result.comment);
      return false;
   }
   else if (result.retcode==10004) // requote
   {
      req.price = __GetCurrPrcByOrderType(req.symbol, req.type) ;
   }
   return true;
}


bool __Fill_OrderType(string symbol, _Out_ ENUM_ORDER_TYPE_FILLING & fillType)
{
   
   bool bOk = false;
   int outputFillType = 0;
   
   
   int inputFillType = SYMBOL_FILLING_FOK;   
   bOk = IsFillingTypeAllowed(symbol, (ENUM_SYMBOL_INFO_INTEGER)inputFillType, outputFillType);
   if(bOk)
   {
      //PrintFormat("IsFillingTypeAllowed(): ORDER_FILLING_FOK");
      fillType = ORDER_FILLING_FOK;
      return true;
   }
   
   
   inputFillType = SYMBOL_FILLING_IOC;
   bOk  = IsFillingTypeAllowed(symbol, (ENUM_SYMBOL_INFO_INTEGER)inputFillType, outputFillType);
   if(bOk)
   {
      //PrintFormat("IsFillingTypeAllowed(): ORDER_FILLING_IOC");
      fillType = ORDER_FILLING_IOC;
      return true;
   }
   
   
   //PrintFormat("[E]IsFillingTypeAllowed error!. symbol(%s) input fill type(%d) output fill type(%d)", symbol,inputFilType,outputFillType  );
   //return false;
   
   
   //PrintFormat("IsFillingTypeAllowed(): ORDER_FILLING_RETURN");
   fillType = ORDER_FILLING_RETURN;
   return true;
}



bool IsFillingTypeAllowed(string symbol, int fill_type, _Out_ int fillTypeBySymbolInfo)
{
//--- Obtain the value of the property that describes allowed filling modes
   fillTypeBySymbolInfo = (int)SymbolInfoInteger(symbol, SYMBOL_FILLING_MODE);

   if( fillTypeBySymbolInfo == 0 )
   {
      //PrintFormat("SYMBOL_FILLING_MODE has No identifier");
      return false;
   }
   
   return ( (fillTypeBySymbolInfo & fill_type) == fill_type );
}

//
//string GetErrMsgOfTradeServer(int retcode)
//{
//   string msg = StringFormat("[%d]", retcode);
//   
//   switch(retcode)
//   {
//   case 10004: msg += "TRADE_RETCODE_REQUOTE";break;  
//   case 10006: msg += "Request rejected";break;
//   case 10007: msg += "Request canceled by trader";break;
//   case 10008: msg += "Order placed";break;
//   case 10009: msg += "Request completed";break;
//   case 10010: msg += "Only part of the request was completed";break;
//   case 10011: msg += "Request processing error";break; 
//   case 10012: msg += "Request canceled by timeout";break;
//   case 10013: msg += "Invalid request";break;
//   case 10014: msg += "Invalid volume in the request";break;
//   case 10015: msg += "Invalid price in the request";break;
//   case 10016: msg += "Invalid stops in the request";break;
//   case 10017: msg += "Trade is disabled";break;
//   case 10018: msg += "Market is closed";break;   
//   case 10019: msg += "There is not enough money to complete the request";break;
//   case 10020: msg += "Prices changed";break;
//   case 10021: msg += "There are no quotes to process the request";break;
//   case 10022: msg += "Invalid order expiration date in the request";break;
//   case 10023: msg += "Order state changed";break;
//   case 10024: msg += "Too frequent requests";break;
//   case 10025: msg += "No changes in request";break;
//   case 10026: msg += "Autotrading disabled by server";break;
//   case 10027: msg += "Autotrading disabled by client terminal";break;   
//   case 10028: msg += "Request locked for processing";break;  
//   case 10029: msg += "Order or position frozen";break;  
//   case 10030: msg += "Invalid order filling type";break;  
//   case 10031: msg += "No connection with the trade server";break;  
//   case 10032: msg += "Operation is allowed only for live accounts";break;  
//   case 10033: msg += "The number of pending orders has reached the limit";break;  
//   case 10034: msg += "The volume of orders and positions for the symbol has reached the limit";break;  
//   case 10035: msg += "Incorrect or prohibited order type";break;   
//   case 10036: msg += "Position with the specified POSITION_IDENTIFIER has already been closed";break;  
//   case 10038: msg += "A close volume exceeds the current position volume";break;  
//   case 10039: msg += "A close order already exists for a specified position";break;  
//   case 10040: msg += "The number of open positions simultaneously present on an account can be limited by the server settings";break;  
//   case 10041: msg += "The pending order activation request is rejected, the order is canceled";break;  
//   case 10042: msg += "The request is rejected, because the [Only long positions are allowed] rule is set for the symbol (POSITION_TYPE_BUY)";break;  
//   case 10043: msg += "The request is rejected, because the [Only short positions are allowed] rule is set for the symbol (POSITION_TYPE_SELL)";break;  
//   case 10044: msg += "The request is rejected, because the [Only position closing is allowed] rule is set for the symbol ";break;  
//   case 10045: msg += "The request is rejected, because [Position closing is allowed only by FIFO rule] flag is set for the trading account (ACCOUNT_FIFO_CLOSE=true)]";break; 
//   case 10046: msg += "The request is rejected, because the [Opposite positions on a single symbol are disabled] rule is set for the trading account";break; 
//   }
//      
//   return msg;
//}
//


#endif