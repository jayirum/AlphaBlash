#ifndef __C_BASETHREAD_H__
#define __C_BASETHREAD_H__

#include <Windows.h>
#include <stdio.h>
#include <process.h>

class CBaseThread
{
public:

	CBaseThread(const char* pzName=NULL, bool bSuspend=true);
	virtual ~CBaseThread();

	void StopThread();
	void ResumeThread();

	static unsigned WINAPI ThreadProc(LPVOID lp);

	unsigned int	GetMyThreadID();
	bool 	Is_TimeOfStop(int nTime=10);
	bool 	Is_Alive();
	void 	CloseThreadHandle();

protected:
	virtual void	ThreadExec()=0;

public:
	HANDLE			m_hThread;
	HANDLE			m_hDie;
	unsigned int	m_dwThreadID;

	char			m_zName[128];
	bool			m_bSuspend;
	bool			m_bContinue;
};


#endif // !defined(AFX_BASETHREAD_H__2B8AC972_6959_4C37_BF19_E5683E3F9AA9__INCLUDED_)
