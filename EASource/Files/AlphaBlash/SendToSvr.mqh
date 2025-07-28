
#ifndef __ALPHA_SENDTOSVR__
#define __ALPHA_SENDTOSVR__

#include "Common.mqh"
#include "_PlatformVer.mqh"
#include "Protocol.mqh"
#include "GlobalCommonVar.mqh"   


/*
@I_MT4_TICKET		varchar(10)
	,@I_MT4_OPEN_TM		varchar(20)
	,@I_MT4_TYPE		int
	,@I_MT4_SIZE		decimal(10,3)
	,@I_MT4_SYMBOL		varchar(20)
	,@I_MT4_OPEN_PRC	decimal(10,5)
	,@I_MT4_SL			decimal(10,5)
	,@I_MT4_TP			decimal(10,5)
	,@I_MT4_CLOSE_PRC	decimal(10,5)
	,@I_MT4_CMSN		decimal(10,2)
	,@I_MT4_SWAP		decimal(10,2)
	,@I_MT4_PROFIT		decimal(10,2)
	,@I_MT4_COMMENTS	varchar(50)
	,@I_ORD_STATUS		int
	,@I_LAST_ACTION_MT4_TM	VARCHAR(20)
	,@I_OPEN_GMT			VARCHAR(20)
	,@I_LAST_ACTION_GMT		VARCHAR(20)
	,@I_USER_ID			varchar(20)
	,@I_MT4_ACC			varchar(20)
	,@I_MC_TP			char(1)
	,@I_MASTER_TICKET	varchar(10)
	,@I_MASTER_ID		varchar(20)
	,@I_MASTER_MT4_ACC	varchar(20)
*/

//
//
//bool SendToSvr_SlaveOpenOrder(
//               CRelayEngine&  tcp
//               ,int         nSlaveTicket     // char Ticket         [10];
//               ,int         nMasterTicket
//               ,string      sSymbol     // char symbol         [10];
//               ,int         nType       // char Type           [1];				// OP_BUY, OP_SELL, OP_BUYLIMIT, OP_SELLLIMIT, OP_BUYSTOP, OP_SELLSTOP               
//               ,double      dOrdPrc     // char OrdPrc         [10];
//               ,double      dSLPrc      // char SLPrc          [10];
//               ,double      dPTPrc      // char PTPrc          [10];
//               ,double      dLots       // char Lots           [7];
//               ,datetime    dtOrdTm   // char OrdTm          [6];
//               ,double      dProfit     // char Profit         [9];
//               ,double      dSwap       // char Swap           [9];
//               ,string      sComments
//)
//{
//
//    CProtoSet   set;
//    set.Begin();
//    
//    set.SetVal(FDS_CODE,             CODE_COPIER_ORDER);
//    set.SetVal(FDS_TM_HEADER,        HeaderTime());
//    set.SetVal(FDS_COMMAND,          TP_ORDER);
//    set.SetVal(FDS_SYS,              "MT4");       
//    set.SetVal(FDS_BROKER,           AccountInfoString(ACCOUNT_COMPANY));
//    set.SetVal(FDS_USERID_MINE,      __USERID_MINE);
//    set.SetVal(FDS_USERID_MASTER,    __USERID_MASTER);
//    set.SetVal(FDS_ACCNO_MY,         __ACC_MINE);
//    set.SetVal(FDS_ACCNO_MASTER,     __ACC_MASTER);
//    set.SetVal(FDS_MASTERCOPIER_TP,  "S");
//    
//    set.SetVal(FDN_MASTER_TICKET,    nMasterTicket);    
//    set.SetVal(FDN_OPENED_TICKET,    nSlaveTicket);    
//    set.SetVal(FDS_SYMBOL,           sSymbol);    
//    set.SetVal(FDS_ORD_ACTION,       "O");    // OPEN
//    set.SetVal(FDN_ORD_TYPE,         nType);
//    
//    
//    set.SetVal(FDD_OPENED_PRC,   dOrdPrc);
//    set.SetVal(FDD_OPENED_LOTS,  dLots);
//    set.SetVal(FDS_OPENED_TM,    DatetimeToStr(dtOrdTm));
//    
//    set.SetVal(FDD_SLPRC,    dSLPrc);
//    set.SetVal(FDD_PTPRC,    dPTPrc);
//    set.SetVal(FDD_PROFIT,   dProfit);
//    set.SetVal(FDD_SWAP,     dSwap);
//    set.SetVal(FDS_COMMENTS, sComments);
//	
//    bool bResult = TcpSend(tcp, set);
//	if (bResult == false)
//	{
//	    //TODO m_sMsg ("[SendOrdToServer]Failed to Send Open Order to Server");
//	    return false;
//	}
//	return true;
//}
//
//
//
//bool SendToSvr_SlaveCloseOrder(
//               CRelayEngine&  tcp
//               ,int         nSlaveTicket     // char Ticket         [10];
//               ,int         nMasterTicket
//               ,string      sSymbol     // char symbol         [10];
//               ,int         nType       // char Type           [1];				// OP_BUY, OP_SELL, OP_BUYLIMIT, OP_SELLLIMIT, OP_BUYSTOP, OP_SELLSTOP               
//               ,double      dClosePrc
//               ,double      dCloseLots
//               ,datetime    dtCloseTm
//               ,double      dOpenedPrc
//               ,double      dOpenedLots
//               ,datetime    dtOpenedTm
//               ,double      dSLPrc      // char SLPrc          [10];
//               ,double      dPTPrc      // char PTPrc          [10];
//               ,double      dProfit     // char Profit         [9];
//               ,double      dSwap       // char Swap           [9];
//               ,string      sComments
//)
//{
//
//    CProtoSet   set;
//    set.Begin();
//    
//    set.SetVal(FDS_CODE,             CODE_COPIER_ORDER);
//    set.SetVal(FDS_TM_HEADER,        HeaderTime());
//    set.SetVal(FDS_COMMAND,          TP_ORDER);
//    set.SetVal(FDS_SYS,              "MT4");       
//    set.SetVal(FDS_BROKER,           AccountInfoString(ACCOUNT_COMPANY));
//    set.SetVal(FDS_USERID_MINE,      __USERID_MINE);
//    set.SetVal(FDS_USERID_MASTER,    __USERID_MASTER);
//    set.SetVal(FDS_ACCNO_MY,         __ACC_MINE);
//    set.SetVal(FDS_ACCNO_MASTER,     __ACC_MASTER);
//    set.SetVal(FDS_MASTERCOPIER_TP,  "S");
//    
//    set.SetVal(FDN_MASTER_TICKET,    nMasterTicket);    
//    set.SetVal(FDN_OPENED_TICKET,    nSlaveTicket);    
//    set.SetVal(FDS_SYMBOL,           sSymbol);    
//    set.SetVal(FDS_ORD_ACTION,       "C");    // CLOSE
//    set.SetVal(FDN_ORD_TYPE,         nType);
//    
//    set.SetVal(FDD_CLOSED_PRC,   dClosePrc);
//    set.SetVal(FDD_CLOSED_LOTS,  dCloseLots);
//    set.SetVal(FDS_CLOSED_TM,    DatetimeToStr(dtCloseTm));
//
//    set.SetVal(FDD_OPENED_PRC,   dOpenedPrc);
//    set.SetVal(FDD_OPENED_LOTS,  dOpenedLots);    
//    set.SetVal(FDS_OPENED_TM,    DatetimeToStr(dtOpenedTm));
//        
//    set.SetVal(FDD_SLPRC,    dSLPrc);
//    set.SetVal(FDD_PTPRC,    dPTPrc);
//    set.SetVal(FDD_PROFIT,   dProfit);
//    set.SetVal(FDD_SWAP,     dSwap);
//    set.SetVal(FDS_COMMENTS, sComments);
//	
//    bool bResult = TcpSend(tcp, set);
//	if (bResult == false)
//	{
//	    //TODO printlog("[SendOrdToServer]Failed to Send Close Order to Server");
//	    return false;
//	}
//	return true;
//}



bool TcpSend(CProtoSet& set)
{
   string sPacket;
   int nSend = set.Complete(sPacket, false);
   char zBuffer[];
   ArrayResize(zBuffer, (int)nSend);
   StringToCharArray(sPacket, zBuffer);
   
   return (__relayEngine.SendData(zBuffer, nSend)==E_OK);
}



int SendToRelay_MasterLogin()
{
    return SendToRelay_EALogin("M");
}


int SendToRelay_CopierLogin()
{
    return SendToRelay_EALogin("C");
}

int SendToRelay_EALogin(string sMCTp)
{
    string sLoginPacket;
    int nSend = SetPack_Login(sMCTp, sLoginPacket);
    char zBuffer[];
    ArrayResize(zBuffer, (int)nSend);
    StringToCharArray(sLoginPacket, zBuffer);
    
    return __relayEngine.SendData(zBuffer, nSend);
}


int SetPack_Login(string sMCTp, string& sPacket)
{    
    CProtoSet   set;
    set.Begin();
    
    set.SetVal(FDS_CODE,             CODE_LOGON);
    set.SetVal(FDS_TM_HEADER,        HeaderTime());
    set.SetVal(FDS_COMMAND,          TP_COMMAND);
    set.SetVal(FDS_SYS,              "MT4");       
    set.SetVal(FDS_BROKER,           AccountInfoString(ACCOUNT_COMPANY));
    set.SetVal(FDS_USERID_MINE,      __USERID_MINE);
    set.SetVal(FDS_USERID_MASTER,    __USERID_MASTER);
    set.SetVal(FDS_ACCNO_MY,         __ACC_MINE);
    set.SetVal(FDS_ACCNO_MASTER,     __ACC_MASTER);
    set.SetVal(FDS_USER_NICK_NM,     __NICK_NAME_MINE);  
    set.SetVal(FDS_MASTERCOPIER_TP,   sMCTp);
    
    int nSend = set.Complete(sPacket, false);
    return nSend;    
}

//
//
//bool SendToTr_MasterLogin()
//{
//    string sLoginPacket;
//    int nSend = SetPack_Login("M", sLoginPacket);
//    char zBuffer[];
//    ArrayResize(zBuffer, (int)nSend);
//    StringToCharArray(sLoginPacket, zBuffer);
//    
//    __trEngine.SendData(zBuffer, nSend);
//    
//    //printlog(StringFormat( "[MASTER SEND LOGIN OK](%s)", sLoginPacket));
//    return true;
//}
//
//
//bool SendToTr_CopierLogin()
//{
//    string sLoginPacket;
//    int nSend = SetPack_Login("C", sLoginPacket);
//    char zBuffer[];
//    ArrayResize(zBuffer, (int)nSend);
//    StringToCharArray(sLoginPacket, zBuffer);
//    
//    __trEngine.SendData(zBuffer, nSend);
//    
//    //printlog(StringFormat( "[MASTER SEND LOGIN OK](%s)", sLoginPacket));
//    return true;
//}


//void SendLog( string sMsg )
//{    
//    CProtoSet   set;
//    set.Begin();
//    
//    set.SetVal(FDS_CODE,            CODE_USER_LOG);
//    set.SetVal(FDS_TM_HEADER,       HeaderTime());
//    set.SetVal(FDS_COMMAND,         TP_COMMAND);
//    set.SetVal(FDS_SYS,             "MT4");       
//    set.SetVal(FDS_BROKER,          AccountInfoString(ACCOUNT_COMPANY));
//    set.SetVal(FDS_USERID_MINE,     __USERID_MINE);
//    set.SetVal(FDS_USERID_MASTER,   __USERID_MASTER);
//    set.SetVal(FDS_ACCNO_MINE,      __ACC_MINE);
//    set.SetVal(FDS_ACCNO_MASTER,    __ACC_MASTER);
//    
//    set.SetVal(FDS_MASTERCOPIER_TP,  __MC_TP);
//    set.SetVal(FDS_USER_LOG,        sMsg);
//    set.SetVal(FDS_LAST_ACTION_MT4_TM,  DatetimeToStr(TimeCurrent()));
//    set.SetVal(FDS_LAST_ACTION_GMT,     DatetimeToStr(TimeGMT()));
//   
//    
//    string sLoginPacket;
//    int nSend = set.Complete(sLoginPacket, false);
//
//    char zBuffer[];
//    ArrayResize(zBuffer, (int)nSend);
//    StringToCharArray(sLoginPacket, zBuffer);
//    
//    __trEngine.SendData(zBuffer, nSend);
//}






#endif // __BPSendOrdToServer_Master__