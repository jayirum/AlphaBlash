#pragma once


#include "../Common/AlphaInc.h"


#ifdef ALPHA_TCP_EXPORTS
#define ALPHA_TCP extern "C" __declspec(dllexport)
#else 
#define ALPHA_TCP extern "C" __declspec(dllimport)
#endif



	// timeout : millisecond
	ALPHA_TCP int AlphaTcp_Init(char* pzPath, _Out_ int* pnIdx );
	ALPHA_TCP int AlphaTcp_InitWithInfo(char* pzConfigFullName, char* pzSeverIP, int nPort, int nSendTimeOut, int nRecvTimeOut, _Out_ int* pnIdx);
	ALPHA_TCP int AlphaTcp_DeInit(int idx);
	ALPHA_TCP int AlphaTcp_Disconnect(int idx);
	ALPHA_TCP int AlphaTcp_Connect(int idx);
	ALPHA_TCP int AlphaTcp_RecvData(int idx, char* pRecvData, int nBuffSize, int* nRecvLen);
	ALPHA_TCP int AlphaTcp_SendData(int idx, char* pSendData, int nSendSize);
	ALPHA_TCP void AlphaTcp_GetLastMsg(char* pzMsg);
//ALPHA_TCP bool AlphaTcp_IsConnected(int idx);
//ALPHA_TCP void AlphaTcp_ServerInfo(char* pzServerIp, int* pnServerPort);
//int CreateSock(int idx);
void DumpErr(char* pSrc);

bool Get_SvrInfo(char* pzDir, char* pSockInfo);
