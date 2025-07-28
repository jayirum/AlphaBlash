// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_)
#define AFX_LOG_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <tchar.h>
#define DEF_LOG_LEN 4096
enum { READ = _S_IREAD, WRITE = _S_IWRITE };
class CLog  
{
public:
	CLog();
	virtual ~CLog();

	BOOL	OpenLogW(wchar_t* psPath, wchar_t* pFileName);
	BOOL	OpenLog(char* psPath, char* pFileName);
	BOOL	ReOpen();
	VOID	logW(const wchar_t* pMsg, ...);
	VOID	log(char* pMsg, ...);
	VOID	Close();
	VOID	LOCK(){ EnterCriticalSection(&m_cs); };
	VOID	UNLOCK(){ LeaveCriticalSection(&m_cs); };
	BOOL	isOpenLog(){ return (m_fd>0); };

	int		GetFileName(_Out_ wchar_t* pwsFileName);
	
//private:
	FILE	*m_fd;
	wchar_t	m_szPath	[_MAX_PATH];
	wchar_t	m_szFileName[_MAX_PATH];
	wchar_t	m_szPureFileName[_MAX_PATH];
	wchar_t	m_szDate	[8+1];
	CRITICAL_SECTION m_cs;

};

#endif // !defined(AFX_LOG_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_)
