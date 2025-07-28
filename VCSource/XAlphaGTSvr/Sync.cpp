#include "Sync.h"


CSync::CSync()
{
	m_nRefCnt = 0;
	InitializeCriticalSection(&m_cs);
}

CSync::~CSync()
{
	DeleteCriticalSection(&m_cs);
}

void CSync::AddRef()
{
	EnterCriticalSection(&m_cs);
	m_nRefCnt++;
	LeaveCriticalSection(&m_cs);
}

void CSync::ReleaseRef()
{
	EnterCriticalSection(&m_cs);
	if(--m_nRefCnt<0) m_nRefCnt=0;
	LeaveCriticalSection(&m_cs);
}

BOOL CSync::IsCleared()
{
	BOOL bCleared = FALSE;
	EnterCriticalSection(&m_cs);
	bCleared = (m_nRefCnt == 0);
	LeaveCriticalSection(&m_cs);
	return bCleared;
}
