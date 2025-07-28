// ADOFunc.h: interface for the CADOFunc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADOFUNC_H__2EF3B881_D0C2_40D8_AF30_075F5606803F__INCLUDED_)
#define AFX_ADOFUNC_H__2EF3B881_D0C2_40D8_AF30_075F5606803F__INCLUDED_


#pragma warning (disable:4530)
#pragma warning(disable : 4146)
#pragma warning(disable:4996)
#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF", "ADOEOF") 
#include <set>
#include <tchar.h>
#define LEN_CMDBUFF 4096

#define DBPoolPtr(x) ((CDBPoolAdo*)##x##)

class CADOFunc  
{
public:
	CADOFunc();
	virtual ~CADOFunc();

//	BOOL Execute(TCHAR * szCmdText, VARIANT vtParam);
	//BOOL Init(BOOL bMSSQL = TRUE);
	BOOL Open(TCHAR *pzServerIP, TCHAR *pzUser, TCHAR *pzPassword, TCHAR* pzDBName);
	//BOOL DB_Open();

	void Close();
	void Destroy();
	TCHAR* GetError();
	long ExecSP(TCHAR *pszFormat, ...);
	long ExecSP(TCHAR *pszFormat, TCHAR *pszRecordArray);
	BOOL QrySelect(TCHAR* pzQry);	//일반 쿼리문 실행해서 레코드셋 반환
	BOOL ExecQuery(TCHAR* pzQry);	//일반 쿼리문 실행.
	BOOL AddParam(TCHAR *pName, enum DataTypeEnum eType, enum ParameterDirectionEnum eDirection, LONG lSize, VARIANT vtValue);
	BOOL DelParam(TCHAR*  pName);
	BOOL DelParam(LONG lIndex);
	BOOL DelParamAll();
	BOOL SetParamValue(LONG lIndex, VARIANT vtValue);
	BOOL SetParamValue(TCHAR*  pName, VARIANT vtValue);
	long GetParamCount();
	VARIANT GetParamValue(LONG lParam);
	VARIANT GetParamValue(TCHAR*  pszParam);
	long GetRecordCount();
	VARIANT GetValue(LONG lField);
	TCHAR* GetStr(int lField, TCHAR* pzOut) { _tcscpy(pzOut, (TCHAR*)(_bstr_t)GetValue((LONG)lField)); return pzOut; }
	long GetLong(int lField){ return (long)(_variant_t)GetValue((LONG)lField); }
	double GetDbl(int lField) { return (double)(_variant_t)GetValue((LONG)lField); }
	double GetDouble(int lField) { return GetDbl(lField); }

	VARIANT GetValue(TCHAR*  pszField);
	BOOL GetValueEx(TCHAR*  pszField, VARIANT* pRet);
	//VOID GetValueEx2(TCHAR*  pszField, VARIANT* pRet);
	TCHAR* GetStr(TCHAR*  pszField, TCHAR* pzOut);// { strcpy(pzOut, (LPCSTR)(_bstr_t)GetValue(pszField)); return pzOut; }
	TCHAR* GetStrWithLen(TCHAR*  pszField, int nMaxLen, TCHAR* pzOut);
	int GetStrEx(TCHAR*  pszField, TCHAR* pzOut, int * pnLen);// { strcpy(pzOut, (LPCSTR)(_bstr_t)GetValue(pszField)); return pzOut; }
	long GetLong(TCHAR*  pszField) { return (long)(_variant_t)GetValue(pszField); }
	double GetDbl(TCHAR*  pszField) { return (double)(_variant_t)GetValue(pszField); }
	double GetDouble(TCHAR*  pszField) { return GetDbl(pszField); }

	BOOL GetRows(VARIANT *pRecordArray);
	BOOL IsEOF();
	BOOL IsNextRow();
	void Next();
	BOOL GetErrFlag();

protected:
	BOOL Execute();
	void DumpError(_com_error &e, TCHAR *szBuffer);

	_ConnectionPtr	m_pConn;
	_CommandPtr		m_pCmd;
	_RecordsetPtr	m_pRs;
	TCHAR			*m_pzCmdBuffer;
	TCHAR			m_szMessage[1024];
	BOOL			m_bErrFlag;
	TCHAR			m_zConnStr[1024];
};


class CDBPoolAdo
{
public:
	CDBPoolAdo(TCHAR *pzServerIP, TCHAR *zUserID, TCHAR *zPassword, TCHAR* zDBName);
	virtual ~CDBPoolAdo();

	BOOL Init(int nInitCnt);
	INT Release(CADOFunc* p);
	CADOFunc* GetAvailableDB();
	CADOFunc* GetAvailableAdo(CADOFunc* p);
	CDBPoolAdo* Get() { return this; };
	TCHAR* GetMsg() { return m_zMsg; };
	INT Available();
private:
	VOID LOCK() { EnterCriticalSection(&m_cs); };
	VOID UNLOCK() { LeaveCriticalSection(&m_cs); };
	BOOL AddNew();
	INT Del(CADOFunc* p);
	VOID Destroy();
private:
	int m_nNowCnt;
	CRITICAL_SECTION m_cs;
	std::set<CADOFunc*>	m_setDB;
	TCHAR	m_zMsg[1024];
	TCHAR	m_zServer[128], m_zID[128], m_zPwd[128], m_zDBName[128];
	//sprintf(connStr, "DRIVER={SQL SERVER};Server=%s;Database=KRHedge;User ID=%s;Password=%s",ip, id, pwd);
};


class CDBHandlerAdo
{
public:
	CDBHandlerAdo(CDBPoolAdo* p);
	virtual ~CDBHandlerAdo();

	CADOFunc* operator ->();

private:
	CDBPoolAdo	*m_pPool;
	CADOFunc	*m_pAdo;
};

#endif // !defined(AFX_ADOFUNC_H__2EF3B881_D0C2_40D8_AF30_075F5606803F__INCLUDED_)
