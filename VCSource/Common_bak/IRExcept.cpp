#include "IRExcept.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "Util.h"

CIRExcept::CIRExcept(int nError)
{
	_tcscpy(m_wzMsg, GetCodeMsg(nError));
}

CIRExcept::CIRExcept(TCHAR* pzMsg)
{
	_tcscpy(m_wzMsg, pzMsg);
}

CIRExcept::CIRExcept(int nError, TCHAR* pzMsg)
{
	_tcscpy(m_wzMsg, pzMsg);
	m_nErrNo = nError;
}

CIRExcept::~CIRExcept()
{

}

const TCHAR* CIRExcept::GetCodeMsg(int nErr)
{
	static TCHAR msg[1024];

	switch (nErr)
	{
	case ERR_COMM_INIT:	_stprintf(msg, TEXT("connection error")); break;
	default:_stprintf(msg, TEXT("exception occurred")); break;
	}

	return msg;
}

char* CIRExcept::GetMsg()
{
	U2A(m_wzMsg, m_zMsg);
	return m_zMsg;
}