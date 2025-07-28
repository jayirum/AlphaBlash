#pragma once

#include "Inc.h"

CLogMsg		g_log;
CConfig		g_config;
CProtoGetList	g_listProtoGet;

//void LOGGING(int iSymbol, LOGMSG_TP tp, BOOL bMain, BOOL bPrintConsole, const char* pMsg, ...)
void LOGGING(LOGMSG_TP tp, BOOL bPrintConsole, const char* pMsg, ...)
{
	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;
	g_log.Log(tp, szBuf, bPrintConsole);
}


char* AppTp_S(EN_APP_TP appTp)
{
	if (appTp == APPTP_EA)
		return DEF_APPTP_EA;
	else if (appTp == APPTP_MANAGER)
		return DEF_APPTP_MANAGER;
	
	return NULL;
}
