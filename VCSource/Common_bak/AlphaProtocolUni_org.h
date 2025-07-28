#pragma once
#pragma warning(disable:4996)



#include <WinSock2.h>
#include "AlphaInc.h"
#include <map>
using namespace std;

#define MAX_BUFFERING	40960
#define MAX_ONEPACKET	4096


/*
	packet 의 구조
	STX + 134=length + 0x01 + DATA
	
	134 : DEF_PACK_LEN
	length : DATA 의 length  => 무조건 4byte
	DATA : code=value+0x01 단위의 연속

*/

class CProtoSet
{
public:
	CProtoSet();
	~CProtoSet();

	void Begin();
	int Complete(/*out*/string& result, BOOL bForDelphi = FALSE);
	int Complete(/*out*/char* pzResult, BOOL bForDelphi=FALSE);

	void CopyFromRecvData(char* pzData);
	void SetVal(int nFd, string val);
	int NormalToDelphi(_InOut_ string& sPacket);
	int DelphiToNormal(_InOut_ string& sPacket);
#ifdef _UNICODE
	void SetVal(int nFd, _tstring val);
	void SetVal(int nFd, wchar_t* val);
#endif
	void SetVal(int nFd, char* val);
	void SetVal(int nFd, char val);
	void SetVal(int nFd, int val);
	void SetVal(int nFd, double val);
	void SetArrayStart(int nArraySize);
	void SetArrayVal(int nFd, const char* val);
	void SetArrayEnd();
private:
	void DelSameField(int nFd);
private:
	string	m_sBuf;
	char	m_zTemp[MAX_BUF];
	bool	m_bSetSuccYN;

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct ST_VAL
{
	string	sVal;
	int			nVal;
	double		dVal;
};

enum {FIELD_STR=0, FIELD_INT, FIELD_DBL};

class CProtoGet
{
public:
	CProtoGet();
	~CProtoGet();

	bool	Parsing(_In_  const char* pData, int nDataLen, BOOL bLog=FALSE);
	bool	ParsingWithHeader(_In_  const char* pRecvData, int nRecvLen, BOOL bLog = FALSE);
	//int		ParsingDebug(_In_  const char* pData, int nDataLen, BOOL bLog = FALSE);
	char*	GetRecvData() { return m_zBuf; }
#ifdef _UNICODE
	bool	GetCode(_Out_ _tstring& sCode);
	bool	GetVal(int nFd, _Out_ wchar_t* pzVal);
	bool	GetVal(int nFd, _Out_ _tstring* psVal);
	wchar_t* GetValS(int nFd, wchar_t* pzVal);
#endif
	bool	GetCode(_Out_ string& sCode);
	bool	GetVal(int nFd, _Out_ char* pzVal);
	bool	GetVal(int nFd, _Out_ string* psVal);
	bool	GetVal(int nFd, _Out_ int* pnVal);
	bool	GetVal(int nFd, _Out_ double* pdVal);
	
	int		GetValN(int nFd);
	double	GetValD(int nFd);

	void	SeeAllData(char* pzOut);

	int		CopyData(_Out_ map<int, string>* pOuter);

	bool	Is_Success();
	int		Get_RsltCode();

	char* GetMsg() { return m_zMsg; }
private:
	bool SetValByField(char* pzFd, char* pzVal, ST_VAL* pVal);
	int GetFieldType(int nField);
	bool IsParsed();
	void removeAll();
private:
	char				m_zBuf[MAX_BUF];
	char				m_zMsg[512];
	map<int, ST_VAL*>	m_mapResult;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CProtoUtils
{
public:
	CProtoUtils() {};
	~CProtoUtils() {};

	bool GetValue(_In_ char* pzRecvData, _In_ char* pzField, char* pzVal);
	bool GetValue(_In_ char* pzRecvData, _In_ int nField, char* pzVal);

	bool GetSymbol(char* pzRecvData, _Out_ string& sSymbol);
	bool GetUserId(char* pzRecvData, _Out_ string& sUserId);
	
	char* PacketCode(_In_ char* pzRecvData, _Out_ char* pzPacketCode);
	bool  IsSuccess(_In_ char* pzRecvData);
	bool  IsSuccess(_In_ char cErrYN);
private:

};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

class CProtoBuffering
{
public:
	CProtoBuffering();
	~CProtoBuffering();

	int		AddPacket(char* pBuf, int nSize);
	BOOL	GetOnePacket(int* pnLen, char* pOutBuf);

	BOOL	GetOnePacketWithHeader(int* pnLen, char* pOutBuf);

	char* GetErrMsg() { return m_msg; }
private:
	//int		GetOnePacketFn(int* pnLen, char* pOutBuf);
	int		GetOnePacketInner(int* pnLen, char* pOutBuf);
	void	MoveData(char* pStart, int nCopySize);
	
	//VOID	Erase(int nStartPos, int nLen);
	VOID	RemoveAll();

	VOID	Lock() { EnterCriticalSection(&m_cs); }
	VOID	Unlock() { LeaveCriticalSection(&m_cs); }

	
private:
	CRITICAL_SECTION	m_cs;
	char				m_buf[MAX_BUFFERING] ;
	char				m_msg[1024];
	int					m_nBufLen;
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class CPacketParser
{
public:
	CPacketParser();
	~CPacketParser();

	int		AddPacket(char* pBuf, int nSize);
	//BOOL	GetOnePacket(int* pnLen, char* pOutBuf);

	BOOL	IsEmpty() { return (m_sBuffer.size() == 0); }
	BOOL	GetOnePacketWithHeaderWithLock(int* pnLen, _Out_ char* pOutBuf);
	BOOL	GetOnePacketWithHeader(int* pnLen, char* pOutBuf);
	INT		GetOnePacketWithHeader2(int* pnLen, char* pOutBuf);

	char*	GetErrMsg() { return m_msg; }
	int		GetBuffLen() { return m_sBuffer.size(); }
protected:
	//int		GetOnePacketFn(int* pnLen, char* pOutBuf);
	//int		GetOnePacketInner(int* pnLen, char* pOutBuf);
	void	MoveData(int nPos);

	//VOID	Erase(int nStartPos, int nLen);
	//VOID	RemoveAll();

	VOID	Lock() { EnterCriticalSection(&m_cs); }
	VOID	Unlock() { LeaveCriticalSection(&m_cs); }


protected:
	CRITICAL_SECTION	m_cs;
	string				m_sBuffer;
	char				m_msg[1024];
	//int					m_nBufLen;
};



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
struct TPacketParserEx
{
	CPacketParser parser;
	void* pCK;

	TPacketParserEx()
	{
		pCK = NULL;
	}
};

class CPacketParserIOCP
{
public:
	CPacketParserIOCP();
	~CPacketParserIOCP();

	void	AddSocket(SOCKET sock);
	int		AddPacket(SOCKET sock, char* pBuf, int nSize);

	BOOL	GetOnePacketWithHeader(SOCKET sock, _Out_ int* pnLen, _Out_ char* pOutBuf);

private:
	void	Lock() { EnterCriticalSection(&m_cs); }
	void	UnLock() { LeaveCriticalSection(&m_cs); }

private:

	map<SOCKET, CPacketParser*>		m_mapBuffer;
	CRITICAL_SECTION		m_cs;
};