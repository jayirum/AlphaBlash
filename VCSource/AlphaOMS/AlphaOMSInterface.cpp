// AlphaUtils.cpp : Defines the exported functions for the DLL application.
//

#define _ALPHA_OMS_EXPORTS
#include "../Common/AlphaInc.h"
#include "AlphaOMSInterface.h"
#include "OrdManager.h"
#include "../Common/Util.h"
#include <set>
#include "../Common/Log.h"

CLog		g_log;
COrdManager	g_oManager;

map<string, string>	g_mapMsg;
set<string>			g_setSymbols;
char				g_zFileDir[_MAX_PATH];


void AlphaOMS_Initialize(char* pzFileDir)
{
	strcpy(g_zFileDir, pzFileDir);
	//g_log.OpenLog(g_zFileDir, "OMS");
}


void AlphaOMS_DeInitialize()
{
	g_oManager.DeInitialize();
}


void AlphaOMS_LoadOpenOrders(
	int ticket
	, char* pzSymbol
	, int		type
	, double	lots
	, double	open_price
	, double	stoploss
	, double	takeprofit
	, int		expiry
)
{
	if (AlphaOMS_IsTradableSymbol(pzSymbol) != ERR_OK)
		return;

	g_oManager.LoadOpenOrders(ticket, type, lots, open_price, stoploss, takeprofit, expiry);
}

int AlphaOMS_Test(
	int		ticket
	, _Out_ double* pdPartialLots
	, _Out_ char* arrChgAction
)
{
	*pdPartialLots = 0;
	*pdPartialLots = ticket;
	strcpy(arrChgAction, "test");
	return 0;
}


__ALPHA::ORD_ACTION AlphaOMS_Add_CheckChange(
	int		ticket
	, char* pzSymbol
	, int		type
	, double	lots
	, double	open_price
	, double	stoploss
	, double	takeprofit
	, int		expiry
	, char*		open_time
	, _Out_		int* pnGroupKey
	, _Out_ double* pdPartialLots
	, _Out_ char* arrChgAction
)
{
	if (AlphaOMS_IsTradableSymbol(pzSymbol) != ERR_OK)
		return __ALPHA::ORD_ACTION_NONTRADE_SYMBOL;

	*pdPartialLots = 0.0;
	ZeroMemory(arrChgAction, CHG_ACTION_SIZE);
	
	return g_oManager.Check_Add_Change(
		ticket,pzSymbol, type,lots,open_price,stoploss,takeprofit,
		expiry, open_time, pnGroupKey, pdPartialLots, arrChgAction
	);
}
//int AlphaOMS_CheckClosed_GetTickets(_Out_ int* arrTicket, _Out_ int* pnCnt)
//{
//	return g_oManager.CheckClosed_GetTickets(arrTicket, pnCnt);
//}
//
//int AlphaOMS_CheckClosed_GetTicketsOrdLots(_Out_ int* arrTicket, _Out_ double* arrLots, _Out_ int* pnCnt)
//{
//	return g_oManager.CheckClosed_GetTicketsOrdLots(arrTicket, arrLots, pnCnt);
//}

int AlphaOMS_ClosedOrd_CheckAndGetInfo(_Out_ int* arrTicket, _Out_ int* arrGroupKey, _Out_ double* arrOrgOrdLots, _Out_ int* pnCnt)
{
	return g_oManager.ClosedOrd_CheckAndGetInfo(arrTicket, arrGroupKey, arrOrgOrdLots, pnCnt);
}

//int AlphaOMS_IsPartialClose(int nTicket, double dOrdLots, _Out_ char* pzPartialYN, _Out_ int* pnGroupKey)
//{
//	return g_oManager.IsPartialClose(nTicket, dOrdLots, pzPartialYN, pnGroupKey);
//}

int AlphaOMS_BeginCheck()
{
	return g_oManager.BeginCheck();
}

int	AlphaOMS_DeletedOrderCnt()
{
	return g_oManager.DeletedOrderCnt();
}


void AlphaOMS_TradableSymbols_Reset()
{
	g_setSymbols.clear();
}


int AlphaOMS_TradableSymbols_Set(char* pzSymbol)
{
	char z[128];
	strcpy(z, pzSymbol);
	CUtil::TrimAll(z, strlen(z));
	g_setSymbols.insert(string(z));
	return ERR_OK;
}

int AlphaOMS_IsTradableSymbol(char* pzSymbol)
{
	char z[128];
	strcpy(z, pzSymbol);
	CUtil::TrimAll(z, strlen(z));
	set<string>::iterator it = g_setSymbols.find(string(z));
	if (it == g_setSymbols.end())
		return E_NO_CODE;

	return ERR_OK;
}
