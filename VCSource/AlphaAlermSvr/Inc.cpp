
#include "Inc.h"

CLogMsg g_log;

void LOGGING(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...)
{
	char szBuf[4096];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	g_log.Log(tp, szBuf, bPrintConsole);
}