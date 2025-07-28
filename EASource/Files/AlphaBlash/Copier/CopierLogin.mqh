//+------------------------------------------------------------------+
//|                                                BPMasterLogin.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict

#ifndef __BPSLAVE_LOGIN___
#define __BPSLAVE_LOGIN___


#include "../BPProtocol.mqh"
#include "../BPSocketDll.mqh"

bool SlaveLoginSend(string sMyId, string sMasterId, string sMasterAcntNo, CTcpClient& tcp)
{
   CProtoSet   set;
   set.Begin();
   
   set.SetVal(FDS_CODE,             CODE_LOGON);
   set.SetVal(FDS_COMMAND,          TP_COMMAND);
   set.SetVal(FDS_SYS,              "MT4");       
   set.SetVal(FDS_BROKER,           AccountInfoString(ACCOUNT_COMPANY));
   set.SetVal(FDS_USERID_MINE,      sMyId);
   set.SetVal(FDS_USERID_MASTER,    sMasterId);
   set.SetVal(FDN_ACCNO_MY,         AccountNumber());     
   set.SetVal(FDN_ACCNO_MASTER,     sMasterAcntNo);
   set.SetVal(FDS_TM_HEADER,        HeaderTime());
   
   set.SetVal(FDS_MASTERSLAVE_TP,   "S");
   
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
	
	printlog(StringFormat( "[Slave SEND LOGIN OK](%s)", sLoginPacket));
	return true;
}










#endif