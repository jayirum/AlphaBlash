#pragma once
#pragma warning(disable:4996)


#include <stdlib.h>  
#include <stdio.h>
#include <time.h>
#include <string>
using namespace std;

class CTimeUtils
{
public:
	CTimeUtils();
	~CTimeUtils();

	void AddSeconds(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addSec, _Out_ char* yyyymmddhhmmss);
	void AddMins(_In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addMin, _Out_ char* yyyymmddhhmmss);
	void AddDates(_In_ char* yyyymmdd,  int addDates, _Out_ char* o_yyyymmdd);
	
	char* LocalTime_Full_WithDot(_Out_ char* pzDate);

	char* Today_yyyymmdd(_Out_ char* pzDate);
	char* Time_hhmmssmmm(_Out_ char* pzTime);
	char* Time_hh_mm_ss_mmm(_Out_ char* pzTime);
	char* DateTime_yyyymmdd_hhmmssmmm(_Out_ char* pzTime);
	char* DateTime_yyyymmdd_hh_mm_ss_mmm(_Out_ char* pzTime);

private:
	void AddTime(_In_ char timeFrame, _In_ char* yyyymmdd, _In_ char* hhmmss, _In_ int addTime, _Out_ char* yyyymmddhhmmss);
};
