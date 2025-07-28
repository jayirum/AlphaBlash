
#include "IocpThread.h"
#include <process.h>
#include "../Common/Util.h"
#include "../Common/LogMsg.h"
#include "../Common/IRExcept.h"
#include "../Common/IRUM_Common.h"
#include "../Common/CommonFunc.h"
#include <assert.h>

extern CLogMsg	g_log;
extern wchar_t	g_zConfig[_MAX_PATH];
extern BOOL		g_bDebugLog;
CIocp::CIocp()
{
	m_hCompletionPort	= NULL;
	m_hListenEvent		= NULL;
	m_sockListen		= INVALID_SOCKET;
	m_hListenThread		= NULL;
	m_dListenThread		= 0;
	m_nListenPort		= 0;
	m_hListenEvent		= NULL;
	m_bRun				= TRUE;
	m_pDBPool			= NULL;
	m_lIocpThreadIdx = 0;
}
CIocp::~CIocp()
{
	Finalize();
}

BOOL CIocp::ReadIPPOrt()
{
	wchar_t zTemp[1024] = { 0, };
	CUtil::GetConfig(g_zConfig, TEXT("NETWORK"), TEXT("LISTEN_IP"), m_wzListenIP);
	CUtil::GetConfig(g_zConfig, TEXT("NETWORK"), TEXT("LISTEN_PORT"), zTemp);
	m_nListenPort = _ttoi(zTemp);
	
	return TRUE;
}

BOOL CIocp::Initialize( )
{
	ReadIPPOrt();

	InitializeCriticalSection(&m_csCK);

	//DB OPEN
	if (!DBOpen())
		return FALSE;

	if (!InitListen()) {
		return FALSE;
	}
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, &ListenThread, this, 0, &m_dListenThread);

	// CPU�� ���� �˱� ���ؼ�....IOCP���� ����� �������� ���� cpu���� �Ǵ� cpu*2
	SYSTEM_INFO         systemInfo;
	GetSystemInfo(&systemInfo);
	m_dwThreadCount = systemInfo.dwNumberOfProcessors;

	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		g_log.logW(LOGTP_ERR, TEXT("IOCP Create Error:%d"), GetLastError());
		return FALSE;
	}

	// ������ recv�� send�� ����� �����带 �����Ѵ�.
	unsigned int dwID;
	for (unsigned int n = 0; n < m_dwThreadCount; n++)
	{
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &IocpWorkerThread, this, 0, &dwID);
		CloseHandle(h);
	}

	return TRUE;
}


BOOL CIocp::DBOpen()
{
	wchar_t ip[32], id[32], pwd[32], cnt[32], name[32];
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_IP"), ip);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_ID"), id);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_PWD"), pwd);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_NAME"), name);
	CUtil::GetConfig(g_zConfig, TEXT("DBINFO"), TEXT("DB_POOL_CNT"), cnt);


	if (!m_pDBPool)
	{
		m_pDBPool = new CDBPoolAdo(ip, id, pwd, name);
		if (!m_pDBPool->Init(_ttoi(cnt)))
		{
			g_log.logW(ERR, TEXT("DB OPEN FAILED.(%s)(%s)(%s)"), ip, id, pwd);
			return 0;
		}
	}
	return TRUE;
}

BOOL CIocp::InitListen()
{
	g_log.logW(INFO, TEXT("CIocp::InitListen() starts.........."));
	CloseListenSock();

	WORD	wVersionRequired;
	WSADATA	wsaData;

	//// WSAStartup
	wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		g_log.logW(LOGTP_ERR, TEXT("WSAStartup Error:%d"), GetLastError());
		return FALSE;
	}

	//DumpWsaData(&wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		g_log.logW(LOGTP_ERR, TEXT("RequiredVersion not Usable"));
		return FALSE;
	}


	// Create a listening socket 
	if ((m_sockListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		g_log.logW(ERR, TEXT("create socket error: %d"), WSAGetLastError());
		return FALSE;
	}

	ReadIPPOrt();

	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = _tinet_addr(m_wzListenIP);
	InternetAddr.sin_port = htons(m_nListenPort);

	BOOL opt = TRUE;
	int optlen = sizeof(opt);
	setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (const char far *)&opt, optlen);


	if (::bind(m_sockListen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		g_log.logW(ERR, TEXT("bind error (ip:%s) (port:%d) (err:%d)"), m_wzListenIP, m_nListenPort, WSAGetLastError());
		return FALSE;
	}
	// Prepare socket for listening 
	if (listen(m_sockListen, 5) == SOCKET_ERROR)
	{
		g_log.logW(ERR, TEXT("listen error: %d"), WSAGetLastError());
		return FALSE;
	}

	m_hListenEvent = WSACreateEvent();
	if (WSAEventSelect(m_sockListen, m_hListenEvent, FD_ACCEPT)) {

		g_log.logW(ERR, TEXT("WSAEventSelect for accept error: %d"), WSAGetLastError());
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

void CIocp::Finalize()
{
	m_bRun = FALSE;

	for (UINT i = 0; i < WORKTHREAD_CNT; i++)
	{
		PostQueuedCompletionStatus(
			m_hCompletionPort
			, 0
			, NULL
			, NULL
		);
	}

	lockCK();
	std::map<std::string, COMPLETION_KEY*>::iterator it;
	for (it = m_mapCK.begin(); it != m_mapCK.end(); ++it)
	{
		COMPLETION_KEY* pCK = (*it).second;
		shutdown(pCK->sock, SD_BOTH);

		// TIME-WAIT ������
		struct linger structLinger;
		structLinger.l_onoff	= TRUE;
		structLinger.l_linger	= 0;

		setsockopt(pCK->sock, SOL_SOCKET, SO_LINGER, (LPSTR)&structLinger, sizeof(structLinger));
		closesocket(pCK->sock);
		
		delete pCK;
	}
	m_mapCK.clear();
	unlockCK();

	CloseListenSock();

	SAFE_CLOSEHANDLE(m_hCompletionPort);

	DeleteCriticalSection(&m_csCK);
	//DeleteCriticalSection(&m_csEA);
	//SAFE_DELETE(m_pack);
	WSACleanup();

	delete m_pDBPool;
}

void CIocp::DeleteSocket(COMPLETION_KEY *pCompletionKey)
{
	if (pCompletionKey->refcnt > 0)
		return;

	lockCK();

	char zSock[32];
	CVT_SOCKET(pCompletionKey->sock, zSock);

	std::map<std::string, COMPLETION_KEY*>::iterator it = m_mapCK.find(std::string(zSock));
	if (it != m_mapCK.end())
	{
		shutdown(pCompletionKey->sock, SD_BOTH);

		// TIME-WAIT ������
		struct linger structLinger;
		structLinger.l_onoff = TRUE;
		structLinger.l_linger = 0;
		setsockopt(pCompletionKey->sock, SOL_SOCKET, SO_LINGER, (LPSTR)& structLinger, sizeof(structLinger));
		closesocket(pCompletionKey->sock);
		delete pCompletionKey;
		m_mapCK.erase(it);
	}
	unlockCK();
}


/*
1.	WSASend / WSARecv �� ȣ���� ���� ����� socket �� CK �� ����Ǿ� �����Ƿ�
pCompletionKey �� ���ؼ� CK �� �����Ͱ� ���´�.

2.	PostQueuedCompletionStatus �� ȣ���� ���� socket �� ������ �����Ƿ�
�̶��� WM_MSG �� �������� �Ѵ�.

3.	Ȯ��� OVERLAPPED �� context �ʵ尡 �����Ƿ� ���⿡ CTX_DIE, CTX_RQST_SEND, CTX_RQST_RECV �� ä���� ������.

*/
unsigned WINAPI CIocp::IocpWorkerThread(LPVOID lp)
{
	CIocp* pThis = (CIocp*)lp;

	COMPLETION_KEY	*pCompletionKey = NULL;
	IO_CONTEXT		*pIoContext = NULL;
	DWORD			dwBytesTransferred = 0;
	DWORD           dwIoSize = 0;
	DWORD           dwRecvNumBytes = 0;
	DWORD           dwSendNumBytes = 0;
	DWORD           dwFlags = 0;
	LPOVERLAPPED	pOverlap = NULL;
	BOOL bRet;
	char			zRecvBuff[BUF_LEN];
	char			zLogBuff[BUF_LEN];
	int nLoop = 0;
	int nRet = 0;
	
	long iocpIdx = InterlockedIncrement(&pThis->m_lIocpThreadIdx)-1;

	g_log.logW(LOGTP_SUCC, TEXT("[%d][%d]IOCPThread Start....."), iocpIdx, GetCurrentThreadId());

	while (pThis->m_bRun)
	{
		bRet = GetQueuedCompletionStatus(pThis->m_hCompletionPort,
			&dwIoSize,
			(LPDWORD)&pCompletionKey, 	//����δ� ���� CK �� ������ �ʴ´�. ������ new, delete �ϹǷ�. 
			(LPOVERLAPPED *)&pOverlap,
			INFINITE);

		// Finalize ���� PostQueuedCompletionStatus �� NULL �Է�
		if (pCompletionKey == NULL) // ����
		{
			break;
		}
		
		// Finalize ���� PostQueuedCompletionStatus �� NULL �Է�
		if (pOverlap == NULL)
		{
			//g_log.log(INFO, "[IOCP](pOverlap == NULL)");
			break;
		}


		pIoContext = (IO_CONTEXT*)pOverlap;

		if (FALSE == bRet)
		{
			if (pIoContext->context == CTX_RQST_RECV)
			{				
				//pThis->RecvLogOffAndClose(pCompletionKey);
				pThis->DeleteSocket(pCompletionKey);
				//g_log.log(LOGTP_ERR, "[IOCP](if (FALSE == bRet)) call DeleteSocket()");
			}
			Sleep(3000);
			continue;
		}
		
		if (dwIoSize == 0)
		{
			//g_log.log(INFO, "[IOCP]if (dwIoSize == 0)");
			if (pIoContext->context == CTX_RQST_RECV)
				pThis->DeleteSocket(pCompletionKey);
			continue;
		}
		
		if (pIoContext->context == CTX_DIE)
		{
			break;
		}

		// Master / Slave �� ���� ������ ����
		if (pIoContext->context == CTX_RQST_RECV)
		{
			sprintf(zRecvBuff, "%.*s", pIoContext->wsaBuf.len, pIoContext->buf);
			sprintf(zLogBuff, "[%d][%d][RECV](%.*s)", iocpIdx, GetCurrentThreadId(), dwIoSize, zRecvBuff);
			g_log.log(INFO, zLogBuff);
			nRet = pThis->m_buffering[iocpIdx].AddPacket(pIoContext->buf, dwIoSize);
			if (nRet < 0) {
				g_log.log(NOTIFY, "[%d][%d]AddPacket Error(%s)", iocpIdx, GetCurrentThreadId(), pThis->m_buffering[iocpIdx].GetErrMsg());
			}

			pThis->RequestRecvIO(pCompletionKey);

			BOOL bContinue ;
			nLoop = 0;
			do
			{
				ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
				nRet = 0;
				bContinue = pThis->m_buffering[iocpIdx].GetOnePacket(&nRet, zRecvBuff);
				if (bRet == FALSE)
				{
					g_log.log(NOTIFY, "[%d][%d][Buffering Err](%s)", iocpIdx, GetCurrentThreadId(), pThis->m_buffering[iocpIdx].GetErrMsg());
					break;
				}
				
				if (nRet > 0)
				{
					g_log.log(INFO, "[%d][%d]Buffering[%d](%s)\n", iocpIdx, GetCurrentThreadId(), ++nLoop, zRecvBuff);
					InterlockedIncrement(&pCompletionKey->refcnt);
					pThis->DispatchData(pCompletionKey, zRecvBuff, nRet);
					InterlockedDecrement(&pCompletionKey->refcnt);
				}

			} while (bContinue);

			//pThis->RequestRecvIO(pCompletionKey);

		}

		if (pIoContext->context == CTX_RQST_SEND)
		{
			//printf("RequestSendIO returned\n");
		}
		delete pIoContext;

	} // while

	//mt4helper.disconnect();

	return 0;
}


/*
CODE_USER_LOG
CODE_COPIER_ORDER
CODE_LOGON
CODE_LOGOFF
*/
void CIocp::DispatchData(COMPLETION_KEY* pCK, const char* pRecvData, const int nRecvLen)
{
	CProtoGet protoGet;
	string sCode;
	string sMasterSlaveTp;
	char zMCTp[32] = { 0, };
	wchar_t wzID[128] = { 0, };
	char zID[128] = { 0, };
	protoGet.Parsing(pRecvData, nRecvLen); 

	try
	{
		ASSERT_BOOL2(protoGet.GetCode(sCode), E_NOCODE, TEXT("Receive data but there is no Code"));

		ASSERT_BOOL2(protoGet.GetVal(FDS_MASTERCOPIER_TP, &sMasterSlaveTp), E_INVALIDE_MASTERCOPIER, TEXT("FDS_MASTERCOPIER_TP is not in the packet"));

		// ID ����
		if (pCK->sUserID.size() == 0)
		{
			ASSERT_BOOL2(protoGet.GetVal(FDS_USERID_MINE, wzID), E_NO_USERID, TEXT("FDS_USERID_MINE is not in the packet"));
			pCK->sUserID = U2A(wzID, zID);
		}

		if ( sCode.compare(__ALPHA::CODE_LOGON) == 0)
		{
			if (CProtoUtils::GetValue((char*)pRecvData, FDS_MASTERCOPIER_TP, zMCTp) == false) {
				assert(false);
			}
			if (__ALPHA::IsMaster(zMCTp))
				Logon_Master(pCK, pRecvData, nRecvLen);
			else
				Logon_Copier(pCK, pRecvData, nRecvLen);
		}
	}
	catch (CIRExcept& e)
	{
		g_log.log(ERR, e.GetMsg());	
		g_log.log(NOTIFY, "[DispatchData Exception](%s)(OrgPacket:%s)",e.GetMsg(), pRecvData);
		ReturnError(pCK, e.GetCode());
		return;
	}

}


void CIocp::ReturnError(COMPLETION_KEY* pCK, int nErrCode)
{
	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };

	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_RETURN_ERROR);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pCK->sUserID.c_str());
	set.SetVal(FDN_ERR_CODE, nErrCode);
	
	int nLen = set.Complete(zSendBuff);

	g_log.log(INFO, "[Return Error to Client](%s)", zSendBuff);
	RequestSendIO(pCK, zSendBuff, nLen);
}





unsigned WINAPI CIocp::ListenThread(LPVOID lp)
{
	CIocp *pThis = (CIocp*)lp;
	g_log.logW(INFO, TEXT("ListenThread starts.....[%s][%d]"), pThis->m_wzListenIP, pThis->m_nListenPort);

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
				continue;
			}
		}

		WSAResetEvent(pThis->m_hListenEvent);		
		
		SOCKET sockClient = accept(pThis->m_sockListen, (LPSOCKADDR)&sinClient, &sinSize);
		if (sockClient == INVALID_SOCKET) {
			int nErr = WSAGetLastError();
			g_log.logW(NOTIFY, TEXT("accept error:%d"), nErr);
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
			g_log.logW(NOTIFY, TEXT("setsockopt error : %d"), WSAGetLastError);
			continue;;
		}


		//	CK �� IOCP ����
		COMPLETION_KEY* pCK = new COMPLETION_KEY;
		pCK->sock		= sockClient;

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

		char zSocket[32]; CVT_SOCKET(sockClient, zSocket);
		pThis->lockCK();
		pThis->m_mapCK[string(zSocket)] = pCK;
		pThis->unlockCK();

		g_log.logW(INFO, TEXT("Accept & Add IOCP.[socket:%d][CK:%x]"), sockClient, pCK);

		//	���� RECV IO ��û
		pThis->RequestRecvIO(pCK);

		//pThis->SendMessageToIocpThread(CTX_MT4PING);

	}//while

	return 0;
}


VOID CIocp::RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen)
{

	BOOL  bRet = TRUE;
	DWORD dwOutBytes = 0;
	DWORD dwFlags = 0;
	IO_CONTEXT* pSend = NULL;

	try {
		pSend = new IO_CONTEXT;

		ZeroMemory(pSend, sizeof(IO_CONTEXT));
		CopyMemory(pSend->buf, pSendBuf, nSendLen);
		//	pSend->sock	= sock;
		pSend->wsaBuf.buf = pSend->buf;
		pSend->wsaBuf.len = nSendLen;
		pSend->context = CTX_RQST_SEND;

		int nRet = WSASend(pCK->sock
			, &pSend->wsaBuf	// wsaBuf �迭�� ������
			, 1					// wsaBuf ������ ����
			, &dwOutBytes		// ���۵� ����Ʈ ��
			, dwFlags
			, &pSend->overLapped	// overlapped ������
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				g_log.logW(LOGTP_ERR, TEXT("WSASend error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		g_log.logW(ERR, TEXT("WSASend try catch error [CIocp]"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;

	return;
}


//
///*
//	relay ip
//	relay port
//	nick name
//	master id
//	master acc
//*/
//BOOL CIocp::Logon_Process(COMPLETION_KEY* pCK, const char* pLogData, int nDataLen)
//{
//	char zMCTp[32] = { 0, };
//	if (CProtoUtils::GetValue((char*)pLogData, FDS_MASTERCOPIER_TP, zMCTp) == false) {
//		g_log.log(NOTIFY, "Logon_Process Get MasterCopier TP Error(%s)", pLogData);
//		return FALSE;
//	}
//
//	BOOL ret;
//	if (__ALPHA::IsMaster(zMCTp))
//		ret = Logon_Master(pCK, pLogData, nDataLen);
//	else 
//		ret = Logon_Copier(pCK, pLogData, nDataLen);
//
//	return ret;
//}

BOOL CIocp::Logon_Copier(COMPLETION_KEY* pCK, const char* pLogData, int nDataLen)
{
	//	client public ip ����
	SOCKADDR_IN peer_addr;
	int			peer_addr_len = sizeof(peer_addr);
	char zClientIp[128] = { 0, };
	wchar_t wzClientIp[128] = { 0, };
	if (getpeername(pCK->sock, (sockaddr*)&peer_addr, &peer_addr_len) == 0)
	{
		strcpy(zClientIp, inet_ntoa(peer_addr.sin_addr));
		A2U(zClientIp, wzClientIp);
	}

	BOOL bRet;

	CDBHandlerAdo db(m_pDBPool->Get());
	wchar_t wzQ[1024];
	CProtoGet protoGet;
	protoGet.Parsing(pLogData, nDataLen, FALSE);


	wchar_t wzUserID[32] = { 0, };
	wchar_t wzMCTp[32] = { 0, };
	wchar_t wzAccNo[32] = { 0, };
	wchar_t wzPassword[32] = { 0, };
	wchar_t wzBroker[128] = { 0, };
	wchar_t wzLiveDemo[32] = { 0, };
	wchar_t wzWebUrl[128] = { 0, };
	protoGet.GetValS(FDS_USERID_MINE, wzUserID);
	protoGet.GetValS(FDS_ACCNO_MINE, wzAccNo);
	protoGet.GetValS(FDS_MASTERCOPIER_TP, wzMCTp);
	protoGet.GetValS(FDS_USER_PASSWORD, wzPassword);
	protoGet.GetValS(FDS_BROKER, wzBroker);
	protoGet.GetValS(FDS_LIVEDEMO, wzLiveDemo);

	_stprintf(wzQ, TEXT("EXEC EA_LOGIN_COPIER ")
		TEXT("'%s'")		//@I_USER_ID
		TEXT(", '%s'")	//@I_PWD
		TEXT(", '%s'")	//@I_MT4_ACC
		TEXT(", '%s'")	// BROKER
		TEXT(",'%s'")		// LD_TP
		TEXT(",'%s'")		// IP
		, wzUserID
		, wzPassword
		, wzAccNo
		, wzBroker
		, wzLiveDemo
		, wzClientIp
	);

	//
	// RETURN 
	//

	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };
	int nRetCode;
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, wzUserID);
	set.SetVal(FDS_ACCNO_MINE, wzAccNo);

	if (FALSE == db->ExecQuery(wzQ))
	{
		g_log.logW(NOTIFY, TEXT("EA_LOGIN_COPIER Error(%s)(%s)"), db->GetError(), wzQ);
		set.SetVal(FDN_RSLT_CODE, E_SYS_DB_EXCEPTION);
		bRet = FALSE;
	}
	else
	{
		if(g_bDebugLog) g_log.logW(INFO, TEXT("[LoginCopier](%s)"), wzQ);

		bRet = TRUE;
		int nLoop = 0;
		wchar_t val[512];
		if (db->IsNextRow())
		{
			nRetCode = db->GetLong(TEXT("RET_CODE"));
			set.SetVal(FDN_RSLT_CODE, nRetCode);
			if (nRetCode != ERR_OK)
			{
				g_log.logW(ERR, TEXT("[%s]DB Login Error(%d)"), wzUserID, nRetCode);
			}
			else
			{
				db->GetStrWithLen(TEXT("RELAY_SVR_IP"), 128, val);
				set.SetVal(FDS_RELAY_IP, val);
				db->GetStrWithLen(TEXT("RELAY_SVR_PORT"), 32, val);		set.SetVal(FDS_RELAY_PORT, val);
				db->GetStrWithLen(TEXT("TR_SVR_PORT"), 32, val);		set.SetVal(FDS_TR_PORT, val);
				db->GetStrWithLen(TEXT("MASTER_ID"), 128, val);			
				set.SetVal(FDS_USERID_MASTER, val);
				db->GetStrWithLen(TEXT("MASTER_ACC"), 128, val);			
				set.SetVal(FDS_ACCNO_MASTER, val);
				db->GetStrWithLen(TEXT("USER_NICK_NM"), 128, val);		set.SetVal(FDS_USER_NICK_NM, val);
				db->GetStrWithLen(TEXT("WEBSITE_URL"), 128, wzWebUrl);	set.SetVal(FDS_WEBSITE_URL, wzWebUrl);
			}
		}
		int nLen = set.Complete(zSendBuff);
		g_log.log(INFO, "[LoginCopier return](%s)", zSendBuff);
		RequestSendIO(pCK, zSendBuff, nLen);
	}
	db->Close();

	return bRet;
}

BOOL CIocp::Logon_Master(COMPLETION_KEY* pCK, const char* pLogData, int nDataLen)
{
	//	client public ip ����
	SOCKADDR_IN peer_addr;
	int			peer_addr_len = sizeof(peer_addr);
	char zClientIp[128] = { 0, };
	wchar_t wzClientIp[128] = { 0, };
	if (getpeername(pCK->sock, (sockaddr*)&peer_addr, &peer_addr_len) == 0)
	{
		strcpy(zClientIp, inet_ntoa(peer_addr.sin_addr));
		A2U(zClientIp, wzClientIp);
	}

	BOOL bRet;

	CDBHandlerAdo db(m_pDBPool->Get());
	wchar_t wzQ[1024];
	CProtoGet protoGet;
	protoGet.Parsing(pLogData, nDataLen, FALSE);


	wchar_t wzUserID[32] = { 0, };
	wchar_t wzMCTp[32] = { 0, };
	wchar_t wzAccNo[32] = { 0, };
	wchar_t wzPassword[32] = { 0, };
	wchar_t wzBroker[128] = { 0, };
	wchar_t wzLiveDemo[32] = { 0, };
	wchar_t wzWebUrl[128] = { 0, };
	protoGet.GetValS(FDS_USERID_MINE, wzUserID);
	protoGet.GetValS(FDS_ACCNO_MINE, wzAccNo);
	protoGet.GetValS(FDS_MASTERCOPIER_TP, wzMCTp);
	protoGet.GetValS(FDS_USER_PASSWORD, wzPassword);
	protoGet.GetValS(FDS_BROKER, wzBroker);
	protoGet.GetValS(FDS_LIVEDEMO, wzLiveDemo);
	
	_stprintf(wzQ, "EXEC EA_LOGIN_MASTER "
		TEXT("'%s'"	)	//@I_USER_ID
		TEXT(", '%s'")	//@I_PWD
		TEXT(", '%s'")	//@I_MT4_ACC
		TEXT(", '%s'")	// BROKER
		TEXT(",'%s'")		// LD_TP
		TEXT(",'%s'")		// IP
		, wzUserID
		, wzPassword
		, wzAccNo
		, wzBroker
		, wzLiveDemo
		, wzClientIp
	);
	if (g_bDebugLog) g_log.logW(INFO, TEXT("[Master Login SP](%s)"), wzQ);

	//
	// RETURN 
	//

	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[32] = { 0, };
	int nRetCode;
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, wzUserID);
	set.SetVal(FDS_ACCNO_MINE, wzAccNo);

	if (FALSE == db->ExecQuery(wzQ))
	{
		g_log.logW(NOTIFY, TEXT("Logon_Master Error(%s)(%s)"), db->GetError(), wzQ);
		set.SetVal(FDN_RSLT_CODE, E_SYS_DB_EXCEPTION);
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
		int nLoop = 0;
		wchar_t relayIP[32], relayPort[32], trPort[32], nickName[32];
		if (db->IsNextRow())
		{
			nRetCode = db->GetLong(TEXT("RET_CODE"));
			set.SetVal(FDN_RSLT_CODE, nRetCode);
			if (nRetCode != ERR_OK)
			{
				g_log.logW(ERR, TEXT("[%s]DB Login Error(%d)"), wzUserID, nRetCode);
			}
			db->GetStrWithLen(TEXT("RELAY_SVR_IP"), 32, relayIP);		set.SetVal(FDS_RELAY_IP, relayIP);
			db->GetStrWithLen(TEXT("RELAY_SVR_PORT"), 32, relayPort);	set.SetVal(FDS_RELAY_PORT, relayPort);
			db->GetStrWithLen(TEXT("TR_SVR_PORT"), 32, trPort);			set.SetVal(FDS_TR_PORT, trPort);
			db->GetStrWithLen(TEXT("USER_NICK_NM"), 32, nickName);		set.SetVal(FDS_USER_NICK_NM, nickName);
			db->GetStrWithLen(TEXT("WEBSITE_URL"), 128, wzWebUrl);		set.SetVal(FDS_WEBSITE_URL, wzWebUrl);
			//db->GetStrWithLen("MASTER_ID", 32, val);	set.SetVal(FDS_USERID_MASTER, val);
			//db->GetStrWithLen("MASTER_ACC", 32, val);	set.SetVal(FDS_ACCNO_MASTER, val);
			
		}
	}
	db->Close();

	int nLen = set.Complete(zSendBuff);
	RequestSendIO(pCK, zSendBuff, nLen);

	return bRet;
}


void  CIocp::RequestRecvIO(COMPLETION_KEY* pCK)
{
	IO_CONTEXT* pRecv = NULL;
	DWORD dwNumberOfBytesRecvd = 0;
	DWORD dwFlags = 0;

	BOOL bRet = TRUE;
	try {
		pRecv = new IO_CONTEXT;
		ZeroMemory(pRecv, CONTEXT_SIZE);
		//ZeroMemory( &(pRecv->overLapped), sizeof(WSAOVERLAPPED));
		pRecv->wsaBuf.buf = pRecv->buf;
		pRecv->wsaBuf.len = MAX_BUF;
		pRecv->context = CTX_RQST_RECV;


		int nRet = WSARecv(pCK->sock
			, &(pRecv->wsaBuf)
			, 1, &dwNumberOfBytesRecvd, &dwFlags
			, &(pRecv->overLapped)
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				g_log.logW(LOGTP_ERR, TEXT("WSARecv error : %d"), WSAGetLastError());
				bRet = FALSE;
			}
		}
	}
	catch (...) {
		g_log.logW(LOGTP_ERR, TEXT("WSASend TRY CATCH"));
		bRet = FALSE;
	}

	if (!bRet)
		delete pRecv;

	printf("RequestRecvIO ok\n");
	return;
}