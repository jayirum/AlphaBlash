#include "CBaseThread.h"



CBaseThread::CBaseThread(const char* pzName, bool bSuspend)
{
	ZeroMemory(m_zName, sizeof(m_zName));
	if (pzName)
		strcpy(m_zName, pzName);


	m_bSuspend = bSuspend;

	m_dwThreadID = 0;
	m_hDie = CreateEvent(NULL, false, false, NULL);
	if (m_bSuspend)
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadProc, this, CREATE_SUSPENDED, &m_dwThreadID);
	else {
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadProc, this, 0, &m_dwThreadID);
		m_bContinue = true;
	}
}

CBaseThread::~CBaseThread()
{
	//printf("CBaseThread destructor(%d)(%s)\n", m_dwThreadID, m_zName);
	StopThread();
}

void CBaseThread::StopThread()
{
	m_bContinue = false;
	if(!m_hThread || !m_hDie )	return;

	SetEvent(m_hDie);
	if (WaitForSingleObject(m_hThread, 3000) != WAIT_OBJECT_0){
		DWORD dwExitCode = 0;
		TerminateThread(m_hThread, dwExitCode);
	}
	CloseHandle(m_hDie);
	CloseHandle(m_hThread);
	m_hDie = m_hThread = NULL;
}


void CBaseThread::ResumeThread()
{
	::ResumeThread(m_hThread);
	m_bContinue = true;
}



unsigned WINAPI CBaseThread::ThreadProc(LPVOID lp)
{
	CBaseThread* p = (CBaseThread*)lp;
	p->ThreadExec();
	return 0;
}



unsigned int CBaseThread::GetMyThreadID()
{
	return m_dwThreadID;
}



bool CBaseThread::Is_TimeOfStop(int nTime)
{
	return (WaitForSingleObject(m_hDie, nTime)==WAIT_OBJECT_0);
}


bool CBaseThread::Is_Alive()
{
	DWORD dw;
	GetExitCodeThread(m_hThread, &dw);
	return (dw==STILL_ACTIVE);
}

 void CBaseThread::CloseThreadHandle()
{
	if (m_hThread)
		CloseHandle(m_hThread);

	m_hThread = NULL;
}


