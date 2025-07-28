#pragma once
#pragma warning(disable:4996)
/*
	The date as a part of the file name MUST change according to MT4_TIME

*/

#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include <string>
using namespace std;

class CCsvFile
{
public:
	CCsvFile(string sSymbol, string sDir);
	~CCsvFile();

	VOID	Close();
	BOOL	WriteData(LPCSTR pzLocalDate, LPCSTR pzLocalTime, LPCSTR pzTimeMT4, LPCSTR broker, LPCSTR bid, LPCSTR ask);
private:
	BOOL	OpenFile( string sTimeMT4);

	VOID	LOCK() { EnterCriticalSection(&m_cs); };
	VOID	UNLOCK() { LeaveCriticalSection(&m_cs); };

private:
	int		m_fd;
	string	m_sSymbol;
	string	m_sDir;
	string	m_sDateMT4;	//yyyy.mm.dd
	char	m_zData[1024];

	CRITICAL_SECTION m_cs;
	
};

