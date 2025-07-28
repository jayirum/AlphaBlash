//+------------------------------------------------------------------+
//|                                                         CLog.mqh |
//|                        Copyright 2019, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+

// logg class


#property copyright "Copyright 2019, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property strict

struct SYSTEMTIME_LOG {
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
    void GetLocalTime(SYSTEMTIME_LOG& lp);
#import

#include "_Platformver.mqh"

enum { INFO=0, ERR, DEBUG };

class CLog
{
public:
    CLog(bool bCommonFolder=false);
    ~CLog();

    bool OpenLog(string sFileName);
    bool Log(int nType, string sData, bool bFlush=false);
    bool log(int nType, string sData, bool bFlush=false) { return Log(nType, sData, bFlush); }
    void CloseLog();
    
private:
    bool    ReOpenLog();
    string  TypeS(int nType);
private:
    int     m_fd;
    bool    m_bCommonFolder;
    string  m_sFileName;
    string  m_sDate;
};

CLog::CLog(bool bCommonFolder)
{
   m_bCommonFolder = bCommonFolder;
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

bool CLog::OpenLog(string sFileName)
{
    m_sFileName = sFileName;
    
    SYSTEMTIME_LOG st;
    GetLocalTime(st);
    m_sDate = StringFormat("%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
    
    string sFullName = m_sFileName+"_"+m_sDate+".log";
    PrintFormat("logname:%s", sFullName);
    
    if(m_bCommonFolder)
    {
       m_fd = FileOpen(sFullName
                       ,FILE_READ | FILE_WRITE | FILE_TXT | FILE_ANSI | FILE_SHARE_READ | FILE_COMMON
                       );
    }
    else
    {
      m_fd = FileOpen(sFullName
                       ,FILE_READ | FILE_WRITE | FILE_TXT | FILE_ANSI | FILE_SHARE_READ
                       );
    }
    if( m_fd==INVALID_HANDLE)
    {
        return false;
    }
    return true;
}

bool CLog::ReOpenLog()
{
    SYSTEMTIME_LOG st;
    GetLocalTime(st);
    string sNow = StringFormat("%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
    
    if( sNow > m_sDate )
    {
		return OpenLog(  m_sFileName );		
	}
	return true;
}


string TimeS(datetime tm)
{
#ifdef __MT5__
   return TimeToString(tm,TIME_DATE|TIME_SECONDS);
#else
    return TimeToStr(tm, TIME_DATE|TIME_SECONDS);
#endif
}

bool CLog::Log(int nType, string sData, bool bFlush=false)
{
    if( !ReOpenLog() )
        return false;

    SYSTEMTIME_LOG st;
    GetLocalTime(st);
    
    string sMsg = StringFormat("[%02d:%02d:%02d:%03d][%s][%s]%s\n"
                                ,st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
                                ,TimeS(TimeCurrent())
                                ,TypeS(nType)
                                ,sData
                                );
    int ret = (int)FileWriteString(m_fd, sMsg);
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
