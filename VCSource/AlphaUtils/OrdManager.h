#pragma once

#pragma warning(disable:4996)

#include "AlphaUtils.h"
#include <map>
using namespace std;


class COrdManager
{
public:
	COrdManager();
	~COrdManager();

	int BeginCheck();
	int	DeletedOrderCnt();


	int	AddNewOrder(
		int		ticket
		, char* symbol
		, int		type
		, double	lots
		, int		open_time
		, double	open_price
		, double	stoploss
		, double	takeprofit
		, int		close_time
		, double	close_price
		, double	commission
		, double	swap
		, double	profit
		, char*		comment
		, int		magic
		, bool		bChecked
	);

	CHANGED_RET CheckChange(
		int		ticket
		, char* symbol
		, int		type
		, double	lots
		, int		open_time
		, double	open_price
		, double	stoploss
		, double	takeprofit
		, int		close_time
		, double	close_price
		, double	commission
		, double	swap
		, double	profit
		, char*		comment
		, int		magic
		, /*out*/int&		refOpenedTicket
		, /*out*/int&		refOpenedOrdType
		, /*out*/double&	refOpenedPrc
		, /*out*/double&	refOpenedLots
		, /*out*/int&		refOpenedTm
	);

	int GetClosedOrd(
		int*	arrTicket,
		char*	arrSymbol,
		double* arrOpenedPrc,
		int*	arrOrdType,
		double* arrLots,
		int*	arrOpenedTime,
		int*	pnCnt
	);

	int GetClosedOrdTicket(int* arrTicket, int* pnCnt);

	int GetSymbol(int nTicket, _Out_ char* zSymbol);

private:
	void	Clear();

private:
	map<int, MT4_ORD*>		m_mapOrdLast;
	bool					m_bDeInit;

	int						m_nUnCheckedCnt;
};

