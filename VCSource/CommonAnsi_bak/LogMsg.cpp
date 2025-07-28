// LogMsg.cpp: implementation of the CLogMsg class, CSendMsg class, CLogMsgPool class
//
//////////////////////////////////////////////////////////////////////

#include "LogMsg.h"
#include <stdio.h>
#include <share.h>
#include "Util.h" //todo after completion - remove ../
#include "TcpClient.h"
#include "../Common/AlphaInc.h"
#include "Prop.h"

#pragma warning(disable:4996)

BOOL CLogMsg::OpenLogEx(char* szApplicationName, char* psPath, char* pFileName, char* szIP,	int nPort )
{
	strcpy(m_szNotifyServerIP, szIP); 
	m_nNotifyServerPort = nPort; 
	m_csmNotifyThread.setNotifyServerIP(m_szNotifyServerIP);
	m_csmNotifyThread.setNotifyServerPort(m_nNotifyServerPort);

	//GetComputerNameIntoString();
	//strcpy(m_zSendSvrName, psSvrName);
	strcpy(m_zSendAppName, szApplicationName);
	return OpenLog(psPath, pFileName);
};




BOOL CLogMsg::OpenLogWithAlerm(char* szApplicationName, char* psPath, char* pFileName)
{
	__ReadAlermSvr_IpPort(m_szNotifyServerIP, &m_nNotifyServerPort);

	m_csmNotifyThread.setNotifyServerIP(m_szNotifyServerIP);
	m_csmNotifyThread.setNotifyServerPort(m_nNotifyServerPort);

	//GetComputerNameIntoString();
	//strcpy(m_zSendSvrName, psSvrName);
	strcpy(m_zSendAppName, szApplicationName);
	return OpenLog(psPath, pFileName);
}

CSendMsg::CSendMsg() :CBaseThread("SendMsg")
{
	ZeroMemory(m_szMsg, sizeof(m_szMsg));
	ZeroMemory(m_szNotifyServerIP, sizeof(m_szNotifyServerIP));
	ZeroMemory(m_szAppName, sizeof(m_szAppName));

	m_nNotifyServerPort = 0;
	m_pMonitorClient = NULL;

	InitializeCriticalSection(&m_cs);
	ResumeThread();
}


CSendMsg::~CSendMsg()
{
	StopThread();
	//delete(m_pMonitorClient);
	DeleteCriticalSection(&m_cs);
}


VOID CSendMsg::ThreadFunc()
{
	//std::printf("CSendMsg thread:%d\n", getMyThreadID());
	while (TRUE)
	{
		if (Is_TimeOfStop(1))
			return;

		DWORD dwRet = MsgWaitForMultipleObjects(1, (HANDLE*)&m_hDie, FALSE, 10, QS_ALLPOSTMESSAGE);
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
			case WM_LOGMSG_NOTI:
			{
				fn_SendMessage((NOTI_LOG*)msg.lParam, TCP_TIMEOUT);
				break;
			}
			}
		} // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	} // while (TRUE)
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CLogMsg::CLogMsg():CBaseThread("CLogMsg")
{
	m_fd = 0;
	ZeroMemory(m_szFileName, sizeof(m_szFileName));
	ZeroMemory(m_szDate, sizeof(m_szDate));
	ZeroMemory(m_szPureFileName, sizeof(m_szPureFileName) );
	InitializeCriticalSection(&m_cs);

	m_pool = new CLogMsgPool;

	ResumeThread();

}


CLogMsg::~CLogMsg()
{
	StopThread();
	Close();
	//delete(m_pMonitorClient);
	DeleteCriticalSection(&m_cs);
	delete m_pool;
}

BOOL CLogMsg::OpenLog(const char* psPath, const char* pFileName)
{
	lstrcpy( m_szPureFileName, pFileName );

	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(m_szDate, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	lstrcpy( m_szPath, psPath);
	sprintf_s(m_szFileName, "%s\\%s_%s.log", m_szPath, pFileName, m_szDate);
	//LOCK();
	Close();

	errno_t err = _sopen_s(&m_fd, m_szFileName, _O_CREAT|_O_APPEND|_O_WRONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);

	if( err < 0 ){
		//UNLOCK();
		return FALSE;
	}
	//UNLOCK();
	return TRUE;
}




BOOL CLogMsg::ReOpen()
{
	char szToday[8+1];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(szToday, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	if( strcmp(szToday, m_szDate)>0 )
	{
		//return OpenLog( m_szPath, m_szFileName );
		return OpenLog( m_szPath, m_szPureFileName );
		
	}
	return	TRUE;
}

VOID CLogMsg::enter()
{
	char enter[32] = "\n";
	_write(m_fd, enter, strlen(enter));
	return;
}

VOID CLogMsg::log(LOGMSG_TP tp, const char* pMsg, ...)
{
	ST_LOGMSG* p = NULL; // error C4703: potentially uninitialized local pointer variable 'p' used , thus on 2017.10.13, Ikram made this modification by adding = nullptr
	__try
	{
		LOCK();
		__try
		{
			p = m_pool->Get();
			if (p == NULL)
				__leave;

			va_list argptr;

			//if (lstrlen(pMsg) >= DEF_LOG_LEN)
			//	*(pMsg + DEF_LOG_LEN - 1) = 0x00;

			va_start(argptr, pMsg);
			vsprintf_s(p->msg, pMsg, argptr);
			va_end(argptr);

			p->tp = tp;

			PostThreadMessage((DWORD)m_dwThreadID, WM_LOGMSG_LOG, (WPARAM)0, (LPARAM)p);
		}
		__except (ReportException(GetExceptionCode(), "LogMsg::log", m_szMsg))
		{
			m_pool->Restore(p);
		}
	}
	__finally
	{
		UNLOCK();
	}
}

VOID CLogMsg::SendAlerm(LOGMSG_TP tp, const char* pMsg)
{
	Log(tp, pMsg, FALSE);
}

VOID CLogMsg::SendAlermEmail(const char* pMsg)
{
	Log(ALERM_EMAIL, pMsg);
}

VOID CLogMsg::SendAlermTelegram(const char* pMsg)
{
	Log(ALERM_TELEGRAM, pMsg);
}

VOID CLogMsg::SendAlermBoth(const char* pMsg)
{
	Log(ALERM_BOTH, pMsg);
}


VOID CLogMsg::log_print(LOGMSG_TP tp, const char* pMsg, ...)
{
	ST_LOGMSG* p = m_pool->Get();
	if (p == NULL)
		return;

	va_list argptr;

	//if (lstrlen(pMsg) >= DEF_LOG_LEN)
	//	*(pMsg + DEF_LOG_LEN - 1) = 0x00;

	va_start(argptr, pMsg);
	vsprintf_s(p->msg, pMsg, argptr);
	va_end(argptr);

	p->tp = tp;
	p->bPrintConsole = TRUE;

	PostThreadMessage((DWORD)m_dwThreadID, WM_LOGMSG_LOG, (WPARAM)0, (LPARAM)p);
}

VOID CLogMsg::Log(LOGMSG_TP tp, const char* pMsg, BOOL bPrintConsole)
{
	ST_LOGMSG* p = NULL; // error C4703: potentially uninitialized local pointer variable 'p' used , thus on 2017.10.13, Ikram made this modification by adding = nullptr
	__try
	{
		LOCK();
		__try
		{
			p = m_pool->Get();
			if (p == NULL)
				__leave;

			sprintf_s(p->msg, pMsg);
			p->tp = tp;
			p->bPrintConsole = bPrintConsole;
			PostThreadMessage((DWORD)m_dwThreadID, WM_LOGMSG_LOG, (WPARAM)0, (LPARAM)p);
		}
		__except (ReportException(GetExceptionCode(), "LogMsg::log", m_szMsg))
		{
			m_pool->Restore(p);
		}
	}
	__finally
	{
		UNLOCK();
	}
}




VOID	CLogMsg::logMsgFunc(ST_LOGMSG* p)
{
	if (p->tp == DEBUG_)
	{
#ifndef _DEBUG
		return;
#endif // !_DEBUG

	}
	char buff[DEF_LOG_LEN] = { 0, };
	//char tmpbuff[DEF_LOG_LEN] = { 0, };
	BOOL bNotify = FALSE;
	SYSTEMTIME	st;

	__try
	{
		__try
		{
			LOCK();
			ReOpen();

			if (m_fd <= 0) {
				UNLOCK();
				return;
			}
			
			GetLocalTime(&st);

			if (p->tp == DATA_DT)
			{
				sprintf(buff, "[%02d:%02d:%02d.%03d]============================================\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
				_write(m_fd, buff, strlen(buff));
				m_pool->Restore(p);
			}
			else if (p->tp == DATA)
			{
				sprintf(buff, "%.*s\n",  DEF_LOG_LEN - 20, p->msg);
				_write(m_fd, buff, strlen(buff));
				m_pool->Restore(p);
			}
			else
			{
				if (p->tp == LOGTP_SUCC)	strcpy(buff, "[I]");
				if (p->tp == INFO)			strcpy(buff, "[I]");
				if (p->tp == LOGTP_ERR)		strcpy(buff, "[E]");
				if (p->tp == ERR)			strcpy(buff, "[E]");
				if (p->tp == LOGTP_FATAL)	strcpy(buff, "[F]");
				if (p->tp == DEBUG_)			strcpy(buff, "[D]");
				if (p->tp == ALERM_EMAIL || p->tp == ALERM_TELEGRAM || p->tp == ALERM_BOTH)
				{
					bNotify = TRUE;
					sprintf(buff, "%s\n",p->msg);
				}

				if (!bNotify)
				{
					sprintf(buff + 3, "[%02d:%02d:%02d.%03d]%.*s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, DEF_LOG_LEN - 20, p->msg);
					_write(m_fd, buff, strlen(buff));

					if (p->bPrintConsole)
						printf("%.80s\n", buff);
				}

				//notification send to Server
				if (bNotify == TRUE)
				{
					//buff[strlen(buff) - 1] = 0x00;

					NOTI_LOG* pNoti = new NOTI_LOG;
					memset(pNoti, 0x20, sizeof(NOTI_LOG));
					pNoti->STX[0] = NOTI_STX;
					sprintf(pNoti->AlermTp, "%02d", p->tp);
					memcpy(pNoti->zAppName, m_zSendAppName, strlen(m_zSendAppName));

					char zBody[BUF_LEN];
					sprintf(zBody, "%.*s", min(sizeof(pNoti->zBody), strlen(buff)), buff);
					memcpy(pNoti->zBody, zBody, strlen(zBody));

					pNoti->ETX[0] = NOTI_ETX;
					
					PostThreadMessage(m_csmNotifyThread.getMyThreadID(), WM_LOGMSG_NOTI, (WPARAM)0, (LPARAM)pNoti);
				}

				m_pool->Restore(p);

			}
		}
		__except (ReportException(GetExceptionCode(), "LogMsg::logMsg", m_szMsg))
		{
		}
	}
	__finally
	{
		UNLOCK();
	}

}

VOID CLogMsg::ThreadFunc()
{
	//printf("CLogMsg thread:%d\n", GetMyThreadID());
	while (TRUE)
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
	} // while (TRUE)
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////

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
	if (m_logPool.empty() == TRUE) {
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
VOID CLogMsgPool::Restore(ST_LOGMSG* p)
{
	EnterCriticalSection(&m_cs);
	if (m_logPool.size() >= MAX_POOL)
		delete p;
	else {
		ZeroMemory(p, sizeof(ST_LOGMSG));
		p->bPrintConsole = TRUE;
		m_logPool.push_back(p);
	}
	LeaveCriticalSection(&m_cs);}

VOID CLogMsg::Close()
{
	if( m_fd > 0){
		_close(m_fd);
		m_fd = 0;
	}
}




//VOID CLogMsg::GetComputerNameIntoString()
//{
//	//TCHAR infoBuf[INFO_BUFFER_SIZE] = { 0, };
//	DWORD bufCharCount = INFO_BUFFER_SIZE;
//
//	//Get the name of the computer.
//	if (!GetComputerName(m_zSendSvrName, &bufCharCount))
//		strcpy(m_zSendSvrName, "NOTAVAILABLE");
//
//}

BOOL CSendMsg::fn_SendMessage(NOTI_LOG* pNoti, int nTimeOut)
{
	int nErrorCode;

	if (m_pMonitorClient == NULL)
		 m_pMonitorClient = new CTcpClient(m_szAppName);
	if (!m_pMonitorClient->IsConnected())
	{
		if (!m_pMonitorClient->Begin(m_szNotifyServerIP, m_nNotifyServerPort, nTimeOut))
		{
			//TODO : Logging must be done by main caller
			return FALSE;
		}
	}

	if (m_pMonitorClient->SendData((char*)pNoti, sizeof(NOTI_LOG), &nErrorCode) < 0)
	{
		//TODO : Logging must be done by main caller
		m_pMonitorClient->End();
		return FALSE;
	}

	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CNotiLogBuffering::CNotiLogBuffering()
{
	m_nBufLen = 0;
	ZeroMemory(m_buf, sizeof(m_buf));
	InitializeCriticalSectionAndSpinCount(&m_cs, 2000);
}

CNotiLogBuffering::~CNotiLogBuffering()
{
	DeleteCriticalSection(&m_cs);
}

int CNotiLogBuffering::AddPacket(char* pBuf)
{
	if (!pBuf)
		return 0;

	int nRet = 0;
	Lock();
	__try
	{
		__try
		{
			//memcpy(m_buf + m_nBufLen, pBuf, nSize);
			strcat(m_buf, pBuf);
			m_nBufLen += strlen(pBuf);
		}
		__except (ReportException(GetExceptionCode(), "AddPacket", m_msg))
		{
			nRet = -1;
		}
	}
	__finally
	{
		Unlock();
	}
	nRet = m_nBufLen;
	return nRet;
}

/*
	return value : 밖에서 이 함수를 한번 더 호출할 것인가 여부

	*pnLen : 실제 pOutBuf 에 copy 되는 size
*/
BOOL	CNotiLogBuffering::GetOnePacket(_Out_ char* pOutBuf, _Out_ BOOL* pbContinue)
{
	if (!pOutBuf) return FALSE;
	
	BOOL bRet;
	Lock();

	__try
	{
		__try
		{
			bRet = GetOnePacketFn(pOutBuf, pbContinue);
		}
		__except (ReportException(GetExceptionCode(), "GetOnePacket", m_msg))
		{
			bRet = FALSE;
		}
	}
	__finally
	{
		Unlock();
	}


	return bRet;
}


/*
	return value : 패킷을copy 했는지 여부

	
*/

void SetContinue(BOOL* pbContinue) { *pbContinue = TRUE; }
void UnSetContinue(BOOL* pbContinue) { *pbContinue = FALSE; }

BOOL	CNotiLogBuffering::GetOnePacketFn(_Out_ char* pOutBuf, _Out_ BOOL* pbContinue)
{
	BOOL bCopied = TRUE;

	if (m_nBufLen == 0) {
		strcpy(m_msg, "No data in the buffer");
		UnSetContinue(pbContinue);
		return (!bCopied);
	}

	//	find stx
	char* pStx;

	pStx = strchr(m_buf, NOTI_STX);
	if (pStx == NULL) {
		strcpy(m_msg, "No STX in the packet");
		RemoveAll();
		UnSetContinue(pbContinue);
		return (!bCopied);
	}

	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
	if (pStx != m_buf)
	{
		char backup[DEF_LOG_LEN] = { 0, };
		strcpy(backup, pStx);
		ZeroMemory(m_buf, sizeof(m_buf));
		strcpy(m_buf, backup);
		
		SetContinue(pbContinue);
		return (!bCopied);
	}

	// 불완전패킷
	if (m_nBufLen < sizeof(NOTI_LOG))
	{
		UnSetContinue(pbContinue);
		return (!bCopied);
	}
	

	// COPY
	// 온전한 하나의 패킷 반환하고 남은 패킷 버퍼 앞으로 이동
	memcpy(pOutBuf, pStx, sizeof(NOTI_LOG));

	char backup[DEF_LOG_LEN] = { 0, };
	strcpy(backup, m_buf + sizeof(NOTI_LOG));
	ZeroMemory(m_buf, sizeof(m_buf));
	m_nBufLen = strlen(backup);

	if (m_nBufLen > 0) {
		strcpy(m_buf, backup);
		SetContinue(pbContinue);
	}
	else
		UnSetContinue(pbContinue);

	return (bCopied);
}

VOID CNotiLogBuffering::RemoveAll()
{
	ZeroMemory(m_buf, sizeof(m_buf));
	m_nBufLen = 0;
}




BOOL __ReadAlermSvr_IpPort(_Out_ char* pzIP, _Out_ int* pnPort)
{
	*pzIP = 0;

	CProp prop;
	prop.SetBaseKey(HKEY_LOCAL_MACHINE, ALPHABLASH_ALERMSVR);
	strcpy(pzIP, prop.GetValue("LISTEN_IP"));
	*pnPort = prop.GetLongValue("LISTEN_PORT");
	prop.Close();

	return(pzIP[0] != NULL);
}