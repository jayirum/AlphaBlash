#include "PacketParserDelphi.h"



CPacketParserDelphi::CPacketParserDelphi()
{
	//m_nBufLen = 0;
	InitializeCriticalSectionAndSpinCount(&m_cs, 2000);
}

CPacketParserDelphi::~CPacketParserDelphi()
{
	DeleteCriticalSection(&m_cs);
}

int CPacketParserDelphi::AddPacket(char* pBuf, int nSize)
{
	if (!pBuf)
		return 0;

	int nRet = 0;
	Lock();
	try
	{
		m_sBuffer.append(pBuf, nSize);
		nRet = m_sBuffer.size(); //m_nBufLen;
	}
	catch (...)
	{
		nRet = -1;
	}
	Unlock();
	return nRet;
}


///*
//	return value : 밖에서 이 함수를 한번 더 호출할 것인가 여부
//
//	*pnLen : 실제 pOutBuf 에 copy 되는 size
//*/
BOOL CPacketParserDelphi::GetOnePacket(_Out_ int* pnLen, char* pOutBuf)
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
BOOL CPacketParserDelphi::GetOnePacketInner(int* pnLen, char* pOutBuf)
{
	*pnLen = 0;

	if (m_sBuffer.size() == 0) {
		strcpy(m_msg, "No data in the buffer");
		*pnLen = 0;
		return FALSE;
	}


	//find ENTER
	int nPosEnter = m_sBuffer.find_first_of(DEF_ENTER);
	if (nPosEnter == string::npos) {
		// It's not complete packet
		return FALSE;
	}

	// copy one packet
	memcpy(pOutBuf, m_sBuffer.c_str(), nPosEnter - 1);

	// remove copied data
	MoveData(nPosEnter);

	return (m_sBuffer.size()>0);
}


void CPacketParserDelphi::MoveData(int nPos)
{
	string backup = m_sBuffer.substr(nPos);
	m_sBuffer.clear();
	m_sBuffer = backup;
}
