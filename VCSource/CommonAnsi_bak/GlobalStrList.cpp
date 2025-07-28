// GlobalList.cpp: implementation of the CGlobalStrList class.
//
//////////////////////////////////////////////////////////////////////

#include "GlobalStrList.h"
#include "Util.h"


//#define LOCK(x)		EnterCriticalSection(x)
//#define UNLOCK(x)	LeaveCriticalSection(x)

list<string>CGlobalStrList::m_lstStrBuf;
long	CGlobalStrList::m_lInstanceStrList =0;
CRITICAL_SECTION CGlobalStrList::m_csStrList;
char	CGlobalStrList::m_zMsgStrList[1024];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalStrList::CGlobalStrList()
{
	if( ++m_lInstanceStrList == 1 )
		InitializeCriticalSection(&m_csStrList);
}

CGlobalStrList::~CGlobalStrList()
{
	if( --m_lInstanceStrList == 0 )
	{
		if( !m_lstStrBuf.empty() )
			RemoveAll();
		DeleteCriticalSection(&m_csStrList);
	}
}


VOID CGlobalStrList::Add(const char* pBuf)
{
	Lock();
	
	string str(pBuf);
	m_lstStrBuf.push_back(str);

	UnLock();
}

int	CGlobalStrList::Peek(char* pOutBuf)
{
	int nRet = 0;
	if(m_lstStrBuf.empty()){
		return 0;
	}

	*(pOutBuf) = 0x00;

	Lock();
	try
	{
		strcpy( pOutBuf, (*m_lstStrBuf.begin()).c_str());		
	}
	catch(...)
	{
		UnLock();
	}
	UnLock();

	return strlen(pOutBuf);
}


int CGlobalStrList::Get(char* pOutBuf)
{
	int nLen = 0;
	BOOL bExist = FALSE;
	Lock();
	__try
	{
		__try {
			bExist = GetInner(pOutBuf, &nLen);
		}
		__except(ReportException(GetExceptionCode(), "CGlobalStrList::Get", m_zMsgStrList))
		{}
	}
	__finally
	{
		if(bExist)	m_lstStrBuf.pop_front();
		UnLock();
	}
	return nLen;
}

BOOL CGlobalStrList::GetInner(char* pOutBuf, int* pnLen)
{
	BOOL bExist = FALSE;
	if(m_lstStrBuf.empty()){
		return 0;
	}

	bExist = TRUE;
	*pOutBuf = 0x00;
	strcpy(pOutBuf, (*m_lstStrBuf.begin()).c_str());		
	*pnLen = strlen(pOutBuf);
	return bExist;
}


/*
char* CGlobalStrList::GetList()
{
	char* pRet = NULL;
	if(m_lstStrBuf.empty()){
		return NULL;
	}

	LOCK(&m_csStrList);
	try{		
		pRet = (char*)(*m_lstStrBuf.begin()).c_str();		
		UNLOCK(&m_csStrList);
	}
	catch(...){			
		UNLOCK(&m_csStrList);
		return NULL;
	}	
	return pRet;
}
*/

void CGlobalStrList::Pop()
{
	Lock();

	__try
	{
		__try
		{
			if (m_lstStrBuf.empty() == false)
			{
				m_lstStrBuf.pop_front();
			}
		}
		__except(ReportException(GetExceptionCode(), "CGlobalStrList::Pop", m_zMsgStrList))
		{ }
	}
	__finally
	{
		UnLock();
	}
}

void CGlobalStrList::RemoveAll()
{
	if(!m_lstStrBuf.empty())
		return ;
	
	Lock();
	m_lstStrBuf.clear();
	UnLock();

}

BOOL CGlobalStrList::IsEmpty()
{
	if(m_lstStrBuf.empty())
		return TRUE;
	return FALSE;
}


int CGlobalStrList::GetListSize()
{
	return m_lstStrBuf.size();
}