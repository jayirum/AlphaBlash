#pragma once
#pragma warning(disable:4996)
#include "AlphaInc.h"
#include "AlphaProtocolUni.h"


#include "IRUM_Common.h"
#include <map>
#include <string>
#include <list>
using namespace std;


#define PROTO_KEY	int
#define PROTO_VAL	string

class CProtoSetEx
{
public:
	CProtoSetEx();
	~CProtoSetEx();

	void Begin();
	int Complete(/*out*/string& result);
	int Complete(/*out*/char* pzResult);

	void CopyFromRecvData(char* pzData, int nRecvLen);
	void SetVal(int nFd, string val);
	void SetVal(int nFd, _tstring val);
	void SetVal(int nFd, char* val);
	void SetVal(int nFd, wchar_t* val);
	void SetVal(int nFd, char val);
	void SetVal(int nFd, int val);
	void SetVal(int nFd, double val);
	
	/*
	[FDN_ARRAY_SIZE=3]0x01[Fd=Val]0x14[Fd=Val]0x14[Fd=Val]0x14 0x01
	*/
	void SetArrayStart();
	void SetArrayVal(int nFd, const char* val);
	void SetArrayEnd();

	/*
	[FDN_RECORD_SIZE=3]0x01[USER_ID=JAY]0x13[USER_NM=KIM]0x13[ACC=1234]0x13 0x14
						   [USER_ID=KEN]0x13[USER_NM=LEE]0x13[ACC=456]0x13 0x14
						   [USER_ID=YOU]0x13[USER_NM=CHO]0x13[ACC=789]0x13 0x14 0x01
	*/
	void RecordStart();
	void RowStart();
	void SetRecordVal(wchar_t* colNm, wchar_t* val);
	void SetRecordVal(wchar_t* colNm, long val);
	void SetRecordVal(wchar_t* colNm, double val);
	void RowEnd();
	void SetRecordEnd();
//private:
	//void DelSameField(int nFd);
private:
	map< PROTO_KEY, PROTO_VAL>	m_map;
	list<string>				m_lstArray;

	string			m_sRow;
	list< string >	m_lstRecord;
	char			m_zTemp[MAX_BUF];
};
