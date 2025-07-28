#pragma once
#pragma warning(disable:4996)
#include <Windows.h>
#include <map>
#include <string>
using namespace std;

#define MAX_BUFFERING	40960
#define DEF_ENTER			0x10
/*
	packet ÀÇ ±¸Á¶
	PACKET_BODY + ENTER
	
*/


class CPacketParserDelphi
{
public:
	CPacketParserDelphi();
	~CPacketParserDelphi();

	int		AddPacket(char* pBuf, int nSize);
	BOOL	GetOnePacket(_Out_ int* pnLen, _Out_ char* pOutBuf);

	BOOL	IsEmpty() { return (m_sBuffer.size() == 0); }
	
	char* GetErrMsg() { return m_msg; }
private:
	//int		GetOnePacketFn(int* pnLen, char* pOutBuf);
	int		GetOnePacketInner(int* pnLen, char* pOutBuf);
	void	MoveData(int nPos);

	//VOID	Erase(int nStartPos, int nLen);
	//VOID	RemoveAll();

	VOID	Lock() { EnterCriticalSection(&m_cs); }
	VOID	Unlock() { LeaveCriticalSection(&m_cs); }


private:
	CRITICAL_SECTION	m_cs;
	string				m_sBuffer;
	char				m_msg[1024];
	//int					m_nBufLen;
};