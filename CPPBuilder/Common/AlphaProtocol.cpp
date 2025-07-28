/*
	Packet structure

	STX
	134=0020	// length of Body (without header)
	DEF_DELI
	Body
	ETX
*/

#include <System.StrUtils.hpp>
#include "AlphaProtocol.h"
#include "CTimeUtils.h"
#include "CUtils.h"


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
	string sNow = time.DateTime_yyyymmdd_hhmmssmmmA().c_str();
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

	//TODO
//	for( UINT i=0; i<m_arrInner.size(); i++ )
//	{
//		if(i==0)
//		{
//			sprintf(m_zTemp, "%d=%d%c", FDN_ARRAY_SIZE, m_arrInner.size(), DEF_DELI);
//			m_sData += string(m_zTemp);
//		}
//
//		ST_VAL* p = m_arrInner[i];
//		if( p->isStr()) sprintf(m_zTemp, "%d=%s%c", 	p->nFd, 					p->sVal.c_str(), 	DEF_DELI_RECORD);
//		if( p->isInt()) sprintf(m_zTemp, "%d=%d%c", 	p->nFd, 					p->nVal, 			DEF_DELI_RECORD);
//		if( p->isDbl()) sprintf(m_zTemp, "%d=%.*f%c", 	p->nFd, __ALPHA::DOT_CNT, 	p->dVal, 			DEF_DELI_RECORD);
//
//		m_sData += string(m_zTemp);
//
//		if(i==m_arrInner.size()-1)
//		{
//			sprintf(m_zTemp, "%c", DEF_DELI);
//			m_sData += string(m_zTemp);
//		}
//	}

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

void CProtoSet::SetVal(int nFd, String val)
{
	if (val.Length() == 0)
		return;

	SetVal(nFd, string(AnsiString(val).c_str()));
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

bool CProtoGet::SetValByField(char* pzFd, char* pzVal)
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

	for( UINT i=0; i<m_arrInner.size() ; i++ )
		delete m_arrInner[i];
	m_arrInner.clear();

	m_sOrgData.clear() ;

}




/*
	FIELD=VALUE;FIELD=VALUE;FIELD=VALUE;FIELD=VALUE;
*/
int CProtoGet::Parsing(_In_  char* pRecvData)
{
	// remove ETX
	int nDataLen = strlen(pRecvData);
	if (pRecvData[nDataLen-1]==DEF_ETX) {
		pRecvData[nDataLen-1] = 0x00;
	}

	String sDeli = StringOfChar(AnsiChar(DEF_DELI),1);

	TStringDynArray arr = SplitString(AnsiString(pRecvData), sDeli);

	for( int i=0; i<arr.Length; i++ )
	{
		TStringDynArray oneSet = SplitString(AnsiString(arr[i]), "=");
		if(oneSet.Length < 2 ){
			//Wrong data
			continue;
		}

		AnsiString  fd = AnsiString(oneSet[0]).c_str();
		AnsiString  val =  AnsiString(oneSet[1]).c_str();

		if( fd == FDS_ARRAY_START )
		{
			m_arrIn.Parsing(val);
		}
		else
		{
			SetValByField(fd.c_str() ,val.c_str()  );
        }
	}

	return m_map.size();
}

int CProtoGet::ParsingWithHeader(_In_  char* pRecvData)
{
	Clear();

	SetOrgData(pRecvData);

	char zBuffer[__ALPHA::LEN_BUF];
	strcpy(zBuffer, pRecvData+DEF_HEADER_SIZE);

	return Parsing(zBuffer);
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


bool CProtoGet::GetVal(int nFd, _Out_ AnsiString* psVal)
{
	if (!IsParsed()) return false;

	IT_MAP_DATA it = m_map.find(nFd);
	if (it == m_map.end())
		return false;

	*psVal = AnsiString((*it).second->sVal.c_str());
	return true;
}



bool CProtoGet::GetVal(int nFd, _Out_ long* pnVal)
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


bool CProtoGet::GetVal(int nFd, _Out_ int* pnVal)
{
	*pnVal = 0;

	return (GetVal(nFd, (long*)pnVal));
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CProtoGetArray::CProtoGetArray()
{
}

CProtoGetArray::~CProtoGetArray()
{
}

void CProtoGetArray::Clear()
{
	for( int k=0; k<deq.size(); k++ )
	{
		deq[k].clear();
	}
	deq.clear();
}

//FDS_ARRAY_START=FDS_KEY=XXX(0x05)FDS_BROKER=XXX(0x05)(0x06)FDS_KEY=XXX(0x05)FDS_BROKER=XXX(0x05)(0x06)FDS_ARRAY_END
// Input param =>  FDS_KEY=XXX(0x05)FDS_BROKER=XXX(0x05)(0x06)FDS_KEY=XXX(0x05)FDS_BROKER=XXX(0x05)(0x06)FDS_ARRAY_END
void CProtoGetArray::Parsing(_In_ AnsiString& data)
{
	int dataLen = data.Length();
	char *pData = new char[dataLen+1];
	strcpy(pData, data.c_str());

	assert( pData[dataLen-1]==FDS_ARRAY_END );

	pData[dataLen-1] = 0x00;	// remove FDS_ARRAY_END
	dataLen -= 1;

	if(pData[dataLen-1]!=DEF_DELI_RECORD )
	   pData[dataLen-1] = DEF_DELI_RECORD;


	String sDeliRecord = StringOfChar(AnsiChar(DEF_DELI_RECORD),1);
	String sDeliColumn = StringOfChar(AnsiChar(DEF_DELI_COLUMN),1);

	TStringDynArray arrData = SplitString(AnsiString(pData), sDeliRecord);

	for( int r=0; r<arrData.High; r++ )   // row
	{
		MAP_VALUE mapVal;
		for( int c=0; c<arrData.Low; c++ ) // column
		{
			TStringDynArray oneSet = SplitString(AnsiString(arrData[r][c]), "=");
			assert(oneSet.Length==2);

			AnsiString  fd 	= AnsiString(oneSet[0]).c_str();
			AnsiString  val = AnsiString(oneSet[1]).c_str();

			mapVal[fd.ToInt()] = val;
		}

		deq.push_back(mapVal);
	}
}


bool	CProtoGetArray::GetVal(int idx, int nFd, _Out_ AnsiString* psVal)
{
	if( idx > deq.size() )
		return false;

	MAP_VALUE mapVal = deq[idx];
	IT_MAP_VALUE it = mapVal.find(nFd);
	if(it==mapVal.end() )
		return false;

	*psVal = (*it).second;
	return true;
}

bool	CProtoGetArray::GetVal(int idx, int nFd, _Out_ char* pzVal)
{
	AnsiString sVal;
	if( !GetVal(idx, nFd, &sVal ) )
		return false;

	strcpy(pzVal, sVal.c_str() );
	return true;
}


bool	CProtoGetArray::GetVal(int idx, int nFd, _Out_ int* pnVal)
{
	if( idx > deq.size() )
		return false;

	MAP_VALUE mapVal = deq[idx];
	IT_MAP_VALUE it = mapVal.find(nFd);
	if(it==mapVal.end() )
		return false;

	*pnVal = (*it).second.ToInt();
	return true;
}

bool	CProtoGetArray::GetVal(int idx, int nFd, _Out_ double* pdVal)
{
	if( idx > deq.size() )
		return false;

	MAP_VALUE mapVal = deq[idx];
	IT_MAP_VALUE it = mapVal.find(nFd);
	if(it==mapVal.end() )
		return false;

	*pdVal = (*it).second.ToDouble();
	return true;
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

bool CProtoUtils::GetCode(_In_ char* pzRecvData, _Out_ char* pzPacketCode)
{
	char zField[32];
	sprintf(zField, "%d=", FDS_CODE);


	char* pFind = strstr(pzRecvData, zField);
	if (pFind == NULL)
		return false;

	sprintf(pzPacketCode, "%.*s", __ALPHA::LEN_CODE_SIZE, (pFind + 4));	// 101=

	return true;
}


bool CProtoUtils::GetCommandCode(_In_ char* pzRecvData, _Out_ char* pzCode)
{
	char zField[32];
	sprintf(zField, "%d=", FDS_COMMAND_CODE);   //102


	char* pFind = strstr(pzRecvData, zField);
	if (pFind == NULL)
		return false;

	sprintf(pzCode, "%.*s", COMMAND_CODE_SIZE, (pFind + 4));	// 102=

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
	if(!pInBuf)
		return false;

	__try
	{
		try{
			Lock();
			m_sBuffer += string(pInBuf, nSize);
		}
		catch(...)
		{
			Unlock();
			m_sMsg = L"Exception while Add";
			return false;
		}
	}
	__finally
	{
		Unlock();
	}
	return true;
}

bool CPacketBuffer::GetOnePacketLock(int* pnTotalLen, char* pOutBuf)
{
	bool bRemain;
	Lock();
	__try
	{
		bRemain	= GetOnePacket(pnTotalLen, pOutBuf);
	}
	__finally
	{
		Unlock();
	}
	return bRemain;
}

/*
	return true : buffer has more packet
*/
bool CPacketBuffer::GetOnePacket(int* pnTotalLen, char* pOutBuf)
{
	*pnTotalLen = 0;

	if (m_sBuffer.size() == 0) {
		return false;
	}

	//	find stx
	//char* pStx;;
	//char temp[128];

	//pStx = strchr(m_buf, DEF_STX);
	UINT nPosStx = m_sBuffer.find_first_of(DEF_STX);
	if (nPosStx == string::npos) {
		m_sMsg = L"No STX in the packet";
		m_sBuffer.clear();
		return false;
	}

	// if STX is not the first byte, discard the packet before STX
	if( nPosStx != 0 )
	{
		int nRemoveLen = nPosStx;	// (int)(pStx - m_buf);
		//m_nBufLen -= nRemoveLen;
		MoveData(nPosStx);
		return true;
	}

	// ��Ŷ �� ���̰� ������� ������ �ƹ��͵� ���ϰ� ������.
	// If the length of the packet is shorter than Header Size (10), do nothing .
	if (m_sBuffer.size() < DEF_HEADER_SIZE)
	{
		return false;
	}


	string sLen 	= m_sBuffer.substr(5, 4);	//[0x01]134=
	int nDataLen 	= atoi(sLen.c_str());
	int nOutBufLen	= DEF_HEADER_SIZE + nDataLen + 1;	// last 1 is for ETX

	// non-completed packet
	if (m_sBuffer.size() < (UINT)nOutBufLen)
	{
		m_sMsg = m_sMsg.sprintf(L"Packet hasn't been received all. (Buf Len:%d)(HeaderLen:%d)(PacketLen:%d)",
								m_sBuffer.size(), DEF_HEADER_SIZE, nDataLen);
		*pnTotalLen = 0;
		return false;
	}

	// copy one packet
	memcpy(pOutBuf, m_sBuffer.c_str(), nOutBufLen);
	//

	// ��Ŷ �߰��� �� STX �� ������ ù��° ���� �߸��� ���. ���� ���� ������.
	// If the packet has another STX, discard the data before 2nd STX
	// STX... STX......
	char* p2ndStx = NULL;
	if ((p2ndStx = strchr(pOutBuf + 1, DEF_STX)) > 0)
	{
		int nRemoveLen = (int)(p2ndStx - pOutBuf);
		//m_nBufLen -= nRemoveLen;
		//MoveData(m_buf + nRemoveLen, m_nBufLen);
		MoveData(nRemoveLen);
		*pnTotalLen = 0;
		return true;
	}


	//m_nBufLen -= nOutBufLen;

	*pnTotalLen = nOutBufLen;

	BOOL bSomthingStillLeft = false;
	if (m_sBuffer.size() == 0)
	{
		m_sBuffer.clear();
	}
	else if (m_sBuffer.size() > 0)
	{
		// ������ �ϳ��� ��Ŷ ��ȯ�ϰ� ���� ��Ŷ ���� ������ �̵�
		MoveData(nOutBufLen);
		bSomthingStillLeft = true;
	}

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



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


