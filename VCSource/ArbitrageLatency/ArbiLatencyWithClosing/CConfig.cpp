#include "CConfig.h"
#include "../../Common/AlphaInc.h"
#include "../../CommonAnsi/Util.h"
#include "Inc.h"


CConfig::CConfig()
{
}

CConfig::~CConfig()
{
	DeInitialize();
}

VOID CConfig::Initialize(string sFileName)
{
	strcpy(m_zFileName,sFileName.c_str());
}



EN_CNFG_RET CConfig::ReLoad_AppConfig(BOOL bInit)
{
	if (bInit == FALSE && !Is_Updated(SEC_APP_CONFIG))
		return CNFG_IDLE;

	if (!CUtil::SetConfigValue(getFileName(), SEC_APP_CONFIG, DEF_UPDATED_KEY, "N"))
		return CNFG_ERR;

	char zIP[1024] = { 0, }, zPort[128] = { 0, };
	CUtil::GetConfig(getFileName(), TEXT("APP_CONFIG"), TEXT("LISTEN_IP"), zIP);
	if(zIP[0]==NULL)
		return CNFG_ERR;

	CUtil::GetConfig(getFileName(), TEXT("APP_CONFIG"), TEXT("LISTEN_PORT"), zPort);
	if (zIP[0] == NULL)
		return CNFG_ERR;

	char zTimeout[32];
	CUtil::GetConfig(getFileName(), TEXT("APP_CONFIG"), TEXT("TIMEOUT_READ_CONFIG_SEC"), zTimeout);
	if (zTimeout[0] == NULL)
		return CNFG_ERR;

	m_appConfig.sListenIP = zIP;
	m_appConfig.nListenPort = atoi(zPort);
	m_appConfig.nTimeoutReadConfig = atoi(zTimeout);

	return CNFG_SUCC;
}



EN_CNFG_RET CConfig::ReLoad_TradeControl(BOOL bInit)
{
	if (bInit == FALSE && !Is_Updated(SEC_TRADE_CONTROL))
		return CNFG_IDLE;

	if (!CUtil::SetConfigValue(getFileName(), SEC_TRADE_CONTROL, DEF_UPDATED_KEY, "N"))
		return CNFG_ERR;


	char zTimeFrom[32] = {}, zTimeTo[32] = {};
	CUtil::GetConfig(getFileName(), SEC_TRADE_CONTROL, TEXT("TRADETIME_FROM"), zTimeFrom);
	if (zTimeFrom[0] == NULL)
	{
		sprintf(m_zMsg, "Failed to get TRADETIME_FROM from config file");
		return CNFG_ERR;
	}
	CUtil::GetConfig(getFileName(), SEC_TRADE_CONTROL, TEXT("TRADETIME_TO"), zTimeTo);
	if (zTimeTo[0] == NULL)
	{
		sprintf(m_zMsg, "Failed to get TRADETIME_TO from config file");
		return CNFG_ERR;
	}

	char zCloseOnly[32];
	CUtil::GetConfig(getFileName(), SEC_TRADE_CONTROL, "CLOSE_ONLY", zCloseOnly);
	if (zCloseOnly[0] == NULL)
	{
		sprintf(m_zMsg, "Failed to get CLOSE_ONLY from config file");
		return CNFG_ERR;
	}

	char zMagicNo[32];
	CUtil::GetConfig(getFileName(), SEC_TRADE_CONTROL, "MAGIC_NO", zMagicNo);
	if (zMagicNo[0] == NULL)
	{
		sprintf(m_zMsg, "Failed to get MAGIC_NO from config file");
		return CNFG_ERR;
	}

	m_TradeControl.sTradeFrom = zTimeFrom;
	m_TradeControl.sTradeTo = zTimeTo;
	m_TradeControl.bCloseOnly = (zCloseOnly[0] == 'Y');
	m_TradeControl.nMagicNo = atoi(zMagicNo);

	return CNFG_SUCC;
}


VOID CConfig::DeInitialize()
{
	map<string, TSymbolInfo*>::iterator it;
	for (it = m_mapSymbols.begin(); it != m_mapSymbols.end(); ++it)
		delete (*it).second;
	m_mapSymbols.clear();
}

BOOL CConfig::Is_Updated(char* pzSec)
{
	char zUpdated[128] = { 0 };
	CUtil::GetConfig(getFileName(), pzSec, DEF_UPDATED_KEY, zUpdated);
	return (zUpdated[0] == 'Y');
}


EN_CNFG_RET	CConfig::ReLoad_ConfigInfo(BOOL bInit)
{
	EN_CNFG_RET ret1, ret2, ret3;
	ret1 = ReLoad_SymbolInfo(bInit);
	ret2 = ReLoad_AppConfig(bInit);
	ret3 = ReLoad_TradeControl(bInit);

	if (ret1 == CNFG_ERR || ret2 == CNFG_ERR || ret3 == CNFG_ERR)
		return CNFG_ERR;

	if (ret1 == CNFG_SUCC || ret2 == CNFG_SUCC || ret3 == CNFG_SUCC)
		return CNFG_SUCC;

	return CNFG_IDLE;
}

EN_CNFG_RET CConfig::ReLoad_SymbolInfo(BOOL bInit)
{
	if (bInit == FALSE && !Is_Updated(SEC_SYMBOL_INFO))
		return CNFG_IDLE;


	if (!CUtil::SetConfigValue(getFileName(), SEC_SYMBOL_INFO, DEF_UPDATED_KEY, "N"))
		return CNFG_ERR;

	char buffer[128] = { 0 };
	CUtil::GetConfig(getFileName(), SEC_SYMBOL_INFO, "COUNT", buffer);
	if (buffer[0] == NULL)
	{
		sprintf(m_zMsg, "Failed to get symbols count from config file");
		return CNFG_ERR;
	}

	char zIdx[32];
	int nCnt = atoi(buffer);

	// ArbTraceLatency.ini
	int IDX_SYMBOL = 0;
	int IDX_POINT_SIZE = 1;
	int IDX_NETPROFIT_OPEN = 2;
	int IDX_NETPROFIT_CLOSE = 3;
	int IDX_VOLUME = 4;
	int IDX_SLIPPAGE_PTS = 5;

	for (int i = 0; i < nCnt; i++)
	{
		char zLine[512] = { 0 };
		sprintf(zIdx, "%d", i);
		CUtil::GetConfig(getFileName(), TEXT("SYMBOLS"), zIdx, zLine);
		vector<string> vec;
		CUtil::SplitData(zLine, '/', &vec);

		TSymbolInfo* p = new TSymbolInfo;
		p->sSymbol				= vec.at(IDX_SYMBOL);
		p->dPtsSize				= atof(vec.at(IDX_POINT_SIZE).c_str());
		p->nTargetPtsOpening	= atoi(vec.at(IDX_NETPROFIT_OPEN).c_str());
		p->nTargetPtsClosing	= atoi(vec.at(IDX_NETPROFIT_CLOSE).c_str());
		p->dVolume				= atof(vec.at(IDX_VOLUME).c_str());
		p->nSlippagePts			= atoi(vec.at(IDX_SLIPPAGE_PTS).c_str());

		m_mapSymbols[p->sSymbol] = p;

		m_vecSymbols.push_back(p->sSymbol);
	}

	return CNFG_SUCC;
}

BOOL CConfig::getSymbolInfo(string sSymbol, _Out_ TSymbolInfo* pInfo)
{
	map<string, TSymbolInfo*>::iterator it = m_mapSymbols.find(sSymbol);
	if (it == m_mapSymbols.end())
		return FALSE;

	CopyMemory(pInfo, (*it).second, sizeof(TSymbolInfo));

	return TRUE;
}

INT	CConfig::getSymbols(_Out_ vector<string>* vec) 
{ 
	*vec = m_vecSymbols; 
	return m_vecSymbols.size(); 
}