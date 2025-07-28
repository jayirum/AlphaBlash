#pragma once

/*

	Master ID 1俺客 辆加等 Slave ID 甸 包府

*/

#include "main.h"
#include "../Common/BaseThread.h"
#include "../Common/AlphaProtocolUni.h"
#include "Publisher.h"
#include "Sync.h"
#include <deque>


#define COPIER_ID	string
enum { EN_ONLINE, EN_OFFLINE };

struct SESSION_TM
{
	SOCKET	sock;
	DWORD	dwLastPing;
	int		nPingCnt;
	string	sAccNo;
	char	UserID[128];
	string	sNickNm;
	//BOOL	bMaster;
	string	sIPAddr;
};


struct MASTER_INFO
{
	SOCKET		sock;
	string		sUserId;
	string		sAccNo;
	string		sNickNm;
	BOOL		bMasterLogon;
	string		sIPAddr;
	BOOL		bDup;
};

class CMasterChannel : public CBaseThread
{
public:
	CMasterChannel();
	~CMasterChannel();

	//BOOL	Initialize(SOCKET sockMaster, unsigned int unSaveThreadId);
	BOOL	Initialize(string sMasterId, unsigned int unSaveThreadId);
	void	DeInitialize();

	VOID	PassData(SOCKET sock, const char* psRecvData, const int nRecvLen);
	BOOL	Remove_Master(string sUserID, SOCKET sock);
	void	Remove_Copier(string sUserID, SOCKET sock);
private:
	BOOL	Remove_Copier_DupPrevious(string sUserID, SOCKET sock);
	BOOL	Remove_Copier_Current(string sUserID, SOCKET sock);
private:

	void	ThreadFunc(); 
	void	_MainProcess();

	BOOL	Master_IsMarked_DupLogon() { return m_masterInfo.bDup; }
	void	Master_SetLogOff() { m_masterInfo.bMasterLogon = FALSE; }
	VOID	Master_UnMark_DupLogon() { m_masterInfo.bDup = FALSE; }
	VOID	Master_Mark_DupLogon() { m_masterInfo.bDup = TRUE; }
	BOOL	Master_IsAlreadyLogon() { return m_masterInfo.bMasterLogon; }

	void	ToAllCopiers_CloseForcely();
	void	ToMaster_Send_DupLogon(char* pNewClientIp);

	// Cublisher 
	void	Publisher_ReArrange();
	void	Publisher_ClearAll();
	void	Publish_Order(RECV_DATA* pRecvData);

	void	LoginProcess(string sMyId, string sMyAccNo, string sCode, string sMasterSlaveTp, string sNickNm, RECV_DATA* pRecvData);
	void	Add_Copier(string sMyId, _In_ SESSION_TM* pSess);
	void	Copier_CheckAndSend_DupLogon(string sMyId, _In_ SESSION_TM* pSess, char* pzNewIp);
	void	ToMaster_Send_CopierOnOff(int nOnOffline, string sUserID_Copier, string sAcc_Copier);
	void	ToAllCopiers_Send_MasterLogon(_In_ CProtoSet& protoSet);
	
	
	void	ReturnError(SOCKET sock, string sUserID, int nErrCode);
	void	RequestSendIO(SOCKET sock, string sID, char* pSendBuf, int nSendLen);

	//BOOL	IsExist(map < COPIER_ID, SESSION_TM*>::iterator it) { return (it != m_mapCopiers.end()); }

private:
	MASTER_INFO		m_masterInfo;
	CProtoGet		m_protoGet;
private:
	
	map < COPIER_ID, SESSION_TM*>	m_mapCopiers;		// SlaveID, TIME
	map < COPIER_ID, SESSION_TM*>	m_mapDuplicatedCopiers;
	map < COPIER_ID, SESSION_TM*>::iterator	m_itCopiers;

	CRITICAL_SECTION			m_csCopiers;
	CRITICAL_SECTION			m_csDuplicated;
	unsigned int				m_unSaveThreadId;

	deque< CPublisher*>			m_lstPublisher;
	CRITICAL_SECTION			m_csPublisher;

	CSync						m_sync;
	int							m_nCopierPerPublisher;
	int							m_nSyncRetryCnt;
	int							m_nSyncRetrySleep;
	wchar_t						m_wzMsg[1024];
};

