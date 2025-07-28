//+------------------------------------------------------------------+
//|                                                BPMasterLogin.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict

#ifndef __ALPHA_RECVPROC__
#define __ALPHA_RECVPROC__


#include "Common.mqh"
#include "Utils.mqh"
#include "Protocol.mqh"
#include "AlphaErrorCode.mqh"

#define  RETRY_SEND     10
#define  RETRY_TIMEOUT  1000

// try to receive data from server
int RecvFromServer(_Out_ char&  zRecvBuff[], _In_ int nBuffLen, 
                            _Out_ string& sCode, _Out_ int& nRecvLen, _Out_ string& sBuffer)
{
    
   int ret = 0;
   int nConnErrCnt = 0;
   
   while(true)
   {
      ret = __relayEngine.RecvData(zRecvBuff, nBuffLen, nRecvLen);
   
      if( ret==E_TIMEOUT ){
         return E_TIMEOUT;
      }
   
      if(ret==E_OK) {
         break;
      }
      
      __GetErrMsg(ret, sBuffer);
      
      if(ret==E_DISCONN_FROM_SVR || ret==E_NON_CONNECT)
      {
         if( ++nConnErrCnt > RETRY_SEND )
         {
            __relayEngine.DisConnect();
            return E_RECV;
         }
      }

      Sleep(RETRY_TIMEOUT);
      continue;
   }

   sBuffer = CharArrayToString(zRecvBuff);
   ret = E_OK;
   sCode = CProtoUtils::PacketCode(sBuffer);
   if(sCode==CODE_RETURN_ERROR)   //9002
   {
      sCode = CProtoUtils::GetErrCode(sBuffer);
      ret = (int)StringToInteger(sCode);
   }
   
   return ret;
}







#endif