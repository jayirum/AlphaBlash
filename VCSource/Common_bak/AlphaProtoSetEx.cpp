#include "AlphaProtoSetEx.h"
#include "util.h"
//#include "Log.h"
//extern CLog	g_log;

CProtoSetEx::CProtoSetEx()
{}
CProtoSetEx::~CProtoSetEx()
{}

void CProtoSetEx::Begin()
{
	m_map.clear();
	m_lstArray.clear();
}

int CProtoSetEx::Complete(/*out*/string& result)
{
	string sData;

	map<PROTO_KEY, PROTO_VAL>::iterator it;
	for (it = m_map.begin(); it != m_map.end(); it++)
	{
		sData += (*it).second;
	}

	if (m_lstArray.size() > 0)
	{
		sprintf(m_zTemp, "%d=%d%c", FDN_ARRAY_SIZE, m_lstArray.size(), DEF_DELI);
		sData += string(m_zTemp);

		list<string>::iterator itList;
		for (itList = m_lstArray.begin(); itList != m_lstArray.end(); itList++)
		{
			sData += *itList;
		}
	}

	if (m_lstRecord.size() > 0)
	{
		sprintf(m_zTemp, "%d=%d%c%d=", FDN_RECORD_CNT, m_lstRecord.size(), DEF_DELI, FDS_DATA);
		sData += string(m_zTemp);

		list<string>::iterator itRecord;
		for (itRecord = m_lstRecord.begin(); itRecord != m_lstRecord.end(); itRecord++)
		{
			sData += *itRecord;
		}
	}

	char zFull[MAX_BUF] = { 0 };
	sprintf(zFull, "%c%d=%0*d%c%s", DEF_STX, FDS_PACK_LEN, DEF_PACKETLEN_SIZE, sData.size(), DEF_DELI, sData.c_str());
	if (zFull[strlen(zFull) - 1] != DEF_DELI)
		zFull[strlen(zFull)] = DEF_DELI;

	result = zFull;
	return result.size();
}
int CProtoSetEx::Complete(/*out*/char* pzResult)
{
	string sData;
	Complete(sData);
	strcpy(pzResult, sData.c_str());
	return sData.size();
}


void CProtoSetEx::CopyFromRecvData(char* pzData, int nRecvLen)
{
	CProtoGet get;
	get.Parsing(pzData, nRecvLen);
	get.CopyData(&m_map);
}


void CProtoSetEx::SetVal(int nFd, string val)
{
	sprintf(m_zTemp, "%d=%s%c", nFd, val.c_str(), DEF_DELI);
	m_map[nFd] = string(m_zTemp);
}


void CProtoSetEx::SetVal(int nFd, _tstring val)
{
	char zVal[512] = { 0, };
	wchar_t wzVal[512] = { 0, };
	_tcscpy(wzVal, val.c_str());
	U2A(wzVal, zVal);
	
	sprintf(m_zTemp, "%d=%s%c", nFd, zVal, DEF_DELI);
	m_map[nFd] = string(m_zTemp);
}

void CProtoSetEx::SetVal(int nFd, char* val)
{
	if (strlen(val) == 0)
		return;
	
	sprintf(m_zTemp, "%d=%s%c", nFd, val, DEF_DELI);
	m_map[nFd] = string(m_zTemp);
}

void CProtoSetEx::SetVal(int nFd, wchar_t* val)
{
	char zVal[512] = { 0, };
	U2A(val, zVal);

	if (strlen(zVal) == 0)
		return;

	sprintf(m_zTemp, "%d=%s%c", nFd, zVal, DEF_DELI);
	m_map[nFd] = string(m_zTemp);
}


void CProtoSetEx::SetVal(int nFd, char val)
{
	sprintf(m_zTemp, "%d=%c%c", nFd, val, DEF_DELI);
	m_map[nFd] = string(m_zTemp);
}


void CProtoSetEx::SetVal(int nFd, int val)
{
	sprintf(m_zTemp, "%d=%d%c", nFd, val, DEF_DELI);
	m_map[nFd] = string(m_zTemp);
}
void CProtoSetEx::SetVal(int nFd, double val)
{
	sprintf(m_zTemp, "%d=%.*f%c", nFd, DOT_CNT, val, DEF_DELI);
	m_map[nFd] = string(m_zTemp);
}



/*
	111(FDN_ARRAY_SIZE)=5 0x01
	22(FDS_USER_NICK_NM)=탑트레이더 0x1F 22=단타 0x1F
	0x01
	
*/
void CProtoSetEx::SetArrayStart()
{
	//sprintf(m_zTemp, "%d=%d%c", FDN_ARRAY_SIZE, nArraySize, DEF_DELI);
	//m_map[FDN_ARRAY_SIZE] = string(m_zTemp);

	m_lstArray.clear();
}

void CProtoSetEx::SetArrayVal(int nFd, const char* val)
{
	if (strlen(val) == 0)
		return;

	sprintf(m_zTemp, "%d=%s%c", nFd, val, DEF_DELI_ARRAY);
	m_lstArray.push_back(string(m_zTemp));
}

void CProtoSetEx::SetArrayEnd()
{
	char zDeli[2]; sprintf(zDeli, "%c", DEF_DELI);
	m_lstArray.push_back(zDeli);
}


void CProtoSetEx::RecordStart()
{
	m_lstRecord.clear();
	m_sRow.clear();
}

void CProtoSetEx::RowStart()
{
	m_sRow.clear();
}

void CProtoSetEx::SetRecordVal(wchar_t* colNm, wchar_t* val)
{
	char zCol[128] = { 0 }, zVal[512] = { 0, };
	U2A(colNm, zCol);
	U2A(val, zVal);

	sprintf(m_zTemp, "%s=%s%c", zCol, zVal, DEF_DELI_COLUMN);
	m_sRow += string(m_zTemp);
}

void CProtoSetEx::SetRecordVal(wchar_t* colNm, long val)
{
	char zCol[128] = { 0 };
	U2A(colNm, zCol);

	sprintf(m_zTemp, "%s=%d%c", zCol, val, DEF_DELI_COLUMN);
	m_sRow += string(m_zTemp);
}

void CProtoSetEx::SetRecordVal(wchar_t* colNm, double val)
{
	char zCol[128] = { 0 };
	U2A(colNm, zCol);

	sprintf(m_zTemp, "%s=%f%c", zCol, val, DEF_DELI_COLUMN);	// 0X13
	m_sRow += string(m_zTemp);
}

void CProtoSetEx::RowEnd()
{	
	sprintf(m_zTemp, "%c", DEF_DELI_ARRAY);	// 0X14
	m_sRow += string(m_zTemp);

	m_lstRecord.push_back(m_sRow);
	m_sRow.clear();
}

void CProtoSetEx::SetRecordEnd()
{
	sprintf(m_zTemp, "%c", DEF_DELI);	// 0X01
	m_lstRecord.push_back(m_zTemp);
}