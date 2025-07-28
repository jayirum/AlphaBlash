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
#include "../Common/ADOFunc.h"
//#include "MasterWithSlaves.h"
#include <windows.h>
#include <map>
#include "../Common/AlphaProtocolUni.h"


using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10


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
	BOOL InitListen();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	static	unsigned WINAPI ListenThread(LPVOID lp);
	static	unsigned WINAPI IocpWorkerThread(LPVOID lp);

	void	DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen); 
	
	BOOL	Request_ConfigSymbol_Master(COMPLETION_KEY* pCK, const char* pRqstData, int nDataLen);
	BOOL	Request_ConfigGeneral_Master(COMPLETION_KEY* pCK, const char* pRqstData, int nDataLen);
	BOOL	Request_OpenOrders(COMPLETION_KEY* pCK, const char* pRqstData, int nDataLen);
	
	BOOL	Request_ConfigSymbol_Copier(COMPLETION_KEY* pCK, const char* pRqstData, int nDataLen);
	BOOL	Request_ConfigGeneral_Copier(COMPLETION_KEY* pCK, const char* pRqstData, int nDataLen);

	BOOL	DBSave_CopierOrder(const char* pOrdData, int nDataLen);
	BOOL	DBSave_UserLog(const char* pLogData, int nDataLen);
	
	void	ReturnError		(COMPLETION_KEY* pCK, int nErrCode);
	
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
	HANDLE			m_hListenThread;
	unsigned int	m_dListenThread;
	wchar_t 		m_wzListenIP[32];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	CProtoGet		m_protoGet;
	CProtoBuffering	m_buffering[MAX_IOCPTHREAD_CNT];

	// Session 관리를 위한 map
	map<string, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;

	BOOL 				m_bRun;	
	CDBPoolAdo*			m_pDBPool;

	wchar_t m_wzMsg[1024];

private:

	//map<string, CMasterWithSlaves*>	m_mapMaster;	// key : id+accno EN_MASTERKEY
	long				m_lIocpThreadIdx;
};