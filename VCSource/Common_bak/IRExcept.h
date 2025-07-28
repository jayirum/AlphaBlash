#pragma once
#pragma warning(disable:4996)

#include <tchar.h>
#define ERR_BASE		90000
#define ERR_COMM_INIT	ERR_BASE + 1

#define ASSERT_BOOL(result, msg){ if(!result) throw CIRExcept(msg);}
#define ASSERT_BOOL2(result, code, msg){ if(!result) throw CIRExcept(code, msg);}
#define ASSERT_ZERO(result, msg){ if(result!=0) throw CIRExcept(msg);}

class CIRExcept
{
public:
	CIRExcept(int nError);
	CIRExcept(TCHAR* pzMsg);
	CIRExcept(int nError, TCHAR* pzMsg);
	virtual ~CIRExcept();

	TCHAR* GetMsgW() { return m_wzMsg; }
	char* GetMsg();
	const TCHAR* GetCodeMsg(int nErr);
	int	GetCode() { return m_nErrNo; }


private:
	int		m_nErrNo;
	TCHAR	m_wzMsg[1024];
	char	m_zMsg[1024];

};