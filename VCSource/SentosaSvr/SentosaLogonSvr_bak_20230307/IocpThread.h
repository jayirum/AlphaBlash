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
#include <vector>
#include <string>
#include "../../Common/AlphaProtocol.h"
#include "CLogonAuth.h"

using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10

//typedef map<TYPE_USER_ID, CAppManager* >				MAP_USER;
//typedef map<TYPE_USER_ID, CAppManager* >::iterator	IT_MAP_USER;



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
	//BOOL CreateSymbolThread();
	//BOOL Load_TradingTime();
	//BOOL RecoverOpenPositions();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);
	//static	unsigned WINAPI Thread_OrderSend(LPVOID lp);
	static	unsigned WINAPI Thread_Config(LPVOID lp);
	


	//void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	//void 	RequestRecvIO(COMPLETION_KEY* pCK);
	void	DeleteSocket(COMPLETION_KEY* pCompletionKey);
	//void	ReturnError(SOCKET sock, const char* pCode, int nErrCode, char* pzMsg);

	void	Delete_AuthCompleted();
	//BOOL	Logon_Process(SOCKET sock, string sClientIp, const char* pLoginData, int nDataLen);
	//void	AddList_ClientRecvSock(string sAppId, SOCKET sock);

	
	//BOOL Find_User(string sUserId, _Out_ IT_MAP_USER& it);

private:
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;

	HANDLE			m_hThread_Listen, m_hParsing, m_hThread_Login, m_hThread_Config;
	unsigned int	m_unThread_Listen, m_unParsing, m_unThread_Login, m_unThread_Config;
	//char			m_zListenIP[128];
	//int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	//int				m_nBrokersCnt;

	CPacketBufferIocp	m_parser;

	BOOL m_bRun;
	char m_zMsg[1024];

	int nDebug ;

private:
	long				m_lIocpThreadIdx;
	vector<CLogonAuth*>	m_vecAuth;
	CRITICAL_SECTION	m_csAuth;
};