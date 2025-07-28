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
#include "../commonAnsi/ADOFunc.h"
//#include "MasterWithSlaves.h"
#include <windows.h>
#include <map>
#include <string>
#include "../Common/AlphaProtocolUni.h"
#include "CStrategyProc.h"
#include "CMarketTimeHandler.h"

using namespace std;

//#define WORKTHREAD_CNT	1
//#define MAX_IOCPTHREAD_CNT	10


typedef map<string, CStrategyProc*>				MAP_STRATEGY;
typedef map<string, CStrategyProc*>::iterator	IT_MAP_STRATEGY;

struct TPacket
{
	COMPLETION_KEY* pCK;
	string			packet;
};

struct TSockets
{
	SOCKET	main;
	SOCKET	hedge;
};

class CIocp 
{
public:
	CIocp();
	virtual ~CIocp();

	BOOL Initialize();
	void Finalize();

private:
	
	//////////////////////////////////////////////////////////////////////////////////
	//	Base functions

	BOOL	DBOpen();
	BOOL	ReadIPPOrt();
	BOOL	InitListen();
	void	CloseListenSock();
	void	DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen);
	void	ReturnError(COMPLETION_KEY* pCK, int nErrCode);
	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);
	void	SaveToDB(int iSymbol, BOOL bBuy, char* pzErrMsg, BOOL bSucc = TRUE, BOOL bMarketClose = FALSE);
	void	DeleteSocket(COMPLETION_KEY* pCompletionKey);
	void	lockCK() { EnterCriticalSection(&m_csCK); }
	void	unlockCK() { LeaveCriticalSection(&m_csCK); }

	
	//////////////////////////////////////////////////////////////////////////////////
	//	thread functions
	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);
	static	unsigned WINAPI Thread_Dispatch(LPVOID lp);

	static	unsigned WINAPI Thread_OrderSend(LPVOID lp);
	static	unsigned WINAPI Thread_MarketTime(LPVOID lp);
	static	unsigned WINAPI Thread_DBSave(LPVOID lp);


	//////////////////////////////////////////////////////////////////////////////////
	//	logic related functions
	
	BOOL	CreateStrategies_BySymbol();
	BOOL	Logon_Process(SOCKET sock, const char* pLoginData, int nDataLen);
	BOOL	Load_TradingTime();
	BOOL	RecoverOpenPositions();
	void	SendOpenOrder(int iSymbol);
	void	SendCloseOrder(int iSymbol, char cOrderSide);
	void	CloseAllOpenPositions(char* pzCloseType);

	BOOL	FindStrategyMap(string sSymbol, IT_MAP_STRATEGY& it);


private:

	//////////////////////////////////////////////////////////////////////////////////
	//	Base variables and class instances
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;

	HANDLE			m_hThread_Listen, m_hThread_Parsing, m_hThread_Dispatch;
	unsigned int	m_unThread_Listen, m_unThread_Parsing, m_unThread_Dispatch;

	HANDLE			m_hThread_OrderSend, m_hThread_MarketTime, m_hThread_Db;
	unsigned int	m_unThread_OrderSend, m_unThread_MarketTime, m_unThread_Db;

	char			m_zListenIP[128];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	CPacketParser	m_parser;
	list< TDeliveryItem*>	m_lstPacket;
	CRITICAL_SECTION		m_csPacket;

	CDBPoolAdo*				m_pDBPool;

	TSockets				m_sockets;

	BOOL					m_bRun;
	char					m_zMsg[1024];

	// Session 관리를 위한 map
	map<string, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;


	////////////////////////////////////////////////////////////////////////////////////////////
	// For logics

	// Strategy instances for each symbol
	map<string, CStrategyProc*>		m_mapStrategy;	// symbol,
	CRITICAL_SECTION				m_csStrategy;

	CMarketTimeHandler				m_marketTimeHandler;

	
	
};