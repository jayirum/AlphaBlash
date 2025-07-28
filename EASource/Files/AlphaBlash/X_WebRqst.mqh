//+------------------------------------------------------------------+
//|                                                  BPSocketDll.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict



#ifndef __ALPHA_WEBRQST__
#define __ALPHA_WEBRQST__


#include "Utils.mqh"

#define  WEBEMUL_SUCCESS	0
#define  WEBEMUL_FAIL		-1


bool  WebMasterLogin(string sLogpath, string sUserId, string sPwd, string sAcntNo, _Out_ string& sServerIp, _Out_ string& sServerPort, _Out_ string& sTrPort)
{
    sServerIp = "110.4.89.206";
    sServerPort = "32132";
    sTrPort = "43214";
    
    return true;
    

   char zUserId[32];    StringToCharArray(sUserId,  zUserId);
   char zPwd[32];       StringToCharArray(sPwd,     zPwd);
   char zAcntNo[32];    StringToCharArray(sAcntNo,  zAcntNo);
   char zLogPath[512];  StringToCharArray(sLogpath, zLogPath);
   char zServerIp[32];
   char zServerPort[32];
   
   //TODO
//   int ret = WEBEMUL_LoginMaster(zLogPath, zUserId, zPwd, zAcntNo, zServerIp, zServerPort);
//   if( ret!=WEBEMUL_SUCCESS )
//   {
//      char zMsg[1024];
//      WEBEMUL_GetMsg(zMsg);
//      printlog(StringFormat("Failed to LoginMaster:%s",CharArrayToString(zMsg)), true);
//   }
//   
//   sServerIp   = CharArrayToString(zServerIp);
//   sServerPort = CharArrayToString(zServerPort);
//   printlog(StringFormat("Succeed in WebMasterLogin(SvrIP:%s)(SvrPort:%s)",sServerIp, sServerPort), false);
   return true;
}



bool  WebSlaveLogin(string sLogpath, string sUserId, string sPwd, string sAcntNo, 
        _Out_ string& sMasterId, _Out_ string& sMasterAcntNo, _Out_ string& sMasterNickNm, 
        _Out_ string& sServerIp, _Out_ string& sServerPort, _Out_ string& sTrPort)
{
   char zUserId      [32]; StringToCharArray(sUserId, zUserId);
   char zPwd         [32]; StringToCharArray(sPwd, zPwd);
   char zAcntNo      [32]; StringToCharArray(sAcntNo, zAcntNo);
   char zLogPath     [512];StringToCharArray(sLogpath, zLogPath);
   char zServerIp    [32];
   char zServerPort  [32];
   char zMasterId    [32];
   char zMasterAcntNo[32];
   char zMasterNickNm[32];
   
   sServerIp = "110.4.89.206";
   sServerPort = "32132";
   sTrPort = "43214";
    
   sMasterId = "MASTER1";
   sMasterAcntNo = "5108760";
   sMasterNickNm = "NickMaster1";
   
   //TODO printlog(StringFormat("try to LoginSlave.%s, %s, %s", sLogpath, sUserId, sPwd));
//   int ret = WEBEMUL_LoginSlave(zLogPath, zUserId, zPwd, zAcntNo, zMasterId, zMasterAcntNo, zMasterNickNm, zServerIp, zServerPort);
//   if( ret!=WEBEMUL_SUCCESS )
//   {
//      char zMsg[1024];
//      WEBEMUL_GetMsg(zMsg);
//      printlog(StringFormat("Failed to LoginSlave(%s)(%s):%s", sUserId, sPwd, CharArrayToString(zMsg)), true);
//   }
//   
//   sMasterId      = CharArrayToString(zMasterId);
//   sMasterAcntNo  = CharArrayToString(zMasterAcntNo);
//   sMasterNickNm  = CharArrayToString(zMasterNickNm);
//   sServerIp      = CharArrayToString(zServerIp);
//   sServerPort    = CharArrayToString(zServerPort);
//   printlog(StringFormat("Succeed in LoginSlave(MasterID:%s)(MasterAcntNo:%s)(MasterNickNm:%s)(SvrIP:%s)(SvrPort:%s)",
//                           sMasterId, sMasterAcntNo, sMasterNickNm, sServerIp, sServerPort), false);
   return true;
}


#endif

