// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "Log.h"
#include <stdio.h>
#include <locale.h>
#include "Util.h"
#pragma warning(disable:4996)
#define BUFLEN 4096L
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog():m_fd(0)
{
	ZeroMemory(m_szFileName, sizeof(m_szFileName));
	ZeroMemory(m_szDate, sizeof(m_szDate));
	ZeroMemory(m_szPureFileName, sizeof(m_szPureFileName) );
	InitializeCriticalSection(&m_cs);
}

CLog::~CLog()
{
	Close();
	DeleteCriticalSection(&m_cs);
}

BOOL CLog::OpenLogW	(wchar_t* psPath, wchar_t* pFileName)
{
	_tcscpy( m_szPureFileName, pFileName );

	SYSTEMTIME st;
	GetLocalTime(&st);
	_stprintf(m_szDate, TEXT("%04d%02d%02d"), st.wYear, st.wMonth, st.wDay);
	_tcscpy(m_szPath, psPath);
	_stprintf(m_szFileName, TEXT("%s\\%s_%s.log"), m_szPath, pFileName, m_szDate);
	//LOCK();
	Close();

	BOOL bFirst = FALSE;
	if (m_fd == NULL)
		bFirst = TRUE;

#ifdef  UNICODE
	setlocale(LC_ALL, ".OCP"); // locale 설정	
	m_fd = _tfopen(m_szFileName, TEXT("ab"));
#elif
	m_fd = _tfopen(m_szFileName, TEXT("at"));
#endif
	if (m_fd == NULL) {
		//UNLOCK();
		return FALSE;
	}

#ifdef  UNICODE
	if (bFirst)
	{
		// 해당 파일이 유니코드 파일이란 것을 명시하기 위해
		// 파일의 가장 앞에 0xfeff 를 write한다.
		wchar_t mark = 0xFEFF;
		fwrite(&mark, sizeof(TCHAR), 1, m_fd);
	}
#endif
	return TRUE;
}

BOOL CLog::OpenLog(char* psPath, char* pFileName)
{
	A2U(psPath, m_szPath);
	A2U(pFileName, m_szPureFileName);
	return OpenLogW(m_szPath, m_szPureFileName);
}



BOOL CLog::ReOpen()
{
	TCHAR szToday[8 + 1];
	SYSTEMTIME st;
	GetLocalTime(&st);
	_stprintf(szToday, TEXT("%04d%02d%02d"), st.wYear, st.wMonth, st.wDay);
	if (_tcscmp(szToday, m_szDate) > 0)
	{
		Close();
		return OpenLogW(m_szPath, m_szPureFileName);

	}
	return	TRUE;
}



VOID CLog::logW(const wchar_t* pMsg,... )
{
	ReOpen();

	//LOCK();
	if( m_fd <= 0 ){
		//UNLOCK();
		return;
	}

	TCHAR szBuf[BUFLEN];
	SYSTEMTIME st;
	GetLocalTime(&st);
	_stprintf(szBuf, TEXT("[%02d:%02d:%02d.%03d]"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	int nPos = _tcslen(szBuf);
	
	va_list argptr;
	va_start(argptr, pMsg);
	_vstprintf(szBuf+nPos, pMsg, argptr);
	va_end(argptr);
	_tcscat(szBuf, TEXT("\n"));
	fwprintf(m_fd, szBuf);
	fflush(m_fd);

	//UNLOCK();
}

VOID	CLog::log(char* pMsg, ...)
{
	char szBuf[BUFLEN];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szBuf, "[%02d:%02d:%02d.%03d]", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	int nPos = strlen(szBuf);

	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf(szBuf+ nPos, pMsg, argptr);
	va_end(argptr);
	strcat(szBuf, "\n");

	TCHAR wzMsg[1024] = { 0, };
	A2U(szBuf, wzMsg);
	fwprintf(m_fd, wzMsg);
	fflush(m_fd);
}

VOID CLog::Close()
{
	if( m_fd > 0){
		fclose(m_fd);
		m_fd = 0;
	}
}

int CLog::GetFileName(_Out_ TCHAR* pwsFileName)
{
	_tcscpy(pwsFileName, m_szFileName);
	return _tcslen(pwsFileName);
}