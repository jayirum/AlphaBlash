#pragma once

#pragma warning(disable:4996)

#include "AlphaOMSInterface.h"
#include <map>
using namespace std;

#define TICKET_NO	int

class COrdManager
{
public:
	COrdManager();
	~COrdManager();
	
	void DeInitialize();

	int BeginCheck();
	int	DeletedOrderCnt();

	__ALPHA::ORD_ACTION Check_Add_Change(
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
		, _Out_		double* pdPartialLots
		, _Out_		char* arrChgAction
	);
	//int CheckClosed_GetTickets(_Out_ int* arrTicket, _Out_ int* pnCnt);
	//int CheckClosed_GetTicketsOrdLots(_Out_ int* arrTicket, _Out_ double* arrOrdLots, _Out_ int* pnCnt);
	int ClosedOrd_CheckAndGetInfo(_Out_ int* arrTicket, _Out_ int* arrGroupKeys, _Out_ double* arrOrgOrdLots, _Out_ int* pnCnt);

	int	LoadOpenOrders(int ticket
		, int		type
		, double	lots
		, double	open_price
		, double	stoploss
		, double	takeprofit
		, int		expiry
	);

private:
	int		AddNewOrder(EN_CHECK_FLAG flag, _Out_ int* pnGroupKey);
	int		Check_Get_OrderGroupKey();
	
	bool	Does_AlreadyExist(map<TICKET_NO, MT4_ORD*>::iterator it) {
		return (it != m_mapOrdLast.end());
	}

private:
	MT4_ORD					m_inputOrd;
	map<TICKET_NO, MT4_ORD*> m_mapOrdLast;
	bool					m_bDeInit;

	int						m_nUnCheckedCnt;
};

