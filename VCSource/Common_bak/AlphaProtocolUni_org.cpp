/*
	Packet structure

	STX
	134=0020	// length of Body (without header)
	DEF_DELI
	Body
*/


#include "AlphaProtocolUni.h"
#include "../CommonAnsi/Util.h"

CProtoSet::CProtoSet()
{}
CProtoSet::~CProtoSet()
{}

void CProtoSet::Begin()
{
	m_sBuf.erase(m_sBuf.begin(), m_sBuf.end());
	m_bSetSuccYN = false;

	char zTime[32] = { 0, };
	__ALPHA::Now(zTime);
	SetVal(FDS_TM_HEADER, zTime);
}
int CProtoSet::Complete(/*out*/string& result, BOOL bForDelphi/*=FALSE*/)
{
	char zResult[MAX_ONEPACKET] = { 0 };
	int nRes = Complete(zResult, bForDelphi);
	if (nRes > 0)
		result = string(zResult);
	return nRes;
}
int CProtoSet::Complete(/*out*/char* pzResult, BOOL bForDelphi/*=FALSE*/)
{
	// FDS_SUCC_YN 을 Set 하지 않았으면 default 로 Y 설정한다.
	if (!m_bSetSuccYN)
	{
		SetVal(FDS_SUCC_YN, "Y");
	}

	if (bForDelphi)
		m_sBuf += DEF_ENTER;	//ENTER

	sprintf(pzResult, "%c%d=%0*d%c%s", 
		DEF_STX, 
		FDS_PACK_LEN, 
		DEF_PACKETLEN_SIZE, 
		m_sBuf.size(), 
		DEF_DELI, 
		m_sBuf.c_str());
	return strlen(pzResult);
}

int CProtoSet::NormalToDelphi(_InOut_ string& sPacket)
{
	sPacket += DEF_ENTER;

	int nNewDataLen = sPacket.size() - DEF_HEADER_SIZE;

	sprintf(m_zTemp, "%c%d=%0*d%c%s",
		DEF_STX,
		FDS_PACK_LEN,
		DEF_PACKETLEN_SIZE,
		nNewDataLen,
		DEF_DELI,
		sPacket.substr(DEF_HEADER_SIZE, nNewDataLen).c_str()
	);
	sPacket.clear();
	sPacket = m_zTemp;
	return sPacket.size();
}

int CProtoSet::DelphiToNormal(_InOut_ string& sPacket)
{
	if (sPacket[sPacket.size() - 1] != DEF_ENTER)
		return sPacket.size();

	int nNewDataLen = sPacket.size() - 1 - DEF_HEADER_SIZE;

	sprintf(m_zTemp, "%c%d=%0*d%c%s",
		DEF_STX,
		FDS_PACK_LEN,
		DEF_PACKETLEN_SIZE,
		nNewDataLen,
		DEF_DELI,
		sPacket.substr(DEF_HEADER_SIZE, nNewDataLen).c_str()
	);
	sPacket.clear();
	sPacket = m_zTemp;
	return sPacket.size();
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
		deli = m_sBuf.find(DEF_DELI, field + 1);
		if (deli != string::npos)
			m_sBuf.erase(field, deli - field);
	}
}

void CProtoSet::SetVal(int nFd, string val)
{
	if (val.empty())
		return;

	char zVal[512] = { 0 };
	strcpy(zVal, val.c_str());
	SetVal(nFd, zVal);
}

void CProtoSet::SetVal(int nFd, char* val)
{
	if (val == NULL)
		return;

	if (strlen(val) == 0)
		return;
	DelSameField(nFd);
	sprintf(m_zTemp, "%d=%s%c", nFd, val, DEF_DELI);
	m_sBuf += m_zTemp;

	if (nFd == FDS_SUCC_YN)
		m_bSetSuccYN = "Y";
}


#ifdef _UNICODE
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
#endif


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

	sprintf(m_zTemp, "%d=%s%c", nFd, val, DEF_DELI_RECORD);
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
	sprintf(m_zTemp, "%d=%.*f%c", nFd, DOT_CNT, val, DEF_DELI);
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
{
	removeAll();
}


int CProtoGet::GetFieldType(int nField)
{
	int nFieldType = -1;
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

bool CProtoGet::SetValByField(char* pzFd, char* pzVal, _Out_ ST_VAL* pVal)
{
	bool bRes = true;
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
	else
		bRes = false;

	return bRes;
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
*	pData : Header 없는 순수 데이터 패킷
	 return : the field count
*/
bool CProtoGet::Parsing(_In_  const char* pData, int nDataLen, BOOL bLog)
{
	memcpy(m_zBuf, pData, nDataLen);

	char zFd[FIELD_LEN] = { 0 };
	smartptr zVal(MAX_BUF);

	char* pEqual;
	char* pStart	= m_zBuf;
	int nTotalLen	= strlen(m_zBuf);
	int nMovedLen	= 0;

	// 1=1004/2=2049/
	char* pDeli = strchr(pStart, DEF_DELI);

	while (pDeli)
	{
		nMovedLen += (int)(pDeli - pStart) + 1;

		pEqual = strchr(pStart, '=');	// find '='

		if (!pEqual) {
			sprintf(m_zMsg, "can't find '='(%s)", pStart);
			return (false);
		}

		//1004[0x01]2=2004  :  =이 deli 보다 뒤에 있다.
		if (pEqual > pDeli) {
			pStart = pDeli + 1;
			pDeli = strchr(pStart, DEF_DELI);

			continue; // back to the first
		}

		// 1=1004[0x01]2=2004
		sprintf(zFd, "%.*s", (int)(pEqual - pStart), pStart);
		sprintf(zVal.get(), "%.*s", (int)(pDeli - pEqual - 1), pEqual + 1);

		ST_VAL* stVal = new ST_VAL;	stVal->dVal = 0; stVal->nVal = 0;
		if (!SetValByField(zFd, zVal.get(), stVal))
		{
			sprintf(m_zMsg, "Undefined Field-1:%s", zFd);

			return (false);
		}
		m_mapResult[atoi(zFd)] = stVal;

		if (nMovedLen == nTotalLen)	// parse the whole packet. finish.
		{
			break;
		}

		// go to the next round
		pStart = pDeli + 1;
		pDeli = strchr(pStart, DEF_DELI);
	}
	return true;
}


/*
*/
bool CProtoGet::ParsingWithHeader(_In_  const char* pRecvData, int nRecvLen, BOOL bLog)
{
	// Remove Header from the orginal packet.
	ZeroMemory(m_zBuf, sizeof(m_zBuf));
	int nCopyLen = nRecvLen - DEF_HEADER_SIZE;
	int ret = sprintf(m_zBuf, "%.*s", nCopyLen, pRecvData + DEF_HEADER_SIZE);
	
	// if there is no delimiter at the end of the packet, attach it for parsing logic
	if (m_zBuf[nCopyLen - 1] != DEF_DELI) {
		m_zBuf[nCopyLen] = DEF_DELI;
		nCopyLen++;
	}

	return Parsing(m_zBuf, nCopyLen);

	

	//// 2=2004  ==> in case there is no delimeter at the end of the packet
	////if (pDeli == NULL || strlen(pDeli) > 1)
	//if(nMovedLen>0 && nMovedLen < nTotalLen )
	//{
	//	pEqual = strchr(m_zBuf+nMovedLen, '=');
	//	if (pEqual)
	//	{
 //			sprintf(zFd,		"%.*s", (int)(pEqual - pStart), pStart);
	//		sprintf(zVal.get(), "%.*s", strlen(pEqual) - 1,		pEqual + 1);
	//		
	//		ST_VAL* stVal = new ST_VAL;	stVal->nVal = 0; stVal->dVal = 0;
	//		if (!SetValByField(zFd, zVal.get(), stVal))
	//		{
	//			sprintf(m_zMsg, "Undefined Field-2:%s", zFd);
	//			
	//			return (false);
	//		}
	//		m_mapResult[atoi(zFd)] = stVal;
	//	}
	//}
	
}

//
//int CProtoGet::ParsingDebug(_In_  const char* pData, int nDataLen, BOOL bLog)
//{
//	// 제일 첫 바이트 STX 는 제거
//	sprintf(m_zBuf, "%.*s", nDataLen - 1, pData + 1);
//	//g_log.log("(DEBUG)(%s)", m_zBuf);
//
//	removeAll();
//
//	//char temp[MAX_BUF] = { 0 };
//	char zFd[FIELD_LEN] = { 0 };// , zVal[MAX_BUF] = { 0 };
//	smartptr temp(MAX_BUF);
//	smartptr zVal(MAX_BUF);
//
//	char* pEqual;
//	char* pStart = m_zBuf;
//	char* pDeli = strchr(pStart, DEF_DELI);
//
//	// 1=1004/2=2049/
//	while (pDeli)
//	{
//		sprintf(temp.get(), "%.*s", (int)(pDeli - pStart), pStart);
//		pEqual = strchr(pStart, '=');
//
//		//g_log.log("Parsing-1[%.20s][%.20s][%.20s]", pStart, pEqual, pDeli);
//
//		//1004/2=2004  :  =이 deli 보다 뒤에 있다.
//		if (pEqual > pDeli)
//		{
//			pStart = pDeli + 1;
//			pDeli = strchr(pStart, DEF_DELI);
//			continue;
//		}
//		// 1-1004/2=2004
//		if (pEqual)
//		{
//			// 1=1004/2=2004/
//			sprintf(zFd, "%.*s", (int)(pEqual - pStart), pStart);
//			sprintf(zVal.get(), "%.*s", (int)(pDeli - pEqual - 1), pEqual + 1);
//			ST_VAL* stVal = new ST_VAL;
//			stVal->dVal = 0; stVal->nVal = 0;
//			SetValByField(zFd, zVal.get(), stVal);
//			m_mapResult[atoi(zFd)] = stVal;
//
//			//g_log.log("Parsing-2[%s][%s]", zFd, zVal);
//
//			// 다음에 데이터가 없으면 나간다.
//			if (strlen(pDeli) == 1)
//				break;
//			pStart = pDeli + 1;
//			pDeli = strchr(pStart, DEF_DELI);
//		}
//		else
//		{
//			// 2=2049/39876
//			break;
//		}
//	}
//
//	// 2=2004
//	if (pDeli == NULL || strlen(pDeli) > 1)
//	{
//		pEqual = strchr(pStart, '=');
//		if (pEqual)
//		{
//			sprintf(zFd, "%.*s", (int)(pEqual - pStart), pStart);
//			sprintf(zVal.get(), "%.*s", strlen(pEqual) - 1, pEqual + 1);
//			ST_VAL* stVal = new ST_VAL;
//			stVal->nVal = 0; stVal->dVal = 0;
//			SetValByField(zFd, zVal.get(), stVal);
//			m_mapResult[atoi(zFd)] = stVal;
//		}
//	}
//	return m_mapResult.size();
//}

bool CProtoGet::IsParsed()
{
	return (m_mapResult.size() > 0);
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


bool  CProtoGet::Is_Success()
{
	string sRslt;
	GetVal(FDS_SUCC_YN, &sRslt);
	return (sRslt == "Y");
}

int CProtoGet::Get_RsltCode()
{
	int nRsltCode;
	GetVal(FDN_RSLT_CODE, &nRsltCode);
	return nRsltCode;
}

#ifdef _UNICODE

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


#endif


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
	char zField[FIELD_LEN];
	*pzOut = 0;
	for (it = m_mapResult.begin(); it != m_mapResult.end(); it++)
	{
		ZeroMemory(zField, sizeof(zField));
		int nFieldTp = GetFieldType((*it).first);
		if (nFieldTp == FIELD_STR)
			sprintf(zField, "[%d=%s]", (*it).first, (*it).second->sVal.c_str());
		if (nFieldTp == FIELD_INT)
			sprintf(zField, "[%d=%d]", (*it).first, (*it).second->nVal);
		if (nFieldTp == FIELD_DBL)
			sprintf(zField, "[%d=%f]", (*it).first, (*it).second->dVal);

		strcat(pzOut, zField);
	}
}

int	CProtoGet::CopyData(_Out_ map<int, string>* pOuter)
{
	map<int, ST_VAL*>::iterator it;
	int nFd;
	smartptr zVal(MAX_BUF);
	//char zVal[MAX_BUF];

	for (it = m_mapResult.begin(); it != m_mapResult.end(); it++)
	{
		ZeroMemory(zVal.get(), MAX_BUF);

		nFd = (*it).first;

		int nFieldTp = GetFieldType((*it).first);
		if (nFieldTp == FIELD_STR)
			sprintf(zVal.get(), (*it).second->sVal.c_str());
		if (nFieldTp == FIELD_INT)
			sprintf(zVal.get(), "%d", (*it).second->nVal);
		if (nFieldTp == FIELD_DBL)
			sprintf(zVal.get(), "%f", (*it).second->dVal);

		(*pOuter)[nFd] = string(zVal.get());
	}
	return (*pOuter).size();
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
	char zField[FIELD_LEN] = { 0, };
	sprintf(zField, "%d", nField);
	return CProtoUtils::GetValue(pzRecvData, zField, pzVal);
}

bool CProtoUtils::GetSymbol(char* pzRecvData, _Out_ string& sSymbol)
{
	char zSymbol[128] = { 0 };
	bool ret = GetValue(pzRecvData, FDS_SYMBOL, zSymbol);
	sSymbol = string(zSymbol);
	return ret;
}

bool CProtoUtils::GetUserId(char* pzRecvData, _Out_ string& sUserId)
{
	char val[128] = { 0 };
	bool ret = GetValue(pzRecvData, FDS_USER_ID, val);
	sUserId = string(val);
	return ret;
}

bool CProtoUtils::GetValue(char* pzRecvData, char* pzField, char* pzVal)
{
	char zField[FIELD_LEN];
	sprintf(zField, "%s=", pzField);

	int nFind = 0;
	char* pFind = NULL;
	char* pPrev = NULL;
	bool bFind = false;
	while (1) {
		pFind = strstr(pzRecvData + nFind, zField);
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

char* CProtoUtils::PacketCode(_In_ char* pzRecvData, _Out_ char* pzPacketCode)
{
	char zField[FIELD_LEN];
	sprintf(zField, "%d=", FDS_CODE);


	char* pFind = strstr(pzRecvData, zField);
	if (pFind == NULL)
		return NULL;

	sprintf(pzPacketCode, "%.*s", PACKET_CODE_SIZE, (pFind + 4));	// 101=

	return pzPacketCode;
}

bool  CProtoUtils::IsSuccess(_In_ char* pzRecvData)
{
	char zSuccYN[32] = { 0 };
	CProtoUtils::GetValue(pzRecvData, FDS_SUCC_YN, zSuccYN);
	return (zSuccYN[0] == 'Y');
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
	try
	{

		memcpy(m_buf + m_nBufLen, pBuf, nSize);
		m_nBufLen += nSize;
		nRet = m_nBufLen;
	}
	catch (...)
	{
		nRet = -1;
	}
	Unlock();
	return nRet;
}



BOOL	CProtoBuffering::GetOnePacketWithHeader(int* pnLen, char* pOutBuf)
{
	*pnLen = 0;

	if (m_nBufLen == 0) {
		strcpy(m_msg, "No data in the buffer");
		*pnLen = 0;
		return FALSE;
	}

	//	find stx
	char* pStx;
	//char temp[MAX_BUF];
	smartptr temp(MAX_BUF);

	pStx = strchr(m_buf, DEF_STX);
	if (pStx == NULL) {
		strcpy(m_msg, "No STX in the packet");
		RemoveAll();
		return FALSE;
	}

	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
	if (pStx != m_buf)
	{
		int nRemoveLen = (int)(pStx - m_buf);
		m_nBufLen -= nRemoveLen;
		MoveData(pStx, m_nBufLen);
		*pnLen = 0;
		return TRUE;
	}

	// 패킷상의 길이
	// STX134=00950x01
	sprintf(temp.get(), "%.*s", DEF_PACKETLEN_SIZE, pStx + 5);	//134=
	int nDataLen = atoi(temp.get());
	int nOutBufLen = PACKET_HEADER_SIZE + nDataLen;
	// 불완전 패킷
	if (m_nBufLen < nOutBufLen)
	{
		sprintf(m_msg, "Remain Len is minus.(Org Buf Len:%d)(HeaderLen:%d)(PacketLen:%d)", m_nBufLen, PACKET_HEADER_SIZE, nDataLen);
		*pnLen = 0;
		return FALSE;
	}

	// copy one packet
	memcpy(pOutBuf, m_buf, nOutBufLen);

	// 패킷 중간에 또 STX 가 있으면 첫번째 블럭이 잘못된 경우. 이전 블럭은 버린다.
	// STX... STX......
	char* p2ndStx = NULL;
	if ( (p2ndStx =strchr(pOutBuf + 1, DEF_STX)) > 0)
	{
		int nRemoveLen = (int)(p2ndStx - pOutBuf);
		m_nBufLen -= nRemoveLen;
		MoveData(m_buf+ nRemoveLen, m_nBufLen);
		*pnLen = 0;
		return TRUE;
	}


	m_nBufLen -= nOutBufLen;

	*pnLen = nDataLen;

	BOOL bSomthingStillLeft = FALSE;
	if (m_nBufLen == 0)
	{
		RemoveAll();
	}
	else if (m_nBufLen > 0)
	{
		// 온전한 하나의 패킷 반환하고 남은 패킷 버퍼 앞으로 이동
		MoveData(m_buf + nOutBufLen, m_nBufLen);
		bSomthingStillLeft = TRUE;
	}

	return bSomthingStillLeft;
}


void CProtoBuffering::MoveData(char* pStart, int nCopySize)
{
	char backup[MAX_BUFFERING] = { 0, };
	memcpy(backup, pStart, nCopySize);
	ZeroMemory(m_buf, sizeof(m_buf));
	memcpy(m_buf, backup, nCopySize);

}

/*
	return value : 밖에서 이 함수를 한번 더 호출할 것인가 여부

	*pnLen : 실제 pOutBuf 에 copy 되는 size
*/
BOOL CProtoBuffering::GetOnePacket(_Out_ int* pnLen, char* pOutBuf)
{
	if (!pOutBuf) return FALSE;
	*pnLen = 0;
	BOOL bRet;
	Lock();

	try
	{
		bRet = GetOnePacketInner(pnLen, pOutBuf);	// GetOnePacketFn(pnLen, pOutBuf);
	}
	catch (...)
	{
		bRet = FALSE;
	}
	Unlock();


	return bRet;
}



/*
	1.Find STX
	2.Copy buffer by Packet Len
	3.move
*/
BOOL CProtoBuffering::GetOnePacketInner(int* pnLen, char* pOutBuf)
{
	*pnLen = 0;

	if (m_nBufLen == 0) {
		strcpy(m_msg, "No data in the buffer");
		*pnLen = 0;
		return FALSE;
	}

	//	find stx
	char* pStx;
	//char temp[MAX_BUF];
	smartptr temp(MAX_BUF);

	pStx = strchr(m_buf, DEF_STX);
	if (pStx == NULL) {
		strcpy(m_msg, "No STX in the packet");
		RemoveAll();
		return FALSE;
	}

	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
	if (pStx != m_buf)
	{
		int nRemoveLen = (int)(pStx - m_buf);
		m_nBufLen -= nRemoveLen;
		MoveData(pStx, m_nBufLen);

		*pnLen = 0;
		return TRUE;
	}

	// 패킷상의 길이
	// STX134=00950x01
	sprintf(temp.get(), "%.*s", DEF_PACKETLEN_SIZE, pStx + 5);	//134=
	int nDataLen = atoi(temp.get());

	// 미완성 패킷
	if (m_nBufLen < (PACKET_HEADER_SIZE + nDataLen))
	{
		sprintf(m_msg, "Remain Len is minus.(Org Buf Len:%d)(HeaderLen:%d)(PacketLen:%d)", m_nBufLen, PACKET_HEADER_SIZE, nDataLen);
		*pnLen = 0;
		return FALSE;
	}

	// copy one packet
	memcpy(pOutBuf, m_buf + PACKET_HEADER_SIZE, nDataLen);

	// 패킷 중간에 또 STX 가 있으면 첫번째 블럭이 잘못된 경우. 이전 블럭은 버린다.
	// STX... STX......
	char* p2ndStx = NULL;
	if ((p2ndStx = strchr(pOutBuf + 1, DEF_STX)) > 0)
	{
		int nRemoveLen = (int)(p2ndStx - pOutBuf);
		m_nBufLen -= nRemoveLen;
		MoveData(m_buf+ nRemoveLen, m_nBufLen);
		*pnLen = 0;
		return TRUE;
	}

	m_nBufLen -= (PACKET_HEADER_SIZE + nDataLen);
	*pnLen = nDataLen;

	BOOL bSomthingStillLeft = FALSE;
	if (m_nBufLen == 0)
	{
		RemoveAll();
	}
	else if (m_nBufLen > 0)
	{
		// 온전한 하나의 패킷 반환하고 남은 패킷 버퍼 앞으로 이동
		MoveData(m_buf + PACKET_HEADER_SIZE + nDataLen, m_nBufLen);
		bSomthingStillLeft = TRUE;
	}

	return bSomthingStillLeft;
}


//
///*
//	return value : 밖에서 이 함수를 한번 더 호출할 것인가 여부
//
//	# pnLen : 실제 pOutBuf 에 copy 되는 size
//	 
//	# Header STX, 134, =, 0092, 0x01 는 제거되고 밖으로 copy 된다.
//*/
//
//BOOL CProtoBuffering::GetOnePacketFn(int* pnLen, char* pOutBuf)
//{
//	*pnLen = 0;
//
//	if (m_nBufLen==0 ) {
//		strcpy(m_msg, "No data in the buffer");
//		*pnLen = 0;
//		return FALSE;
//	}
//
//	//	find stx
//	char* pStx;;
//	char temp[128];
//
//	pStx = strchr(m_buf,DEF_STX);
//	if (pStx == NULL) {
//		strcpy(m_msg, "No STX in the packet");
//		RemoveAll();
//		return FALSE;
//	}
//
//	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
//	if (pStx != m_buf)
//	{
//		char backup[MAX_BUFFERING] = { 0, };
//
//		int nRemoveLen = (int)(pStx - m_buf);
//		m_nBufLen -= nRemoveLen;
//		
//		memcpy(backup, pStx, m_nBufLen);
//		ZeroMemory(m_buf, sizeof(m_buf));
//		memcpy(m_buf, backup, m_nBufLen);
//		*pnLen = 0;
//		return TRUE;
//	}
//
//	// 패킷상의 길이
//	// STX134=00950x01
//	sprintf(temp, "%.*s", DEF_PACKETLEN_SIZE, pStx + 5);	//134=
//	int nPacketLen = atoi(temp);
//	//int nHeaderLen = PACKET_HEADER_SIZE;	// 1 + 3 + 1 + DEF_PACKETLEN_SIZE + 1; // STX, 134, =, 0092, 0x01
//
//	// 다음에 STX 가 또 있고, 지금 이 패킷이 잘못된 패킷이면 잘못된 패킷 제거하고, 이후 패킷 살린다.
//	if (m_nBufLen > nPacketLen + PACKET_HEADER_SIZE)
//	{
//		char* pNext = strchr(pStx + 1, DEF_STX);
//		if (pNext)
//		{
//			int nGapLen = (int)(pNext - pStx);
//			if (nGapLen < nPacketLen + PACKET_HEADER_SIZE)
//			{
//				sprintf(m_msg, "Remain abnormal packet. remove it.(%.*s)", nPacketLen + PACKET_HEADER_SIZE, pStx);
//				*pnLen = 0;
//
//				char backup[MAX_BUFFERING] = { 0, };
//				m_nBufLen -= (nPacketLen + PACKET_HEADER_SIZE);
//				memcpy(backup, pNext, m_nBufLen);
//				ZeroMemory(m_buf, sizeof(m_buf));
//				memcpy(m_buf, backup, m_nBufLen);
//				return TRUE;
//			}
//		}
//	}
//
//	// 불완전 패킷
//	if ( m_nBufLen < (PACKET_HEADER_SIZE + nPacketLen)) 
//	{
//		sprintf(m_msg, "Remain Len is minus.(Org Buf Len:%d)(HeaderLen:%d)(PacketLen:%d)", m_nBufLen, PACKET_HEADER_SIZE, nPacketLen);
//		*pnLen = 0;
//		return FALSE;
//	}
//
//	// copy one packet
//	memcpy(pOutBuf, m_buf + PACKET_HEADER_SIZE, nPacketLen);
//	m_nBufLen -= (PACKET_HEADER_SIZE + nPacketLen);
//	*pnLen = nPacketLen;
//
//	BOOL bSomthingStillLeft = FALSE;
//	if (m_nBufLen == 0)
//	{
//		RemoveAll();
//	}
//	else if (m_nBufLen > 0)
//	{
//		// 온전한 하나의 패킷 반환하고 남은 패킷 버퍼 앞으로 이동
//		char backup[MAX_BUFFERING] = { 0, };
//		memcpy(backup, m_buf + PACKET_HEADER_SIZE + nPacketLen, m_nBufLen);
//		ZeroMemory(m_buf, sizeof(m_buf));
//		memcpy(m_buf, backup, m_nBufLen);
//		bSomthingStillLeft = TRUE;
//	}
//	
//	return bSomthingStillLeft;
//}

VOID CProtoBuffering::RemoveAll()
{
	ZeroMemory(m_buf, sizeof(m_buf));
	m_nBufLen = 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CPacketParser::CPacketParser()
{
	//m_nBufLen = 0;
	InitializeCriticalSectionAndSpinCount(&m_cs, 2000);
}

CPacketParser::~CPacketParser()
{
	DeleteCriticalSection(&m_cs);
}

int CPacketParser::AddPacket(char* pBuf, int nSize)
{
	if (!pBuf)
		return 0;

	int nRet = 0;
	Lock();
	try
	{
		m_sBuffer.append(pBuf, nSize);
		//memcpy(m_buf + m_nBufLen, pBuf, nSize);
		//m_nBufLen += nSize;
		nRet = m_sBuffer.size(); //m_nBufLen;
	}
	catch (...)
	{
		nRet = -1;
	}
	Unlock();
	return nRet;
}



BOOL CPacketParser::GetOnePacketWithHeaderWithLock(int* pnLen, _Out_ char* pOutBuf)
{
	Lock();
	BOOL bRes = GetOnePacketWithHeader(pnLen, pOutBuf);
	Unlock();
	return bRes;
}

BOOL CPacketParser::GetOnePacketWithHeader(int* pnLen, char* pOutBuf)
{
	*pnLen = 0;

	//if (m_nBufLen == 0) {
	if (m_sBuffer.size() == 0) {
		strcpy(m_msg, "No data in the buffer");
		return FALSE;
	}

	//	find stx
	//char* pStx;;
	//char temp[128];

	//pStx = strchr(m_buf, DEF_STX);
	int nPosStx = m_sBuffer.find_first_of(DEF_STX);
	if (nPosStx == string::npos) {
		strcpy(m_msg, "No STX in the packet");
		//RemoveAll();
		m_sBuffer.clear();
		return FALSE;
	}

	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
	//if (pStx != m_buf)
	if( nPosStx != 0 )
	{
		int nRemoveLen = nPosStx;	// (int)(pStx - m_buf);
		//m_nBufLen -= nRemoveLen;
		MoveData(nPosStx);
		return TRUE;
	}

	// 패킷 총 길이가 헤더보다 작으면 아무것도 안하고 나간다.
	if (m_sBuffer.size() < DEF_HEADER_SIZE)
	{
		return FALSE;
	}


	//sprintf(temp, "%.*s", DEF_PACKETLEN_SIZE, pStx + 5);	// STX134=0080SOH
	string sLen = m_sBuffer.substr(5, 4);	//[0x01]134=
	int nDataLen = atoi(sLen.c_str());
	int nOutBufLen = PACKET_HEADER_SIZE + nDataLen + 1;	// the last 1 is for ETX

	// 미완성 패킷
	if (m_sBuffer.size() < (UINT)nOutBufLen)
	{
		sprintf(m_msg, "Packet hasn't been received all. (Buf Len:%d)(HeaderLen:%d)(PacketLen:%d)", m_sBuffer.size(), PACKET_HEADER_SIZE, nDataLen);
		*pnLen = 0;
		return FALSE;
	}

	// copy one packet
	memcpy(pOutBuf, m_sBuffer.c_str(), nOutBufLen);

	// 패킷 중간에 또 STX 가 있으면 첫번째 블럭이 잘못된 경우. 이전 블럭은 버린다.
	// STX... STX......
	char* p2ndStx = NULL;
	if ((p2ndStx = strchr(pOutBuf + 1, DEF_STX)) > 0)
	{
		int nRemoveLen = (int)(p2ndStx - pOutBuf);
		//m_nBufLen -= nRemoveLen;
		//MoveData(m_buf + nRemoveLen, m_nBufLen);
		MoveData(nRemoveLen);
		*pnLen = 0;
		return TRUE;
	}


	//m_nBufLen -= nOutBufLen;

	*pnLen = nDataLen;

	BOOL bSomthingStillLeft = FALSE;
	if (m_sBuffer.size() == 0)
	{
		//RemoveAll();
		m_sBuffer.clear();
	}
	else if (m_sBuffer.size() > 0)
	{
		// 온전한 하나의 패킷 반환하고 남은 패킷 버퍼 앞으로 이동
		MoveData(nOutBufLen);
		bSomthingStillLeft = TRUE;
	}

	return bSomthingStillLeft;
}



INT CPacketParser::GetOnePacketWithHeader2(int* pnLen, char* pOutBuf)
{
	*pnLen = 0;

	//if (m_nBufLen == 0) {
	if (m_sBuffer.size() == 0) {
		strcpy(m_msg, "No data in the buffer");
		return -1;
	}

	//	find stx
	//char* pStx;;
	//char temp[128];

	//pStx = strchr(m_buf, DEF_STX);
	int nPosStx = m_sBuffer.find_first_of(DEF_STX);
	if (nPosStx == string::npos) {
		strcpy(m_msg, "No STX in the packet");
		//RemoveAll();
		m_sBuffer.clear();
		return -1;
	}

	// stx 가 패킷 중간에 있으면 이전 패킷은 버린다.
	//if (pStx != m_buf)
	if (nPosStx != 0)
	{
		int nRemoveLen = nPosStx;	// (int)(pStx - m_buf);
		//m_nBufLen -= nRemoveLen;
		MoveData(nPosStx);
		return 1;
	}

	// 패킷 총 길이가 헤더보다 작으면 아무것도 안하고 나간다.
	if (m_sBuffer.size() < DEF_HEADER_SIZE)
	{
		return -1;
	}


	//sprintf(temp, "%.*s", DEF_PACKETLEN_SIZE, pStx + 5);	// STX134=0080SOH
	string sLen = m_sBuffer.substr(1 + 3 + 1, 4);
	int nDataLen = atoi(sLen.c_str());
	int nOutBufLen = PACKET_HEADER_SIZE + nDataLen;

	// 미완성 패킷
	if (m_sBuffer.size() < (UINT)nOutBufLen)
	{
		sprintf(m_msg, "Packet hasn't been received all. (Buf Len:%d)(HeaderLen:%d)(PacketLen:%d)", m_sBuffer.size(), PACKET_HEADER_SIZE, nDataLen);
		*pnLen = 0;
		return -1;
	}

	// copy one packet
	memcpy(pOutBuf, m_sBuffer.c_str(), nOutBufLen);

	// 패킷 중간에 또 STX 가 있으면 첫번째 블럭이 잘못된 경우. 이전 블럭은 버린다.
	// STX... STX......
	char* p2ndStx = NULL;
	if ((p2ndStx = strchr(pOutBuf + 1, DEF_STX)) > 0)
	{
		int nRemoveLen = (int)(p2ndStx - pOutBuf);
		//m_nBufLen -= nRemoveLen;
		//MoveData(m_buf + nRemoveLen, m_nBufLen);
		MoveData(nRemoveLen);
		*pnLen = 0;
		return 2;
	}


	//m_nBufLen -= nOutBufLen;

	*pnLen = nDataLen;

	INT nSomthingStillLeft = -1;
	if (m_sBuffer.size() == 0)
	{
		//RemoveAll();
		m_sBuffer.clear();
	}
	else if (m_sBuffer.size() > 0)
	{
		// 온전한 하나의 패킷 반환하고 남은 패킷 버퍼 앞으로 이동
		MoveData(nOutBufLen);
		nSomthingStillLeft = 3;
	}

	return nSomthingStillLeft;
}


void CPacketParser::MoveData(int nPos)
{
	string backup = m_sBuffer.substr(nPos);
	m_sBuffer.clear();
	m_sBuffer = backup;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CPacketParserIOCP::CPacketParserIOCP()
{
	InitializeCriticalSection(&m_cs);
}

CPacketParserIOCP::~CPacketParserIOCP()
{
	map<SOCKET, CPacketParser*>::iterator it;
	Lock();
	for (it = m_mapBuffer.begin(); it != m_mapBuffer.end(); ++it)
		delete (*it).second;

	m_mapBuffer.clear();
	UnLock();

	DeleteCriticalSection(&m_cs);
}

void CPacketParserIOCP::AddSocket(SOCKET sock)
{
	Lock();

	map<SOCKET, CPacketParser*>::iterator it = m_mapBuffer.find(sock);
	if (it != m_mapBuffer.end())
		delete (*it).second;

	CPacketParser* parser = new CPacketParser;
	m_mapBuffer[sock] = parser;

	UnLock();
}

int	CPacketParserIOCP::AddPacket(SOCKET sock, char* pBuf, int nSize)
{
	Lock();

	int nLen = 0;
	CPacketParser* pPacket = NULL;
	map<SOCKET, CPacketParser*>::iterator it = m_mapBuffer.find(sock);
	if (it == m_mapBuffer.end())
	{
		pPacket = new CPacketParser;
	}
	else
	{
		pPacket = (*it).second;
	}

	pPacket->AddPacket(pBuf, nSize);
	nLen = pPacket->GetBuffLen();

	UnLock();

	return nLen;
}

BOOL CPacketParserIOCP::GetOnePacketWithHeader(SOCKET sock, _Out_ int* pnLen, _Out_ char* pOutBuf)
{
	*pnLen = 0;
	*pOutBuf = 0;
	BOOL bResult = FALSE;

	Lock();
	map<SOCKET, CPacketParser*>::iterator it = m_mapBuffer.find(sock);
	if (it == m_mapBuffer.end())
	{
		return bResult;
	}

	CPacketParser* pPacket = (*it).second;

	bResult = pPacket->GetOnePacketWithHeader(pnLen, pOutBuf);


	m_mapBuffer[sock] = pPacket;

	UnLock();

	return bResult;
}