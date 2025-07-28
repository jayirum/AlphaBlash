#pragma warning(disable:4786)
#pragma warning(disable:4503)



#include "main.h"
#include "IocpThread.h"
#include <Windows.h>
#include <stdio.h>
//#include "../CommonAnsi/IRUM_Common.h"
#include "../CommonAnsi/LogMsg.h"
#include "../CommonAnsi/util.h"
#include "../CommonAnsi/prop.h"
#include <tchar.h>
//#include <locale.h>

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
TCHAR	g_zConfig[_MAX_PATH];

CRITICAL_SECTION	g_Console;

extern CLogMsg g_log, g_debug;


int  _Start()
{
	TCHAR	msg[512] = { 0, };
	TCHAR	szDir[_MAX_PATH];
	
	//	GET LOG DIR
	if (g_bDebug)
		CUtil::GetMyModuleAndDir(szDir, msg, g_zConfig);
	else
		CUtil::GetCnfgFileNmOfSvc(SERVICENAME, szDir, g_zConfig);

	g_log.OpenLogWithAlerm(EXENAME, szDir, EXENAME);

	char zDebugNm[128]; sprintf(zDebugNm, "%s_Debug", EXENAME);
	g_debug.OpenLog(szDir, zDebugNm);
	//g_log0.OpenLog(szDir, "AlphaBasketFX_0.exe");
	//g_log1.OpenLog(szDir, "AlphaBasketFX_1.exe");
	//g_log2.OpenLog(szDir, "AlphaBasketFX_2.exe");
	//g_log3.OpenLog(szDir, "AlphaBasketFX_3.exe");
	//g_log4.OpenLog(szDir, "AlphaBasketFX_4.exe");
	//g_log5.OpenLog(szDir, "AlphaBasketFX_5.exe");
	//g_log6.OpenLog(szDir, "AlphaBasketFX_6.exe");
	//g_log7.OpenLog(szDir, "AlphaBasketFX_7.exe");
	//g_log8.OpenLog(szDir, "AlphaBasketFX_8.exe");
	//g_log9.OpenLog(szDir, "AlphaBasketFX_9.exe");

	LOGGING(INFO, TRUE, FALSE, "-----------------------------------------------------");
	LOGGING(INFO, TRUE, FALSE, "-----------------------------------------------------");
	LOGGING(INFO, TRUE, FALSE, "-----------------------------------------------------");
	LOGGING(INFO, TRUE, FALSE, "(DIR:%s)(cnfg:%s)", szDir, g_zConfig);
	LOGGING(INFO, TRUE, FALSE, "Version[%s]", __DATE__);

	//---------------------------------------------
	//---------------------------------------------
	g_hDieEvent = CreateEvent(&sa, TRUE, FALSE, NULL);

	if (g_bDebug) {
		printf( "**************************\n");
		printf( "** ���񽺸� �����մϴ�. **\n");
		printf( "**************************\n");
	}
	else {
		SetStatus(SERVICE_RUNNING);
		//log.LogEventInf(-1," ���񽺸� �����մϴ�.");
	}

	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


	CIocp workers;
	if (workers.Initialize() == FALSE)
	{
		LOGGING(ERR,TRUE, TRUE, TEXT("IOCP initialize failed"));
	}
	else
	{
		DWORD ret = WaitForSingleObject(g_hDieEvent, INFINITE);
	}

	DeleteCriticalSection(&g_Console);
	printf("Stopped.\n");
	return 0;
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
	g_XSS = dwState;			//���� ���� ����

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
		//log.LogEventErr(-1,"���� ��Ʈ�� �ڵ鷯�� ����� �� �����ϴ�.");
		return;
	}

	//���񽺰� ���� ������ �˸���
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
	_Start();												//���� ����

	//log.LogEventInf(-1,"���񽺰� ���������� �����߽��ϴ�.");
	SetStatus(SERVICE_STOPPED, 0);					///���� ����
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
		//log.LogEventErr(-1, "���� ���α׷��� ���� ���丮�� �����ϴ�");
		printf("There is no service process in same directory");
		return;
	}

	////	���� ����
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
		//log.LogEventInf(-1, "���񽺸� ���������� ��ġ�߽��ϴ�.");
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
		//log.LogEventErr(-1, "���� �޴����� ������ �� �����ϴ�");
		printf("Can't connect to SCM\n");
		return;
	}

	hSrv = OpenService(hScm, SERVICENAME, SERVICE_ALL_ACCESS);
	if (hSrv == NULL)
	{
		CloseServiceHandle(hScm);
		//log.LogEventErr(-1, "���񽺰� ��ġ�Ǿ� ���� �ʽ��ϴ�");
		printf("Service is not installed: %d\n", GetLastError());
		return;
	}

	//���� ����
	QueryServiceStatus(hSrv, &ss);
	if (ss.dwCurrentState != SERVICE_STOPPED)
	{
		ControlService(hSrv, SERVICE_CONTROL_STOP, &ss);
		Sleep(2000);
	}

	//���� ����
	if (DeleteService(hSrv))
	{
		//log.LogEventInf(-1, "���������� ���񽺸� �����߽��ϴ�");
		printf("succeeded in removing service\n");
	}
	else {
		printf("failed to remove service\n");
	}
	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);
}
