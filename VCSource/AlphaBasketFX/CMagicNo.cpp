#include "CMagicNo.h"

#pragma warning( disable : 26495)
#pragma warning( disable : 4996)
CMagicNo::CMagicNo()
{
	m_lSeed = 0;
	InitializeCriticalSection(&m_cs);
}

CMagicNo::~CMagicNo()
{
	DeleteCriticalSection(&m_cs);
}

long CMagicNo::getMagicNo()
{
	SYSTEMTIME st;
	char zNow[9];
	GetLocalTime(&st);
	sprintf(zNow, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);

	if (strcmp(m_zToday, zNow) < 0)
	{
		EnterCriticalSection(&m_cs);
		m_lSeed = 1;
		strcpy(m_zToday, zNow);
		LeaveCriticalSection(&m_cs);
	}
	else
	{
		EnterCriticalSection(&m_cs);
		m_lSeed++;
		strcpy(m_zToday, zNow);
		LeaveCriticalSection(&m_cs);
	}
	long No = atol(m_zToday) + m_lSeed;
	return No;
}