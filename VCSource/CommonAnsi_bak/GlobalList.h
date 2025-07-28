
#pragma once

#include <windows.h>
#pragma warning( disable : 4786 )
#include <list>
#include <string>
using namespace std;

#define	MAX_LIST_CNT	300

struct ST_COPIED
{
	int size;
	void* p;
};

class CGlobalList
{
public:
	CGlobalList();
	virtual ~CGlobalList();

	VOID	Add(const void* pBuf, int nSize);
	BOOL	Get(void * pOutBuf, int* pnSize);
	
	//void	Pop();
	void	RemoveAll();
	BOOL	IsEmpty();
	int		GetListSize();

private:
	BOOL	GetInner(void* pOutBuf, int* pnSize);
	void	Lock() { EnterCriticalSection(&m_cs); }
	void	UnLock() { LeaveCriticalSection(&m_cs); }
private:
	static	long	m_lInstance;
	static	CRITICAL_SECTION m_cs;
	static	list<ST_COPIED*>m_lstBuf;
	static	char m_zMsg[1024];

};
