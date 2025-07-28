// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_)
#define AFX_LOG_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_

#include <System.Classes.hpp>
#include <System.SysUtils.hpp>

                                 #include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <share.h>

enum { READ = _S_IREAD, WRITE = _S_IWRITE };

#include "AlphaInc.h"

class CLog  
{
public:
	CLog();
	virtual ~CLog();

	bool	OpenLog(String sPath, String spFileName, String sToday="");
	bool	ReOpen();

//	//int		GetData(/*out*/char* o_pBuf, int nReadLen);
	void	Close();

	void	log(const char* pMsg,... );
	void	logW(const wchar_t* pMsg );
//	void	LogEx(char* pMsg );
//	void	log(char* pMsg);
//
//// 	void	WriteFormat( char* pTr, bool bSend, char* pMsg );
//// 	void	WriteFormat( char* pTr, char* pMsg );
////	void	WriteFormat2(char* pMsg,... );
//	void	WriteNonString(char* pData, int nLen);
//	void	WriteErr( int nErrCode, char* pMsg, ... );
//	void	WriteByByte(char* pData, int nLen);
	void	Lock(){ EnterCriticalSection(&m_cs); };
	void	Unlock(){ LeaveCriticalSection(&m_cs); };
//	bool	isOpenLog(){ return (m_fd>0); };
//
	String	GetFileName() { return m_sFullFileName; }
	String 	GetMsg() { return m_sMsg;}
private:
	int		m_fd;
	String	m_sPath	;
	String 	m_sFullFileName;
	String 	m_sPureFileName;
	String	m_sDate;
	String	m_sMsg;
	CRITICAL_SECTION m_cs;

	char 	m_zBuff[__ALPHA::LEN_BUF];
	char 	m_zBuffTmp[__ALPHA::LEN_BUF];

	//wchar_t m_wzBuff[__ALPHA::LEN_BUF];
	//wchar_t m_wzBuffTmp[__ALPHA::LEN_BUF];

	};

#endif // !defined(AFX_LOG_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_)
