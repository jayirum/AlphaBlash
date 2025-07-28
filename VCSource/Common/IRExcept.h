#pragma once
#pragma warning(disable:4996)

#define ERR_BASE		90000
#define ERR_COMM_INIT	ERR_BASE + 1

#define ASSERT_BOOL(result, msg){ if(!result) throw CIRExcept(msg);}
#define ASSERT_BOOL2(result, code, msg){ if(!result) throw CIRExcept(code, msg);}
#define ASSERT_ZERO(result, msg){ if(result!=0) throw CIRExcept(msg);}
#define ASSERT_NULL(result, msg){ if(result==NULL) throw CIRExcept(msg);}

class CIRExcept
{
public:
	CIRExcept(int nError);
	CIRExcept(char* pzMsg);
	CIRExcept(const char* pzMsg);
	CIRExcept(int nError, char* pzMsg);
	virtual ~CIRExcept();

	char* GetMsg() { return m_zMsg; }
	const char* GetCodeMsg(int nErr);
	int	GetCode() { return m_nErrNo; }


private:
	int		m_nErrNo;
	char	m_zMsg[1024];
};