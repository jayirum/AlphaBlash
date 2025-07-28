#ifndef __C_UTILS_H__
#define __C_UTILS_H__

#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include "AlphaInc.h"

#define SAFE_CLOSE(x) { if(x!=NULL) CloseHandle(x); x=NULL; }
#define SAFE_DELETE(x) { if(x!=NULL) delete x; x=NULL; }

#define LOCK_CS(x) EnterCriticalSection(&##x##);
#define UNLOCK_CS(x) LeaveCriticalSection(&##x##);

#define CHECK_BOOL(rslt,msg) { if(!rslt) throw msg; }
void __CheckBool(bool res, char* msg) ;

/////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////
class CUtils
{
public:
	CUtils(){};
	~CUtils(){};

	String 	GetPureModuleName();
	char*  	GetMacAddr(_Out_ char* pMac);

	char*	FormatMoney(double dMoney, int nDotCnt, _Out_ char* buf);
};

class CIniFile
{
public:
	CIniFile(){}
	~CIniFile(){}

	AnsiString 	GetCnfgFileName(AnsiString sExtension="ini");
	AnsiString 	GetVal(AnsiString sSec, AnsiString sKey);
	bool 		SetVal(AnsiString sSec, AnsiString sKey, AnsiString sVal);
private:
	AnsiString 	m_sIniFile;
};

class smartptr
{
public:
	smartptr(int size) { m_p = new char[size]; ZeroMemory(m_p, size); }
	~smartptr() { if (m_p) delete[] m_p; m_p = NULL; }

	char* get() { return m_p; };

private:
	char* m_p;
};

class smartptrDbl
{
public:
	smartptrDbl(int size) { m_p = new double[size]; ZeroMemory(m_p, size); }
	~smartptrDbl() { if (m_p) delete[] m_p; m_p = NULL; }

	double* get() { return m_p; };

private:
	double* m_p;
};




#endif // !defined(AFX_LOG_H__124A47E2_E716_4D95_B88D_50C41838F37F__INCLUDED_)
