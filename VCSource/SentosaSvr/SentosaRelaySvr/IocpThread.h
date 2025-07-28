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
#include <windows.h>
#include <map>
#include <list>
#include <string>
#include <memory>
#include "../../Common/AlphaProtocol.h"
#include "../../Common/CMySqlHandler.h"
#include "../Common/Inc.h"
#include "../Common/ErrCodes.h"
#include "CAppManager.h"

using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10

typedef map<TYPE_USER_ID, CAppManager* >			MAP_USER;
typedef map<TYPE_USER_ID, CAppManager* >::iterator	IT_MAP_USER;

typedef map<SOCKET, COMPLETION_KEY* >		MAP_CK;
typedef map<SOCKET, COMPLETION_KEY* >::iterator	IT_MAP_CK;

class CIocp 
{
public:
	CIocp();
	virtual ~CIocp();

	BOOL Initialize();

private:
	void Finalize();

	BOOL Initialize_RelaySvrID();
	BOOL Reset_Session();


	BOOL InitListen();
	void CloseListenSock();
	void CloseClientSock(SOCKET sock);
	VOID SendMessageToIocpThread(int Message);

	BOOL Logon_Process(TPacket* pPacket);
	RET_SENTOSA Logon_DBProc(auto_ptr< CMySqlHandler>& handler,
		char* pzUserId, char* pzPwd, char* pzAppId, EN_APP_TP enAppTp, char* pzBrokerName,
		char* pzAccNo, char* pzLiveDemo, char* pzClientIp, char* pzMac, char* pzMarketTime, BOOL bDupLogon);
	BOOL Logoff_DBProc(char* pzUserId, char* pzAppId, EN_APP_TP enAppTp, BOOL bDupLogOn);

	void AddList_DeletingCK(COMPLETION_KEY* pCompletionKey);
	void Try_Delete_ClosedCK();

	void RemoveApp_ClosingMainSocket(COMPLETION_KEY* pCK);

	void RemoveCK_FromMap(SOCKET sock);
	void Set_DupLogOn(SOCKET sock);

	void LockUser() { EnterCriticalSection(&m_csUser); }
	void UnlockUser() { LeaveCriticalSection(&m_csUser); }

	BOOL Find_User(string sUserId, _Out_ IT_MAP_USER& it);

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);
	static	unsigned WINAPI Thread_Login(LPVOID lp);
	static	unsigned WINAPI Thread_Config(LPVOID lp);
	static	unsigned WINAPI Thread_DBProc(LPVOID lp);

	BOOL	OpenDB_ForLogging();

private:
	list<string>	m_lstRelaySvrID;
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;

	HANDLE			m_hThread_Listen, m_hParsing, m_hThread_Login, m_hThread_Config, m_hThread_Db;
	unsigned int	m_unThread_Listen, m_unParsing, m_unThread_Login, m_unThread_Config, m_unThread_Db;
	WSAEVENT		m_hListenEvent;

	CPacketBufferIocp	m_parser;

	MAP_USER			m_mapUser;
	CRITICAL_SECTION	m_csUser;

	MAP_CK				m_mapCK;
	CRITICAL_SECTION	m_csCK;


	list<COMPLETION_KEY*>	m_lstDeletingCK;
	CRITICAL_SECTION		m_csDeletingCK;

	CMySqlHandler			* m_pDbForLogging;
	BOOL m_bRun;
	char m_zMsg[1024];
	int nDebug ;

private:
	long				m_lIocpThreadIdx;
};