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
#include <string>
#include "../../Common/AlphaProtocolUni.h"
#include "CompareLatency.h"

using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10


typedef map<string, CCompareLatency*>			MAP_SYMBOL;
typedef map<string, CCompareLatency*>::iterator	IT_MAP_SYMBOL;


struct TPacket
{
	COMPLETION_KEY* pCK;
	string			packet;
};

class CIocp 
{
public:
	CIocp();
	virtual ~CIocp();

	BOOL Initialize();
	void Finalize();

private:
	BOOL ReadIPPOrt();
	BOOL ReadWorkerThreadCnt(int *pnCnt);

	BOOL InitListen();
	BOOL LoadSymbols();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);


	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);
	void	DeleteSocket(COMPLETION_KEY* pCompletionKey);
	void	ReturnError(COMPLETION_KEY* pCK, int nErrCode);

	void lockCK() { EnterCriticalSection(&m_csCK); }
	void unlockCK() { LeaveCriticalSection(&m_csCK); }
	void lockSymbol() { EnterCriticalSection(&m_csSymbol); }
	void unlockSymbol() { LeaveCriticalSection(&m_csSymbol); }

private:
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;

	HANDLE			m_hThread_Listen, m_hParsing;
	unsigned int	m_unThread_Listen, m_unParsing;
	char			m_zListenIP[128];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	CPacketParser		m_parser;

	// Session 관리를 위한 map
	map<string, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;

	map<string, CCompareLatency*>	m_mapSymbol;
	CRITICAL_SECTION				m_csSymbol;

	//CDBPoolAdo*			m_pDBPool;


	BOOL m_bRun;
	char m_zMsg[1024];

	int nDebug ;
private:

	long				m_lIocpThreadIdx;
};