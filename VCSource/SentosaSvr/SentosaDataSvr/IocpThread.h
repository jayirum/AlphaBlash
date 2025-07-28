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
//#include "../../commonAnsi/ADOFunc.h"
//#include "MasterWithSlaves.h"
#include <windows.h>
#include <map>
#include <list>
#include <string>
#include "../../Common/AlphaProtocol.h"
#include "CLogonAuth.h"

using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10

//typedef map<TYPE_USER_ID, CLogonAuth* >				MAP_USER;
//typedef map<TYPE_USER_ID, CLogonAuth* >::iterator	IT_MAP_USER;


class CIocp 
{
public:
	CIocp();
	virtual ~CIocp();

	BOOL Initialize();
	void Finalize();

private:

	BOOL InitListen();
	void CloseListenSock();
	void CloseClientSock(SOCKET sock);
	VOID SendMessageToIocpThread(int Message);
	void AddList_DeletingCK(COMPLETION_KEY* pCompletionKey);
	void Try_Delete_ClosedCK();

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);
	static	unsigned WINAPI Thread_Auth(LPVOID lp);
	void	ExecuteAuth(TPacket* pPacket);
	static	unsigned WINAPI Thread_Config(LPVOID lp);

private:
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;

	HANDLE			m_hThread_Listen, m_hParsing, m_hThread_Config;
	unsigned int	m_unThread_Listen, m_unParsing, m_unThread_Config;
	WSAEVENT		m_hListenEvent;

	CPacketBufferIocp	m_parser;

	//vector<CLogonAuth*>	m_vecAuth;
	//CRITICAL_SECTION	m_csAuth;

	list<COMPLETION_KEY*>	m_lstDeletingCK;
	CRITICAL_SECTION		m_csDeletingCK;

	BOOL m_bRun;
	char m_zMsg[1024];
	int nDebug ;

private:
	long				m_lIocpThreadIdx;
};