/*
	- receive data from Master / Slave

	- Use only 1 thread

	- Dispatch to CMasterWithSlaves thread
*/

#pragma once

#pragma warning( disable : 4786 )
#pragma warning( disable : 4819 )
#pragma warning( disable : 26496)
#pragma warning( disable : 26495)

#include "main.h"
//#include "MasterWithSlaves.h"
#include <windows.h>
#include <map>
#include <string>
#include "PythonEmailSender.h"
#include "../CommonAnsi/LogMsg.h"
#include "CTelegramSender.h"


using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	1


class CIocp 
{
public:
	CIocp();
	virtual ~CIocp();

	BOOL Initialize();
	void Finalize();

private:
	BOOL ReadIPPOrt();
	BOOL InitListen();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_IocpWork(LPVOID lp);

	BOOL	InitEmail();
	BOOL	InitTelegram();

	void	SendAlermData(const char* pRecvData);
	
	void	RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);

	//void	RecvLogOffAndClose	(COMPLETION_KEY *pCompletionKey);
	void	DeleteSocket		(COMPLETION_KEY *pCompletionKey);

	void lockCK() { EnterCriticalSection(&m_csCK); }
	void unlockCK() { LeaveCriticalSection(&m_csCK); }

private:
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;
	HANDLE			m_hThread_Listen;
	unsigned int	m_dThread_Listen;
	char 			m_zListenIP[32];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	CNotiLogBuffering	m_buffering;

	// Session 관리를 위한 map
	map<string, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;

	BOOL 				m_bRun;	

	char m_zMsg[1024];

private:

	long				m_lIocpThreadIdx;

	CPythonEmailSender	m_emailSender;
	char				m_zEmailSender[128];
	char				m_zEmailReceiver[128];

	CTelegramSender		m_teleSender;
};