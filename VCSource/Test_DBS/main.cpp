
#pragma warning(disable:4786)
#pragma warning(disable:4503)


//#include "AlgoFront.h"


#include "../Common/ADOFunc.h"
#include <stdio.h>
#include "../Common/IRUM_Common.h"
#include "../Common/LogMsg.h"
#include "../Common/util.h"
#include "../Common/AlphaProtocolUni.h"
#include "../Common/AlphaProtoSetEx.h"
#include "../Common/AlphaInc.h"

#include <tchar.h>
#include <locale.h>

HANDLE	g_hDieEvent;				// event for terminating process
CRITICAL_SECTION	g_Console;

/// global variables shared with all classes
BOOL		g_bContinue = TRUE;	// flag whether continue process or not
CLogMsg		g_log;
TCHAR		g_zConfig[_MAX_PATH];
BOOL		g_bDebugLog = FALSE;
//CMemPool	g_memPool(_IRUM::MEM_PRE_ALLOC, _IRUM::MEM_MAX_ALLOC, _IRUM::MEM_BLOCK_SIZE);
TCHAR		g_zMyName[128];

BOOL WINAPI ControlHandler(DWORD dwCtrlType);

int main(int argc, LPWSTR* argv)
{
	wchar_t ip[] = TEXT("110.4.89.206,33411");
	wchar_t id[] = TEXT("alphauser");
	wchar_t pwd[] = TEXT("AlphaPassw0rd123$%^");
	wchar_t name[] = TEXT("AlphaBlash");
	CDBPoolAdo* m_pDBPool = new CDBPoolAdo(ip, id, pwd, name);
	if (!m_pDBPool->Init(1))
		return -1;

	CDBHandlerAdo db(m_pDBPool->Get());
	wchar_t wzMasterID[32] = TEXT("JAYKIM_M");
	wchar_t wzQ[1024];


	_swprintf(wzQ, "EXEC WEB_MasterCopier_Link_GET "
		TEXT("'%s'")	//@I_USER_ID
		, wzMasterID
	);

	char zSendBuff[1024] = { 0, };
	char zTime[32] = { 0, };

	int nRetCode;
	CProtoSetEx	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_COPIER_LIST);

	wchar_t wzVal[128] = { 0 };

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	if (TRUE == db->ExecQuery(wzQ))
	{
		int nLoop = 0;
		while (db->IsNextRow())
		{
			if (nLoop == 0)
				set.RecordStart();

			set.RowStart();

			db->GetStrWithLen(TEXT("COPIER_ID"), 32, wzVal);
			set.SetRecordVal(TEXT("COPIER_ID"), wzVal);

			long nSubsStatus = db->GetLong(TEXT("SUBS_STATUS"));
			set.SetRecordVal(TEXT("SUBS_STATUS"), nSubsStatus);

			db->GetStrWithLen(TEXT("RQST_DT"), 32, wzVal);
			set.SetRecordVal(TEXT("RQST_DT"), wzVal);

			db->GetStrWithLen(TEXT("DTDATE_DT"), 32, wzVal);
			set.SetRecordVal(TEXT("DTDATE_DT"), wzVal);

			db->GetStrWithLen(TEXT("COPIER_MT4_ACC"), 32, wzVal);
			set.SetRecordVal(TEXT("COPIER_MT4_ACC"), wzVal);

			set.RowEnd();

			nLoop++;
			db->Next();
		}
		set.SetRecordEnd();
	}
	db->Close();

	int nLen = set.Complete(zSendBuff);
	printf(zSendBuff);
	getchar();
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

