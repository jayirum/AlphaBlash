// Log.h: interface for the CLogCsv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGCSV_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_)
#define AFX_LOGCSV_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4996)
#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <list>
#include <string>
using namespace std;



class CLogCsv  
{
public:
	

	CLogCsv();
	virtual ~CLogCsv();

	BOOL	Open(const char* psPath, const char* pFileName, const char* pzSubFolder=NULL, bool bDateTailed=false);
	BOOL	ReOpen();
	VOID	Close();

	BOOL	Column_AlreadExist(const char* pzColumName);
	BOOL	Column_Add(const char* pzColumn, bool bLastColumn);
	VOID	Column_Clear();
	

	void	Data_Add(const char*	pzData, bool bLastData);
	void	Data_Add(const char cData, bool bLastData);
	void	Data_Add(const string   sData, bool bLastData);
	void	Data_Add(const int		nData, bool bLastData);
	void	Data_Add(const double	dData, bool bLastData);

	void	BeginTx() { EnterCriticalSection(&m_cs); }
	void	EndTx() { LeaveCriticalSection(&m_cs); }

	char*	GetMsg() { return m_zMsg; }


private:
	VOID	log(const char* pzMsg);
	VOID	LOCK(){ EnterCriticalSection(&m_cs); };
	VOID	UNLOCK(){ LeaveCriticalSection(&m_cs); };
	BOOL	isOpenLog(){ return (m_fd>0); };

	char	*GetFileName() { return m_szFileName; }

private:
	int		m_fd;
	char	m_szPath	[_MAX_PATH];
	char	m_szFileName[_MAX_PATH];
	char	m_szPureFileName[_MAX_PATH];
	char	m_szDate	[8+1];
	bool	m_bDateTailed;
	CRITICAL_SECTION m_cs;

	char	m_zMsg[1024];
	list<string>	m_lstColumn;
	list<string>	m_lstData;
	bool			m_bColumnDone;
};

#endif // !defined(AFX_LOG_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_)
