// Log.h: interface for the CLogMsg class.
//
//////////////////////////////////////////////////////////////////////

#pragma once 

#include <stdio.h>
#include <list>
#include <Windows.h>

const int MEM_BLOCK_SIZE = 1024;
const int MEM_PRE_ALLOC = 100;
const int MEM_MAX_ALLOC = 1000;


class CMemPool
{
public:
	CMemPool(int nPreAlloc= MEM_PRE_ALLOC, int nMaxAlloc= MEM_MAX_ALLOC, int nBlockSize= MEM_BLOCK_SIZE);
	virtual ~CMemPool();

	//bool	Begin();
	void	End();
	bool	get(_Out_ char** pBuf);
	char*	get();
	char*	Get();
	void	release(char* ptr);
	int		available() { return m_listPool.size(); }
private:
	void	clear();


	void LOCK() {EnterCriticalSection(&m_cs);}
	void UNLOCK() {LeaveCriticalSection(&m_cs);}
private:

	std::size_t			m_nPrevAlloc, m_nMaxAlloc;
	std::size_t			m_nBlockSize;
	std::list<char*>		m_listPool;
	

	CRITICAL_SECTION   m_cs;
};