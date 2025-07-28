#ifndef _C_TIME_UTILS_H__
#define _C_TIME_UTILS_H__


#include <System.SysUtils.hpp>



class CTimeUtils
{
public:
	CTimeUtils();
	~CTimeUtils();

public:
	String		Today_yyyymmdd();
	AnsiString	Today_yyyymmddA() { return AnsiString(Today_yyyymmdd()); }

	String		Time_hhmmssmmm();
	AnsiString	Time_hhmmssmmmA() { return AnsiString(Time_hhmmssmmm()); }

	String		Time_hhmmss();
	AnsiString	Time_hhmmssA() { return AnsiString(Time_hhmmss()); }

	String      DateTime_yyyymmdd_hhmmssmmm();
	AnsiString  DateTime_yyyymmdd_hhmmssmmmA(){ return AnsiString(DateTime_yyyymmdd_hhmmssmmm()); }

	String      DateTime_yyyymmdd_hhmmss();
	AnsiString  DateTime_yyyymmdd_hhmmssA(){ return AnsiString(DateTime_yyyymmdd_hhmmss()); }

	String      DateTime_yyyy_mm_dd__hh_mm_ss();
	AnsiString  DateTime_yyyy_mm_dd__hh_mm_ssA(){ return AnsiString(DateTime_yyyy_mm_dd__hh_mm_ss()); }
};



#endif