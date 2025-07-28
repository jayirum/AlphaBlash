
#pragma once

#include <windows.h>
#pragma warning( disable : 4786 )
#include <list>
#include <string>
using namespace std;

#define	MAX_LIST_CNT	300


///////////////////////////////////////////////////////////
////
////	list<string>m_lstStr  를 static 으로 선언하여
////	전역변수로 사용할 수 있도록 한다.
////
class CGlobalStrList
{
public:
	CGlobalStrList();
	virtual ~CGlobalStrList();

	VOID	Add(const char *pBuf);
	int		Get(char* pOutBuf);	
	int		Peek(char* pOutBuf);

	void	Pop();
	void	RemoveAll();
	BOOL	IsEmpty();
	int		GetListSize();
	
private:
	BOOL	GetInner(char* pOutBuf, int *pnLen);
	void	Lock() { EnterCriticalSection(&m_csStrList); }
	void	UnLock() { LeaveCriticalSection(&m_csStrList); }
private:	
	static	long	m_lInstanceStrList;
	static	CRITICAL_SECTION m_csStrList;	
	static	list<string>m_lstStrBuf;
	static	char m_zMsgStrList[1024];

};


//
//struct ST_COPIED
//{
//	int size;
//	void* p;
//};
//
//class CGlobalList
//{
//public:
//	CGlobalList();
//	virtual ~CGlobalList();
//
//	VOID	Add(const void* pBuf, int nSize);
//	int		Get(void * pOutBuf);
//	int		Peek(void * pOutBuf);
//
//	void	Pop();
//	void	RemoveAll();
//	BOOL	IsEmpty();
//	int		GetListSize();
//
//private:
//	BOOL	GetInner(void* pOutBuf, int* pnLen);
//	void	Lock() { EnterCriticalSection(&m_csStrList); }
//	void	UnLock() { LeaveCriticalSection(&m_csStrList); }
//private:
//	static	long	m_lInstance;
//	static	CRITICAL_SECTION m_csStrList;
//	static	list<ST_COPIED>m_lstStrBuf;
//	static	char m_zMsg[1024];
//
//};
