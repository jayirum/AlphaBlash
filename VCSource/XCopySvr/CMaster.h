#pragma once

#include "Main.h"
#include "../Common/IRUM_Common.h"
#include "../Common/ADOFunc.h"

#define OLDDATA_GAP_SEC	30

struct MASTER_STATUS
{
	char	logInOff[1];
	wchar_t	wzLastLogOnOffTm[20];	//yyyymmddhh:mm:ss:mmm
	int		nLastCntrNo;
};

class CMaster
{
public:
	CMaster();
	~CMaster();

	BOOL Initialize(int idx, int nSendAllThreadId);
	VOID DeInitialize();

	BOOL CopierRqst_LogOnOff();
	BOOL CopierRqst_CntrHist(COMPLETION_KEY* pCK);

private:
	BOOL	Init_ReadConfig();
	BOOL	Init_DBOpen();
	BOOL	Init_Get_LastCntrNo();
	static	unsigned WINAPI DBReadThread(LPVOID lp);

	BOOL	_M_Conn_MainProc(BOOL bCopierCall);
	BOOL	_M_Conn_Publish_LoginStatus(wchar_t* masterNm, wchar_t* Tm, wchar_t* loginTp, int nGapSec);

	BOOL	_M_Cntr_MainProc(BOOL bHistory, COMPLETION_KEY* pCK);
	BOOL	_M_Cntr_Publish(
		BOOL bHistory
		, int cntrNo
		, wchar_t* stkCd
		, wchar_t* bsTp
		, int cntrQty
		, double cntrPrc
		, double clrPl
		, double cmsn
		, wchar_t* clrTp
		, int bf_nclrQty
		, int af_nclrQty
		, double bf_avgPrc
		, double af_avgPrc
		, double bf_amt
		, double af_amt
		, wchar_t* ordTp
		, wchar_t* tradeTm
		, int lvg
		, COMPLETION_KEY* pCK
	);

	void	RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen);

private:
	CDBPoolAdo*		m_pDBPool;

	int				m_nIdx;
	HANDLE			m_hDBThread;
	unsigned int	m_dDBThread;

	wchar_t			m_wzMasterId[128];
	wchar_t			m_wzComp[128];
	wchar_t			m_wzDBIp[128];
	wchar_t			m_wzDBId[128];
	wchar_t			m_wzDBPwd[128];
	wchar_t			m_wzDBName[128];
	int				m_nThreadTimeOut_MS;

	MASTER_STATUS	m_masterStatus;
	int				m_nSendAllThreadId;
	BOOL			m_bRun;
};

