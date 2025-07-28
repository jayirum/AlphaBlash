//+------------------------------------------------------------------+
//|                                                  BPSocketDll.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict



#ifndef _TR_ENGINE
#define _TR_ENGINE


#include "Common.mqh"
#include "Utils.mqh"
#include "AlphaErrorCode.mqh"
#include "Protocol.mqh"


// Every return value is same with ALPHA_RESULT
#ifdef __MQL5__ 
#import "AlphaTrEngine_mt5.dll"
#else
#import "AlphaTrEngine_mt4.dll"
#endif


   int TRAPI_Init(_In_ char& zPath[], _In_ char& zServerIp[], _In_ int nServerPort, _Out_ char& pzMsg[]);
   void TRAPI_DeInit();
   void TRAPI_Disconnect();
   
   int TRAPI_SendData(_In_ char& sndBuf[], _In_ int nSendSize, _Out_ char& zErrMsg[]);
   
//   int TRAPI_Login_Master(char& pzUserID[], char& pzPwd[], char& pzMCTp[],  char& pzAccMine[], char& pzBroker[],char& pzLiveDemo[],
//							_Out_ char& pzRelayIp[], _Out_ int& pnRelayPort, 
//							_Out_ char& pzNickName[], _Out_ char& pzWebUrl[], _Out_ char& pzErrMsg[]);
//   
//   int TRAPI_Login_Copier(char& pzUserID[], char& pzPwd[], char& pzMCTp[],  char& pzAccMine[], char& pzBroker[],char& pzLiveDemo[],
//							_Out_ char& pzRelayIp[], _Out_ int& pnRelayPort, _Out_ char& pzNickName[], 
//							_Out_ char& pzMasterID[], _Out_ char& pzMasterAcc[], _Out_ char& pzWebUrl[], _Out_ char& pzErrMsg[]);
							
   int TRAPI_ConfigSymbol(char& pzMCTp[], char& pzUserID[], char& pzAccMine[], 
                     _Out_ int& pnArrSize, _Out_ char& pzArrSymbols[],_Out_ char& pzErrMsg[]);

   int TRAPI_ConfigGeneral(char& pzMCTp[], char& pzUserID[], char& pzAccMine[], 
                     _Out_ char& pzRecvPacket[], _Out_ int& pnPacketSize,_Out_ char& pzErrMsg[]);


   int TRAPI_OpenOrders_Master(char& pzUserID[], char& pzAccMine[], 
                     _Out_ int& pnArrSize, _Out_ char& pzArrTickets[],_Out_ char& pzErrMsg[]);


   void TRAPI_GetMsg(_Out_ char& msg[]);
   bool TRAPI_IsConnected();
#import


class CTrEngine
{
public:
   CTrEngine();
   ~CTrEngine();
   
   int  Initialize(string sServerIp, int nServerPort);
   void  DeInitialize();
   void  DisConnect();
//   int  Login_Master(string sID, string sPwd, string sAccMine, string sBroker, string sLiveDemo,
//            _Out_ string& psRelayIp, _Out_ int& pnRelayPort, _Out_ string& psNickName, _Out_ string &psWebUrl);
//            
//   int  Login_Copier(string sID, string sPwd, string sAccMine, string sBroker, string sLiveDemo,
//            _Out_ string& psRelayIp, _Out_ int& pnRelayPort, _Out_ string& psNickName, _Out_ string& psMasterID, 
//            _Out_ string& psMasterAcc, _Out_ string &psWebUrl);

   int  Request_ConfigSymbol_Master(string sID, string sAccMine, int& nArrSize, _Out_ string& psArrSymbols);
   int  Request_ConfigSymbol_Copier(string sID, string sAccMine, int& nArrSize, _Out_ string& psArrSymbols);

   int  Request_ConfigGeneral_Master(string sID, string sAccMine, _Out_ string& psRecvpacket, _Out_ int& pnRecvLen);
   int  Request_ConfigGeneral_Copier(string sID, string sAccMine, _Out_ string& psRecvpacket, _Out_ int& pnRecvLen);

   int  Request_OpenOrders_Master(string sID, string sAccMine, int& nArrSize, _Out_ string& psArrTickets);

   int     SendData(char& sndBuf[], int nSendSize);
   string   GetMsgS(){ return m_sMsg; };
   bool     IsAreadyConnected();
   void     SetIpPort(string sIp, string sPort);
private:
   char     m_zMsg[BUF_LEN];
   string   m_sMsg;
   string   m_sServerIp;
   string   m_sServerPort;
   bool     m_bDeInited;
};

CTrEngine::CTrEngine()
{
   m_bDeInited = false;
}

CTrEngine::~CTrEngine()
{
   DeInitialize();
}



void CTrEngine::SetIpPort(string sIp, string sPort)
{
   m_sServerIp = sIp;
   m_sServerPort = sPort;
}

int  CTrEngine::Initialize(string sServerIp, int nServerPort)
{
   char zDllPath[512]; StringToCharArray(DIR_FILE, zDllPath);
   
   char zServerIp[32];
   StringToCharArray(sServerIp, zServerIp);
   
   int nRet = TRAPI_Init(zDllPath, zServerIp, nServerPort, m_zMsg);
   if(nRet!=E_OK)
   {
      m_sMsg = StringFormat( "Failed to Init TrEngine:%s",CharArrayToString(m_zMsg));
      return nRet;
   }
   m_sMsg = CharArrayToString(m_zMsg);
   return E_OK;
}

void  CTrEngine::DeInitialize()
{
   if(m_bDeInited==false)
   {
      TRAPI_DeInit();
      m_bDeInited = true;
   }
}


void  CTrEngine::DisConnect()
{
   TRAPI_Disconnect();
}


int CTrEngine::SendData(char& sndBuf[], int nSendSize)
{
    int ret = TRAPI_SendData(sndBuf, nSendSize, m_zMsg);
    if( ret!=E_OK )
    {
        m_sMsg = StringFormat( "Failed to SendData:%s",CharArrayToString(m_zMsg));
    }
    return ret;
}

   
//int  CTrEngine::Login_Master(string sID, string sPwd,string sAccMine, string sBroker,string sLiveDemo,
//                     _Out_ string& psRelayIp,    _Out_ int& pnRelayPort, _Out_ string& psNickName, _Out_ string &psWebUrl)
//{
//    char zUserId[32], zPwd[32], zIp[32], zMCTp[32], zAccMine[32], zNickNm[32], zBroker[128], zLiveDemo[32], zWebUrl[128];
//    StringToCharArray(sID, zUserId);
//    StringToCharArray(sPwd, zPwd);
//    StringToCharArray("M", zMCTp);
//    StringToCharArray(sAccMine, zAccMine);
//    StringToCharArray(sBroker, zBroker);
//    StringToCharArray(sLiveDemo, zLiveDemo);
//    
//    //int TRAPI_Login_Master(char& pzUserID[], char& pzPwd[], char& pzMCTp[],  char& pzAccMine[], char& pzBroker[],char& pzLiveDemo[],
//				//			_Out_ char& pzRelayIp[], _Out_ int& pnRelayPort, 
//				//			_Out_ char& pzNickName[], _Out_ char& pzAutoKey[], _Out_ char& pzErrMsg[]);
//   
//    int ret = TRAPI_Login_Master(zUserId, zPwd, zMCTp, zAccMine, zBroker,zLiveDemo,
//							zIp, pnRelayPort, zNickNm, zWebUrl, m_zMsg);
//     if( ret!=E_OK )
//    {
//        m_sMsg = StringFormat( "Failed to Login:%s",CharArrayToString(m_zMsg));
//        return ret;
//    }
//    
//    psRelayIp = CharArrayToString(zIp);
//    psNickName = CharArrayToString(zNickNm);
//    psWebUrl = CharArrayToString(zWebUrl);
//    
//    return ret;
//}
//
//
//int  CTrEngine::Login_Copier(string sID,string sPwd, string sAccMine,string sBroker,string sLiveDemo,
//                              string &psRelayIp,int &pnRelayPort,string &psNickName,string &psMasterID,string &psMasterAcc, _Out_ string &psWebUrl)
//{
//   char zUserId[32], zPwd[32], zIp[32], zMCTp[32], zAccMine[32], zBroker[128], zLiveDemo[32];
//   char zNickNm[32], zMasterID[32], zMasterAcc[32], zWebUrl[128];
//   StringToCharArray(sID, zUserId);
//   StringToCharArray(sPwd, zPwd);
//   StringToCharArray("C", zMCTp);
//   StringToCharArray(sAccMine, zAccMine);
//   StringToCharArray(sBroker, zBroker);
//   StringToCharArray(sLiveDemo, zLiveDemo);
//    
//   int ret = TRAPI_Login_Copier(zUserId, zPwd, zMCTp, zAccMine, zBroker,zLiveDemo,
//							zIp, pnRelayPort, zNickNm, zMasterID, zMasterAcc, zWebUrl, m_zMsg);
//   if( ret!=E_OK )
//   {
//      GetErrMsg(ret, m_sMsg);
//      return ret;
//   }
//    
//   psRelayIp = CharArrayToString(zIp);
//   psNickName = CharArrayToString(zNickNm);
//   psMasterID = CharArrayToString(zMasterID);
//   psMasterAcc = CharArrayToString(zMasterAcc);
//   psWebUrl = CharArrayToString(zWebUrl);
//   
//   return ret;
//}

int  CTrEngine::Request_ConfigSymbol_Master(string sID,string sAccMine,int &nArrSize,string &psArrSymbols)
{
    char zUserId[32], zAccMine[32];
    char zArrSymbols[512], zMCTp[32];
    StringToCharArray(sID, zUserId);
    StringToCharArray(sAccMine, zAccMine);
    StringToCharArray(MC_TP_MASTER, zMCTp);
    
    int ret = TRAPI_ConfigSymbol(zMCTp, zUserId, zAccMine, nArrSize, zArrSymbols, m_zMsg);
     if( ret!=E_OK )
    {
        m_sMsg = StringFormat( "Failed to TRAPI_ConfigSymbol:%s",CharArrayToString(m_zMsg));
        Alert(m_sMsg);
        return ret;
    }
    
    psArrSymbols = CharArrayToString(zArrSymbols);
    
    return ret;
}


int  CTrEngine::Request_ConfigSymbol_Copier(string sID,string sAccMine,int &nArrSize,string &psArrSymbols)
{
   char zUserId[32], zAccMine[32];
   char zArrSymbols[512], zMCTp[32];
   StringToCharArray(sID, zUserId);
   StringToCharArray(sAccMine, zAccMine);
   StringToCharArray(MC_TP_COPIER, zMCTp);
    
   int ret = TRAPI_ConfigSymbol(zMCTp, zUserId, zAccMine, nArrSize, zArrSymbols, m_zMsg);
   if( ret!=E_OK )
   {
      GetErrMsg(ret, m_sMsg);  
      //  m_sMsg = StringFormat( "Failed to TRAPI_ConfigSymbol:%s",CharArrayToString(m_zMsg));
      //  Alert(m_sMsg);
      return ret;
   }
    
   psArrSymbols = CharArrayToString(zArrSymbols);
   return ret;
}

                     
                     
int  CTrEngine::Request_ConfigGeneral_Copier(string sID,string sAccMine, _Out_ string& psRecvpacket, _Out_ int& pnRecvLen)
{
   char zUserId[32], zAccMine[32];
   char zRecvPacket[512], zMCTp[32];
   StringToCharArray(sID, zUserId);
   StringToCharArray(sAccMine, zAccMine);
   StringToCharArray(MC_TP_COPIER, zMCTp);
   
   int ret = TRAPI_ConfigGeneral(zMCTp, zUserId, zAccMine, zRecvPacket, pnRecvLen, m_zMsg);
   if( ret!=E_OK )
   {
      GetErrMsg(ret, m_sMsg);  
        ///m_sMsg = StringFormat( "Failed to TRAPI_ConfigGeneral:%s",CharArrayToString(m_zMsg));
        //Alert(m_sMsg);
        return ret;
   }
    
   psRecvpacket = CharArrayToString(zRecvPacket);
   PrintFormat("TRAPI_ConfigGeneral:", psRecvpacket);
 return ret;
}


int  CTrEngine::Request_ConfigGeneral_Master(string sID,string sAccMine, _Out_ string& psRecvpacket, _Out_ int& pnRecvLen)
{
   AssertEx("No implementation for Master config",__FILE__, __LINE__, false, false);
   return E_OK;
}

int  CTrEngine::Request_OpenOrders_Master(string sID,string sAccMine, _Out_ int &nArrSize, _Out_ string &psArrTickets)
{
    char zUserId[32], zAccMine[32];
    char zArrTickets[512];
    StringToCharArray(sID, zUserId);
    StringToCharArray(sAccMine, zAccMine);
    
    int ret = TRAPI_OpenOrders_Master(zUserId, zAccMine, nArrSize, zArrTickets, m_zMsg);
     if( ret!=E_OK )
    {
        m_sMsg = StringFormat( "Failed to Request_OpenOrders_Master:%s",CharArrayToString(m_zMsg));
        Alert(m_sMsg);
        return ret;
    }
    
    if(nArrSize>0)
      psArrTickets = CharArrayToString(zArrTickets);
    return ret;
}



bool  CTrEngine::IsAreadyConnected()
{
   return TRAPI_IsConnected();
}

#endif

