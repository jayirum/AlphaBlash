/*
	Packet structure

	STX
	134=0020	// length of Body (without header)
	DEF_DELI
	Body
	ETX
*/


#include "AlphaProtocol.h"
#include "TimeUtils.h"
#include "Util.h"
#include "StringUtils.h"

///////////////////////////////////////////////////////////////////////////////
// CProtoSet
//
CProtoSet::CProtoSet()
{
	InitializeCriticalSection(&m_cs) ;
}
CProtoSet::~CProtoSet()
{
	Clear();
	DeleteCriticalSection(&m_cs);
}


void CProtoSet::Clear()
{
	Lock();
	IT_MAP_DATA it;
	for(it=m_map.begin(); it!=m_map.end();it++ )
	{
		delete (*it).second;
	}
	m_map.clear();
	Unlock();

	for( UINT i=0; i<m_arrInner.size() ; i++ )
		delete m_arrInner[i];
	m_arrInner.clear();

	m_sData.clear();

	m_bSetSuccYN = false;
}

void CProtoSet::Begin()
{
	Clear();

	CTimeUtils time;
	char zTime[128] = { 0 };
	string sNow = time.DateTime_yyyymmdd_hhmmssmmm();
	SetVal(FDS_TM_HEADER, sNow);
}

int CProtoSet::Complete(/*out*/string& result, bool bForDelphi/*=FALSE*/)
{
	char zResult[__ALPHA::LEN_BUF] = { 0 };
	int nRes = Complete(zResult, bForDelphi);
	if (nRes > 0)
		result = string(zResult);
	return nRes;
}


int CProtoSet::Complete(/*out*/char* pzResult, bool bForDelphi/*=FALSE*/)
{
	// If FDS_SUCC_YN is not set yet, set it as Y
	if (!m_bSetSuccYN)
	{
		SetVal(FDS_SUCC_YN, "Y");
	}

	Lock();
	for(IT_MAP_DATA it=m_map.begin(); it!=m_map.end(); ++it)
	{
		ST_VAL* p = (*it).second;
		if( p->isStr()) sprintf(m_zTemp, "%d=%s%c", p->nFd, p->sVal.c_str(), 	DEF_DELI);
		if( p->isInt()) sprintf(m_zTemp, "%d=%d%c", p->nFd, p->nVal, 			DEF_DELI);
		if( p->isDbl()) sprintf(m_zTemp, "%d=%.*f%c", p->nFd, __ALPHA::DOT_CNT, p->dVal, DEF_DELI);

		m_sData += string(m_zTemp);
	}
	Unlock();

	for( UINT i=0; i<m_arrInner.size(); i++ )
	{
		if(i==0)
		{
			sprintf(m_zTemp, "%d=%d%c", FDN_ARRAY_SIZE, m_arrInner.size(), DEF_DELI);
			m_sData += string(m_zTemp);
		}

		ST_VAL* p = m_arrInner[i];
		if( p->isStr()) sprintf(m_zTemp, "%d=%s%c", 	p->nFd, 					p->sVal.c_str(), 	DEF_DELI_RECORD);
		if( p->isInt()) sprintf(m_zTemp, "%d=%d%c", 	p->nFd, 					p->nVal, 			DEF_DELI_RECORD);
		if( p->isDbl()) sprintf(m_zTemp, "%d=%.*f%c", 	p->nFd, __ALPHA::DOT_CNT, 	p->dVal, 			DEF_DELI_RECORD);

		m_sData += string(m_zTemp);

		if(i==m_arrInner.size()-1)
		{
			sprintf(m_zTemp, "%c", DEF_DELI);
			m_sData += string(m_zTemp);
		}
	}

	sprintf(pzResult, "%c%d=%0*d%c%s%c",
		DEF_STX,
		FDS_PACK_LEN,
		DEF_PACKETLEN_SIZE,
		m_sData.size(),	// Data length
		DEF_DELI,
		m_sData.c_str(),
		DEF_ETX
		);

	Clear();

	return strlen(pzResult);
}



void CProtoSet::SetVal(int nFd, string val)
{
	if (val.empty())
		return;

	ST_VAL* p 	= new ST_VAL;
	p->nFd		= nFd;
	p->sVal 	= val;
	p->enField  = FIELD_STR;

	if (nFd == FDS_SUCC_YN)
		m_bSetSuccYN = "Y";

	Lock();
	m_map[p->nFd] = p;
	Unlock();
}

void CProtoSet::SetVal(int nFd, char* val)
{
	if (val == NULL)
		return;

	if (strlen(val) == 0)
		return;

	SetVal(nFd, string(val));
}


void CProtoSet::SetVal(int nFd, char val)
{
	char z[2]; sprintf(z, "%c",val);
	string s = string(z);
	SetVal(nFd, s);
}


void CProtoSet::SetVal(int nFd, int val)
{
	ST_VAL* p 	= new ST_VAL;
	p->nFd		= nFd;
	p->nVal 	= val;
	p->enField  = FIELD_INT;

	Lock();
	m_map[nFd] = p;
	Unlock();
}

void CProtoSet::SetVal(int nFd, double val)
{
	ST_VAL* p 	= new ST_VAL;
	p->nFd		= nFd;
	p->dVal 	= val;
	p->enField  = FIELD_DBL;

	Lock();
	m_map[nFd] = p;
	Unlock();
}


void CProtoSet::SetInnerArrayVal(int nFd, string val)
{
	if (val.empty())
		return;

	ST_VAL *p 	= new ST_VAL;
	p->nFd 		= nFd;
	p->sVal     = val;
	p->enField  = FIELD_STR;

	m_arrInner.push_back(p);
}

void CProtoSet::SetInnerArrayVal(int nFd, char* val)
{
	SetInnerArrayVal(nFd, string(val));
}


void CProtoSet::SetInnerArrayVal(int nFd, int val)
{
	ST_VAL* p 	= new ST_VAL;
	p->nFd		= nFd;
	p->nVal 	= val;
	p->enField  = FIELD_INT;

	m_arrInner.push_back(p);
}


void CProtoSet::SetInnerArrayVal(int nFd, double val)
{
	ST_VAL* p 	= new ST_VAL;
	p->nFd		= nFd;
	p->dVal 	= val;
	p->enField  = FIELD_DBL;

	m_arrInner.push_back(p);
}





//int CProtoSet::NormalToDelphi(_InOut_ string& sPacket)
//{
//	sPacket += DEF_ENTER;
//
//	int nNewDataLen = sPacket.size() - DEF_HEADER_SIZE;
//
//	sprintf(m_zTemp, "%c%d=%0*d%c%s",
//		DEF_STX,
//		FDS_PACK_LEN,
//		DEF_PACKETLEN_SIZE,
//		nNewDataLen,
//		DEF_DELI,
//		sPacket.substr(DEF_HEADER_SIZE, nNewDataLen).c_str()
//	);
//	sPacket.clear();
//	sPacket = m_zTemp;
//	return sPacket.size();
//}
//
//int CProtoSet::DelphiToNormal(_InOut_ string& sPacket)
//{
//	if (sPacket[sPacket.size() - 1] != DEF_ENTER)
//		return sPacket.size();
//
//	int nNewDataLen = sPacket.size() - 1 - DEF_HEADER_SIZE;
//
//	sprintf(m_zTemp, "%c%d=%0*d%c%s",
//		DEF_STX,
//		FDS_PACK_LEN,
//		DEF_PACKETLEN_SIZE,
//		nNewDataLen,
//		DEF_DELI,
//		sPacket.substr(DEF_HEADER_SIZE, nNewDataLen).c_str()
//	);
//	sPacket.clear();
//	sPacket = m_zTemp;
//	return sPacket.size();
//}
//
//
//
//void CProtoSet::CopyFromRecvData(char* pzData)
//{
//	m_sData = pzData;
//}
//

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
{
	InitializeCriticalSection(&m_cs) ;
}

CProtoGet::CProtoGet(const CProtoGet& rhs)
{
	// copy the original data
	m_sOrgData = rhs.m_sOrgData;
	m_map.clear();

	// deep copy each value in the map
	for (const auto& pair : rhs.m_map)
	{
		int nFd = pair.first;
		const ST_VAL* pOtherVal = pair.second;
		ST_VAL* pNewVal = new ST_VAL(*pOtherVal); // create a new ST_VAL object using its copy constructor
		m_map[nFd] = pNewVal; // add the new object to the map
	}
}


CProtoGet::~CProtoGet()
{
	Clear();
	DeleteCriticalSection(&m_cs);
}



EN_FIELD 	CProtoGet::GetFieldType(int nField)
{
	EN_FIELD 	nFieldType;
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

bool CProtoGet::SetValByField(const char* pzFd, const  char* pzVal)
{
	bool bRes = true;
	int nFd = atoi(pzFd);

	EN_FIELD nFieldTp = GetFieldType(nFd);

	ST_VAL *pVal 	= new ST_VAL;
	pVal->nFd 		= nFd;
	pVal->enField 	= nFieldTp;

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

	if(bRes)
	{
		Lock();
		m_map[pVal->nFd] = pVal;
		Unlock();
	}

	return bRes;
}


void CProtoGet::Clear()
{
	Lock();
	IT_MAP_DATA it;
	for(it=m_map.begin(); it!=m_map.end();it++ )
	{
		delete (*it).second;
	}
	m_map.clear();
	Unlock();

	//for( UINT i=0; i<m_arrInner.size() ; i++ )
	//	delete m_arrInner[i];
	//m_arrInner.clear();

	m_sOrgData.clear();
}




/*
	FIELD=VALUE;FIELD=VALUE;FIELD=VALUE;FIELD=VALUE;
*/
int CProtoGet::Parsing(_In_  char* pRecvData)	//, _Out_ int* pnInnerArrCnt)
{
	//*pnInnerArrCnt  = 0;

	// remove ETX
	int nDataLen = strlen(pRecvData);
	if (pRecvData[nDataLen-1]==DEF_ETX) {
		pRecvData[nDataLen-1] = 0x00;
	}

	CSplitString split;
	int nCnt = split.Split(pRecvData, DEF_DELI);
	if (nCnt == 0)
		return 0;


	for( int i=0; i<split.size(); i++ )
	{
		CSplitString oneSet;
		oneSet.Split(split.at(i).c_str(), '=');
		
		if(oneSet.size() < 2 ){
			//Wrong data
			continue;
		}

		SetValByField(oneSet.at(0).c_str(), oneSet.at(1).c_str());

		//  506=3;121=OANDA0x06121=FXOPEN0x06121=ICMARKET0x06
		// TODO. INNER ARRAY
	}

	return m_map.size();
}

/*
[stx]134 = 0010[deli]112 = 123456[deli][etx]
total : 22
header size : 10
nPosStx = 0
nPosEtx = 21
datalen = 11 = 22-10-1
*/
int CProtoGet::ParsingWithHeader(_In_  char* pRecvData)	//, _Out_ int* pnInnerArrCnt)
{
	Clear();

	SetOrgData(pRecvData);

	char zBuffer[__ALPHA::LEN_BUF];
	
	strcpy(zBuffer, pRecvData+DEF_HEADER_SIZE);
	
	// remove ETX
	int nDataLen = strlen(zBuffer);
	if (pRecvData[nDataLen - 1] == DEF_ETX) {
		pRecvData[nDataLen - 1] = 0x00;
	}
	return Parsing(zBuffer);	// , pnInnerArrCnt);
}


bool CProtoGet::IsParsed()
{
	return (m_map.size() > 0);
}

bool CProtoGet::GetCode(_Out_ string& sCode)
{
	if (!IsParsed())
		return false;

	IT_MAP_DATA it = m_map.find(FDS_CODE);
	if (it == m_map.end())
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

bool CProtoGet::GetVal(int nFd, _Out_ char* pzVal)
{
	*pzVal = 0x00;

	if (!IsParsed())
		return false;

	IT_MAP_DATA it = m_map.find(nFd);
	if (it == m_map.end())
		return false;

	strcpy(pzVal, (*it).second->sVal.c_str());
	return true;
}


bool CProtoGet::GetVal(int nFd, _Out_ string* psVal)
{
	if (!IsParsed()) return false;

	IT_MAP_DATA it = m_map.find(nFd);
	if (it == m_map.end())
		return false;

	*psVal = (*it).second->sVal;
	return true;
}

bool CProtoGet::GetVal(int nFd, _Out_ int* pnVal)
{
	*pnVal = 0;

	if (!IsParsed())
		return false;
	IT_MAP_DATA it = m_map.find(nFd);
	if (it == m_map.end())
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

	IT_MAP_DATA it = m_map.find(nFd);
	if (it == m_map.end())
		return false;

	*pdVal = (*it).second->dVal;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CProtoGetList::CProtoGetList()
{
	InitializeCriticalSection(&m_cs);
}
CProtoGetList::~CProtoGetList()
{
	DeleteCriticalSection(&m_cs);
}


void CProtoGetList::Add(CProtoGet& get)
{
	EnterCriticalSection(&m_cs);
	m_lstGet.push_back(get);
	LeaveCriticalSection(&m_cs);
}

bool CProtoGetList::Get(_Out_ CProtoGet& get)
{
	bool bExists = false;
	EnterCriticalSection(&m_cs);
	if (!m_lstGet.empty())
	{
		get = *m_lstGet.begin();
	}
	LeaveCriticalSection(&m_cs);
	return bExists;
}

void CProtoGetList::Clear()
{
	EnterCriticalSection(&m_cs);
	m_lstGet.clear();
	LeaveCriticalSection(&m_cs);
}

bool CProtoGetList::IsEmpty()
{
	bool bEmpty = false;
	EnterCriticalSection(&m_cs);
	bEmpty = m_lstGet.empty();
	LeaveCriticalSection(&m_cs);
	return bEmpty;
}

int CProtoGetList::GetListSize()
{
	int size = 0;
	EnterCriticalSection(&m_cs);
	size = m_lstGet.size();
	LeaveCriticalSection(&m_cs);
	return size;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* CProtoUtils::PacketCode(_In_ char* pzRecvData, _Out_ char* pzPacketCode)
{
	char zField[32];
	sprintf(zField, "%d=", FDS_CODE);


	char* pFind = strstr(pzRecvData, zField);
	if (pFind == NULL)
		return NULL;

	sprintf(pzPacketCode, "%.*s", __ALPHA::LEN_CODE_SIZE, (pFind + 4));	// 101=

	return pzPacketCode;
}

bool CProtoUtils::GetUserId(char* pzRecvData, _Out_ string& sUserId)
{
	char val[128] = { 0 };
	CStringCvt cvt;
	bool ret = GetValue(pzRecvData, cvt.Int2Str(FDS_USER_ID), val);
	sUserId = string(val);
	return ret;
}

bool CProtoUtils::GetValue(char* pzRecvData, char* pzField, char* pzVal)
{
	char zField[__ALPHA::FIELD_LEN];
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


bool  CProtoUtils::IsSuccess(_In_ char* pzRecvData)
{
	char zField[32];
	sprintf(zField, "%c%d=", DEF_DELI, FDS_SUCC_YN);

	char* pFind = strstr(pzRecvData, zField);
	if (pFind == NULL)
		return NULL;

	char zSuccYN[32] = { 0 };
	sprintf(zSuccYN, "%.1s", (pFind + 5));	// 101=

	return (zSuccYN[0] == 'Y');
}

bool  CProtoUtils::Is_JustRelay(_In_ char* pzRecvData)
{
	char zField[32];
	sprintf(zField, "%c%d=", DEF_DELI, FDS_JUSTRELAY_YN);

	char* pFind = strstr(pzRecvData, zField);
	if (pFind == NULL)
		return false;

	char zYN[32] = { 0 };
	sprintf(zYN, "%.1s", (pFind + 5));	// 101=

	return (zYN[0] == 'Y');
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



CPacketBuffer::CPacketBuffer()
{
	InitializeCriticalSectionAndSpinCount(&m_cs, 1000);
}

CPacketBuffer::~CPacketBuffer()
{
	RemoveAll(true);
	DeleteCriticalSection(&m_cs);
}

bool CPacketBuffer::Add(_In_ char* pInBuf, int nSize)
{
	if (!pInBuf)
		return false;

	bool bRet = true;
	Lock();

	try {
		m_sBuffer += string(pInBuf, nSize);
	}
	catch (...)
	{
		Unlock();
		m_sMsg = "Exception while Add";
		bRet = false;
	}
	Unlock();

	return bRet;
}

bool CPacketBuffer::GetOnePacketLock(int* pnLen, char* pOutBuf)
{
	bool bRemain;
	Lock();
	__try
	{
		bRemain	= GetOnePacket(pnLen, pOutBuf);
	}
	__finally
	{
		Unlock();
	}
	return bRemain;
}

/*
	return true : buffer has more packet

	Just check STX & ETX
*/

bool CPacketBuffer::GetOnePacket(_Out_ int* pnLen, _Out_ char* pOutBuf)
{
	*pnLen = 0;

	if (m_sBuffer.size() == 0) {
		return false;
	}

	//pStx = strchr(m_buf, DEF_STX);
	int nPosStx = m_sBuffer.find_first_of(DEF_STX);
	if (nPosStx == string::npos) {
		m_sMsg = "No STX in the packet";
		m_sBuffer.clear();
		return false;
	}

	// if STX is not the first byte, discard the packet before STX
	if (nPosStx != 0)
	{
		MoveData(nPosStx);
		return true;
	}


	// find ETX
	int nPosEtx = m_sBuffer.find_first_of(DEF_ETX, nPosStx);
	if (nPosEtx == string::npos) {
		m_sMsg = "No ETX in the packet";
		return false;
	}

	/*
	[stx]134=0010[deli]112=123456[deli][etx]
	total : 22
	header size : 10
	nPosStx = 0
	nPosEtx = 21 
	*/

	int nOnePacketSize = nPosEtx + 1;
	*pnLen = nOnePacketSize;
	memcpy(pOutBuf, m_sBuffer.c_str(), nOnePacketSize);

	MoveData(nOnePacketSize);;

	BOOL bSomthingStillLeft = false;
	if (m_sBuffer.size() > 0)
		bSomthingStillLeft = true;

	return bSomthingStillLeft;
}




void CPacketBuffer::RemoveAll(bool bLock)
{
	if (bLock)	Lock();
	m_sBuffer.erase(m_sBuffer.begin(), m_sBuffer.end());
	if (bLock)	Unlock();
}



void  CPacketBuffer::Erase(int nStartPos, int nLen)
{
	m_sBuffer.erase( nStartPos, nLen );

}


void CPacketBuffer::MoveData(int nPos)
{
	string backup = m_sBuffer.substr(nPos);
	m_sBuffer.clear();
	m_sBuffer = backup;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CPacketBufferIocp::CPacketBufferIocp()
{
	InitializeCriticalSection(&m_cs);
}

CPacketBufferIocp::~CPacketBufferIocp()
{
	map<SOCKET, CPacketBuffer*>::iterator it;
	Lock();
	for (it = m_mapBuffer.begin(); it != m_mapBuffer.end(); ++it)
		delete (*it).second;

	m_mapBuffer.clear();
	UnLock();

	DeleteCriticalSection(&m_cs);
}

void CPacketBufferIocp::AddSocket(SOCKET sock)
{
	Lock();

	map<SOCKET, CPacketBuffer*>::iterator it = m_mapBuffer.find(sock);
	if (it != m_mapBuffer.end())
		delete (*it).second;

	CPacketBuffer* parser = new CPacketBuffer;
	m_mapBuffer[sock] = parser;

	UnLock();
}

int	CPacketBufferIocp::AddPacket(SOCKET sock, char* pBuf, int nSize)
{
	Lock();

	int nLen = 0;
	CPacketBuffer* pPacket = NULL;
	map<SOCKET, CPacketBuffer*>::iterator it = m_mapBuffer.find(sock);
	if (it == m_mapBuffer.end())
	{
		pPacket = new CPacketBuffer;
	}
	else
	{
		pPacket = (*it).second;
	}

	pPacket->Add(pBuf, nSize);
	nLen = pPacket->GetBuffLen();

	UnLock();

	return nLen;
}

BOOL CPacketBufferIocp::GetOnePacket(SOCKET sock, _Out_ int* pnLen, _Out_ char* pOutBuf)
{
	*pnLen = 0;
	*pOutBuf = 0;
	BOOL bResult = FALSE;

	Lock();
	map<SOCKET, CPacketBuffer*>::iterator it = m_mapBuffer.find(sock);
	if (it == m_mapBuffer.end())
	{
		return bResult;
	}

	CPacketBuffer* pPacket = (*it).second;

	bResult = pPacket->GetOnePacket(pnLen, pOutBuf);


	m_mapBuffer[sock] = pPacket;

	UnLock();

	return bResult;
}