#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define ALPHA_API_EXPORTS


#include "TcpClientDll.h"
#include "../Common/AlphaInc.h"
#include "../Common/Util.h"
#include "../Common/AlphaProtocolUni.h"


#define DEF_SEND_RETRY	3
#define DEF_BUF_SIZE	1024


struct SOCK_INFO
{
	SOCKET		sock;
	WSAEVENT	hwsa;
	bool		bConn;
	char		msg[DEF_BUF_SIZE];
	char		zServerIP[128];
	int			nServerPort;
	int			nRecvTimeout;
	int			nSendTimeout;
	SOCKADDR_IN		sin;
};
SOCK_INFO	g_sock;


//bool Get_SvrInfo(char* pzDir, char* pzIniFile, _Out_ int* pnSendTimeout, _Out_ int* pnRecvTimeout)
//{
//	wchar_t zFullName[_MAX_PATH] = { 0, };
//	wchar_t zFileName[_MAX_PATH] = { 0, };
//	wchar_t wzDir[1024] = { 0, };
//	A2U(pzDir, wzDir);
//	A2U(pzDir, zFileName);
//
//	if (pzDir[_tcslen(wzDir) - 1] == '\\')
//		_stprintf(zFullName, TEXT("%s%s"), wzDir, zFileName);
//	else
//		_stprintf(zFullName, TEXT("%s\\%s"), wzDir, zFileName);
//
//	
//	//__ALPHA::ComposeEAConfigFileName(pzDir, zFileName);
//
//	wchar_t wzVal[128] = { 0, };
//
//	// SENDTIMEOUT
//	ZeroMemory(wzVal, sizeof(wzVal));
//	if (CUtil::GetConfig(zFileName, TEXT("RELAYSERVER_INFO"), TEXT("SENDTIMEOUT"), wzVal) == NULL)	
//	{
//		return false;
//	}
//	*pnSendTimeout = _ttoi(wzVal);
//
//	// RECVTIMEOUT
//	ZeroMemory(wzVal, sizeof(wzVal));
//	if (CUtil::GetConfig(zFileName, TEXT("RELAYSERVER_INFO"), TEXT("RECVTIMEOUT"), wzVal) == NULL)
//	{
//		return false;
//	}
//	*pnRecvTimeout = _ttoi(wzVal);
//
//
//	return true;
//}


int RELAYAPI_Init(char* pzPath, char* pzServerIp, int nServerPort, int nSendTimeout, int nRecvTimeout)
{
	ZeroMemory(&g_sock, sizeof(g_sock));
	strcpy(g_sock.zServerIP, pzServerIp);
	g_sock.nServerPort = nServerPort;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		sprintf(g_sock.msg, "WSAStartup error:%d", GetLastError());
		return E_WSA_STARTUP;
	}

	g_sock.sin.sin_family = AF_INET;
	g_sock.sin.sin_port = htons(nServerPort);
	g_sock.sin.sin_addr.s_addr = inet_addr(pzServerIp);

	g_sock.nSendTimeout = nSendTimeout;
	g_sock.nRecvTimeout = nRecvTimeout;
	

	//Get_SvrInfo(pzPath, pzIniFile, &g_sock.nRecvTimeout, &g_sock.nSendTimeout);
	return CreateSock();
}

int CreateSock()
{
	g_sock.sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock.sock == INVALID_SOCKET)
	{
		DumpErr("create socket");
		return E_CREATE_SOCK;
	}
	
	setsockopt(g_sock.sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&g_sock.nSendTimeout, sizeof(g_sock.nSendTimeout));
	setsockopt(g_sock.sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_sock.nRecvTimeout, sizeof(g_sock.nRecvTimeout));
	return ERR_OK;
}


bool RELAYAPI_IsConnected()
{
	return g_sock.bConn;
}

int RELAYAPI_DeInit()
{
	RELAYAPI_Disconnect();
	return ERR_OK;
}

int RELAYAPI_Disconnect()
{
	if (g_sock.sock != INVALID_SOCKET) {
		struct linger ling;
		ling.l_onoff = 1;   // 0 ? use default, 1 ? use new value
		ling.l_linger = 0;  // close session in this time
		setsockopt(g_sock.sock, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));
		//-We can avoid TIME_WAIT on both of client and server side as we code above.
		closesocket(g_sock.sock);
		g_sock.sock = INVALID_SOCKET;
	}
	if (g_sock.hwsa) {
		WSACloseEvent(g_sock.hwsa);
		g_sock.hwsa = NULL;
	}
	
	g_sock.bConn	= false;
	return ERR_OK;
}

int RELAYAPI_Connect()
{
	int nRet = 0;
	if (g_sock.sock == INVALID_SOCKET) 
	{
		if ((nRet=CreateSock()) < 0)
			return nRet;
	}


	if (connect(g_sock.sock, (LPSOCKADDR)&g_sock.sin, sizeof(g_sock.sin)) == SOCKET_ERROR)
	{
		g_sock.bConn = FALSE;
		DumpErr("connect");
		RELAYAPI_Disconnect();
		return E_CONNECT;
	}

	g_sock.hwsa = WSACreateEvent();
	if (WSAEventSelect(g_sock.sock, g_sock.hwsa, FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		g_sock.bConn = FALSE;
		DumpErr("WSAEventSelect");
		RELAYAPI_Disconnect();
		return E_CONNECT;
	}

	sprintf(g_sock.msg, "connect ok(%s)(%d)", g_sock.zServerIP, g_sock.nServerPort);
	g_sock.bConn = true;
	return ERR_OK;
}

int RELAYAPI_RecvData(char* pRecvData, int nBuffSize, int* nRecvLen)
{
	*nRecvLen = 0;
	if (!RELAYAPI_IsConnected()) {
		sprintf(g_sock.msg, "[RecvData]socket is not connected!");
		return E_NON_CONNECT;
	}

	DWORD dwRet = WSAWaitForMultipleEvents(1, &g_sock.hwsa, FALSE, g_sock.nRecvTimeout, TRUE);
	if (dwRet != WAIT_OBJECT_0)
	{
		if (dwRet == WAIT_TIMEOUT)
		{
			return E_TIMEOUT;
		}
		else 
		{
			DumpErr("WSAWaitForMultipleEvents");
			return E_RECV;
		}
	}

	//		LOCK_RECV();

	WSAResetEvent(g_sock.hwsa);

	WSANETWORKEVENTS enumEvent;
	if (WSAEnumNetworkEvents(g_sock.sock, g_sock.hwsa, &enumEvent) == SOCKET_ERROR)
	{
		DumpErr( "WSAEnumNetworkEvents Err");
		return E_RECV;
	}

	//	SMILOR MODIFY 2004-12-08
	if ((enumEvent.lNetworkEvents == FD_CLOSE) ||
		(enumEvent.lNetworkEvents == (FD_CLOSE | FD_READ)))
	{
		g_sock.bConn = FALSE;
		sprintf(g_sock.msg, "Receive Close Event");
		return E_DISCONN_FROM_SVR;
	}

	char zRecvBuff[MAX_BUF] = { 0, };

	long nRet = recv(g_sock.sock, pRecvData, nBuffSize, 0);
	if (nRet == SOCKET_ERROR)
	{	
		long nErr = GetLastError();
		if (nErr != WSAEWOULDBLOCK)
		{
			//TODO DumpErr("recv");
			sprintf(g_sock.msg, "recv error(%d)", nErr);
			return E_RECV;
		}
	}
	else if (nRet == 0) {
		sprintf(g_sock.msg, "recv 0 byte");
		return E_RECV;
	}
	
	return ERR_OK;
}


int RELAYAPI_SendData(char* pSendData, int nSendSize)
{
	if (!RELAYAPI_IsConnected()) {
		sprintf(g_sock.msg, "[SendData]socket is not connected!");
		return E_NON_CONNECT;
	}
	int Ret;
	int nRetryCnt = 0, nRetryBlock = 0;

	while (1)
	{
		Ret = send(g_sock.sock, pSendData, nSendSize, 0);
		if (Ret > 0) {
			return ERR_OK;
		}

		//	if(Ret == SOCKET_ERROR)		
		int nErr = GetLastError();
		if (nErr == WSAETIMEDOUT)
		{
			if (++nRetryCnt > DEF_SEND_RETRY)
			{
				sprintf(g_sock.msg, "[SendData]WSAETIMEDOUT %d회 반복으로 에러 리턴", nRetryCnt);
				return E_SEND;
			}
			continue;
		}
		else if (nErr == WSAEWOULDBLOCK)
		{
			if (++nRetryBlock > DEF_SEND_RETRY)
			{
				sprintf(g_sock.msg, "[SendData]WSAEWOULDBLOCK %d회 반복으로 에러 리턴", nRetryBlock);
				return E_SEND;
			}
			continue;
		}
		else
		{
			DumpErr("send");
			return E_SEND;
		}
	}

	return E_SEND;
}



void RELAYAPI_ServerInfo(char* pzServerIp, int* pnServerPort)
{
	strcpy(pzServerIp, g_sock.zServerIP);
	*pnServerPort = g_sock.nServerPort;
}


void RELAYAPI_GetMsg(char* pzMsg)
{
	pzMsg[0] = 0;
	strcpy(pzMsg, g_sock.msg);
}


void DumpErr(char* pSrc)
{
	int nErr = GetLastError();
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		nErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	ZeroMemory(g_sock.msg, sizeof(g_sock.msg));
	sprintf(g_sock.msg, "[%s](%d)%s", pSrc, nErr,(char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}