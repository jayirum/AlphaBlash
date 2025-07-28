#pragma once
#pragma warning(disable:4996)
#include "AlphaInc.h"



#include <string>
#include <map>
using namespace std;


/*
	packet 의 구조
	STX + 99=length + 0x01 + DATA
	
	99 : DEF_PACK_LEN
	length : DATA 의 length  => 무조건 4byte
	DATA : code=value+0x01 단위의 연속

*/

class CProtoSet
{
public:
	CProtoSet();
	~CProtoSet();

	void Begin();
	int Complete(/*out*/string& result);
	int Complete(/*out*/char* pzResult);

	void CopyFromRecvData(char* pzData);
	void SetVal(int nFd, string val);
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
};

struct ST_VAL
{
	string	sVal;
	int		nVal;
	double	dVal;
};

class CProtoGet
{
public:
	CProtoGet();
	~CProtoGet();

	int		Parsing(_In_  const char* pData, int nDataLen, BOOL bLog=FALSE);
	char*	GetRecvData() { return m_zBuf; }
	bool	GetCode(_Out_ string& sCode);
	bool	GetVal(int nFd, _Out_ char* pzVal);
	bool	GetVal(int nFd, _Out_ string* psVal);
	bool	GetVal(int nFd, _Out_ int* pnVal);
	bool	GetVal(int nFd, _Out_ double* pdVal);

	char*	GetValS(int nFd, char* pzVal);
	int		GetValN(int nFd);
	double	GetValD(int nFd);

private:
	void SetValByField(char* pzFd, char* pzVal, ST_VAL* pVal);
	bool IsParsed();
private:
	char				m_zBuf[MAX_BUF];
	map<int, ST_VAL>	m_mapResult;
};


class CProtoUtils
{
public:
	CProtoUtils() {};
	~CProtoUtils() {};

	static bool GetValue(_In_ char* pzRecvData, _In_ char* pzField, char* pzVal);
	static bool GetValue(_In_ char* pzRecvData, _In_ int nField, char* pzVal);
};

///////////////////////////////////////////////
#define MAX_BUFFERING	4096

class CProtoBuffering
{
public:
	CProtoBuffering();
	~CProtoBuffering();

	int		AddPacket(char* pBuf, int nSize);
	BOOL	GetOnePacket(int* pnLen, char* pOutBuf);
	char* GetErrMsg() { return m_msg; }
private:
	int		GetOnePacketFn(int* pnLen, char* pOutBuf);
	
	//VOID	Erase(int nStartPos, int nLen);
	VOID	RemoveAll();

	VOID	Lock() { EnterCriticalSection(&m_cs); }
	VOID	Unlock() { LeaveCriticalSection(&m_cs); }

	
private:
	CRITICAL_SECTION	m_cs;
	char				m_buf[MAX_BUFFERING];
	char				m_msg[1024];
	int					m_nBufLen;
};