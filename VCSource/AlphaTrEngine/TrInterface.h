#pragma once


#include "../Common/AlphaInc.h"
#include "../Common/Log.h"
CLog	g_log;


#ifdef ALPHA_TR_EXPORTS
#define ALPHA_API extern "C" __declspec(dllexport)
#else 
#define ALPHA_API extern "C" __declspec(dllimport)
#endif


// public
// timeout : millisecond
ALPHA_API int TRAPI_Init(char* pzPath, char* pzServerIp, int nServerPort, _Out_ char* pOutMsg);
ALPHA_API void TRAPI_DeInit();
ALPHA_API void TRAPI_Disconnect();
ALPHA_API int TRAPI_SendData(char* pSendData, int nSendSize, _Out_ char* pzErrMsg);

ALPHA_API int TRAPI_ConfigSymbol(char* pzMCTp, char* pzUserID, char* pzMT4Acc, _Out_ int* pnArrSize, _Out_ char* pArrSymbols, _Out_ char* pzErrMsg);
ALPHA_API int TRAPI_ConfigGeneral(char* pzMCTp, char* pzUserID, char* pzMT4Acc, _Out_ char* pzTotalPacket, _Out_ int* pnPacketSize, _Out_ char* pzErrMsg);

ALPHA_API int TRAPI_OpenOrders_Master(char* pzUserID, char* pzMT4Acc, _Out_ int* pnArrSize, _Out_ char* pArrTickets, _Out_ char* pzErrMsg);

ALPHA_API void TRAPI_GetMsg(char* pzMsg);
ALPHA_API bool TRAPI_IsConnected();
//ALPHA_API void TRAPI_ServerInfo(char* pzServerIp, long nServerPort);
//ALPHA_API bool TRAPI_HasErrHappened(char* pzMsg); // 

bool Get_TrSvrInfo(char* pzDir, _Out_ int* pnSendTimeout, _Out_ int* pnRecvTimeout);
//void GetConfigFileName(_In_ char* pzDir, _Out_ char* pzFileName);
//void RemoveComment(_Out_ char* pzData);
