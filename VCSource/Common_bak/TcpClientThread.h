#pragma once

#include "PktHandler.h"
#include "BaseThread.h"
#pragma warning(disable:4996)

#pragma comment(lib, "ws2_32.lib")

#define DEF_SEND_RETRY 3

/*
	Use blocking mode socket
	- set timeout for send and recv
*/

typedef void (*RECV_CALLBACK)(void*, BOOL, int, char*, char*);	// result, len, buffer, message 

class CTcpClientThread : public CBaseThread
{
public:
	
	CTcpClientThread();
	virtual ~CTcpClientThread();

	BOOL	Begin(void* callbackClassPtr, RECV_CALLBACK callBack, char* pRemoteIP, int nPort, int nTimeOut);
	VOID	End();

	VOID	Set_IPPort(char* psIP, int nPort);
	BOOL	Connect();
	VOID	Run() { ResumeThread(); }
	INT		SendData(char* pInBuf, int nBufLen, int *o_ErrCode);
	
	char* GetMsg() { return m_zMsg; };

	//VOID	SetNagle(BOOL bOn);
private:
	BOOL	IsConnected();
	virtual VOID	ThreadFunc();
	VOID	RecvThread();
	VOID	Disconnect();
	VOID	DumpErr( char* pSrc, int nErr );

	
	char			m_zRemoteIP[128];
	int				m_nRemotePort;
	char			m_zMsg[512];
	SOCKET			m_sock;
	SOCKADDR_IN		m_sin;
	//WSAEVENT		m_hwsa;
	BOOL			m_bConn;
	int				m_nTimeout;
	RECV_CALLBACK	m_outerCallback;
	void			*m_callbackClassPtr;
};

