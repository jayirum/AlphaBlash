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
#property version   "2.00"
#property description   "Use the new-non block socket library\n Market Order"

#property strict


#include "ArbiLatency.mqh"
#include "../../include/Alpha/IniFile.mqh";
#include "../../include/Alpha/socketlib.mqh"
#include "../../include/Alpha/Protocol.mqh"
#include "../../include/Alpha/UtilDateTime.mqh"
#include "../../include/Alpha/OrderFuncMT5.mqh"
#include "../../include/Alpha/CPacketParser.mqh"

enum EN_SOCK_TP { SOCKTP_RECV=0, SOCKTP_SEND};



string   _IniFileName;
string   _sMsg;
char     _zMsg[BUF_LEN];
char     _zRecvBuff[BUF_LEN];

int _nDebug = 0;

input uint I_MAGIC_NO = 20221116;
uint _uMagicNo = I_MAGIC_NO;


CPacketParser  _parser;
SOCKET64 _sockSend=INVALID_SOCKET64;
SOCKET64 _sockRecv=INVALID_SOCKET64;

bool InitSocket()
{
   char wsaData[]; ArrayResize(wsaData,sizeof(WSAData));
   int res=WSAStartup(MAKEWORD(2,2), wsaData);
   if(res!=0) { Print("-WSAStartup failed error: "+string(res)); return false; }

// create a socket
   _sockSend=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
   if(_sockSend==INVALID_SOCKET64) { Print("-Create failed error: "+WSAErrorDescript(WSAGetLastError()));  return false; }

   _sockRecv=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
   if(_sockRecv==INVALID_SOCKET64) { Print("-Create failed error: "+WSAErrorDescript(WSAGetLastError())); return false; }


   return true;
}

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{

   //__ShowAllSymbols();
  
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
   
   Expand_TradingSymbols();
   

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

   if(__Ini_GetVal(_IniFileName, "TRADE_TIME", "TRADE_FROM", _TRADETIME_FROM)==false)
      return -6;

   if(__Ini_GetVal(_IniFileName, "TRADE_TIME", "TRADE_TO", _TRADETIME_TO)==false)
      return -7;

   PrintFormat("Timeout for MD Fetch(%d), Timeout for Reconnect(%d).TRADETIME(%s ~ %s)", _TIMEOUT_MDFETCH, _TIMEOUT_RECONN, _TRADETIME_FROM, _TRADETIME_TO);
   
   //+----------------------------------------------------------------------------------------
   //  Initialize socket library
   //+----------------------------------------------------------------------------------------
   if(InitSocket()==false)
      return -1;
      
      
   _timerTp =  TIMER_STEP_1;
   
   __RunEA_Start();
   
   
//--- create timer
   EventSetMillisecondTimer((int)_TIMEOUT_MDFETCH);
   
//---
   return(INIT_SUCCEEDED);
  }
  
void Expand_TradingSymbols()
{
   for( int i=0; i<ArraySize(_arrMD); i++ )
   {
      SymbolSelect(_arrMD[i].sSymbol,true);
   }
}  
  
  
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
//--- destroy timer
   EventKillTimer();

   //delete _sockSend;
   //delete _sockRecv;
   
   CloseClean();
   
   string arrSymbols[];
   CPosition* PosInfos[];
   _mapOpenPos.CopyTo(arrSymbols, PosInfos);
   
   for( int i=0; i<ArraySize(arrSymbols); i++)
   {
      CPosition* pos;
      if(_mapOpenPos.TryGetValue(arrSymbols[i], pos)){
         PrintFormat("[ClearMap]Delete(%d-%s)", i, arrSymbols[i]);
         delete pos;
      }
   }
   _mapOpenPos.Clear();
   ArrayFree(arrSymbols);
   ArrayFree(PosInfos);
   
   __RunEA_Stop();
   
   Print("OnDeInit...");
}


void CloseClean()
{
   if(_sockSend!=INVALID_SOCKET64)
   {
      if(shutdown(_sockSend,SD_BOTH)==SOCKET_ERROR) Print("-Shutdown failed error: "+WSAErrorDescript(WSAGetLastError()));
      closesocket(_sockSend); _sockSend=INVALID_SOCKET64;
   }
   
   
   if(_sockRecv!=INVALID_SOCKET64)
   {
      if(shutdown(_sockRecv,SD_BOTH)==SOCKET_ERROR) Print("-Shutdown failed error: "+WSAErrorDescript(WSAGetLastError()));
      closesocket(_sockRecv); _sockSend=INVALID_SOCKET64;
   }
   WSACleanup();
   Print("connect closed");
}

    
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
{  
   if(!__RunEA_IsRunnable()) { return; }
   
   if(IsStopped() || !__IsExpertEnabled()) { return; }
   
   if( _timerTp==TIMER_STEP_1 )
   {
      if( !ConnectSvr() )
      {
         // retry connect
         Sleep((int)_TIMEOUT_RECONN);
      }
      else
      {
         if( SendLogin_RecvSocket() )
         {
            Print("Recv Socket Login OK");
            
            Sleep(3000);
            if(!Send_OpenPosition(0, true))
            {
               Print("Failed to Send Open Position for initializtion");
               ExpertRemove();
               return;
            }
            _timerTp = TIMER_STEP_2;
         }
         else
         {
            showErr("Failed to send Logon. Re-load EA", true, true);
            EventKillTimer();
         }
      }
      
      //
      return;
      //
   }
   else if (_timerTp==TIMER_STEP_2 )
   {
      EN_RET ret = Receive_OrderCommand();
      if( ret==RET_OK )
      {
         while(_parser.Size()>0 )
         {
            string sOnePacket;
            bool bExist = false;
            _parser.Get_OnePacket(sOnePacket, bExist);
            if(!bExist) { return; }
               
            CProtoUtils util;
            string sPacketCode = util.PacketCode(sOnePacket);
            
            if(sPacketCode==CODE_ORDER_OPEN)
            {
               ulong uTicket;
               if(Open_NewPosition(sOnePacket, StringLen(sOnePacket), _Out_ uTicket) ) { Send_OpenPosition(_In_ uTicket, false); }
               else                                        { ExpertRemove(); return; }
            }
            else if (sPacketCode==CODE_ORDER_CLOSE)
            {
               if(Close_Position(sOnePacket, StringLen(sOnePacket)) ) {}
               else                                     { ExpertRemove(); return;}
               
            }
            else 
            {
               StringFormat("Wrong Packet(%s)", sPacketCode); ExpertRemove(); return;
            }
         }
      }   
      else if (ret==RET_TIMEOUT) { Send_MarketData(); }
      else                       { return; }

   }
}

//void CloseOpenPosition()
//{
//   string arrSymbols[];
//   CPosition* PosInfos[];
//   _mapOpenPos.CopyTo(arrSymbols, PosInfos);
//   
//   for( int i=0; i<ArraySize(arrSymbols)-1; i++)
//   {
//      CPosition* pos;
//      _mapOpenPos.TryGetValue(arrSymbols[i], pos);
//      if( pos.HasPassedWaitMinutes(_WAITMINUTES_CLOSE) )
//      {
//         if(Place_CloseOrder(arrSymbols[i],pos.getTicket(),pos.getVol(),pos.getBuySellTp(),pos.getFillTp()))
//         {
//            delete pos;
//            _mapOpenPos.Remove(arrSymbols[i]);
//         }
//      }
//   }
//   
//   
//   ArrayFree(arrSymbols);
//   ArrayFree(PosInfos);
//}



EN_RET Receive_OrderCommand()
{
   uchar FullData[];
   char recvBuf[512]; int nRecvLen=512; int nAllRecvLen=0; bool bNext=false;
   int res = 0;
   while(true)
   {
      res=recv(_sockRecv,recvBuf,nRecvLen,0);
      if(res<0)
      {
         int err=WSAGetLastError();
         //PrintFormat("---------WSAGetLastError(%d)", err);
         if(err==WSAEWOULDBLOCK) { return RET_TIMEOUT; }
         else { 
            //Print("-Receive failed error: "+string(err)+" "+WSAErrorDescript(err)); /*CloseClean();*/ 
            return RET_ERR; 
         }
            
      }
      else if(res==0 && nAllRecvLen==0) { Print("-Receive. connection closed"); return RET_DISCONN ; }
      else if(res>0) { nAllRecvLen+=res; ArrayCopy(FullData,recvBuf,ArraySize(FullData),0,res); }
      
      if(res>=0 && res<nRecvLen) break;
   }

   if(nAllRecvLen<=0)
      return RET_ERR;
      
   
   string sFullData = CharArrayToString(FullData);
   PrintFormat("[RECEIVE](%s)", sFullData);
   
   if(_parser.Add_Packet(sFullData)<0)
   {
      PrintFormat("[Receive_OrderCommand]Packet error!!!");
      ExpertRemove();
   }

   //o_nRecvSize = StringLen(o_sRecvPacket);
   
   
 
   return RET_OK;  
}

bool Open_NewPosition(string sOnePack, int nPackLen, _Out_ ulong& uTicket)
{
   uTicket = 0;
   string sCode;
   CProtoGet get;
   if(!get.ParsingWithHeader(sOnePack, nPackLen))
   {
      PrintFormat("ProtoGet ParsingWithHeader Error(%s)(%s)", get.GetMsg(), sOnePack);         
      return false;
   }

   if( !get.GetCode(_Out_ sCode) )
   {
      PrintFormat("ProtoGet GetCode Error(%s)",sCode);
      return false;
   }
   //PrintFormat("[Receive_OrderCommand-3](Code:%s)", sCode);
   
   if(sCode==CODE_RETURN_ERROR)   //9002
   {
      CProtoUtils util2;
      sCode = util2.GetErrCode(sOnePack);
      PrintFormat("[Recv Error Code](%s)(%s)", sCode, sOnePack);
      //showErr(_sMsg);
      return false;
   }
   
   //PrintFormat("[Receive_OrderCommand-4](Before Place_OpenOrder:%s)", sCode);
   if( !Place_OpenOrder(get, uTicket) )
      return false;

   return true;
}


bool Close_Position(string sOnePack, int nPackLen)
{
   string sCode;
   CProtoGet get;
   if(!get.ParsingWithHeader(sOnePack, nPackLen))
   {
      PrintFormat("ProtoGet ParsingWithHeader Error(%s)(%s)", get.GetMsg(), sOnePack);         
      return false;
   }

   if( !get.GetCode(_Out_ sCode) )
   {
      PrintFormat("ProtoGet GetCode Error(%s)",sCode);
      return false;
   }
   //PrintFormat("[Receive_OrderCommand-3](Code:%s)", sCode);
   
   if(sCode==CODE_RETURN_ERROR)   //9002
   {
      CProtoUtils util2;
      sCode = util2.GetErrCode(sOnePack);
      PrintFormat("[Recv Error Code](%s)(%s)", sCode, sOnePack);
      //showErr(_sMsg);
      return false;
   }


   int uTicket = 0;
   get.GetVal(FDN_TICKET, uTicket);
   
   return Place_CloseOrder(uTicket);
}

bool Place_CloseOrder(ulong uTicket)
{
   if(!PositionSelectByTicket(uTicket))
   {
      PrintFormat("Failed to PositionSelectByTicket(%d)-%d", uTicket, GetLastError());
      return false;
   }


   //--- Declare and initialize the trade request and result of trade request
   MqlTradeRequest req={};
   MqlTradeResult result={};

      
   //--- Setting the operation params
   req.action = TRADE_ACTION_DEAL;
   req.position = uTicket;
   req.symbol = PositionGetString(POSITION_SYMBOL);
   req.volume = PositionGetDouble(POSITION_VOLUME);
   req.deviation = _SLIPPAGE_POINT;
   
   //--- Set the price and order type depending on the position type
   if(PositionGetInteger(POSITION_TYPE) == POSITION_TYPE_BUY)  {req.type = ORDER_TYPE_SELL;}
   else                                                        {req.type = ORDER_TYPE_BUY;}
   
   ENUM_ORDER_TYPE_FILLING fillTp;
   if( !__Fill_OrderType(req.symbol, _Out_ fillTp))
   {
      Print("[Place_CloseOrder]Fill_OrderType error");
      return false;
   }
   req.type_filling = fillTp; 
   
   PrintFormat("Before Place CloseOrder(%d)(%s)(Vol:%f)(Bid:%f)(Ask:%f)(Spread:%d)(MDTime:%s)", 
   uTicket, req.symbol, req.volume, __GetBid(req.symbol), __GetAsk(req.symbol), __GetSpreadPts(req.symbol), __GetMarketTime_S(req.symbol));
   
   if( !__PlaceOrderMT5(req,result,_RETRYCNT_ORD, _sMsg) )
   { return false; }
   
   PrintFormat("After Place CloseOrder(%d)", uTicket);
   
   CPosition* pos;
   if(_mapOpenPos.TryGetValue(req.symbol, pos))
      delete pos;

   return true;
}

bool Place_OpenOrder(_In_ CProtoGet& get, _Out_ ulong &uTicket)
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
   //req.price      = __GetCurrPrc(sSymbol, sBuySell) ;
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
   req.magic = _uMagicNo;
   //req.order = 0;
   //req.type_time;
   //req.expiration;
   //req.comment;
   //req.position  = ;          // ticket of the position
   //req.position_by;      
   
   PrintFormat("Before Place OpenOrder(%s)(%s)(Vol:%f)(RecvPrc:%.5f)(OrdPrc:%f)(Bid:%f)(Ask:%f)(Spread:%d)(MDTime:%s)", 
      sSymbol, sBuySell, req.volume, dRecvPrc, req.price, __GetBid(sSymbol), __GetAsk(sSymbol), __GetSpreadPts(sSymbol), __GetMarketTime_S(sSymbol));

   if( !__PlaceOrderMT5(req,result,_RETRYCNT_ORD, _sMsg) )
      return false;
   
   PrintFormat("After Place OpenOrder(%s)(%s)(Ticket:%d)(Magic:%d)(RecvPrc:%.5f)(OrdPrc:%f)(Bid:%f)(Ask:%f)(MDTime:%s)", 
      sSymbol, sBuySell, result.order, _uMagicNo, dRecvPrc, req.price, __GetBid(sSymbol), __GetAsk(sSymbol), __GetMarketTime_S(sSymbol));

   CPosition* pos = new CPosition(result.order, result.volume, sBuySell, req.type_filling );
   _mapOpenPos.Add(sSymbol, pos);
   
   uTicket = result.order;
   
   return true;
}
//
//bool InitSock()
//{
//   _sockSend = new ClientSocket(true);
//   _sockRecv = new ClientSocket(true);
//   
//   int nResult = 0;
//   string sIP, sPort, sRecvTimeout, sSendTimeout, s;
//   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "IP", sIP)) nResult++;
//   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "PORT", sPort)) nResult++;
//   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "SENDTIMEOUT", sSendTimeout)) nResult++;
//   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "RECVTIMEOUT", sRecvTimeout)) nResult++;
//   
//   if(nResult<4){
//      showErr("Wrong ini file(%s) info(IP, Port,RETRY_CONN_TIMEOUT)");
//      return false;
//   }
//   
//   PrintFormat("Server (IP:%s)(Port:%s)(SendTimeOut:%s)(RecvTimeOut:%s)", sIP, sPort,sSendTimeout,sRecvTimeout);
//   
//   _sockRecv.Initialize(sIP, (ushort)StringToInteger(sPort), (int)StringToInteger(sSendTimeout), (int)StringToInteger(sRecvTimeout));
//   _sockSend.Initialize(sIP, (ushort)StringToInteger(sPort), (int)StringToInteger(sSendTimeout), (int)StringToInteger(sRecvTimeout));
//   
//   
//   return true;
//}
  

bool Is_AlreadyOpened(string sSymbol)
{
   return _mapOpenPos.ContainsKey(sSymbol);
}

bool Is_TradingTime()
{
   string sNow = StringSubstr( __TimeToStr_hh_mm_ss(TimeCurrent()), 0, 5);
   
   if( StringCompare(_TRADETIME_FROM, sNow)<=0 && StringCompare(sNow, _TRADETIME_TO) <0 )
      return true; 
   
   return false;
}

void Send_MarketData()
{  
   if(!Is_TradingTime())
      return;


   for (int i=0; i<ArraySize(_arrMD); i++ )
   {
      //if(Is_AlreadyOpened(_arrMD[i].sSymbol)){
      //   //PrintFormat("Send_MarketData-2:%s",_arrMD[i].sSymbol);
      //   continue;
      //}
         
      datetime nNewTime = (datetime) SymbolInfoInteger(_arrMD[i].sSymbol, SYMBOL_TIME);
      if( _arrMD[i].time!=0 && _arrMD[i].time == nNewTime ){
         //PrintFormat("[Send_MarketData]symbol:%s, lasttime:%d, newtime:%d",_arrMD[i].sSymbol,_arrMD[i].time, nNewTime );
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
      //Send2Svr(_sockSend, sendBuf, len, "MD_CheckNSend", false);   
      Send2Svr(_sockSend, sendBuf, len, "MD_CheckNSend", false);   
      
      //PrintFormat("[MD_SEND-%d](%s)", i, sendBuf);
      delete set;
   }

}



bool Send_OpenPosition(ulong uTicket, bool bInit=false)
{  

   for( int i=PositionsTotal()-1; i>-1; i-- )
   {
      string symbol = PositionGetSymbol(i);
      
      if( PositionGetInteger(POSITION_MAGIC)!=_uMagicNo ){
         PrintFormat("[%s] MagicNo(%d) is different from MyMagicNo(%d)", symbol, PositionGetInteger(POSITION_MAGIC), _uMagicNo);
         continue;
      }
      
      bool bSend = false;
      if(bInit)
      {
         bSend = true;
      }
      else
      {
         if(uTicket==PositionGetInteger(POSITION_TICKET))
         {
            bSend = true;
         }
         else
         {
            CPosition* pPos;
            if( _mapOpenPos.TryGetValue(symbol, pPos)==true )
               continue;
         }   
      }
      
      if(!bSend)
         continue;
         
      CProtoSet* set = new CProtoSet;
      set.Begin();
      set.SetVal(FDS_CODE,       CODE_POSITION);
      set.SetVal(FDS_KEY,        _brokerName);
      set.SetVal(FDS_SYMBOL,     PositionGetString(POSITION_SYMBOL));       
      set.SetVal(FDN_TICKET,     PositionGetInteger(POSITION_TICKET));
      set.SetVal(FDD_OPEN_PRC,   PositionGetDouble(POSITION_PRICE_OPEN));
      set.SetVal(FDD_LOTS,       PositionGetDouble(POSITION_VOLUME));
      
      string sBuySell;
      if( PositionGetInteger(POSITION_TYPE)==POSITION_TYPE_BUY )
         sBuySell = DEF_BUY;
      else
         sBuySell = DEF_SELL;
         
      set.SetVal(FDS_ORD_SIDE,  sBuySell);
         
      string sendBuf;
      int len = set.Complete(sendBuf, true);
      //Send2Svr(_sockSend, sendBuf, len, "MD_CheckNSend", false);   
      if(!Send2Svr(_sockSend, sendBuf, len, "Send_OpenPosition", false))
      {
         Print("[Send_OpenPosition] Failed to send data");
         return false;
      }
      
      PrintFormat("[Position_SEND](%s)", sendBuf);
      delete set;
      
      if(bInit)
      {
         CPosition* pos = new CPosition(PositionGetInteger(POSITION_TICKET), PositionGetDouble(POSITION_VOLUME), sBuySell, 0 );
         _mapOpenPos.Add(symbol, pos);
         PrintFormat("AddPosition when Init:%s", symbol);
      }
   }
   return true;
}




//bool Send2Svr(_In_ ClientSocket& sock,  _In_ string& sendBuf, int nSendLen, string sCaller, bool bLog=true)
bool Send2Svr( _In_ SOCKET64& sock, _In_ string& sendBuf, int nSendLen, string sCaller, bool bLog=true)
{  
   //if( ERR_OK != sock.Send(sendBuf, nSendLen) )
   //{
   //   _sMsg = StringFormat("[%s]%s", sCaller, sock.GetMsg());
   //   //showErr( sMsg );
   //   return false;
   //}
   
   int nToSend = StringLen(sendBuf);      
   bool bRetval = true;
   uchar arr[];
   StringToCharArray(sendBuf, arr);
   
   int res=send(sock, arr,nToSend,0);
   if(res==SOCKET_ERROR){ Print("-Send failed error: "+WSAErrorDescript(WSAGetLastError())); return false; }
   //else printf("Sent %d bytes of %d",res,nToSend);
   return true;
}

bool ConnectSvr()
{   
//   int nRet = _sockSend.ConnectSvr();
//   if(nRet!=E_OK)
//   {
//      showErr(StringFormat("SendSocket Connect Error[%d]%s",nRet, _sockSend.GetMsg()));
//      return false;
//   }
//   
//   nRet = _sockRecv.ConnectSvr();
//   if(nRet!=E_OK)
//   {
//      showErr(StringFormat("RecvSocket Connect Error[%d]%s",nRet, _sockRecv.GetMsg()));
//      return false;
//   }
//   showMsg("Connect ok");
//   return true;

   int nResult = 0;
   string sIP, sPort, sRecvTimeout, sSendTimeout, s;
   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "IP", sIP)) nResult++;
   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "PORT", sPort)) nResult++;
//   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "SENDTIMEOUT", sSendTimeout)) nResult++;
//   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "RECVTIMEOUT", sRecvTimeout)) nResult++;
//   
   if(nResult<2){
      showErr("Wrong ini file(%s) info(IP, Port,RETRY_CONN_TIMEOUT)");
      return false;
   }
   ushort usPort = (ushort)StringToInteger(sPort);
   
   char ch[]; StringToCharArray(sIP,ch);
   sockaddr_in addrin;
   addrin.sin_family=AF_INET;
   addrin.sin_addr.u.S_addr=inet_addr(ch);
   addrin.sin_port=htons(usPort);

   ref_sockaddr ref; ref.in=addrin;
   int res=connect(_sockSend,ref.ref,sizeof(addrin));
   if(res==SOCKET_ERROR)
     {
      int err=WSAGetLastError();
      if(err!=WSAEISCONN) { Print("-SendSock Connect failed error: "+WSAErrorDescript(err)); ; return false; }
     }

// set to nonblocking mode
   int non_block=1;
   res=ioctlsocket(_sockSend,(int)FIONBIO,non_block);
   if(res!=NO_ERROR) { Print("SendSock ioctlsocket failed error: "+string(res)); CloseClean(); return false; }
   
   PrintFormat("SendSock connect ok(IP:%s)(Port:%s)", sIP, sPort);
   
   
   res=connect(_sockRecv,ref.ref,sizeof(addrin));
   if(res==SOCKET_ERROR)
     {
      int err=WSAGetLastError();
      if(err!=WSAEISCONN) { Print("-_sockRecv Connect failed error: "+WSAErrorDescript(err)); CloseClean(); return false; }
     }

// set to nonblocking mode
   non_block=1;
   res=ioctlsocket(_sockRecv,(int)FIONBIO,non_block);
   if(res!=NO_ERROR) { Print("_sockRecv ioctlsocket failed error: "+string(res)); CloseClean(); return false; }
   
   PrintFormat("RecvSock connect ok(IP:%s)(Port:%s)", sIP, sPort);
   

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
bool SendLogin_RecvSocket()
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

   //bRslt = Send2Svr(_sockRecv, sResultPacket, nSend, "SendLogin_RecvSocket-R");
   bRslt = Send2Svr( _sockRecv, sResultPacket, nSend, "SendLogin_RecvSocket-R");
   
   return bRslt;
}



