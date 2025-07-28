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
#include "ArbiLatency.h"

using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10


typedef map<string, CArbiLatency*>				MAP_SYMBOL;
typedef map<string, CArbiLatency*>::iterator	IT_MAP_SYMBOL;


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
	//BOOL DBOpen();
	//BOOL ReadIPPOrt();
	//BOOL ReadBrokerCount();
	//BOOL ReadTSApplyYN();
	//BOOL ReadProfitCutThreshold();
	//BOOL ReadTradeCloseTime();
	//BOOL ReadWorkerThreadCnt(int *pnCnt);

	BOOL InitListen();
	BOOL CreateSymbolThread();
	//BOOL Load_TradingTime();
	//BOOL RecoverOpenPositions();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);
	static	unsigned WINAPI Thread_Login(LPVOID lp);
	static	unsigned WINAPI Thread_OrderSend(LPVOID lp);
	static	unsigned WINAPI Thread_Config(LPVOID lp);


	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);
	void	DeleteSocket(COMPLETION_KEY* pCompletionKey);
	void	ReturnError(COMPLETION_KEY* pCK, int nErrCode);


	BOOL	Logon_Process(SOCKET sock, const char* pLoginData, int nDataLen);
	void	AddList_ClientRecvSock(char* pzBrokerKey, SOCKET sock);

	void lockCK() { EnterCriticalSection(&m_csCK); }
	void unlockCK() { LeaveCriticalSection(&m_csCK); }
	void lockSymbol() { EnterCriticalSection(&m_csSymbol); }
	void unlockSymbol() { LeaveCriticalSection(&m_csSymbol); }

private:
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;

	HANDLE			m_hThread_Listen, m_hParsing, m_hThread_OrderBest, m_hThread_OrderWorst, m_hThread_Login, m_hThread_Config;
	unsigned int	m_unThread_Listen, m_unParsing, m_unThread_OrderBest, m_unThread_OrderWorst, m_unThread_Login, m_unThread_Config;
	//char			m_zListenIP[128];
	//int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	//int				m_nBrokersCnt;

	CPacketParser		m_parser;

	// Session 관리를 위한 map
	map<string, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION				m_csCK;

	map<string, SOCKET>				m_mapToSend;			// BROKER KEY, SOCKET
	CRITICAL_SECTION				m_csToSend;

	map<string, CArbiLatency*>	m_mapSymbol;
	CRITICAL_SECTION				m_csSymbol;


	BOOL m_bRun;
	char m_zMsg[1024];

	int nDebug ;

private:
	long				m_lIocpThreadIdx;
};