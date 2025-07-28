//+------------------------------------------------------------------+
//|                                                         CLog.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+

// logg class


#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict

struct SYSTEMTIME {
    ushort wYear;
    ushort wMonth;
    ushort wDayOfWeek;
    ushort wDay;
    ushort wHour;
    ushort wMinute;
    ushort wSecond;
    ushort wMilliseconds;
} ;

#import "Kernel32.dll"
    void GetLocalTime(SYSTEMTIME& lp);
#import

#include "Utils.mqh"

enum { INFO=0, ERR, DEBUG };

class CLog
{
public:
    CLog();
    ~CLog();

    bool OpenLog(string sPath, string sFileName);
    bool Log(int nType, string sData, bool bFlush=false);
    void CloseLog();
    
private:
    bool    ReOpenLog();
    string  TypeS(int nType);
private:
    int     m_fd;
    string  m_sPath;
    string  m_sFileName;
    string  m_sDate;
};

CLog::CLog()
{
    m_fd    = INVALID_HANDLE;
    m_sDate = "";
}


CLog::~CLog()
{
    CloseLog();
}


void CLog::CloseLog()
{
    if(m_fd)
    {
        FileFlush(m_fd);
        FileClose(m_fd);
        m_fd = INVALID_HANDLE;
    }
}

bool CLog::OpenLog(string sPath, string sFileName)
{
    m_sPath     = sPath;
    m_sFileName = sFileName;
    
    SYSTEMTIME st;
    GetLocalTime(st);
    m_sDate = StringFormat("%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
    
    m_fd = FileOpen(m_sPath+m_sFileName+m_sDate+".log"
                    ,FILE_READ | FILE_WRITE | FILE_TXT | FILE_ANSI // |FILE_SHARE_READ shared reading from several programs
                    );
    if( m_fd==INVALID_HANDLE)
    {
        return false;
    }
    return true;
}

bool CLog::ReOpenLog()
{
    SYSTEMTIME st;
    GetLocalTime(st);
    string sNow = StringFormat("%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
    
    if( sNow > m_sDate )
    {
		return OpenLog( m_sPath, m_sFileName );		
	}
	return true;
}

bool CLog::Log(int nType, string sData, bool bFlush=false)
{
    if( !ReOpenLog() )
        return false;

    SYSTEMTIME st;
    GetLocalTime(st);
    
    string sMsg = StringFormat("[%02d:%02d:%02d:%03d][%s][%s]%s\n"
                                ,st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
                                ,TimeS(TimeCurrent())
                                ,TypeS(nType)
                                ,sData
                                );
    int ret = FileWriteString(m_fd, sMsg);
    if(ret>0 && bFlush )
        FileFlush(m_fd);
    return (ret>0);
}


string CLog::TypeS(int nType)
{
    string sType;
    switch(nType)
    {
    case INFO:  sType="I"; break;
    case ERR:   sType="E"; break;
    case DEBUG: sType="D"; break;
    default:    sType="I"; break;
    }
    return sType;
}
