#include <Windows.h>
#include "TimeUtils.h"

CTimeUtils::CTimeUtils()
{}

CTimeUtils::~CTimeUtils()
{


}

char* CTimeUtils::DateTime_yyyymmdd_hh_mm_ss_mmm()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%04d%02d%02d_%02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return m_z;
}

char* CTimeUtils::Today_yyyymmdd()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%04d%02d%02d",st.wYear, st.wMonth, st.wDay);
	return m_z;
}

char* CTimeUtils::Time_hhmmssmmm()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%02d%02d%02d%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return m_z;
}

char* CTimeUtils::Time_hh_mm_ss_mmm()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%02d:%02d:%02d:%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return m_z;
}

char* CTimeUtils::DateTime_yyyymmdd_hhmmssmmm()
{
	SYSTEMTIME st; GetLocalTime(&st);
	sprintf(m_z, "%04d%02d%02d_%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return m_z;
}

int CTimeUtils::Get_WeekDay()
{	struct tm today;
	time_t now = time(0);
	localtime_s(&today, &now);
	return today.tm_wday;
}


// //yyyymmdd-hhmmss
char* CTimeUtils::GmtDateTime_yyyymmdd_hhmmssmmm()
{
	time_t rawtime;
	time(&rawtime);
	struct tm* ltime = gmtime(&rawtime);

	sprintf(m_z, "%04d%02d%02d_%02d%02d%02d",
		1900 + ltime->tm_year, ltime->tm_mon + 1, ltime->tm_mday, ltime->tm_hour, ltime->tm_min, ltime->tm_sec);

	return m_z;
}


char* CTimeUtils::Get_WeekDayS()
{
	int nDay = Get_WeekDay();
	switch (nDay)
	{
	case EN_SUN:strcpy(m_z, "SUNDAY"); break;
	case EN_MON:strcpy(m_z, "MONDAY"); break;
	case EN_TUE:strcpy(m_z, "TUESDAY"); break;
	case EN_WED:strcpy(m_z, "WEDNESDAY"); break;
	case EN_THUR:strcpy(m_z, "THURSDAY"); break;
	case EN_FRI:strcpy(m_z, "FRIDAY"); break;
	case EN_SAT:strcpy(m_z, "SATURDAY"); break;
	}
	return m_z;
}

bool CTimeUtils::Is_Weekend()
{
	struct tm today;
	time_t now = time(0);
	localtime_s(&today, &now);
	bool ret = false;
	if (today.tm_wday == EN_SUN || today.tm_wday == EN_SAT)
		ret = true;
	return ret;
}

char* CTimeUtils::GetYesterday()
{
	time_t timer;
	struct tm* t;


	timer = time(NULL) - (24 * 60 * 60);
	t = localtime(&timer);

	sprintf(m_z, "%04d%02d%02d",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);

	return m_z;
}


bool CTimeUtils::Is_DayPassed(char* pCompared, bool bChange, bool bDot)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	TCHAR szToday[11];

	int nComp = 0;
	if (bDot) {
		sprintf_s(szToday, "%04d.%02d.%02d", st.wYear, st.wMonth, st.wDay);
		nComp = strncmp(szToday, pCompared, 10);
	}
	else {
		sprintf_s(szToday, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
		nComp = strncmp(szToday, pCompared, 8);
	}

	if (nComp > 0)
	{
		if (bChange)
			sprintf_s(pCompared, lstrlen(szToday) + 1, "%.*s", lstrlen(szToday), szToday);
	}

	return (nComp > 0);
}


void CTimeUtils::AddSeconds(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addSec, _Out_ char* yyyymmddhhmmss)
{
	AddTime('s', yyyymmdd, hhmmss, addSec, yyyymmddhhmmss);
}

void CTimeUtils::AddMins(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addMin, _Out_ char* yyyymmddhhmmss)
{
	AddTime('m', yyyymmdd, hhmmss, addMin, yyyymmddhhmmss);
}

void CTimeUtils::AddDates(_In_ char* yyyymmdd, _In_ int addDates, _Out_ char* o_yyyymmdd)
{
	// typedef __int64. get now time
	time_t now = time(&now);

	// breadown the current time into tm struct.
	struct tm * tmLocal = localtime(&now);

	char y[32], m[32], d[32];
	sprintf(y, "%.4s", yyyymmdd);
	sprintf(m, "%.2s", yyyymmdd+4);
	sprintf(d, "%.2s", yyyymmdd+6);

	tmLocal->tm_year = atoi(y) - 1900;
	tmLocal->tm_mon = atoi(m) - 1;
	tmLocal->tm_mday = atoi(d);

	tmLocal->tm_mday += addDates;
	
	
	// make new time_t data using modified tm struct
	time_t newTimeT = mktime(tmLocal);
	struct tm* newTm = localtime(&newTimeT);

	sprintf(o_yyyymmdd, "%04d%02d%02d",
		newTm->tm_year + 1900,
		newTm->tm_mon + 1,
		newTm->tm_mday
	);
}

void CTimeUtils::AddTime(_In_ char timeFrame, _In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addTime, _Out_ char* yyyymmddhhmmss)
{
	// typedef __int64. get now time
	time_t now = time(&now);

	// breadown the current time into tm struct.
	struct tm * tmLocal = localtime(&now);

	char y[32], m[32], d[32], h[32], min[32], s[32];
	sprintf(y, "%.4s", yyyymmdd);
	sprintf(m, "%.2s", yyyymmdd+4);
	sprintf(d, "%.2s", yyyymmdd+6);

	sprintf(h, "%.2s", hhmmss);
	sprintf(min, "%.2s", hhmmss+2);
	sprintf(s, "%.2s", hhmmss+4);

	tmLocal->tm_year = atoi(y) - 1900;
	tmLocal->tm_mon = atoi(m) - 1;
	tmLocal->tm_mday = atoi(d);
	tmLocal->tm_hour = atoi(h);
	tmLocal->tm_min = atoi(min);
	tmLocal->tm_sec = atoi(s);

	switch (timeFrame)
	{
	case 'Y':	tmLocal->tm_year += addTime;	break;
	case 'M':	tmLocal->tm_mon += addTime;		break;
	case 'D':	tmLocal->tm_mday += addTime;	break;
	case 'h':	tmLocal->tm_hour += addTime;	break;
	case 'm':	tmLocal->tm_min += addTime;	break;
	case 's':	tmLocal->tm_sec += addTime;	break;
	}
	
	// make new time_t data using modified tm struct
	time_t newTimeT = mktime(tmLocal);
	struct tm* newTm = localtime(&newTimeT);

	sprintf(yyyymmddhhmmss, "%04d%02d%02d%02d%02d%02d",
		newTm->tm_year + 1900,
		newTm->tm_mon + 1,
		newTm->tm_mday,
		newTm->tm_hour,
		newTm->tm_min,
		newTm->tm_sec
	);
}