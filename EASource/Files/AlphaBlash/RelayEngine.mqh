//+------------------------------------------------------------------+
//|                                                  BPSocketDll.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict



#ifndef _RELAY_ENGINE
#define _RELAY_ENGINE


#include "Common.mqh"
#include "Utils.mqh"
#include "AlphaErrorCode.mqh"


// Every return value is same with ALPHA_RESULT
#ifdef __MQL5__ 
#import "AlphaRelayEngine_mt5.dll"
#else
#import "AlphaRelayEngine_mt4.dll"
#endif

   int RELAYAPI_Init(char& zPath[], char& zServerIp[], int nServerPort, int nSendTimeout, int nRecvTimeout);
   int RELAYAPI_DeInit();
   int RELAYAPI_Disconnect();
   int RELAYAPI_Connect();
   int RELAYAPI_RecvData(char& rcvBuf[], int nBuffSize, int& nRecvLen);
   int RELAYAPI_SendData(char& sndBuf[], int nSendSize);
   void RELAYAPI_GetMsg(char& msg[]);
   bool RELAYAPI_IsConnected();
   void RELAYAPI_ServerInfo(char& pzServerIp[], int& pnServerPort);
#import


class CRelayEngine
{
public:
   CRelayEngine();
   ~CRelayEngine();
   
   int  Initialize(string sServerIp, string sServerPort, string sSendTimout, string sRecvTimeout);
   bool  DeInitialize();
   
   void  SetIpPort(string sIp, string sPort);
   int  Connect();
   void  DisConnect();
   
   int      RecvData(char& rcvBuf[], int nBuffSize, int &pnRecvSize);
   int      SendData(char& sndBuf[], int nSendSize);
   int      SendData(string sndBuf, int nSendSize);
   string   GetMsgS(){ return m_sMsg; };
   bool     IsAreadyConnected();
   
private:
   char     m_zMsg[BUF_LEN];
   string   m_sMsg;
   char     m_zServerIp[32];
   int     m_nServerPort;
   bool     m_bDeInited;
};

CRelayEngine::CRelayEngine()
{
   m_bDeInited = false;
}

CRelayEngine::~CRelayEngine()
{
   DeInitialize();
}


void CRelayEngine::SetIpPort(string sIp, string sPort)
{
   StringToCharArray(sIp, m_zServerIp);
   m_nServerPort = (int)StringToInteger(sPort);
}


int  CRelayEngine::Initialize(string sServerIp, string sServerPort, string sSendTimout, string sRecvTimeout)
{
   StringToCharArray(sServerIp, m_zServerIp);
   m_nServerPort = (int)StringToInteger(sServerPort);
   
   char zDllPath[512]; StringToCharArray(DIR_FILE(), zDllPath);
   int ret = RELAYAPI_Init(zDllPath, m_zServerIp, m_nServerPort, (int)StringToInteger(sSendTimout), (int)StringToInteger(sRecvTimeout));
   if(ret!=E_OK)
   { 
      RELAYAPI_GetMsg(m_zMsg);
      StringFormat(m_sMsg, "Failed to Init TcpClientDll:%s",CharArrayToString(m_zMsg));
      return ret;
   }
   return Connect();
}

bool  CRelayEngine::DeInitialize()
{
   if(m_bDeInited==false)
   {
      RELAYAPI_DeInit();
      m_bDeInited = true;
   }
   return true;
}

int  CRelayEngine::Connect()
{
   int ret = RELAYAPI_Connect();
   if(ret!=E_OK)
   {
      RELAYAPI_GetMsg(m_zMsg);
      m_sMsg = StringFormat("Failed to TcpClientDll connect(%s)(%d):%s", 
                           CharArrayToString(m_zServerIp), m_nServerPort, CharArrayToString(m_zMsg));
   }
   return ret;
}

void  CRelayEngine::DisConnect()
{
   RELAYAPI_Disconnect();
}

int CRelayEngine::RecvData(char& rcvBuf[], int nBuffSize, int &pnRecvSize)
{
   // if success, return receiving size. otherwise return code;
   int ret = RELAYAPI_RecvData(rcvBuf, nBuffSize, pnRecvSize);
   
   if( ret!=E_OK && ret!=E_TIMEOUT )
   {
      RELAYAPI_GetMsg(m_zMsg);
      m_sMsg = StringFormat("Failed to TcpClientDll Recv(%d)(%s)",ret, CharArrayToString(m_zMsg));
      //__Debug(m_sMsg);
      //__AlphaAssert(m_sMsg,__FILE__, __LINE__);
   }
   
   return ret;
}

int CRelayEngine::SendData(string sndBuf, int nSendSize)
{
   char zSendBuf[MAX_BUF]={0};
   StringToCharArray(sndBuf, zSendBuf);
   return SendData(zSendBuf, nSendSize);
}

int CRelayEngine::SendData(char& sndBuf[], int nSendSize)
{
   int ret = RELAYAPI_SendData(sndBuf, nSendSize);
   if( ret!=E_OK)
   {
      RELAYAPI_GetMsg(m_zMsg);
      m_sMsg = StringFormat("Failed to Relay Send(%s)", CharArrayToString(m_zMsg));
   }
   return ret;
}


bool  CRelayEngine::IsAreadyConnected()
{
   return RELAYAPI_IsConnected();
}

#endif

