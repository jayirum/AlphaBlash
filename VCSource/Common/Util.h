// CommonUtil.h: interface for the CCommonUtil class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4996)


#include <time.h>
#include <windows.h>
#include <math.h>
#include "Log.h"
#include <functional>
#include <string.h>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <assert.h>
using namespace std;

// ReportException(GetExceptionCode(), "CCreateSaveShm::LoadShmWrapper", m_zMsg))
DWORD	ReportException(DWORD dExitCode, const char* psPos, _Out_ char* pzMsgBuff);
char*	GetCnfgValue(char* i_psCnfgFileNm, char* i_psSectionNm, char* i_psKeyNm, char* o_psValue);
VOID	getGMTtime(char* pOut);	//yyyymmdd-hhmmss

void __CheckBool(bool res, char* msg);
#define CHECK_BOOL(rslt,msg) { if(!rslt) throw msg; }


//#define	CNFG_PATH	"D:\\cnfg"
#define property(DATATYPE, READ, WRITE) __declspec(property(get=READ, put=WRITE)) DATATYPE

enum { FULLMODE=0, NORMALMODE, DATEMODE, TIMEMODE, HOURMIN, MILLISECMODE };
//enum { EN_SUNDAY=0, EN_MONDAY, EN_TUESDAY, EN_WEDNESDAY, EN_THURSDAY, EN_FRIDAY, EN_SATURDAY};
typedef enum {
	TIME_HHMM		//HHMM
	,TIME_HHMMSS	//HHMMSS
	,TIME_HH_MM		//HH:MM
	,TIME_HH_MM_SS	//HH:MM:SS
}EN_TIMEMODE;

#define FMT_GETTIME_DOT_DATEMODE_LEN			10		/*! YYYY.MM.DD */
#define FMT_GETTIME_DOT_TIMEMODE_LEN			8		/*! HH:MM:SS */
#define FMT_GETTIME_DOT_HOURMIN_LEN				5		/*! HH:MM */
#define FMT_GETTIME_DOT_MILLISECMODE_LEN		(FMT_GETTIME_DOT_TIMEMODE_LEN + 4) /*! HH:MM:SS.mmm */
#define FMT_GETTIME_DOT_FULLMODE_LEN			(FMT_GETTIME_DOT_DATEMODE_LEN + FMT_GETTIME_DOT_MILLISECMODE_LEN)		
#define FMT_GETTIME_DOT_NORMALMODE_LEN			(FMT_GETTIME_DOT_DATEMODE_LEN + FMT_GETTIME_DOT_TIMEMODE_LEN)		

#define FMT_GETTIME_NODOT_DATEMODE_LEN			8		/*! YYYYMMDD */
#define FMT_GETTIME_NODOT_TIMEMODE_LEN			6		/*! HHMMSS */
#define FMT_GETTIME_NODOT_HOURMIN_LEN			4		/*! HHMM */
#define FMT_GETTIME_NODOT_MILLISECMODE_LEN		(FMT_GETTIME_NODOT_TIMEMODE_LEN + 3)	/*! HHMMSSmmm */
#define FMT_GETTIME_NODOT_FULLMODE_LEN			(FMT_GETTIME_NODOT_DATEMODE_LEN + FMT_GETTIME_NODOT_MILLISECMODE_LEN)		
#define FMT_GETTIME_NODOT_NORMALMODE_LEN		(FMT_GETTIME_NODOT_DATEMODE_LEN + FMT_GETTIME_NODOT_TIMEMODE_LEN)		
#define SELFSIZE(F, X) F##(X,sizeof(##X##))
#define MEMCPY(dest, src) memcpy(&dest, src, __min(sizeof(##dest##), sizeof(##src##)))
#define STRMEMCPY(dest, src) memcpy(&dest, src, __min(sizeof(##dest##), lstrlen(src)))
#define MEMSET(dest, fil) memset(&dest, fil, sizeof(##dest##))
#define N2S0(out,in,len) {TCHAR c = *(out+len); sprintf_s( out, len+1, "%0*d", len, in); *(out+len) = c; }
#define	FORMAT_PRC(prc,dotcnt,out) { sprintf(out, "%0*.*f", __ALPHA::LEN_PRC, dotcnt, prc); } // 000000012.12
#define LOCK_CS(x) EnterCriticalSection(&##x##);
#define UNLOCK_CS(x) LeaveCriticalSection(&##x##);

#ifndef SAFE_ARR_DELETE
#define SAFE_ARR_DELETE(p)					if (p != NULL){ delete []p; p = NULL;}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)						if (p != NULL){ delete p; p = NULL;}
#endif

#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(p)					if (p != NULL){ CloseHandle(p); p = NULL;}
#endif


#ifndef SAFE_CLOSESOCKET
#define SAFE_CLOSESOCKET(p)					if (p != INVALID_SOCKET){ closesocket(p); p = INVALID_SOCKET;}
#endif

#define MAX_MESSAGE_BUFF	512


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

//#ifndef min
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
//#endif

//////////////////////////////////////////////////////////////////////////
//	COMPONENT 에서 반환할 ㅐ
#define _SetComReturn(pvRet,pvOut,lRet,pMsg)	\
{												\
	VariantCopy(pvRet, &_variant_t(lRet));		\
	VariantCopy(pvOut, &_variant_t(pMsg));		\
}

#define _SetComReturn2(pvRet,pvOut,pvSLen, pvHeader, lRet,pMsg, lSLen, pHeader)	\
{												\
	VariantCopy(pvRet, &_variant_t(lRet));		\
	VariantCopy(pvOut, &_variant_t(pMsg));		\
	VariantCopy(pvSLen, &_variant_t(lSLen));		\
	VariantCopy(pvHeader, &_variant_t(pHeader));		\
}

//====================================
//	형 변환
//====================================

#define	VARIANT2DBL(pvt)				CUtil::Variant2Dbl(pvt)
#define	VARIANT2LONG(pvt)				CUtil::Variant2Long(pvt)
#define TODAY()							CUtil::Get_NowTime(1)
#define NOWTIME()						CUtil::Get_NowTime(0)
#define NOWTIME_SEC(out)				{SYSTEMTIME st; GetLocalTime(&st); sprintf(out,"%02d:%02d:%02d",st.wHour, st.wMinute, st.wSecond); }
#define NOWTIME_MIN(out)				{SYSTEMTIME st; GetLocalTime(&st); sprintf(out,"%02d:%02d",st.wHour, st.wMinute); }
#define NOWTIME_SHORT(out)				{SYSTEMTIME st; GetLocalTime(&st); sprintf(out,"%02d:%02d:%02d",st.wHour, st.wMinute, st.wSecond); }
#define NOWTIME_NONE_DOT(out)			{SYSTEMTIME st; GetLocalTime(&st); sprintf(out,"%02d%02d%02d%03d",st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); }
#define TODAY_NONE_DOT(out)				{SYSTEMTIME st; GetLocalTime(&st); sprintf(out,"%04d%02d%02d",st.wYear, st.wMonth, st.wDay); }

#define TRUNCDBL(src)					CUtil::TruncDbl(src);
#define	TRUNCDBL2(src, point)			CUtil::TruncDbl2(src,point)
#define	ROUND(src, OffSet)				CUtil::Round(src, OffSet)


char* MakeGUID(char *pzGUID);
//TODO BOOL IsPassedTime(char* pzBaseTime, EN_TIMEMODE timeMode);


class CUtil
{
public:
	CUtil();
	virtual ~CUtil();

	static	double	TruncDbl(double dSrc);
	static	double	TruncDbl2(double dSrc, double dPos);
	static	double	Round(double src, int nOffSet);
	static	double	roundoff(double src, int offset);
	//static	long	Variant2Long(VARIANT* pVt);
	
	static void logOutput(char* pMsg, ...);
	
	static void		GetMyModuleAndDir(char *o_psDir, char* o_psModule, char* o_psConfig);
	static	char*	GetCnfgFileNm(char *i_psDir, char* i_psFileNm, char* o_psValue );
	static	TCHAR*	GetCnfgFileNmOfSvc(TCHAR* i_psSvcNm, char* o_pzDir, TCHAR* o_pzCfgFileNm);
	static	char*	GetCnfgXMLFileNm(char *i_psDir, char* i_psFileNm, char* o_psValue);
	static	char*	GetConfig(char* i_psCnfgFileNm, char* i_psSectionNm, char* i_psKeyNm, char* o_psValue );
	static  BOOL SetConfigValue(char* i_psCnfgFileNm, char* i_psSectionNm, char* i_psKeyNm, char* o_psValue);
	static	LPWSTR	GetConfigUni(LPWSTR i_psCnfgFileNm, LPWSTR i_psSectionNm, LPWSTR i_psKeyNm, LPWSTR o_psValue);
	//TODO static  BOOL	GetNextConfigData(char* pzCnfgFileNm, char* pzSectionNm, char* pzPrevKeyNm, char* o_pzNextKeyNm, char* o_pzNextValue);
	static	bool	Load_MsgFile(char* pzMsgFile, map<string, string>& map);

	static int		GetTickGap(double dFirstPrc, double dSndPrc, int nDotCnt, double dTickSize);
	static double	GetPrcByTick(char* pzOrigPrc, double dTickCnt, double dTickSize, char cPlusMinus);
	static int CompPrc(const char* pPrc1, const int nLen1, const char* pPrc2, const int nLen2, const int nDotCnt, const int nFormatLen);
	static int CompPrc(const double pPrc1, const double pPrc2, const int nDotCnt, const int nFormatLen);
	static BOOL IsSamePrice( char* pPrc1,  int nLen1,  char* pPrc2,  int nLen2,  int nDotCnt,  int nFormatLen);

	//static	BOOL	Send2MngMQ(char* pszLabel, char* pszBody);
	//static	BOOL	Send2FrontMQ(char* pszLabel, char* pszBody);

	static VOID FormatErrMsg(_In_ int nErrNo, _Out_ char* pzMsg);
	

	//TODO static int GetPassedSeconds(char* pStartTime, BOOL bColon);

	

};


class smartptrDbl
{
public:
	smartptrDbl(int size) { m_p = new double[size]; ZeroMemory(m_p, size); }
	~smartptrDbl() { if (m_p) delete[] m_p; m_p = NULL; }

	double* get() { return m_p; };

private:
	double* m_p;
};
