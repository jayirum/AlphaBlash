#pragma once

#include <Windows.h>
#include <tchar.h>
#pragma warning(disable:4996)

#pragma comment(lib, "ws2_32.lib")

#define DEF_SEND_RETRY 3

/*
	Use blocking mode socket
	- set timeout for send and recv
*/
class CTcpClient 
{
public:
	
	CTcpClient();
	virtual ~CTcpClient();

	BOOL	Begin( TCHAR* pRemoteIP, int nPort, int nSendTimeOut, int nRecvTimeOut);
	VOID	End();

	BOOL	Initialize(TCHAR* pRemoteIP, int nPort, int nSendTimeOut, int nRecvTimeOut) 
			{ return Begin(pRemoteIP, nPort, nSendTimeOut, nRecvTimeOut); }
	BOOL	Initialize(char* pRemoteIP, int nPort, int nSendTimeOut, int nRecvTimeOut);
	VOID	DeInitialize(){ return End() ;}
	
	BOOL	Connect();
	VOID	Disconnect();

	INT		SendData(char* pInBuf, int nBufLen, int *o_ErrCode);
	INT		SendData(char* pInBuf, int nBufLen);
	
	int		RecvData(char* pOutBuf, int nBufLen);
	
	VOID	SetIP_Port(TCHAR* psIP, int nPort);
	BOOL	IsConnected();

	TCHAR*	GetMsgW() { return m_wzMsg; };
	char* GetMsg();
	int		GetErrCode() { return m_nErrCode; }
private:
	int		RecvData(char* pOutBuf, int nBufLen, int* o_ErrCode);

	VOID	DumpErr( TCHAR* pSrc, int nErr );
//protected:
	//TCHAR			m_zMyName[32];
	TCHAR			m_zRemoteIP[128];
	int				m_nRemotePort;
	TCHAR			m_wzMsg[512];
	char			m_zMsg[512];
	SOCKET			m_sock;
	SOCKADDR_IN		m_sin;
	int				m_nErrCode;
	BOOL			m_bConn;
	int				m_nSendTimeOut, m_nRecvTimeOut;
	BOOL			m_bRecvErr;

};

