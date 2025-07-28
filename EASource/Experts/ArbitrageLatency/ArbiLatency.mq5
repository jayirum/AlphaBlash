//+------------------------------------------------------------------+
//|                                             ArbiLatency_v1.0.mq5 |
//+------------------------------------------------------------------+
/*
   1. Send MarketData to the controller (VC++ application)
   
   2. If receives open order, place order
   
   3. For the open position symbol, close the position after the specific interval
   
*/

#property copyright "Copyright 2022, Irumnet Pte Ltd."
#property link      "https://www.irumnet.com"
#property version   "1.00"
#property description   "Use the legacy socket library"
#property strict


#include "ArbiLatency.mqh"
#include "../../include/Alpha/IniFile.mqh";
#include "../../include/Alpha/CClientSocket_Legacy.mqh"
#include "../../include/Alpha/Protocol.mqh"
#include "../../include/Alpha/UtilDateTime.mqh"
#include "../../include/Alpha/OrderFuncMT5.mqh"

enum EN_SOCK_TP { SOCKTP_RECV=0, SOCKTP_SEND};

ClientSocket   *_sockSend, *_sockRecv;

string   _IniFileName;
string   _sMsg;
char     _zMsg[BUF_LEN];
char     _zRecvBuff[BUF_LEN];

int _nDebug = 0;

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
   
   if(__Ini_GetVal(_IniFileName, "ORDER_OPTION", "SLIPPAGE_POINT", s)==false)
      return -3;
   _SLIPPAGE_POINT = (int)StringToInteger(s);

   if(__Ini_GetVal(_IniFileName, "ORDER_OPTION", "RETRY_COUNT", s)==false)
      return -4;
   _RETRYCNT_ORD = (int)StringToInteger(s);
   
   if(__Ini_GetVal(_IniFileName, "ORDER_OPTION", "WAITMINUTES_FOR_CLOSE", s)==false)
      return -5;
   _WAITMINUTES_CLOSE = (int)StringToInteger(s);


   PrintFormat("Timeout for MD Fetch(%d), Timeout for Reconnect(%d)", _TIMEOUT_MDFETCH, _TIMEOUT_RECONN);
   
   //+----------------------------------------------------------------------------------------
   //  Initialize socket library
   //+----------------------------------------------------------------------------------------
   if( InitSock()==false )
      return -1;
      
      
   _timerTp =  TIMER_STEP_1;
   
   __RunEA_Start();
   
   
//--- create timer
   EventSetMillisecondTimer((int)_TIMEOUT_MDFETCH);
   
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
   delete _sockRecv;
   
   string arrSymbols[];
   CPosition* PosInfos[];
   _mapOpenPos.CopyTo(arrSymbols, PosInfos);
   
   for( int i=0; i<ArraySize(arrSymbols); i++)
   {
      CPosition* pos;
      if(_mapOpenPos.TryGetValue(arrSymbols[i], pos))
         delete pos;
   }
   _mapOpenPos.Clear();
   ArrayFree(arrSymbols);
   ArrayFree(PosInfos);
   
   __RunEA_Stop();
   
   Print("OnDeInit...");
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
         Sleep((int)_TIMEOUT_RECONN);
         return;
      }
      else
      {
         if( Login_RecvSocket() )
         {
            Print("Recv Socket Login OK");
            _timerTp = TIMER_STEP_2;
         }
         else
         {
            showErr("Failed to send Logon. Re-load EA", true, true);
            EventKillTimer();
            return;
         }
      }
      return;
   }
   else if (_timerTp==TIMER_STEP_2 )
   {
      if( ReceiveNOrder()!= RET_OK ){
         return;
      }

      SendMarketData();
      
      CloseOpenPosition();
   }
}


void CloseOpenPosition()
{
   string arrSymbols[];
   CPosition* PosInfos[];
   _mapOpenPos.CopyTo(arrSymbols, PosInfos);
   
   for( int i=0; i<ArraySize(arrSymbols)-1; i++)
   {
      CPosition* pos;
      _mapOpenPos.TryGetValue(arrSymbols[i], pos);
      if( pos.HasPassedWaitMinutes(_WAITMINUTES_CLOSE) )
      {
         if(Place_CloseOrder(arrSymbols[i],pos.getTicket(),pos.getVol(),pos.getBuySellTp(),pos.getFillTp()))
         {
            delete pos;
            _mapOpenPos.Remove(arrSymbols[i]);
         }
      }
   }
   
   
   ArrayFree(arrSymbols);
   ArrayFree(PosInfos);
}


bool Place_CloseOrder(string sSymbol,ulong PositionTicket, double dVol, string sBuySellTp, ENUM_ORDER_TYPE_FILLING fillTp )
{
   MqlTradeRequest req={};
   MqlTradeResult result={};
   
   req.action     = TRADE_ACTION_DEAL;        // type of trade operation
   req.symbol     = sSymbol;
   req.volume     = dVol;
   req.price      = __GetCurrPrc(sSymbol, sBuySellTp) ;
   req.deviation  = _SLIPPAGE_POINT;
   if(sBuySellTp==DEF_BUY)    req.type = ORDER_TYPE_SELL;
   else                       req.type = ORDER_TYPE_BUY;
   req.type_filling = fillTp; 

   //req.stoplimit = ;
   //req.sl = 0;
   //req.tp = 0;
   //req.magic = 0;
   //req.order = 0;
   //req.type_time;
   //req.expiration;
   //req.comment;
   req.position  = PositionTicket;          // ticket of the position
   //req.position_by;      
   
   if( __PlaceOrderMT5(req,result,_RETRYCNT_ORD, _sMsg) )
      return false;

   
   showMsg(StringFormat("CloseOrder OK(%s)(Ticket:%d)", sSymbol,result.order));
   
   return true;
}


EN_RET ReceiveNOrder()
{  
   int nRecvSize = 0;
   ALPHA_RET ret = _sockRecv.RecvWithBuffering(_Out_ nRecvSize);
   if( nRecvSize<=0 )
   {
      if(ret==E_OK || ret==E_TIMEOUT){
         return RET_OK;
      }
      else if ( ret==E_DISCONN_FROM_SVR || ret == E_NON_CONNECT )
      {
         _timerTp=TIMER_STEP_1;
         return RET_DISCONN;
      }
      showErr(_sockRecv.GetMsg());
      return RET_ERR;
   }
   
   //PrintFormat("[ReceiveNOrder-1](receive Size:%d)", nRecvSize);
   
   bool bDataReceived = true;
   while(bDataReceived)
   {  
      bool bErr;
      string sOnePack, sCode;
      bDataReceived = _sockRecv.GetOnePacket(sOnePack, bErr);
      if(!bDataReceived)
      {
         if(bErr) showErr(_sockRecv.GetMsg());
         break;
      }
      
      int nPackLen = StringLen(sOnePack);
      PrintFormat("[GetOnePacket](%s)", sOnePack);

      CProtoGet get;
      if(!get.ParsingWithHeader(sOnePack, nPackLen))
      {
         PrintFormat("ProtoGet Parsing Error(%s)",sOnePack);         
         return RET_ERR;
      }

      if( !get.GetCode(_Out_ sCode) )
      {
         PrintFormat("ProtoGet GetCode Error(%s)",sCode);
         return RET_ERR;
      }
      //PrintFormat("[ReceiveNOrder-3](Code:%s)", sCode);
      
      if(sCode==CODE_RETURN_ERROR)   //9002
      {
         CProtoUtils util2;
         sCode = util2.GetErrCode(sOnePack);
         PrintFormat("[Recv Error Code](%s)(%s)", sCode, sOnePack);
         //showErr(_sMsg);
         return RET_ERR;
      }
      
      if(sCode==CODE_ORDER_OPEN)
      {
         //PrintFormat("[ReceiveNOrder-4](Before Place_OpenOrder:%s)", sCode);
         Place_OpenOrder(get);
      }
   }
   return RET_OK;
}


bool Place_OpenOrder(_In_ CProtoGet& get)
{
   string sSymbol;
   string sBuySell;
   double dRecvPrc= 0;
   double dVol= 0;
   double dBid=0, dAsk = 0;
   
   get.GetVal(FDS_SYMBOL, sSymbol);
   get.GetVal(FDS_ORD_SIDE, sBuySell);
   get.GetVal(FDD_OPEN_PRC, dRecvPrc);
   get.GetVal(FDD_LOTS, dVol);

   MqlTradeRequest req={};
   MqlTradeResult result={};
   
   req.action     = TRADE_ACTION_DEAL;        // type of trade operation
   req.symbol     = sSymbol;
   req.volume     = dVol;
   req.price      = __GetCurrPrc(sSymbol, sBuySell) ;
   req.deviation  = _SLIPPAGE_POINT;
   if(sBuySell==DEF_BUY)   req.type = ORDER_TYPE_BUY;
   else                    req.type = ORDER_TYPE_SELL;
   
   //dBid = __GetCurrPrc(sSymbol,DEF_SIDE_SELL);
   //dAsk = __GetCurrPrc(sSymbol,DEF_SIDE_BUY);
      
   ENUM_ORDER_TYPE_FILLING fillTp;
   if( !__Fill_OrderType(sSymbol, _Out_ fillTp))
   {
      Print("__Fill_OrderType error");
      return false;
   }
   req.type_filling = fillTp; 

   //req.stoplimit = ;
   //req.sl = 0;
   //req.tp = 0;
   //req.magic = 0;
   //req.order = 0;
   //req.type_time;
   //req.expiration;
   //req.comment;
   //req.position  = ;          // ticket of the position
   //req.position_by;      
   
   PrintFormat("Before OpenOrder(%s)(%s)(Vol:%f)(RecvPrc:%.5f)(OrdPrc:%f)(Bid:%f)(Ask:%f)(Spread:%d)(MDTime:%s)", 
      sSymbol, sBuySell, req.volume, dRecvPrc, req.price, __GetBid(sSymbol), __GetAsk(sSymbol), __GetSpreadPts(sSymbol), __GetMarketTime_S(sSymbol));

   if( !__PlaceOrderMT5(req,result,_RETRYCNT_ORD, _sMsg) )
      return false;
   
   PrintFormat("After OpenOrder(%s)(%s)(Ticket:%d)(RecvPrc:%.5f)(OrdPrc:%f)(Bid:%f)(Ask:%f)(MDTime:%s)", 
      sSymbol, sBuySell, result.order, dRecvPrc, req.price, __GetBid(sSymbol), __GetAsk(sSymbol), __GetMarketTime_S(sSymbol));

   CPosition* pos = new CPosition(result.order, result.volume, sBuySell, req.type_filling );
   _mapOpenPos.Add(sSymbol, pos);
   
   //showMsg(StringFormat("OpenOrder OK(%s)(Ticket:%d)(Try price:%.5f)", sSymbol,result.order, dRecvPrc));
   
   return true;
}

bool InitSock()
{
   _sockSend = new ClientSocket(true);
   _sockRecv = new ClientSocket(true);
   
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
   
   PrintFormat("Server (IP:%s)(Port:%s)(SendTimeOut:%s)(RecvTimeOut:%s)", sIP, sPort,sSendTimeout,sRecvTimeout);
   
   _sockRecv.Initialize(sIP, (ushort)StringToInteger(sPort), (int)StringToInteger(sSendTimeout), (int)StringToInteger(sRecvTimeout));
   _sockSend.Initialize(sIP, (ushort)StringToInteger(sPort), (int)StringToInteger(sSendTimeout), (int)StringToInteger(sRecvTimeout));
   
   
   return true;
}
  

bool Is_AlreadyOpened(string sSymbol)
{
   return _mapOpenPos.ContainsKey(sSymbol);
}

void SendMarketData()
{  
   for (int i=0; i<ArraySize(_arrMD); i++ )
   {
      if(Is_AlreadyOpened(_arrMD[i].sSymbol)){
         //PrintFormat("SendMarketData-2:%s",_arrMD[i].sSymbol);
         continue;
      }
         
      datetime nNewTime = (datetime) SymbolInfoInteger(_arrMD[i].sSymbol, SYMBOL_TIME);
      if( _arrMD[i].time!=0 && _arrMD[i].time == nNewTime ){
         //PrintFormat("[SendMarketData]symbol:%s, lasttime:%d, newtime:%d",_arrMD[i].sSymbol,_arrMD[i].time, nNewTime );
         continue;
      }
      
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
   
   nRet = _sockRecv.ConnectSvr();
   if(nRet!=E_OK)
   {
      showErr(StringFormat("RecvSocket Connect Error[%d]%s",nRet, _sockRecv.GetMsg()));
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
      if( __ExistSymbol(arrSymbols[i])==false )
      {
         PrintFormat("Symbol does NOT exist(%s)", arrSymbols[i]);
         return false;
      }
      
      arrMD[i].sSymbol = arrSymbols[i];
   }

   PrintFormat("[%d](%s)", ArraySize(arrMD), sSymbols);

   return true;
}



//+----------------------------------------------------------------------------------------
//  Login
//+----------------------------------------------------------------------------------------
bool Login_RecvSocket()
{
   int nRet = 0;
   bool bRslt = false;

   string      sResultPacket;
   
   CProtoSet   set;
   set.Begin();
   set.SetVal(FDS_CODE, CODE_LOGON);
   set.SetVal(FDS_KEY,  _brokerName);
   set.SetVal(FDS_CLIENT_SOCKET_TP, "R");
   
   int nSend = set.Complete(sResultPacket, true);

   bRslt = Send2Svr(_sockRecv, sResultPacket, nSend, "Login_RecvSocket-R");
   
   return bRslt;
}



