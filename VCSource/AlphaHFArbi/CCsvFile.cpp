#include "CCsvFile.h"


CCsvFile::CCsvFile(string sSymbol, string sDir)
{
	m_sSymbol	= sSymbol;
	m_sDir		= sDir;
	m_fd		= -1;

	InitializeCriticalSection(&m_cs);
}


CCsvFile::~CCsvFile()
{
	DeleteCriticalSection(&m_cs);
}


BOOL CCsvFile::OpenFile(string sTimeMT4)
{
	BOOL bReOpen = FALSE;
	string sDate = sTimeMT4.substr(0, 10);
	if (m_sDateMT4 != sTimeMT4)
	{
		m_sDateMT4 = sDate;

		char zFileName[_MAX_PATH];
		sprintf(zFileName, "%s\\%s_%s.csv",
			m_sDir.c_str(),
			m_sSymbol.c_str(),
			m_sDateMT4.c_str()
		);

		Close();

		errno_t err = _sopen_s(&m_fd, zFileName, _O_CREAT | _O_APPEND | _O_WRONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);

		if (err < 0) {
			return FALSE;
		}

		sprintf(m_zData, "LocalDate,LocalTime,Broker,Symbol,Bid,Ask,MT4Time");
		LOCK();
		_write(m_fd, m_zData, strlen(m_zData));
		UNLOCK();
	}
	return TRUE;
}



VOID CCsvFile::Close()
{
	if (m_fd > 0) 
	{
		_close(m_fd);
		m_fd = 0;

		m_sDateMT4.clear();
	}
}


BOOL CCsvFile::WriteData(LPCSTR pzLocalDate, LPCSTR pzLocalTime, LPCSTR pzTimeMT4, LPCSTR broker, LPCSTR bid, LPCSTR ask)
{
	if (!OpenFile(pzTimeMT4))
		return FALSE;

	// LocalDate,LocalTime,Broker,Symbol,Bid,Ask,MT4Time
	sprintf(m_zData, "%s,%s,%s,%s,%s,%s,%s", pzLocalDate, pzLocalTime, broker, m_sSymbol.c_str(), bid, ask, pzTimeMT4);

	LOCK();
	int len = _write(m_fd, m_zData, strlen(m_zData));
	UNLOCK();

	return (len > 0);
}
