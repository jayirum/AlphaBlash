#ifndef _ALPHA_SEND_ORDER_H
#define _ALPHA_SEND_ORDER_H

///#include "BPSlaveConfigUI.mqh"
#ifndef __MQL5__
   #include <stderror.mqh>
   #include <stdlib.mqh>
#endif

#include "../Utils.mqh"

/*
OP_BUY - buy order,
OP_SELL - sell order,
OP_BUYLIMIT - buy limit pending order,
OP_BUYSTOP - buy stop pending order,
OP_SELLLIMIT - sell limit pending order,
OP_SELLSTOP - sell stop pending order.

int  OrderSend(
   string   symbol,              // symbol
   int      cmd,                 // operation
   double   volume,              // volume
   double   price,               // price
   int      slippage,            // slippage
   double   stoploss,            // stop loss
   double   takeprofit,          // take profit
   string   comment=NULL,        // comment
   int      magic=0,             // magic number
   datetime expiration=0,        // pending order expiration
   color    arrow_color=clrNONE  // color
   
*/
class CPlaceMT4Order
{
public:
    CPlaceMT4Order(){};
    ~CPlaceMT4Order(){};
    
    int     PlaceOpenOrder(string sSymbol, int nMasterOrdType, double dOrdLots, double dOrdPrc, int nMasterTicket);
    bool    PlaceCloseOrder(int nOpenedTicket, double dOrdLots, double dOrdPrc);
    bool    CheckOrderResult(bool bOpen, int nSlaveTicket);
    
    string  GetMsg(){return m_sMsg;}
    
    
    //bool    LinkFilter(CConfigUI* config);

private:
    //CConfigUI*    m_pConfig;
    string        m_sMsg;   
};

//bool CPlaceMT4Order::LinkFilter(CConfigUI* config)
//{
//    if(CheckPointer(config)==POINTER_INVALID){
//        printlog("CConfigUI is not valid pointer",true);
//        return false;
//    }
//    
//    m_pConfig = config;
//    return true;
//}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Place Open Order via MT4 API
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CPlaceMT4Order::PlaceOpenOrder(string sSymbol, int nMasterOrdType, double dOrdLots, double dOrdPrc, int nMasterTicket)
{
    int nTicket = OrderSend(
                       sSymbol,     // symbol
                       nMasterOrdType,    // operation
                       dOrdLots,      // volume
                       dOrdPrc,     // price
                       0,   // slippage
                       0,           // stop loss
                       0,           // take profit
                       NULL,        // comment
                       nMasterTicket,   //nMasterAcc,  // magic number
                       0,           // pending order expiration
                       0         // color
                    );
    
    if(nTicket==-1)
    {
        //printlog(StringFormat("[OrderSend error](symbol:%s)(OrdType:%d)(lots:%f)(Prc:%f)(Magic:%d)",
        //                sSymbol, nMasterOrdType, dOrdLots, dOrdPrc, nMasterTicket));
        //int nErr = GetLastError();
        //m_sMsg = StringFormat("[%d](%s)", nErr, ErrorDescription(nErr));
        return -1;
    }
    
    //bool bResult = true;
    //for(int i=0; i<5; i++)
    //{
    //    bool bOpened = true;
    //    bResult = CheckOrderResult(bOpened, nTicket);
    //    if(bResult) break;
    //    else{
    //        nTicket = -1;
    //        Sleep(10);
    //        continue;
    //    }
    //}
    return nTicket;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Place Close Order via MT4 API
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CPlaceMT4Order::PlaceCloseOrder(int nOpenedTicket, double dOrdLots, double dOrdPrc)
{
    bool bReturn = OrderClose(
                       nOpenedTicket    //int        ticket,      // ticket
                       ,dOrdLots        //double     lots,        // volume
                       ,dOrdPrc         //double     price,       // close price
                       ,0               //int        slippage,    // slippage
                       ,Red             // color      arrow_color  // color
    );
    
    if(!bReturn)
    {
        //int nErr = GetLastError();
        //m_sMsg = StringFormat("[%d]%s", nErr, ErrorDescription(nErr));
        //printlog(m_sMsg, true);
        return false;
    }

    //bool bOpen = false;
    //return CheckOrderResult(bOpen, nOpenedTicket);
    return true;
}


bool CPlaceMT4Order::CheckOrderResult(bool bOpen, int nSlaveTicket)
{
    string   sOpenClose;
    bool    bReturn;
    if(bOpen){
        sOpenClose = "[OPEN ]";
        bReturn = OrderSelect(nSlaveTicket, SELECT_BY_TICKET);
    }
    else{
        sOpenClose = "[CLOSE]";
        bReturn = OrderSelect(nSlaveTicket, SELECT_BY_TICKET, MODE_HISTORY);
    }
    if(!bReturn) 
    {
        int err = GetLastError();
        m_sMsg = StringFormat("Fail to OrderSelect after Place Order[%d]%s",err, ErrorDescription(err));
        return false;
    }
    
//    m_sMsg = StringFormat("%s[%s](MasterAcc:%d)(Masterticket:%d)(MasterOrdType:%d)(MasterLots:%f)(MasterPrc:%f)(MasterSlippage:%d)"
//                        "(Acc:%d)(SlaveTicket:%d)(OrdType:%d)(Lots:%f)(OpenTM:%d)(OpenPrc:%f)(CloseTM:%d)(ClosePrc:%f)(Prfit:%f)(Magic:%d)"
//                        , sOpenClose, sSymbol
//                        , nMasterAcc, nMasterTicket, nMasterOrdType, dMasterLots, dMasterPrc, nMasterSlippage
//                        , AccountNumber(), OrderTicket(), OrderType(), OrderLots(), OrderOpenTime(), OrderOpenPrice()
//                        , OrderCloseTime(), OrderClosePrice(), OrderProfit(),OrderMagicNumber()
//                        );
//   
//    printlog(m_sMsg);

    return true;
}








#endif //_SEND_ORDER_H