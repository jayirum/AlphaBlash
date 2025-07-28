#include "OrdManager.h"
#include "../Common/Log.h"

extern CLog		g_log;


COrdManager::COrdManager()
{
	m_bDeInit = false;
	m_nUnCheckedCnt = 0;
	ZeroMemory(&m_inputOrd, sizeof(m_inputOrd));
}


COrdManager::~COrdManager()
{
	DeInitialize();
	
}

void COrdManager::DeInitialize()
{
	if (m_bDeInit)
		return;

	map<TICKET_NO, MT4_ORD*>::iterator it;
	if (m_mapOrdLast.size() > 0)
	{
		for (it = m_mapOrdLast.begin(); it != m_mapOrdLast.end(); it++)
		{
			delete (*it).second;
		}
		m_mapOrdLast.clear();
	}
	m_bDeInit = true;
}


int COrdManager::AddNewOrder(EN_CHECK_FLAG flag, _Out_ int* pnGroupKey)
{
	MT4_ORD* p = new MT4_ORD;
	memcpy(p, &m_inputOrd, sizeof(MT4_ORD));

	// 신규주문이 Partial 인 경우 기존 원주문의 groupkey 를 가져온다.
	// partial 관련 신규이면 기존원주문의 groupkey, 완전 신규이면 자기 ticket
	int nGroupKey = Check_Get_OrderGroupKey();
	if (nGroupKey == 0) 
	{
		nGroupKey = m_inputOrd.ticket;
	}
	*pnGroupKey = nGroupKey;
	p->groupKey = nGroupKey;
	p->enFlag = flag;

	m_mapOrdLast[p->ticket] = p;

	//g_log.log("[NEW](%d)(symbol:%s)(type:%d)(openprc:%s)(opentime:%s)",p->ticket, p->symbol, p->type, p->open_price, p->open_time);
	
	return __ALPHA::ORD_ACTION_OPEN;
}

int	COrdManager::Check_Get_OrderGroupKey()
{
	int nGroupKey = 0;
	map<TICKET_NO, MT4_ORD*>::iterator it;

	for (it = m_mapOrdLast.begin(); it != m_mapOrdLast.end(); )
	{
		MT4_ORD* p = (*it).second;

		if (p->ticket == m_inputOrd.ticket)
		{
			it++;
			continue;
		}

		if (p->type == m_inputOrd.type)
		{
			if (strcmp(p->symbol, m_inputOrd.symbol) == 0)
			{
				if (strcmp(p->open_price, m_inputOrd.open_price) == 0)
				{
					if (strcmp(p->open_time, m_inputOrd.open_time) == 0)
					{
						nGroupKey = p->groupKey;
					}
				}
			}
		}
		if (p->enFlag == FLAG_DELETED)
		{
			delete p;
			it = m_mapOrdLast.erase(it);
		}
		else
			it++;
	}
	
	return nGroupKey;
}

/*
return
	
*/
__ALPHA::ORD_ACTION COrdManager::Check_Add_Change(
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
	ZeroMemory(&m_inputOrd, sizeof(m_inputOrd));
	m_inputOrd.ticket		= ticket;
	m_inputOrd.type			= type;
	m_inputOrd.lots			= lots;
	m_inputOrd.stoploss		= stoploss;
	m_inputOrd.takeprofit	= takeprofit;
	m_inputOrd.expiry		= expiry;
	strcpy(m_inputOrd.symbol, pzSymbol);
	strcpy(m_inputOrd.open_time, open_time);
	FORMAT_PRICE(m_inputOrd.open_price, open_price);

	memset(arrChgAction, 0x20, CHG_ACTION_SIZE);
	*pdPartialLots = 0;

	// Does this order already exist in the Map?

	map<TICKET_NO, MT4_ORD*>::iterator it = m_mapOrdLast.find(ticket);
	bool bExist = Does_AlreadyExist(it);

	if (bExist==DEF_NO)
	{
		AddNewOrder( FLAG_CHECKED, pnGroupKey);  // mark as checked in order not to recognize as deleted order
		
		return __ALPHA::ORD_ACTION_OPEN;
	}// end.

	// Yes. This is already in the map ==> check the change.
	
	__ALPHA::ORD_ACTION	ret = __ALPHA::ORD_ACTION_NONE;

	MT4_ORD *pLast = (*it).second;

	*pnGroupKey = pLast->groupKey;

	// check partial
	if (pLast->lots > m_inputOrd.lots)
	{
		*pdPartialLots = pLast->lots - m_inputOrd.lots;
		pLast->lots = m_inputOrd.lots;
		ret = __ALPHA::ORD_ACTION_CLOSE_PARTIAL;
	}
	else
	{
		// open price (pending order price 정정의 경우)
		if ( strcmp(pLast->open_price, m_inputOrd.open_price)!=0 )
		{
			strcpy(pLast->open_price, m_inputOrd.open_price);
			arrChgAction[__ALPHA::IDX_CHG_OPEN_PRC] = '1';	// TRUE
			ret = __ALPHA::ORD_ACTION_CHANGE;
		}
		// SL
		if (pLast->stoploss != m_inputOrd.stoploss)
		{
			pLast->stoploss = m_inputOrd.stoploss;
			arrChgAction[__ALPHA::IDX_CHG_SL] = '1';	// TRUE
			ret = __ALPHA::ORD_ACTION_CHANGE;
		}
		// SL/TP
		if (pLast->takeprofit != m_inputOrd.takeprofit)
		{
			pLast->takeprofit = m_inputOrd.takeprofit;
			arrChgAction[__ALPHA::IDX_CHG_TP] = '1';	// TRUE
			ret = __ALPHA::ORD_ACTION_CHANGE;
		}
		// expiry
		if (pLast->expiry != m_inputOrd.expiry)
		{
			pLast->expiry = m_inputOrd.expiry;
			arrChgAction[__ALPHA::IDX_CHG_EXPIRY] = '1';	// TRUE
			ret = __ALPHA::ORD_ACTION_CHANGE;
		}

	}

	m_nUnCheckedCnt--;

	pLast->enFlag = FLAG_CHECKED;
	m_mapOrdLast[ticket] = pLast;
	return ret;
}




// Orders which Deleted from MT4 Trade tab 
int COrdManager::ClosedOrd_CheckAndGetInfo(_Out_ int* arrTicket, _Out_ int* arrGroupKeys, _Out_ double* arrOrgOrdLots, _Out_ int* pnCnt)
{
	*pnCnt = 0;

	if (m_mapOrdLast.size() == 0) {
		return ERR_OK;
	}

	int nDeletedCnt = 0;
	map<TICKET_NO, MT4_ORD*>::iterator itLast;
	for (itLast = m_mapOrdLast.begin(); itLast != m_mapOrdLast.end(); ++itLast)
	{
		int nTicket = (*itLast).first;
		MT4_ORD* p = (*itLast).second;
			
		if (p->enFlag == FLAG_CHECKED)
		{
			p->enFlag = FLAG_NONE;
			m_mapOrdLast[nTicket] = p;
		}
		else if (p->enFlag == FLAG_DELETED)
		{
			//DO Nothing.
			//m_mapOrdLast[(*itLast).first] = p;
		}
		else if (p->enFlag == FLAG_NONE)
		{
			arrTicket[nDeletedCnt] = p->ticket;
			arrGroupKeys[nDeletedCnt] = p->groupKey;
			arrOrgOrdLots[nDeletedCnt] = p->lots;
			nDeletedCnt++;

			p->enFlag = FLAG_DELETED;
			m_mapOrdLast[nTicket] = p;
		}
	}

	*pnCnt = nDeletedCnt;
	return ERR_OK;
}


int	COrdManager::LoadOpenOrders(int ticket
	, int		type
	, double	lots
	, double	open_price
	, double	stoploss
	, double	takeprofit
	, int		expiry
)
{
	ZeroMemory(&m_inputOrd, sizeof(m_inputOrd));
	m_inputOrd.ticket = ticket;
	m_inputOrd.type = type;
	FORMAT_PRICE(m_inputOrd.open_price, open_price);
	m_inputOrd.stoploss = stoploss;
	m_inputOrd.takeprofit = takeprofit;
	m_inputOrd.expiry = expiry;

	int nGroupKey = 0;
	return AddNewOrder(FLAG_NONE, &nGroupKey);
}



int COrdManager::BeginCheck()
{
	map<TICKET_NO, MT4_ORD*>::iterator itLast;
	for (itLast = m_mapOrdLast.begin(); itLast != m_mapOrdLast.end(); itLast++)
	{
		if (((*itLast).second)->enFlag != FLAG_DELETED)
		{
			((*itLast).second)->enFlag = FLAG_NONE;
			m_mapOrdLast[(*itLast).first] = (*itLast).second;
		}
	}
	m_nUnCheckedCnt = m_mapOrdLast.size();
	return ERR_OK;
}



int	COrdManager::DeletedOrderCnt() 
{ 
	return m_nUnCheckedCnt; 
}

//
//int COrdManager::CheckClosed_GetTickets(int* arrTicket, int* pnCnt)
//{
//	*pnCnt = 0;
//
//	if (m_mapOrdLast.size() == 0) {
//		return ERR_OK;
//	}
//
//	int nDeletedCnt = 0;
//	map<TICKET_NO, MT4_ORD*>::iterator itLast;
//	for (itLast = m_mapOrdLast.begin(); itLast != m_mapOrdLast.end();)
//	{
//		MT4_ORD* p = (*itLast).second;
//		//if (p->bAlreadChecked == Marked_AsAlreadyChecked() )
//		if (p->enFlag == FLAG_CHECKED )
//		{
//			//p->bAlreadChecked = !Marked_AsAlreadyChecked();
//			p->enFlag = FLAG_NONE;
//			m_mapOrdLast[(*itLast).first] = p;
//			++itLast;
//			continue;
//		}
//
//		arrTicket[nDeletedCnt] = p->ticket;
//		nDeletedCnt++;
//
//		//g_log.log(INFO, "[Close/Delete]Ticket:%d", p->ticket);
//
//		// 여기서 지우지 않고, IsPartialClose 여기서 지운다.
//		//delete p;
//		//itLast = m_mapOrdLast.erase(itLast);
//		//m_nUnCheckedCnt--;
//	}
//
//	*pnCnt = nDeletedCnt;
//	return ERR_OK;
//}
//
//
//
//int COrdManager::CheckClosed_GetTicketsOrdLots(_Out_ int* arrTicket, _Out_ double* arrOrdLots, _Out_ int* pnCnt)
//{
//	*pnCnt = 0;
//
//	if (m_mapOrdLast.size() == 0) {
//		return ERR_OK;
//	}
//
//	int nDeletedCnt = 0;
//	map<TICKET_NO, MT4_ORD*>::iterator itLast;
//	for (itLast = m_mapOrdLast.begin(); itLast != m_mapOrdLast.end();)
//	{
//		MT4_ORD* p = (*itLast).second;
//		//if (p->bAlreadChecked == Marked_AsAlreadyChecked())
//		if(p->enFlag == FLAG_CHECKED)
//		{
//			p->enFlag = FLAG_NONE;
//			m_mapOrdLast[(*itLast).first] = p;
//			++itLast;
//			continue;
//		}
//
//		arrTicket[nDeletedCnt] = p->ticket;
//		arrOrdLots[nDeletedCnt] = p->lots;
//		nDeletedCnt++;
//
//		p->enFlag = FLAG_DELETED;
//		m_mapOrdLast[(*itLast).first] = p;
//
//
//		//g_log.log(INFO, "[Close/Delete]Ticket:%d", p->ticket);
//
//		// IsPartialClose 에서 지운다.
//		//delete p;
//		//itLast = m_mapOrdLast.erase(itLast);
//		//m_nUnCheckedCnt--;
//	}
//
//	*pnCnt = nDeletedCnt;
//	return ERR_OK;
//}
//
//int COrdManager::IsPartialClose(int nTicket, double dOrdLots, _Out_ char* pzPartialYN, _Out_ int* pnGroupKey)
//{
//	strcpy(pzPartialYN, "N");
//	*pnGroupKey = 0;
//
//	map<TICKET_NO, MT4_ORD*>::iterator it = m_mapOrdLast.find(nTicket);
//	if (it != m_mapOrdLast.end())
//	{
//		strcpy(pzPartialYN, "Y");
//		double dLots = ((*it).second)->lots;
//		if( dLots > dOrdLots )
//			*pnGroupKey = ((*it).second)->groupKey;
//		
//		delete (*it).second;
//		m_mapOrdLast.erase(it);
//	}
//
//	return ERR_OK;
//}
