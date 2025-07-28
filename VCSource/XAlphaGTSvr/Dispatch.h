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
#include <windows.h>
#include <map>
#include <string>
#include "../Common/XAlphaGT_Common.h"

#define STR_MASTER_ID	string
#define STR_SOCKET		string
#define STR_USER_ID		string

using namespace std;

#define WORKTHREAD_CNT	1
//#define MAX_IOCPTHREAD_CNT	10

#define LOG_IN	"I"
#define LOG_OFF	"O"

struct USER_INFO
{
	wchar_t	zUserID[32];
	char	logInOff[1];
	int		nLastCntrNo;
};

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

	BOOL ReadCnfg_UserID();
	BOOL ReadCnfg_DBReadTimeOut();
	BOOL ReadCnfg_IocpCnt();

	BOOL DBOpen();

	static	unsigned WINAPI ListenThread(LPVOID lp);
	static	unsigned WINAPI IocpWorkerThread(LPVOID lp);
	static	unsigned WINAPI DBReadThread(LPVOID lp);

	//static	unsigned WINAPI SendInitDataThread(LPVOID lp);

	BOOL	Exist_SameIP(SOCKET sock, _Out_ char* pzClientIp);

	BOOL	_CheckAndSend_LogonOff(int idx);
	BOOL	Compare_Send_LogonStatus(wchar_t* userId, wchar_t* userNm, wchar_t* Tm, wchar_t* loginTp);
	BOOL	_CheckAndSend_Cntr(int idx);
	BOOL	Send_Cntr(BOOL bInit
		, int cntrNo
		, wchar_t* userId
		, wchar_t* stkCd
		, wchar_t* bsTp
		, int cntrQty
		, double cntrPrc
		, double clrPl
		, double cmsn
		, wchar_t* clrTp
		, int bf_nclrQty
		, int af_nclrQty
		, double bf_avgPrc
		, double af_avgPrc
		, double bf_amt
		, double af_amt
		, wchar_t* ordTp
		, wchar_t* tradeTm
		, int lvg
	);
	
	void	SendToAll(char* psData, int nDataLen);
	void	RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen);
	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);

	void	Recv_CloseEvent_FromEA		(COMPLETION_KEY *pCompletionKey);

	void lockCK() { EnterCriticalSection(&m_csCK); }
	void unlockCK() { LeaveCriticalSection(&m_csCK); }

private:
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;
	HANDLE			m_hListenThread, m_hQThread;
	unsigned int	m_dListenThread, m_dQThread;
	wchar_t			m_wzListenIP[128];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	
	// Session 관리를 위한 map
	map<STR_SOCKET, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;

	USER_INFO			m_User[2];
	BOOL				m_bSendCntrInit;
	BOOL 				m_bRun;	
	CDBPoolAdo*			m_pDBPool;
	int					m_nIocpThreadCnt;
	wchar_t				m_wzMsg[1024];

	int					m_nTimeoutDB;	// sec	TODO

private:
	long				m_lIocpThreadIdx;
};