#include "StringUtils.h"



CStringUtils::CStringUtils()
{}
CStringUtils::~CStringUtils()
{}

void CStringUtils::RTrim(char* pBuf, int nScope)
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

void CStringUtils::LTrim(char* pBuf)
{
	int nLen = lstrlen(pBuf);
	if (nLen == 0)
		return;

	char *pTmp = new char[nLen+1];
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
void CStringUtils::RemoveChar(char* pData, int nLen, char i_cTarget)
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

void CStringUtils::ReplaceChar(char* pData, char cOldChar, char cNewChar, int posStart, int nCnt)
{
	int nLen = (nCnt == 0) ? strlen(pData + posStart) : nCnt;

	for(int i=posStart; i< nLen;i++)
	{
		if(pData[i] == cOldChar)
			pData[i] = cNewChar;
	}
}
char* CStringUtils::SubStr(char* p, int start, int len)
{
	int nCnt = (len == 0) ? strlen(p + start) : len;

	sprintf_s(m_z, "%.*s", nCnt, p + start);
	return m_z;
}


string CStringUtils::stringFormat(const char* p, ...)
{
	smartptrBuf buf(1024);

	va_list argptr;
	va_start(argptr, p);
	vsprintf(buf.get(), p, argptr);
	va_end(argptr);

	return string(buf.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////////////////////

CStringCvt::CStringCvt() {}
CStringCvt::~CStringCvt() {}

int		CStringCvt::Str2Int(char* pszIn, int nLen)
{
	if (!nLen)	return atoi(pszIn);

	char result[128];
	sprintf_s(result, "%*.*s", nLen, nLen, pszIn);
	return atoi(result);
}


long CStringCvt::Str2Long(char* pszIn, int nLen)
{
	if (!nLen)	return atoi(pszIn);

	char result[128];
	sprintf_s(result, "%*.*s", nLen, nLen, pszIn);
	return atol(result);
}

double	CStringCvt::Str2Dbl(char* pszIn, int nLen )
{
	if (!nLen)	return atof(pszIn);

	char result[128];
	sprintf_s(result, "%*.*s", nLen, nLen, pszIn);
	return atof(result);
}


LONGLONG CStringCvt::Str2LongLong(char* pszIn, int nLen)
{
	if (!nLen)	return _atoi64(pszIn);

	char result[128];
	sprintf_s(result, "%*.*s", nLen, nLen, pszIn);
	//ReplaceChr(result, result+nLen, 0x20, '0');
	return _atoi64(result);
}

/////////////////////////////////////////////////////////////////////////////////////////////
/// 
//
/////////////////////////////////////////////////////////////////////////////////////////////

CSplitString::CSplitString() {
	InitializeCriticalSection(&m_cs);
}
CSplitString::~CSplitString() {
	Clear();
	DeleteCriticalSection(&m_cs);
}


/*
	123/456/
	123/456
*/
int CSplitString::Split(const char* pString, char cDeli)
{
	Clear();

	char* pFind;
	const char* pData = pString;

	while (TRUE)
	{
		pFind = strchr((char*)pData, cDeli);		

		if (pFind)
		{
			// list 에 넣는다. ( 123/456/)
			sprintf(m_z, "%.*s", strlen(pData) - strlen(pFind), pData);

			EnterCriticalSection(&m_cs);
			m_vec.push_back(string(m_z));
			LeaveCriticalSection(&m_cs);

			// '/' 다음에 데이터가 없으면
			// 123/456/
			if (strlen(pFind) == 1)
				break;

			pData = pFind + 1;
		}
		else
		{
			if (strlen(pData) > 0)
			{
				EnterCriticalSection(&m_cs);
				m_vec.push_back(string(pData));
				LeaveCriticalSection(&m_cs);
			}
			break;
		}
	}

	return m_vec.size();
}

string	CSplitString::at(int idx)
{
	if (idx >= (int)m_vec.size())
		return "";

	
	EnterCriticalSection(&m_cs);
	string sRes = m_vec[idx];
	LeaveCriticalSection(&m_cs);

	return sRes;
}

void CSplitString::Clear()
{
	ZeroMemory(m_z, sizeof(m_z));
	m_vec.clear();
}