#include "CMaster.h"

#include "../Common/LogMsg.h"
#include "../Common/IRExcept.h"
#include "../Common/IRUM_Common.h"
#include "../Common/CommonFunc.h"
#include "../Common/Util.h"
#include "../Common/XAlpha_Common.h"
#include "../Common/MemPool.h"

extern CLogMsg	g_log;
extern wchar_t	g_wzConfig[_MAX_PATH];
extern BOOL		g_bDebugLog;
extern CMemPool	g_memPool;

CMaster::CMaster()
{
	m_pDBPool	= NULL;
	m_nIdx		= 0;
	m_hDBThread = NULL;
	m_dDBThread = 0;

	m_wzMasterId[0]=0x00;
	m_wzComp[0] = 0x00;
	m_wzDBIp[0] = 0x00;
	m_wzDBId[0] = 0x00;
	m_wzDBPwd[0] = 0x00;
	m_wzDBName[0] = 0x00;
}

CMaster::~CMaster()
{
	DeInitialize();
}

VOID CMaster::DeInitialize()
{
	m_bRun = FALSE;
}

BOOL CMaster::Initialize(int idx, int nSendAllThreadId)
{
	m_nIdx = idx;
	m_bRun = TRUE;
	m_nSendAllThreadId = nSendAllThreadId;

	if (!Init_ReadConfig())
		return FALSE;

	if (!Init_DBOpen())
		return FALSE;

	if (!Init_Get_LastCntrNo())
		return FALSE;

	m_hDBThread = (HANDLE)_beginthreadex(NULL, 0, &DBReadThread, this, 0, &m_dDBThread);

	return TRUE;
}

BOOL CMaster::Init_ReadConfig()
{
	wchar_t wzKey[128];

	wsprintf(wzKey, TEXT("COMP_%d"), m_nIdx);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), wzKey, m_wzComp);

	wsprintf(wzKey, TEXT("DB_IP_%d"), m_nIdx);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), wzKey, m_wzDBIp);

	wsprintf(wzKey, TEXT("DB_ID_%d"), m_nIdx);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), wzKey, m_wzDBId);

	wsprintf(wzKey, TEXT("DB_PWD_%d"), m_nIdx);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), wzKey, m_wzDBPwd);

	wsprintf(wzKey, TEXT("DB_NAME_%d"), m_nIdx);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), wzKey, m_wzDBName);

	wsprintf(wzKey, TEXT("MASTER_%d"), m_nIdx);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), wzKey, m_wzMasterId);

	wsprintf(wzKey, TEXT("MASTER_%d"), m_nIdx);
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), wzKey, m_wzMasterId);

	wchar_t wz[32];
	CUtil::GetConfig(g_wzConfig, TEXT("DBINFO_MASTER"), TEXT("DB_SEL_TIMEOUT_MS"), wz);
	m_nThreadTimeOut_MS = _ttoi(wz);
	if (m_nThreadTimeOut_MS <= 0)
	{
		g_log.log(ERR, "DB SEL TIMEOUT Error:%d", m_nThreadTimeOut_MS);
		return FALSE;
	}


	if (
		(wcslen(m_wzComp) == 0) ||
		(wcslen(m_wzDBIp) == 0) ||
		(wcslen(m_wzDBId) == 0) ||
		(wcslen(m_wzDBPwd) == 0) ||
		(wcslen(m_wzDBName) == 0) ||
		(wcslen(m_wzMasterId) == 0)
		)
	{
		g_log.log(NOTIFY, "[IDX:%d] Failed to read Master Info ", m_nIdx);
		return FALSE;
	}

	ZeroMemory(&m_masterStatus, sizeof(m_masterStatus));

	g_log.logW(INFO, TEXT("[Read Master ID] (%s)"), m_wzMasterId);
	return TRUE;
}


BOOL CMaster::Init_DBOpen()
{
	m_pDBPool = new CDBPoolAdo(m_wzDBIp, m_wzDBId, m_wzDBPwd, m_wzDBName);
	if (!m_pDBPool->Init(1))
	{
		g_log.logW(NOTIFY, TEXT("DB OPEN FAILED.(%s)(%s)(%s)"), m_wzDBIp, m_wzDBId, m_wzDBPwd);
		return FALSE;
	}

	return TRUE;
}


BOOL CMaster::Init_Get_LastCntrNo()
{
	wchar_t wzQ[1024] = { 0, };
	CDBHandlerAdo db(m_pDBPool->Get());

	if (m_wzMasterId[0] == 0x00)
	{
		return FALSE;
	}
	m_masterStatus.nLastCntrNo = 0;

	wsprintf(wzQ, TEXT("SELECT  TOP 1 CNTR_NO FROM CNTR WHERE USER_ID = ")
		TEXT("'%s'")
		TEXT(" ORDER BY CNTR_NO DESC ")
		, m_wzMasterId
	);

	if (db->ExecQuery(wzQ) == FALSE)
	{
		g_log.logW(NOTIFY, TEXT("[Idx:%d]Get CNTR Last No Error(%s)"), m_nIdx, wzQ);
		return FALSE;
	}

	if (db->IsNextRow())
	{
		m_masterStatus.nLastCntrNo = db->GetLong(TEXT("CNTR_NO"));
		g_log.logW(INFO, TEXT("[Idx:%d][%s]Last Cntr No(%d)"), m_nIdx, m_wzMasterId, m_masterStatus.nLastCntrNo);
	}

	return TRUE;
}


BOOL CMaster::CopierRqst_LogOnOff()
{
	return _M_Conn_MainProc(TRUE);
}

BOOL CMaster::_M_Conn_MainProc(BOOL bCopierCall)
{
	wchar_t wzUserNm[32] = { 0 };
	wchar_t wzDt[32];
	wchar_t wzTm[32] = { 0 };
	wchar_t wzLoginTp[32] = { 0 };
	int		nGapSec;

	wchar_t wzDtTm[128];
	wchar_t wzQ[1024] = { 0, };

	CDBHandlerAdo db(m_pDBPool->Get());

	wsprintf(wzQ, TEXT(
		"SELECT TOP 3 A.USER_ID, A.LOGIN_DT, A.LOGIN_TM, A.LOGIN_TP, B.USER_NM "
		" ,DATEDIFF(SS, (CONVERT(CHAR(10), CONVERT(DATETIME, LOGIN_DT),121)+' '+LOGIN_TM), CONVERT(CHAR(22),getdate(), 121))  AS TIMEGAP_SEC "
		" FROM LOGIN_HIS A, USER_MST B "
		" WHERE A.USER_ID = '%s' AND LOGIN_DT >= DBO.FP_TRADE_DT() AND A.USER_ID=B.USER_ID  ORDER BY LOGIN_DT DESC,  LOGIN_TM DESC"
		)
		, m_wzMasterId
	);
	if (FALSE == db->ExecQuery(wzQ))
	{
		g_log.logW(NOTIFY, TEXT("[Idx:%d] LogonStatus Get Error(%s)(%s)"),m_nIdx, db->GetError(), wzQ);
		return FALSE;
	}
	if (db->IsNextRow() == FALSE)
	{
		// NO RECORDSET => LogOff 처리
		
		_M_Conn_Publish_LoginStatus(m_wzMasterId, TEXT(""), TEXT("O"), OLDDATA_GAP_SEC+10);
		// update 
		m_masterStatus.logInOff[0] = 'O';
		lstrcpyW(m_masterStatus.wzLastLogOnOffTm, TEXT(""));
		return TRUE;
	}


	if (db->IsNextRow())
	{
		ZeroMemory(wzUserNm, sizeof(wzUserNm));
		ZeroMemory(wzDt, sizeof(wzDt));
		ZeroMemory(wzTm, sizeof(wzTm));
		ZeroMemory(wzLoginTp, sizeof(wzLoginTp));
		ZeroMemory(wzDtTm, sizeof(wzDtTm));
		nGapSec = 0;

		//db->GetStrWithLen(TEXT("USER_ID"), sizeof(wzMasterId), wzMasterId);
		db->GetStrWithLen(TEXT("USER_NM"), sizeof(wzUserNm), wzUserNm);
		db->GetStrWithLen(TEXT("LOGIN_DT"), sizeof(wzDt), wzDt);
		db->GetStrWithLen(TEXT("LOGIN_TM"), sizeof(wzTm), wzTm);
		db->GetStrWithLen(TEXT("LOGIN_TP"), sizeof(wzLoginTp), wzLoginTp);
		nGapSec = db->GetLong(TEXT("TIMEGAP_SEC"));

		if ((bCopierCall == FALSE) && nGapSec > OLDDATA_GAP_SEC)
		{
			db->Close();
			return TRUE;
		}

		if (bCopierCall == FALSE)
		{
			wsprintf(wzDtTm, TEXT("%s%s"), wzDt, wzTm);
			if (wcsncmp(wzDtTm, m_masterStatus.wzLastLogOnOffTm, lstrlenW(wzDtTm)) <= 0)
			{
				db->Close();
				return TRUE;
			}
		}

		_M_Conn_Publish_LoginStatus(wzUserNm, wzTm, wzLoginTp, nGapSec);

		char z[32] = { 0 }; U2A(wzLoginTp, z);

		// update 
		m_masterStatus.logInOff[0] = z[0];
		lstrcpyW(m_masterStatus.wzLastLogOnOffTm, wzDtTm);
	}
	db->Close();

	return TRUE;
}

BOOL CMaster::_M_Conn_Publish_LoginStatus(wchar_t* userNm, wchar_t* Tm, wchar_t* loginTp, int nGapSec)
{
	char z[128];
	char zSendBuf[1024] = { 0 };
	_XAlpha::TLOGON* pSend = (_XAlpha::TLOGON*)zSendBuf;
	int len = sizeof(_XAlpha::TLOGON);
	memset(pSend, 0x20, len);

	memcpy(pSend->Code, CODE_LOGONOFF, strlen(CODE_LOGONOFF));

	BOOL bOldData = (nGapSec > OLDDATA_GAP_SEC);
	if (bOldData)	pSend->oldDataYN[0] = 'Y';
	else			pSend->oldDataYN[0] = 'N';

	U2A(m_wzMasterId, z);
	memcpy(pSend->masterId, z, strlen(z));

	U2A(userNm, z);
	memcpy(pSend->masterNm, z, strlen(z));
	 //en = wcslen(userNm);

	//len = U2ALen(userNm, z);
	 //int len2 = strlen(z);
	//memcpy(pSend->masterNm, z, len);

	U2A(Tm, z);
	memcpy(pSend->Tm, z, strlen(z));

	U2A(loginTp, z);
	memcpy(pSend->loginTp, z, 1);

	//p->ETX[0] = DEF_ETX;
	pSend->Enter[0] = DEF_ENTER;


	//g_log.log(INFO, "[LOGONOFF](%.20s)<%c>(%.12s)(%c)(%s)",
	//	pSend->masterId, pSend->oldDataYN[0], pSend->Tm, pSend->loginTp[0], zSendBuf);

	char* pData = g_memPool.Get();
	memcpy(pData, zSendBuf, len);
	PostThreadMessage(m_nSendAllThreadId, WM_SENDALL_DATA, (WPARAM)len, (LPARAM)pData);

	return TRUE;
}


BOOL CMaster::CopierRqst_CntrHist(COMPLETION_KEY* pCK)
{
	return _M_Cntr_MainProc(TRUE, pCK);
}

BOOL CMaster::_M_Cntr_MainProc(BOOL bHistory, COMPLETION_KEY* pCK)
{
	wchar_t wzQ[1024] = { 0, };
	CDBHandlerAdo db(m_pDBPool->Get());
	//BOOL bInit = (m_Master[idx].nLastCntrNo <= 0);
	if (bHistory)
	{
		wsprintf(wzQ, TEXT("SELECT * FROM CNTR WHERE USER_ID = ")
			TEXT("'%s'")
			TEXT(" ORDER BY CNTR_NO ")
			, m_wzMasterId
		);
	}
	else
	{
		wsprintf(wzQ, TEXT("SELECT TOP 1 * FROM CNTR WHERE USER_ID = ")
			TEXT("'%s'")
			TEXT(" AND CNTR_NO > %d ORDER BY CNTR_NO")
			, m_wzMasterId
			, m_masterStatus.nLastCntrNo
		);
	}
	if (FALSE == db->ExecQuery(wzQ))
	{
		g_log.logW(NOTIFY, TEXT("[Ix:%d]CNTR Get Error(%s)(%s)"), m_nIdx, db->GetError(), wzQ);
		return FALSE;
	}
	if (db->IsNextRow() == FALSE)
	{
		if (bHistory)
		{
			char returnBuf[128] = { 0 };
			_XAlpha::TRET_MSG* p = (_XAlpha::TRET_MSG*)returnBuf;
			memcpy(p->Code, CODE_MSG, sizeof(p->Code));
			memcpy(p->RetCode, RETCODE_CNTR_NODATA, sizeof(p->RetCode));
			p->Enter[0] = DEF_ENTER;
			RequestSendIO(pCK, returnBuf, strlen(returnBuf));
		}
		return TRUE;
	}

	int nCntrNo;
	wchar_t wzStkCd[32];
	wchar_t wzBsTp[32];
	int		nCntrQty;
	double	dCntrPrc;
	double	dClrPl;
	double	dCmsn;
	wchar_t wzClrTp[32];
	int		nBf_Pos;
	int		nAf_Pos;
	double	dBf_Avg;
	double	dAf_Avg;
	double	dBf_Amt;
	double	dAf_Amt;
	wchar_t wzOrdTp[32];
	wchar_t wzCntrTm[32];
	int		nLvg;

	while (db->IsNextRow())
	{
		nCntrNo = 0;
		nBf_Pos = 0;
		nAf_Pos = 0;
		nLvg = 0;
		nCntrQty = 0;

		dCntrPrc = 0;
		dClrPl = 0;
		dCmsn = 0;
		dBf_Avg = 0;
		dAf_Avg = 0;
		dBf_Amt = 0;
		dAf_Amt = 0;


		ZeroMemory(wzStkCd, sizeof(wzStkCd));
		ZeroMemory(wzBsTp, sizeof(wzBsTp));
		ZeroMemory(wzOrdTp, sizeof(wzOrdTp));
		ZeroMemory(wzCntrTm, sizeof(wzCntrTm));
		ZeroMemory(wzClrTp, sizeof(wzClrTp));

		nCntrNo = db->GetLong(TEXT("CNTR_NO"));
		nCntrQty = db->GetLong(TEXT("CNTR_QTY"));
		nBf_Pos = db->GetLong(TEXT("BF_NCLR_POS_QTY"));
		nAf_Pos = db->GetLong(TEXT("AF_NCLR_POS_QTY"));
		nLvg = db->GetLong(TEXT("LEVERAGE"));

		dCntrPrc = db->GetDbl(TEXT("CNTR_PRC"));
		dClrPl = db->GetDbl(TEXT("CLR_PL"));
		dCmsn = db->GetDbl(TEXT("CMSN_AMT"));
		dBf_Avg = db->GetDbl(TEXT("BF_AVG_PRC"));
		dAf_Avg = db->GetDbl(TEXT("AF_AVG_PRC"));
		dBf_Amt = db->GetDbl(TEXT("BF_NET_ACNT_AMT"));
		dAf_Amt = db->GetDbl(TEXT("AF_NET_ACNT_AMT"));

		db->GetStrWithLen(TEXT("STK_CD"), sizeof(wzStkCd), wzStkCd);
		db->GetStrWithLen(TEXT("BS_TP"), sizeof(wzBsTp), wzBsTp);
		db->GetStrWithLen(TEXT("ORD_TP"), sizeof(wzOrdTp), wzOrdTp);
		db->GetStrWithLen(TEXT("CNTR_TM"), sizeof(wzCntrTm), wzCntrTm);
		db->GetStrWithLen(TEXT("CLR_TP"), sizeof(wzClrTp), wzClrTp);

		if ((bHistory == FALSE) && (m_masterStatus.nLastCntrNo >= nCntrNo))
		{
			db->Next();
			continue;
		}

		_M_Cntr_Publish(bHistory, nCntrNo, wzStkCd, wzBsTp, nCntrQty, dCntrPrc,
			dClrPl, dCmsn, wzClrTp, nBf_Pos, nAf_Pos, dBf_Avg, dAf_Avg, dBf_Amt,
			dAf_Amt, wzOrdTp, wzCntrTm, nLvg, pCK);

		if (bHistory == FALSE)
			m_masterStatus.nLastCntrNo = nCntrNo;


		db->Next();
	}
	db->Close();

	return TRUE;
}

BOOL CMaster::_M_Cntr_Publish(
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
)
{
	char zSendBuf[1024] = { 0 };
	_XAlpha::TCNTR* p = (_XAlpha::TCNTR*)zSendBuf;

	char z[128];
	int nTotLen = sizeof(_XAlpha::TCNTR);
	memset(zSendBuf, 0x20, nTotLen);

	if (bHistory)
		memcpy(p->Code, CODE_CNTR_HIST, strlen(CODE_CNTR));
	else
		memcpy(p->Code, CODE_CNTR, strlen(CODE_CNTR));


	sprintf(z, "%d", cntrNo);
	memcpy(p->cntrNo, z, strlen(z));

	U2A(m_wzMasterId, z);
	memcpy(p->masterId, z, strlen(z));

	U2A(stkCd, z);
	memcpy(p->stkCd, z, strlen(z));

	U2A(bsTp, z);
	memcpy(p->bsTp, z, strlen(z));

	sprintf(z, "%d", cntrQty);
	memcpy(p->cntrQty, z, strlen(z));

	sprintf(z, "%.5f", cntrPrc);
	memcpy(p->cntrPrc, z, strlen(z));

	sprintf(z, "%.0f", clrPl);
	memcpy(p->clrPl, z, strlen(z));

	sprintf(z, "%.0f", cmsn);
	memcpy(p->cmsn, z, strlen(z));

	U2A(clrTp, z);
	memcpy(p->clrTp, z, 1);

	sprintf(z, "%d", bf_nclrQty);
	memcpy(p->bf_nclrQty, z, strlen(z));

	sprintf(z, "%d", af_nclrQty);
	memcpy(p->af_nclrQty, z, strlen(z));

	sprintf(z, "%.5f", bf_avgPrc);
	memcpy(p->bf_avgPrc, z, strlen(z));

	sprintf(z, "%.5f", af_avgPrc);
	memcpy(p->af_avgPrc, z, strlen(z));

	sprintf(z, "%.0f", bf_amt);
	memcpy(p->bf_amt, z, strlen(z));

	sprintf(z, "%.0f", af_amt);
	memcpy(p->af_amt, z, strlen(z));

	U2A(ordTp, z);
	memcpy(p->ordTp, z, strlen(z));

	U2A(tradeTm, z);
	memcpy(p->tradeTm, z, strlen(z));

	sprintf(z, "%d", lvg);
	memcpy(p->lvg, z, strlen(z));

	p->Enter[0] = DEF_ENTER;

	g_log.log(INFO, "[CNTR](%.2s)(%d)(%.12s)(%.5s)(%.2s)(%c)(%.5f)(%d)",
		p->Code, cntrNo, p->masterId, p->stkCd, p->ordTp, p->bsTp[0], cntrPrc, cntrQty);

	if (bHistory)
	{
		RequestSendIO(pCK, zSendBuf, nTotLen);
	}
	else
	{ 
		char* pData = g_memPool.Get();
		memcpy(pData, zSendBuf, nTotLen);
		PostThreadMessage(m_nSendAllThreadId, WM_SENDALL_DATA, (WPARAM)nTotLen, (LPARAM)pData);
	}

	return TRUE;
}

	int LOGON_RETRY_CNT;
	int g_GapCnt_Logon;

unsigned WINAPI CMaster::DBReadThread(LPVOID lp)
{
	CMaster* pThis = (CMaster*)lp;
	BOOL bNotCopierCall = FALSE;
	BOOL bNotHistory = FALSE;

	if (pThis->m_nThreadTimeOut_MS <= 100)
		LOGON_RETRY_CNT = 10;
	else
		LOGON_RETRY_CNT = 5;

	g_GapCnt_Logon = 0;
	while (pThis->m_bRun)
	{
		Sleep(pThis->m_nThreadTimeOut_MS);

		if (pThis->m_wzMasterId[0] != 0x00)
		{
			if (++g_GapCnt_Logon == LOGON_RETRY_CNT)
			{
				pThis->_M_Conn_MainProc(bNotCopierCall);
				g_GapCnt_Logon = 0;
			}

			pThis->_M_Cntr_MainProc(bNotHistory, NULL);
		}
	}
	return 0;
}



VOID CMaster::RequestSendIO(COMPLETION_KEY* pCK, char* pSendBuf, int nSendLen)
{
	SOCKET sock = pCK->sock;

	BOOL  bRet = TRUE;
	DWORD dwOutBytes = 0;
	DWORD dwFlags = 0;
	IO_CONTEXT* pSend = NULL;

	try {
		pSend = new IO_CONTEXT;

		ZeroMemory(pSend, sizeof(IO_CONTEXT));
		CopyMemory(pSend->buf, pSendBuf, nSendLen);
		pSend->wsaBuf.buf = pSend->buf;
		pSend->wsaBuf.len = nSendLen;
		pSend->context = CTX_RQST_SEND;

		int nRet = WSASend(sock
			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
			, 1					// wsaBuf 포인터 갯수
			, &dwOutBytes		// 전송된 바이트 수
			, dwFlags
			, &pSend->overLapped	// overlapped 포인터
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				g_log.logW(LOGTP_ERR, TEXT("WSASend error : %d"), WSAGetLastError);
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		g_log.logW(ERR, TEXT("WSASend try catch error [CDispatch]"));
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;

	return;
}

