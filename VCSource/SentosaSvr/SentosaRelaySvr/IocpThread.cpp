
#include "IocpThread.h"
#include <process.h>
#include "../../Common/Util.h"
#include "../../Common/LogMsg.h"
#include "../../Common/IRExcept.h"
#include "../../Common/AlphaInc.h"
#include "../../Common/TimeInterval.h"
#include "../Common/ErrCodes.h"
#include <assert.h>

#define __REF_1_HEAD__
#define __REF_1_TAIL__

#define __REF_2_HEAD__
#define __REF_2_TAIL__

//extern CLogMsg	g_log;
extern CConfig	g_config;
extern BOOL		g_bDebugLog;
extern CProtoGetList	g_listProtoGet;

CIocp::CIocp()
{
	m_hCompletionPort	= NULL;
	m_hListenEvent		= NULL;
	m_sockListen		= INVALID_SOCKET;
	m_hListenEvent		= NULL;
	m_bRun				= TRUE;
	m_pDbForLogging = NULL;
	m_lIocpThreadIdx = 0;

	m_hThread_Listen = m_hParsing = m_hThread_Config = NULL;	// m_hNoMoreOpenThread = m_hWorkerThread = NULL;
	m_unThread_Listen = m_unParsing = m_unThread_Config = 0;	// m_unNoMoreOpenThread = m_unWorkerThread = 0;

}
CIocp::~CIocp()
{
	Finalize();
}


void CIocp::Finalize()
{
	m_bRun = FALSE;

	Reset_Session();

	EnterCriticalSection(&m_csCK);
	m_mapCK.clear();
	LeaveCriticalSection(&m_csCK);

	LockUser();
	IT_MAP_USER itSymbol;
	for (itSymbol = m_mapUser.begin(); itSymbol != m_mapUser.end(); itSymbol++)
	{
		delete (*itSymbol).second;
	}
	m_mapUser.clear();
	UnlockUser();

	for (UINT i = 0; i < WORKTHREAD_CNT; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			, 0
			, NULL
			, NULL
		);
	}

	CloseListenSock();

	SAFE_CLOSEHANDLE(m_hCompletionPort);

	DeleteCriticalSection(&m_csUser);
	DeleteCriticalSection(&m_csCK);
	DeleteCriticalSection(&m_csDeletingCK);

	WSACleanup();

	//delete m_pDBPool;
}

BOOL CIocp::Initialize_RelaySvrID()
{
	char zQ[1024];
	auto_ptr< CMySqlHandler> handler(new CMySqlHandler);

	handler->Initialize(g_config.getDBIp(), g_config.getDBPort(), g_config.getDBUserId()
		, g_config.getDBUserPwd(), g_config.getDBName());
	if (!handler->OpenDB())
	{
		LOGGING(ERR, TRUE, "[Initialize_RelaySvrID]DB Open Error(%s)", handler->GetMsg());
		return FALSE;
	}

	sprintf(zQ, "select svr_id from relaysvr_mst where svr_ip='%s';", g_config.getListenIP());
	auto_ptr<Rs> rs(handler->Execute(zQ));
	if (!rs->IsValid())
	{
		LOGGING(ERR, TRUE, "[Initialize_RelaySvrID]DB EXEC Error(%s)", handler->GetMsg());
		return FALSE;
	}
	if (rs->getRecordCnt() == 0)
	{
		LOGGING(ERR, TRUE, "[Initialize_RelaySvrID]No Recordsets");
		return FALSE;
	}
	
	for (int i = 0; i < rs->getRecordCnt(); i++)
	{
		string sID;
		if (rs->getString(i, "svr_id", &sID))
			m_lstRelaySvrID.push_back(sID);
	}
	if (m_lstRelaySvrID.size() == 0)
	{
		LOGGING(ERR, TRUE, "[Initialize_RelaySvrID]No RelaySvr ID");
		return FALSE;
	}
	return TRUE;
}



BOOL CIocp::Reset_Session()
{
	char zQ[1024];
	auto_ptr< CMySqlHandler> handler(new CMySqlHandler);

	handler->Initialize(g_config.getDBIp(), g_config.getDBPort(), g_config.getDBUserId()
		, g_config.getDBUserPwd(), g_config.getDBName());
	if (!handler->OpenDB())
	{
		LOGGING(ERR, TRUE, "[Reset_Session]DB Open Error(%s)", handler->GetMsg());
		return FALSE;
	}

	for (auto it : m_lstRelaySvrID)
	{
		sprintf(zQ, "CALL sp_reset_session('%s');", it.c_str());

		auto_ptr<Rs> rs(handler->Execute(zQ));
		string sMsg; int ret_code;
		if (!rs->Is_Successful_ExcutingSP(ret_code, sMsg))
		{
			LOGGING(ERR, TRUE, "[Reset_Session]%s", sMsg.c_str());
			return FALSE;
		}

	}
	
	return TRUE;
}


BOOL CIocp::Initialize( )
{
	InitializeCriticalSection(&m_csUser);
	InitializeCriticalSection(&m_csCK);
	InitializeCriticalSection(&m_csDeletingCK);
	
	if (g_config.ReLoad_ConfigInfo(TRUE) != CNFG_SUCC)
		return FALSE;

	if (!Initialize_RelaySvrID())
		return FALSE;
	if (!Reset_Session())
		return FALSE;

	if (!InitListen()) {
		return FALSE;
	}
	LOGGING(INFO, TRUE, "Init Listen(%s)(%d)", g_config.getListenIP(), g_config.getListenPort());

	m_hThread_Listen	= (HANDLE)_beginthreadex(NULL, 0, &Thread_Listen, this, 0, &m_unThread_Listen);
	m_hParsing			= (HANDLE)_beginthreadex(NULL, 0, &Thread_Parsing, this, 0, &m_unParsing);
	m_hThread_Login		= (HANDLE)_beginthreadex(NULL, 0, &Thread_Login, this, 0, &m_unThread_Login);
	m_hThread_Config	= (HANDLE)_beginthreadex(NULL, 0, &Thread_Config, this, 0, &m_unThread_Config);
	m_hThread_Db		= (HANDLE)_beginthreadex(NULL, 0, &Thread_DBProc, this, 0, &m_unThread_Db);

	// CPU의 수를 알기 위해서....IOCP에서 사용할 쓰레드의 수는 cpu개수 또는 cpu*2
	SYSTEM_INFO         systemInfo;
	GetSystemInfo(&systemInfo);
	m_dwThreadCount = 1;	// systemInfo.dwNumberOfProcessors;

	
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_dwThreadCount);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("IOCP Create Error:%d"), GetLastError());
		return FALSE;
	}

	// 실제로 recv와 send를 담당할 스레드를 생성한다.
	
	for (unsigned int n = 0; n < m_dwThreadCount; n++)
	{
		UINT dwID;
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &Thread_Iocp, this, 0, &dwID);
		CloseHandle(h);
	}

	//if (!CreateSymbolThread())
	//	return FALSE;



	return TRUE;
}


BOOL CIocp::InitListen()
{
	LOGGING(INFO, TRUE, TEXT("CIocp::InitListen() starts.........."));
	CloseListenSock();

	WORD	wVersionRequired;
	WSADATA	wsaData;

	//// WSAStartup
	wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("WSAStartup Error:%d"), GetLastError());
		return FALSE;
	}

	//DumpWsaData(&wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		LOGGING(LOGTP_ERR, TRUE, TEXT("RequiredVersion not Usable"));
		return FALSE;
	}


	// Create a listening socket 
	if ((m_sockListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		LOGGING(ERR,TRUE, TEXT("create socket error: %d"), WSAGetLastError());
		return FALSE;
	}


	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = inet_addr(g_config.getListenIP());
	InternetAddr.sin_port = htons(g_config.getListenPort());

	BOOL opt = TRUE;
	int optlen = sizeof(opt);
	setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (const char far *)&opt, optlen);


	if (::bind(m_sockListen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		LOGGING(ERR, TRUE, TEXT("bind error (ip:%s) (port:%d) (err:%d)"), g_config.getListenIP(), g_config.getListenPort(), WSAGetLastError());
		return FALSE;
	}
	// Prepare socket for listening 
	if (listen(m_sockListen, 5) == SOCKET_ERROR)
	{
		LOGGING(ERR, TRUE, TEXT("listen error: %d"), WSAGetLastError());
		return FALSE;
	}

	m_hListenEvent = WSACreateEvent();
	if (WSAEventSelect(m_sockListen, m_hListenEvent, FD_ACCEPT)) {

		LOGGING(ERR, TRUE, TEXT("WSAEventSelect for accept error: %d"), WSAGetLastError());
		return FALSE;
	}
	return TRUE;
}

/*
BOOL WINAPI dQueuedCompletionStatus(
_In_     HANDLE       CompletionPort,
_In_     DWORD        dwNumberOfBytesTransferred,
_In_     ULONG_PTR    dwCompletionKey,
_In_opt_ LPOVERLAPPED lpOverlapped
);
*/


void CIocp::CloseListenSock()
{
	SAFE_CLOSEHANDLE(m_hListenEvent);
	if (m_sockListen != INVALID_SOCKET) {
		struct linger ling;
		ling.l_onoff = 1;   // 0 ? use default, 1 ? use new value
		ling.l_linger = 0;  // close session in this time
		setsockopt(m_sockListen, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));
		//-We can avoid TIME_WAIT on both of client and server side as we code above.
		closesocket(m_sockListen);
	}
	SAFE_CLOSESOCKET(m_sockListen);
}

VOID CIocp::SendMessageToIocpThread(int Message)
{
	for( int i=0; i<WORKTHREAD_CNT; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			,0
			, (ULONG_PTR)Message
			, NULL
		);
	}
}

void CIocp::Try_Delete_ClosedCK()
{
	EnterCriticalSection(&m_csDeletingCK);

	list<COMPLETION_KEY*>::iterator it;
	for (it = m_lstDeletingCK.begin(); it != m_lstDeletingCK.end(); )
	{
		if ((*it)->Is_BeingUsed()) {
			++it;
			continue;
		}

		COMPLETION_KEY* pCK = (*it);

		if(pCK->cSockTp==DEF_CLIENT_SOCKTP_RECV)
			RemoveApp_ClosingMainSocket(pCK);

		CloseClientSock(pCK->sock);
		delete pCK;

		it = m_lstDeletingCK.erase(it);

	}
	LeaveCriticalSection(&m_csDeletingCK);
}

void CIocp::CloseClientSock(SOCKET sock)
{
	shutdown(sock, SD_BOTH);
	// TIME-WAIT 없도록
	struct linger structLinger;
	structLinger.l_onoff = TRUE;
	structLinger.l_linger = 0;
	setsockopt(sock, SOL_SOCKET, SO_LINGER, (LPSTR)&structLinger, sizeof(structLinger));
	closesocket(sock);	
}

void CIocp::AddList_DeletingCK(COMPLETION_KEY* pCompletionKey)
{
	//
	RemoveCK_FromMap(pCompletionKey->sock);

	//
	EnterCriticalSection(&m_csDeletingCK);
	m_lstDeletingCK.push_back(pCompletionKey);
	LeaveCriticalSection(&m_csDeletingCK);
}


/*
1.	WSASend / WSARecv 를 호출한 경우는 사용한 socket 과 CK 가 연결되어 있으므로
pCompletionKey 를 통해서 CK 의 포인터가 나온다.

2.	PostQueuedCompletionStatus 를 호출한 경우는 socket 이 사용되지 않으므로
이때는 WM_MSG 를 보내도록 한다.

3.	확장된 OVERLAPPED 에 context 필드가 있으므로 여기에 CTX_DIE, CTX_RQST_SEND, CTX_RQST_RECV 를 채워서 보낸다.

*/
unsigned WINAPI CIocp::Thread_Iocp(LPVOID lp)
{
	CIocp* pThis = (CIocp*)lp;

	COMPLETION_KEY	*pCK = NULL;
	IO_CONTEXT		*pIoContext = NULL;
	DWORD			dwBytesTransferred = 0;
	DWORD           dwIoSize = 0;
	DWORD           dwRecvNumBytes = 0;
	DWORD           dwSendNumBytes = 0;
	DWORD           dwFlags = 0;
	LPOVERLAPPED	pOverlap = NULL;
	BOOL bRet;
	
	long iocpIdx = InterlockedIncrement(&pThis->m_lIocpThreadIdx)-1;

	LOGGING(INFO, FALSE, TEXT("[%d][%d]IOCPThread Start....."), iocpIdx, GetCurrentThreadId());

	while (pThis->m_bRun)
	{
		bRet = GetQueuedCompletionStatus(pThis->m_hCompletionPort,
			&dwIoSize,
			(LPDWORD)&pCK,
			(LPOVERLAPPED *)&pOverlap,
			INFINITE);

		pIoContext = (IO_CONTEXT*)pOverlap;

		// Finalize 에서 PostQueuedCompletionStatus 에 NULL 입력
		if (pCK == NULL)	return -1;
		
		// Finalize 에서 PostQueuedCompletionStatus 에 NULL 입력
		if (pOverlap == NULL)	return -1;

		if (pIoContext->context == CTX_DIE)	return -1;

		if (pIoContext->context == CTX_RQST_RECV)
		{
			if (FALSE == bRet)
			{
				LOGGING(INFO, TRUE, "[Close Client-1]GetQueuedCompletionStatus failed(%d)", pCK->sock);
				pThis->AddList_DeletingCK(pCK);
				continue;
			}

			if (dwIoSize == 0)
			{
				LOGGING(INFO, TRUE, "[Close Client-2]dwIoSize == 0 (%d)(%x)", pCK->sock, pCK);
				pThis->AddList_DeletingCK(pCK);
				continue;
			}

			__REF_1_HEAD__
			pCK->AddRefer();
			
			RequestRecvIO(pCK->sock);

			pThis->m_parser.AddPacket(pCK->sock, pIoContext->buf, dwIoSize);

			PostThreadMessage(pThis->m_unParsing, WM_RECEIVE_DATA, (WPARAM)0, (LPARAM)pCK);
		}

		if (pIoContext->context == CTX_RQST_SEND)
		{
			
		}
		delete pIoContext;

	} // while

	//mt4helper.disconnect();

	return 0;
}


void CIocp::RemoveApp_ClosingMainSocket(COMPLETION_KEY* pCK)
{
	LockUser();
	IT_MAP_USER itMap;
	if (Find_User(pCK->sUserID, itMap) == FALSE)
	{
		UnlockUser();
		return;
	}
	CAppManager* pApp = (*itMap).second;
	UnlockUser();

	Logoff_DBProc((char*)pCK->sUserID.c_str(), (char*)pCK->sAppId.c_str(),
					pApp->GetAppTp(pCK->sAppId), pCK->bDupLogon);

	pApp->Remove_App_OnlyForRecvSocket(pCK->sAppId, pCK->sock, pCK->cSockTp);
}


unsigned WINAPI CIocp::Thread_Parsing(LPVOID lp)
{
	CIocp*	pThis = (CIocp*)lp;
	char	zRecvBuff[__ALPHA::LEN_BUF];
	int		nLen = 0;
	string	sUserID;
	BOOL	bContinue = FALSE;

	while (pThis->m_bRun)
	{
		Sleep(1);
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == FALSE)
			continue;

		COMPLETION_KEY* pCK = (COMPLETION_KEY*)msg.lParam;
		bContinue = TRUE;
		while (bContinue)
		{
			ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
			bContinue = pThis->m_parser.GetOnePacket(pCK->sock, &nLen, zRecvBuff);
			if (nLen <= 0)
			{
				break;	//--------------------------------//
			}


			CProtoUtils util;	char zCode[32] = { 0 };
			if (util.PacketCode((char*)zRecvBuff, zCode) == NULL) {
				LOGGING(ERR, TRUE, "Packet doesn't have packet code(FDS_CODE.101)(%s)", zRecvBuff);
				pCK->Release();	continue;	//---------------------------------------------//
			}
			if (util.GetUserId(zRecvBuff, _Out_ sUserID) == false) {
				LOGGING(ERR, TRUE, "Packet Doesn't have UserID(FDS_USERID_MINE.114)(%s)", zRecvBuff);
				pCK->Release();	continue;	//---------------------------------------------//
			}

			

			if (
				(strcmp(zCode, __ALPHA::CODE_MARKET_DATA) != 0) &&
				(strcmp(zCode, __ALPHA::CODE_BALANCE) != 0) &&
				(strcmp(zCode, __ALPHA::CODE_POSITION) != 0)
				)
			{
				LOGGING(INFO, FALSE, "[RECV][%s]", zRecvBuff);
			}

			if (strcmp(zCode, __ALPHA::CODE_LOGON) == 0)
			{
				__REF_2_HEAD__
				pCK->AddRefer();
				TPacket* pPacket = new TPacket(pCK, zRecvBuff, nLen);
				LOGGING(INFO, FALSE, "[LOGIN-1]%d", pCK->sock);
				pPacket->packet = string(zRecvBuff);
				PostThreadMessage(pThis->m_unThread_Login, WM_LOGON, (WPARAM)0, (LPARAM)pPacket);
			}
			else
			{
				pThis->LockUser();
				IT_MAP_USER itMap;
				if (pThis->Find_User(sUserID, itMap))
				{
					(*itMap).second->Execute(zCode, zRecvBuff);
				}
				pThis->UnlockUser();
			}

			Sleep(0);

		} // while (bContinue)

		__REF_1_TAIL__
		pCK->Release(); /******************/


	} // while(pThis->m_bRun)
	
	
	return 0;
}


unsigned WINAPI CIocp::Thread_Login(LPVOID lp)
{
	CIocp* pThis = (CIocp*)lp;

	//char zRecvBuff[BUF_LEN];
	string sSymbol;
	//int nRet;
	while (pThis->m_bRun)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_LOGON)
			{

				TPacket* p = (TPacket*)msg.lParam;
				LOGGING(INFO, FALSE, "[LOGIN-2]%d", p->pCK->sock);
				pThis->Logon_Process(p);

				__REF_2_TAIL__
				p->pCK->Release();

				delete p;
			}
		}
	}

	return 0;
}


/*
	BrokerKey
	BrokerName
*/
BOOL CIocp::Logon_Process(TPacket * pPacket)
{
	CProtoGet get;
	int nFieldCnt = get.ParsingWithHeader((CHAR*)pPacket->packet.c_str());
	if (nFieldCnt==0)
	{
		LOGGING(ERR, TRUE, "Wrong packet(%s)", pPacket->packet.c_str());
		return FALSE;
	}
	int res = 0;
	char zBrokerName	[128] = { 0 };
	char zAccNo			[128] = { 0 };
	char zClientSockTp	[128] = { 0 };
	char zUserId		[128] = { 0 };
	char zPwd			[128] = { 0 };
	char zAppId			[128] = { 0 };
	char zLiveDemo		[128] = { 0 };
	char zMacAdddr		[128] = { 0 };
	char zMktTime		[128] = { 0 };
	EN_APP_TP enAppTp;

	enAppTp = (EN_APP_TP)get.GetValN(FDN_APP_TP);
	res += get.GetVal(FDS_BROKER,			zBrokerName);
	res += get.GetVal(FDS_ACCNO_MINE,		zAccNo);
	res += get.GetVal(FDS_USER_ID,			zUserId);
	res += get.GetVal(FDS_USER_PASSWORD,	zPwd);
	res += get.GetVal(FDS_CLIENT_SOCKET_TP,	zClientSockTp);
	res += get.GetVal(FDS_KEY,				zAppId);
	res += get.GetVal(FDS_LIVEDEMO,			zLiveDemo);
	res += get.GetVal(FDS_MAC_ADDR,			zMacAdddr);
	res += get.GetVal(FDS_TIME, zMktTime);

	LOGGING(INFO, TRUE, "(UserID:%s)(SocketTp:%c)(AppID:%s)(Broker:%s)(AccNo:%s)",
		zUserId, zClientSockTp[0], zAppId, zBrokerName, zAccNo);

	if (res < 6)
	{
		char zMsg[] = "Logon packet is not correct";
		LOGGING(ERR, TRUE, zMsg);
		ReturnError(pPacket->pCK->sock, __ALPHA::CODE_LOGON, 9999, zMsg);
		return FALSE;
	}

	// Get app instance
	CAppManager* pApp;
	IT_MAP_USER it;
	LockUser();
	if (Find_User(zUserId, it) == FALSE)
	{
		pApp = new CAppManager();
	}
	else
	{
		pApp = (*it).second;
	}

	BOOL bAlreadLogOn = pApp->Is_MainSock_AlreadyLogOn(zAppId, pPacket->pCK->sock, enAppTp, zClientSockTp[0], FALSE);
	UnlockUser();

	///////////////////////////////////////////////////////////////////////////////
	//DB PROC - Validate Credential
	///////////////////////////////////////////////////////////////////////////////
	if (zClientSockTp[0] == DEF_CLIENT_SOCKTP_RECV)
	{
		auto_ptr< CMySqlHandler> handler(new CMySqlHandler);
		handler->Initialize(g_config.getDBIp(), g_config.getDBPort(), g_config.getDBUserId()
			, g_config.getDBUserPwd(), g_config.getDBName());

		if (!handler->OpenDB())
		{
			LOGGING(ERR, TRUE, "DB Open Error(%s)", handler->GetMsg());
			ReturnError(pPacket->pCK->sock, __ALPHA::CODE_LOGON, (int)ERR_DBOPEN_ERROR, "ERR_DBOPEN_ERROR");
			return FALSE;
		}
		RET_SENTOSA ret = Logon_DBProc(handler, zUserId, zPwd, zAppId, enAppTp, zBrokerName,
			zAccNo, zLiveDemo, (char*)pPacket->pCK->sClientIp.c_str(),
			zMacAdddr, zMktTime, bAlreadLogOn);
		if (ret!=ERR_SUCCESS)
		{
			ReturnError(pPacket->pCK->sock, __ALPHA::CODE_LOGON, (int)ret, m_zMsg);
			return FALSE;
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	//App proc
	///////////////////////////////////////////////////////////////////////////////
	SOCKET sockPrevSession = INVALID_SOCKET;
	LockUser();
	string sAppId = pApp->Add_AppInfo(zUserId, enAppTp, zAppId, zBrokerName, zAccNo,
		zClientSockTp[0], pPacket->pCK->sock, pPacket->pCK->sClientIp, zMacAdddr, 
		zLiveDemo, zMktTime, bAlreadLogOn, sockPrevSession);

	m_mapUser[zUserId] = pApp;
	UnlockUser();

	if (bAlreadLogOn && (sockPrevSession != INVALID_SOCKET))
	{
		Set_DupLogOn(sockPrevSession);
		pApp->ReturnClose_Of_DupLogon(sockPrevSession);    // close the prev session
		LOGGING(INFO, TRUE, "Send LogOff of DupLogon:%d", sockPrevSession);
	}


	//Update CK
	pPacket->pCK->sAppId	= string(zAppId);
	pPacket->pCK->sUserID	= string(zUserId);
	pPacket->pCK->cSockTp	= zClientSockTp[0];

	
	/// /////////////////////////////////////////////////////////////////////////////
	// return to client
	char zSendBuff[__ALPHA::LEN_BUF] = { 0, };
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON);
	set.SetVal(FDS_SUCC_YN, "Y");
	set.SetVal(FDS_KEY, sAppId.c_str());
	//set.SetVal(FDS_RELAY_IP, sRelaySvrIp);
	//set.SetVal(FDS_RELAY_PORT, nRelaySvrPort);
	//set.SetVal(FDS_VERSION, zVersion);

	int nLen = set.Complete(zSendBuff, false);

	LOGGING(INFO, TRUE, "[Return Login to Client][%c][%d](%s)", 
						pPacket->pCK->cSockTp,  
						pPacket->pCK->sock,
						zSendBuff
	);

	RequestSendIO(pPacket->pCK->sock, zSendBuff, nLen);


	/// /////////////////////////////////////////////////////////////////////////////
	/// Notificate to Manager
	if (enAppTp == APPTP_MANAGER)
		pApp->NoticeEA_ManagerLogOnOff(true);

	pApp->NoticeManager_EALogon(sAppId, pPacket->pCK->sock, enAppTp);
		
	return TRUE;
}



RET_SENTOSA CIocp::Logon_DBProc(auto_ptr< CMySqlHandler>& handler,
	char* pzUserId, char* pzPwd, char* pzAppId, EN_APP_TP enAppTp, char* pzBrokerName,
	char* pzAccNo, char* pzLiveDemo, char* pzClientIp, char* pzMac, char* pzMarketTime, BOOL bDupLogon)
{
	char zQ[1024];
	sprintf(zQ, "Call sp_logon("
		"'%s'"  //i_user_id	varchar(20)
		",'%s'" //i_user_pwd	varchar(20)
		",'%s'" //i_app_id	varchar(20)
		",'%s'" //i_app_tp	char(1)
		",'%s'" //i_broker_nm	varchar(50)
		",'%s'" //i_acc_no
		",'%s'" //i_livedemo	 	char(1)
		",'%s'" //i_logon_ip	 	varchar(15)
		",'%s'" //i_logon_mac 	varchar(20)
		",'%s'" //i_logontime_broker	varchar(21)
		",'%s'" //i_svr_tp			char(1)	-- // 'A':AUTHSVR, 'R':RELAYSVR, 'D':DATASVR)
		",'%s'" //i_dup_logon_yn
		");"
		, pzUserId
		, pzPwd
		, pzAppId
		, AppTp_S(enAppTp)
		, pzBrokerName
		, pzAccNo
		, pzLiveDemo
		, pzClientIp
		, pzMac
		, pzMarketTime
		, DEF_SVRTP_RELAY
		, (bDupLogon)? "Y":"N"
	);
	auto_ptr<Rs> rs(handler->Execute(zQ));
	int ret_code; string sMsg;
	if (!rs->Is_Successful_ExcutingSP(ret_code, sMsg))
	{
		LOGGING(ERR, TRUE, "[Logon_DBProc]%s", sMsg.c_str());
		return ERR_DB_EXEC;
	}
	if (ret_code != ERR_SUCCESS)
	{
		strcpy(m_zMsg, sMsg.c_str());
	}
	return (RET_SENTOSA)ret_code;
}



BOOL CIocp::Logoff_DBProc(char* pzUserId, char* pzAppId, EN_APP_TP enAppTp, BOOL bDupLogOn)
{
	char zQ[1024];
	auto_ptr< CMySqlHandler> handler(new CMySqlHandler);

	handler->Initialize(g_config.getDBIp(), g_config.getDBPort(), g_config.getDBUserId()
		, g_config.getDBUserPwd(), g_config.getDBName());
	if (!handler->OpenDB())
	{
		LOGGING(ERR, TRUE, "[Logoff_DBProc]DB Open Error(%s)", handler->GetMsg());
		return FALSE;
	}

	sprintf(zQ, "Call sp_logoff('%s', '%s','%s', '%s');", 
			pzUserId, pzAppId, AppTp_S(enAppTp), (bDupLogOn)?"Y":"N");
	auto_ptr<Rs> rs(handler->Execute(zQ));
	string sMsg; int ret_code;
	if (!rs->Is_Successful_ExcutingSP(ret_code, sMsg))
	{
		LOGGING(ERR, TRUE, "[Logoff_DBProc]%s", sMsg.c_str());
		return FALSE;
	}
	return TRUE;
}
BOOL CIocp::Find_User(string sUserId, _Out_ IT_MAP_USER& it)
{
	it = m_mapUser.find(sUserId);
	return (it != m_mapUser.end());
}


unsigned WINAPI CIocp::Thread_Listen(LPVOID lp)
{
	CIocp *pThis = (CIocp*)lp;
	LOGGING(INFO, FALSE, TEXT("Thread_Listen starts.....[%s][%d]"), g_config.getListenIP(), g_config.getListenPort());

	SOCKADDR_IN			sinClient;
	int	sinSize			= sizeof(sinClient);
	long nLoop			= 0;
	int nHearbeatCnt	= 0;

	while (pThis->m_bRun)
	{
		//DWORD dw = WSAWaitForMultipleEvents(1, &pThis->m_hListenEvent, TRUE, HEARTBEAT_TIMEOUT, FALSE);
		DWORD dw = WSAWaitForMultipleEvents(1, &pThis->m_hListenEvent, TRUE, 10, FALSE);
		if (dw != WSA_WAIT_EVENT_0) 
		{
			if (dw == WSA_WAIT_TIMEOUT)
			{
				pThis->Try_Delete_ClosedCK();
				continue;
			}
		}

		WSAResetEvent(pThis->m_hListenEvent);		
		
		SOCKET sockClient = accept(pThis->m_sockListen, (LPSOCKADDR)&sinClient, &sinSize);
		if (sockClient == INVALID_SOCKET) {
			int nErr = WSAGetLastError();
			LOGGING(ERR, TRUE, TEXT("accept error:%d"), nErr);
			{ // Socket operation on nonsocket.
				pThis->InitListen();
				Sleep(3000);
			}
			continue;
		}

		int nZero = 0;
		if (SOCKET_ERROR == setsockopt(sockClient, SOL_SOCKET, SO_SNDBUF, (const char*)&nZero, sizeof(int)))
		{
			shutdown(sockClient, SD_SEND);
			closesocket(sockClient);
			LOGGING(ERR, TRUE, TEXT("setsockopt error : %d"), WSAGetLastError);
			continue;;
		}

		char zIp[32];
		strcpy(zIp, inet_ntoa(sinClient.sin_addr));

		//	CK 와 IOCP 연결
		COMPLETION_KEY* pCK = new COMPLETION_KEY;
		pCK->sock		= sockClient;
		pCK->sClientIp = zIp;
		

		HANDLE h = CreateIoCompletionPort((HANDLE)pCK->sock,
			pThis->m_hCompletionPort,
			(DWORD)pCK,
			0);
		if (h == NULL)
		{
			delete pCK;
			closesocket(sockClient);
			continue;
		}

		EnterCriticalSection(&pThis->m_csCK);
		pThis->m_mapCK[pCK->sock] = pCK;
		LeaveCriticalSection(&pThis->m_csCK);

		// PacketParser
		pThis->m_parser.AddSocket(sockClient);

		LOGGING(INFO, TRUE, TEXT("Accept & Add IOCP.[socket:%d][CK:%x]"), sockClient, pCK);

		//	최초 RECV IO 요청
		RequestRecvIO(pCK->sock);

		//pThis->SendMessageToIocpThread(CTX_MT4PING);

	}//while

	return 0;
}

void CIocp::RemoveCK_FromMap(SOCKET sock)
{
	EnterCriticalSection(&m_csCK);
	m_mapCK.erase(sock);
	LeaveCriticalSection(&m_csCK);
}

void CIocp::Set_DupLogOn(SOCKET sock)
{
	EnterCriticalSection(&m_csCK);
	IT_MAP_CK it = m_mapCK.find(sock);
	if (it != m_mapCK.end()) {
		(*it).second->bDupLogon = TRUE;
	}
	LeaveCriticalSection(&m_csCK);
}

unsigned WINAPI CIocp::Thread_Config(LPVOID lp)
{
	CIocp* p = (CIocp*)lp;

	while (p->m_bRun)
	{
		EN_CNFG_RET ret = g_config.ReLoad_ConfigInfo();
		if (ret == CNFG_ERR)
		{
			LOGGING(ERR, TRUE, "Read cnfg error!!!");
		}
		else if (ret == CNFG_SUCC)
		{
			LOGGING(INFO, TRUE, "Cnfg Updated !");
		}

		Sleep(g_config.getTimeoutReadCnfg() * 1000);
	}
	return 0;
}


BOOL CIocp::OpenDB_ForLogging()
{
	if (m_pDbForLogging)
		delete m_pDbForLogging;

	m_pDbForLogging->Initialize(g_config.getDBIp(), g_config.getDBPort(), g_config.getDBUserId()
		, g_config.getDBUserPwd(), g_config.getDBName());
	if (!m_pDbForLogging->OpenDB())
	{
		LOGGING(ERR, TRUE, "[OpenDB_ForLogging]DB Open Error(%s)", m_pDbForLogging->GetMsg());
		return FALSE;
	}
	return TRUE;
}

unsigned WINAPI CIocp::Thread_DBProc(LPVOID lp)
{
	CIocp* p = (CIocp*)lp;

	
	while (p->m_bRun)
	{
		Sleep(10);
		if (p->m_pDbForLogging->IsConned())
		{
			if (!p->OpenDB_ForLogging())
			{
				Sleep(5000);
				continue;
			}
		}
		
		CProtoGet get;
		if (!g_listProtoGet.Get(get))
			continue;

		string sCode;
		string sCommand;
		string sUserId;
		string sAppId;
		string sOrdPosTp;
		string sValue;
		double slprc, tpprc, slpt, tppt;
		string sPacket;
		string sTargetBrokerNm;
		int nTicket;
		__ALPHA::EN_USER_ACTION		userAction;
		__ALPHA::EN_ORD_ACTION		ordAction;
		__ALPHA::EN_ACTION_SCOPE	scope;
		__ALPHA::EN_CLOSE_TP		closeTp;
		int nVal;
		
		get.GetVal(FDS_USER_ID, &sUserId);
		get.GetVal(FDS_CODE, &sCode);
		get.GetVal(FDS_ORD_POS_TP, &sOrdPosTp);
		get.GetVal(FDS_KEY, &sAppId);
		get.GetVal(FDN_ACTION_SCOPE, &nVal);	scope = (__ALPHA::EN_ACTION_SCOPE)nVal;		
		get.GetVal(FDN_CLOSE_TP, &nVal);		closeTp = (__ALPHA::EN_CLOSE_TP)nVal;
		get.GetVal(FDN_TICKET, &nTicket);
		get.GetVal(FDD_SLPRC, &slprc);
		get.GetVal(FDD_SL_PT, &slpt);
		get.GetVal(FDD_TPPRC, &tpprc);
		get.GetVal(FDD_TP_PT, &tppt);
		sPacket = get.GetOrgData();

		if (sCode == __ALPHA::CODE_ORDER_CLOSE)
		{
			userAction = __ALPHA::USERACTION_ORD;
			ordAction = __ALPHA::ORDACTION_CLOSE;
		}
		else if (sCode == __ALPHA::CODE_ORDER_CHANGE)
		{
			userAction = __ALPHA::USERACTION_ORD;
			ordAction = __ALPHA::ORDACTION_CHANGE;
		}
		else if (sCode == __ALPHA::CODE_LOGOFF)
		{
			userAction = __ALPHA::USERACTION_LOGOFF_FORCELY;
		}
		//else if (sCode == __ALPHA::CODE_COMMAND_BY_CODE)
		//{
		//	get.GetVal(FDS_COMMAND_CODE, &sCommand);
		//}
		else
		{
			LOGGING(ERR, TRUE, "NOT defined code for logging(%s)", sCode.c_str());
			continue;
		}

		if (sCode == __ALPHA::CODE_ORDER_CLOSE || sCode == __ALPHA::CODE_ORDER_CHANGE)
		{
			if (closeTp == __ALPHA::CLOSETP_ALL) {
			}
			else if (closeTp == __ALPHA::CLOSETP_SYMBOL) {
				get.GetVal(FDS_SYMBOL, &sValue);
			}
			else if (closeTp == __ALPHA::CLOSETP_MAGIC) {
				get.GetVal(FDN_MAGIC_NO, &sValue);
			}
			else if (closeTp == __ALPHA::CLOSETP_PROFIT) {
				sValue = "CLOSETP_PROFIT";
			}
			else if( closeTp == __ALPHA::CLOSETP_LOSS ) {
				sValue = "CLOSETP_LOSS";
			}
			else if (closeTp == __ALPHA::CLOSETP_TICKET) {
				char zTicket[128] = { 0 }; sprintf(zTicket, "%d", nTicket);
				sValue = zTicket;
			}
			else if( closeTp == __ALPHA::CLOSETP_BUY  ) {
				sValue = "CLOSETP_BUY";
			}
			else if( closeTp == __ALPHA::CLOSETP_SELL  ) {
				sValue = "CLOSETP_SELL";
			}
		}

		char zQ[1024] = { 0 };
		sprintf(zQ, "Call sp_user_log_save("
			"'%s'"	//i_user_id		varchar(20)
			"%d"	//i_user_action	int
			"'%s'"	//i_ord_pos_tp 	char(1)
			"%d"	//i_ord_action	int
			"%d"	//i_action_scope	int
			"'%s'"	//i_target_app_id varchar(50)
			"%d"	//i_close_tp		int
			"'%s'"	//i_key_value	varchar(20)
			"%f"	//i_sl_prc		varchar(20)
			"%f"	//i_tp_prc		varchar(20)
			"%d"	//i_sl_pt		int
			"%d"	//i_tp_pt		int
			"'%s'"	//i_packet		varchar(256)")
			,
			sUserId.c_str()
			, (int)userAction
			, sOrdPosTp.c_str()
			, (int)ordAction
			, (int)scope
			, sAppId.c_str()
			, (int)closeTp
			, sValue.c_str()
			, slprc
			, tpprc
			, (int)slpt
			, (int)tppt
			, sPacket.c_str()
		);

		auto_ptr<Rs> rs(p->m_pDbForLogging->Execute(zQ));
	}
	return 0;
}
