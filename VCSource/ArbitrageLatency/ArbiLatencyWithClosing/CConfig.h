#pragma once

#include <Windows.h>
#include <map>
#include <string>
#include <vector>
using namespace std;


#define SEC_SYMBOL_INFO	"SYMBOLS"
#define SEC_APP_CONFIG "APP_CONFIG"
#define SEC_TRADE_CONTROL "TRADE_CONTROL"
#define DEF_UPDATED_KEY	"UPDATED"

enum EN_CNFG_RET { CNFG_ERR = -1, CNFG_SUCC, CNFG_IDLE };

struct TSymbolInfo
{
	string 	sSymbol;
	double	dPtsSize;
	int 	nTargetPtsOpening;
	int 	nTargetPtsClosing;
	double 	dVolume;
	int 	nSlippagePts;
	
	TSymbolInfo()
	{
		dPtsSize = 0;
		nTargetPtsOpening = 0;
		nTargetPtsClosing = 0;
		dVolume = 0;
		nSlippagePts = 0;
	}	
};

struct TAppConfig
{
	string sListenIP;
	int 	nListenPort;
	int 	nWorkerThreadCnt;
	double 	dCutOffTimeRate;
	int		nTimeoutReadConfig;
	TAppConfig()
	{
		sListenIP = "";
		nListenPort = 0;
		nWorkerThreadCnt = 0;
		dCutOffTimeRate = 0;
		nTimeoutReadConfig = 0;
	}
};

struct TTradeControl
{
	string sTradeFrom;
	string sTradeTo;
	BOOL	bCloseOnly;
	int		nMagicNo;

	TTradeControl()
	{
		sTradeFrom = "";
		sTradeTo = "";
		bCloseOnly = FALSE;
		nMagicNo = 0;
	}
};

class CConfig
{
public:
	CConfig();
	~CConfig();
	
	VOID 	Initialize(string sFileName);
	EN_CNFG_RET	ReLoad_ConfigInfo(BOOL bInit = FALSE);

private:
	VOID	DeInitialize();
	BOOL	Is_Updated(char* pzSec);
	EN_CNFG_RET	ReLoad_SymbolInfo(BOOL bInit = FALSE);
	EN_CNFG_RET ReLoad_AppConfig(BOOL bInit = FALSE);
	EN_CNFG_RET ReLoad_TradeControl(BOOL bInit = FALSE);

public:
	char*	getFileName() { return m_zFileName;}
	char*	getListenIP() { return (char*)m_appConfig.sListenIP.c_str(); }
	int		getListenPort() { return m_appConfig.nListenPort; }
	int		getWorkThreadCnt() { return m_appConfig.nWorkerThreadCnt; }
	INT		getSymbols(_Out_ vector<string>* vec);
	BOOL	getSymbolInfo(string sSymbol, _Out_ TSymbolInfo* pInfo);
	char* getTradeTimeFrom() { return (char*)m_TradeControl.sTradeFrom.c_str(); }
	char* getTradeTimeTo() { return (char*)m_TradeControl.sTradeTo.c_str(); }
	int getTimeoutReadCnfg() { return m_appConfig.nTimeoutReadConfig; }
	int getMagicNo() { return m_TradeControl.nMagicNo; }
private:	
	char 						m_zFileName[MAX_PATH];
	map<string, TSymbolInfo*>	m_mapSymbols;
	vector<string>				m_vecSymbols;
	TAppConfig					m_appConfig;
	TTradeControl				m_TradeControl;
	char 						m_zMsg[1024];
	
	
};
