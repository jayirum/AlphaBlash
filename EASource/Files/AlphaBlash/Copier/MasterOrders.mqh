#ifndef __ALPHA_MASTER_ORD_H__
#define __ALPHA_MASTER_ORD_H__

/*
    Parsing the Master Order Information received
*/

/*
OP_BUY - buy order,
OP_SELL - sell order,
OP_BUYLIMIT - buy limit pending order,
OP_BUYSTOP - buy stop pending order,
OP_SELLLIMIT - sell limit pending order,
OP_SELLSTOP - sell stop pending order.

sAction           // O:Open, C:Close, M:Modify
*/

#include "../Protocol.mqh"
#include "../common.mqh"
#include "../Utils.mqh"

class CMasterOrders : public CProtoGet
{
public:
   CMasterOrders();
   ~CMasterOrders();
   
   void  Set_MasterId(string sMasterId, string sMyId){   m_sMasterId_EA = sMasterId; m_sMyId_EA=sMyId; }
   bool  Parse_MasterOrd(char& RecvPack[], int nRecvLen);
   
   bool  Is_OpenOrder();
   bool  Is_ChageOrder();
   bool  Is_DeleteOrder();
   bool  Is_PartialOrder();
   bool  Is_CloseFullOrder();
   
   void  Set_ToCloseFull();
   void  Set_ToDelete();
protected:
   bool  ParsingPacket(char& RecvPack[], int nRecvLen);
   void  ResetData();

public:
   int      m_nMasterTicket;
   string   m_sOpenTm, m_sOpenGmt;
   string   OpenGmt() { return m_sOpenGmt;}
   int      Ticket() { return m_nMasterTicket;}

   int      m_nOrdType;
   int      OrdType() { return m_nOrdType;}
   
   string   m_sOrdSide;
   string   OrdSide() { return m_sOrdSide;}

   double	m_dOpenLots, m_dCloseLots;
   double   OpenLots()  { return m_dOpenLots;}
   double   CloseLots() { return m_dCloseLots;}

   string   m_sSymbol;
   string   OrdSymbol() { return m_sSymbol;}

   double   m_dOpenPrc, m_dClosePrc;
   double   OpenPrc() { return m_dOpenPrc;}
   double   ClosePrc() { return m_dClosePrc;}

   double	m_dSL;
   double	m_dTP;
   string   m_sCloseTm;
   double StopLoss() { return m_dSL;}
   double TakeProfit() { return m_dTP;}

   double  m_dCmsn;
   double  m_dSwap;
   double  m_dProfit;
   string  m_sComment;
   string  Comments() { return m_sComment; }

   string   m_sExpiry;
   double   m_dMasterEqty;
   double   MasterEqty()   { return m_dMasterEqty; }
   datetime  Expiry()       { return __StrToTime(m_sExpiry); }
    
   int      m_nOrdAction;
   int      OrdAction()    { return m_nOrdAction;}

   char     m_zOrdActionChg[CHG_ACTION_SIZE+1];
   string   OrdChgAction() { return CharArrayToString(m_zOrdActionChg); }

   bool  IsChg_OpenPrc()   { return (m_zOrdActionChg[IDX_CHG_OPEN_PRC]=='1'); }
   bool  IsChg_SL()        { return (m_zOrdActionChg[IDX_CHG_SL]=='1'); }
   bool  IsChg_TP()        { return (m_zOrdActionChg[IDX_CHG_TP]=='1'); }
   bool  IsChg_Expiry()    { return (m_zOrdActionChg[IDX_CHG_EXPIRY]=='1'); }    

   int      m_nMasterGroupKey;  //
   string   m_sKeepOrgTicketYN;

   int      LinkNo() { return m_nMasterGroupKey; }
   
   string   m_sOrdAct_SubPartialYN;
   bool     Is_SubPartialNew() { return (m_sOrdAct_SubPartialYN=="Y"); }
   
   string   m_sPlatform;
   string   Platform() { return m_sPlatform;}
   
   string   m_sOrdPosTp;   //O, P
   string   OrdPosTp() { return m_sOrdPosTp;}
};

CMasterOrders::CMasterOrders()
{

}

CMasterOrders::~CMasterOrders()
{
}



void  CMasterOrders::ResetData()
{
   m_nMasterTicket   = 0;
   m_sOpenTm         = "";
   m_sOpenGmt        = "";
   m_nOrdType        = 0;
   m_dOpenLots       = 0;
   m_sSymbol         = "";
   m_dOpenPrc        = 0;
   m_dClosePrc       = 0;
   m_dSL             = 0;
   m_dTP             = 0;
   m_sCloseTm        = "";
   m_dCmsn           = 0;
   m_dSwap           = 0;
   m_dProfit         = 0;
   m_sComment        = "";
   m_sExpiry         = "";
   m_nOrdAction      = 0;
   m_nMasterGroupKey = 0;
   m_sKeepOrgTicketYN = "";
   m_sOrdAct_SubPartialYN = "";
}

bool CMasterOrders::Is_OpenOrder()
{
   return (m_nOrdAction==ORD_ACTION_OPEN);
}

bool CMasterOrders::Is_ChageOrder()
{
   return (m_nOrdAction==ORD_ACTION_CHANGE);
}

bool CMasterOrders::Is_DeleteOrder()
{
   return (m_nOrdAction==ORD_ACTION_DELETE);
}

void CMasterOrders::Set_ToCloseFull()
{
   m_nOrdAction = ORD_ACTION_CLOSE_FULL;
}

void CMasterOrders::Set_ToDelete()
{
   m_nOrdAction = ORD_ACTION_DELETE;
}

bool CMasterOrders::Is_PartialOrder()
{
   return (m_nOrdAction==ORD_ACTION_CLOSE_PARTIAL);
}

bool CMasterOrders::Is_CloseFullOrder()
{
   return (m_nOrdAction==ORD_ACTION_CLOSE_FULL);
}
   
bool CMasterOrders::Parse_MasterOrd(char& RecvPack[], int nRecvLen)
{
   ResetData();
   return ParsingPacket(RecvPack, nRecvLen);
}

bool CMasterOrders::ParsingPacket(char& RecvPack[], int nRecvLen)
{
    /*
        1=1004/2=C/ array
    */
    string result[];
    
    int nCnt = CProtoGet::SplitPacket(RecvPack, nRecvLen, result);
    //printlog(StringFormat("ParsingPacket:%d", nCnt));   
    
    int nField=0;
    string unit[];
    string val;
    ushort equal = StringGetCharacter("=",0);
    for(int i=0; i<nCnt; i++)
    {
        int kCnt = StringSplit(result[i], equal, unit);
        
        // this value is zero(0) when the loop count is the last
        //if( (i!=nCnt-1) && (kCnt!=2))
        if( kCnt!=2 )
        {
            if( i == (nCnt-1) )
            {    
                m_sMsg = "";    //StringFormat("[i:%d, k:%d]this is not error", i, kCnt);
            }
            else{
                m_sMsg = StringFormat("[i:%d, k:%d]Packet Unit error(%s)", i, kCnt, result[i]);
            }
            continue;
        }
        nField =  (int)StringToInteger(unit[0]);
        
        //printlog(StringFormat("[%d][Code:%d]nField",i, nField));
        
        switch(nField)
        {
        case FDS_SYS:
            m_sPlatform = unit[1];
            break;
        case FDS_USERID_MASTER:
            m_header.sMasterId = unit[1];
            if( !CProtoGet::IsMyMaster() )
            {
                m_sMsg = StringFormat("[PACK MASTER:%s][MY MASTER:%s]",Packet_MasterId(), m_sMasterId_EA);
                //TODO __AlphaAssert("!IsMyMaster()",__FILE__, __LINE__);
                return false;
            }
            break;
        case FDS_ACCNO_MASTER://     101:
            m_header.nMasterAcc = (int)StringToInteger(unit[1]);
            //debug(StringFormat("FDS_ACCNO_MASTER(%d)", m_header.nMasterAcc));
            break;
        case FDS_MT4_TICKET:
            m_nMasterTicket = (int)StringToInteger(unit[1]);
            //debug(StringFormat("FDS_MT4_TICKET(%d)", m_nMasterTicket));
            break;
        case FDS_OPEN_TM:
            m_sOpenTm = unit[1];
            //debug(StringFormat("FDS_OPEN_TM(%s)", m_sOpenTm));
            break;
        case FDS_OPEN_GMT:
            m_sOpenGmt = unit[1];
            //debug(StringFormat("FDS_OPEN_GMT(%s)", m_sOpenGmt));
            break;
        case FDN_ORD_TYPE:
            m_nOrdType = (int)StringToInteger(unit[1]);
            //debug(StringFormat("FDN_ORD_TYPE(%d)", m_nOrdType));
            break;
        case FDD_LOTS:
            m_dOpenLots = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_LOTS(%f)", m_dOpenLots));
            break;
        case FDD_CLOSE_LOTS:
            m_dCloseLots = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_CLOSE_LOTS(%f)", m_dCloseLots));
            break;
        case FDS_SYMBOL: 
            m_sSymbol = unit[1];
            //debug(StringFormat("FDS_OPEN_TM(%s)", m_sOpenTm));
            break;
        case FDD_OPEN_PRC:
            m_dOpenPrc = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_OPEN_PRC(%f)", m_dOpenPrc));
            break;
        case FDD_CLOSE_PRC:
            m_dClosePrc = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_CLOSE_PRC(%f)", m_dClosePrc));
            break;
        case FDD_SLPRC:
            m_dSL = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_SLPRC(%f)", m_dSL));
            break;
        case FDD_TPPRC:
            m_dTP = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_TPPRC(%f)", m_dTP));
            break;
        case FDS_CLOSE_TM:
            m_sCloseTm = unit[1];
            //debug(StringFormat("FDS_CLOSE_TM(%s)", m_sCloseTm));
            break;            
        case FDD_CMSN:
            m_dCmsn = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_CMSN(%f)", m_dCmsn));
            break;
        case FDD_SWAP:
            m_dSwap = (int)StringToInteger(unit[1]);
            //debug(StringFormat("FDD_SWAP(%f)", m_dSwap));
            break;
        case FDD_PROFIT:
            m_dProfit = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_PROFIT(%f)", m_dProfit));
            break;
        case FDS_COMMENTS:
            m_sComment = unit[1];
            //debug(StringFormat("FDS_COMMENTS(%s)", m_sComment));
            break;
        case FDS_EXPIRY:
            m_sExpiry = unit[1];
            //debug(StringFormat("FDS_EXPIRY(%s)", m_sExpiry));
            break;
        case FDN_ORD_ACTION:
            m_nOrdAction = (int)StringToInteger(unit[1]);
            //debug(StringFormat("FDN_ORD_ACTION(%d)", m_nOrdAction));
            break;
        case FDD_EQUITY:
            m_dMasterEqty = StringToDouble(unit[1]);
            //debug(StringFormat("FDD_MASTER_EQUITY(%f)", m_dMasterEqty));
            break;
        case FDS_ORD_ACTION_CHG:
            val = unit[1];
            StringToCharArray(val, m_zOrdActionChg);
            //debug(StringFormat("FDS_ORD_ACTION_CHG[%c][%c][%c]", val[0],val[1],val[2]));
            break;
        case FDS_ORDER_GROUPKEY:
            m_nMasterGroupKey = (int)StringToInteger(unit[1]);
            break;
        case FDS_ORD_ACTION_SUB_PARTIAL_YN:
            val = unit[1];
            m_sOrdAct_SubPartialYN = val;
            break;
        case FDS_ORD_POS_TP:
            m_sOrdPosTp = unit[1];
            break;
        case FDS_ORD_SIDE:
            m_sOrdSide = unit[1];
            break;
        
        }//switch(nField)
    }//for(int i=0; i<nCnt; i++)

   return true;
}




//int CMasterOrders::OrdType()
//{ 
//   return m_nOrdType;
//}
//
//bool CMasterOrders::IsMarketOrdType()
//{
//   if( OrdType()==OP_BUY || OrdType()==OP_SELL )
//      return true;
//      
//   return false;
//}
//
//bool CMasterOrders::IsBuyOrder()
//{
//   if( OrdType()==OP_BUY || OrdType()==OP_BUYLIMIT || OrdType()==OP_BUYSTOP )
//      return true;
//   return false;
//}
//
//bool CMasterOrders::IsSellOrder()
//{
//   return (!IsBuyOrder());
//}
//
//
//double  CMasterOrders::OrdLots()
//{
//    if(m_sOrdAction=="O")
//        return m_dOpenLots;
//    else
//        return m_dCloseLots;
//}
//
//
//double  CMasterOrders::OrdPrc()
//{
//    if(m_sOrdAction=="O")
//        return m_dOpenPrc;
//    else
//        return m_dClosePrc;
//}
//


#endif // CMASTER_ORD_H