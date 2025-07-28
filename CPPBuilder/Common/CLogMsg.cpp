// LogMsg.cpp: implementation of the CLogMsg class, CSendMsg class, CLogMsgPool class
//
//////////////////////////////////////////////////////////////////////

#include "CLogMsg.h"
#include <stdio.h>
#include <share.h>

CLogMsg __log;

CLogMsg::CLogMsg():CBaseThread("CLogMsg", false)
{
	m_fd = -1;
	InitializeCriticalSection(&m_cs);
	m_pool = new CLogMsgPool;
}


CLogMsg::~CLogMsg()
{
	CBaseThread::StopThread();
	Close();
	DeleteCriticalSection(&m_cs);
	delete m_pool;
}

bool CLogMsg::OpenLog(String sPath, String sFileName, String sToday)
{
	m_sPath			= sPath;
	m_sPureFileName = sFileName;

	if(sToday.Length()==0)
	{
		CTimeUtils util;
		m_sDate = util.Today_yyyymmdd();
	}
	else
	{
		m_sDate = sToday;
	}

	m_sFullFileName = m_sFullFileName.sprintf(L"%s\\%s_%s.log", m_sPath, m_sPureFileName,m_sDate );

	Close();

	m_fd = _wsopen( m_sFullFileName.c_str(), _O_CREAT|_O_APPEND|_O_RDWR, fmShareDenyNone, _S_IREAD | _S_IWRITE);

	if( m_fd < 0 )
	{
		int err = GetLastError();
		sprintf(m_zMsg, "file open error:%d_%s", GetLastError(), AnsiString(SysErrorMessage(GetLastError())).c_str());
		return false;
	}

	Sleep(100);

	return true;
}




bool CLogMsg::ReOpen()
{
	CTimeUtils util;
	String sToday = util.Today_yyyymmdd();
	if( CompareStr(sToday,m_sDate)>0 )
	{
		return OpenLog( m_sPath, m_sPureFileName, sToday );

	}
	return	true;
}

void CLogMsg::enter()
{
	char enter[32] = "\n";
	_write(m_fd, enter, strlen(enter));
	return;
}

void CLogMsg::log(LOGMSG_TP tp, const char* pMsg, ...)
{
	ST_LOGMSG* p = NULL; // error C4703: potentially uninitialized local pointer variable 'p' used , thus on 2017.10.13, Ikram made this modification by adding = nullptr
	__try
	{
		LOCK();
		__try
		{
			p = m_pool->Get();
			assert(p!=NULL);
			if (p == NULL)
				RaiseException(0, 0,0, NULL);

			va_list argptr;

			va_start(argptr, pMsg);
			vsprintf_s(p->msg, __ALPHA::LEN_BUF, pMsg, argptr);
			va_end(argptr);

			p->tp = tp;

			PostThreadMessage((DWORD)m_dwThreadID, WM_LOGMSG_LOG, (WPARAM)0, (LPARAM)p);
		}
		__except (ReportException(GetExceptionCode(), "LogMsg::log", m_zMsg))
		{
			m_pool->Restore(p);
		}
	}
	__finally
	{
		UNLOCK();
	}
}



void CLogMsg::Log(LOGMSG_TP tp, const char* pMsg, bool bPrintConsole)
{
	ST_LOGMSG* p = NULL; // error C4703: potentially uninitialized local pointer variable 'p' used , thus on 2017.10.13, Ikram made this modification by adding = nullptr
	__try
	{
		LOCK();
		__try
		{
			p = m_pool->Get();
			if (p == NULL)
				RaiseException(0, 0,0, NULL);

			sprintf(p->msg, pMsg);
			p->tp = tp;
			p->bPrintConsole = bPrintConsole;
			PostThreadMessage((DWORD)m_dwThreadID, WM_LOGMSG_LOG, (WPARAM)0, (LPARAM)p);
		}
		__except (ReportException(GetExceptionCode(), "LogMsg::Log", m_zMsg))
		{
			m_pool->Restore(p);
		}
	}
	__finally
	{
		UNLOCK();
	}
}




void	CLogMsg::logMsgFunc(ST_LOGMSG* p)
{
	if (p->tp == DEBUG_)
	{
#ifndef _DEBUG
		return;
#endif // !_DEBUG
    }

	__try
	{
		__try
		{
			LOCK();
			ReOpen();

			if (m_fd < 0) {
				UNLOCK();
				return;
			}


			if (p->tp == DATA_DT)
			{
				sprintf(m_zBuff, "[%s]============================================\n", m_timeUtil.Time_hhmmssmmmA());
				_write(m_fd, m_zBuff, strlen(m_zBuff));
				m_pool->Restore(p);
			}
			else if (p->tp == DATA)
			{
				sprintf(m_zBuff, "%.*s\n",  __ALPHA::BUF_LEN - 20, p->msg);
				_write(m_fd, m_zBuff, strlen(m_zBuff));
				m_pool->Restore(p);
			}
			else
			{
				if (p->tp == LOGTP_SUCC)	strcpy(m_zBuff, "[I]");
				if (p->tp == INFO)			strcpy(m_zBuff, "[I]");
				if (p->tp == LOGTP_ERR)		strcpy(m_zBuff, "[E]");
				if (p->tp == ERR)			strcpy(m_zBuff, "[E]");
				if (p->tp == LOGTP_FATAL)	strcpy(m_zBuff, "[F]");
				if (p->tp == DEBUG_)		strcpy(m_zBuff, "[D]");

					sprintf(m_zBuff + 3, "[%s]%.*s\n", m_timeUtil.Time_hhmmssmmmA(), __ALPHA::BUF_LEN - 20, p->msg);
					_write(m_fd, m_zBuff, strlen(m_zBuff));

					if (p->bPrintConsole)
						printf("%.80s\n", m_zBuff);

					m_pool->Restore(p);
			}

		}
		__except (ReportException(GetExceptionCode(), "LogMsg::logMsg", m_zMsg))
		{
		}
	}
	__finally
	{
		UNLOCK();
	}

}
//
void CLogMsg::ThreadExec()
{

	while (true)
	{
		DWORD dwRet = MsgWaitForMultipleObjects(1, (HANDLE*)&m_hDie, FALSE, 1, QS_ALLPOSTMESSAGE);
		if (dwRet == WAIT_OBJECT_0)
		{
			break;
		}
		else if (dwRet == WAIT_ABANDONED_0) {
			Sleep(1000);
			continue;
		}

		MSG msg;



		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_LOGMSG_LOG:
			{
				logMsgFunc((ST_LOGMSG*)msg.lParam);
				break;
			}
			}
		} // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	} // while (true)
}
void CLogMsg::Close()
{
	if( m_fd > -1)
	{
		_close(m_fd);
		m_fd = -1;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////
//
CLogMsgPool::CLogMsgPool()
{
	InitializeCriticalSectionAndSpinCount(&m_cs, 2000);
	for (int i = 0; i < MIN_POOL; i++)
	{
		ST_LOGMSG* p = new ST_LOGMSG;
		ZeroMemory(p, sizeof(ST_LOGMSG));
		m_logPool.push_back(p);
	}
}

CLogMsgPool::~CLogMsgPool()
{
	DeleteCriticalSection(&m_cs);
}

ST_LOGMSG* CLogMsgPool::Get()
{
	ST_LOGMSG *p;
	EnterCriticalSection(&m_cs);
	if (m_logPool.empty() == true)
	{
		p = new ST_LOGMSG;
		ZeroMemory(p, sizeof(ST_LOGMSG));
	}
	else
	{
		p = *(m_logPool.begin());
		m_logPool.pop_front();
	}
	LeaveCriticalSection(&m_cs);
	return p;
}


void CLogMsgPool::Restore(ST_LOGMSG* p)
{
	EnterCriticalSection(&m_cs);
	if (m_logPool.size() >= MAX_POOL)
	{	delete p; }
	else
	{
		ZeroMemory(p, sizeof(ST_LOGMSG));
		p->bPrintConsole = false;
		m_logPool.push_back(p);
	}
	LeaveCriticalSection(&m_cs);
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// BACKUP

//bool CLogMsg::OpenLogEx(char* szApplicationName, char* psPath, char* pFileName, char* szIP,	int nPort )
//{
//	strcpy(m_szNotifyServerIP, szIP);
//	m_nNotifyServerPort = nPort;
//	m_csmNotifyThread.setNotifyServerIP(m_szNotifyServerIP);
//	m_csmNotifyThread.setNotifyServerPort(m_nNotifyServerPort);
//
//	//GetComputerNameIntoString();
//	//strcpy(m_zSendSvrName, psSvrName);
//	strcpy(m_zSendAppName, szApplicationName);
//	return OpenLog(psPath, pFileName);
//};



//
//bool CLogMsg::OpenLogWithAlerm(char* szApplicationName, char* psPath, char* pFileName)
//{
//	__ReadAlermSvr_IpPort(m_szNotifyServerIP, &m_nNotifyServerPort);
//
//	m_csmNotifyThread.setNotifyServerIP(m_szNotifyServerIP);
//	m_csmNotifyThread.setNotifyServerPort(m_nNotifyServerPort);
//
//	//GetComputerNameIntoString();
//	//strcpy(m_zSendSvrName, psSvrName);
//	strcpy(m_zSendAppName, szApplicationName);
//	return OpenLog(psPath, pFileName);
//}
//
//CSendMsg::CSendMsg() :CBaseThread("SendMsg")
//{
//	ZeroMemory(m_zMsg, sizeof(m_zMsg));
//	ZeroMemory(m_szNotifyServerIP, sizeof(m_szNotifyServerIP));
//	ZeroMemory(m_szAppName, sizeof(m_szAppName));
//
//	m_nNotifyServerPort = 0;
//	m_pMonitorClient = NULL;
//
//	InitializeCriticalSection(&m_cs);
//	ResumeThread();
//}
//
//
//CSendMsg::~CSendMsg()
//{
//	StopThread();
//	//delete(m_pMonitorClient);
//	DeleteCriticalSection(&m_cs);
//}
//
//
//void CSendMsg::ThreadFunc()
//{
//	//std::printf("CSendMsg thread:%d\n", getMyThreadID());
//	while (true)
//	{
//		if (Is_TimeOfStop(1))
//			return;
//
//		DWORD dwRet = MsgWaitForMultipleObjects(1, (HANDLE*)&m_hDie, FALSE, 10, QS_ALLPOSTMESSAGE);
//		if (dwRet == WAIT_OBJECT_0)
//		{
//			break;
//		}
//		else if (dwRet == WAIT_ABANDONED_0) {
//			Sleep(1000);
//			continue;
//		}
//
//		MSG msg;
//		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{
//			switch (msg.message)
//			{
//			case WM_LOGMSG_NOTI:
//			{
//				fn_SendMessage((NOTI_LOG*)msg.lParam, TCP_TIMEOUT);
//				break;
//			}
//			}
//		} // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//	} // while (true)
//}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//bool CSendMsg::fn_SendMessage(NOTI_LOG* pNoti, int nTimeOut)
//{
//	int nErrorCode;
//
//	if (m_pMonitorClient == NULL)
//		 m_pMonitorClient = new CTcpClient(m_szAppName);
//	if (!m_pMonitorClient->IsConnected())
//	{
//		if (!m_pMonitorClient->Begin(m_szNotifyServerIP, m_nNotifyServerPort, nTimeOut))
//		{
//			//TODO : Logging must be done by main caller
//			return FALSE;
//		}
//	}
//
//	if (m_pMonitorClient->SendData((char*)pNoti, sizeof(NOTI_LOG), &nErrorCode) < 0)
//	{
//		//TODO : Logging must be done by main caller
//		m_pMonitorClient->End();
//		return FALSE;
//	}
//
//	return true;
//}
//CNotiLogBuffering::CNotiLogBuffering()
//{
//	m_nBufLen = 0;
//	ZeroMemory(m_buf, sizeof(m_buf));
//	InitializeCriticalSectionAndSpinCount(&m_cs, 2000);
//}
//
//CNotiLogBuffering::~CNotiLogBuffering()
//{
//	DeleteCriticalSection(&m_cs);
//}
//
//int CNotiLogBuffering::AddPacket(char* pBuf)
//{
//	if (!pBuf)
//		return 0;
//
//	int nRet = 0;
//	Lock();
//	__try
//	{
//		__try
//		{
//			//memcpy(m_buf + m_nBufLen, pBuf, nSize);
//			strcat(m_buf, pBuf);
//			m_nBufLen += strlen(pBuf);
//		}
//		__except (ReportException(GetExceptionCode(), "AddPacket", m_msg))
//		{
//			nRet = -1;
//		}
//	}
//	__finally
//	{
//		Unlock();
//	}
//	nRet = m_nBufLen;
//	return nRet;
//}
//
///*
//	return value : 밖에서 이 함수를 한번 더 호출할 것인가 여부
//
//	*pnLen : 실제 pOutBuf 에 copy 되는 size
//*/
//bool	CNotiLogBuffering::GetOnePacket(_Out_ char* pOutBuf, _Out_ bool* pbContinue)
//{
//	if (!pOutBuf) return FALSE;
//
//	bool bRet;
//	Lock();
//
//	__try
//	{
//		__try
//		{
//			bRet = GetOnePacketFn(pOutBuf, pbContinue);
//		}
//		__except (ReportException(GetExceptionCode(), "GetOnePacket", m_msg))
//		{
//			bRet = FALSE;
//		}
//	}
//	__finally
//	{
//		Unlock();
//	}
//
//
//	return bRet;
//}
//
//
///*
//	return value : 패킷을copy 했는지 여부
//
//
//*/
//
//void SetContinue(bool* pbContinue) { *pbContinue = true; }
//void UnSetContinue(bool* pbContinue) { *pbContinue = FALSE; }
//
//bool	CNotiLogBuffering::GetOnePacketFn(_Out_ char* pOutBuf, _Out_ bool* pbContinue)
//{
//	bool bCopied = true;
//
//	if (m_nBufLen == 0) {
//		strcpy(m_msg, "No data in the buffer");
//		UnSetContinue(pbContinue);
//		return (!bCopied);
//	}
//
//	//	find stx
//	char* pStx;
//
//	pStx = strchr(m_buf, NOTI_STX);
//	if (pStx == NULL) {
//		strcpy(m_msg, "No STX in the packet");
//		RemoveAll();
//		UnSetContinue(pbContinue);
//		return (!bCopied);
//	}
//
//	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
//	if (pStx != m_buf)
//	{
//		char backup[DEF_LOG_LEN] = { 0, };
//		strcpy(backup, pStx);
//		ZeroMemory(m_buf, sizeof(m_buf));
//		strcpy(m_buf, backup);
//
//		SetContinue(pbContinue);
//		return (!bCopied);
//	}
//
//	// 불완전패킷
//	if (m_nBufLen < sizeof(NOTI_LOG))
//	{
//		UnSetContinue(pbContinue);
//		return (!bCopied);
//	}
//
//
//	// COPY
//	// 온전한 하나의 패킷 반환하고 남은 패킷 버퍼 앞으로 이동
//	memcpy(pOutBuf, pStx, sizeof(NOTI_LOG));
//
//	char backup[DEF_LOG_LEN] = { 0, };
//	strcpy(backup, m_buf + sizeof(NOTI_LOG));
//	ZeroMemory(m_buf, sizeof(m_buf));
//	m_nBufLen = strlen(backup);
//
//	if (m_nBufLen > 0) {
//		strcpy(m_buf, backup);
//		SetContinue(pbContinue);
//	}
//	else
//		UnSetContinue(pbContinue);
//
//	return (bCopied);
//}
//
//void CNotiLogBuffering::RemoveAll()
//{
//	ZeroMemory(m_buf, sizeof(m_buf));
//	m_nBufLen = 0;
//}
//
//
//
//
//bool __ReadAlermSvr_IpPort(_Out_ char* pzIP, _Out_ int* pnPort)
//{
//	*pzIP = 0;
//
//	CProp prop;
//	prop.SetBaseKey(HKEY_LOCAL_MACHINE, ALPHABLASH_ALERMSVR);
//	strcpy(pzIP, prop.GetValue("LISTEN_IP"));
//	*pnPort = prop.GetLongValue("LISTEN_PORT");
//	prop.Close();
//
//	return(pzIP[0] != NULL);
//}
//
