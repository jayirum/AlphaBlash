/*
	- receive data from Master / Slave

	- Use only 1 thread

	- Dispatch to CMasterChannel thread
*/

#pragma once

#pragma warning( disable : 4786 )
#pragma warning( disable : 4819 )
#pragma warning( disable : 26496)
#pragma warning( disable : 26495)

#include "main.h"
#include "../Common/ADOFunc.h"
#include "MasterChannel.h"
#include <windows.h>
#include <map>
#include <string>
#include "../Common/AlphaProtocolUni.h"

#define STR_MASTER_ID	string
#define STR_SOCKET		string
#define STR_USER_ID		string

using namespace std;

#define WORKTHREAD_CNT	1
#define MAX_IOCPTHREAD_CNT	10


class CDispatch 
{
public:
	CDispatch();
	virtual ~CDispatch();

	BOOL Initialize();
	void Finalize();

private:
	BOOL ReadIPPOrt();
	BOOL InitListen();
	void CloseListenSock();
	VOID SendMessageToIocpThread(int Message);

	static	unsigned WINAPI ListenThread(LPVOID lp);
	static	unsigned WINAPI IocpWorkerThread(LPVOID lp);
	static	unsigned WINAPI DBSaveThread(LPVOID lp);
	static	unsigned WINAPI QReadThread(LPVOID lp);
	unsigned	DBSaveThreadFn();

	BOOL	DBSave_Order(char* pOrdData, int nDataLen);
	BOOL	DBSave_LogOnOff(char* pzUserID, SOCKET sock, BOOL bLogon);

	void	DispatchData	(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen);
	BOOL	Dispatch_To_MasterChannel(BOOL bIsMastr, string sUserID, COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen, CProtoGet* pProtoGet);
	void	ReturnError		(COMPLETION_KEY* pCK, int nErrCode);
	

	void	AddQMap(string sUserID, SOCKET sock);
	void	RemoveQMap(string sUserID);
	void	QSendData(TCHAR cAllYN, TCHAR* pwzUserID, char* pzData, int nDataLen);
	void	CompleteRead(int nSeq);

	void	RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen);
	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);

	void	Recv_CloseEvent_FromEA		(COMPLETION_KEY *pCompletionKey);

	void lockCK() { EnterCriticalSection(&m_csCK); }
	void unlockCK() { LeaveCriticalSection(&m_csCK); }

	BOOL	IsMasterMapExist(map < STR_MASTER_ID, CMasterChannel*>::iterator it) { return (it != m_mapEA.end()); }

private:
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;
	HANDLE			m_hListenThread, m_hSaveThread, m_hQThread;
	unsigned int	m_dListenThread, m_dSaveThread, m_dQThread;
	wchar_t			m_wzListenIP[128];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	//CProtoGet		m_protoGet;
	CProtoBuffering	m_buffering[MAX_IOCPTHREAD_CNT];

	// Session 관리를 위한 map
	map<STR_SOCKET, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;

	// MasterID - Info class(Slave List 포함)
	map<STR_MASTER_ID, CMasterChannel*>		m_mapEA;	// Master ID
	CRITICAL_SECTION	m_csEA;

	// QTABLE 송신을 위한 MAP
	map<STR_USER_ID, SOCKET>	m_mapQ;
	CRITICAL_SECTION			m_csQ;

	BOOL 				m_bRun;	
	CDBPoolAdo*			m_pDBPool;

	wchar_t				m_wzMsg[1024];

private:
	long				m_lIocpThreadIdx;
};