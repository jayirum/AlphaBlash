#include "AlphaProtocol.h"
#include <string.h>
#include "util.h"


#include "LogMsg.h"
//extern CLogMsg	g_log;

CProtoSet::CProtoSet()
{}
CProtoSet::~CProtoSet()
{}

void CProtoSet::Begin()
{
	m_sBuf.erase(m_sBuf.begin(), m_sBuf.end());
}
int CProtoSet::Complete(/*out*/string& result)
{
	char temp[2048];
	sprintf(temp, "%c%d=%0*d%c%s", 0x02, FDS_PACK_LEN, DEF_PACKETLEN_SIZE, m_sBuf.size(), DEF_DELI, m_sBuf.c_str());
	result = temp;
	return result.size();
}
int CProtoSet::Complete(/*out*/char* pzResult)
{
	sprintf(pzResult, "%c%d=%0*d%c%s", 0x02, FDS_PACK_LEN, DEF_PACKETLEN_SIZE, m_sBuf.size(), DEF_DELI, m_sBuf.c_str());
	return strlen(pzResult);
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

void CProtoSet::SetVal(int nFd, char* val)
{
	if (strlen(val) == 0)
		return;
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%s%c", nFd, val, DEF_DELI);
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

void CProtoGet::SetValByField(char* pzFd, char* pzVal, ST_VAL* pVal)
{
	int nFd = atoi(pzFd);
	if (nFd > 0 && nFd < 500)
	{
		pVal->sVal = string(pzVal);
	}
	else if (nFd >= 500 && nFd < 700)
	{
		pVal->nVal = atoi(pzVal);
	}
	else if (nFd>=700 && nFd<800){
		pVal->dVal = atof(pzVal);
	}
}

int CProtoGet::Parsing(_In_  const char* pData, int nDataLen, BOOL bLog)
{
	// 제일 첫 바이트 STX 는 제거
	sprintf(m_zBuf, "%.*s",nDataLen, pData);
	//if(bLog) g_log.log(INFO, "[GET](RECV)(%s)", m_zBuf);

	m_mapResult.erase(m_mapResult.begin(), m_mapResult.end());
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
			ST_VAL stVal; 
			stVal.dVal = 0; stVal.nVal = 0;
			SetValByField(zFd, zVal, &stVal);
			m_mapResult[atoi(zFd)] = stVal;

			//if (bLog) g_log.log(INFO, "[GET](ADD)(KEY=%s)(VALUE=%s)", zFd, zVal);
			//printf("[MAP][%s][%s]\n", zFd, zVal);

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
			ST_VAL stVal; stVal.nVal = 0; stVal.dVal = 0;
			SetValByField(zFd, zVal, &stVal);
			m_mapResult[atoi(zFd)] = stVal;
			//printf("[MAP][%s][%s]\n", zFd, zVal);
		}
	}
	return m_mapResult.size();
}

bool CProtoGet::IsParsed()
{
	return (m_mapResult.size() > 0);
}

bool CProtoGet::GetCode(_Out_ string& sCode)
{
	if (!IsParsed())
		return false;
	map<int, ST_VAL>::iterator it = m_mapResult.find(FDS_CODE);
	if (it == m_mapResult.end())
		return false;

	sCode = (*it).second.sVal;
	return true;
}


bool CProtoGet::GetVal(int nFd, _Out_ char* pzVal)
{
	*pzVal = 0x00;

	if (!IsParsed())
		return false;
	map<int, ST_VAL>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	strcpy(pzVal, (*it).second.sVal.c_str());
	return true;
}

char* CProtoGet::GetValS(int nFd, char* pzVal)
{
	*pzVal = 0x00;
	if (!GetVal(nFd, pzVal))
		return NULL;

	return pzVal;
}


bool CProtoGet::GetVal(int nFd, _Out_ string* psVal)
{
	if (!IsParsed())
		return false;
	map<int, ST_VAL>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	*psVal = (*it).second.sVal;
	return true;
}
bool CProtoGet::GetVal(int nFd, _Out_ int* pnVal)
{
	*pnVal = 0;

	if (!IsParsed())
		return false;
	map<int, ST_VAL>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	*pnVal = (*it).second.nVal;
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

	map<int, ST_VAL>::iterator it = m_mapResult.find(nFd);
	if (it == m_mapResult.end())
		return false;

	*pdVal = (*it).second.dVal;
	return true;
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
	char temp[128];

	pStx = strchr(m_buf, 0x02);
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
	int nHeaderLen = 1 + 3 + 1 + DEF_PACKETLEN_SIZE + 1; // STX, 134, =, 0092, 0x01

	// 다음에 STX 가 또 있고, 지금 이 패킷이 불완전 패킷이다.
	if (m_nBufLen > nPacketLen + nHeaderLen)
	{
		char* pNext = strchr(pStx + 1, 0x02);
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

	// copy one packet
	memcpy(pOutBuf, m_buf + nHeaderLen, nPacketLen);
	m_nBufLen -= (nHeaderLen + nPacketLen);
	*pnLen = nPacketLen;

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
