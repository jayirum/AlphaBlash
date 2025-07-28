#pragma once


#include "../Common/AlphaInc.h"


#ifdef ALPHA_LOGIN_EXPORTS
#define ALPHA_API extern "C" __declspec(dllexport)
#else 
#define ALPHA_API extern "C" __declspec(dllimport)
#endif


// public
// timeout : millisecond
ALPHA_API  int LoginAPI_Init(char* pzConfigDir, _Out_ char* pzErrMsg);
ALPHA_API void LoginAPI_DeInit();
ALPHA_API void LoginAPI_Disconnect();
ALPHA_API int LoginAPI_SendData(char* pSendData, int nSendSize, _Out_ char* pzErrMsg);
ALPHA_API int LoginAPI_UniTest(_Out_ wchar_t* pwzMsg);

ALPHA_API int LoginAPI_Login_Master(char* pzUserID, char* pzPwd, char* pzMCTp, char* pzMT4Acc, char* pzBroker, char* pzLiveDemo,
	_Out_ char* pzRelayIp, _Out_ int* pnRelayPort, _Out_ int* pnTrPort, _Out_ char* pzNickName, _Out_ char* pzWebUrl,
	_Out_ char* pzErrMsg);

ALPHA_API int LoginAPI_Login_Copier(char* pzUserID, char* pzPwd, char* pzMCTp, char* pzMT4Acc, char* pzBroker, char* pzLiveDemo,
	_Out_ char* pzRelayIp, _Out_ int* pnRelayPort, _Out_ int* pnTrPort, _Out_ char* pzNickName,
	_Out_ char* pzMasterID, _Out_ char* pzMasterAcc, _Out_ char* pzWebUrl,
	_Out_ char* pzErrMsg);

ALPHA_API void LoginAPI_GetMsg(char* pzMsg);
ALPHA_API bool LoginAPI_IsConnected();

bool Get_SvrInfo(char* pzDir, _Out_ wchar_t* pzIp, _Out_ int* pnPort, _Out_ int* pnSendTimeout, _Out_ int* pnRecvTimeout);
