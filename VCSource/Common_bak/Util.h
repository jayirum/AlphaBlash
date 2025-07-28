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
#include <string>
#include <map>
#include "IRUM_Common.h"
using namespace std;


DWORD	ReportException(DWORD dExitCode, const TCHAR* psPos, _Out_ TCHAR* pzMsgBuff);
DWORD	ReportException(DWORD dExitCode, const char* psPos, _Out_ char* pzMsgBuff);
VOID	DumpErr(TCHAR* pzSrc, int nErr, TCHAR* pzMsg);
TCHAR*	GetCnfgValue(TCHAR* i_psCnfgFileNm, TCHAR* i_psSectionNm, TCHAR* i_psKeyNm, TCHAR* o_psValue);
VOID	getGMTtime(TCHAR* pOut);	//yyyymmdd-hhmmss
char*	U2A(_In_ wchar_t* pUniStr, _Out_ char* pAnsiStr);
int		U2ALen(_In_ wchar_t* pUniStr, _Out_ char* pAnsiStr);
wchar_t* A2U(_In_ char* pAnsiStr, _Out_ wchar_t* pUniStr);


//#define	CNFG_PATH	"D:\\cnfg"
#define property(DATATYPE, READ, WRITE) __declspec(property(get=READ, put=WRITE)) DATATYPE

enum { FULLMODE=0, NORMALMODE, DATEMODE, TIMEMODE, HOURMIN, MILLISECMODE };
enum { EN_SUNDAY=0, EN_MONDAY, EN_TUESDAY, EN_WEDNESDAY, EN_THURSDAY, EN_FRIDAY, EN_SATURDAY};
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
//#define SELFSIZE(F, X) F##(X,sizeof(##X##))
#define MEMCPY(dest, src) memcpy(&dest, src, __min(sizeof(##dest##), sizeof(##src##)))
#define STRMEMCPY(dest, src) memcpy(&dest, src, __min(sizeof(##dest##), _tcslen(src)))
#define MEMSET(dest, fil) memset(&dest, fil, sizeof(##dest##))

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
#define	S2N(src,len)					CUtil::Str2N(src,len)
#define	S2D(src,len)					CUtil::Str2D(src,len)
#define	S2L(src,len)					CUtil::Str2L(src,len)
#define	S2LL(src,len)					CUtil::Str2LL(src,len)
#define N2S(out,in,len)					CUtil::N2Str(out, in,len)
//#define N2SZ(out,in)					CUtil::N2StrNull(out, in)
#define L2S(out,in,len)					CUtil::L2Str(out, in,len)
#define LL2S(out,in,len)				CUtil::LL2Str(out, in,len)
#define D2S(out,in,len,dotcnt)			CUtil::D2Str(out, in,len,dotcnt)
#define	VARIANT2DBL(pvt)				CUtil::Variant2Dbl(pvt)
#define	VARIANT2LONG(pvt)				CUtil::Variant2Long(pvt)
#define SUBSTR(buf,start,len)			CUtil::SubString(buf,start,len)

#define RTRIM(src,len)					CUtil::RTrim(src, len)
#define	TRIMALL(src,len)				CUtil::LTrim(RTrim(src,len))
#define REPLACE(start, len, before, after)	CUtil::Replace(start,len,before,after)
#define TRUNCDBL(src)					CUtil::TruncDbl(src);
#define	TRUNCDBL2(src, point)			CUtil::TruncDbl2(src,point)
#define	ROUND(src, OffSet)				CUtil::Round(src, OffSet)


TCHAR* MakeGUID(TCHAR *pzGUID);
//BOOL IsPassedTime(TCHAR* pzBaseTime, EN_TIMEMODE timeMode);


class CUtil
{
public:
	CUtil();
	virtual ~CUtil();

	//static	TCHAR*	Get_NowTime(BOOL bFull=FALSE);
	static	int		Get_WeekDay();
	static	TCHAR*	GetToday(TCHAR* pzToday);
	static	TCHAR*	GetYesterday(TCHAR* pzYesterday);
	static	bool	Is_PassedDay(TCHAR* pCompared, bool bChange, bool bDot = false);


	static	void	RTrim( TCHAR* pBuf, int nScope );
	static	void	LTrim( TCHAR* pBuf);
	static	void	TrimAll( TCHAR* pBuf, int nScope );
	//static	void	RTrim(char* pBuf, int nScope);
	//static	void	LTrim(char* pBuf);
	//static	void	TrimAll(char* pBuf, int nScope);
	static	TCHAR*	TrimAllEx( TCHAR* pBuf, int nScope );

	static	void	RemoveDot(TCHAR* pData, int nLen);
	static	void	Remove_TCHAR(TCHAR* pData, int nLen, TCHAR i_cTarget);
	static	void	Replace(TCHAR* pSrc, int nChgLen, TCHAR cSrc, TCHAR cChg);
	static	TCHAR* SubString(TCHAR* p, int start, int len);

	static	double	TruncDbl(double dSrc);
	static	double	TruncDbl2(double dSrc, double dPos);
	static	double	Round(double src, int nOffSet);
	static	double	roundoff(double src, int offset);
	//static	long	Variant2Long(VARIANT* pVt);
	
	static	int		Str2N( TCHAR* pszIn, int nLen=0 );
	static	int		Str2L( TCHAR* pszIn, int nLen=0 );
	static	double	Str2D( TCHAR* pszIn, int nLen=0 );
	static	LONGLONG Str2LL( TCHAR* pszIn, int nLen=0 );
	static	TCHAR*	N2Str(  TCHAR* result, int nIn, int nLen, BOOL bNullPointing=FALSE);
	static	TCHAR*	L2Str(  TCHAR* result, LONG lIn, int nLen, BOOL bNullPointing=FALSE);
	static	TCHAR*	D2Str(  TCHAR* result, double dIn, int nLen, int nDot, BOOL bNullPointing=FALSE);
	static	TCHAR*	LL2Str( TCHAR* result, LONGLONG llIn, int nLen, BOOL bNullPointing=FALSE);

	static	int		GetDecimalCnt( TCHAR* psNTTN );	//	소숫점 이하 갯수
	//static	TCHAR*	CvtInt_SpaceLeading( TCHAR* i_psOrg, int i_nOrgLen, int i_nOutLen, TCHAR* o_psOut );
	//static	TCHAR*	CvtDbl_SpaceLeading( TCHAR* i_psOrg, int i_nOrgLen, int i_nOutLen, TCHAR* o_psOut );
	
	static void		GetMyModuleAndDir(TCHAR *o_psDir, TCHAR* o_psModule, TCHAR* o_psConfig);

	static	TCHAR*	GetCnfgFileNm(TCHAR *i_psDir, TCHAR* i_psFileNm, TCHAR* o_psValue );
	static	TCHAR*	GetCnfgFileNmOfSvc(TCHAR* i_psSvcNm, TCHAR* o_psValue);
	static	TCHAR*	GetCnfgXMLFileNm(TCHAR *i_psDir, TCHAR* i_psFileNm, TCHAR* o_psValue);
	static	TCHAR*	GetConfig(TCHAR* i_psCnfgFileNm, TCHAR* i_psSectionNm, TCHAR* i_psKeyNm, TCHAR* o_psValue );
	static  BOOL	GetNextConfigData(TCHAR* pzCnfgFileNm, TCHAR* pzSectionNm, TCHAR* pzPrevKeyNm, TCHAR* o_pzNextKeyNm, TCHAR* o_pzNextValue);
	static	bool	Load_MsgFile(wchar_t* pzMsgFile, map<_tstring, _tstring>& map);
	static	void	RemoveComment(_Out_ TCHAR* pzData);

	//static int		GetTickGap(double dFirstPrc, double dSndPrc, int nDotCnt, double dTickSize);
	//static double	GetPrcByTick(TCHAR* pzOrigPrc, double dTickCnt, double dTickSize, TCHAR cPlusMinus);
	//static int CompPrc(const TCHAR* pPrc1, const int nLen1, const TCHAR* pPrc2, const int nLen2, const int nDotCnt, const int nFormatLen);
	//static int CompPrc(const double pPrc1, const double pPrc2, const int nDotCnt, const int nFormatLen);
	//static BOOL IsSamePrice( TCHAR* pPrc1,  int nLen1,  TCHAR* pPrc2,  int nLen2,  int nDotCnt,  int nFormatLen);

	static VOID FormatErrMsg(_In_ int nErrNo, _Out_ TCHAR* pzMsg);
	static VOID SplitData(_In_ TCHAR* psData, _In_ TCHAR cDelimeter, _Out_ std::list<_tstring>* pListResult);
	static VOID SplitDataEx(_In_ TCHAR* psData, _In_ TCHAR cDelimeter, _In_ int nSize, _Out_ std::list<_tstring>* pListResult);

	//static int GetPassedSeconds(TCHAR* pStartTime, BOOL bColon);

	static void Assert(_In_ wchar_t* wpzMsg);
	static void Assert(_In_ char* pzMsg);
};





