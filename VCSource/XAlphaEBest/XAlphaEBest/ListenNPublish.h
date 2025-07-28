// TcpSocket.h: interface for the CListenNPublsh class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <winsock2.h>
#include <stdio.h>
#include <list>
#include <set>
#include "../../Common/XAlpha_Common.h"

#pragma comment(lib, "ws2_32.lib")

const int SEND_TIMEOUT_MS = 3000;


class CListenNPublsh  
{
public:
	CListenNPublsh();
	virtual ~CListenNPublsh();

	int		Initialize();	// 서버용 소켓 초기화
	VOID	UnInitialize();
	VOID	SendData(UINT message, char* pData, int nDataLen);
	LPSTR	GetMsg() { return m_zMsg; };

private:
	BOOL	Send_KospiFut(char* pEbestData, std::set<SOCKET>* setSock);
	BOOL	Send_OverseasFut(char* pEbestData, std::set<SOCKET>* setSock);
	VOID	SendToClient(_XAlpha::TTICK* pSendData, std::set<SOCKET>* setSock);
	int		SetSockErrMsg(const char* pzMsg);
	VOID	CloseListenSock(); 
	void	CloseClient(SOCKET sock);
private:
	static unsigned WINAPI AcptThread(LPVOID lp);
	static unsigned WINAPI SendThread(LPVOID lp);

private:
	
	SOCKET	m_sockListen;
	int		m_nListenPort;
	char	m_zListenIP[32];	
	SOCKADDR_IN		m_sock_addr;
	int				m_nlen_addr;
	WSAEVENT	m_hwsa;
	HANDLE		m_hListenThread;
	unsigned	m_unListenThread;

	std::list<SOCKET>	m_lstSocks;

	CRITICAL_SECTION	m_csThreadId;
	std::list<UINT>		m_lstThreadId;

	//HANDLE	m_hSignal;

	int		m_nThreadCnt;
	BOOL	m_bContinue;
	char	m_zMsg[1024];

};