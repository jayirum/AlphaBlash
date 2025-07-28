
//#include <afx.h>
#include <stdio.h>
#include "Util.h"
#include "Prop.h"
#include <locale>

#pragma warning(disable : 4530)


//3F2504E0-4F89-11D3-9A0C0305E82C3301
TCHAR* MakeGUID(TCHAR *pzGUID)
{
	_GUID TestGUID;

	// CoCreateGuid 생성하기
	CoCreateGuid(&TestGUID);

	// 생성한 GUID를 829C1584-C57B-4dac-BCE7-6F33455F747A 와 같은 포멧으로 변환.
	_stprintf(pzGUID, TEXT("%.8X-%.4X-%.4X-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X"),
		TestGUID.Data1, TestGUID.Data2, TestGUID.Data3, TestGUID.Data4[0],
		TestGUID.Data4[1], TestGUID.Data4[2], TestGUID.Data4[3], TestGUID.Data4[4],
		TestGUID.Data4[5], TestGUID.Data4[6], TestGUID.Data4[7]
	);

	return pzGUID;
}

DWORD	ReportException(DWORD dExitCode, const TCHAR* psPos, _Out_ TCHAR* pzMsgBuff) // 20120510
{
	switch (dExitCode)
	{
	case EXCEPTION_ACCESS_VIOLATION: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_ACCESS_VIOLATION"));		break;
	case EXCEPTION_BREAKPOINT: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_BREAKPOINT"));			break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_DATATYPE_MISALIGNMENT")); break;
	case EXCEPTION_SINGLE_STEP: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_SINGLE_STEP"));			break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_ARRAY_BOUNDS_EXCEEDED")); break;
	case EXCEPTION_FLT_DENORMAL_OPERAND: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_FLT_DENORMAL_OPERAND"));	break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_FLT_DIVIDE_BY_ZERO"));	break;
	case EXCEPTION_FLT_INEXACT_RESULT: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_FLT_INEXACT_RESULT"));	break;
	case EXCEPTION_FLT_INVALID_OPERATION: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_FLT_INVALID_OPERATION")); break;
	case EXCEPTION_FLT_OVERFLOW: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_FLT_OVERFLOW"));			break;
	case EXCEPTION_FLT_STACK_CHECK: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_FLT_STACK_CHECK"));		break;
	case EXCEPTION_FLT_UNDERFLOW: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_FLT_UNDERFLOW"));		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_INT_DIVIDE_BY_ZERO"));	break;
	case EXCEPTION_INT_OVERFLOW: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_INT_OVERFLOW"));			break;
	case EXCEPTION_PRIV_INSTRUCTION: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_PRIV_INSTRUCTION"));		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: _tcscpy(pzMsgBuff, TEXT("EXCEPTION_NONCONTINUABLE_EXCEPTION")); break;
	default:_stprintf(pzMsgBuff, TEXT("[except code:%d]undefined error"), dExitCode); break;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}


DWORD	ReportException(DWORD dExitCode, const char* psPos, _Out_ char* pzMsgBuff) // 20120510
{
	switch (dExitCode)
	{
	case EXCEPTION_ACCESS_VIOLATION: strcpy(pzMsgBuff, "EXCEPTION_ACCESS_VIOLATION");		break;
	case EXCEPTION_BREAKPOINT: strcpy(pzMsgBuff, "EXCEPTION_BREAKPOINT");			break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: strcpy(pzMsgBuff, "EXCEPTION_DATATYPE_MISALIGNMENT"); break;
	case EXCEPTION_SINGLE_STEP: strcpy(pzMsgBuff, "EXCEPTION_SINGLE_STEP");			break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: strcpy(pzMsgBuff, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"); break;
	case EXCEPTION_FLT_DENORMAL_OPERAND: strcpy(pzMsgBuff, "EXCEPTION_FLT_DENORMAL_OPERAND");	break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: strcpy(pzMsgBuff, "EXCEPTION_FLT_DIVIDE_BY_ZERO");	break;
	case EXCEPTION_FLT_INEXACT_RESULT: strcpy(pzMsgBuff, "EXCEPTION_FLT_INEXACT_RESULT");	break;
	case EXCEPTION_FLT_INVALID_OPERATION: strcpy(pzMsgBuff, "EXCEPTION_FLT_INVALID_OPERATION"); break;
	case EXCEPTION_FLT_OVERFLOW: strcpy(pzMsgBuff, "EXCEPTION_FLT_OVERFLOW");			break;
	case EXCEPTION_FLT_STACK_CHECK: strcpy(pzMsgBuff, "EXCEPTION_FLT_STACK_CHECK");		break;
	case EXCEPTION_FLT_UNDERFLOW: strcpy(pzMsgBuff, "EXCEPTION_FLT_UNDERFLOW");		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO: strcpy(pzMsgBuff, "EXCEPTION_INT_DIVIDE_BY_ZERO");	break;
	case EXCEPTION_INT_OVERFLOW: strcpy(pzMsgBuff, "EXCEPTION_INT_OVERFLOW");			break;
	case EXCEPTION_PRIV_INSTRUCTION: strcpy(pzMsgBuff, "EXCEPTION_PRIV_INSTRUCTION");		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: strcpy(pzMsgBuff, "EXCEPTION_NONCONTINUABLE_EXCEPTION"); break;
	default:sprintf(pzMsgBuff, "[except code:%d]undefined error", dExitCode); break;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}



CUtil::CUtil(){}
CUtil::~CUtil(){}




/**	\brief	The CUtil::Get_WeekDay function


	\return	int

	* 오늘의 요일을 가져온다.
	- enum { SUNDAY=0, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY };	
*/

int CUtil::Get_WeekDay()
{
	time_t ltime;
    struct tm *today = NULL;
	time( &ltime );
	localtime_s(today, &ltime );
	return today->tm_wday;
}

// 오늘일자반환
TCHAR* CUtil::GetToday(TCHAR* pzToday)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	_stprintf(pzToday, TEXT("%04d%02d%02d"), st.wYear, st.wMonth, st.wDay);
	return pzToday;
}


// 어제일자 반환
TCHAR* CUtil::GetYesterday(TCHAR* pzYesterday)
{
	time_t timer;
	struct tm *t;


	timer = time(NULL) - (24 * 60 * 60);
	t = localtime(&timer);

	_stprintf(pzYesterday, TEXT("%04d%02d%02d"),
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);

	return pzYesterday;
}


void CUtil::RTrim( TCHAR* pBuf, int nScope )
{
	if (_tcslen(pBuf) == 0)
		return;

	for( int i=nScope-1; i>=0; i--)
	{
		if( *( pBuf+i ) == 0x20 ||
			*( pBuf+i ) == '\t'	||
			*( pBuf+i ) == '\r'	||
			*( pBuf+i ) == '\n'	
			)
		{
			*( pBuf+i ) = 0x00;
		}
		else
		{
			break;
		}
	}
}

void CUtil::LTrim( TCHAR* pBuf)
{
	int nLen = _tcslen(pBuf);
	if (nLen == 0)
		return;

	TCHAR *pTmp = new TCHAR[nLen+1];
	lstrcpy(pTmp, pBuf);

	int nPos = 0;
	for(int i=0; i<nLen; i++){
		if( *(pTmp+nPos)==0x20 ||
			*(pTmp+nPos)=='\n' ||
			*(pTmp+nPos)=='\r' ||
			*(pTmp+nPos)=='\t' 
			)
		{
			nPos++;
		}
		else
		{
			break;
		}
	}

	lstrcpy(pBuf, pTmp+nPos);
	delete[] pTmp;

}

TCHAR* CUtil::TrimAllEx( TCHAR* pBuf, int nScope )
{
	static TCHAR b_szRESULT_BUFFER[256] = {0,};
	_stprintf( b_szRESULT_BUFFER, TEXT("%.*s"), nScope, pBuf );
	CUtil::RTrim(b_szRESULT_BUFFER, nScope );
	CUtil::LTrim(b_szRESULT_BUFFER);
	return b_szRESULT_BUFFER;
}

void CUtil::TrimAll( TCHAR* pBuf, int nScope )
{
	CUtil::RTrim(pBuf,nScope);
	CUtil::LTrim(pBuf);
}


void CUtil::TrimAll(char* pBuf, int nScope)
{
	CUtil::RTrim(pBuf, nScope);
	CUtil::LTrim(pBuf);
}

void CUtil::RTrim(char* pBuf, int nScope)
{
	if (strlen(pBuf) == 0)
		return;

	for (int i = nScope - 1; i >= 0; i--)
	{
		if (*(pBuf + i) == 0x20 ||
			*(pBuf + i) == '\t' ||
			*(pBuf + i) == '\r' ||
			*(pBuf + i) == '\n'
			)
		{
			*(pBuf + i) = 0x00;
		}
		else
		{
			break;
		}
	}
}

void CUtil::LTrim(char* pBuf)
{
	int nLen = strlen(pBuf);
	if (nLen == 0)
		return;

	char* pTmp = new char[nLen + 1];
	strcpy(pTmp, pBuf);

	int nPos = 0;
	for (int i = 0; i < nLen; i++) {
		if (*(pTmp + nPos) == 0x20 ||
			*(pTmp + nPos) == '\n' ||
			*(pTmp + nPos) == '\r' ||
			*(pTmp + nPos) == '\t'
			)
		{
			nPos++;
		}
		else
		{
			break;
		}
	}

	strcpy(pBuf, pTmp + nPos);
	delete[] pTmp;

}


/**	\brief	The CUtil::RemoveDot function

	\param	pData	a parameter of type TCHAR *
	\param	nLen	a parameter of type int

	\return	void

	패킷에 있는 '.' 을 제거한다.
*/

void CUtil::RemoveDot(TCHAR *pData, int nLen)
{
	int nChangedLen = nLen;
	for(int i=0; i<nLen; i++)
	{
		if(*(pData+i)=='.')
		{
			memmove(pData+i, pData+i+1, nLen-i-2);
			nChangedLen--;
		}
	}
	*(pData+nChangedLen-1) = 0x00;
}

/**	\brief	The CUtil::RemoveDot function

	\param	pData	a parameter of type TCHAR *
	\param	nLen	a parameter of type int

	\return	void

	패킷에 있는 특정 TCHAR ('-','/' 등등) 을 제거한다.
*/

void CUtil::Remove_TCHAR(TCHAR* pData, int nLen, TCHAR i_cTarget)
{
	int nChangedLen = nLen;
	for(int i=0; i<nLen; i++)
	{
		if(*(pData+i)==i_cTarget)
		{
			memmove(pData+i, pData+i+1, nLen-i-2);
			nChangedLen--;
		}
	}
	*(pData+nChangedLen-1) = 0x00;
}



/**	\brief	The CUtil::Replace function

	\param	pSrc	a parameter of type TCHAR*
	\param	nChgLen	a parameter of type int
	\param	cSrc	a parameter of type TCHAR
	\param	cChg	a parameter of type TCHAR

	\return	void

	* pSrc 에 포함된 cSrc 를 cChg 로 변경한다.
*/

void CUtil::Replace(TCHAR* pSrc, int nChgLen, TCHAR cSrc, TCHAR cChg)
{
	for(int i=0; i<nChgLen;i++)
	{
		if(*(pSrc+i) == cSrc)
			*(pSrc+i) = cChg;
	}
}


/**	\brief	The CUtil::Is_PassedDay function

	\param	pCompared	a parameter of type TCHAR*
	\param	bChange	a parameter of type bool
	\param	bDot	a parameter of type bool

	\return	bool

	일자가 지났는지 판단한다.
*/

bool CUtil::Is_PassedDay(TCHAR* pCompared, bool bChange, bool bDot)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	TCHAR szToday[11];

	int nComp = 0;
	if(bDot){
		_stprintf(szToday, TEXT("%04d.%02d.%02d"), st.wYear, st.wMonth, st.wDay);
		nComp = _tcsncmp(szToday, pCompared, 10);
	}
	else{
		_stprintf(szToday, TEXT("%04d%02d%02d"), st.wYear, st.wMonth, st.wDay);
		nComp = _tcsncmp(szToday, pCompared, 8);
	}

	if(nComp>0)
	{
		if(bChange)
			_stprintf(pCompared, TEXT("%.*s"), _tcslen(szToday), szToday);
	}

	return (nComp>0);
}


/**	\brief	double 형 데이터의 소숫점 이하를 떼어내고 정수부분만 리턴한다.

  \param	dSrc	a parameter of type double
  
	\return	static inline double
*/
double CUtil::TruncDbl(double dSrc)
{
	double dInt = 0;
	modf(dSrc, &dInt);
	return dInt;
}


/**	\brief	double 형 데이터의 일정 자릿수 이하의 소숫점 버린다.

  \param	dSrc	변환하고자 하는 수
  \param	dPos	자릿수 위치
  
	\return	static inline double
	
	  ex) 123.123 을 소수점 2자리 이하는 버린다.
	  => TruncDbl2(123.123, 2)
*/
double CUtil::TruncDbl2(double dSrc, double dPos)
{
	double dInt = 0;
	double dMultiple = pow(10, dPos);	//	100
	double dSrcCopy = dSrc;				//	123.123
	
	dSrcCopy = dSrcCopy * dMultiple;	//	dSrcCopy = 12312.3
	modf(dSrcCopy, &dInt);				//	dInt = 12312
	
	double dResult = dInt / dMultiple;	//	dResult = 12312 / 100 = 123.12
	return dResult;
}


/**	\brief	double 형 데이터의 소수점 이상의 일정 자리는 버린다.

  \param	src			변환하고자 하는 수
  \param	nOffSet		버리고자 하는 자릿수
  
	\return	static inline double
	
	  ex) 12345 ==> 10000
	  Round(12345, 4)
	  
*/
double CUtil::Round(double src, int nOffSet)
{
	double dMultiple = pow((double)10, (double)nOffSet);	//	10000
	
	//	floor : 해당 수 이하 최대 정수 ex) 1.2345 => 1
	double dRet = floor( src / dMultiple );	//	dRet = 1
	dRet *= dMultiple;						//	dRet = 1 * 10000 = 10000
	return dRet;
}


/*
	반올림.

	56.349  => 소수세자리에서 자르기 : 56.34

	roundoff(56.349, 2)
*/
double CUtil::roundoff(double src, int offset)
{
	//double dMultiple = pow((double)10, (double)offset);	//	100

	//int nRet = (int)((src * dMultiple)+0.5);	//	nRet = 56.349*100 + 0.5 = 5634.9 + 0.5 = 5635.4 => 5635
	//double dRet = (double)(nRet / dMultiple);	//	dRet = 5635 / 100 = 56.35

	double dMultiple = pow((double)10, (double)offset);
	double dRet = floor(src * dMultiple + 0.5) / dMultiple;
	return dRet;
}


// VOID CUtil::CopyRAlign(TCHAR* pDest, const TCHAR *pSrc, long destSize, long srcSize,TCHAR cFiller)
// {
// 	//	Filler 로 초기화
// 	FillMemory(pDest, destSize, cFiller);
// 	
// 	//	할당한 메모리 block 보다 copy할 block 이 더 크면 
// 	//	할당된 메모리 block 크기로 맞춘다.
// 	if(srcSize>destSize)
// 		srcSize = destSize;
// 	long lPos = destSize - srcSize;
// 	CopyMemory( pDest+lPos, pSrc, srcSize );
// }


/**	\brief	TCHAR string 을 원하는 만큼 자른다.

  \param	p		src 데이터
  \param	start	시작점 
  \param	len		자르는 길이
  
	\return	static inline TCHAR*
*/
TCHAR* CUtil::SubString(TCHAR* p, int start, int len)
{
	static TCHAR result[128];
	_stprintf(result, TEXT("%.*s"), len, p+start);
	return result;
}


/**	\brief	VARIANT TYPE 을 double 로 변환

  \param	pVt	a parameter of type VARIANT*
  
	\return	static inline double
*/
// double CUtil::Variant2Dbl(VARIANT* pVt)
// {
// 	HRESULT hr;
// 	hr = VariantChangeType(pVt, pVt, 0, VT_R8);
// 	return pVt->dblVal;
// }

/**	\brief	VARIANT TYPE 을 long 로 변환

  \param	pVt	a parameter of type VARIANT*
  
	\return	static inline double
*/
//long CUtil::Variant2Long(VARIANT* pVt)
//{
//	HRESULT hr;
//	hr = VariantChangeType(pVt, pVt, 0,  VT_I4);
//	return pVt->lVal;
//}


	//////////////////////////////////////////////////////////////////////////
	//	string 을 여러 숫자 type 으로 변환
	//////////////////////////////////////////////////////////////////////////

/**	\brief	TCHAR string 을 int 형으로 변환

	\param	pszIn	a parameter of type TCHAR*
	\param	nLen	a parameter of type int

	\return	static inline int
*/
int CUtil::Str2N( TCHAR* pszIn, int nLen )
{
	if(!nLen)	return _ttoi(pszIn);

	TCHAR result[128];
	_stprintf(result, TEXT("%*.*s"), nLen, nLen, pszIn);
	//ReplaceChr(result, result+nLen, 0x20, '0');
	return _ttoi(result);
}

/**	\brief	TCHAR string 을 long 형으로 변환

	\param	pszIn	a parameter of type TCHAR*
	\param	nLen	a parameter of type int

	\return	static inline int
*/
int CUtil::Str2L( TCHAR* pszIn, int nLen )
{
	if(!nLen)	return _ttoi(pszIn);

	TCHAR result[128];
	_stprintf(result, TEXT("%*.*s"), nLen, nLen, pszIn);
	//ReplaceChr(result, result+nLen, 0x20, '0');
	return _ttol(result);
}

/**	\brief	TCHAR string 을 double 형으로 변환

	\param	pszIn	a parameter of type TCHAR*
	\param	nLen	a parameter of type int

	\return	static inline int
*/
double CUtil::Str2D( TCHAR* pszIn, int nLen )
{
	TCHAR* stopstring;
	if(!nLen)	return _tcstod(pszIn, &stopstring);

	TCHAR result[128];
	_stprintf(result, TEXT("%*.*s"), nLen, nLen, pszIn);
	//ReplaceChr(result, result+nLen, 0x20, '0');
	return _tcstod(result, &stopstring);
}

/**	\brief	TCHAR string 을 LONGLONG 형으로 변환

	\param	pszIn	a parameter of type TCHAR*
	\param	nLen	a parameter of type int

	\return	static inline int
*/
LONGLONG CUtil::Str2LL( TCHAR* pszIn, int nLen )
{
	
	if(!nLen)	return _ttoi64(pszIn);

	TCHAR result[128];
	_stprintf(result, TEXT("%*.*s"), nLen, nLen, pszIn);
	//ReplaceChr(result, result+nLen, 0x20, '0');
	return _ttoi64(result);
}

//	TCHAR* sprintf_s_d( TCHAR* pszIn, TCHAR* pOut, int nLen )
//	{
//		int n = StrToN(pIn, nResultBufLen);
//		NToStr(n,nResultBufLen,true,pOut);
//		return pOut;
//	}


	//////////////////////////////////////////////////////////////////////////
	//	여러 숫자 type 을 str 으로 변환
	//////////////////////////////////////////////////////////////////////////

/**	\brief	int 형을 TCHAR string 형으로 변환

	\param	result	a parameter of type TCHAR*
	\param	nIn	a parameter of type int
	\param	nLen	a parameter of type int
	\param	bNullPointing	a parameter of type BOOL

	\return	static inline TCHAR*
	
	
*/
TCHAR* CUtil::N2Str(  TCHAR* result, int nIn, int nLen, BOOL bNullPointing)
{
	bool bMinus = false;
	if(nIn<0){
		nIn *= -1;
		bMinus=true;
	}

	TCHAR tmp[128];
	_stprintf( tmp, TEXT("%*d"), nLen, nIn);
	CopyMemory(result, tmp, nLen);
	if(bMinus)
	{
		result[0] = '-';
		Replace(result, nLen, 0x20, '0');
	}
	if(bNullPointing)
		result[nLen] = 0x00;
	return result;
}

// TCHAR* CUtil::N2StrNull(  TCHAR* result, int nIn)
// {
// 	_stprintf(result, "%d", nIn);
// 	return result;
// }

/**	\brief	long 형을 TCHAR string 형으로 변환

	\param	result	a parameter of type TCHAR*
	\param	lIn	a parameter of type LONG
	\param	nLen	a parameter of type int
	\param	bNullPointing	a parameter of type BOOL

	\return	static inline TCHAR*

	
*/
TCHAR* CUtil::L2Str(  TCHAR* result, LONG lIn, int nLen, BOOL bNullPointing)
{
	bool bMinus = false;
	if(lIn<0){
		lIn *= -1;
		bMinus=true;
	}

	TCHAR tmp[128];
	_stprintf( tmp, TEXT("%*ld"), nLen, lIn);
	CopyMemory(result, tmp, nLen);
	if(bMinus)
	{
		result[0] = '-';
		Replace(result, nLen, 0x20, '0');
	}
	if(bNullPointing)
		result[nLen] = 0x00;
	return result;
}

/**	\brief	double 형을 TCHAR string 형으로 변환

	\param	result	a parameter of type TCHAR*
	\param	dIn	a parameter of type double
	\param	nLen	a parameter of type int
	\param	nDot	a parameter of type int
	\param	bNullPointing	a parameter of type BOOL

	\return	static inline TCHAR*

	*	값은 뒤로 정렬된다.
		-1234 를 10BYTE 버퍼에 저장을 하면
		"     -1234" 가 된다.
*/
TCHAR* CUtil::D2Str(  TCHAR* result, double dIn, int nLen, int nDot, BOOL bNullPointing)
{	
//		bool bMinus = false;
//		if(dIn<0){
//			dIn*=-1.;
//			bMinus=true;
//		}
//
//		TCHAR tmp[128];
//		_stprintf( tmp, "%*.*f", nLen, nDot, dIn);
//		int len = _tcslen(tmp);
//		
//		CopyMemory(result, tmp, nLen);
//		if(bMinus)
//		{
//			result[0] = '-';
//			Replace(result, nLen, 0x20, '0');
//		}
	FillMemory(result, nLen, 0x20);
	TCHAR tmp[128];
	_stprintf( tmp, TEXT("%.*f"), nDot, dIn);
	int size = _tcslen(tmp);

	//	필요길이보다 데이터 길이가 더 길면 데이터를 자른다.
	if(size>nLen)
	{
		int nDiff = size-nLen;
		size -= nDiff;
		tmp[size] = 0x00;
	}

	CopyMemory(result+(nLen-size), tmp, size);

	if(bNullPointing)
		result[nLen] = 0x00;
	return result;
}

	//////////////////////////////////////////
	//	LLToStr
	//	- dIn : 변환하고자 하는 수


/**	\brief	LONGLONG 형을 TCHAR string 형으로 변환

	\param	result	a parameter of type TCHAR*
	\param	llIn	a parameter of type LONGLONG
	\param	nLen	a parameter of type int
	\param	bNullPointing	a parameter of type BOOL

	\return	static TCHAR*

	
*/
TCHAR* CUtil::LL2Str( TCHAR* result, LONGLONG llIn, int nLen, BOOL bNullPointing)
{
	bool bMinus = false;
	if(llIn<0){
		llIn *= -1;
		bMinus=true;
	}

	TCHAR tmp[128];
	_stprintf( tmp, TEXT("%*I64d"), nLen, llIn);
	CopyMemory(result, tmp, nLen);
	if(bMinus)
	{
		result[0] = '-';
		Replace(result, nLen, 0x20, '0');
	}
	if(bNullPointing)
		result[nLen] = 0x00;
	return result;
}

VOID CUtil::FormatErrMsg(_In_ int nErrNo, _Out_ TCHAR* pzMsg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		nErrNo,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	_tcscpy(pzMsg, (TCHAR*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//VOID CUtil::PrintErr(CLog* pLog, BOOL bDebug, int nErrNo)
//{
//	LPVOID lpMsgBuf=NULL;
//	FormatMessage( 
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//		FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL,
//		nErrNo,
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//		(LPTSTR) &lpMsgBuf,
//		0,
//		NULL 
//		);
//	if (bDebug) 
//	{
//		printf("[Error](%d)(%s)",nErrNo, (LPCTSTR)lpMsgBuf);
//	}
//	
//	if(pLog)
//	{
//		pLog->Log("[Error](%d)(%s)",nErrNo, (LPSTR)lpMsgBuf);
//	}
//	
//	LocalFree( lpMsgBuf );
//}



#define	DEF_BUF_LEN	4096
//
//void CUtil::LogMsg( CLog *log, BOOL bSucc, TCHAR* pMsg, ...)
//{
//	TCHAR buff1[DEF_BUF_LEN];
//	TCHAR buff2[DEF_BUF_LEN];
//	va_list argptr;
//	SYSTEMTIME	st;
//	
//	if(_tcslen(pMsg)>=DEF_BUF_LEN)
//		*(pMsg+DEF_BUF_LEN-1) = 0x00;
//
//	va_start(argptr, pMsg);
//	vsprintf_s(buff1, pMsg, argptr);
//	va_end(argptr);
//	
//	
//	GetLocalTime(&st);
//	if(bSucc)
//		_stprintf(buff2, "[I][%02d:%02d:%02d.%03d]%s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//	else
//		_stprintf(buff2, "[F][%02d:%02d:%02d.%03d]%s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, buff1);
//
//	log->LogEx(buff2);
//}






/************************************************************************
	진법코드를 받아서 소숫점 이하 갯수를 반환한다.

	D0, D1, D2
************************************************************************/
int	CUtil::GetDecimalCnt( TCHAR* psNTTN )
{
	int nRet;
	int nLen = _tcslen(psNTTN);
	TCHAR temp[3];
	_stprintf(temp, TEXT("%.*s"), nLen-1, psNTTN+1 );
	nRet = _ttoi(temp);
	return nRet;
}





//
///************************************************************************
//	"0000123" => "    123" 과 같이 변경 (클라이언트(델파이)를 위해)
//************************************************************************/
//TCHAR* CUtil::CvtInt_SpaceLeading( TCHAR* i_psOrg, int i_nOrgLen, int i_nOutLen, TCHAR* o_psOut )
//{
//	_stprintf( o_psOut, i_nOrgLen, TEXT("%.*s"), i_nOrgLen, i_psOrg );
//
//	int nOrg = _ttoi(o_psOut);
//
//	_stprintf( o_psOut, i_nOutLen, TEXT("%*d"), i_nOutLen, nOrg );
//
//	return o_psOut;	
//}
//
//
///************************************************************************
//	"0000123" => "    123" 과 같이 변경 (클라이언트(델파이)를 위해)
//************************************************************************/
//TCHAR* CUtil::CvtDbl_SpaceLeading( TCHAR* i_psOrg, int i_nOrgLen, int i_nOutLen, TCHAR* o_psOut )
//{
//_stprintf(o_psOut, i_nOrgLen, TEXT("%.*s"), i_nOrgLen, i_psOrg);
//
//double dOrg = _tcstod(o_psOut);
//
//_stprintf(o_psOut, i_nOutLen, TEXT("%*f"), i_nOutLen, dOrg);
//
//return o_psOut;
//}


void CUtil::GetMyModuleAndDir(TCHAR *o_psDir, TCHAR* o_psModule, TCHAR* o_psConfig)
{

	// config file
	TCHAR szFullName[_MAX_PATH];
	GetModuleFileName(NULL, szFullName, _MAX_PATH);

	int nLen = _tcslen(szFullName);
	for (int i = nLen - 1; i > 0; i--)
	{
		if (szFullName[i] == '\\')
		{
			_stprintf(o_psDir, TEXT("%.*s"), i, szFullName);
			_tcscpy(o_psModule, &(szFullName[i + 1]));
			_tcscpy(o_psConfig, GetCnfgFileNm(o_psDir, o_psModule, o_psConfig));

			return;
		}
	}
}


TCHAR* CUtil::GetCnfgFileNmOfSvc(TCHAR* i_psSvcNm, TCHAR* o_psValue)
{
	TCHAR zRegistry[512];
	_stprintf(zRegistry, TEXT("SYSTEM\\CurrentControlSet\\services\\%s"), i_psSvcNm);
	CProp prop;
	_tcscpy(o_psValue, prop.GetValue(L"ImagePath"));
	return o_psValue;
}

/*
	config file 이름
*/
TCHAR* CUtil::GetCnfgFileNm(TCHAR *i_psDir, TCHAR* i_psFileNm, TCHAR* o_psValue)
{
	BOOL bSameDir = FALSE;
	TCHAR szDir[MAX_PATH], szFileNm[MAX_PATH];

	_tcscpy(szDir, i_psDir);
	CUtil::TrimAll(szDir, _tcslen(szDir));
	if (szDir[0] == '.') {
		bSameDir = TRUE;
	}
	if(_tcslen(szDir) == 0)
		bSameDir = TRUE;

	// 현재 폴더가 아닌 경우
	if (bSameDir == TRUE)
	{
		GetCurrentDirectory(_MAX_PATH, szDir);
	}
	
	if (i_psDir[_tcslen(szDir) - 1] != '\\')
		_tcscat(szDir, TEXT("\\"));
	

	TCHAR temp[1024];
	_tcscpy(temp, i_psFileNm);
	_tcsupr(temp);
	TCHAR* pos = _tcsstr(temp, TEXT(".EXE"));
	if (pos == 0)
	{
		_stprintf(szFileNm, TEXT("%s%s.ini"), szDir, i_psFileNm);
	}
	else
	{
		int nLen = _tcslen(i_psFileNm) - _tcslen(pos);
		_stprintf(szFileNm, TEXT("%s%.*s.ini"), szDir, nLen, i_psFileNm);
	}

	_tcscpy(o_psValue, szFileNm);
	return o_psValue;
}


/*
config file 이름
*/
TCHAR* CUtil::GetCnfgXMLFileNm(TCHAR *i_psDir, TCHAR* i_psFileNm, TCHAR* o_psValue)
{
	TCHAR szDir[MAX_PATH], szFileNm[MAX_PATH];
	BOOL bSameDir = FALSE;

	_tcscpy(szDir, i_psDir);
	CUtil::TrimAll(szDir, _tcslen(szDir));
	if (szDir[0] == '.') {
		bSameDir = TRUE;
	}
	if (_tcslen(szDir) == 0)
		bSameDir = TRUE;

	// 현재 폴더가 아닌 경우
	if (bSameDir == TRUE)
	{
		GetCurrentDirectory(_MAX_PATH, szDir);
	}

	if (i_psDir[_tcslen(szDir) - 1] != '\\')
		_tcscat(szDir, TEXT("\\"));

	TCHAR temp[1024];
	_tcscpy(temp, i_psFileNm);
	_tcsupr(temp);
	TCHAR* pos = _tcsstr(temp, TEXT(".EXE"));
	if (pos == 0)
	{
		_stprintf(szFileNm, TEXT("%s%s.xml"), szDir, i_psFileNm);
	}
	else
	{
		int nLen = _tcslen(i_psFileNm) - _tcslen(pos);
		_stprintf(szFileNm, TEXT("%s%.*s.xml"), szDir, nLen, i_psFileNm);
	}

	_tcscpy(o_psValue, szFileNm);
	return o_psValue;
}


TCHAR* CUtil::GetConfig(TCHAR* i_psCnfgFileNm, TCHAR* i_psSectionNm, TCHAR* i_psKeyNm, TCHAR* o_psValue)
{
	*o_psValue = 0x00;
	DWORD dwRet = GetPrivateProfileString(i_psSectionNm, i_psKeyNm, NULL, o_psValue, 1024, (const TCHAR*)i_psCnfgFileNm);
	if (dwRet == 0)
		return NULL;

	// 주석은 제거
	TCHAR* pComment = _tcsstr(o_psValue, TEXT("//"));
	if (pComment)
		*(pComment) = 0x00;
	return o_psValue;
}

void CUtil::RemoveComment(_Out_ TCHAR* pzData)
{
	TCHAR* pComment = _tcsstr(pzData, TEXT("//"));
	if (pComment)
		*(pComment) = 0x00;
}

BOOL CUtil::GetNextConfigData(TCHAR* pzCnfgFileNm, TCHAR* pzSectionNm, TCHAR* pzPrevKeyNm, TCHAR* o_pzNextKeyNm, TCHAR* o_pzNextValue)
{
	TCHAR zTemp[1024] = { 0, };
	DWORD dwRet;

	dwRet = GetPrivateProfileString(pzSectionNm, NULL, NULL, zTemp, sizeof(zTemp), (const TCHAR*)pzCnfgFileNm);
	if (dwRet <= 0)
		return FALSE;

	std::list<_tstring> listKey;
	SplitDataEx(zTemp, NULL, dwRet, &listKey);
	if (listKey.empty())
		return FALSE;

	// 최초 조회
	if (pzPrevKeyNm[0] == NULL) {
		// 첫번째 key 와 value 반환
		_tcscpy(o_pzNextKeyNm, (*listKey.begin()).c_str());
		listKey.pop_front();
		GetPrivateProfileString(pzSectionNm, o_pzNextKeyNm, NULL, o_pzNextValue, sizeof(zTemp), (const TCHAR*)pzCnfgFileNm);
		return TRUE;
	}
	else
	{
		std::list<_tstring>::iterator it;
		for (it = listKey.begin(); it != listKey.end(); it++)
		{
			_tcscpy(zTemp, (*it).c_str());
			if (_tcscmp(zTemp, pzPrevKeyNm) == 0)
			{
				it++;
				if (it == listKey.end())
					return FALSE;

				_tcscpy(o_pzNextKeyNm, (*it).c_str());
				GetPrivateProfileString(pzSectionNm, o_pzNextKeyNm, NULL, o_pzNextValue, sizeof(zTemp), (const TCHAR*)pzCnfgFileNm);
				return TRUE;
			}
		}
	}
	return FALSE;
}
	

bool CUtil::Load_MsgFile(wchar_t* pzMsgFile, map<_tstring, _tstring>& map)
{
	wchar_t line[128] = { 0, };
	wchar_t buf[128] = { 0, };

	bool bFoundSection = false;

	setlocale(LC_ALL, ".OCP"); // locale 설정
	FILE* fp = _tfopen(pzMsgFile, TEXT("rb"));
	if (fp == NULL)
		return false;

	map.clear();
	while (_fgetts(line, 100, fp) != NULL)
	{
		wchar_t* pEqual = _tcschr(line, '=');
		if (pEqual == NULL)
			continue;

		wchar_t* pComment = _tcsstr(line, TEXT("//"));
		if (pComment)
			*(pComment) = 0x00;

		/*
		EURUSD=EURUSD.g // this is test
		*/
		int nLenOfLine = _tcslen(line);	// 15
		int nLenOfEqual = _tcslen(pEqual);	// 9

		
		// ACNT=12345
		_stprintf(buf, TEXT("%.*s"), nLenOfLine - nLenOfEqual, line);
		_tstring sKey = buf;

		_stprintf(buf, TEXT("%.*s"), nLenOfEqual - 1, pEqual + 1);
		_tstring sValue = buf;

		map[sKey] = sValue;
	}

	return true;

}

//
//int CUtil::GetTickGap(double dFirstPrc, double dSndPrc, int nDotCnt, double dTickSize)
//{
//	double dPow = pow(10., (double)nDotCnt);
//
//	dFirstPrc = dFirstPrc * dPow;
//	dSndPrc = dSndPrc * dPow;
//	int nGapTick = 0;
//	nGapTick = (int)((dFirstPrc - dSndPrc) / dTickSize / dPow);
//	return nGapTick;
//}
//
//double CUtil::GetPrcByTick(TCHAR* pzOrigPrc, double dTickCnt, double dTickSize, TCHAR cPlusMinus)
//{
//	double dRsltPrc = _tcstod(pzOrigPrc);
//	if(cPlusMinus=='+') dRsltPrc += dTickCnt* dTickSize;
//	else				dRsltPrc -= dTickCnt* dTickSize;
//
//	return dRsltPrc;
//}
//
///*
//#define	FORMAT_PRC(prc,dotcnt,out) { _stprintf(out, "%0*.*f", LEN_PRC, dotcnt, prc); }
//*/
//int CUtil::CompPrc(const TCHAR* pPrc1, const int nLen1, const TCHAR* pPrc2, const int nLen2, const int nDotCnt, const int nFormatLen)
//{
//	TCHAR zPrc1[32], zPrc2[32];
//	_stprintf(zPrc1, TEXT("%.*s"), nLen1, pPrc1);
//	_stprintf(zPrc1, TEXT("%0*.*f"), nFormatLen, nDotCnt, _tcstod(zPrc1));
//
//	_stprintf(zPrc2,TEXT( "%.*s"), nLen2, pPrc2);
//	_stprintf(zPrc2,TEXT( "%0*.*f"), nFormatLen, nDotCnt, _tcstod(zPrc2));
//	
//	return strncmp(zPrc1, zPrc2, nFormatLen);
//
//}
//
//int CUtil::CompPrc(const double pPrc1, const double pPrc2, const int nDotCnt, const int nFormatLen)
//{
//	TCHAR zPrc1[32], zPrc2[32];
//	_stprintf(zPrc1, TEXT("%0*.*f"), nFormatLen, nDotCnt, pPrc1);
//
//	_stprintf(zPrc2, TEXT("%0*.*f"), nFormatLen, nDotCnt, pPrc2);
//
//	return strncmp(zPrc1, zPrc2, nFormatLen);
//
//}
//
//
//BOOL CUtil::IsSamePrice( TCHAR* pPrc1, int nLen1, TCHAR* pPrc2, int nLen2, int nDotCnt, int nFormatLen)
//{
//	TCHAR zPrc1[32], zPrc2[32];
//	_stprintf(zPrc1, "%.*s", nLen1, pPrc1);
//	_stprintf(zPrc1, "%0*.*f", nFormatLen, nDotCnt, _tcstod(zPrc1));
//
//	_stprintf(zPrc2, "%.*s", nLen2, pPrc2);
//	_stprintf(zPrc2, "%0*.*f", nFormatLen, nDotCnt, _tcstod(zPrc2));
//
//	return (strncmp(zPrc1, zPrc2, nFormatLen)==0);
//
//}

VOID CUtil::SplitDataEx(_In_ TCHAR* psData, _In_ TCHAR cDelimeter, _In_ int nSize, _Out_ std::list<_tstring>* pListResult)
{
	TCHAR pData[1024] = { 0, };
	memcpy(pData, psData, nSize);

	if (cDelimeter == 0x00)
	{
		for (int i = 0; i < nSize; i++)
		{
			if (*(pData + i) == 0x00)
			{
				*(pData + i) = '@';
			}
		}
	}

	SplitData(pData, '@', pListResult);
}

/*
	24시간 고려한 점검

	07~23:59:59
	00 ~ 05:59:59

	TIME_HHMM		//HHMM
	,TIME_HHMMSS	//HHMMSS
	,TIME_HH_MM		//HH:MM
	,TIME_HH_MM_SS	//HH:MM:SS
*/
//BOOL IsPassedTime(TCHAR* pzBaseTime, EN_TIMEMODE timeMode)
//{
//	BOOL bPassed = FALSE;
//
//	SYSTEMTIME st;
//	GetLocalTime(&st); 
//	TCHAR now1[32];
//	_stprintf(now1, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);
//	BOOL bNowIsToday = FALSE;
//	if (strncmp(now1, "07:00:00", 8) >= 0 &&
//		strncmp(now1, "23:59:59", 8) <= 0)
//	{
//		bNowIsToday = TRUE;
//	}
//
//	TCHAR now[32];
//	BOOL bBaseTimeIsToday = FALSE;
//	switch (timeMode)
//	{
//	case TIME_HHMM:
//		_stprintf(now, "%02d%02d", st.wHour, st.wMinute);
//
//		if (strncmp(pzBaseTime, "0700", 4) >= 0 &&
//			strncmp(pzBaseTime, "2359", 4) <= 0)
//			bBaseTimeIsToday = TRUE;
//
//		break;
//
//	case TIME_HHMMSS:
//		_stprintf(now, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);
//
//		if (strncmp(pzBaseTime, "070000", 6) >= 0 &&
//			strncmp(pzBaseTime, "235959", 6) <= 0)
//			bBaseTimeIsToday = TRUE;
//		break;
//
//	case TIME_HH_MM:
//		_stprintf(now, "%02d:%02d", st.wHour, st.wMinute);
//
//		if (strncmp(pzBaseTime, "07:00", 5) >= 0 &&
//			strncmp(pzBaseTime, "23:59", 5) <= 0)
//			bBaseTimeIsToday = TRUE;
//		break;
//
//	case TIME_HH_MM_SS:
//		_stprintf(now, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
//
//		if (strncmp(pzBaseTime, "07:00:00", 8) >= 0 &&
//			strncmp(pzBaseTime, "23:59:59", 8) <= 0)
//			bBaseTimeIsToday = TRUE;
//		break;
//	}
//
//
//	
//	
//	
//
//	// 07~23
//	if (bNowIsToday)
//	{
//		// 07~23
//		if (bBaseTimeIsToday)
//		{
//			if (_tcscmp(now, pzBaseTime) >= 0)
//				bPassed = TRUE;
//		}
//		// 00 ~ 06
//		else
//		{
//			bPassed = FALSE;
//		}
//	}
//	// 00~06
//	if (!bNowIsToday)
//	{
//		// 00~06
//		if (!bBaseTimeIsToday)
//		{
//			if (_tcscmp(now, pzBaseTime) >= 0)
//				bPassed = TRUE;
//		}
//		// 07 ~ 23
//		else
//		{
//			bPassed = TRUE;
//		}
//	}
//
//	return bPassed;
//}

VOID CUtil::SplitData(_In_ TCHAR* psData, _In_ TCHAR cDelimeter, _Out_ std::list<_tstring>* pListResult)
{
	TCHAR* pFind;
	TCHAR temp[1024];
	TCHAR* pData = psData;



	while (TRUE)
	{
		pFind = _tcschr(pData, cDelimeter);

		// 123/456
		if (!pFind)
		{
			if (_tcslen(pData) > 0)
			{
				_tstring sData = temp;
				pListResult->push_back(sData);
			}
			break;
		}
			

		// list 에 넣는다. ( 123/456/)
		_stprintf(temp, TEXT("%.*s"), _tcslen(pData) - _tcslen(pFind), pData);
		_tstring sData = temp;
		pListResult->push_back(sData);
		
		// '/' 다음에 데이터가 없으면
		// 123/456/
		if (_tcslen(pFind) == 1)
			break;

		pData = pFind + 1;
	}
}

void CUtil::Assert(_In_ wchar_t* wpzMsg)
{
	MessageBox(NULL, wpzMsg, TEXT("Assert"), MB_OK);
}

void CUtil::Assert(_In_ char* pzMsg)
{
	wchar_t wzMsg[128] = { 0, };
	A2U(pzMsg, wzMsg);
	Assert(wzMsg);
}

char* U2A(_In_ TCHAR* pUniStr, _Out_ char* pAnsiStr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, pUniStr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pUniStr, -1, pAnsiStr, len, NULL, NULL);
	return pAnsiStr;
}

int U2ALen(_In_ TCHAR* pUniStr, _Out_ char* pAnsiStr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, pUniStr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pUniStr, -1, pAnsiStr, len, NULL, NULL);
	return len;
}


TCHAR* A2U(_In_ char* pAnsiStr, _Out_ TCHAR* pUniStr)
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, pAnsiStr, strlen(pAnsiStr), NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, pAnsiStr, strlen(pAnsiStr), pUniStr, nLen);
	return pUniStr;
}


VOID	DumpErr(TCHAR* pzSrc, int nErr, TCHAR* pzMsg)
{
	LPVOID lpMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		nErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	_stprintf(pzMsg, TEXT("[%s:%d]%s"), pzSrc, nErr, (TCHAR*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}



TCHAR* GetCnfgValue(TCHAR* i_psCnfgFileNm, TCHAR* i_psSectionNm, TCHAR* i_psKeyNm, TCHAR* o_psValue)
{
	*o_psValue = 0x00;
	DWORD dwRET = GetPrivateProfileString(i_psSectionNm, i_psKeyNm, NULL, o_psValue, 1024, (const TCHAR*)i_psCnfgFileNm);
	// 주석은 제거
	TCHAR* pComment = _tcsstr(o_psValue, TEXT("//"));
	if (pComment)
		*(pComment) = 0x00;
	return o_psValue;
}

// //yyyymmdd-hhmmss
VOID	getGMTtime(TCHAR* pOut)
{
	time_t rawtime;
	time(&rawtime);
	struct tm *ltime = gmtime(&rawtime);

	_stprintf(pOut, TEXT("%04d%02d%02d-%02d%02d%02d"),
		1900 + ltime->tm_year, ltime->tm_mon + 1, ltime->tm_mday, ltime->tm_hour, ltime->tm_min, ltime->tm_sec);
}



//
///*
//파라미터로 들어온 시간과
//현재시간 비교해서
//몇초가 지났는지 반환
//
//pStartTime - hhmmss  (235015)
//*/
//int CUtil::GetPassedSeconds(TCHAR* psStartTime, BOOL bColon)
//{
//	int nPassedSeconds = 0;
//	
//	// now
//	TCHAR zNow[32], zStartTime[32];;
//	SYSTEMTIME st; GetLocalTime(&st);
//	_stprintf(zNow, "%02d%02d%02d", st.wHour, st.wMinute, st.wSecond);
//
//
//	if (bColon)
//		_stprintf(zStartTime, "%.2s%.2s%.2s", psStartTime, psStartTime + 3, psStartTime + 6);
//	else
//		_stprintf(zStartTime, "%.6s", psStartTime);
//
//	int nComp = strncmp(zStartTime, zNow, 6);
//
//	if (nComp = 0)
//		return 0;
//
//	// 날짜가 지난 경우
//	// 하루의 초 - 235015의 초 + 현재까지의 초
//	if (nComp < 0)
//	{
//		// 전날 00시 부터 pStartTime까지의 초 
//		// pStartTime이 23:50:15 이면 23 * 60*60 + 50*60 + 15
//		int nHours = S2N(zStartTime, 2) * 60 * 60;	// 현재 시간을 초로
//		int nMins = S2N(zStartTime + 2, 2) * 60;	// 현재 분을 초로
//		int nTotalSecs = nHours + nMins + S2N(zStartTime + 4, 2);
//
//		// pStartTime 부터 자정까지의 시간
//		int nPassedSecs1 = 60 * 60 * 24 - nTotalSecs;
//
//		// 당일 자정부터 현재까지의 시간
//		nHours = st.wHour * 60 * 60;
//		nMins = st.wMinute * 60;
//		nTotalSecs = nHours + nMins + st.wSecond;
//
//		nPassedSeconds = nTotalSecs + nTotalSecs;
//	}
//	// 당일인 경우
//	// 현재까지의 초 - 235015의 초
//	else
//	{
//		// 당일 00시 부터 pStartTime까지의 초 
//		// pStartTime이 23:50:15 이면 23 * 60*60 + 50*60 + 15
//		int nHours = S2N(zStartTime, 2) * 60 * 60;	// 현재 시간을 초로
//		int nMins = S2N(zStartTime + 2, 2) * 60;	// 현재 분을 초로
//		int nTotalSecs = nHours + nMins + S2N(zStartTime + 4, 2);
//
//		// 당일 자정부터 현재까지의 시간
//		nHours = st.wHour * 60 * 60;
//		nMins = st.wMinute * 60;
//		nPassedSeconds = nHours + nMins + st.wSecond - nTotalSecs;
//	}
//
//	return nPassedSeconds;
//}