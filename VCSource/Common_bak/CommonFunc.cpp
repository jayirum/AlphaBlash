#include "ADOFunc.h"
#include "CommonFunc.h"
#include "AlphaProtocolUni.h"
#include "AlphaInc.h"
#include "Util.h"

BOOL Comm_DBSave_TraceOrder(void* pDBPool, const char* pOrdData, int nDataLen, _Out_ wchar_t* pzMsg)
{
	*pzMsg = 0;
	BOOL bRet;

	CDBHandlerAdo db(((CDBPoolAdo*)pDBPool)->Get());
	wchar_t wzQ[1024] = { 0, };
	CProtoGet protoGet;
	protoGet.Parsing(pOrdData, nDataLen, FALSE);

	wchar_t wzOpenTicket[32] = { 0, };
	wchar_t wzOrderGroupKey[32] = { 0, };
	wchar_t wzClosingTicket[32] = { 0, };
	wchar_t wzMasterTp[32] = { 0, };
	wchar_t wzSymbol[32] = { 0, };
	wchar_t wzTime[64] = { 0, }, wzOpenTime[64] = { 0, }, wzCloseTime[64] = { 0, }, wzExpiry[64] = { 0, };
	wchar_t wzOpenGmt[64] = { 0, }, wzCloseGmt[64] = { 0, }, wzLastActionTM_MT4[64] = { 0, }, wzLastActionTM_Gmt[64] = { 0, };
	wchar_t wzMasterTicket[64] = { 0, };
	wchar_t wzMasterUserID[64] = { 0, };
	wchar_t wzMasterAcc[64] = { 0, };
	wchar_t wzUserID[64] = { 0, };
	wchar_t wzMyAcc[64] = { 0, };
	wchar_t wzComments[256] = { 0, };
	double dEquity;
	wchar_t wzPlaform[64] = { 0, };
	wchar_t wzKeepOrgYN[32] = { 0, };
	wchar_t wzSubOrdAction_PartialNewYN[32] = { 0, };
	wchar_t	wzOrdSide[32] = { 0, };

	protoGet.GetValS(FDS_TM_HEADER, wzTime);
	protoGet.GetValS(FDS_USERID_MASTER, wzMasterUserID);
	protoGet.GetValS(FDS_ACCNO_MASTER, wzMasterAcc);
	protoGet.GetValS(FDS_MASTERCOPIER_TP, wzMasterTp);

	protoGet.GetValS(FDS_MT4_TICKET, wzOpenTicket);
	protoGet.GetValS(FDS_ORDER_GROUPKEY, wzOrderGroupKey);
	protoGet.GetValS(FDS_MT4_TICKET_CLOSING, wzClosingTicket);
	protoGet.GetValS(FDS_MASTER_TICKET, wzMasterTicket);
	protoGet.GetValS(FDS_SYMBOL, wzSymbol);
	protoGet.GetValS(FDS_OPEN_TM, wzOpenTime);
	protoGet.GetValS(FDS_CLOSE_TM, wzCloseTime);
	protoGet.GetValS(FDS_USERID_MINE, wzUserID);
	protoGet.GetValS(FDS_ACCNO_MINE, wzMyAcc);
	protoGet.GetValS(FDS_COMMENTS, wzComments);
	protoGet.GetValS(FDS_EXPIRY, wzExpiry);
	protoGet.GetValS(FDS_OPEN_GMT, wzOpenGmt);
	protoGet.GetValS(FDS_LAST_ACTION_MT4_TM, wzLastActionTM_MT4);
	protoGet.GetValS(FDS_LAST_ACTION_GMT, wzLastActionTM_Gmt);
	protoGet.GetValS(FDS_CLOSE_GMT, wzCloseGmt);
	protoGet.GetVal(FDD_EQUITY, &dEquity);
	protoGet.GetValS(FDS_SYS, wzPlaform);
	protoGet.GetValS(FDS_KEEP_ORGTICKET_YN, wzKeepOrgYN);
	protoGet.GetValS(FDS_ORD_ACTION_SUB_PARTIAL_YN, wzSubOrdAction_PartialNewYN);
	protoGet.GetValS(FDS_ORD_SIDE, wzOrdSide);

	_stprintf(wzQ, TEXT("EXEC TRACE_ORDER ")
		TEXT("'%s'")	//@I_MT4_TICKET		varchar(10)
		TEXT(",'%s'")	//@@I_ORDER_GROUPKEY		varchar(10)
		TEXT(",'%s'")	//@@I_MT4_TICKET_CLOSING		varchar(10)
		TEXT(",'%s'")	//, @I_MT4_OPEN_TM		varchar(20)
		TEXT(",%d")	//@I_MT4_TYPE		int
		TEXT(",%f")	//@I_MT4_SIZE		decimal(10, 3)
		TEXT(",'%s'")	//@I_MT4_SYMBOL		varchar(20)
		TEXT(",%f")	//@I_MT4_OPEN_PRC	decimal(10, 5)
		TEXT(",%f")	//@I_MT4_SL			decimal(10, 5)
		TEXT(",%f")	//@I_MT4_TP			decimal(10, 5)
		TEXT(",'%s'")	// MT4_CLOSE_TM
		TEXT(",%f")	//@I_MT4_CLOSE_PRC	decimal(10, 5)
		TEXT(",%f")	//@I_MT4_CMSN		decimal(10, 2)
		TEXT(",%f")	//@I_MT4_SWAP		decimal(10, 2)
		TEXT(",%f")	//@I_MT4_PROFIT		decimal(10, 2)
		TEXT(",'%s'")	//@I_MT4_COMMENTS	varchar(50)
		TEXT(",'%s'")	//@I_EXPIRY			VARCHAR(20)
		TEXT(",%d")	//@I_ORD_STATUS		int
		TEXT(",'%s'")	//@I_LAST_ACTION_MT4_TM	VARCHAR(20)
		TEXT(",'%s'")	//@I_OPEN_GMT			VARCHAR(20)
		TEXT(",'%s'")	//@I_LAST_ACTION_GMT		VARCHAR(20)
		TEXT(",'%s'")	//@I_USER_ID			varchar(20)
		TEXT(",'%s'")	//@I_MT4_ACC			varchar(20)
		TEXT(",'%.1s'")	//@I_MC_TP			char(1)
		TEXT(",'%s'")	//@I_MASTER_TICKET	varchar(10)
		TEXT(",'%s'")	//@I_MASTER_ID		varchar(20)
		TEXT(",'%s'")	//@I_MASTER_MT4_ACC	varchar(20)
		TEXT(",'%s'")	//@I_CLOSE_GMT
		TEXT(",%f")		//@I_EQUITY
		TEXT(",'%s'")	//@I_PLATFORM
		TEXT(",'%s'")	//@I_KEEP_ORGTICKET_YN
		TEXT(",'%s'")	//@I_SUBORDACTION_PARTIAL_YN
		TEXT(",'%s'")	//@I_ORD_SIDE
		, wzOpenTicket	//@I_MT4_TICKET		varchar(10)
		, wzOrderGroupKey
		, wzClosingTicket
		, wzOpenTime	//, @I_MT4_OPEN_TM		varchar(20)
		, protoGet.GetValN(FDN_ORD_TYPE)	//@I_MT4_TYPE		int
		, protoGet.GetValD(FDD_LOTS)		//@I_MT4_SIZE		decimal(10, 3)
		, wzSymbol	//",'%s'"	//@I_MT4_SYMBOL		varchar(20)
		, protoGet.GetValD(FDD_OPEN_PRC)	//@I_MT4_OPEN_PRC	decimal(10, 5)
		, protoGet.GetValD(FDD_SLPRC)		//@I_MT4_SL			decimal(10, 5)
		, protoGet.GetValD(FDD_TPPRC)		//@I_MT4_TP			decimal(10, 5)
		, wzCloseTime
		, protoGet.GetValD(FDD_CLOSE_PRC)	//@I_MT4_CLOSE_PRC	decimal(10, 5)
		, protoGet.GetValD(FDD_CMSN)	//@I_MT4_CMSN		decimal(10, 2)
		, protoGet.GetValD(FDD_SWAP)	//@I_MT4_SWAP		decimal(10, 2)
		, protoGet.GetValD(FDD_PROFIT)	//@I_MT4_PROFIT		decimal(10, 2)
		, wzComments						//@I_MT4_COMMENTS	varchar(50)
		, wzExpiry						//@I_EXPIRY			VARCHAR(20)
		, protoGet.GetValN(FDN_ORD_ACTION)	//@I_ORD_STATUS		int
		, wzLastActionTM_MT4					//@I_LAST_ACTION_MT4_TM	VARCHAR(20)
		, wzOpenGmt							//@I_OPEN_GMT			VARCHAR(20)
		, wzLastActionTM_Gmt					//@I_LAST_ACTION_GMT		VARCHAR(20)
		, wzUserID							//@I_USER_ID			varchar(20)
		, wzMyAcc							//@I_MT4_ACC			varchar(20)
		, wzMasterTp							//@I_MC_TP			char(1)
		, wzMasterTicket						//@I_MASTER_TICKET	varchar(10)
		, wzMasterUserID						//@I_MASTER_ID		varchar(20)
		, wzMasterAcc						//@I_MASTER_MT4_ACC	varchar(20)
		, wzCloseGmt
		, dEquity
		, wzPlaform
		, wzKeepOrgYN
		, wzSubOrdAction_PartialNewYN
		, wzOrdSide
	);
	if (FALSE == db->ExecQuery(wzQ))
	{
		_stprintf(pzMsg, TEXT("SAVE_ORDER Error(%s)(%s)"), db->GetError(), wzQ);
		bRet = FALSE;
	}
	else
	{
		_stprintf(pzMsg, wzQ);
		bRet = TRUE;
	}
	db->Close();

	return bRet;
}


BOOL Comm_Compose_ConfigSymbol_MasterW(void* pDBPool, const wchar_t* pzUserID, const wchar_t* pzMT4Acc,
	_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg)
{
	char zUserID[128] = { 0, };
	char zAcc[128] = { 0, };
	U2A((TCHAR*)pzUserID, zUserID);
	U2A((TCHAR*)pzMT4Acc, zAcc);

	return Comm_Compose_ConfigSymbol_Master(pDBPool, (const char* )zUserID, (const char* )zAcc, pzRslt, pnRslt, pzErrMsg);
}

BOOL Comm_Compose_ConfigSymbol_Master(void* pDBPool, const char* pzUserID, const char* pzMT4Acc, 
									_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg)
{
	BOOL bRet;
	*pzRslt = 0;
	*pnRslt = 0;

	CDBHandlerAdo db(((CDBPoolAdo*)pDBPool)->Get());
	wchar_t wzQ[1024] = { 0, };
	wchar_t wzUserID[128] = { 0, }, wzAcc[128] = { 0, };
	A2U((char*)pzUserID, wzUserID);
	A2U((char*)pzMT4Acc, wzAcc);

	_stprintf(wzQ, TEXT("EXEC EA_RQST_CONFIG_SYMBOL_MASTER '%s', '%s' "), wzUserID, wzAcc);

	//
	// RETURN 
	//

	char zTime[128] = { 0, };
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_CONFIG_SYMBOL);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pzUserID);
	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);
	
	int nSymbolCnt;
	if (FALSE == db->ExecQuery(wzQ))
	{
		_stprintf(pzErrMsg, TEXT("Failed Get Master Symbol(%s)"), wzQ);
		set.SetVal(FDN_RSLT_CODE, E_SYS_DB_EXCEPTION);
		*pnRslt = set.Complete(pzRslt);
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
		nSymbolCnt = 0;
		wchar_t wzVal[128], wzColumn[128];

		char zVal[128], zCol[128];
		char zSymbolArray[1024] = { 0, };
		if (db->IsNextRow())
		{
			for (int i = 0; i < SYMBOL_CONFIG_CNT; i++)
			{
				ZeroMemory(wzVal, sizeof(wzVal));
				ZeroMemory(wzColumn, sizeof(wzColumn));
				_stprintf(wzColumn, TEXT("SYMBOL%d"), i + 1);

				db->GetStrWithLen(wzColumn, 32, wzVal);
				if (_tcslen(wzVal) > 0) 
				{
					U2A(wzColumn, zCol);
					U2A(wzVal, zVal);
					nSymbolCnt++;
					strcat(zSymbolArray, zVal);
					zSymbolArray[strlen(zSymbolArray)] = DEF_DELI_ARRAY;
				}
			}
			if (zSymbolArray[strlen(zSymbolArray) - 1] == DEF_DELI_ARRAY)
				zSymbolArray[strlen(zSymbolArray) - 1] = 0x00;
		}

		strcpy(pzRslt,zSymbolArray);
		set.SetVal(FDN_RSLT_CODE, ERR_OK);
		set.SetVal(FDN_ARRAY_SIZE, nSymbolCnt);
		set.SetVal(FDS_ARRAY_SYMBOL, pzRslt);

		*pnRslt = set.Complete(pzRslt);

		//RequestSendIO(pCK, zSendBuff, nLen);
	}
	db->Close();

	return bRet;
}


BOOL Comm_Compose_ConfigSymbol_CopierW(void* pDBPool, const wchar_t* pzUserID, const wchar_t* pzMT4Acc,
	_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg)
{

	char zUserID[128] = { 0, };
	char zAcc[128] = { 0, };
	U2A((TCHAR*)pzUserID, zUserID);
	U2A((TCHAR*)pzMT4Acc, zAcc);

	return Comm_Compose_ConfigSymbol_Copier(pDBPool, (const char*)zUserID, (const char*)zAcc, pzRslt, pnRslt, pzErrMsg);
}

BOOL Comm_Compose_ConfigSymbol_Copier(void* pDBPool, const char* pzUserID, const char* pzMT4Acc,
	_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg)
{
	BOOL bRet;
	*pzRslt = 0;
	*pnRslt = 0;

	CDBHandlerAdo db(((CDBPoolAdo*)pDBPool)->Get());
	wchar_t wzQ[1024] = { 0, };
	wchar_t wzUserID[128] = { 0, }, wzAcc[128] = { 0, };
	A2U((char*)pzUserID, wzUserID);
	A2U((char*)pzMT4Acc, wzAcc);
	
	_stprintf(wzQ, TEXT("EXEC EA_RQST_CONFIG_SYMBOL_COPIER '%s','%s'"), wzUserID, wzAcc);

	//
	// RETURN 
	//

	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[128] = { 0, };
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_CONFIG_SYMBOL);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pzUserID);
	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);

	int nOrdersCnt;
	if (FALSE == db->ExecQuery(wzQ))
	{
		_stprintf(pzErrMsg, TEXT("Failed Get Copier Symbol(%s)"), wzQ);
		set.SetVal(FDN_RSLT_CODE, E_SYS_DB_EXCEPTION);
		*pnRslt = set.Complete(pzRslt);
		bRet = FALSE;
	}
	else
	{
		_tcscpy(pzErrMsg, wzQ);
		bRet = TRUE;
		nOrdersCnt = 0;
		int nLoop = 0, nCnt = 0;
		wchar_t wzC[128] = { 0, }, wzM[128] = { 0, };
		wchar_t wzFieldC[128] = { 0, }, wzFieldM[128] = { 0, };

		char zSymbolArray[1024] = { 0, };
		char zC[128], zM[128], zOneSymbol[128] = { 0, };

		if (db->IsNextRow())
		{
			for (int i = 0; i < SYMBOL_CONFIG_CNT; i++)
			{
				ZeroMemory(wzC, sizeof(wzC));
				ZeroMemory(wzM, sizeof(wzM));
				_stprintf(wzFieldC, TEXT("SYMBOL%d_COPIER"), i+1);
				_stprintf(wzFieldM, TEXT("SYMBOL%d_MASTER"), i+1);
				db->GetStrWithLen(wzFieldC, 32, wzC);
				db->GetStrWithLen(wzFieldM, 32, wzM);
				
				if ((_tcslen(wzC) > 0) && (_tcslen(wzM) > 0))
				{
					U2A(wzC, zC); U2A(wzM, zM);
					sprintf(zOneSymbol, "%s:%s", zM, zC);

					if (nCnt++ > 0) zSymbolArray[strlen(zSymbolArray)] = DEF_DELI_ARRAY;
					strcat(zSymbolArray, zOneSymbol);
				}
			}
			if(zSymbolArray[strlen(zSymbolArray) - 1]== DEF_DELI_ARRAY)
				zSymbolArray[strlen(zSymbolArray) - 1] = 0x00;
		}
		set.SetVal(FDN_RSLT_CODE, ERR_OK);
		set.SetVal(FDN_ARRAY_SIZE, nCnt);
		set.SetVal(FDS_ARRAY_SYMBOL, zSymbolArray);

		*pnRslt = set.Complete(pzRslt);
	}
	db->Close();

	return bRet;
}

BOOL Comm_Compose_ConfigGeneral_Master
(void* pDBPool, const char* pzUserID, const char* pzMT4Acc, _Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg)
{
	return TRUE;
}

BOOL Comm_Compose_ConfigGeneral_MasterW
(void* pDBPool, const wchar_t* pzUserID, const wchar_t* pzMT4Acc, _Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg)
{
	char zUserID[128] = { 0, };
	char zAcc[128] = { 0, };
	U2A((TCHAR*)pzUserID, zUserID);
	U2A((TCHAR*)pzMT4Acc, zAcc);
	return Comm_Compose_ConfigGeneral_Master(pDBPool, (const char*)zUserID, (const char*)zAcc, pzRslt, pnRslt, pzErrMsg);
}


BOOL Comm_Compose_ConfigGeneral_CopierW(void* pDBPool, const wchar_t* pzUserID, const wchar_t* pzMT4Acc, _Out_ char* pzRslt, _Out_ int* pnRsltLen, _Out_ TCHAR* pzErrMsg)
{
	char zUserID[128] = { 0, };
	char zAcc[128] = { 0, };
	U2A((TCHAR*)pzUserID, zUserID);
	U2A((TCHAR*)pzMT4Acc, zAcc);
	return Comm_Compose_ConfigGeneral_Copier(pDBPool, (const char*)zUserID, (const char*)zAcc, pzRslt, pnRsltLen, pzErrMsg);
}

BOOL Comm_Compose_ConfigGeneral_Copier(void* pDBPool, const char* pzUserID, const char* pzMT4Acc, _Out_ char* pzRslt, 
	_Out_ int* pnRsltLen, _Out_ TCHAR* pzErrMsg)
{
	BOOL bRet;
	*pzRslt = 0;
	*pnRsltLen = 0;

	CDBHandlerAdo db(((CDBPoolAdo*)pDBPool)->Get());
	wchar_t wzQ[1024] = { 0, };
	wchar_t wzUserId[128] = { 0, }, wzAcc[128] = { 0, };
	A2U((char*)pzUserID, wzUserId);
	A2U((char*)pzMT4Acc, wzAcc);

	_stprintf(wzQ, TEXT("EXEC EA_RQST_CONFIG_GENERAL_COPIER '%s','%s'"), wzUserId, wzAcc);

	//
	// RETURN 
	//

	char zSendBuff[MAX_BUF] = { 0, };
	char zTime[128] = { 0, };
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_CONFIG_GENERAL);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pzUserID);
	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);

	int nOrdersCnt;
	if (FALSE == db->ExecQuery(wzQ))
	{
		_stprintf(pzErrMsg, TEXT("Failed Get Copier General Config(%s)"), wzQ);
		set.SetVal(FDN_RSLT_CODE, E_SYS_DB_EXCEPTION);
		*pnRsltLen = set.Complete(pzRslt);
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
		nOrdersCnt = 0;
		wchar_t val[128];
		int n;
		double d;
		if (db->IsNextRow())
		{
			db->GetStrWithLen(TEXT("COPY_TP"), 32, val);			set.SetVal(FDS_COPY_TP, val);
			//db->GetStrWithLen("COPY_OPEN_YN", 32, val);		set.SetVal(FDS_COPY_OPEN_YN, val);
			
			db->GetStrWithLen(TEXT("ORD_MARKET_YN"), 32, val);		set.SetVal(FDS_MARKETORD_YN, val);
			db->GetStrWithLen(TEXT("ORD_CLOSE_YN"), 32, val);		set.SetVal(FDS_COPY_CLOSE_YN, val);
			db->GetStrWithLen(TEXT("ORD_PENDING_YN"), 32, val);		set.SetVal(FDS_COPY_PENDING_YN, val);
			db->GetStrWithLen(TEXT("ORD_LIMIT_YN"), 32, val);		set.SetVal(FDS_LIMITORD_YN, val);
			db->GetStrWithLen(TEXT("ORD_STOP_YN"), 32, val);		set.SetVal(FDS_STOPORD_YN, val);
			db->GetStrWithLen(TEXT("SL_YN"), 32, val);				set.SetVal(FDS_COPY_SL_YN, val);
			db->GetStrWithLen(TEXT("TP_YN"), 32, val);				set.SetVal(FDS_COPY_TP_YN, val);
			db->GetStrWithLen(TEXT("VOL_EQTYRATIO_MULTIPLE_YN"), 32, val);	set.SetVal(FDS_VOL_EQTYRATIO_MULTIPLE_YN, val);
			db->GetStrWithLen(TEXT("MAXLOT_ONEORDER_YN"), 32, val);set.SetVal(FDS_MAXLOT_ONEORDER_YN, val);
			db->GetStrWithLen(TEXT("MAXLOT_TOTORDER_YN"), 32, val);	set.SetVal(FDS_MAXLOT_TOTORDER_YN, val);
			db->GetStrWithLen(TEXT("MAX_SLPG_YN"), 32, val);		set.SetVal(FDS_MAX_SLPG_YN, val);
			db->GetStrWithLen(TEXT("MARGINLVL_LIMIT_YN"), 32, val);	set.SetVal(FDS_MARGINLVL_LIMIT_YN, val);
			db->GetStrWithLen(TEXT("TIMEFILTER_YN"), 32, val);		set.SetVal(FDS_TIMEFILTER_YN, val);
			db->GetStrWithLen(TEXT("TIMEFILTER_YN"), 32, val);		set.SetVal(FDS_TIMEFILTER_YN, val);

			n = db->GetLong(TEXT("SLTP_TYPE"));					set.SetVal(FDN_SLTP_TP, n);
			n = db->GetLong(TEXT("COPY_VOL_TP"));				set.SetVal(FDN_COPY_VOL_TP, n);
			n = db->GetLong(TEXT("MARGINLVL_LIMIT_ACTION"));	set.SetVal(FDN_MARGINLVL_LIMIT_ACTION, n);
			n = db->GetLong(TEXT("TIMEFILTER_H"));				set.SetVal(FDN_TIMEFILTER_H, n);
			n = db->GetLong(TEXT("TIMEFILTER_N"));				set.SetVal(FDN_TIMEFILTER_M, n);
				
			d = db->GetDbl(TEXT("VOL_MULTIPLIER_VAL"));			set.SetVal(FDD_VOL_MULTIPLIER_VAL, d);
			d = db->GetDbl(TEXT("VOL_FIXED_VAL"));				set.SetVal(FDD_VOL_FIXED_VAL, d);
			d = db->GetDbl(TEXT("VOL_EQTYRATIO_MULTIPLE_VAL"));	set.SetVal(FDD_VOL_EQTYRATIO_MULTIPLE_VAL, d);
			d = db->GetDbl(TEXT("MAXLOT_ONEORDER_VAL"));			set.SetVal(FDD_MAXLOT_ONEORDER_VAL, d);
			d = db->GetDbl(TEXT("MAXLOT_TOTORDER_VAL"));			set.SetVal(FDD_MAXLOT_TOTORDER_VAL, d);
			d = db->GetDbl(TEXT("MAX_SLPG_VAL"));					set.SetVal(FDD_MAX_SLPG_VAL, d);
			d = db->GetDbl(TEXT("MARGINLVL_LIMIT_VAL"));			set.SetVal(FDD_MARGINLVL_LIMIT_VAL, d);

		}

		set.SetVal(FDN_RSLT_CODE, ERR_OK);
		*pnRsltLen = set.Complete(pzRslt);
	}
	db->Close();

	return bRet;
}