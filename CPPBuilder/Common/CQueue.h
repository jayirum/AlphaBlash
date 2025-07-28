#ifndef _C_QUEUE_H__
#define _C_QUEUE_H__


#include <System.SysUtils.hpp>
#include <deque>
using namespace std;

typedef struct _QItem
{
	AnsiString	sKey;
	AnsiString	sCode;
	AnsiString	sData;
	AnsiString	etc;
}QItem, *PQItem;

class CQueue
{
public:
	CQueue();
	~CQueue();

public:
	void	Add(AnsiString sKey, AnsiString sCode, AnsiString sData, AnsiString etc="");
	PQItem	Get();
	String 	GetMsg(){ return m_msg;}
private:
	void 	Lock() { EnterCriticalSection(&m_cs); }
	void 	Unlock() { LeaveCriticalSection(&m_cs); }
private:
	deque<QItem*>		m_deq;
	CRITICAL_SECTION	m_cs;
	String				m_msg;
};



#endif