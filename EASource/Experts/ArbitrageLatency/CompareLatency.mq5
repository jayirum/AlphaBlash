//+------------------------------------------------------------------+
//|                                               CompareLatency.mq5 |
//+------------------------------------------------------------------+
#property copyright "Copyright 2022, Irumnet Pte Ltd."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict

#include "../../include/Alpha/_IncCommon.mqh"
#include "../../include/Alpha/IniFile.mqh";
#include "../../include/Alpha/CClientSocket.mqh"
#include "../../include/Alpha/Protocol.mqh"
#include "../../include/Alpha/UtilDateTime.mqh"



ClientSocket   *_sockSend;

string   _IniFileName;
string   _sMsg;
char     _zMsg[BUF_LEN];
char     _zRecvBuff[BUF_LEN];

int      _TIMEOUT_MDFETCH=0;
int      _TIMEOUT_RECONN=0;

int      _timerTp = 0;
enum     { TIMER_STEP_1=1, TIMER_STEP_2};


struct TMD
{
   string   sSymbol;
   double   bid, ask;
   long     spread;
   datetime time;  
};

TMD   _arrMD[];
string _brokerName = StringFormat("%.10s", __GetBrokerName());


//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{

   __ShowAllSymbols();
  
  if( __CheckEAInitCondition(_sMsg)==false)
   {
      Alert(_sMsg);
      Sleep(5000);
      return 0;
   }
   
    //+----------------------------------------------------------------------------------------
   //  read config file 
   //+----------------------------------------------------------------------------------------

   __Ini_GetIniFileFullName(_IniFileName);
   Print("IniFile : "+_IniFileName);
   
   if( !ReadSymbols_from_IniFile(_IniFileName, _sMsg, _arrMD))
   {
      Print(_sMsg);
      return -1;
   }
   

   string s;
   if(__Ini_GetVal(_IniFileName, "TIMEOUT_MS", "MD_FETCH", s)==false)
      return -1;
   _TIMEOUT_MDFETCH = (int)StringToInteger(s);
   
   if(__Ini_GetVal(_IniFileName, "TIMEOUT_MS", "RECONNECT", s)==false)
      return -1;
   _TIMEOUT_RECONN = (int)StringToInteger(s);
   
   PrintFormat("Timeout for MD Fetch(%d), Timeout for Reconnect(%d)", _TIMEOUT_MDFETCH, _TIMEOUT_RECONN);
   
   //+----------------------------------------------------------------------------------------
   //  Initialize socket library
   //+----------------------------------------------------------------------------------------
   if( InitSock()==false )
      return -1;
      
      
   _timerTp =  TIMER_STEP_1;
   
   __RunEA_Start();
   
   
//--- create timer
   EventSetMillisecondTimer(_TIMEOUT_MDFETCH);
   
//---
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
//--- destroy timer
   EventKillTimer();

   delete _sockSend;
   __RunEA_Stop();   
}
  
    
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
{  
   if(!__RunEA_IsRunnable()) return;
   
   if(IsStopped() || !__IsExpertEnabled()) return;
   
   if( _timerTp==TIMER_STEP_1 )
   {
      if( !ConnectSvr() )
      {
         // retry connect
         Sleep(_TIMEOUT_RECONN);
      }
      else
      {
         _timerTp = TIMER_STEP_2;
      }
      return;
   }
   
   if (_timerTp==TIMER_STEP_2 )
   {
      SendMarketData();
   }

}

  

bool InitSock()
{
   _sockSend = new ClientSocket;
   int nResult = 0;
   string sIP, sPort, sRecvTimeout, sSendTimeout, s;
   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "IP", sIP)) nResult++;
   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "PORT", sPort)) nResult++;
   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "SENDTIMEOUT", sSendTimeout)) nResult++;
   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "RECVTIMEOUT", sRecvTimeout)) nResult++;
   
   if(nResult<4){
      showErr("Wrong ini file(%s) info(IP, Port,RETRY_CONN_TIMEOUT)");
      return false;
   }
   
   PrintFormat("Server IP and Port(%s)(%s)", sIP, sPort);
   
   //_sockRecv.Initialize(sIP, (int)StringToInteger(sPort), (int)StringToInteger(sSendTimeout), (int)StringToInteger(sRecvTimeout));
   _sockSend.Initialize(sIP, (ushort)StringToInteger(sPort), (int)StringToInteger(sSendTimeout), (int)StringToInteger(sRecvTimeout));
   
   
   return true;
}
  

void SendMarketData()
{  
   for (int i=0; i<ArraySize(_arrMD); i++ )
   {  
      datetime nNewTime = (datetime) SymbolInfoInteger(_arrMD[i].sSymbol, SYMBOL_TIME);
      if( _arrMD[i].time == nNewTime )
         continue;
      
      _arrMD[i].bid = __GetBid(_arrMD[i].sSymbol);
      _arrMD[i].ask = __GetAsk(_arrMD[i].sSymbol);        
      _arrMD[i].spread = __GetSpreadPts(_arrMD[i].sSymbol);
      _arrMD[i].time = nNewTime;
   
      CProtoSet* set = new CProtoSet;
      set.Begin();
      set.SetVal(FDS_CODE,       CODE_MARKET_DATA);
      set.SetVal(FDS_KEY,        _brokerName);
      set.SetVal(FDS_SYMBOL,     _arrMD[i].sSymbol);       
      set.SetVal(FDD_BID,        _arrMD[i].bid);
      set.SetVal(FDD_ASK,        _arrMD[i].ask);
      set.SetVal(FDD_SPREAD,     _arrMD[i].spread);
      set.SetVal(FDS_MARKETDATA_TIME,       __TimeToStr_yyyymmddhhmmss(nNewTime));
      
      string sendBuf;
      int len = set.Complete(sendBuf, true);
      Send2Svr(_sockSend, sendBuf, len, "MD_CheckNSend", false);   
      
      //PrintFormat("[MD_SEND-%d](%s)", i, sendBuf);
      delete set;
   }

}



bool Send2Svr(_In_ ClientSocket& sock,  _In_ string& sendBuf, int nSendLen, string sCaller, bool bLog=true)
{  
   if( ERR_OK != sock.Send(sendBuf, nSendLen) )
   {
      _sMsg = StringFormat("[%s]%s", sCaller, sock.GetMsg());
      //showErr( sMsg );
      return false;
   }
   
   return true;
}


bool ConnectSvr()
{   
   int nRet = _sockSend.ConnectSvr();
   if(nRet!=E_OK)
   {
      showErr(StringFormat("SendSocket Connect Error[%d]%s",nRet, _sockSend.GetMsg()));
      return false;
   }
   
   nRet = _sockSend.ConnectSvr();
   if(nRet!=E_OK)
   {
      showErr(StringFormat("Connect Error[%d]%s",nRet, _sockSend.GetMsg()));
      return false;
   }
   showMsg("Connect ok");
   return true;
}


bool ReadSymbols_from_IniFile(_In_ string sIniFileName, _Out_ string& sMsg, _Out_ TMD& arrMD[])
{
   string sSymbols;
   if(__Ini_GetVal(sIniFileName, "SYMBOL", "SYMBOL", sSymbols)==false)
   {
      sMsg = StringFormat("[%s]No symbol info in the ini file", sIniFileName);
      return false;
   }
      
   string arrSymbols[];
   ushort deli = StringGetCharacter(",",0);
   int nCnt = StringSplit(sSymbols, deli, arrSymbols);

   ArrayResize(arrMD, nCnt);
   for( int i=0; i<nCnt; i++ )
   {
      arrMD[i].sSymbol = arrSymbols[i];
   }

   PrintFormat("[%d](%s)", ArraySize(arrMD), sSymbols);

   return true;
}


