//+------------------------------------------------------------------+
//|                                             Sentosa_v1.mq5 |
//+------------------------------------------------------------------+

#property copyright "Copyright 2023, Irumnet Pte Ltd."
#property link      "https://www.irumnet.com"
#property version   "1.0"
string __VERSION = "1.0";
//#property description   "Use the new-non block socket library\n Market Order"

#property strict


#include "SentosaCommon.mqh"
#include "../../include/Alpha/IniFile.mqh";
#include "../../include/Alpha/socketlib.mqh"
#include "../../include/Alpha/Protocol.mqh"
#include "../../include/Alpha/UtilDateTime.mqh"
#include "../../include/Alpha/OrderFuncMT5.mqh"
#include "../../include/Alpha/CPacketParser.mqh"
#include "../../include/Alpha/CommandCodes.mqh"
#include "../../include/Alpha/getMacAddress.mqh"
#include "H_Real_BalancePosProfits.mqh"
#include "H_CheckOnTrade.mqh"
#include "H_Exec_PosOrd.mqh"
#include "H_CMsgToManager.mqh"

input string I_USER_ID = "test01";
input string I_PASSCODE = "1111";



string   _IniFileName;
string   _sMsg;
char     _zMsg[BUF_LEN];
char     _zRecvBuff[BUF_LEN];
string   _sMacAddr;

int _nDebug = 0;

CCheckBalance  _balance;

CPacketParser  _parser;
SOCKET64 _sockAuth=INVALID_SOCKET64;
SOCKET64 _sockSend=INVALID_SOCKET64;
SOCKET64 _sockRecv=INVALID_SOCKET64;


bool InitSocket(string sSockTp, bool bInit)
{
   if(bInit)
   {
      char wsaData[]; ArrayResize(wsaData,sizeof(WSAData));
      int res=WSAStartup(MAKEWORD(2,2), wsaData);
      if(res!=0) { Print("-WSAStartup failed error: "+string(res)); return false; }
   }
   
   if(sSockTp==DEF_SOCKTP_AUTH || bInit==true)
   {
      CloseSocket(sSockTp, false);   
      _sockAuth=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      if(_sockAuth==INVALID_SOCKET64) { Print("[E]Failed to create auth socket: "+WSAErrorDescript(WSAGetLastError()));  return false; }
   }

   if(sSockTp==DEF_SOCKTP_SEND || bInit==true)
   {
      CloseSocket(sSockTp, false);
      _sockSend=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      if(_sockSend==INVALID_SOCKET64) { Print("[E]Failed to create send socket: "+WSAErrorDescript(WSAGetLastError()));  return false; }
   }
   
   if(sSockTp==DEF_SOCKTP_RECV || bInit==true)
   {
      CloseSocket(sSockTp, false);
      _sockRecv=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      if(_sockRecv==INVALID_SOCKET64) { Print("[E]Failed to create recv socket: "+WSAErrorDescript(WSAGetLastError())); return false; }
   }

   

   return true;
}

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{

   _sUserId = I_USER_ID;
   _sPasscode = I_PASSCODE;
  
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
   __PrintDebug(StringFormat("IniFile :%s",_IniFileName));
      
   string s;
   if(__Ini_GetVal(_IniFileName, "TIMEOUT_MS", "MD_FETCH", s)==false)
      return -1;
   _TIMEOUT_MDFETCH = (int)StringToInteger(s);
   
   if(__Ini_GetVal(_IniFileName, "TIMEOUT_MS", "RECONNECT", s)==false)
      return -2;
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
      
   string sIP, sPort;
   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "LOGON_AUTH_IP", sIP)==false)
      return -8;
      
   if(__Ini_GetVal(_IniFileName, "SERVER_INFO", "LOGON_AUTH_PORT", sPort)==false)
      return -9;
   
   _SvrInfo.Ip_Auth = sIP;
   _SvrInfo.Port_Auth = (ushort)StringToInteger(sPort);
   

   __PrintDebug(StringFormat("Timeout for MD Fetch(%d), Timeout for Reconnect(%d).TRADETIME(%s ~ %s)", _TIMEOUT_MDFETCH, _TIMEOUT_RECONN, _TRADETIME_FROM, _TRADETIME_TO));
   
   if( !__getMacAddress(_sMacAddr, _sMsg) )
   {
      PrintFormat("failed to get Mac:%s", _sMsg);
      return -11;
   }
   
   //+----------------------------------------------------------------------------------------
   //  Initialize socket library
   //+----------------------------------------------------------------------------------------
   if(InitSocket("", true)==false)
      return -1;
    
   
   ComposeAppId();
   
   _posHandler = new CPosChangeHandler;
   _ordHandler = new COrdChangeHandler;
        
   __RunEA_Start();
   
   
//--- create timer
   EventSetMillisecondTimer((int)_TIMEOUT_MDFETCH);
   //EventSetMillisecondTimer(1000);
   
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

   //delete _sockSend;
   //delete _sockRecv;
   
   CloseSocket("", true);
   
//   string arrSymbols[];
//   CPosition* PosInfos[];
//   _mapOpenPos.CopyTo(arrSymbols, PosInfos);
//   
//   for( int i=0; i<ArraySize(arrSymbols); i++)
//   {
//      CPosition* pos;
//      if(_mapOpenPos.TryGetValue(arrSymbols[i], pos)){
//         PrintFormat("[ClearMap]Delete(%d-%s)", i, arrSymbols[i]);
//         delete pos;
//      }
//   }
//   _mapOpenPos.Clear();
//   ArrayFree(arrSymbols);
//   ArrayFree(PosInfos);
   
   delete _posHandler;
   delete _ordHandler;
   __RunEA_Stop();
   
   Print("OnDeInit...");
}


void CloseSocket(string sSockTp, bool bAll)
{
   if(sSockTp==DEF_SOCKTP_SEND || bAll==true)
   {
      __PrintDebug(StringFormat("Close Send Socket(%d)",_sockSend));
      shutdown(_sockSend,SD_BOTH);
      closesocket(_sockSend); _sockSend=INVALID_SOCKET64;
   }
   
   if(sSockTp==DEF_SOCKTP_RECV || bAll==true)
   {
      __PrintDebug(StringFormat("Close Recv Socket(%d)",_sockRecv));
      shutdown(_sockRecv,SD_BOTH);
      closesocket(_sockRecv); _sockRecv=INVALID_SOCKET64;
   }

   if(sSockTp==DEF_SOCKTP_AUTH || bAll==true)
   {
      __PrintDebug(StringFormat("Close Auth Socket(%d)",_sockAuth));
      shutdown(_sockAuth,SD_BOTH);
      closesocket(_sockAuth); _sockAuth=INVALID_SOCKET64;
   }

   if(bAll) WSACleanup();
}




//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
{  
   if(!__RunEA_IsRunnable()) { return; }
   
   if(IsStopped() || !__IsExpertEnabled()) { return; }
   
   if(_Admin.Is_Paused())  return;   
   
   
   if( _CURR_STEP==STEP_INIT )
   {
      if(!Conn_Login(DEF_SOCKTP_AUTH) ) {__KillEA("[E]Failed to LogonAuth");}
      
      UpdateLogonStep(STEP_LOGINAUTH_SENT);
   }
   else if( _CURR_STEP==STEP_LOGINAUTH_RECV )
   {
      if( !Conn_Login(DEF_SOCKTP_SEND) ) {__KillEA("[E]Failed to LOGON SendSocket");}
      
      UpdateLogonStep(STEP_LOGINSEND_SENT);
   }
   else if( _CURR_STEP==STEP_LOGINSEND_RECV )
   {
      if( !Conn_Login(DEF_SOCKTP_RECV) ) {__KillEA("[E]Failed to LOGON RecvSocket");}
      
      UpdateLogonStep(STEP_LOGINRECV_SENT);
   }
   else
   {
      _RecvProc_ByCode();
   }   
   
}


void OnTrade()
{
   string sSendBuf;
   if( CheckPos_Action(sSendBuf) )
   {
      Send2Svr(_sockSend, DEF_SOCKTP_SEND, sSendBuf, StringLen(sSendBuf), "Send Position Action", false);   
   }
   
   StringInit(sSendBuf);
   if( CheckOrd_Action(sSendBuf) )
   {
      Send2Svr(_sockSend, DEF_SOCKTP_SEND, sSendBuf, StringLen(sSendBuf), "Send Order Action", false);   
   }
}

void _RecvProc_ByCode()
{
   string   sSockTp;
   SOCKET64 sock;
   
   if( _CURR_STEP==STEP_INIT || _CURR_STEP==STEP_LOGINAUTH_SENT) 
   {
      sSockTp  = DEF_SOCKTP_AUTH;
      sock     = _sockAuth;
   }
   if( _CURR_STEP==STEP_LOGINAUTH_RECV || _CURR_STEP==STEP_LOGINSEND_SENT)
   {
      sSockTp  = DEF_SOCKTP_SEND;
      sock     = _sockSend;
   }
   if ( _CURR_STEP==STEP_LOGINSEND_RECV || _CURR_STEP==STEP_LOGINRECV_SENT || _CURR_STEP==STEP_LOGIN_DONE)
   { // STEP_LOGIN_DONE
      sSockTp  = DEF_SOCKTP_RECV;
      sock     = _sockRecv;
   }
   
   EN_RET ret = RecvCommand_FromSvr(sock, sSockTp);
   if(ret!=RET_OK)
   {
      if ((ret==RET_TIMEOUT) && (_CURR_STEP == STEP_LOGIN_DONE)) 
      { 
         Send_MarketData();
         Send_Balance_PosProfits(); 
      }
      return;
   }

   while(_parser.Size()>0 )
   {
      string sOnePacket;
      bool bExist = false;
      int nRemainCnt = _parser.Get_OnePacket(sOnePacket, bExist);
      if(!bExist) { return; }
         
      //CProtoUtils util; string sPacketCode = util.PacketCode(sOnePacket);
      CProtoGet get;
      int nFieldCnt = get.ParsingWithHeader(sOnePacket, StringLen(sOnePacket));
      if(nFieldCnt<1)
      {
         PrintFormat("Receive the wrong Packet(%s)", sOnePacket);
         continue;
      }
   
      string sPacketCode; 
      if(!get.GetCode(sPacketCode))
      {
         PrintFormat("No Packet Code(%s)", sOnePacket);
         continue;
      }
      if(!get.Is_Success())
      {
         int nRsltCode = get.Get_RsltCode();
         get.GetVal(FDS_MSG, _sMsg);
         _sMsg = StringFormat("Server returns error(%d)(%s)",nRsltCode, _sMsg);
         
         if( sPacketCode==CODE_LOGON_AUTH || sPacketCode==CODE_LOGON )
         {
            __KillEA(_sMsg);
            return;
         }
         Print(_sMsg);
         continue;
      }
      
      if(sPacketCode==CODE_LOGON_AUTH)
      {
         if(!Login_Proc(get, DEF_SOCKTP_AUTH)) return;
         UpdateLogonStep(STEP_LOGINAUTH_RECV);
         return;
      }
      else if(sPacketCode==CODE_LOGON)
      {
         if( _CURR_STEP==STEP_LOGINSEND_SENT )
         {
            if(!Login_Proc(get, DEF_SOCKTP_SEND)) return;
            UpdateLogonStep(STEP_LOGINSEND_RECV);
         }
         else if( _CURR_STEP==STEP_LOGINRECV_SENT )
         {
            if(!Login_Proc(get, DEF_SOCKTP_RECV)) return;
            //
            UpdateLogonStep(STEP_LOGIN_DONE);
            Print("Succeeded in LogOn.Ready to Trade");            
         }
      }
      else if(sPacketCode==CODE_DUP_LOGON)
      {
         get.GetVal(FDS_MSG, _sMsg);
         __KillEA(_sMsg); return;
      }
      else if(sPacketCode==CODE_BALANCE)
      {
         Decide_SubUnSub_Balance(get, sOnePacket);
      }
      else if(sPacketCode==CODE_LOGOFF)
      {
         __KillEA("Receoive Log Off command from Manager");
         return;
      }
      else if (sPacketCode==CODE_POSORD)
      {
         Send_PosOrdSnapshot();
      }
      else if (sPacketCode==CODE_ORDER_CLOSE)
      {
         _PosOrd_Close(get);
      }
      else if (sPacketCode==CODE_ORDER_CHANGE)
      {
         _PosOrd_Change(get);
      }
      else if(sPacketCode==CODE_COMMAND_BY_CODE)
      {
         Excute_ByCommand(get);
      }
      else 
      {
         StringFormat("Wrong Packet(%s)", sPacketCode); ExpertRemove(); return;
      }
      
      SendMsg_ToManager();
      
   } // while(_parser.Size()>0 )
}

void SendMsg_ToManager()
{
   while( _msgToManager.IsEmpty()==false)
   {
      string sBuffer;
      if( _msgToManager.GetMsg(_Out_ sBuffer) )
      {
         Send2Svr(_sockSend, DEF_SOCKTP_SEND, sBuffer, StringLen(sBuffer), "Send Message", true);        
      }
   }
}

void Send_PosOrdSnapshot()
{
   string sPos;
   if( Snapshot_Pos(_sUserId, _sAppId, _brokerName, sPos) )
   {
      Send2Svr(_sockSend, DEF_SOCKTP_SEND, sPos, StringLen(sPos), "Send Position Snaptshot", false);      
   }
   
   string sOrd;
   if( Snapshot_Ord(_sUserId, _sAppId, _brokerName, sOrd) )
   {
      Send2Svr(_sockSend, DEF_SOCKTP_SEND, sOrd, StringLen(sOrd), "Send Order Snaptshot", false);      
   }
}
 
void Decide_SubUnSub_Balance(_In_ CProtoGet &get, string sOnePack)
{
   _balance.Reset();
   //Print("[Decide_SubUnSub_Balance]_balance.Reset();");

   string sRegUnreg;
   get.GetVal(FDS_REGUNREG, sRegUnreg);
   
   _sub.BalanceSub_Update( (sRegUnreg==DEF_REG)? true:false);
}

void Send_Balance_PosProfits()
{
   if(_sub.Is_BalanceSub()==false)
      return;
   
   string sendBuf;
   if( _balance.GetPacket_Balance_PosProfits(sendBuf) )
   {
      Send2Svr(_sockSend, DEF_SOCKTP_SEND, sendBuf, StringLen(sendBuf), "Send_Balance_PosProfits", false);   
   }
}


void Excute_ByCommand(_In_ CProtoGet& get)
{
   string sCommand;
   if(!get.GetVal(FDS_COMMAND_CODE, sCommand))  //TODO.TEST
   {
      PrintFormat("There is no Command Code(%s)", get.GetOrgData());
      return;
   }
      
   if( StringSubstr(sCommand, 0, 1)=="1" )
   {
      _Admin.Excute_ByAdminCode(sCommand);
      return;
   }
   
   if ( sCommand==CMD_MD_SUB || sCommand==CMD_MD_UNSUB)
   {
      string sSymbol;
      if(! get.GetVal(FDS_SYMBOL, sSymbol))   //TODO. TEST
      {
         Print("SUB MD Packet doesn't have symbol:%s");
         return;
      }
      if(sCommand==CMD_MD_SUB)   _AddSymbol_ForMD(sSymbol);
      else                       _RemoveSymbol_ForMD(sSymbol);
   }
   else if ( sCommand==CMD_NOTI_LOGONOUT) // Manager LogOut
   {
      int nAppTp; get.GetVal(FDN_APP_TP, nAppTp);
      if(nAppTp == (int)APPTP_MANAGER )
      {
         Print("Manager Logged Out");
         _sub.ManagerLogOff();
      }
   }
}



_Private_ EN_RET RecvCommand_FromSvr(SOCKET64 &sock, string sSockTp)
{
   uchar FullData[];
   int nAllRecvLen=0; bool bNext=false;
   int res = 0;
   while(true)
   {
      char recvBuf[512]={0}; int nRecvLen=512;
      
      res=recv(sock,recvBuf,nRecvLen,0);      
      if(res<0)
      {
         int err=WSAGetLastError();
         if(err==WSAEWOULDBLOCK) { 
            return RET_TIMEOUT; 
         }
         else 
         { 
            PrintFormat("Receive error(%d)(%s)",err, WSAErrorDescript(err));
            if(err==WSAECONNRESET || err==WSAENETDOWN || err==WSAENETRESET || err==WSAENOTCONN || err==WSAESHUTDOWN || err==WSAECONNREFUSED)
               __KillEA("Kill EA on connection issue");
            return RET_ERR; 
         }
            
      }
      else if(res==0 && nAllRecvLen==0) { Print("-Receive. connection closed"); return RET_DISCONN ; }
      else if(res>0) { 
         nAllRecvLen+=res; 
         ArrayCopy(FullData,recvBuf,ArraySize(FullData),0,res); 
         
      }
      
      if(res>=0 && res<nRecvLen) break;
   }

   if(nAllRecvLen<=0)
      return RET_ERR;
      
   
   string sFullData = CharArrayToString(FullData);
   __PrintDebug(StringFormat("[RECEIVE][%s](%s)",sSockTp, sFullData));
   
   if(_parser.Add_Packet(sFullData)<0)
   {
      PrintFormat("[RecvCommand_FromSvr]Packet error!!!");
      return RET_ERR;
   }

   //o_nRecvSize = StringLen(o_sRecvPacket);
   
   
 
   return RET_OK;  
}


bool Login_Proc(_In_ CProtoGet &get, string sSockTp)
{  
   if(sSockTp==DEF_SOCKTP_AUTH)
   {
      get.GetVal(FDS_RELAY_IP, _SvrInfo.Ip_Relay);
      
      string s;
      get.GetVal(FDS_RELAY_PORT, s);
      _SvrInfo.Port_Relay = (ushort)StringToInteger(s);
      
      string sVersion;
      get.GetVal(FDS_VERSION, sVersion);
      if( sVersion!= __VERSION)  //TODO. TEST
      {
         __KillEA(StringFormat("[LogOn]Version is not matched(Current:%s)(ShouldBe:%s)", __VERSION, sVersion));
         return false;
      }

      PrintFormat("LoginAuth ok. RelayIP(%s) Port(%d), AppId(%s)", _SvrInfo.Ip_Relay, _SvrInfo.Port_Relay, _sAppId);
   }
   
   if(sSockTp==DEF_SOCKTP_RECV)
   {
      Print("Login Recvok");
   }
   
   return true;
}


bool Is_TradingTime()
{
   string sNow = StringSubstr( __TimeToStr_hh_mm_ss(TimeCurrent()), 0, 5);
   
   if( StringCompare(_TRADETIME_FROM, sNow)<=0 && StringCompare(sNow, _TRADETIME_TO) <0 )
      return true; 
   
   return false;
}

bool Send_MarketData()
{
   if( _sub.Is_MDSub()==false)
      return false;
      
   if ( _mdInfo.sSymbol=="")
      return false;
      
   datetime nNewTime = (datetime) SymbolInfoInteger(_mdInfo.sSymbol, SYMBOL_TIME);
   if( _mdInfo.time == nNewTime )
      return false;
      
   MqlTick tick; SymbolInfoTick(_mdInfo.sSymbol,  tick);
   
   _mdInfo.bid = tick.bid;
   _mdInfo.ask = tick.ask;
   _mdInfo.spread = __GetSpreadPts(_mdInfo.sSymbol);
   _mdInfo.time = nNewTime;

   CProtoSet* set = new CProtoSet;
   set.Begin();
   set.SetVal(FDS_CODE,       CODE_MARKET_DATA);
   set.SetVal(FDS_BROKER,     _brokerName);
   set.SetVal(FDS_SUCC_YN,    "Y");
   set.SetVal(FDS_USER_ID,    I_USER_ID);
   set.SetVal(FDS_KEY,        _sAppId);
   set.SetVal(FDS_SYMBOL,     _mdInfo.sSymbol);       
   set.SetVal(FDD_BID,        _mdInfo.bid);
   set.SetVal(FDD_ASK,        _mdInfo.ask);
   set.SetVal(FDD_SPREAD,     _mdInfo.spread);
   set.SetVal(FDS_MARKETDATA_TIME,       __TimeToStr_hh_mm_ss(nNewTime));
   set.SetVal(FDS_LIVEDEMO,    _LiveDemo);
   set.SetVal(FDN_DECIMAL,     __GetDigits(_mdInfo.sSymbol));
   set.SetVal(FDS_FLOW_DIRECTION,   DIRECTION_TO_MGR);
   
   string sendBuf; bool forDelphi=true;
   int len = set.Complete(sendBuf, !forDelphi);
   Send2Svr(_sockSend, DEF_SOCKTP_SEND, sendBuf, len, "Send_MarketData", false);   
   
   delete set;
   
   return true;
}

bool Send2Svr( SOCKET64 &sock, string sSockTp, _In_ string& sendBuf, int nSendLen, string sCaller, bool bLog=true)
{  
  
   int nToSend = StringLen(sendBuf);      
   bool bRetval = true;
   uchar arr[];
   StringToCharArray(sendBuf, arr);
   
   int res = send(sock, arr,nToSend,0);
   
   if(res==SOCKET_ERROR)
   { 
      PrintFormat("[%s] socket Send failed error:%s", sSockTp,WSAErrorDescript(WSAGetLastError())); 
      return false; 
   }
   if(bLog)
      PrintFormat("[Send2Svr]%s", sendBuf);
   return true;
}



bool ConnectSvr(_In_ SOCKET64& sockConn, _In_ string sSockTp) //  _Private_
{
   string sIP     = (sSockTp==DEF_SOCKTP_AUTH)? _SvrInfo.Ip_Auth   : _SvrInfo.Ip_Relay;
   ushort usPort  = (sSockTp==DEF_SOCKTP_AUTH)? _SvrInfo.Port_Auth : _SvrInfo.Port_Relay;
   
   char ch[]; StringToCharArray(sIP,ch);
   sockaddr_in addrin;
   addrin.sin_family=AF_INET;
   addrin.sin_addr.u.S_addr=inet_addr(ch);
   addrin.sin_port=htons(usPort);

   ref_sockaddr ref; ref.in=addrin;
   
   int res=connect(sockConn,ref.ref,sizeof(addrin));
   if(res==SOCKET_ERROR)
     {
      int err=WSAGetLastError();
      if(err!=WSAEISCONN) { Print("[E]Connect failed error: "+WSAErrorDescript(err)); ; return false; }
     }

// set to nonblocking mode
   int non_block=1;
   res=ioctlsocket(sockConn,(int)FIONBIO,non_block);
   if(res!=NO_ERROR) { Print("[E]ioctlsocket failed error: "+string(res)); CloseSocket(sSockTp, false); return false; }
   
   PrintFormat("[%s][%d]Socket connect ok(IP:%s)(Port:%d)",sSockTp, sockConn,  sIP, usPort);
   
   return true;
}

//+----------------------------------------------------------------------------------------
//  Login
//+----------------------------------------------------------------------------------------
bool Conn_Login(string sSockTp)
{
   SOCKET64 sockConn;
   if (sSockTp==DEF_SOCKTP_AUTH) sockConn = _sockAuth ;
   if (sSockTp==DEF_SOCKTP_RECV) sockConn = _sockRecv; 
   if (sSockTp==DEF_SOCKTP_SEND) sockConn = _sockSend;

   if(!ConnectSvr(sockConn, sSockTp))
      return false;

   int nRet = 0;
   bool bRslt = false;

   string sResultPacket;
   
   CProtoSet   set;
   set.Begin();
   
   string sTime =  __TimeToStr_yyyymmddhhmmss(TimeCurrent());
   
   set.SetVal(FDS_KEY,  _sAppId);
   set.SetVal(FDS_BROKER,  _brokerName);
   set.SetVal(FDS_ACCNO_MINE,  _AccNo);
   set.SetVal(FDS_USER_ID,  I_USER_ID);
   set.SetVal(FDS_USER_PASSWORD,  I_PASSCODE);
   set.SetVal(FDS_MAC_ADDR,  _sMacAddr);
   set.SetVal(FDS_LIVEDEMO,  _LiveDemo);
   set.SetVal(FDN_APP_TP, (int)APPTP_EA);
   set.SetVal(FDS_TIME, sTime);
   set.SetVal(FDS_CLIENT_SOCKET_TP, sSockTp);
   set.SetVal(FDS_FLOW_DIRECTION,   DIRECTION_TO_MGR);
   
   if(sSockTp==DEF_SOCKTP_AUTH)
   {
      set.SetVal(FDS_CODE, CODE_LOGON_AUTH);
      
   }
   else
   {
      set.SetVal(FDS_CODE, CODE_LOGON);
   }
   int nSend = set.Complete(sResultPacket, false);

   bRslt = Send2Svr( sockConn, sSockTp, sResultPacket, nSend, "Conn_Login");
   __PrintDebug(StringFormat("[%s]Sending Login(%s)", sSockTp, sResultPacket));
   
   
   return bRslt;
}