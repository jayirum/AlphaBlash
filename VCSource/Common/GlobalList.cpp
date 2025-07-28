// GlobalList.cpp: implementation of the CGlobalList class.
//
//////////////////////////////////////////////////////////////////////

#include "GlobalList.h"
#include "Util.h"


//#define LOCK(x)		EnterCriticalSection(x)
//#define UNLOCK(x)	LeaveCriticalSection(x)

list<ST_COPIED*>CGlobalList::m_lstBuf;
long	CGlobalList::m_lInstance=0;
CRITICAL_SECTION CGlobalList::m_cs;
char	CGlobalList::m_zMsg[1024];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalList::CGlobalList()
{
	if( ++m_lInstance == 1 )
		InitializeCriticalSection(&m_cs);
}

CGlobalList::~CGlobalList()
{
	if( --m_lInstance == 0 )
	{
		if( !m_lstBuf.empty() )
			RemoveAll();
		DeleteCriticalSection(&m_cs);
	}
}


VOID CGlobalList::Add(const void* pBuf, int nSize)
{
	ST_COPIED* pCopied = new ST_COPIED;
	pCopied->size = nSize;
	pCopied->p = new char[nSize];
	memcpy(pCopied->p, pBuf, nSize);

	Lock();
	
	m_lstBuf.push_back(pCopied);

	UnLock();
}


BOOL CGlobalList::Get(void* pOutBuf, int* pnSize)
{
	BOOL bExist = FALSE;
	Lock();
	__try
	{
		__try {
			bExist = GetInner(pOutBuf, pnSize);
		}
		__except(ReportException(GetExceptionCode(), "CGlobalList::Get", m_zMsg))
		{
			*pnSize = -1;
		}
	}
	__finally
	{
		if(bExist)	m_lstBuf.pop_front();
		UnLock();
	}
	return (*pnSize>0);
}

BOOL CGlobalList::GetInner(void* pOutBuf, int* pnSize)
{
	BOOL bExist = FALSE;
	if(m_lstBuf.empty()){
		return 0;
	}
	

	bExist = TRUE;

	ST_COPIED* pCopied = *m_lstBuf.begin();
	int nSize = pCopied->size;
	memcpy(pOutBuf, pCopied->p, nSize);
	*pnSize = nSize;
	delete[] pCopied->p;
	delete pCopied;
	
	return bExist;
}



//void CGlobalList::Pop()
//{
//	Lock();
//
//	__try
//	{
//		__try
//		{
//			if (m_lstBuf.empty() == false)
//			{
//				m_lstBuf.pop_front();
//			}
//		}
//		__except(ReportException(GetExceptionCode(), "CGlobalList::Pop", m_zMsg))
//		{ }
//	}
//	__finally
//	{
//		UnLock();
//	}
//}

void CGlobalList::RemoveAll()
{
	if(m_lstBuf.empty())
		return ;
	
	Lock();
	list<ST_COPIED*>::iterator it;
	for (it = m_lstBuf.begin(); it != m_lstBuf.end(); it++)
	{
		ST_COPIED* pCopied = *it;
		delete[] pCopied->p;
		delete pCopied;
	}
	m_lstBuf.clear();
	UnLock();

}

BOOL CGlobalList::IsEmpty()
{
	if(m_lstBuf.empty())
		return TRUE;
	return FALSE;
}


int CGlobalList::GetListSize()
{
	return m_lstBuf.size();
}