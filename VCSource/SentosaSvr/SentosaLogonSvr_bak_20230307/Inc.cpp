#pragma once

#include "Inc.h"

CLogMsg g_log;
CConfig g_config;

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



