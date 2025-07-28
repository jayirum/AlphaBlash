#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define ALPHA_TCP_EXPORTS


#include "TcpClientDll.h"
#include "../Common/AlphaInc.h"
#include "../Common/Util.h"
#include <map>
using namespace std;

#define DEF_IDX	INT
#define DEF_SEND_RETRY	3
#define DEF_BUF_SIZE	1024


struct SOCK_INFO
{
	SOCKET		sock;
	WSAEVENT	hwsa;
	bool		bConn;
	char		zServerIP[128];
	int			nServerPort;
	int			nRecvTimeout;
	int			nSendTimeout;
	SOCKADDR_IN		sin;
};
map<DEF_IDX, SOCK_INFO*>	g_sock;
long						g_sockCnt = 0;
char						g_msg[DEF_BUF_SIZE];

SOCK_INFO* Sock(int idx)
{
	map<DEF_IDX, SOCK_INFO*>::iterator it = g_sock.find(idx);
	if (it == g_sock.end())
		return NULL;
	return (*it).second;
}

bool Get_SvrInfo(char* pzDir, char* pSockInfo)
{
	TCHAR zFileName[_MAX_PATH] = { 0, };
	__ALPHA::ComposeEAConfigFileName(pzDir, zFileName);


	SOCK_INFO* p = (SOCK_INFO*)pSockInfo;

	TCHAR wzVal[128] = { 0, };

	// IP
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("TWOWAY_INFO"), TEXT("SVR_IP"), wzVal) == NULL)
	{
		return false;
	}
	U2A(wzVal, p->zServerIP);

	// Port
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("TWOWAY_INFO"), TEXT("SVR_PORT"), wzVal) == NULL)
	{
		return false;
	}
	p->nServerPort = _ttoi(wzVal);

	// SENDTIMEOUT
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("TWOWAY_INFO"), TEXT("SENDTIMEOUT"), wzVal) == NULL)
	{
		return false;
	}
	p->nSendTimeout = _ttoi(wzVal);

	// RECVTIMEOUT
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("TWOWAY_INFO"), TEXT("RECVTIMEOUT"), wzVal) == NULL)
	{
		return false;
	}
	p->nRecvTimeout = _ttoi(wzVal);


	return true;
}


int AlphaTcp_Init(char* pzPath, _Out_ int* pnIdx)
{
	INT idx = g_sockCnt;

	SOCK_INFO* p = new SOCK_INFO;
	ZeroMemory(p, sizeof(SOCK_INFO));

	if (Get_SvrInfo(pzPath,(char*)p) == false)
	{
		delete p;
		return E_READ_CONFIG;
	}

	if (idx == 0)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			return E_WSA_STARTUP;
		}
	}

	p->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (p->sock == INVALID_SOCKET)
	{
		return E_CREATE_SOCK;
	}

	setsockopt(p->sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&p->nSendTimeout, sizeof(p->nSendTimeout));
	setsockopt(p->sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&p->nRecvTimeout, sizeof(p->nRecvTimeout));

	p->sin.sin_family = AF_INET;
	p->sin.sin_port = htons(p->nServerPort);
	p->sin.sin_addr.s_addr = inet_addr(p->zServerIP);

	*pnIdx = idx;
	g_sockCnt++;
	g_sock[idx] = p;
	
	return E_OK;
}



int AlphaTcp_InitWithInfo(char* pzConfigFullName, char* pzSeverIP, int nPort, int nSendTimeOut, int nRecvTimeOut, _Out_ int* pnIdx)
{
	INT idx = g_sockCnt;

	SOCK_INFO* p = new SOCK_INFO;
	ZeroMemory(p, sizeof(SOCK_INFO));

	strcpy(p->zServerIP, pzSeverIP);
	p->nServerPort = nPort;
	p->nSendTimeout = nSendTimeOut;
	p->nRecvTimeout = nRecvTimeOut;

	if (idx == 0)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			return E_WSA_STARTUP;
		}
	}

	p->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (p->sock == INVALID_SOCKET)
	{
		return E_CREATE_SOCK;
	}

	setsockopt(p->sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&p->nSendTimeout, sizeof(p->nSendTimeout));
	setsockopt(p->sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&p->nRecvTimeout, sizeof(p->nRecvTimeout));

	p->sin.sin_family = AF_INET;
	p->sin.sin_port = htons(p->nServerPort);
	p->sin.sin_addr.s_addr = inet_addr(p->zServerIP);

	*pnIdx = idx;
	g_sockCnt++;
	g_sock[idx] = p;

	return E_OK;
}


//
//bool AlphaTcp_IsConnected(int idx)
//{
//	SOCK_INFO* p = Sock(idx);
//	if (p == NULL)
//		return false;
//	return p->bConn;
//}

int AlphaTcp_DeInit(int idx)
{
	AlphaTcp_Disconnect(idx);
	return ERR_OK;
}

int AlphaTcp_Disconnect(int idx)
{
	SOCK_INFO* p = Sock(idx);
	if (p == NULL)
		return E_INVALID_SOCK;

	if (p->sock != INVALID_SOCKET) {
		struct linger ling;
		ling.l_onoff = 1;   // 0 ? use default, 1 ? use new value
		ling.l_linger = 0;  // close session in this time
		setsockopt(p->sock, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));
		//-We can avoid TIME_WAIT on both of client and server side as we code above.
		closesocket(p->sock);
		p->sock = INVALID_SOCKET;
	}
	if (p->hwsa) {
		WSACloseEvent(p->hwsa);
		p->hwsa = NULL;
	}
	
	p->bConn	= false;

	delete p;
	g_sock.erase(idx);

	return ERR_OK;
}

int AlphaTcp_Connect(int idx)
{
	SOCK_INFO* p = Sock(idx);
	if (p == NULL)
		return E_INVALID_SOCK;

	int nRet = 0;

	if (connect(p->sock, (LPSOCKADDR)&p->sin, sizeof(p->sin)) == SOCKET_ERROR)
	{
		p->bConn = FALSE;
		DumpErr("connect");
		AlphaTcp_Disconnect(idx);
		return E_CONNECT;
	}

	//g_sock.hwsa = WSACreateEvent();
	//if (WSAEventSelect(g_sock.sock, g_sock.hwsa, FD_READ | FD_CLOSE) == SOCKET_ERROR)
	//{
	//	g_sock.bConn = FALSE;
	//	DumpErr("WSAEventSelect");
	//	AlphaTcp_Disconnect();
	//	return E_CONNECT;
	//}

	//sprintf(p->msg, "connect ok(%s)(%d)", p->zServerIP, p->nServerPort);
	p->bConn = true;
	return ERR_OK;
}

int AlphaTcp_RecvData(int idx, char* pRecvData, int nBuffSize, int* nRecvLen)
{
	SOCK_INFO* p = Sock(idx);
	if (p == NULL)
		return E_INVALID_SOCK;

	*nRecvLen = 0;
	if (!p->bConn) {
		sprintf(g_msg, "[RecvData]socket is not connected!");
		return E_NON_CONNECT;
	}

	int Ret = recv(p->sock, pRecvData, nBuffSize, 0);
	if (Ret == SOCKET_ERROR)
	{
		int nErr = GetLastError();
		if (nErr == WSAETIMEDOUT) {
			return E_TIMEOUT;
		}
		else
		{
			DumpErr("recv");
			Ret = E_RECV;
			return Ret;
		}
	}
	else if (Ret == 0) {
		sprintf(g_msg, "[RecvData]Server closed connection!");
		return E_DISCONN_FROM_SVR;
	}
	*nRecvLen = Ret;
	return ERR_OK;
}


int AlphaTcp_SendData(int idx, char* pSendData, int nSendSize)
{
	SOCK_INFO* p = Sock(idx);
	if (p == NULL)
		return E_INVALID_SOCK;

	if (!p->bConn) {
		sprintf(g_msg, "[SendData]socket is not connected!");
		return E_NON_CONNECT;
	}
	int Ret;
	int nRetryCnt = 0, nRetryBlock = 0;

	while (1)
	{
		Ret = send(p->sock, pSendData, nSendSize, 0);
		if (Ret > 0) {
			return ERR_OK;
		}

		//	if(Ret == SOCKET_ERROR)		
		int nErr = GetLastError();
		if (nErr == WSAETIMEDOUT)
		{
			if (++nRetryCnt > DEF_SEND_RETRY)
			{
				sprintf(g_msg, "[SendData]WSAETIMEDOUT %d회 반복으로 에러 리턴", nRetryCnt);
				return E_SEND;
			}
			continue;
		}
		else if (nErr == WSAEWOULDBLOCK)
		{
			if (++nRetryBlock > DEF_SEND_RETRY)
			{
				sprintf(g_msg, "[SendData]WSAEWOULDBLOCK %d회 반복으로 에러 리턴", nRetryBlock);
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



//void AlphaTcp_ServerInfo(char* pzServerIp, int* pnServerPort)
//{
//	strcpy(pzServerIp, g_sock.zServerIP);
//	*pnServerPort = g_sock.nServerPort;
//}


void AlphaTcp_GetLastMsg(char* pzMsg)
{
	pzMsg[0] = 0;
	strcpy(pzMsg, g_msg);
}


void DumpErr( char* pSrc)
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

	ZeroMemory(g_msg, sizeof(g_msg));
	sprintf(g_msg, "[%s](%d)%s", pSrc, nErr,(char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}