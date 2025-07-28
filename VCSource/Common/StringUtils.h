// CommonUtil.h: interface for the CCommonUtil class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4996)


#include "AlphaInc.h"
#include <string.h>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <assert.h>
using namespace std;

class smartptrBuf;
class CStringUtils;
class CStringCvt;
class CSplitString;

class smartptrBuf
{
public:
	smartptrBuf(int size) { m_p = new char[size]; ZeroMemory(m_p, size); }
	~smartptrBuf() { if (m_p) delete[] m_p; m_p = NULL; }

	char* get() { return m_p; };

private:
	char* m_p;
};




class CStringUtils
{
public:
	CStringUtils();
	~CStringUtils();

	void	RTrim(char* pBuf, int nScope);
	void	LTrim(char* pBuf);
	
	void	RemoveChar(char* pData, int nLen, char i_cTarget);
	void	ReplaceChar(char* pData, char cOldChar, char cNewChar, int posStart=0, int nCnt=0 );

	string	stringFormat(const char* p, ...);

	char* SubStr(char* p, int start, int len = 0);
private:
	char	m_z[__ALPHA::LEN_BUF];
};


class CStringCvt
{
public:
	CStringCvt();
	~CStringCvt();

	int		Str2Int(char* pszIn, int nLen = 0);
	long	Str2Long(char* pszIn, int nLen = 0);
	double	Str2Dbl(char* pszIn, int nLen = 0);
	LONGLONG Str2LongLong(char* pszIn, int nLen = 0);

	char* Int2Str(int nIn)					{ sprintf_s(m_z, "%d", nIn); return m_z; }
	char* Long2Str(LONG lIn)				{ sprintf_s(m_z, "%ld", lIn); return m_z; }
	char* Dbl2Str(double dIn, int nDotCnt)	{ sprintf_s(m_z, "%.*f", dIn,nDotCnt); return m_z; }
	char* LongLong2Str(LONGLONG llIn)		{ sprintf_s(m_z, "%I64d", llIn); return m_z; }

private:
	char	m_z[__ALPHA::LEN_BUF];

};

class CSplitString
{
public:
	CSplitString();
	~CSplitString();

	int		Split(const char* pString, char cDeli);
	string	at(int idx);
	int		size() { return m_vec.size(); }
private:
	void	Clear();

private:
	vector<string>		m_vec;
	CRITICAL_SECTION	m_cs;
	char				m_z[__ALPHA::LEN_BUF];
};