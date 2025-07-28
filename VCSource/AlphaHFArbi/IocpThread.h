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
#include "../Common/AlphaProtocolUni.h"
#include "CSaveMD.h"
#include <list>

using namespace std;

//#define WORKTHREAD_CNT	1
//#define MAX_IOCPTHREAD_CNT	10


typedef map<string, CSaveMD*>				MAP_SYMBOL;
typedef map<string, CSaveMD*>::iterator		IT_MAP_SYMBOL;

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
	
	//////////////////////////////////////////////////////////////////////////////////
	//	Base functions

	BOOL	DBOpen();
	BOOL	ReadIPPOrt();
	BOOL	InitListen();
	void	CloseListenSock();
	void	DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen);
	void	AddList_ClientRecvSock(char* pzBrokerKey, SOCKET sock);
	void	SaveToDB(int iSymbol, BOOL bBuy, char* pzErrMsg, BOOL bSucc = TRUE, BOOL bMarketClose = FALSE);

	void	ReturnError(COMPLETION_KEY* pCK, int nErrCode);
	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);
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
	
	BOOL	CreateSaveMD_BySymbol();
	BOOL	Logon_Process(SOCKET sock, const char* pLoginData, int nDataLen);
	//BOOL	Load_TradingTime();
	//BOOL	RecoverOpenPositions();
	//void	SendOpenOrder(int iSymbol);
	//void	SendCloseOrder(int iSymbol, char cOrderSide);
	//void	CloseAllOpenPositions(char* pzCloseType);

	BOOL	FindSymbolMDMap(string sSymbol, map<string, CSaveMD*>::iterator &it){
		it = m_mapSymbolMD.find(sSymbol);	return (it != m_mapSymbolMD.end());
	}


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

	

	BOOL					m_bRun;
	char					m_zMsg[1024];

	// Session 관리를 위한 map
	map<string, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;


	////////////////////////////////////////////////////////////////////////////////////////////
	// For logics

	// map containing Saving MD class for each symbol
	map<string, CSaveMD*>				m_mapSymbolMD;	// symbol, 
	CRITICAL_SECTION					m_csSymbolMD;

	map<string, SOCKET>					m_mapToSend;	// BROKER KEY, SOCKET
	CRITICAL_SECTION					m_csToSend;


	
	
};