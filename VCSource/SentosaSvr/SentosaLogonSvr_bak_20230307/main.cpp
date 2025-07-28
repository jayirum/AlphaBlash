#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "main.h"
#include "IocpThread.h"
#include <Windows.h>
#include <stdio.h>
//#include "../CommonAnsi/IRUM_Common.h"
#include "../../Common/LogMsg.h"
#include "../../Common/util.h"
#include "../../Common/prop.h"
#include <tchar.h>
#include "Inc.h"
#include "../../Common/LogCsv.h"

// service main function
void __stdcall ServiceStart(DWORD argc, LPTSTR* argv);

// service control
void __stdcall SCMHandler(DWORD opcode);

// service status
void __stdcall SetStatus(DWORD dwState,
	DWORD dwAccept = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE);
void install();
void uninstall();

SC_HANDLE hScm;
SC_HANDLE hSrv;
SERVICE_STATUS ss;

PSECURITY_DESCRIPTOR pSD;			// Pointer to SD.
SECURITY_ATTRIBUTES sa;
//DWORD	m_dwThrID;					

SERVICE_STATUS_HANDLE g_hXSS;		// service env global handle
DWORD	g_XSS;

BOOL	g_bDebug;
HANDLE	g_hDieEvent;				// event for terminating process
volatile 
BOOL	g_bContinue = TRUE;	// flag whether continue process or not
//TCHAR	g_zConfig[_MAX_PATH];
TCHAR	g_zLogDir[_MAX_PATH];

CRITICAL_SECTION	g_Console;

extern CLogMsg	g_log;
extern CConfig	g_config;


int  _Start()
{
	TCHAR	msg[512] = { 0, };
	char zConfigFileName[MAX_PATH] = { 0 };

	//	GET LOG DIR
	if (g_bDebug)
		CUtil::GetMyModuleAndDir(g_zLogDir, msg, zConfigFileName);
	else
		CUtil::GetCnfgFileNmOfSvc(SERVICENAME, g_zLogDir, zConfigFileName);

	g_config.Initialize(zConfigFileName);

	g_log.OpenLog(g_zLogDir, EXENAME);

	
	char zDebugNm[128]; sprintf(zDebugNm, "%s_Debug", EXENAME);
	
	LOGGING(INFO, FALSE, "-----------------------------------------------------");
	LOGGING(INFO, FALSE, "-----------------------------------------------------");
	LOGGING(INFO, FALSE, "-----------------------------------------------------");
	LOGGING(INFO, TRUE, "(DIR:%s)(cnfg:%s)", g_zLogDir, g_config.getFileName());
	LOGGING(INFO, FALSE, "Version[%s]", __DATE__);

	//---------------------------------------------
	//---------------------------------------------
	g_hDieEvent = CreateEvent(&sa, TRUE, FALSE, NULL);

	if (g_bDebug) {
		printf( "**************************\n");
		printf( "** Logon server starts.... **\n");
		printf( "**************************\n");
	}
	else {
		SetStatus(SERVICE_RUNNING);
	}

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


	CIocp workers;
	if (workers.Initialize() == FALSE)
	{
		LOGGING(ERR,TRUE, TEXT("IOCP initialize failed"));
	}
	else
	{
		DWORD ret = WaitForSingleObject(g_hDieEvent, INFINITE);
	}

	DeleteCriticalSection(&g_Console);
	printf("Stopped.\n");
	return 0;
}


void	ReturnError(SOCKET sock, const char* pCode, int nErrCode, char* pzMsg)
{
	char zSendBuff[__ALPHA::LEN_BUF] = { 0, };
	char zTime[32] = { 0, };

	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, pCode);
	set.SetVal(FDS_SUCC_YN, "N");
	set.SetVal(FDN_ERR_CODE, nErrCode);
	set.SetVal(FDS_MSG, pzMsg);

	int nLen = set.Complete(zSendBuff);

	LOGGING(INFO, TRUE, "[Return Error to Client](%s)", zSendBuff);
	RequestSendIO(sock, zSendBuff, nLen);
}


void RequestRecvIO(COMPLETION_KEY* pCK)
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
		pRecv->wsaBuf.len = __ALPHA::LEN_BUF;
		pRecv->context = CTX_RQST_RECV;


		int nRet = WSARecv(pCK->sock
			, &(pRecv->wsaBuf)
			, 1, &dwNumberOfBytesRecvd, &dwFlags
			, &(pRecv->overLapped)
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				LOGGING(LOGTP_ERR, TRUE, TEXT("WSARecv error : %d"), WSAGetLastError());
				bRet = FALSE;
			}
		}
	}
	catch (...) {
		LOGGING(LOGTP_ERR, TRUE, TEXT("WSASend TRY CATCH"));
		bRet = FALSE;
	}

	if (!bRet)
		delete pRecv;

	//printf("RequestRecvIO ok\n");
	return;
}

VOID RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen)
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

		int nRet = WSASend(sock
			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
			, 1					// wsaBuf 포인터 갯수
			, &dwOutBytes		// 전송된 바이트 수
			, dwFlags
			, &pSend->overLapped	// overlapped 포인터
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				LOGGING(LOGTP_ERR, TRUE, TEXT("WSASend error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		LOGGING(ERR, TRUE, TEXT("WSASend try catch error [CIocp]"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;
	//else
	//	LOGGING(INFO, FALSE, "[SEND](sock:%d)(%s)", sock, pSendBuf);
	return;
}




BOOL WINAPI ControlHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate  
	case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode  
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		printf("Stopping %s...\n", DISPNAME);
		SetEvent(g_hDieEvent);
		g_bContinue = FALSE;
		return TRUE;
		break;

	}
	return FALSE;
}


int main(int argc, LPSTR* argv)
{
	g_bDebug = FALSE;

	if ((argc > 1) &&
		((*argv[1] == '-') || (*argv[1] == '/')))
	{
		if (_stricmp("install", argv[1] + 1) == 0)
		{
			install();
		}
		else if (_stricmp("remove", argv[1] + 1) == 0 || _stricmp("delete", argv[1] + 1) == 0)
		{
			uninstall();
		}
		else if (_stricmp("debug", argv[1] + 1) == 0)
		{
			g_bDebug = TRUE;
			SetConsoleCtrlHandler(ControlHandler, TRUE);
			InitializeCriticalSection(&g_Console);

			_Start();

			DeleteCriticalSection(&g_Console);
			printf("Stopped.\n");
			return 0;
		}
		else
		{
			return 0;
		}
	}
	SERVICE_TABLE_ENTRY stbl[] =
	{
		{SERVICENAME, (LPSERVICE_MAIN_FUNCTION)ServiceStart },
		{NULL, NULL}
	};

	if (!StartServiceCtrlDispatcher(stbl))
	{
		return  -1;
	}

	return 0;
}

void  __stdcall SetStatus(DWORD dwState, DWORD dwAccept)
{
	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState = dwState;
	ss.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	ss.dwWin32ExitCode = 0;
	ss.dwServiceSpecificExitCode = 0;
	ss.dwCheckPoint = 0;
	ss.dwWaitHint = 0;
	g_XSS = dwState;			//현재 상태 보관

	SetServiceStatus(g_hXSS, &ss);
	return;
}

void __stdcall SCMHandler(DWORD opcode)
{
	if (opcode == g_XSS)
		return;

	switch (opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		SetStatus(SERVICE_PAUSE_PENDING, 0);
		SetStatus(SERVICE_PAUSED);
		break;

	case SERVICE_CONTROL_CONTINUE:
		SetStatus(SERVICE_CONTINUE_PENDING, 0);
		break;

	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		SetStatus(SERVICE_STOP_PENDING, 0);
		SetEvent(g_hDieEvent);
		g_bContinue = FALSE;
		break;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		SetStatus(g_XSS);
		break;
	}
}

void __stdcall ServiceStart(DWORD argc, LPTSTR* argv)
{
	g_hXSS = RegisterServiceCtrlHandler(SERVICENAME,
		(LPHANDLER_FUNCTION)SCMHandler);
	if (g_hXSS == 0)
	{
		//log.LogEventErr(-1,"서비스 컨트롤 핸들러를 등록할 수 없습니다.");
		return;
	}

	//서비스가 시작 중임을 알린다
	SetStatus(SERVICE_START_PENDING);

	//// Allocate memory for the security descriptor.	
	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(
		LPTR		//Specifies how to allocate memory
					// Allocates fixed memory && Initializes memory contents to zero.
		, SECURITY_DESCRIPTOR_MIN_LENGTH	//number of bytes to allocate
	);


	//// Initialize the new security descriptor.
	InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);


	//// Add a NULL descriptor ACL to the security descriptor.
	SetSecurityDescriptorDacl(pSD, TRUE, (PACL)NULL, FALSE);

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = TRUE;


	g_bDebug = FALSE;
	_Start();												//서비스 실행

	//log.LogEventInf(-1,"서비스가 정상적으로 종료했습니다.");
	SetStatus(SERVICE_STOPPED, 0);					///서비스 멈춤
	return;
}

void install()
{
	TCHAR SrvPath[MAX_PATH];
	SERVICE_DESCRIPTION lpDes;
	char Desc[64];
	strcpy(Desc, TEXT(DESC));

	hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hScm == NULL)
	{
		printf("Failed to connect Service Manager:%d\n", GetLastError());
		return;
	}
	GetCurrentDirectory(MAX_PATH, SrvPath);
	strcat(SrvPath, "\\");
	strcat(SrvPath, EXENAME);
	
	printf("Install Service(%s)\n", SrvPath);

	if (_access(SrvPath, 0) != 0)
	{
		CloseServiceHandle(hScm);
		//log.LogEventErr(-1, "서비스 프로그램이 같은 디렉토리에 없습니다");
		printf("There is no service process in same directory");
		return;
	}

	////	종속 서비스
//	char szDependency[64];
// 	memset(szDependency, 0x00, sizeof(szDependency));	
// 	CProp prop;
// 	prop.SetBaseKey(HKEY_LOCAL_MACHINE, REG_FRONTSERVER);
// 	strcpy(szDependency, prop.GetValue("Dependency"));
// 	prop.Close();

	hSrv = CreateService(hScm, SERVICENAME, DISPNAME,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		SrvPath,
		NULL,
		NULL,
		NULL, //szDependency,
		NULL,
		NULL);

	if (hSrv == NULL)
	{
		printf("Failed to install the service : %d\n", GetLastError());
	}
	else
	{
		lpDes.lpDescription = Desc;
		ChangeServiceConfig2(hSrv, SERVICE_CONFIG_DESCRIPTION, &lpDes);
		//log.LogEventInf(-1, "서비스를 성공적으로 설치했습니다.");
		printf("Succeeded in installing the service.\n");
		CloseServiceHandle(hSrv);
	}
	CloseServiceHandle(hScm);
}

void uninstall()
{
	hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (hScm == NULL)
	{
		//log.LogEventErr(-1, "서비스 메니져와 연결할 수 없습니다");
		printf("Can't connect to SCM\n");
		return;
	}

	hSrv = OpenService(hScm, SERVICENAME, SERVICE_ALL_ACCESS);
	if (hSrv == NULL)
	{
		CloseServiceHandle(hScm);
		//log.LogEventErr(-1, "서비스가 설치되어 있지 않습니다");
		printf("Service is not installed: %d\n", GetLastError());
		return;
	}

	//서비스 중지
	QueryServiceStatus(hSrv, &ss);
	if (ss.dwCurrentState != SERVICE_STOPPED)
	{
		ControlService(hSrv, SERVICE_CONTROL_STOP, &ss);
		Sleep(2000);
	}

	//서비스 제거
	if (DeleteService(hSrv))
	{
		//log.LogEventInf(-1, "성공적으로 서비스를 제거했습니다");
		printf("succeeded in removing service\n");
	}
	else {
		printf("failed to remove service\n");
	}
	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);
}
