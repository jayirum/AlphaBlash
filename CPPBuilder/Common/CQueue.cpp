
#include "CQueue.h"



CQueue::CQueue()
{
	InitializeCriticalSection(&m_cs);
}

CQueue::~CQueue()
{
	DeleteCriticalSection(&m_cs);
}

void CQueue::Add(AnsiString sKey, AnsiString sCode, AnsiString sData, AnsiString etc)
{
	QItem *p 	= new QItem;
	p->sKey		= sKey;
	p->sCode 	= sCode;
	p->sData 	= sData;
	p->etc 		= etc;

	Lock();
	m_deq.push_back(p);
	Unlock();
}

PQItem	CQueue::Get()
{
	PQItem p = NULL;
	Lock();
	if( m_deq.size() > 1 )
	{
		p = m_deq[0];
		m_deq.erase(m_deq.begin());
    }
	Unlock();

	return p;
}

