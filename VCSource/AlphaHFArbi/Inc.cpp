#pragma once

#include "Inc.h"

CLogMsg g_log, g_debug;// , g_log0, g_log1, g_log2, g_log3, g_log4, g_log5, g_log6, g_log7, g_log8, g_log9;

//void LOGGING(int iSymbol, LOGMSG_TP tp, BOOL bMain, BOOL bPrintConsole, const char* pMsg, ...)
void LOGGING(LOGMSG_TP tp, BOOL bMain, BOOL bPrintConsole, const char* pMsg, ...)
{
	const int size = 10000;
	char szBuf[size];

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(szBuf, pMsg, argptr);
	va_end(argptr);

	szBuf[size - 1] = 0;

	g_debug.Log(tp, szBuf, FALSE);

	//if (iSymbol > -1)
	//{
	//	CLogMsg* p;
	//	if (iSymbol == 0 || iSymbol==1 || iSymbol==2)	p = (CLogMsg*)&g_log0;
	//	if (iSymbol == 3 || iSymbol == 4 || iSymbol == 5)	p = (CLogMsg*)&g_log1;
	//	if (iSymbol == 6 || iSymbol == 7 || iSymbol == 8)	p = (CLogMsg*)&g_log2;
	//	if (iSymbol == 9 || iSymbol == 10 || iSymbol == 11)	p = (CLogMsg*)&g_log3;
	//	if (iSymbol == 12 || iSymbol == 13 || iSymbol == 14)	p = (CLogMsg*)&g_log4;
	//	if (iSymbol == 15 || iSymbol == 16 || iSymbol == 17)	p = (CLogMsg*)&g_log5;
	//	if (iSymbol == 18 || iSymbol == 19 || iSymbol == 20)	p = (CLogMsg*)&g_log6;
	//	if (iSymbol > 20 )	p = (CLogMsg*)&g_log7;


	//	p->Log(tp, szBuf, bPrintConsole);
	//}
	if (bMain)
		g_log.Log(tp, szBuf, bPrintConsole);
}