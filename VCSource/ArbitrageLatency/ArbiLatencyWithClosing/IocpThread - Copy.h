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
	//BOOL DBOpen();
	BOOL ReadIPPOrt();
	//BOOL ReadBrokerCount();
	//BOOL ReadTSApplyYN();
	//BOOL ReadProfitCutThreshold();
	//BOOL ReadTradeCloseTime();
	BOOL ReadWorkerThreadCnt(int *pnCnt);

	BOOL InitListen();
	BOOL LoadSymbols();
	//BOOL Load_TradingTime();
	//BOOL RecoverOpenPositions();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);


	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);
	void	DeleteSocket(COMPLETION_KEY* pCompletionKey);
	void	ReturnError(COMPLETION_KEY* pCK, int nErrCode);


	static	unsigned WINAPI Thread_OrderSend(LPVOID lp);
	//static	unsigned WINAPI Thread_MarketTime(LPVOID lp);
	//static	unsigned WINAPI Thread_Worker(LPVOID lp);

	//void	DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen); 
	//BOOL	Logon_Process(SOCKET sock, const char* pLoginData, int nDataLen);
	//VOID	MD_Process(const char* pRecvData, int nDataLen);
	////VOID	OpenPositions_Process(COMPLETION_KEY* pCK, const char* pRecvData, int nDataLen);
	//VOID	OrderReceive_Open(const char* pRecvData, int nDataLen);
	//VOID	OrderReceive_Close(const char* pRecvData, int nDataLen);
	//void	AddList_ClientRecvSock(char* pzBrokerKey, SOCKET sock);
	//void	SendOpenOrder(int iSymbol);
	//void	SendCloseOrder(int iSymbol, char cOrderSide);
	//void	SaveToDB(int iSymbol, BOOL bBuy, char* pzErrMsg, BOOL bSucc = TRUE, BOOL bMarketClose = FALSE);
	//void	SaveLogToDB(char* pzTitle, char* pzMsg);
	//BOOL	Weekend_Check_Set();
	//void	MarketClose_Check_Order();
	//BOOL	TradeCloseTimeCheck();
	//void	CloseAllOpenPositions(char* pzCloseType);
	//void	Snapshot();

	void lockCK() { EnterCriticalSection(&m_csCK); }
	void unlockCK() { LeaveCriticalSection(&m_csCK); }
	void lockSymbol() { EnterCriticalSection(&m_csSymbol); }
	void unlockSymbol() { LeaveCriticalSection(&m_csSymbol); }

private:
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;

	HANDLE			m_hThread_Listen, m_hParsing, m_hThread_OrderSend;// m_hNoMoreOpenThread, m_hWorkerThread, ;
	unsigned int	m_unThread_Listen, m_unParsing, m_unThread_OrderSend;//, m_unNoMoreOpenThread, m_unWorkerThread, ;
	char			m_zListenIP[128];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	//CProtoGet		m_protoGet;
	//CProtoBuffering	m_buffering;// [MAX_IOCPTHREAD_CNT] ;
	CPacketParser		m_parser;

	// Session 관리를 위한 map
	map<string, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION				m_csCK;

	map<string, SOCKET>				m_mapToSend;			// BROKER KEY, SOCKET
	CRITICAL_SECTION				m_csToSend;

	map<string, CCompareLatency*>	m_mapSymbol;
	CRITICAL_SECTION				m_csSymbol;

	//CDBPoolAdo*			m_pDBPool;


	BOOL m_bRun;
	char m_zMsg[1024];

	int nDebug ;

	//BOOL	m_bUse_NoMoreOpenByTime;
	//BOOL	m_bUse_MarketCloseClear;
	//BOOL	m_bMarketCloseClearedAlready;
	//BOOL	m_bTradeClose;
	//char	m_zMarketCloseClearTime[32];
	//char	m_zWeekendStartTime[32];
	//char	m_zTradeCloseTime[32];
	//char	m_zLastSnapshotHour[6];	// hh:00

	//BOOL	m_bWeekendStartAlready;

	//int		m_intevalLoggingCnt;
private:

	//map<string, CMasterWithSlaves*>	m_mapMaster;	// key : id+accno EN_MASTERKEY
	long				m_lIocpThreadIdx;

	//list<TPacket*>		m_lstPacket;
	//list< COMPLETION_KEY*> m_lstPacket;
	//list< TDeliveryItem*>	m_lstPacket;
	//CRITICAL_SECTION	m_csPacket;
};