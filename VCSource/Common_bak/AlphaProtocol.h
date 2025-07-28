#ifndef __ALPHA_PROTOCOL_H__
#define __ALPHA_PROTOCOL_H__


#pragma warning(disable:4996)


#include "AlphaInc.h"
#include <map>
#include <deque>
using namespace std;


/*
	[ Structure of Packet ]
	STX + 134=length + 0x01 + DATA + ETX
	
	134 : DEF_PACK_LEN
	length : length of data  ==> 4byte
	DATA : code=value+0x01   ==> repeat this structure

*/

enum EN_FIELD {FIELD_STR=0, FIELD_INT, FIELD_DBL};
struct ST_VAL
{
	int 		nFd;
	string		sVal;
	int			nVal;
	double		dVal;

	EN_FIELD   	enField;
	bool 		isStr() { return (enField==FIELD_STR); }
	bool 		isInt() { return (enField==FIELD_INT); }
	bool 		isDbl() { return (enField==FIELD_DBL); }
};

typedef map<int, ST_VAL*>			MAP_DATA;
typedef map<int, ST_VAL*>::iterator	IT_MAP_DATA;
typedef deque<ST_VAL*>				DEQ_INNER_ARR;

class CProtoSet
{
public:
	CProtoSet();
	~CProtoSet();

	void Begin();
	int Complete(/*out*/string& result, bool bForDelphi=false);
	int Complete(/*out*/char* pzResult, bool bForDelphi=false);

	void SetVal(int nFd, char* val);
	void SetVal(int nFd, string val);
	void SetVal(int nFd, char val);
	void SetVal(int nFd, int val);
	void SetVal(int nFd, double val);

	void SetInnerArrayVal(int nFd, string val);
	void SetInnerArrayVal(int nFd, char* val);
	void SetInnerArrayVal(int nFd, int val);
	void SetInnerArrayVal(int nFd, double val);

    //	void CopyFromRecvData(char* pzData);
//	int NormalToDelphi(_InOut_ string& sPacket);
//	int DelphiToNormal(_InOut_ string& sPacket);

private:
	void Clear();
	void Lock() { EnterCriticalSection(&m_cs); }
	void Unlock() { LeaveCriticalSection(&m_cs); }
private:
	string	m_sData;
	char	m_zTemp[__ALPHA::LEN_BUF];
	bool	m_bSetSuccYN;

	MAP_DATA			m_map;
	CRITICAL_SECTION	m_cs;

	DEQ_INNER_ARR		m_arrInner;

};
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//


class CProtoGet
{
public:
	CProtoGet();
	~CProtoGet();

	void	Parsing(_In_  char* pRecvData, _Out_ int* pnFieldCnt, _Out_ int* pnInnerArrCnt);
	void	ParsingWithHeader(_In_  char* pRecvData, _Out_ int* pnFieldCnt, _Out_ int* pnInnerArrCnt);
//	//int		ParsingDebug(_In_  const char* pData, int nDataLen, bool bLog = FALSE);
//	char*	GetRecvData() { return m_zBuf; }

	bool	GetCode(_Out_ string& sCode);
	bool	GetVal(int nFd, _Out_ char* pzVal);
	bool	GetVal(int nFd, _Out_ string* psVal);
	bool	GetVal(int nFd, _Out_ int* pnVal);
	bool	GetVal(int nFd, _Out_ double* pdVal);

	int		GetValN(int nFd);
	double	GetValD(int nFd);

	bool	Is_Success();
	int		Get_RsltCode();

//	char* GetMsg() { return m_zMsg; }
private:
	bool 		SetValByField(char* pzFd, char* pzVal);
	EN_FIELD 	GetFieldType(int nField);

	bool 		IsParsed();
	void 		Clear();
	void 		Lock(){ EnterCriticalSection(&m_cs); }
	void 		Unlock(){ LeaveCriticalSection(&m_cs); }

private:
	char				m_zMsg[__ALPHA::LEN_BUF];
	MAP_DATA			m_map;
	CRITICAL_SECTION	m_cs;
	DEQ_INNER_ARR		m_arrInner;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
class CProtoUtils
{
public:
	CProtoUtils() {};
	~CProtoUtils() {};

//	bool GetValue(_In_ char* pzRecvData, _In_ char* pzField, char* pzVal);
//	bool GetValue(_In_ char* pzRecvData, _In_ int nField, char* pzVal);
//
//	bool GetSymbol(char* pzRecvData, _Out_ string& sSymbol);
//	bool GetUserId(char* pzRecvData, _Out_ string& sUserId);

	char* PacketCode(_In_ char* pzRecvData, _Out_ char* pzPacketCode);
	bool  IsSuccess(_In_ char* pzRecvData);
private:

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class CPacketBuffer
{
public:
	CPacketBuffer();
	~CPacketBuffer();

	bool	Add(_In_ char* pRecvBuf, int nSize);
	bool	GetOnePacketLock(_Out_ int* pnLen, _Out_ char* pOutBuf);
	bool 	GetOnePacket(_Out_ int* pnLen, _Out_ char* pOutBuf);

	string	GetMsg() { return m_sMsg; }
private:
	void	MoveData(int nPos);
	void	Erase(int nStartPos, int nLen);
	void	RemoveAll(bool bLock);

	void	Lock() { EnterCriticalSection(&m_cs); }
	void	Unlock() { LeaveCriticalSection(&m_cs); }
private:
	CRITICAL_SECTION	m_cs;
	string				m_sBuffer;
	string 				m_sMsg;
};

extern CPacketBuffer __PacketBuffer;


#endif



/////////////////////////////////////////////////////////////////////////
// BACKUP
//class CProtoBuffering
//{
//public:
//	CProtoBuffering();
//	~CProtoBuffering();
//
//	int		AddPacket(char* pBuf, int nSize);
//	bool	GetOnePacket(int* pnLen, char* pOutBuf);
//
//	bool	GetOnePacketWithHeader(int* pnLen, char* pOutBuf);
//
//	char* GetErrMsg() { return m_msg; }
//private:
//	//int		GetOnePacketFn(int* pnLen, char* pOutBuf);
//	int		GetOnePacketInner(int* pnLen, char* pOutBuf);
//	void	MoveData(char* pStart, int nCopySize);
//
//	//void	Erase(int nStartPos, int nLen);
//	void	RemoveAll();
//
//	void	Lock() { EnterCriticalSection(&m_cs); }
//	void	Unlock() { LeaveCriticalSection(&m_cs); }
//
//
//private:
//	CRITICAL_SECTION	m_cs;
//	char				m_buf[MAX_BUFFERING] ;
//	char				m_msg[1024];
//	int					m_nBufLen;
//