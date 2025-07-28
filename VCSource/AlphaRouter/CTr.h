#pragma once

#include <WinSock2.h>
#include <windows.h>
#include <set>
#include <string>


using namespace std;

class CTr
{
public:
	CTr();
	virtual ~CTr();

	BOOL	RegUnreg(SOCKET sock, const char* pzCode);

	VOID	SendData(char* pRecvData, int nRecvLen);

	wchar_t* GetMsg() { return m_wzMsg; }
private:
	void	RegUnregIn(SOCKET sock, const char* pzCode);
	
	VOID	SendDataIn(char* pRecvData, int nRecvLen);
	VOID	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void	Lock() { EnterCriticalSection(&m_cs); }
	void	UnLock() { LeaveCriticalSection(&m_cs); }
private:

	set<SOCKET>			m_setSock;
	CRITICAL_SECTION	m_cs;
	wchar_t				m_wzMsg[1024];
};

