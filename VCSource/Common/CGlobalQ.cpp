// GlobalList.cpp: implementation of the CGlobalQ class.
//
//////////////////////////////////////////////////////////////////////

#include "CGlobalQ.h"
#include "Util.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalQ::CGlobalQ()
{
	InitializeCriticalSection(&m_cs);
}

CGlobalQ::~CGlobalQ()
{
	RemoveAll();
	DeleteCriticalSection(&m_cs);
}

VOID CGlobalQ::Add(const void* pData, int nSize)
{
	TUnit* pUnit = new TUnit;
	pUnit->size = nSize;
	pUnit->p = new char[nSize];
	memcpy(pUnit->p, pData, nSize);
	Lock();
	
	m_lstBuf.push_back(pUnit);

	UnLock();
}


BOOL CGlobalQ::Get(void* pOutBuf, int* pnSize)
{
	BOOL bExist = FALSE;
	Lock();
	__try
	{
		__try {
			bExist = GetInner(pOutBuf, pnSize);
		}
		__except(ReportException(GetExceptionCode(), "CGlobalQ::Get", m_zMsg))
		{
			*pnSize = -1;
		}
	}
	__finally
	{
		UnLock();
	}
	return bExist;
}

BOOL CGlobalQ::GetInner(void* pOutBuf, int* pnSize)
{
	if(m_lstBuf.empty()){
		return FALSE;
	}
	
	TUnit* pUnit = *m_lstBuf.begin();
	m_lstBuf.pop_front();
	
	*pnSize = pUnit->size;
	pOutBuf = new char[pUnit->size];
	memcpy(pOutBuf, pUnit->p, pUnit->size);
	
	delete[] pUnit->p;
	delete pUnit;
	
	return TRUE;
}

void CGlobalQ::RemoveAll()
{
	Lock();
	if (!m_lstBuf.empty())
	{

		list<TUnit*>::iterator it;
		for (auto& it : m_lstBuf)
		{
			TUnit* pCopied = it;
			delete[] pCopied->p;
			delete pCopied;
		}
		m_lstBuf.clear();
	}
	UnLock();
}

BOOL CGlobalQ::IsEmpty()
{
	Lock();
	BOOL bEmpty = m_lstBuf.empty();
	UnLock();
	return bEmpty;
}


int CGlobalQ::GetListSize()
{
	Lock();
	int size = m_lstBuf.size();
	UnLock();
	return size;
}