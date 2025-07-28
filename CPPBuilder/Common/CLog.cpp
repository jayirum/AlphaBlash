// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "CLog.h"
#include <stdio.h>
#include <share.h>
#include <System.IOUtils.hpp>
#include <assert.h>
#include "CTimeUtils.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog():m_fd(-1)
{
	InitializeCriticalSection(&m_cs);
}

CLog::~CLog()
{
	//Close();
	DeleteCriticalSection(&m_cs);
}

bool CLog::OpenLog(String sPath, String sFileName, String sToday)
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

	//m_fd = FileOpen(m_sFullFileName, fmOpenReadWrite | fmShareDenyNone );

	if( m_fd < 0 )
	{
		int err = GetLastError();
		m_sMsg = m_sMsg.sprintf(L"file open error:%d_%s", GetLastError(), SysErrorMessage(GetLastError()));
		return false;
	}


	return true;
}


bool CLog::ReOpen()
{
	CTimeUtils util;
	String sToday = util.Today_yyyymmdd();
	if( CompareStr(sToday,m_sDate)>0 )
	{
		return OpenLog( m_sPath, m_sPureFileName, sToday );

	}
	return	true;
}

void CLog::log(const char* pMsg,... )
{
	ReOpen();

	//LOCK();
	if( m_fd <= 0 ){
		//UNLOCK();
		return;
	}



	m_zBuffTmp[0] = 0;
	va_list argptr;
	va_start(argptr, pMsg);
	vsprintf_s(m_zBuffTmp, __ALPHA::LEN_BUF, pMsg, argptr);
	va_end(argptr);

	CTimeUtils util;
	sprintf( m_zBuff, "[%s]%s\n", util.Time_hhmmssmmmA().c_str(), m_zBuffTmp);

	//FileWrite(m_fd, m_zBuff, strlen(m_zBuff));
	_write(m_fd, m_zBuff, strlen(m_zBuff));

	//UNLOCK();
}


void CLog::logW(const wchar_t* pMsg )
{
	ReOpen();

	//LOCK();
	if( m_fd <= 0 ){
		//UNLOCK();
		return;
	}

	CTimeUtils util;
	sprintf( m_zBuff, "[%s]%s\n", util.Time_hhmmssmmmA().c_str(), AnsiString(String(pMsg)).c_str());

	_write(m_fd, m_zBuff, strlen(m_zBuff));

	//UNLOCK();
}

void CLog::Close(
{
	if( m_fd > -1){
		_close(m_fd);
		m_fd = -1;
	}
)
