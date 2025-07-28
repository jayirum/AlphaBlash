#include "MasterChannel.h"
#include "../Common/IRUM_Common.h"
#include "../Common/LogMsg.h"
#include "../Common/IRExcept.h"
#include "../Common/util.h"
#include "../Common/MemPool.h"
#include <memory>
#include <assert.h>

extern CLogMsg	g_log;
extern wchar_t	g_wzConfig[_MAX_PATH];
extern BOOL		g_bDebugLog;

CMemPool	g_memPool;


CMasterChannel::CMasterChannel()
{
	m_masterInfo.bMasterLogon	= FALSE;
	m_masterInfo.bDup = FALSE;
	m_masterInfo.sock = INVALID_SOCKET;
}


CMasterChannel::~CMasterChannel()
{
	DeInitialize();
}


//BOOL CMasterChannel::Initialize(SOCKET sockMaster, unsigned int unSaveThreadId)
BOOL CMasterChannel::Initialize(string sMasterId, unsigned int unSaveThreadId)
{
	m_masterInfo.sUserId = sMasterId;

	wchar_t temp[32];
	CUtil::GetConfig(g_wzConfig, TEXT("PUBLISH"), TEXT("SYNC_RETRY_SLEEP"), temp);
	m_nSyncRetrySleep = _ttoi(temp);

	CUtil::GetConfig(g_wzConfig, TEXT("PUBLISH"), TEXT("SYNC_RETRY_CNT"), temp);
	m_nSyncRetryCnt = _ttoi(temp);

	CUtil::GetConfig(g_wzConfig, TEXT("PUBLISH"), TEXT("COPIERS_PER_PUBLISHER"), temp);
	m_nCopierPerPublisher = _ttoi(temp);

	ResumeThread();
	InitializeCriticalSection(&m_csCopiers);
	InitializeCriticalSection(&m_csDuplicated);
	InitializeCriticalSection(&m_csPublisher);

	m_unSaveThreadId	= unSaveThreadId;
	return TRUE;
}

void CMasterChannel::DeInitialize()
{
	Publisher_ClearAll();

	ToAllCopiers_CloseForcely();

	DeleteCriticalSection(&m_csCopiers);
	DeleteCriticalSection(&m_csDuplicated);
	DeleteCriticalSection(&m_csPublisher);
}

VOID CMasterChannel::PassData(SOCKET sock, const char* psRecvData, const int nRecvLen)
{
	//char* p = g_memPool.get();
	RECV_DATA* pData	= new RECV_DATA;
	pData->sock			= sock;
	pData->len			= nRecvLen;
	sprintf(pData->data, "%.*s", nRecvLen, psRecvData);
	PostThreadMessage(m_dwThreadID, WM_PASS_DATA, 0, (LPARAM)pData);
}


void CMasterChannel::ThreadFunc()
{
	__try
	{
		_MainProcess();
	}
	__except (ReportException(GetExceptionCode(), TEXT("MainProcess"), m_wzMsg))
	{
		g_log.logW(NOTIFY, TEXT("MasterWithSlaves::MainProcess"));
	}
}

void CMasterChannel::_MainProcess()
{
	g_log.log(INFO, "[%s] Master Thread starts....", m_masterInfo.sUserId.c_str());

	MSG msg;
	RECV_DATA Buffer;
	int nRecvLen;
	while (!Is_TimeOfStop(1))
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_PASS_DATA)
			{
				ZeroMemory(&Buffer, sizeof(Buffer));
				memcpy(&Buffer, (char*)msg.lParam, sizeof(RECV_DATA));
				delete (char*)msg.lParam;

				RECV_DATA* pRecvData = (RECV_DATA*)& Buffer;
				nRecvLen = pRecvData->len;
				m_protoGet.Parsing(pRecvData->data, nRecvLen, FALSE);

				//g_log.log(INFO, "[PeekMessage](data:%s)", pRecvData->data);

				string sCode;
				string sMasterSlaveTp;
				string sMyID;
				string sMyAcc;
				string sNickNm;
				try
				{
					ASSERT_BOOL2(m_protoGet.GetCode(sCode), E_NO_CODE, TEXT("[CMasterChannel]CMasterChannel get Code in the packet Error!!!"));
					ASSERT_BOOL2(m_protoGet.GetVal(FDS_MASTERCOPIER_TP, &sMasterSlaveTp), E_INVALIDE_MASTERCOPIER, TEXT("FDS_MASTERCOPIER_TP is not in the packet"));
					ASSERT_BOOL2(m_protoGet.GetVal(FDS_USERID_MINE, &sMyID), E_NO_USERID, TEXT("FDS_USERID_MINE is not in the packet"));
					ASSERT_BOOL2(m_protoGet.GetVal(FDS_ACCNO_MY, &sMyAcc), E_NO_ACNTNO, TEXT("FDN_ACCNO_MY is not in the packet"));
					m_protoGet.GetVal(FDS_USER_NICK_NM, &sNickNm);
				}
				catch (CIRExcept e)
				{
					ReturnError(pRecvData->sock, "", e.GetCode());
					g_log.logW(NOTIFY, e.GetMsgW());
					break;
				}

				if (sCode.compare(__ALPHA::CODE_LOGON) == 0)
				{
					LoginProcess(sMyID, sMyAcc, sCode, sMasterSlaveTp, sNickNm, pRecvData);

					//g_log.log(DEBUG_, "Publisher_ReArrange 호출 after (LoginProcess)(%d)(%s)", pRecvData->sock, sMyID.c_str());
					Publisher_ReArrange();
				}
				//else if (sCode.compare(__ALPHA::CODE_LOGOFF) == 0)
				//{
					//g_log.log(INFO, "LogOff 수신(%s)", sMyID.c_str());
				//}
				else if (sCode.compare(__ALPHA::CODE_MASTER_ORDER) == 0)
				{
					g_log.log(DEBUG_, "Post WM_SAVE_ORDER (ThreadID:%d)", GetCurrentThreadId());
					Publish_Order(pRecvData);
				}
				else if (sCode.compare(__ALPHA::CODE_COPIER_ORDER) == 0)
				{
					//g_log.log(INFO, "code:CODE_SLAVE_ORDER");
					TAG_BUF* buf = new TAG_BUF;
					sprintf(buf->buf, "%.*s", pRecvData->len, pRecvData->data);
					PostThreadMessage(m_unSaveThreadId, WM_SAVE_ORDER, pRecvData->len, (LPARAM)buf);
				}
				else if (sCode.compare(__ALPHA::CODE_USER_LOG) == 0)
				{
					TAG_BUF* buf = new TAG_BUF;
					sprintf(buf->buf, "%.*s", pRecvData->len, pRecvData->data);
					PostThreadMessage(m_unSaveThreadId, WM_SAVE_USERLOG, pRecvData->len, (LPARAM)buf);
				}
				else
				{
					g_log.log(NOTIFY, "undefined code:%s;", sCode.c_str());
				}
				
			} // if (msg.message == WM_PASS_DATA)

		}// while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))

	}// while (Is_TimeOfStop(1))

	g_log.log(INFO, "[%s] Master Thread ends....", m_masterInfo.sUserId.c_str());
}

// When a user(a Master or a Copier) logged on/off, re-arrange the publish threads
void CMasterChannel::Publisher_ReArrange()
{
	int nRetry = 0;
	int nLoop = 0;
	CPublisher* pPub = NULL;


	// Make sure that all Publishers are not publishing now.
	while (m_sync.IsCleared() == FALSE)
	{
		if (nRetry++ > m_nSyncRetryCnt)
		{
			g_log.log(ERR, "Publishers are still doing something. Failed to Re-Arrange");
			return;
		}
		Sleep(m_nSyncRetrySleep);
	}

	///
	Publisher_ClearAll(); 
	///

	EnterCriticalSection(&m_csCopiers);
	for (m_itCopiers = m_mapCopiers.begin(); m_itCopiers != m_mapCopiers.end(); m_itCopiers++)
	{
		int nRemain = nLoop % m_nCopierPerPublisher;
		if (nRemain == 0)
		{
			// save the previous publisher in the list
			if (nLoop > 0)
			{
				m_lstPublisher.push_back(pPub);
			}
			pPub = new CPublisher( &m_sync, m_masterInfo.sUserId, m_masterInfo.sAccNo);
		}

		pPub->AddCopier((*m_itCopiers).second->sock, (*m_itCopiers).second->UserID, (*m_itCopiers).second->sAccNo);

		nLoop++;
	}
	if(pPub)	m_lstPublisher.push_back(pPub);

	LeaveCriticalSection(&m_csCopiers);
}



void CMasterChannel::Publisher_ClearAll()
{
	EnterCriticalSection(&m_csPublisher);
	deque< CPublisher*>::iterator it;
	for (it = m_lstPublisher.begin(); it != m_lstPublisher.end(); it++)
	{
		PostThreadMessage((*it)->GetMyThreadID(), WM_DIE, 0, 0);
	}
	for (it = m_lstPublisher.begin(); it != m_lstPublisher.end(); it++)
	{
		delete (*it);
	}
	m_lstPublisher.clear();
	LeaveCriticalSection(&m_csPublisher);
}



void CMasterChannel::ReturnError(SOCKET sock, string sUserID, int nErrCode)
{
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };

	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_RETURN_ERROR);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, sUserID.c_str());
	set.SetVal(FDN_ERR_CODE, nErrCode);
	set.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());

	int nLen = set.Complete(zSendBuff);

	RequestSendIO(sock, sUserID, zSendBuff, nLen);
}


void CMasterChannel::ToMaster_Send_DupLogon(char* pNewClientIp)
{
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };

	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_DUP_LOGON);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, m_masterInfo.sUserId);
	set.SetVal(FDS_USERID_MASTER, m_masterInfo.sUserId);
	set.SetVal(FDS_ACCNO_MINE, m_masterInfo.sAccNo);
	set.SetVal(FDS_ACCNO_MASTER, m_masterInfo.sAccNo);
	set.SetVal(FDS_CLIENT_IP, pNewClientIp);
	set.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());

	int nLen = set.Complete(zSendBuff);

	RequestSendIO(m_masterInfo.sock, m_masterInfo.sUserId, zSendBuff, nLen);
	g_log.log(INFO, "[ToMaster_Send_DupLogon] Close previous ID due to Dup logon(Old:%s)(New:%s)",
		m_masterInfo.sIPAddr.c_str(), pNewClientIp);

}

/*

struct SESSION_TM
{
SOCKET	sock;
DWORD	dwPing;
DWORD	dwRePing;
};*/
void CMasterChannel::LoginProcess(string sMyId, string sMyAcc, string sCode, string sMasterSlaveTp, string sNickNm, RECV_DATA* pRecvData)
{
	g_log.log(DEBUG_, "[LoginProcess]start...(%d)(%s)", pRecvData->sock, sMyId.c_str());

	//	client public ip 추출
	SOCKADDR_IN peer_addr;
	int			peer_addr_len = sizeof(peer_addr);
	char zClientIp[128] = { 0, };
	if (getpeername(pRecvData->sock, (sockaddr*)&peer_addr, &peer_addr_len) == 0)
	{
		strcpy(zClientIp, inet_ntoa(peer_addr.sin_addr));
	}

	BOOL bIsMaster = __ALPHA::IsMaster(sMasterSlaveTp);
	if (bIsMaster)
	{
		if (Master_IsAlreadyLogon())
		{
			Master_Mark_DupLogon();
			ToMaster_Send_DupLogon(zClientIp);
			g_log.log(DEBUG_, "(%s)(%d)Master DupLogon(Previous:%s)", sMyId.c_str(), m_masterInfo.sock, zClientIp);
		}
		m_masterInfo.sock = pRecvData->sock;
		m_masterInfo.bMasterLogon = TRUE;
		m_masterInfo.sUserId = sMyId;
		m_masterInfo.sIPAddr = zClientIp;
		m_protoGet.GetVal(FDS_ACCNO_MASTER, &m_masterInfo.sAccNo);
		//g_log.log(DEBUG_, "[LoginProcess](%s)(%d)Master Logon", sMyId.c_str(), pRecvData->sock);
	}
	else if (bIsMaster==FALSE)
	{
		//g_log.log(DEBUG_, "[LoginProcess](%s)(%d)-1", sMyId.c_str(), pRecvData->sock);

		SESSION_TM* pSess = new SESSION_TM;
		pSess->sock = pRecvData->sock;
		pSess->dwLastPing = 0;
		pSess->nPingCnt = 0;
		pSess->sAccNo = sMyAcc;
		pSess->sNickNm = sNickNm;
		strcpy(pSess->UserID, sMyId.c_str());
		pSess->sIPAddr = zClientIp;

		//g_log.log(DEBUG_, "[LoginProcess](%s)(%d)-2", sMyId.c_str(), pRecvData->sock);

		Copier_CheckAndSend_DupLogon(sMyId, pSess, zClientIp);

		//g_log.log(DEBUG_, "[LoginProcess](%s)(%d)-3", sMyId.c_str(), pRecvData->sock);

		Add_Copier(sMyId, pSess);

		//g_log.log(DEBUG_, "[LoginProcess](%s)(%d)-4", sMyId.c_str(), pRecvData->sock);

		ToMaster_Send_CopierOnOff(EN_ONLINE, pSess->UserID, pSess->sAccNo);

		//g_log.log(DEBUG_, "[LoginProcess](%s)(%d)Copier Logon", sMyId.c_str(), pRecvData->sock);
	}

	

	// Return to the Client who has sent this logon packet.
	char zSendBuff[512] = { 0, };
	char zTime			[32] = { 0, };
	char zMasterLogonYN	[2] = { 0, };
	if (m_masterInfo.bMasterLogon) zMasterLogonYN[0] = 'Y';
	else				zMasterLogonYN[0] = 'N';

	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE,	__ALPHA::CODE_LOGON);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);	

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER,		zTime);
	set.SetVal(FDS_USERID_MINE,		sMyId.c_str());
	set.SetVal(FDS_USERID_MASTER, m_masterInfo.sUserId.c_str());
	set.SetVal(FDS_MASTER_LOGON_YN, zMasterLogonYN);
	set.SetVal(FDS_ACCNO_MASTER, m_masterInfo.sAccNo.c_str());
	set.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());

	int nLen = set.Complete(zSendBuff);
	g_log.log(INFO, "[LoginProcess]Return Login OK to client(%d)(ID:%s)(IP:%s)", pRecvData->sock, sMyId.c_str(), zClientIp);
	RequestSendIO(pRecvData->sock, sMyId, zSendBuff, nLen);


	// If this guy is a Master, tell all Copiers that the Master is online now.
	if (__ALPHA::IsMaster(sMasterSlaveTp))
	{
		ToAllCopiers_Send_MasterLogon(set);
	}
	else
	{
		//TODO ToMaster_Request_CurrentOrders();
	}


	// Save in DB
	TAG_BUF* buf = new TAG_BUF;
	memcpy(buf->buf, sMyId.c_str(), sMyId.size());
	buf->bufsize = pRecvData->len;
	PostThreadMessage(m_unSaveThreadId, WM_LOGON, (WPARAM)pRecvData->sock, (LPARAM)buf);
}

void CMasterChannel::Copier_CheckAndSend_DupLogon(string sMyId, _In_ SESSION_TM* pSess, char* pzNewIp)
{
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };

	EnterCriticalSection(&m_csCopiers);

	map < COPIER_ID, SESSION_TM*>::iterator it;
	for (it = m_mapCopiers.begin(); it != m_mapCopiers.end(); )
	{
		string sCopierID = (*it).first;
		SESSION_TM* pSess = (*it).second;

		if (sCopierID != sMyId) {
			it++;
			continue;
		}

		CProtoSet	set;
		set.Begin();
		set.SetVal(FDS_CODE, __ALPHA::CODE_DUP_LOGON);
		set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

		__ALPHA::Now(zTime);
		set.SetVal(FDS_TM_HEADER, zTime);
		set.SetVal(FDS_USERID_MINE, sCopierID);
		set.SetVal(FDS_USERID_MASTER, m_masterInfo.sUserId);
		set.SetVal(FDS_ACCNO_MINE, pSess->sAccNo);
		set.SetVal(FDS_ACCNO_MASTER, m_masterInfo.sAccNo);
		set.SetVal(FDS_CLIENT_IP, pzNewIp);
		set.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());

		int nLen = set.Complete(zSendBuff);

		RequestSendIO(pSess->sock, sCopierID, zSendBuff, nLen);
		//g_log.log(INFO, "[Copier_CheckAndSend_DupLogon]Send Dup Logon(%s)", sCopierID.c_str());

		// Save on Database ==> only when recognize socket has been closed
		//TAG_BUF* buf = new TAG_BUF;
		//memcpy(buf->buf, zSendBuff, nLen);
		//PostThreadMessage(m_unSaveThreadId, WM_LOGOUT, nLen, (LPARAM)buf);

		// move to another map
		EnterCriticalSection(&m_csDuplicated);
		m_mapDuplicatedCopiers[sCopierID] = (*it).second;
		LeaveCriticalSection(&m_csDuplicated);
		
		// erase from current map
		it = m_mapCopiers.erase(it);

	} // for (it = m_mapCopiers.begin(); it != m_mapCopiers.end(); it++)

	LeaveCriticalSection(&m_csCopiers);
}

void CMasterChannel::ToMaster_Send_CopierOnOff(int nOnOffline, string sUserID_Copier, string sAcc_Copier)
{
	if (Master_IsAlreadyLogon() == FALSE)
		return;

	char zSendBuff[512] = { 0, };
	char zTime[32] = { 0, };

	CProtoSet	set;
	set.Begin();
	if (nOnOffline == EN_ONLINE)
		set.SetVal(FDS_CODE, __ALPHA::CODE_ONLINE_COPIERS);
	else if (nOnOffline == EN_OFFLINE)
		set.SetVal(FDS_CODE, __ALPHA::CODE_OFFLINE_COPIERS);
	else
		assert(false);

	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, m_masterInfo.sUserId);
	set.SetVal(FDS_USERID_MASTER, m_masterInfo.sUserId);
	set.SetVal(FDS_ACCNO_MASTER, m_masterInfo.sAccNo);
	set.SetVal(FDS_USERID_COPIER, sUserID_Copier);
	set.SetVal(FDS_ACCNO_COPIER, sAcc_Copier);
	set.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());

	int nLen = set.Complete(zSendBuff);
	RequestSendIO(m_masterInfo.sock, m_masterInfo.sUserId, zSendBuff, nLen);


	//g_log.log(INFO, "[ToMaster_Send_CopierOnOff][SOCK:%d](MasterID:%s)(CopierID:%s)(%s)",
	//		m_masterInfo.sock, m_masterInfo.sUserId.c_str(), sUserID_Copier.c_str(), zSendBuff);

}

void CMasterChannel::Add_Copier(string sMyId, _In_ SESSION_TM* pSess)
{
	EnterCriticalSection(&m_csCopiers);
	m_mapCopiers[sMyId] = pSess;
	LeaveCriticalSection(&m_csCopiers);

	//if (g_bDebugLog) g_log.log(INFO, "[Add_Copier](Copier:%s)(count:%d)", sMyId.c_str(), m_mapCopiers.size());
}


BOOL CMasterChannel::Remove_Master(string sUserID, SOCKET sock)
{
	// 중복로그인으로 기존SOCKET 이 CLOSE 하는 경우 별도의 작업을 하지 않는다.
	if (Master_IsMarked_DupLogon() == TRUE)
	{
		Master_UnMark_DupLogon();
		//g_log.log(DEBUG_, "[Remove_Master]Closing Previous (%s)(%d)", sUserID.c_str(), sock);
	}
	else
	{
		ToAllCopiers_CloseForcely();
		Master_SetLogOff();
		//g_log.log(DEBUG_, "[Remove_Master]Closing (%s)(%d)", sUserID.c_str(), sock);
	}

	// Save in DB
	TAG_BUF* buf = new TAG_BUF;
	memcpy(buf->buf, sUserID.c_str(), sUserID.size());
	buf->bufsize = sUserID.size();
	PostThreadMessage(m_unSaveThreadId, WM_LOGOUT, (WPARAM)sock, (LPARAM)buf);


	return TRUE;
}

//------------------------------------------------------------------------
// Check Duplicate Map of duplicating loggon
BOOL CMasterChannel::Remove_Copier_DupPrevious(string sUserID, SOCKET sock)
{
	map < COPIER_ID, SESSION_TM*>::iterator it = m_mapDuplicatedCopiers.find(sUserID);
	if (it == m_mapDuplicatedCopiers.end())
		return FALSE;

	SESSION_TM* pSess = (SESSION_TM*)(*it).second;
	delete pSess;
	m_mapDuplicatedCopiers.erase(it);

	//g_log.log(DEBUG_, "[Remove_Copier] Dup LogOff(%s)(%d)", sUserID.c_str(), sock);
	
	//ToMaster_Send_CopierOnOff(EN_OFFLINE, pSess->UserID, pSess->sAccNo);

	return TRUE;
}

BOOL CMasterChannel::Remove_Copier_Current(string sUserID, SOCKET sock)
{
	map < COPIER_ID, SESSION_TM*>::iterator it2 = m_mapCopiers.find(sUserID);
	if (it2 == m_mapCopiers.end())
		return FALSE;

	SESSION_TM* p = (*it2).second;
	SESSION_TM* pSess = (SESSION_TM*)(*it2).second;
	ToMaster_Send_CopierOnOff(EN_OFFLINE, pSess->UserID, pSess->sAccNo);
	delete pSess;
	m_mapCopiers.erase(it2);

	//
	//g_log.log(DEBUG_, "Publisher_ReArrange 호출 IN (Remove_Copier_Current)(%d)(%s)", sock, sUserID.c_str());
	Publisher_ReArrange();
	//

	g_log.log(DEBUG_, "[Remove_Copier] LogOff(%s)(%d)", sUserID.c_str(), sock);

	return TRUE;
}

void CMasterChannel::Remove_Copier(string sUserID, SOCKET sock)
{
	g_log.log(DEBUG_, "[Remove_Copier] start(%s)(%d)", sUserID.c_str(), sock);
	
	BOOL bDupRet = FALSE, bRet = FALSE;

	EnterCriticalSection(&m_csDuplicated);
	bDupRet = Remove_Copier_DupPrevious(sUserID, sock);
	LeaveCriticalSection(&m_csDuplicated);
	
	if (bDupRet == FALSE)
	{
		EnterCriticalSection(&m_csCopiers);
		bRet = Remove_Copier_Current(sUserID, sock);
		LeaveCriticalSection(&m_csCopiers);
	}

	if (bDupRet || bRet )
	{
		// Save in DB
		TAG_BUF* buf = new TAG_BUF;
		memcpy(buf->buf, sUserID.c_str(), sUserID.size());
		buf->bufsize = sUserID.size();
		PostThreadMessage(m_unSaveThreadId, WM_LOGOUT, (WPARAM)sock, (LPARAM)buf);
	}
}



void CMasterChannel::ToAllCopiers_Send_MasterLogon(_In_ CProtoSet& protoSet)
{
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };

	protoSet.SetVal(FDS_CODE, __ALPHA::CODE_LOGON_MASTER);
	protoSet.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());

	ZeroMemory(zSendBuff, sizeof(zSendBuff));
	int nLen = protoSet.Complete(zSendBuff);

	EnterCriticalSection(&m_csCopiers);
	map < string, SESSION_TM*>::iterator it;
	for (it = m_mapCopiers.begin(); it != m_mapCopiers.end(); it++)
	{
		SESSION_TM* pSess = (*it).second;
		SOCKET sock = pSess->sock;

		// Copier 들에게만 보낸다.
		RequestSendIO(sock, (*it).first, zSendBuff, nLen);
		g_log.log(INFO, "[LoginProcess]Tell all Copiers that Master Logon(SOCK:%d)(Copier ID:%s)", sock, pSess->UserID);
	}
	LeaveCriticalSection(&m_csCopiers);
}



// MASTER 에 달려있는 모든 Slave 들 강제 로그오프
void CMasterChannel::ToAllCopiers_CloseForcely()
{
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };

	EnterCriticalSection(&m_csCopiers);

	map < COPIER_ID, SESSION_TM*>::iterator it;
	for (it = m_mapCopiers.begin(); it != m_mapCopiers.end(); it++)
	{
		string sCopierID		= (*it).first;
		SESSION_TM* pSess	= (*it).second;
		CProtoSet	set;
		set.Begin();
		set.SetVal(FDS_CODE,	__ALPHA::CODE_LOGOFF_MASTER);
		set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

		__ALPHA::Now(zTime);
		set.SetVal(FDS_TM_HEADER,		zTime);
		set.SetVal(FDS_USERID_MINE, sCopierID);
		set.SetVal(FDS_USERID_MASTER, m_masterInfo.sUserId);
		set.SetVal(FDS_ACCNO_MINE, pSess->sAccNo);
		set.SetVal(FDS_ACCNO_MASTER, m_masterInfo.sAccNo);
		set.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());

		int nLen = set.Complete(zSendBuff);

		RequestSendIO(pSess->sock, sCopierID, zSendBuff, nLen);
		if(g_bDebugLog) g_log.log(INFO, "[ToAllCopiers_CloseForcely]Send Master LogOff(%s)", sCopierID.c_str());
		 
		//// Save on Database
		//TAG_BUF* buf = new TAG_BUF;
		//memcpy(buf->buf, zSendBuff, nLen);
		//PostThreadMessage(m_unSaveThreadId, WM_LOGOUT, nLen, (LPARAM)buf);

		//delete (*it).second;
		//it = m_mapCopiers.erase(it);

	} // for (it = m_mapCopiers.begin(); it != m_mapCopiers.end(); it++)

	LeaveCriticalSection(&m_csCopiers);
}

void CMasterChannel::Publish_Order(RECV_DATA* pRecvData)
{
	deque< CPublisher*>::iterator it;

	for (it = m_lstPublisher.begin(); it != m_lstPublisher.end(); it++)
	{
		char* pData = g_memPool.get();
		strcpy(pData, m_protoGet.GetRecvData());
		int nLen = strlen(pData);

		PostThreadMessage((*it)->GetMyThreadID(), WM_PUBLISH_ORD, nLen, (LPARAM)pData);
	}


	// Post data to save on DB
	TAG_BUF* buf = new TAG_BUF;
	memcpy(buf->buf, pRecvData->data, pRecvData->len);
	PostThreadMessage(m_unSaveThreadId, WM_SAVE_ORDER, pRecvData->len, (LPARAM)buf);
}


void CMasterChannel::RequestSendIO(SOCKET sock, string sID, char* pSendBuf, int nSendLen)
{
	BOOL  bRet			= TRUE;
	DWORD dwOutBytes	= 0;
	DWORD dwFlags		= 0;
	IO_CONTEXT* pSend	= NULL;

	COMPLETION_KEY*	pCK = new COMPLETION_KEY;
	ZeroMemory(pCK, sizeof(COMPLETION_KEY)); 
	pCK->sock		= sock;
	pCK->sUserID	= sID;

	try {
		pSend = new IO_CONTEXT;

		ZeroMemory(pSend, sizeof(IO_CONTEXT));
		CopyMemory(pSend->buf, pSendBuf, nSendLen);
		//	pSend->sock	= sock;
		pSend->wsaBuf.buf	= pSend->buf;
		pSend->wsaBuf.len	= nSendLen;
		pSend->context		= CTX_RQST_SEND;

		int nRet = WSASend(pCK->sock
			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
			, 1					// wsaBuf 포인터 갯수
			, &dwOutBytes		// 전송된 바이트 수
			, dwFlags
			, &pSend->overLapped	// overlapped 포인터
			, NULL);
		if (nRet == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				g_log.logW(LOGTP_ERR, TEXT("WSASend error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
			//else
			//	printf("WSA_IO_PENDING....\n");
		}
		//else
		//	g_log.log(INFO, "[SEND](%s)", pSend->buf);
	}
	catch (...) {
		g_log.logW(LOGTP_ERR, TEXT("WSASend TRY CATCH"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;

	return;
}

//
//// 마스터에게 current 주문정보 요청
//void	CMasterChannel::ToMaster_Request_CurrentOrders()
//{
//	// RETURN to CLIENT
//	char zSendBuff[512] = { 0, };
//	char zTime[32] = { 0, };
//
//	CProtoSet	set;
//	set.Begin();
//	set.SetVal(FDS_CODE, __ALPHA::CODE_OPEN_ORDERS);
//	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);
//
//	__ALPHA::Now(zTime);
//	set.SetVal(FDS_TM_HEADER, zTime);
//	set.SetVal(FDS_USERID_MINE, m_masterInfo.sUserId);
//	set.SetVal(FDS_USERID_MASTER, m_masterInfo.sUserId);
//	set.SetVal(FDS_ACCNO_MASTER, m_masterInfo.sAccNo);
//	set.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());
//
//	int nLen = set.Complete(zSendBuff);
//	g_log.log(INFO, "[ToMaster_Request_CurrentOrders](ID:%s)", m_masterInfo.sUserId.c_str());
//	RequestSendIO(m_masterInfo.sock, m_masterInfo.sUserId, zSendBuff, nLen);
//}



//
//// Log off the specific ID forcelly
//BOOL CMasterChannel::ToCopier_CloseForcely(string sUserID, BOOL bAlreadClosed)
//{
//	char zSendBuff[MAX_BUF] = { 0, };
//	char zTime[32] = { 0, };
//
//	map < string, SESSION_TM*>::iterator it = m_mapCopiers.find(sUserID);
//	if ( it== m_mapCopiers.end())
//		return FALSE;
//
//	BOOL bDel = FALSE;
//	//if (bAlreadClosed == FALSE) ?
//	{
//		SESSION_TM* pSess = (*it).second;
//		CProtoSet	set;
//		set.Begin();
//		set.SetVal(FDS_CODE, __ALPHA::CODE_LOGOFF);
//		set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);
//
//		__ALPHA::Now(zTime);
//		set.SetVal(FDS_TM_HEADER, zTime);
//		set.SetVal(FDS_USERID_MINE, sUserID.c_str());
//		set.SetVal(FDS_USERID_MASTER, m_masterInfo.sUserId);
//		set.SetVal(FDS_ACCNO_MINE, pSess->sAccNo);
//		set.SetVal(FDS_ACCNO_MASTER, m_masterInfo.sAccNo);
//		set.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());
//
//		int nLen = set.Complete(zSendBuff);
//
//		RequestSendIO(pSess->sock, sUserID, zSendBuff, nLen);
//		if (g_bDebugLog) g_log.log(INFO, "[ToCopier_CloseForcely](Copier Sock:%d)(Copier:%s)", pSess->sock, sUserID.c_str());
//
//		bDel = TRUE;
//
//		// Save on Database
//		TAG_BUF* buf = new TAG_BUF;
//		memcpy(buf->buf, zSendBuff, nLen);
//		PostThreadMessage(m_unSaveThreadId, WM_LOGOUT, nLen, (LPARAM)buf);
//
//	} // if (sID == sSlaveID)
//
//	return bDel;
//}
//