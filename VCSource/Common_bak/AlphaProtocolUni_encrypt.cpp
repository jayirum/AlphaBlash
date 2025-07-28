#include "AlphaProtocolUni.h"
#include "util.h"
#include "CCrypt.h"
#include <memory.h>
//#include "Log.h"
//extern CLog	g_log;

CProtoSet::CProtoSet()
{}
CProtoSet::~CProtoSet()
{}

void CProtoSet::Begin()
{
	m_sBuf.erase(m_sBuf.begin(), m_sBuf.end());
}

int CProtoSet::Complete(/*out*/char* pzResult)
{
	CCrypt crypt;
	char szEncrypt[2048] = { 0, };
	DWORD len = 0;
	if (crypt.Encrypt((char*)m_sBuf.c_str(), szEncrypt, m_sBuf.size(), m_sBuf.size(), &len) == FALSE)
		return -1;

	// STX
	// 134=0195
	// 0x01
	sprintf(pzResult, "%c%d=%0*d%c", DEF_STX, FDS_PACK_LEN, DEF_PACKETLEN_SIZE, len, DEF_DELI);
	memcpy(pzResult + DEF_HEADER_SIZE, szEncrypt, len);

	return (len+DEF_HEADER_SIZE);
}


void CProtoSet::CopyFromRecvData(char* pzData)
{
	m_sBuf = pzData;
}

void CProtoSet::DelSameField(int nFd)
{
	sprintf(m_zTemp, "%d=", nFd);
	basic_string <char>::size_type deli;
	basic_string <char>::size_type field = m_sBuf.find(m_zTemp);
	if (field == string::npos)
		return;

	// 제일 첫 데이터
	if (field == 0)
	{
		deli = m_sBuf.find(DEF_DELI, field);
		if (deli != string::npos)
		{
			m_sBuf.erase(field, deli - field + 1);
		}
	}
	else {
		sprintf(m_zTemp, "%c%d=", DEF_DELI, nFd);
		field = m_sBuf.find(m_zTemp);
		if (field == string::npos)
			return;
		deli = m_sBuf.find(0x01, field+1);
		if (deli != string::npos)
			m_sBuf.erase(field, deli - field);
	}
}

void CProtoSet::SetVal(int nFd, string val)
{
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%s%c", nFd, val.c_str(), DEF_DELI);
	m_sBuf += m_zTemp;
}


void CProtoSet::SetVal(int nFd, _tstring val)
{
	char zVal[512] = { 0, };
	wchar_t wzVal[512] = { 0, };
	_tcscpy(wzVal, val.c_str());
	U2A(wzVal, zVal);
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%s%c", nFd, zVal, DEF_DELI);
	m_sBuf += m_zTemp;
}

void CProtoSet::SetVal(int nFd, char* val)
{
	if (strlen(val) == 0)
		return;
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%s%c", nFd, val, DEF_DELI);
	m_sBuf += m_zTemp;
}


void CProtoSet::SetVal(int nFd, TCHAR* val)
{
	char zVal[512] = { 0, };
	U2A(val, zVal);

	if (strlen(zVal) == 0)
		return;
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%s%c", nFd, zVal, DEF_DELI);
	m_sBuf += m_zTemp;
}


void CProtoSet::SetVal(int nFd, char val)
{
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%c%c", nFd, val, DEF_DELI);
	m_sBuf += m_zTemp;
}

/*
	111(FDN_ARRAY_SIZE)=5 0x01
	22(FDS_USER_NICK_NM)=탑트레이더 0x1F 22=단타 0x1F
	0x01
	
*/
void CProtoSet::SetArrayStart(int nArraySize)
{
	DelSameField(FDN_ARRAY_SIZE);
	sprintf(m_zTemp, "%d=%d%c", FDN_ARRAY_SIZE, nArraySize, DEF_DELI);
	m_sBuf += m_zTemp;
}

void CProtoSet::SetArrayVal(int nFd, const char* val)
{
	if (strlen(val) == 0)
		return;

	sprintf(m_zTemp, "%d=%s%c", nFd, val, DEF_DELI_ARRAY);
	m_sBuf += m_zTemp;
}

void CProtoSet::SetArrayEnd()
{
	m_sBuf += DEF_DELI;
}


void CProtoSet::SetVal(int nFd, int val)
{
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%d%c", nFd, val, DEF_DELI);
	m_sBuf += m_zTemp;
}
void CProtoSet::SetVal(int nFd, double val)
{
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%.*f%c", nFd,DOT_CNT, val, DEF_DELI);
	m_sBuf += m_zTemp;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

CProtoGet::CProtoGet()
{}

CProtoGet::~CProtoGet()
{}


int CProtoGet::GetFieldType(int nField)
{
	int nFieldType;
	if (nField > 0 && nField < 500)
	{
		nFieldType = FIELD_STR;
	}
	else if (nField >= 500 && nField < 700)
	{
		nFieldType = FIELD_INT;
	}
	else if (nField >= 700 && nField < 800) {
		nFieldType = FIELD_DBL;
	}

	return nFieldType;
}

void CProtoGet::SetValByField(char* pzFd, char* pzVal, _Out_ ST_VAL* pVal)
{
	int nFd = atoi(pzFd);
	
	int nFieldTp = GetFieldType(nFd);
	if (nFieldTp == FIELD_STR)
	{
		pVal->sVal = string(pzVal);
	}
	else if (nFieldTp == FIELD_INT)
	{
		pVal->nVal = atoi(pzVal);
	}
	else if (nFieldTp == FIELD_DBL) {
		pVal->dVal = atof(pzVal);
	}
}


void CProtoGet::removeAll()
{
	map<int, ST_VAL*>::iterator it;
	for (it = m_mapResult.begin(); it != m_mapResult.end(); it++)
	{
		delete (*it).second;
	}
	m_mapResult.clear();
}


/*
	STX
	134=0195
	0x01
	134=0195
*/
int CProtoGet::Parsing(_In_  const char* pData, int nDataLen, BOOL bLog)
{
	sprintf(m_zBuf, "%.*s",nDataLen, pData);
	
	removeAll();

	char temp[1024];
	char zFd[32], zVal[1024];
	char* pEqual;
	char* pFirst = m_zBuf;
	char* pDeli = strchr(pFirst, DEF_DELI);

	// 1=1004/2=2049/
	while (pDeli)
	{
		sprintf(temp, "%.*s", (int)(pDeli - pFirst), pFirst);
		pEqual = strchr(pFirst, '=');

		//1004/2=2004  :  =이 deli 보다 뒤에 있다.
		if(pEqual>pDeli)
		{
			pFirst = pDeli + 1;
			pDeli = strchr(pFirst, DEF_DELI);
			continue;
		}
		// 1-1004/2=2004
		if (pEqual)
		{
			// 1=1004/2=2004/
			sprintf(zFd, "%.*s", (int)(pEqual - pFirst), pFirst);
			sprintf(zVal, "%.*s", (int)(pDeli - pEqual-1), pEqual + 1);
			ST_VAL* stVal= new ST_VAL;
			stVal->dVal = 0; stVal->nVal = 0;
			SetValByField(zFd, zVal, stVal);
			m_mapResult[atoi(zFd)] = stVal;

			// 다음에 데이터가 없으면 나간다.
			if (strlen(pDeli) == 1)
				break;
			pFirst = pDeli + 1;
			pDeli = strchr(pFirst, DEF_DELI);
		}
		else
		{
			// 2=2049/39876
			break;
		}
	}

	// 2=2004
	if (pDeli==NULL || strlen(pDeli) > 1)
	{
		pEqual = strchr(pFirst, '=');
		if (pEqual)
		{
			sprintf(zFd, "%.*s", (int)(pEqual - pFirst), pFirst);
			sprintf(zVal, "%.*s", strlen(pEqual) - 1, pEqual + 1);
			ST_VAL *stVal = new ST_VAL; 
			stVal->nVal = 0; stVal->dVal = 0;
			SetValByField(zFd, zVal, stVal);
			m_mapResult[atoi(zFd)] = stVal;
		}
	}
	return m_mapResult.size();
}



/*
	STX
	134=0195
	0x01
	134=0195
*/
int CProtoGet::ParsingEncrypt(_In_  const char* pData, int nDataLen, BOOL bLog)
{
	char zEnc[2048] = { 0, }, zDec[2048] = { 0, };
	int nEncSize = nDataLen - DEF_HEADER_SIZE;
	memcpy(zEnc, pData + DEF_HEADER_SIZE, nEncSize);
	CCrypt crypt;
	DWORD len;
	crypt.Decrypt(zEnc, zDec, nEncSize,&len);

	return Parsing(zDec, len);
}


int CProtoGet::ParsingDebug(_In_  const char* pData, int nDataLen, BOOL bLog)
{
	// 제일 첫 바이트 STX 는 제거
	sprintf(m_zBuf, "%.*s", nDataLen - 1, pData + 1);
	//g_log.log("(DEBUG)(%s)", m_zBuf);

	removeAll();

	char temp[1024];
	char zFd[32], zVal[1024];
	char* pEqual;
	char* pFirst = m_zBuf;
	char* pDeli = strchr(pFirst, DEF_DELI);

	// 1=1004/2=2049/
	while (pDeli)
	{
		sprintf(temp, "%.*s", (int)(pDeli - pFirst), pFirst);
		pEqual = strchr(pFirst, '=');

		//g_log.log("Parsing-1[%.20s][%.20s][%.20s]", pFirst, pEqual, pDeli);

		//1004/2=2004  :  =이 deli 보다 뒤에 있다.
		if (pEqual > pDeli)
		{
			pFirst = pDeli + 1;
			pDeli = strchr(pFirst, DEF_DELI);
			continue;
		}
		// 1-1004/2=2004
		if (pEqual)
		{
			// 1=1004/2=2004/
			sprintf(zFd, "%.*s", (int)(pEqual - pFirst), pFirst);
			sprintf(zVal, "%.*s", (int)(pDeli - pEqual - 1), pEqual + 1);
			ST_VAL* stVal = new ST_VAL;
			stVal->dVal = 0; stVal->nVal = 0;
			SetValByField(zFd, zVal, stVal);
			m_mapResult[atoi(zFd)] = stVal;

			//g_log.log("Parsing-2[%s][%s]", zFd, zVal);

			// 다음에 데이터가 없으면 나간다.
			if (strlen(pDeli) == 1)
				break;
			pFirst = pDeli + 1;
			pDeli = strchr(pFirst, DEF_DELI);
		}
		else
		{
			// 2=2049/39876
			break;
		}
	}

	// 2=2004
	if (pDeli == NULL || strlen(pDeli) > 1)
	{
		pEqual = strchr(pFirst, '=');
		if (pEqual)
		{
			sprintf(zFd, "%.*s", (int)(pEqual - pFirst), pFirst);
			sprintf(zVal, "%.*s", strlen(pEqual) - 1, pEqual + 1);
			ST_VAL* stVal = new ST_VAL;
			stVal->nVal = 0; stVal->dVal = 0;
			SetValByField(zFd, zVal, stVal);
			m_mapResult[atoi(zFd)] = stVal;
		}
	}
	return m_mapResult.size();
}

bool CProtoGet::IsParsed()
{
	return (m_mapResult.size() > 0);
}

bool CProtoGet::GetCode(_Out_ _tstring& sCode)
{
	if (!IsParsed())
		return false;
	map<int, ST_VAL*>::iterator it = m_mapResult.find(FDS_CODE);
	if (it == m_mapResult.end())
		return false;

	wchar_t wzCode[32];
	_stprintf(wzCode, TEXT("%s"), ((*it).second)->sVal.c_str());
	sCode = wzCode;
	return true;
}

bool CProtoGet::GetCode(_Out_ string& sCode)
{
	if (!IsParsed())
		return false;
	map<int, ST_VAL*>::iterator it = m_mapResult.find(FDS_CODE);
	if (it == m_mapResult.end())
		return false;

	sCode = (*it).second->sVal;
	return true;
}

bool CProtoGet::GetVal(int nFd, _Out_ wchar_t* pzVal)
{
	*pzVal = 0x00;

	if (!IsParsed())
		return false;
	map<int, ST_VAL*>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	char zVal[1024] = { 0, };
	strcpy(zVal, (*it).second->sVal.c_str());
	A2U(zVal, pzVal);
	return true;
}


bool CProtoGet::GetVal(int nFd, _Out_ char* pzVal)
{
	*pzVal = 0x00;

	if (!IsParsed())
		return false;
	map<int, ST_VAL*>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	strcpy(pzVal, (*it).second->sVal.c_str());
	return true;
}

wchar_t* CProtoGet::GetValS(int nFd, wchar_t* pzVal)
{
	*pzVal = 0x00;
	if (!GetVal(nFd, pzVal))
		return NULL;

	return pzVal;
}


bool CProtoGet::GetVal(int nFd, _Out_ _tstring* psVal)
{
	if (!IsParsed())
		return false;
	map<int, ST_VAL*>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	wchar_t wzVal[1024] = { 0, };
	A2U((char*)(*it).second->sVal.c_str(), wzVal);
	*psVal = wzVal;
	return true;
}

bool CProtoGet::GetVal(int nFd, _Out_ string* psVal)
{
	if (!IsParsed())
		return false;
	map<int, ST_VAL*>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	*psVal = (*it).second->sVal;
	return true;
}

bool CProtoGet::GetVal(int nFd, _Out_ int* pnVal)
{
	*pnVal = 0;

	if (!IsParsed())
		return false;
	map<int, ST_VAL*>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	*pnVal = (*it).second->nVal;
	return true;
}


int	CProtoGet::GetValN(int nFd)
{
	int val = 0;
	GetVal(nFd, &val);
	return val;
}

double	CProtoGet::GetValD(int nFd)
{
	double val = 0;
	GetVal(nFd, &val);
	return val;
}


bool CProtoGet::GetVal(int nFd, _Out_ double* pdVal)
{
	*pdVal = 0;
	if (!IsParsed())
		return false;

	map<int, ST_VAL*>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	*pdVal = (*it).second->dVal;
	return true;
}


void CProtoGet::SeeAllData(char* pzOut)
{
	map<int, ST_VAL*>::iterator it;
	char zField[512];
	*pzOut = 0;
	for (it = m_mapResult.begin(); it != m_mapResult.end(); it++)
	{
		ZeroMemory(zField, sizeof(zField));
		int nFieldTp = GetFieldType((*it).first);
		if(nFieldTp==FIELD_STR)
			sprintf(zField, "[%d=%s]", (*it).first, (*it).second->sVal.c_str());
		if (nFieldTp == FIELD_INT)
			sprintf(zField, "[%d=%d]", (*it).first, (*it).second->nVal);
		if (nFieldTp == FIELD_DBL)
			sprintf(zField, "[%d=%f]", (*it).first, (*it).second->dVal);

		strcat(pzOut, zField);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CProtoUtils::GetValue(_In_ char* pzRecvData, _In_ int nField, char* pzVal)
{
	char zField[32] = { 0, };
	sprintf(zField, "%d", nField);
	return CProtoUtils::GetValue(pzRecvData, zField, pzVal);
}

bool CProtoUtils::GetValue(char* pzRecvData, char* pzField, char* pzVal)
{
	char zField[32];
	sprintf(zField, "%s=", pzField);

	int nFind = 0;
	char* pFind = NULL;
	char* pPrev = NULL;
	bool bFind = false;
	while (1) {
		pFind = strstr(pzRecvData+ nFind, zField);
		if (pFind == NULL)
			return false;

		// 2=xxx 를 찾았는데, 혹시 22=yyy 가 찾아질 수도 있다. 
		
		// pFind 가 처음이거나, 
		if (pFind == pzRecvData) {
			bFind = true;
			break;
		}
		//아니면 한바이트 앞에가 DELI 이어야 한다.
		pPrev = pFind;
		pPrev--;
		if (*pPrev == DEF_DELI) {
			bFind = true;
			break;
		}

		nFind = (int)(pFind - pzRecvData);
	}

	if (!bFind)
		return false;

	// Equal
	char* pEqual = strchr(pFind, '=');
	if (pEqual == NULL)
		return false;

	// 다음 Deli
	char* pDeli = strchr(pEqual, DEF_DELI);
	if (pDeli == NULL) {
		sprintf(pzVal, "%.*s", strlen(pEqual + 1), pEqual + 1);
	}
	else
	{
		sprintf(pzVal, "%.*s", (int)(pDeli - (pEqual + 1)), pEqual + 1);
	}
	return true;
}

/*
STX
134 = 0195
0x01
134 = 0195
*/
bool CProtoUtils::GetPlainFullPacket(_In_ char* pzEncFullPacket, _Out_ char* pzPlainFullPacket, _Out_ int* pnPlainFullLen)
{
	char zEncBody[2048] = { 0, };
	char zDecBody[2048] = { 0, };
	char zEncLen[32] = { 0, };
	sprintf(zEncLen, "%.*s", PACKET_CODE_SIZE, pzEncFullPacket + 1/*STX*/ + 4/*134=*/);
	int nEncLen = atoi(zEncLen);
	memcpy(zEncBody, pzEncFullPacket + DEF_HEADER_SIZE, nEncLen);
	CCrypt crypt;
	DWORD len;
	BOOL bRet = crypt.Decrypt(zEncBody, zDecBody, nEncLen, &len);
	sprintf(pzPlainFullPacket, "%.*s%.*s", DEF_HEADER_SIZE, pzEncFullPacket, len, zDecBody);
	*pnPlainFullLen = strlen(pzPlainFullPacket);
	return bRet;
}

void CProtoUtils::GetEncryptFullPacket(_In_ char* pzPlainFullPacket, _Out_ char* pzEncryptFullPacket, _Out_ int* pnPackLen)
{
	char zPlainBody[2048] = { 0, };
	char zEncBody[2048] = { 0, };
	
	char zDataLen[32] = { 0, };
	sprintf(zDataLen, "%.*s", PACKET_CODE_SIZE, pzPlainFullPacket + 1/*STX*/ + 4/*134=*/);
	int nPlainDataLen = atoi(zDataLen);

	sprintf(zPlainBody, "%*s", nPlainDataLen, pzPlainFullPacket + DEF_HEADER_SIZE);

	CCrypt crypt;
	DWORD len;
	BOOL bRet = crypt.Encrypt(zPlainBody, zEncBody, nPlainDataLen, nPlainDataLen, &len);

	sprintf(pzEncryptFullPacket, "%.*s", DEF_HEADER_SIZE, pzPlainFullPacket);
	memcpy(pzEncryptFullPacket + DEF_HEADER_SIZE, zEncBody, len);

	*pnPackLen = DEF_HEADER_SIZE + len;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CProtoBuffering::CProtoBuffering()
{
	m_nBufLen = 0;
	ZeroMemory(m_buf, sizeof(m_buf));
	InitializeCriticalSectionAndSpinCount(&m_cs, 2000);
}

CProtoBuffering::~CProtoBuffering()
{
	DeleteCriticalSection(&m_cs);
}

int CProtoBuffering::AddPacket(char* pBuf, int nSize)
{
	if (!pBuf)
		return 0;

	int nRet = 0;
	Lock();
	__try
	{
		__try
		{
			memcpy(m_buf + m_nBufLen, pBuf, nSize);
			m_nBufLen += nSize;
		}
		__except (ReportException(GetExceptionCode(), "AddPacket", m_msg))
		{
			nRet = -1;
		}
	}
	__finally
	{
		Unlock();
	}
	nRet = m_nBufLen;
	return nRet;
}

/*
	return value : 밖에서 이 함수를 한번 더 호출할 것인가 여부

	*pnLen : 실제 pOutBuf 에 copy 되는 size
*/
BOOL	CProtoBuffering::GetOnePacket(_Out_ int* pnLen, char* pOutBuf)
{
	if (!pOutBuf) return FALSE;
	*pnLen = 0;
	BOOL bRet;
	Lock();

	__try
	{
		__try
		{
			bRet = GetOnePacketFn(pnLen, pOutBuf);
		}
		__except (ReportException(GetExceptionCode(), "GetOnePacket", m_msg))
		{
			bRet = FALSE;
		}
	}
	__finally
	{
		Unlock();
	}
	

	return bRet;
}


/*
	return value : 밖에서 이 함수를 한번 더 호출할 것인가 여부

	*pnLen : 실제 pOutBuf 에 copy 되는 size
*/

BOOL	CProtoBuffering::GetOnePacketFn(int* pnLen, char* pOutBuf)
{
	*pnLen = 0;

	if (m_nBufLen==0 ) {
		strcpy(m_msg, "No data in the buffer");
		*pnLen = 0;
		return FALSE;
	}

	//	find stx
	char* pStx;;
	char temp[1024];

	pStx = (char*)memchr(m_buf, 0x02, m_nBufLen);
	if (pStx == NULL) {
		strcpy(m_msg, "No STX in the packet");
		RemoveAll();
		return FALSE;
	}

	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
	if (pStx != m_buf)
	{
		char backup[MAX_BUFFERING] = { 0, };

		int nRemoveLen = (int)(pStx - m_buf);
		m_nBufLen -= nRemoveLen;
		
		memcpy(backup, pStx, m_nBufLen);
		ZeroMemory(m_buf, sizeof(m_buf));
		memcpy(m_buf, backup, m_nBufLen);
		*pnLen = 0;
		return TRUE;
	}

	// 패킷상의 길이
	// STX134=00950x01
	sprintf(temp, "%.*s", DEF_PACKETLEN_SIZE, pStx + 5);	//134=
	int nPacketLen = atoi(temp);
	int nHeaderLen = DEF_HEADER_SIZE;	// 1 + 3 + 1 + DEF_PACKETLEN_SIZE + 1; // STX, 134, =, 0092, 0x01

	// 다음에 STX 가 또 있고, 지금 이 패킷이 불완전 패킷이다.
	if (m_nBufLen > nPacketLen + nHeaderLen)
	{
		char* pNext = (char*)memchr(pStx + 1, 0x02, m_nBufLen-1);
		if (pNext)
		{
			int nGapLen = (int)(pNext - pStx);
			if (nGapLen < nPacketLen + nHeaderLen)
			{
				sprintf(m_msg, "Remain abnormal packet. remove it.(%.*s)", nPacketLen + nHeaderLen, pStx);
				*pnLen = 0;

				char backup[MAX_BUFFERING] = { 0, };
				m_nBufLen -= (nPacketLen + nHeaderLen);
				memcpy(backup, pNext, m_nBufLen);
				ZeroMemory(m_buf, sizeof(m_buf));
				memcpy(m_buf, backup, m_nBufLen);
				return TRUE;
			}
		}
	}

	// 불완전 패킷
	if ( (m_nBufLen - (nHeaderLen + nPacketLen)) < 0)
	{
		sprintf(m_msg, "Remain Len is minus.(Org Buf Len:%d)(HeaderLen:%d)(PacketLen:%d)", m_nBufLen, nHeaderLen, nPacketLen);
		*pnLen = 0;
		return FALSE;
	}

	/////////////////////////////////////////////////////////////////
	// copy one packet
	/////////////////////////////////////////////////////////////////
	ZeroMemory(temp, sizeof(temp));
	memcpy(temp, m_buf + nHeaderLen, nPacketLen);
	CCrypt crypt;
	DWORD nDescLen = 0;
	crypt.Decrypt(temp, pOutBuf, nPacketLen, &nDescLen);
	m_nBufLen -= (nHeaderLen + nPacketLen);
	*pnLen = nPacketLen;
	/////////////////////////////////////////////////////////////////



	BOOL bSomthingStillLeft = FALSE;
	if (m_nBufLen == 0)
	{
		RemoveAll();
	}
	else if (m_nBufLen > 0)
	{
		// 온전한 하나의 패킷 반환하고 남은 패킷 버퍼 앞으로 이동
		char backup[MAX_BUFFERING] = { 0, };
		memcpy(backup, m_buf + nHeaderLen + nPacketLen, m_nBufLen);
		ZeroMemory(m_buf, sizeof(m_buf));
		memcpy(m_buf, backup, m_nBufLen);
		bSomthingStillLeft = TRUE;
	}
	
	return bSomthingStillLeft;
}

VOID CProtoBuffering::RemoveAll()
{
	ZeroMemory(m_buf, sizeof(m_buf));
	m_nBufLen = 0;
}
