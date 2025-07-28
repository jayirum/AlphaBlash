// LogMsg.cpp: implementation of the CLogMsg class, CSendMsg class, CLogMsgPool class
//
//////////////////////////////////////////////////////////////////////

#include "LogMsg.h"
#include <stdio.h>
#include <share.h>
#include <locale.h>
#include "Util.h" //todo after completion - remove ../
#include "TcpClient.h"

#pragma warning(disable:4996)

BOOL CLogMsg::OpenLogEx(wchar_t* psSvrName, wchar_t* psPath, wchar_t* pFileName, wchar_t* szIP,
	int nPort, wchar_t* szApplicationName)
{
	_tcscpy(m_szNotifyServerIP, szIP); 
	m_nNotifyServerPort = nPort; 
	m_csmNotifyThread.setNotifyServerIP(m_szNotifyServerIP);
	m_csmNotifyThread.setNotifyServerPort(m_nNotifyServerPort);

	//GetComputerNameIntoString();
	_tcscpy(m_zSendSvrName, psSvrName);
	_tcscpy(m_zSendAppName, szApplicationName);
	return OpenLog(psPath, pFileName);
};


CSendMsg::CSendMsg() :CBaseThread()
{
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


CLogMsg::CLogMsg():CBaseThread()
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

BOOL CLogMsg::OpenLog(const wchar_t* psPath, const wchar_t* pFileName)
{
	_tcscpy( m_szPureFileName, pFileName );

	SYSTEMTIME st;
	GetLocalTime(&st);
	_stprintf(m_szDate, TEXT("%04d%02d%02d"), st.wYear, st.wMonth, st.wDay);
	lstrcpy( m_szPath, psPath);
	_stprintf(m_szFileName, TEXT("%s\\%s_%s.log"), m_szPath, pFileName, m_szDate);
	//LOCK();
	Close();

	BOOL bFirst = FALSE;
	if (m_fd == NULL)
		bFirst = TRUE;

#ifdef  UNICODE
	setlocale(LC_ALL, ".OCP"); // locale 설정	
	m_fd = _tfopen(m_szFileName, TEXT("ab"));
#elif
	m_fd = _tfopen(m_szFileName, TEXT("at"));
#endif
	if( m_fd == NULL ){
		//UNLOCK();
		return FALSE;
	}

#ifdef  UNICODE
	if (bFirst) 
	{
		// 해당 파일이 유니코드 파일이란 것을 명시하기 위해
		// 파일의 가장 앞에 0xfeff 를 write한다.
		wchar_t mark = 0xFEFF;
		fwrite(&mark, sizeof(wchar_t), 1, m_fd);
	}
#endif
	return TRUE;
}




BOOL CLogMsg::ReOpen()
{
	wchar_t szToday[8+1];
	SYSTEMTIME st;
	GetLocalTime(&st);
	_stprintf(szToday, TEXT("%04d%02d%02d"), st.wYear, st.wMonth, st.wDay);
	if(_tcscmp(szToday, m_szDate)>0 )
	{
		Close();
		return OpenLog( m_szPath, m_szPureFileName );
		
	}
	return	TRUE;
}

VOID CLogMsg::enter()
{
	char enter[32] = "\n";
	fwrite(enter, 1, 1, m_fd);
	return;
}

VOID CLogMsg::logW(LOGMSG_TP tp, const wchar_t* pMsg, ...)
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
			va_start(argptr, pMsg);
			_vstprintf(p->msg, pMsg, argptr);
			va_end(argptr);

			p->tp = tp;

			PostThreadMessage((DWORD)m_dwThreadID, WM_LOGMSG_LOG, (WPARAM)0, (LPARAM)p);
		}
		__except (ReportException(GetExceptionCode(), TEXT("LogMsg::log"), m_szMsg))
		{
			m_pool->Restore(p);
		}
	}
	__finally
	{
		UNLOCK();
	}
}


VOID CLogMsg::log(LOGMSG_TP tp, const char* pMsg,...)
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

			char zMsg[4096] = { 0, };
			va_list argptr;
			va_start(argptr, pMsg);
			vsprintf(zMsg, pMsg, argptr);
			va_end(argptr);

			p->tp = tp;
			A2U(zMsg, p->msg);

			PostThreadMessage((DWORD)m_dwThreadID, WM_LOGMSG_LOG, (WPARAM)0, (LPARAM)p);
		}
		__except (ReportException(GetExceptionCode(), TEXT("LogMsg::log"), m_szMsg))
		{
			m_pool->Restore(p);
		}
	}
	__finally
	{
		UNLOCK();
	}
}

VOID CLogMsg::Log(LOGMSG_TP tp, const wchar_t* pMsg)
{
	ST_LOGMSG* p = NULL; // error C4703: potentially uninitialized local pointer variable 'p' used , thus on 2017.10.13, Ikram made this modification by adding = nullptr
	__try
	{
		LOCK();
		//__try
		{
			p = m_pool->Get();
			if (p == NULL)
				__leave;

			_stprintf(p->msg, pMsg);
			p->tp = tp;

			PostThreadMessage((DWORD)m_dwThreadID, WM_LOGMSG_LOG, (WPARAM)0, (LPARAM)p);
		}
		//__except (ReportException(GetExceptionCode(), "LogMsg::log", m_szMsg))
		//{
		//	m_pool->Restore(p);
		//}
	}
	__finally
	{
		UNLOCK();
	}
}




VOID	CLogMsg::logMsg(ST_LOGMSG* p)
{
	if (p->tp == DEBUG_)
	{
#ifndef _DEBUG
		return;
#endif // !_DEBUG

	}
	wchar_t buff[DEF_LOG_LEN] = { 0, };
	//char tmpbuff[DEF_LOG_LEN] = { 0, };
	int nNotify(0);
	SYSTEMTIME	st;

	__try
	{
		//__try
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
				_stprintf(buff, TEXT("[%02d:%02d:%02d.%03d]============================================\n"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
				fwprintf(m_fd, buff);
				fflush(m_fd);
				m_pool->Restore(p);
			}
			else if (p->tp == DATA)
			{
				_stprintf(buff, TEXT("%.*s\n"),  DEF_LOG_LEN*sizeof(wchar_t) - 20, p->msg);
				fwprintf(m_fd, buff);
				fflush(m_fd);
				m_pool->Restore(p);
			}
			else
			{
				if (p->tp == LOGTP_SUCC)	_tcscpy(buff, TEXT("[I]"));
				if (p->tp == INFO)			_tcscpy(buff, TEXT("[I]"));
				if (p->tp == LOGTP_ERR)		_tcscpy(buff, TEXT("[E]"));
				if (p->tp == ERR)			_tcscpy(buff, TEXT("[E]"));
				if (p->tp == LOGTP_FATAL)	_tcscpy(buff, TEXT("[F]"));
				if (p->tp == DEBUG_)		_tcscpy(buff, TEXT("[D]"));
				if (p->tp == NOTIFY)
				{
					_tcscpy(buff, TEXT("[N]"));
					nNotify = 1;
				}

				_stprintf(buff + 3, TEXT("[%02d:%02d:%02d.%03d]%.*s\n"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, DEF_LOG_LEN - 20, p->msg);
				fwprintf(m_fd, buff);
				fflush(m_fd);
				printf("%.80ls\n", buff);
				m_pool->Restore(p);

				//notification send to Server
				if (nNotify == 1)
				{
					buff[_tcslen(buff) - 1] = 0x00;

					NOTI_LOG* pNoti = new NOTI_LOG;
					memset(pNoti, 0x20, sizeof(NOTI_LOG));
					pNoti->STX[0] = NOTI_STX;
					
					memcpy(pNoti->zSrvName, m_zSendSvrName, _tcslen(m_zSendSvrName));
					memcpy(pNoti->zAppName, m_zSendAppName, _tcslen(m_zSendAppName));

					memcpy(pNoti->zBody, buff, _tcslen(buff));

					pNoti->ETX[0] = NOTI_ETX;
					
					PostThreadMessage(m_csmNotifyThread.getMyThreadID(), WM_LOGMSG_NOTI, (WPARAM)0, (LPARAM)pNoti);
				}
			}
		}
		//__except (ReportException(GetExceptionCode(), "LogMsg::logMsg", m_szMsg))
		//{
		//}
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
				logMsg((ST_LOGMSG*)msg.lParam);
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
		m_logPool.push_back(p);
	}
	LeaveCriticalSection(&m_cs);}

VOID CLogMsg::Close()
{
	if( m_fd != NULL){
		fclose(m_fd);
		m_fd = NULL;
	}
}




VOID CLogMsg::GetComputerNameIntoString()
{
	//wchar_t infoBuf[INFO_BUFFER_SIZE] = { 0, };
	DWORD bufCharCount = INFO_BUFFER_SIZE;

	//Get the name of the computer.
	if (!GetComputerName(m_zSendSvrName, &bufCharCount))
		_tcscpy(m_zSendSvrName, TEXT("NOTAVAILABLE"));

}

BOOL CSendMsg::fn_SendMessage(NOTI_LOG* pNoti, int nTimeOut)
{
	int nErrorCode;

	if (m_pMonitorClient == NULL)
		 m_pMonitorClient = new CTcpClient();
	if (!m_pMonitorClient->IsConnected())
	{
		if (!m_pMonitorClient->Begin(m_szNotifyServerIP, m_nNotifyServerPort, nTimeOut, nTimeOut))
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

int CNotiLogBuffering::AddPacket(wchar_t* pBuf)
{
	if (!pBuf)
		return 0;

	int nRet = 0;
	Lock();
	__try
	{
		//__try
		{
			//memcpy(m_buf + m_nBufLen, pBuf, nSize);
			_tcscat(m_buf, pBuf);
			m_nBufLen += _tcslen(pBuf);
		}
		//__except (ReportException(GetExceptionCode(), "AddPacket", m_msg))
		//{
		//	nRet = -1;
		//}
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
BOOL	CNotiLogBuffering::GetOnePacket(_Out_ wchar_t* pOutBuf, _Out_ BOOL* pbContinue)
{
	if (!pOutBuf) return FALSE;
	
	BOOL bRet;
	Lock();

	__try
	{
		//__try
		{
			bRet = GetOnePacketFn(pOutBuf, pbContinue);
		}
		//__except (ReportException(GetExceptionCode(), "GetOnePacket", m_msg))
		//{
		//	bRet = FALSE;
		//}
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

BOOL	CNotiLogBuffering::GetOnePacketFn(_Out_ wchar_t* pOutBuf, _Out_ BOOL* pbContinue)
{
	BOOL bCopied = TRUE;

	if (m_nBufLen == 0) {
		_tcscpy(m_msg, TEXT("No data in the buffer"));
		UnSetContinue(pbContinue);
		return (!bCopied);
	}

	//	find stx
	wchar_t* pStx;

	pStx = _tcschr(m_buf, NOTI_STX);
	if (pStx == NULL) {
		_tcscpy(m_msg, TEXT("No STX in the packet"));
		RemoveAll();
		UnSetContinue(pbContinue);
		return (!bCopied);
	}

	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
	if (pStx != m_buf)
	{
		wchar_t backup[MAX_BUFFERING] = { 0, };
		_tcscpy(backup, pStx);
		ZeroMemory(m_buf, sizeof(m_buf));
		_tcscpy(m_buf, backup);
		
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

	wchar_t backup[MAX_BUFFERING] = { 0, };
	_tcscpy(backup, m_buf + sizeof(NOTI_LOG));
	ZeroMemory(m_buf, sizeof(m_buf));
	m_nBufLen = _tcslen(backup);

	if (m_nBufLen > 0) {
		_tcscpy(m_buf, backup);
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
