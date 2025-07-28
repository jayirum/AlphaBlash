#pragma once

#ifdef _ALPHA_OMS_EXPORTS
#define ALPHA_OMS extern "C" __declspec(dllexport)
#else
#define ALPHA_OMS extern "C" __declspec(dllimport)
#endif

#include "../Common/AlphaInc.h"
//#include <string>
using namespace std;

#define FORMAT_PRICE(buf,prc) sprintf(buf, "%010.5f",prc);

enum EN_CHECK_FLAG {
	FLAG_NONE = 0,
	FLAG_CHECKED,
	FLAG_DELETED
};

struct MT4_ORD
{
	int         ticket;
	int			groupKey;
	char		symbol[32];
	int         type;
	char		open_price[32];
	char		open_time[32];
	double      lots;
	double      stoploss;
	double      takeprofit;
	int			expiry;
	EN_CHECK_FLAG	enFlag;
};

//+------------------------------------------------------------+
//	COrdManager
//+------------------------------------------------------------+
ALPHA_OMS void AlphaOMS_Initialize(char* pzFileDir);
ALPHA_OMS void AlphaOMS_DeInitialize();

ALPHA_OMS void AlphaOMS_LoadOpenOrders(
	int ticket
	, char*		pzSymbol
	, int		type
	, double	lots
	, double	open_price
	, double	stoploss
	, double	takeprofit
	, int		expiry
);

ALPHA_OMS __ALPHA::ORD_ACTION AlphaOMS_Add_CheckChange(
	int		ticket
	, char*		pzSymbol
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
);

ALPHA_OMS int AlphaOMS_Test(
	int		ticket
	, _Out_ double* pdPartialLots
	, _Out_ char* arrChgAction
);


//ALPHA_OMS int AlphaOMS_CheckClosed_GetTickets(_Out_ int* arrTicket, _Out_ int* pnCnt);
//ALPHA_OMS int AlphaOMS_CheckClosed_GetTicketsOrdLots(_Out_ int* arrTicket, _Out_ double* arrLots, _Out_ int* pnCnt);
ALPHA_OMS int AlphaOMS_ClosedOrd_CheckAndGetInfo(_Out_ int* arrTicket, _Out_ int* arrGroupKey, _Out_ double* arrOrgOrdLots, _Out_ int* pnCnt);

//ALPHA_OMS int AlphaOMS_IsPartialClose(int nTicket, double dOrdLots, _Out_ char* pzPartialYN, _Out_ int* pnGroupKey);

// return BP_RESULT
ALPHA_OMS int AlphaOMS_BeginCheck();

// return count
ALPHA_OMS int AlphaOMS_DeletedOrderCnt();

//+------------------------------------------------------------+
//	Master Tradable Symbols
//+------------------------------------------------------------+
ALPHA_OMS void AlphaOMS_TradableSymbols_Reset();
ALPHA_OMS int AlphaOMS_TradableSymbols_Set(char* pzSymbol);
ALPHA_OMS int AlphaOMS_IsTradableSymbol(char* pzSymbol);
