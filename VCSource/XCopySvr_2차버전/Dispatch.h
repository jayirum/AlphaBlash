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
#include "../Common/IRUM_Common.h"
#include "../Common/ADOFunc.h"
#include <windows.h>
#include <map>
#include <string>
#include "../Common/XAlphaGT_Common.h"

using namespace std;

#define STR_MASTER_ID	string
#define STR_SOCKET		string
#define STR_USER_ID		string


#define WORKTHREAD_CNT	1
//#define MAX_IOCPTHREAD_CNT	10

#define LOG_IN	"I"
#define LOG_OFF	"O"

#define OLDDATA_GAP_SEC	30

#define MAX_MASTERS_CNT	10

struct MASTER_INFO
{
	wchar_t	wzID[32];
	char	logInOff[1];
	wchar_t	wzLastLogOnOffTm[20];	//yyyymmddhh:mm:ss:mmm
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

	BOOL ReadCnfg_MasterID();
	BOOL ReadCnfg_DBReadTimeOut();
	BOOL ReadCnfg_IocpCnt();

	BOOL DBOpen();

	static	unsigned WINAPI ListenThread(LPVOID lp);
	static	unsigned WINAPI IocpWorkerThread(LPVOID lp);
	static	unsigned WINAPI DBReadThread(LPVOID lp);


	BOOL	Get_LastCntrNo();

	BOOL	Exist_SameIP(SOCKET sock, _Out_ char* pzClientIp);

	BOOL	_M_Conn_MainProc(_InOut_ MASTER_INFO* p, BOOL bCopierCall);
	BOOL	_M_Conn_Publish_LoginStatus(wchar_t* masterId, wchar_t* masterNm, wchar_t* Tm, wchar_t* loginTp, int nGapSec);
	BOOL	_M_Cntr_MainProc(_InOut_ MASTER_INFO* p, BOOL bHistory, COMPLETION_KEY* pCK);
	BOOL	_M_Cntr_Publish(
		BOOL bHistory
		, int cntrNo
		, wchar_t* masterId
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
		, COMPLETION_KEY *pCK
	);
	
	void	SendToAll(char* psData, int nDataLen);
	void	RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen);
	void	RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);

	void	Recv_CloseEvent_FromEA		(COMPLETION_KEY *pCompletionKey);
	void	Copier_CheckPwd_SendInitData(COMPLETION_KEY* pCK, char* pRecvBuf);
	void	Copier_Rqst_CntrHist(COMPLETION_KEY* pCK, char* pRecvBuf);

	void	lockCK() { EnterCriticalSection(&m_csCK); }
	void	unlockCK() { LeaveCriticalSection(&m_csCK); }
	
	void	lockMasters() { EnterCriticalSection(&m_csMasters); }
	void	unlockMasters() { LeaveCriticalSection(&m_csMasters); }
	void	RemoveAllMaster();

	int		MasterIdx(char* pzMasterId);

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

	MASTER_INFO			m_arrMasters[MAX_MASTERS_CNT];
	CRITICAL_SECTION	m_csMasters;
	
	BOOL				m_bSendCntrInit;
	BOOL 				m_bRun;	
	CDBPoolAdo*			m_pDBPool;
	int					m_nIocpThreadCnt;
	wchar_t				m_wzMsg[1024];

	int					m_nTimeoutDB;	// sec	TODO

private:
	long				m_lIocpThreadIdx;
};