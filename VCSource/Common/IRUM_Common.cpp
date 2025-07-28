#include "IRUM_Common.h" //todo after completion - remove ../NEW/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "TimeUtils.h"


// 00:01:00 ~ 00:01:59 ==> 01분 차트
// date : yyyymmdd, time:hh:mm:ss
// 120분 차트는 6시 부터 시작한다
char* __ComposeChartName(char* date, char* time, int chartTp, int nameTp, char* out)
{
	if (chartTp == TP_1MIN)
		return __ComposeChartName_1min(date, time, nameTp, out);


	int divider, ret;
	char zHour[32], zMin[32], zSec[32];
	char zTm[32];
	sprintf(zTm, "%.8s", time);
	sprintf(zHour, "%.2s", zTm);
	sprintf(zMin, "%.2s", zTm + 3);
	sprintf(zSec, "%.2s", zTm + 6);

	if (chartTp == TP_1MIN) divider = 1;
	else if (chartTp == TP_3MIN) divider = 3;
	else if (chartTp == TP_5MIN) divider = 5;
	else if (chartTp == TP_10MIN) divider = 10;
	else if (chartTp == TP_15MIN) divider = 15;
	else if (chartTp == TP_20MIN) divider = 20;
	else if (chartTp == TP_60MIN) divider = 60;
	else if (chartTp == TP_120MIN) divider = 120;

	// 00:01:00 ~ 00:01:59 ==> 01분 차트
	if (nameTp == CHARTNAME_TP_NEAR)
	{
		ret = (atoi(zMin) / divider);
	}
	// 00:01:01 ~ 00:02:00 ==> 02분 차트
	else
	{
		if (strncmp(time + 6, "00", 2) == 0)
			ret = ((atoi(zMin)) / divider);
		else {
			int nMin = atoi(zMin) + 1;

			// 시간 변경
			if (nMin == 60)
			{
				ret = 0;
				int nHour = atoi(zHour) + 1;
				if (nHour == 24)
					strcpy(zHour, "00");
				else
					sprintf(zHour, "%02d", nHour);
			}
			else
				ret = nMin / divider;
		}
	}
	int min = (ret)*divider;

	if (chartTp == TP_60MIN)
	{
		int h = atoi(zHour);
		if (h == 24)
			h = 0;
		sprintf(out, "%.8s%02d00", date, h);
	}
	else if (chartTp == TP_120MIN) {
		int h = atoi(zHour);
		int hRemain = h % 2;
		if (hRemain == 1)
			h -= 1;
		sprintf(out, "%.8s%02d00", date, h);
	}
	else {
		sprintf(out, "%.8s%.2s%02d", date, zHour, min);
	}

	return out;
}



/*
// date : yyyymmdd
// time : hh:mm:ss 

// 2019.11.08 19시에 수행
[20191108-18:55:00][201911081855]
[20191108-18:55:01][201911081856]
[20191108-23:59:01][201911090000]
[20191108-00:00:00][201911090000]
[20191108-00:00:01][201911090001]

// 2019.11.09 02시에 수행
[20191109-23:58:01][201911082359]
[20191109-23:59:01][201911090000]
[20191109-00:00:00][201911090000]
[20191109-00:00:01][201911090001]

*/
char* __ComposeChartName_1min(char* date, char* time, int nameTp, char* out)
{
	char zHour[32], zMin[32], zSec[32];
	char zTm[32];
	char zNewDate[32];

	sprintf(zTm, "%.8s", time);
	sprintf(zHour, "%.2s", zTm);
	sprintf(zMin, "%.2s", zTm + 3);
	sprintf(zSec, "%.2s", zTm + 6);

	// case 1
	// 00:01:00 ~ 00:01:59 ==> candle is 01 min
	if (nameTp == CHARTNAME_TP_NEAR)
	{
		__GetDate_ByTimeSection(time, zNewDate);
		sprintf(out, "%.8s%.2s%02s", zNewDate, zHour, zMin);
	}
	else if (nameTp == CHARTNAME_TP_FAR)
	{
		// 현재 캔들이 끝나는 시간을 차트시간으로 한다.
		// 10:01:34 ==> 10:02 캔들

		// second가 00 으로 끝나면 +1 하지 않는다.
		if (strncmp(zSec, "00", 2) == 0)
		{
			__GetDate_ByTimeSection(time, zNewDate);
			sprintf(out, "%.8s%.2s%02s", zNewDate, zHour, zMin);
		}
		else
		{
			/*
				23:58:50	==> 59 min candle
				23:59:01	==> +1 min. It may change hour/day/month/year
			*/
			//printf("ComposeChartName_1min-3\n");
			char hhmmss[32];
			sprintf(hhmmss, "%s%s%s", zHour, zMin, zSec);
			CTimeUtils::AddMins(date, hhmmss, 1, zTm);
			sprintf(out, "%.12s", zTm);	// yyyymmddhhmm

			__GetDate_ByTimeSection(zTm + 8, zNewDate, false);
			sprintf(out, "%.8s%.4s", zNewDate, zTm + 8);
		}
	}
	return out;
}


TIME_SECTION __GetTimeSection(const char* psTime)	// hh:mm:ss
{
	TIME_SECTION sec;
	if (strncmp("07:00:00", psTime, 8) <= 0 &&
		strncmp("23:59:59", psTime, 8) >= 0
		)
	{
		sec = SECTION_START;
	}
	else
	{
		sec = SECTION_NEXT;
	}
	return sec;
}

/*
	psCompTime : 날짜가 필요한 시간 

	SYS시간		PACKET시간
	~23:59:59	~23:59:59	==> 같은 날짜
	~23:59:59	00:00:00~	==> 다음 날짜
	00:00:00~	~23:59:59	==> 이전 날짜
*/
char* __GetDate_ByTimeSection(const char* psCompTime, _Out_ char* psDate, bool bWithColon /*= TRUE*/)
{
	SYSTEMTIME st; 
	char sysDate[32], sysTime[32]; 
	char zCompTime[32];
	
	if (bWithColon == FALSE)
	{
		sprintf(zCompTime, "%.2s:%.2s:%.2s", psCompTime, psCompTime + 2, psCompTime + 4);
	}
	else
	{
		sprintf(zCompTime, "%.8s", psCompTime);
	}
	
	GetLocalTime(&st); 

	sprintf(sysDate, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(sysTime, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);

	TIME_SECTION secSys = __GetTimeSection(sysTime);
	TIME_SECTION secComp = __GetTimeSection(zCompTime);

	

	if (secSys == secComp)
	{
		strcpy(psDate, sysDate);
	}
	else if (secSys < secComp)
	{
		// 2019.11.08 23:59:00 vs 2019.11.09 00:00:01
		CTimeUtils::AddDates(sysDate, 1, psDate);
	}
	else if (secSys > secComp)
	{
		// 2019.11.08 00:00:01 vs 2019.11.08 23:59:20
		CTimeUtils::AddDates(sysDate, -1, psDate);
	}
	return psDate;
}



int StrToN(char* pszIn, int nLen)
{
	if (!nLen)	return atoi(pszIn);

	char result[128];
	sprintf(result, "%*.*s", nLen, nLen, pszIn);
	//ReplaceChr(result, result+nLen, 0x20, '0');
	return atoi(result);
}


//
//bool ir_isbrokerKR(char* psBroker)
//{
//	return (strncmp(psBroker, IR_BROKER_KR, 2) == 0);
//}
//
//bool ir_isbrokerHD(char* psBroker)
//{
//	return (strncmp(psBroker, IR_BROKER_HD, 2) == 0);
//}
//
//// CLQ17 -> CLQ7
////#define CVTCODE_17_7(symbol, out){sprintf(out,"%.*s%c", strlen(symbol) - 2, symbol, symbol[strlen(symbol) - 1]);}
//char* ir_cvtcode_HD_KR(char *pzhdCode, char* pzOut)	// CLQ17 -> CLQ7
//{
//	int len = strlen(pzhdCode);
//
//	// CLQ17, CLQ20 => 2nd character from right is 1 or 2
//	if (pzhdCode[len - 2] == '1' || pzhdCode[len - 2] == '2')
//		sprintf(pzOut, "%.*s%c", strlen(pzhdCode) - 2, pzhdCode, pzhdCode[strlen(pzhdCode) - 1]);
//	else
//		strcpy(pzOut, pzhdCode);
//	return pzOut;
//}

// URO -> 6E
char* ir_cvtcode_uro_6e(char* pzIn, char* pzOut)
{
	if (strncmp(pzIn, "URO", 3) == 0) sprintf(pzOut, "6E%s", pzIn + 3);
	else if(strncmp(pzIn,"AD",2)==0) sprintf(pzOut,"6A%s", pzIn +2);
	else if(strncmp(pzIn,"BP",2)==0) sprintf(pzOut,"6B%s", pzIn +2);
	else if(strncmp(pzIn,"JY",2)==0) sprintf(pzOut,"6J%s", pzIn +2);
	else strcpy(pzOut, pzIn);

	return pzOut;
}


// 6E -> URO
char* ir_cvtcode_6e_uro(char* pzIn, char* pzOut)
{
	if (strncmp(pzIn, "6E", 2) == 0) sprintf(pzOut, "URO%s", pzIn + 2);
	else if (strncmp(pzIn, "6A", 2) == 0) sprintf(pzOut, "AD%s", pzIn + 2);
	else if (strncmp(pzIn, "6B", 2) == 0) sprintf(pzOut, "BP%s", pzIn + 2);
	else if (strncmp(pzIn, "6J", 2) == 0) sprintf(pzOut, "JY%s", pzIn + 2);
	else strcpy(pzOut, pzIn);

	return pzOut;
}

int TFutExec_to_SMPACK_FX_EXEC(const void* pIn, void* pOut)
{
	TFutExec2* pOrg = (TFutExec2*)pIn;
	SMPACK_FX_EXEC* pNew = (SMPACK_FX_EXEC*)pOut;

	char s[128], s2[128];
	memset(pNew, 0x20, sizeof(SMPACK_FX_EXEC));
	
	sprintf(s, "%.*s", sizeof(pOrg->issue), pOrg->issue);
	memcpy(pNew->issue, s, min(sizeof(pNew->issue),strlen(s)));

	sprintf(s, "%.*f", sizeof(pNew->gap), pOrg->gap);
	memcpy(pNew->gap, s, min(sizeof(pNew->gap), strlen(s)));

	sprintf(s, "%.*f", sizeof(pNew->cup), pOrg->cup);
	memcpy(pNew->cup, s, min(sizeof(pNew->cup), strlen(s)));

	sprintf(s, "%.*f", sizeof(pNew->sip), pOrg->sip);
	memcpy(pNew->sip, s, min(sizeof(pNew->sip), strlen(s)));
	
	sprintf(s, "%.*f", sizeof(pNew->hip), pOrg->hip);
	memcpy(pNew->hip, s, min(sizeof(pNew->hip), strlen(s)));

	sprintf(s, "%.*f", sizeof(pNew->lip), pOrg->lip);
	memcpy(pNew->lip, s, min(sizeof(pNew->lip), strlen(s)));

	sprintf(s, "%d", pOrg->vol);
	memcpy(pNew->vol, s, min(sizeof(pNew->vol), strlen(s)));
	
	sprintf(s, "%.*f", sizeof(pNew->amt), pOrg->amt);
	memcpy(pNew->amt, s, min(sizeof(pNew->amt), strlen(s)));

	/*
		FBI_FXSiseGWE 에서 ebest 의 한국시각필드(Kortm - hhmmss) 을 integer 로 변환을 한다.
		000101 ==> 101
		001029 ==> 1029
		081245 ==> 81245
		134323 ==> 134323
	*/
	char time[32];
	sprintf(s2, "%d", pOrg->time);
	if (strlen(s2) == 1)		sprintf(time, "00000%s", s2);
	else if (strlen(s2) == 2)		sprintf(time, "0000%s", s2);
	else if (strlen(s2) == 3)		sprintf(time, "000%s", s2);
	else if (strlen(s2) == 4)	sprintf(time, "00%s", s2);
	else if (strlen(s2) == 5)	sprintf(time, "0%s", s2);
	else						sprintf(time, "%s", s2);
	memcpy(pNew->time, time, strlen(time));

	sprintf(s, "%.*s", sizeof(pOrg->side), pOrg->side);
	memcpy(pNew->side, s, min(sizeof(pNew->side), strlen(s)));

	sprintf(s, "%.*s", sizeof(pOrg->ydiffSign), pOrg->ydiffSign);
	memcpy(pNew->ydiffSign, s, min(sizeof(pNew->ydiffSign), strlen(s)));

	sprintf(s, "%.*s", sizeof(pOrg->chgrate), pOrg->chgrate);
	memcpy(pNew->chgrate, s, min(sizeof(pNew->chgrate), strlen(s)));

	return (sizeof(SMPACK_FX_EXEC));
}




//char* pack_header_fields(char* pData, char* pzName, char* pzOut)
//{
//	*pzOut = 0x00;
//	PACK_HEADER_S2C* h = (PACK_HEADER_S2C*)pData;
//	char zCode[32];
//	sprintf(zCode, "%.*s", LEN_PACKET_CODE, h->Code);
//
//	if (!strcmp(pzName, "STRAT_ID"))
//	{
//		if (!strcmp(zCode, CD_SIG_OPENPRC))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_OPENPRC*)pData)->StratID), ((PACK_STRAT_FB_OPENPRC*)pData)->StratID);
//		if (!strcmp(zCode, CD_SIG_ASSIST))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_ASSIST*)pData)->StratID), ((PACK_STRAT_FB_ASSIST*)pData)->StratID);
//		if (!strcmp(zCode, CD_SIG_MACROSS))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_CROSSMA*)pData)->StratID), ((PACK_STRAT_FB_CROSSMA*)pData)->StratID);
//		if (!strcmp(zCode, CD_SIG_SIDEWAY))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_SIDEWAY*)pData)->StratID), ((PACK_STRAT_FB_SIDEWAY*)pData)->StratID);
//	}
//	if (!strcmp(pzName, "CHART_TP"))
//	{
//		if (!strcmp(zCode, CD_SIG_OPENPRC))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_OPENPRC*)pData)->ChartTp), ((PACK_STRAT_FB_OPENPRC*)pData)->ChartTp);
//		if (!strcmp(zCode, CD_SIG_ASSIST))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_ASSIST*)pData)->ChartTp), ((PACK_STRAT_FB_ASSIST*)pData)->ChartTp);
//		if (!strcmp(zCode, CD_SIG_MACROSS))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_CROSSMA*)pData)->ChartTp), ((PACK_STRAT_FB_CROSSMA*)pData)->ChartTp);
//		if (!strcmp(zCode, CD_SIG_SIDEWAY))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_SIDEWAY*)pData)->ChartTp), ((PACK_STRAT_FB_SIDEWAY*)pData)->ChartTp);
//	}
//	if (!strcmp(pzName, "CHART_NM"))
//	{
//		if (!strcmp(zCode, CD_SIG_OPENPRC))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_OPENPRC*)pData)->ChartNm), ((PACK_STRAT_FB_OPENPRC*)pData)->ChartNm);
//		if (!strcmp(zCode, CD_SIG_ASSIST))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_ASSIST*)pData)->ChartNm), ((PACK_STRAT_FB_ASSIST*)pData)->ChartNm);
//		if (!strcmp(zCode, CD_SIG_MACROSS))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_CROSSMA*)pData)->ChartNm), ((PACK_STRAT_FB_CROSSMA*)pData)->ChartNm);
//		if (!strcmp(zCode, CD_SIG_SIDEWAY))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_SIDEWAY*)pData)->ChartNm), ((PACK_STRAT_FB_SIDEWAY*)pData)->ChartNm);
//	}
//	if (!strcmp(pzName, "STRAT_PRC"))
//	{
//		if (!strcmp(zCode, CD_SIG_OPENPRC))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_OPENPRC*)pData)->StratPrc), ((PACK_STRAT_FB_OPENPRC*)pData)->StratPrc);
//		if (!strcmp(zCode, CD_SIG_ASSIST))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_ASSIST*)pData)->StratPrc), ((PACK_STRAT_FB_ASSIST*)pData)->StratPrc);
//		if (!strcmp(zCode, CD_SIG_MACROSS))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_CROSSMA*)pData)->StratPrc), ((PACK_STRAT_FB_CROSSMA*)pData)->StratPrc);
//		if (!strcmp(zCode, CD_SIG_SIDEWAY))
//			sprintf(pzOut, "%.*s", sizeof(((PACK_STRAT_FB_SIDEWAY*)pData)->StratPrc), ((PACK_STRAT_FB_SIDEWAY*)pData)->StratPrc);
//	}
//	return pzOut;
//}