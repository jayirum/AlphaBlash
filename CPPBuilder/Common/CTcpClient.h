#ifndef __C_TCP_CLIENT_H__
#define __C_TCP_CLIENT_H__

#include <winsock2.h>
#include "AlphaInc.h"
#include "AlphaProtocol.h"
#include "CBaseThread.h" //todo after completion - remove ../
#include <System.hpp>

#pragma comment(lib, "ws2_32.lib")

#define DEF_SEND_RETRY 3



class CTcpClient : public CBaseThread
{
public:

	CTcpClient(char* pzName);
	virtual ~CTcpClient();

	bool	Init_Connect( char* pRemoteIP, int nPort, int nSendTimeout, int nRecvTimeout);

	bool	Connect();
	bool 	ReConnect();
	bool 	IsConnected();

	int		SendData(char* pInBuf, int nBufLen, int *o_ErrCode);

	String GetMsg() { return m_sMsg; };

	bool	GetOnePacket(_Out_ int* pnLen, _Out_ char* pOutBuf);

protected:
	virtual void ThreadExec();
	void 	Disconnect();
	void	DumpErr( char* pSrc, int nErr );

protected:
	String			m_sMyName;
	char			m_zRemoteIP[128];
	int				m_nRemotePort;
	AnsiString		m_sMsg;

	SOCKET			m_sock;
	SOCKADDR_IN		m_sin;
	//WSAEVENT		m_hwsa;

	bool			m_bConn;
	int				m_nSendTimeout, m_nRecvTimeout;

	CPacketBuffer	m_buffer;

};


#endif
