#include "TrThread.h"
#include "irum_common.h"
#include <process.h>
#include <Log.h>

extern CLog	g_log;
extern bool	g_bLog;


CTrThread::CTrThread()
{
	m_tcp = NULL;
	ErrHappened_Reset();
}


CTrThread::~CTrThread()
{
	m_bContinue = false;
	ErrHappened_Reset();
	DeInitialize();
}


bool CTrThread::Initialize(char* pzServerIp, int nServerPort, int nSendTimeout)
{
	if (m_tcp == NULL)
		m_tcp = new CTcp;

	g_log.Log("[CTrThread::Initialize-1]%s, %d", pzServerIp, nServerPort);
	if (m_tcp->Initialize(pzServerIp, nServerPort, nSendTimeout) == FALSE)
	{
		ErrHappened_Set();
		strcpy(m_zMsg, m_tcp->GetMsg());
		return false;
	}
	g_log.Log("[CTrThread::Initialize-2]%s, %d", pzServerIp, nServerPort);
	
	m_hSendThread = (HANDLE)_beginthreadex(NULL, 0, &SendThread, this, 0, &m_dwSendThread);
	g_log.Log("[CTrThread::Initialize-3]%s, %d", pzServerIp, nServerPort);
	return true;
}

void CTrThread::DeInitialize()
{
	if (m_tcp)
	{
		m_tcp->Disconnect();
		delete m_tcp;
	}
	m_tcp = NULL;
}



void CTrThread::SendData(char* pSendBuf, int nBufLen)
{
	char* pData = new char[nBufLen];
	memcpy(pData, pSendBuf, nBufLen);
	PostThreadMessage(m_dwSendThread, WM_PASS_DATA, (WPARAM)nBufLen, (LPARAM)pData);
}

unsigned WINAPI CTrThread::SendThread(LPVOID lp)
{
	g_log.log("SendThread-1");
	CTrThread* p = (CTrThread*)lp;
	p->ThreadFunc();
	return 0;
}


void CTrThread::ThreadFunc()
{
	g_log.Log("ThreadFunc-1");

	// connect
	while (1) 
	{
		if (g_bLog) g_log.Log("Before Connect");
		if (m_tcp->Connect() == true)
			break;
		
		strcpy(m_zMsg, m_tcp->GetMsg());
		if (g_bLog) g_log.Log(m_zMsg);
		ErrHappened_Set();
		Sleep(3000);
	}
	if (g_bLog) g_log.Log("Connect OK in Thread"); 
	ErrHappened_Reset();

	m_bContinue = true;

	MSG msg;
	int nSentSize;
	while (m_bContinue)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_PASS_DATA)
			{
				nSentSize = 0;
				if (m_tcp->SendData((char*)msg.lParam, (int)msg.wParam, &nSentSize) == false) {
					ErrHappened_Set();
					strcpy(m_zMsg, m_tcp->GetMsg());

					//TODO. EMAIL TO Admin
				}
				else {
					ErrHappened_Reset();
					if (g_bLog) g_log.Log("Send OK(%s)", (char*)msg.lParam);
				}

				delete[](char*)msg.lParam;
			} 

		}// while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))

	}// while (Is_TimeOfStop(1))
}

bool CTrThread::Has_ErrorHappened(char* pErrMsg)
{
	if (m_bErrHappened == FALSE)
		return false;

	strcpy(pErrMsg, m_zMsg);
	return true;
}



void CTrThread::SetSvrInfo(char* pzServerIp, int nServerPort)
{
	m_tcp->SetSvrInfo(pzServerIp, nServerPort);
}

