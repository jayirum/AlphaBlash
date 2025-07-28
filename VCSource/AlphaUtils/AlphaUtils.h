#pragma once

#ifdef _ALPHA_UTILS_EXPORTS
#define ALPHA_UTILS extern "C" __declspec(dllexport)
#else
#define ALPHA_UTILS extern "C" __declspec(dllimport)
#endif

#include "../Common/AlphaInc.h"
//#include <string>
using namespace std;



//+------------------------------------------------------------+
//	DB
//+------------------------------------------------------------+

ALPHA_UTILS	int	Alpha_DBOpen(char* pzSvrIpPort, char* pzUser, char* pzPwd, char* pzDBName, _Out_ char* pzMsg);
ALPHA_UTILS	int	Alpha_DBExec(char* pzQ, _Out_ char* pzMsg);
ALPHA_UTILS	int	Alpha_DBClose();




//+------------------------------------------------------------+
//	Utility
//+------------------------------------------------------------+

ALPHA_UTILS void AlphaUtils_OpenHomepage(_In_ char* pzWebsiteUrl);



//+------------------------------------------------------------+
//	AlphaBlash.ini
//+------------------------------------------------------------+
//ALPHA_UTILS int AlphaUtils_Get_TimeoutSendRecv(_In_ char* pzDir, _Out_ int* pnRecvTimeout, _Out_ int* pnsendTimeout);
//ALPHA_UTILS int AlphaUtils_Get_MT4OrderRetry(_In_ char* pzDir, char* pzIniFile, _Out_ int* pnRetryCnt, _Out_ int* pnRetrySleep);

ALPHA_UTILS int AlphaUtils_Get_Msg(_In_ char* pzDir, _In_ char* pzErrCode, _Out_ wchar_t* pwzMsg);
ALPHA_UTILS int AlphaUtils_Get_MsgEx(_In_ char* pzDir, _In_ char* pzErrCode, _In_ char* pzLanguage,  _Out_ wchar_t* pwzMsg);
//ALPHA_UTILS int AlphaUtils_Get_SleepMS(_In_ char* pzDir, char* pzIniFile, _Out_ int* pnMili);
