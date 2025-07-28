#ifndef __TEST_MQ5_ORD_POS_H__
#define __TEST_MQ5_ORD_POS_H__


string DatetimeToStr(datetime tm)
{
   return TimeToString(tm,TIME_DATE|TIME_SECONDS);
}

void LookTrade()
{
   int nTot = PositionsTotal();
   for(int i=0; i<nTot; i++)
   {
      int ticket = PositionGetTicket(i);
      
      Alert(
         StringFormat("[POS](lot:%f)(open:%f)(close:%f)(sl:%f)(tp:%f)",
                        PositionGetDouble(POSITION_VOLUME),
                        PositionGetDouble(POSITION_PRICE_OPEN),
                        PositionGetDouble(POSITION_PRICE_CURRENT),
                        PositionGetDouble(POSITION_SL),
                        PositionGetDouble(POSITION_TP)
         )
      );
   }
   
   nTot = OrdersTotal();
   for(int i=0; i<nTot; i++)
   {
      int ticket = OrderGetTicket(i);
      
      Alert(
         StringFormat("[ORD](lotInit:%f)(lotCurr:%f)(open:%f)(close:%f)(sl:%f)(tp:%f)",
                        OrderGetDouble(ORDER_VOLUME_INITIAL),
                        OrderGetDouble(ORDER_VOLUME_CURRENT),
                        OrderGetDouble(ORDER_PRICE_OPEN),
                        OrderGetDouble(ORDER_PRICE_CURRENT),
                        OrderGetDouble(ORDER_SL),
                        OrderGetDouble(ORDER_TP)
         )
      );
   }
}

void PositionID()
{
   HistorySelect(0, TimeCurrent());
   int nTotal = HistoryOrdersTotal();
   
   
   for (int i=0; i<nTotal; i++)
   {
      int nTicket = HistoryOrderGetTicket(i);
      PrintFormat("[OrdTicket:%d][PosID:%d]", nTicket, HistoryOrderGetInteger(nTicket, ORDER_POSITION_ID));
   }
}


void SelSamePosId(int nOrdTicket)
{
   HistoryOrderSelect(nOrdTicket);
   int nPosID = HistoryOrderGetInteger(nOrdTicket, ORDER_POSITION_ID);
   
   HistorySelectByPosition(nPosID);
   int nTotal = HistoryOrdersTotal();
   
   for (int i=nTotal-1; i>=0; i--)
   {
      int nRelatedOrdTicket = HistoryOrderGetTicket(i);
      int nDealTicket = HistoryDealGetTicket(i);
      Alert(StringFormat("[PosID:%d][OrdTicket:%d][DealTicket:%d][DealCmsn:%f]",nPosID, nRelatedOrdTicket, nDealTicket, HistoryDealGetDouble(nDealTicket, DEAL_COMMISSION)));
   }
}

void ClosedPosInfo()
{
   HistorySelect(0, TimeCurrent());
   int nTotal = HistoryOrdersTotal();
   
   
   for (int i=0; i<nTotal; i++)
   {
      int nTicket = HistoryOrderGetTicket(i);
      Alert(StringFormat("Ticket:%d, PosID:%d, OpenPrc:%f, CurrPrc:%f, InitLots:%f, CurrLots:%f, TIME_SETUP:%s, TIME_DONE:%s, State:%d", 
                  nTicket,
                  HistoryOrderGetInteger(nTicket, ORDER_POSITION_ID),
                  HistoryOrderGetDouble(nTicket, ORDER_PRICE_OPEN),
                  HistoryOrderGetDouble(nTicket, ORDER_PRICE_CURRENT),
                  HistoryOrderGetDouble(nTicket, ORDER_VOLUME_INITIAL),
                  HistoryOrderGetDouble(nTicket, ORDER_VOLUME_CURRENT),
                  DatetimeToStr(HistoryOrderGetInteger(nTicket, ORDER_TIME_SETUP)),
                  DatetimeToStr(HistoryOrderGetInteger(nTicket, ORDER_TIME_DONE)),
                  HistoryOrderGetInteger(nTicket, ORDER_STATE)
                  ));
   }

}

//   int no = 245477237;
//   HistorySelectByPosition(no);
//   int cnt = HistoryOrdersTotal();
//   
//   for (int i=0; i<cnt; i++)
//   {
//      int ordTicket = HistoryOrderGetTicket(i);
//      Alert(StringFormat("ord ticket:%d, symbol:%s, pos id:%d, open:%f, close:%f, lots:%f, lots:%f, state:%d, time:%d", 
//                  HistoryOrderGetInteger(ordTicket, ORDER_TICKET),
//                  HistoryOrderGetString(ordTicket, ORDER_SYMBOL),
//                  HistoryOrderGetInteger(ordTicket, ORDER_POSITION_ID),
//                  HistoryOrderGetDouble(ordTicket, ORDER_PRICE_OPEN),
//                  HistoryOrderGetDouble(ordTicket, ORDER_PRICE_CURRENT),
//                  HistoryOrderGetDouble(ordTicket, ORDER_VOLUME_INITIAL),
//                  HistoryOrderGetDouble(ordTicket, ORDER_VOLUME_CURRENT),
//                  HistoryOrderGetInteger(ordTicket, ORDER_STATE),
//                  HistoryOrderGetInteger(ordTicket, ORDER_TIME_DONE_MSC)
//                  ));
//   }
//
//   int ordTicket = 245477237;
//   HistoryOrderSelect(ordTicket);
//   Alert(StringFormat("ord_ticket:%d, symbol:%s, posid:%d, open:%f, close:%f, init lots:%f, curr lots:%f, state:%d, time:%d", 
//                  HistoryOrderGetInteger(ordTicket, ORDER_TICKET),
//                  HistoryOrderGetString(ordTicket, ORDER_SYMBOL),
//                  HistoryOrderGetInteger(ordTicket, ORDER_POSITION_ID),
//                  HistoryOrderGetDouble(ordTicket, ORDER_PRICE_OPEN),
//                  HistoryOrderGetDouble(ordTicket, ORDER_PRICE_CURRENT),
//                  HistoryOrderGetDouble(ordTicket, ORDER_VOLUME_INITIAL),
//                  HistoryOrderGetDouble(ordTicket, ORDER_VOLUME_CURRENT),
//                  HistoryOrderGetInteger(ordTicket, ORDER_STATE),
//                  HistoryOrderGetInteger(ordTicket, ORDER_TIME_DONE_MSC)
//                  ));
//                  
//   ordTicket = 245477265;
//   HistoryOrderSelect(ordTicket);
//   Alert(StringFormat("ord_ticket:%d, symbol:%s, posid:%d, open:%f, close:%f, init lots:%f, curr lots:%f, state:%d, time:%d", 
//                  HistoryOrderGetInteger(ordTicket, ORDER_TICKET),
//                  HistoryOrderGetString(ordTicket, ORDER_SYMBOL),
//                  HistoryOrderGetInteger(ordTicket, ORDER_POSITION_ID),
//                  HistoryOrderGetDouble(ordTicket, ORDER_PRICE_OPEN),
//                  HistoryOrderGetDouble(ordTicket, ORDER_PRICE_CURRENT),
//                  HistoryOrderGetDouble(ordTicket, ORDER_VOLUME_INITIAL),
//                  HistoryOrderGetDouble(ordTicket, ORDER_VOLUME_CURRENT),
//                  HistoryOrderGetInteger(ordTicket, ORDER_STATE),
//                  HistoryOrderGetInteger(ordTicket, ORDER_TIME_DONE_MSC)
//                  ));                  
////   
//   //HistoryDealSelect(242071793);
//   //Alert(StringFormat("ticket:%d, symbol:%s, type:%d, open:%f, close:%f", 
//   //               HistoryDealGetInteger(242071793, DEAL_ORDER),
//   //               HistoryDealGetString(242071793, DEAL_SYMBOL),
//   //               HistoryDealGetInteger(242071793, DEAL_TYPE),
//   //               HistoryDealGetDouble(242071793, DEAL_PRICE),
//   //               HistoryDealGetDouble(242071793, DEAL_PRICE)    
//   //               ));        
//   return(INIT_SUCCEEDED);
//  }


#endif