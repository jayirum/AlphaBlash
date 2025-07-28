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

#include "CMaster.h"
//#include "../Common/IRUM_Common.h"
//#include "../Common/ADOFunc.h"
#include <windows.h>
#include <map>
#include <string>
#include "../Common/XAlpha_Common.h"


using namespace std;

#define STR_MASTER_ID	string
#define STR_SOCKET		string
#define STR_USER_ID		string
#define STR_MASTER_ID	string


//#define WORKTHREAD_CNT	1
//#define MAX_IOCPTHREAD_CNT	10

#define LOG_IN	"I"
#define LOG_OFF	"O"

//#define OLDDATA_GAP_SEC	30

//#define MAX_MASTERS_CNT	10


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

	BOOL Create_MasterInstances();
	BOOL ReadCnfg_IocpCnt();

	

	static	unsigned WINAPI ListenThread(LPVOID lp);
	static	unsigned WINAPI IocpWorkerThread(LPVOID lp);
	static	unsigned WINAPI SendToAllThread(LPVOID lp);

	BOOL	Exist_SameIP(SOCKET sock, _Out_ char* pzClientIp);

	void	SendToAll(char* psData, int nDataLen);
	void	RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen);
	void 	RequestRecvIO(COMPLETION_KEY* pCK);

	void	Recv_CloseEvent_FromEA		(COMPLETION_KEY *pCompletionKey);
	void	Copier_CheckPwd_SendInitData(COMPLETION_KEY* pCK, char* pRecvBuf);
	void	Copier_Rqst_CntrHist(COMPLETION_KEY* pCK, char* pRecvBuf);

	void	lockCK() { EnterCriticalSection(&m_csCK); }
	void	unlockCK() { LeaveCriticalSection(&m_csCK); }
	
	void	lockMasters() { EnterCriticalSection(&m_csMasters); }
	void	unlockMasters() { LeaveCriticalSection(&m_csMasters); }
	void	RemoveAllMaster();

	void	FormatMasterId(wchar_t* pwzMasterID, _Out_ string* pID);
	void	FormatMasterId(char* pzMasterID, _Out_ string* pID);
private:
	HANDLE			m_hCompletionPort;
	SOCKET			m_sockListen;
	HANDLE			m_hListenThread, m_hSendToAllThread;
	unsigned int	m_dListenThread, m_dSendToAllThread;
	wchar_t			m_wzListenIP[128];
	int 			m_nListenPort;
	WSAEVENT		m_hListenEvent;

	
	// Session 관리를 위한 map
	map<STR_SOCKET, COMPLETION_KEY*>	m_mapCK;		//socket, ck
	CRITICAL_SECTION	m_csCK;

	map<STR_MASTER_ID, CMaster*>	m_mapMaster;
	CRITICAL_SECTION	m_csMasters;
	
	BOOL				m_bSendCntrInit;
	BOOL 				m_bRun;	
	//CDBPoolAdo*			m_pDBPool;
	int					m_nIocpThreadCnt;
	wchar_t				m_wzMsg[1024];

	int					m_nMastersCnt;

private:
	long				m_lIocpThreadIdx;
};