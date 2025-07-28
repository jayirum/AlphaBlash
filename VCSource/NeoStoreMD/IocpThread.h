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
#include <windows.h>
#include <map>
#include <string>
#include <list>
#include "../Common/AlphaProtocolUni.h"

using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10


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
	BOOL DBOpen();
	BOOL ReadIPPOrt();
	BOOL ReadWorkerThreadCnt(int *pnCnt);

	BOOL InitListen();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	static	unsigned WINAPI Thread_Listen(LPVOID lp);
	static	unsigned WINAPI Thread_Iocp(LPVOID lp);
	static	unsigned WINAPI Thread_Parsing(LPVOID lp);
	static	unsigned WINAPI Thread_Worker(LPVOID lp);

	void	DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen); 
	VOID	SaveMD(const char* pRecvData, int nDataLen);
	VOID	SaveCandle(const char* pRecvData, int nDataLen);

	void	ReturnError		(COMPLETION_KEY* pCK, int nErrCode);
	//void	AddList_ClientRecvSock(char* pzBrokerKey, SOCKET sock);
	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);
	
	void	DeleteSocket		(COMPLETION_KEY *pCompletionKey);
	
	void lockCK() { EnterCriticalSection(&m_csCK); }
	void unlockCK() { LeaveCriticalSection(&m_csCK); }

private:
	DWORD			m_dwThreadCount;
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;

	HANDLE			m_hThread_Listen, m_hWorkerThread, m_hParsing;
	unsigned int	m_unThread_Listen, m_unWorkerThread, m_unParsing;
	char			m_zListenIP[128];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	//CProtoGet		m_protoGet;
	//CProtoBuffering	m_buffering;// [MAX_IOCPTHREAD_CNT] ;
	CPacketParser		m_parser;

	// Session 관리를 위한 map
	map<string, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;

	
	CDBPoolAdo*			m_pDBPool;


	BOOL m_bRun;
	char m_zMsg[1024];

private:

	//map<string, CMasterWithSlaves*>	m_mapMaster;	// key : id+accno EN_MASTERKEY
	long				m_lIocpThreadIdx;

	//list<TPacket*>		m_lstPacket;
	//list< COMPLETION_KEY*> m_lstPacket;
	list< TDeliveryItem*>	m_lstPacket;
	CRITICAL_SECTION	m_csPacket;
};