
#pragma warning(disable:4786)
#pragma warning(disable:4503)


//#include "AlgoFront.h"

#include "main.h"
#include "Dispatch.h"
#include <Windows.h>
#include <stdio.h>
#include "../Common/IRUM_Common.h"
#include "../Common/LogMsg.h"
#include "../Common/util.h"
#include <tchar.h>
#include <locale.h>

HANDLE	g_hDieEvent;				// event for terminating process
CRITICAL_SECTION	g_Console;

/// global variables shared with all classes
BOOL		g_bContinue = TRUE;	// flag whether continue process or not
CLogMsg		g_log;
wchar_t		g_wzConfig[_MAX_PATH];
BOOL		g_bDebugLog = FALSE;
//CMemPool	g_memPool(_IRUM::MEM_PRE_ALLOC, _IRUM::MEM_MAX_ALLOC, _IRUM::MEM_BLOCK_SIZE);
TCHAR		g_zMyName[128];

BOOL WINAPI ControlHandler(DWORD dwCtrlType);

LONG g_lSeq = 0;
LONG __IncPacketSeq()
{
	return InterlockedAdd(&g_lSeq, 1);
}


int main(int argc, LPWSTR* argv)
{
	SetConsoleCtrlHandler(ControlHandler, TRUE);
	InitializeCriticalSection(&g_Console);

	TCHAR	msg[512] = { 0, };
	TCHAR	szDir[_MAX_PATH];

	//	GET LOG DIR
	CUtil::GetMyModuleAndDir(szDir, msg, g_wzConfig);
	CUtil::GetConfig(g_wzConfig, TEXT("DIR"), TEXT("LOG"), szDir);

	TCHAR zSvrName[32], zNotiIP[32], zNotiPort[32];
	CUtil::GetConfig(g_wzConfig, TEXT("NOTIFICATION"), TEXT("MYSERVER_NAME"), zSvrName);
	CUtil::GetConfig(g_wzConfig, TEXT("NOTIFICATION"), TEXT("NOTIFICATION_SERVER_IP"), zNotiIP);
	CUtil::GetConfig(g_wzConfig, TEXT("NOTIFICATION"), TEXT("NOTIFICATION_SERVER_PORT"), zNotiPort);

	g_log.OpenLogEx(zSvrName, szDir, TEXT(EXENAME), zNotiIP, _ttoi(zNotiPort), TEXT(EXENAME));

	g_log.logW(LOGTP_SUCC, TEXT("-----------------------------------------------------"));
	g_log.logW(LOGTP_SUCC, TEXT("Version[%s] %s"), TEXT(__DATE__), TEXT(__APP_VERSION));
	g_log.logW(LOGTP_SUCC, TEXT("-----------------------------------------------------"));

	CUtil::GetConfig(g_wzConfig, TEXT("DEBUG"), TEXT("LOG_DEBUG"), msg);
	if (msg[0] == 'Y')	g_bDebugLog = TRUE;

	//---------------------------------------------
	//---------------------------------------------
	g_hDieEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


	CDispatch dispatch;
	if (dispatch.Initialize() == FALSE)
	{
		g_log.logW(NOTIFY, TEXT("IOCP initialize failed"));
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
		printf("Stopping ...\n");
		SetEvent(g_hDieEvent);
		g_bContinue = FALSE;
		return TRUE;
		break;

	}
	return FALSE;
}

