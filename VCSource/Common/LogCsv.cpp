// Log.cpp: implementation of the CLogCsv class.
//
//////////////////////////////////////////////////////////////////////

#include "LogCsv.h"
#include <stdio.h>
#include <share.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogCsv::CLogCsv():m_fd(0)
{
	ZeroMemory(m_szFileName, sizeof(m_szFileName));
	ZeroMemory(m_szDate, sizeof(m_szDate));
	ZeroMemory(m_szPureFileName, sizeof(m_szPureFileName) );
	m_bDateTailed = false;
	m_bColumnDone = false;
	InitializeCriticalSection(&m_cs);
}

CLogCsv::~CLogCsv()
{
	Close();
	DeleteCriticalSection(&m_cs);
}

BOOL CLogCsv::Open(const char* psPath, const char* pFileName, const char* pzSubFolder, bool bDateTailed)
{
	m_bDateTailed = bDateTailed;

	lstrcpy( m_szPureFileName, pFileName );

	m_szDate[0] = 0;

	if (m_bDateTailed)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf_s(m_szDate, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	}
	
	lstrcpy( m_szPath, psPath);
	if (m_szPath[strlen(m_szPath) - 1] == '.')	// remove dot if it exists at the last
		m_szPath[strlen(m_szPath) - 1] = 0;

	if (m_szPath[strlen(m_szPath) - 1] == '\\')	// remove dot if it exists at the last
		m_szPath[strlen(m_szPath) - 1] = 0;

	if (pzSubFolder != NULL)
		sprintf(&m_szPath[strlen(m_szPath)], "\\%s", pzSubFolder);

	if(bDateTailed)
		sprintf_s(m_szFileName, "%s\\%s_%s.csv", m_szPath, pFileName, m_szDate);
	else
		sprintf_s(m_szFileName, "%s\\%s.csv", m_szPath, pFileName);

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


BOOL CLogCsv::ReOpen()
{
	if (m_bDateTailed)
	{
		char szToday[8 + 1] = { 0 };
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf_s(szToday, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
		if (strcmp(szToday, m_szDate) > 0)
		{
			//return OpenLog( m_szPath, m_szFileName );
			return Open(m_szPath, m_szPureFileName, NULL, m_bDateTailed);

		}
	}
	return	TRUE;
}

BOOL CLogCsv::Column_AlreadExist(const char* pzColumName)
{
	int fd;
	errno_t err = _sopen_s(&fd, m_szFileName, _O_RDONLY, _SH_DENYNO, _S_IREAD );

	if (err < 0) {
		return FALSE;
	}
	
	char zLine[1024] = { 0 };
	int size = _read(fd, zLine, sizeof(zLine));
	if (size <= 0)
	{
		_close(fd);
		return FALSE;
	}
	_close(fd);
	char* pFind = strstr(zLine, pzColumName);
	return (pFind != NULL);
}

BOOL CLogCsv::Column_Add(const char* pzColumn, bool bLastColumn)
{
	if (m_bColumnDone)
	{
		sprintf(m_zMsg, "Columns exist already. Clear first");
		return FALSE;
	}

	if (Column_AlreadExist(pzColumn))
		return TRUE;

	m_lstColumn.push_back(string(pzColumn));

	if (bLastColumn)
	{
		m_bColumnDone = bLastColumn;

		string buf;
		list<string>::iterator it;
		UINT nLoop = 0;
		for (it = m_lstColumn.begin(); it != m_lstColumn.end(); ++it, nLoop++)
		{
			buf += *it;
			if (nLoop < m_lstColumn.size() - 1)
			{
				buf += ",";
			}
		}
		log(buf.c_str());
		log("\n");
	}
	return TRUE;
}


VOID CLogCsv::Column_Clear()
{
	m_lstColumn.clear();
}

VOID CLogCsv::Data_Add(const char* pzData, bool bLastData)
{
	m_lstData.push_back(string(pzData));

	if (bLastData)
	{
		string buf;
		list<string>::iterator it;
		UINT nLoop = 0;
		for (it = m_lstData.begin(); it != m_lstData.end(); ++it, nLoop++)
		{
			buf += *it;
			if (nLoop < m_lstData.size() - 1)
			{
				buf += ",";
			}
		}
		log(buf.c_str());
		log("\n");
		m_lstData.clear();
	}
}


VOID CLogCsv::Data_Add(const string sData, bool bLastData)
{
	Data_Add(sData.c_str(), bLastData);
}


VOID CLogCsv::Data_Add(const char cData, bool bLastData)
{
	char zData[512] = { 0 };
	sprintf(zData, "%c", cData);
	Data_Add(zData, bLastData);
}


VOID CLogCsv::Data_Add(const int nData, bool bLastData)
{
	char zData[512] = { 0 };
	sprintf(zData, "%d", nData);
	
	Data_Add(zData, bLastData);
}

VOID CLogCsv::Data_Add(const double	dData, bool bLastData)
{
	char zData[512] = { 0 }; 
	sprintf(zData, "%f", dData);

	Data_Add(zData, bLastData);
}


VOID CLogCsv::log(const char* pzMsg)
{
	ReOpen();

	
	if (m_fd <= 0) {
		return;
	}

	
	_write(m_fd, pzMsg, (unsigned int)strlen(pzMsg));
	
	
}


VOID CLogCsv::Close()
{
	if( m_fd > 0){
		_close(m_fd);
		m_fd = 0;
	}
}
