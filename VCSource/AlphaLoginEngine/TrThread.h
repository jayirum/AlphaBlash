#pragma once


#include "Tcp.h"
#include "../Common/AlphaProtocol.h"


class CTrThread 
{
public:
	CTrThread();
	~CTrThread();

	bool	Initialize(char* pzServerIp, int nServerPort, int nSendTimeout);
	void	DeInitialize();
	void	SendData(char* pSendBuf, int nBufLen);
	bool	Has_ErrorHappened(char* pErrMsg);
	void	DisConnect() { m_tcp->Disconnect(); }
	bool	IsConnected() { return m_tcp->IsConnected(); }
	void	SetSvrInfo(char* pzServerIp, int nServerPort);
	char* GetMsg() { return m_zMsg; }
private:
	static	unsigned WINAPI SendThread(LPVOID lp);
	void	ThreadFunc(); 
	void	ErrHappened_Set() { m_bErrHappened = true; }
	void	ErrHappened_Reset() { m_bErrHappened = false; ZeroMemory(m_zMsg, sizeof(m_zMsg)); }

private:
	CTcp	*m_tcp;
	char	m_zMsg[1024];
	bool	m_bErrHappened;

	HANDLE			m_hSendThread;
	unsigned int	m_dwSendThread;
	bool			m_bContinue;
};

