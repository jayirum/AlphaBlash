
#pragma once

#include <windows.h>
#pragma warning( disable : 4786 )
#include <list>
using namespace std;


struct TUnit
{
	int size;
	void* p;
};

class CGlobalQ
{
public:
	CGlobalQ();
	virtual ~CGlobalQ();

	VOID	Add(const void* pData, int nSize);
	BOOL	Get(void * pData, int* pnSize);
	
	//void	Pop();
	void	RemoveAll();
	BOOL	IsEmpty();
	int		GetListSize();

private:
	BOOL	GetInner(void* pOut, int* pnSize);
	void	Lock() { EnterCriticalSection(&m_cs); }
	void	UnLock() { LeaveCriticalSection(&m_cs); }
private:
	CRITICAL_SECTION	m_cs;
	list<TUnit*>		m_lstBuf;
	char				m_zMsg[1024];

};
