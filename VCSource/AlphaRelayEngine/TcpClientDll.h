#pragma once


#include "../Common/AlphaInc.h"


#ifdef ALPHA_API_EXPORTS
#define ALPHA_API extern "C" __declspec(dllexport)
#else 
#define ALPHA_API extern "C" __declspec(dllimport)
#endif



// timeout : millisecond
ALPHA_API int RELAYAPI_Init(char* pzPath, char* pzServerIp, int nServerPort, int nSendTimeout, int nRecvTimeout);
ALPHA_API int RELAYAPI_DeInit();
ALPHA_API int RELAYAPI_Disconnect();
ALPHA_API int RELAYAPI_Connect();
ALPHA_API int RELAYAPI_RecvData(char* pRecvData, int nBuffSize, int* nRecvLen);
ALPHA_API int RELAYAPI_SendData(char* pSendData, int nSendSize);
ALPHA_API void RELAYAPI_GetMsg(char* pzMsg);
ALPHA_API bool RELAYAPI_IsConnected();
ALPHA_API void RELAYAPI_ServerInfo(char* pzServerIp, int* pnServerPort);
int CreateSock();
void DumpErr(char* pSrc);

bool Get_SvrInfo(char* pzDir, char* pzIniFile, _Out_ int* pnSendTimeout, _Out_ int* pnRecvTimeout);
