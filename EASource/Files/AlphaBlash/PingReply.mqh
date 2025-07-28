//+------------------------------------------------------------------+
//|                                                BPMasterLogin.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict

#ifndef __BPPING_REPLY__
#define __BPPING_REPLY__


#include "BPProtocol.mqh"
#include "BPSocketDll.mqh"
#include "CommonVar.mqh"

bool PingReply(CTcpClient& tcp)
{
   CProtoSet   set;
   set.Begin();
   
   set.SetVal(FDS_CODE,             CODE_PING);
   set.SetVal(FDS_COMMAND,          TP_COMMAND);
   set.SetVal(FDS_SYS,              "MT4");       
   set.SetVal(FDS_BROKER,           AccountInfoString(ACCOUNT_COMPANY));
   set.SetVal(FDS_USERID_MINE,      __USERID_MINE);
   set.SetVal(FDS_USERID_MASTER,    __USERID_MASTER);
   set.SetVal(FDS_ACCNO_MY,         __ACC_MINE);     
   set.SetVal(FDS_ACCNO_MASTER,     __ACC_MASTER);
   set.SetVal(FDS_TM_HEADER,        HeaderTime());
   
   set.SetVal(FDS_MASTERSLAVE_TP,   __MS_TP);
   
	string sLoginPacket;
	int nSend = set.Complete(sLoginPacket);
	char zBuffer[];
   ArrayResize(zBuffer, nSend);
   StringToCharArray(sLoginPacket, zBuffer);

   bool bResult = tcp.SendData(zBuffer, nSend);
	if (bResult == false)
	{
		return false;
	}
	
	//printlog(StringFormat( "[SEND Ping OK](%s)", sLoginPacket));
	return true;
}










#endif