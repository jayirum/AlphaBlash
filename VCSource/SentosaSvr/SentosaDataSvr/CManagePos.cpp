#include "CManagePos.h"
#include "../../Common/AlphaInc.h"

extern CConfig g_config;

CManagePos::CManagePos()
{
}


CManagePos::~CManagePos()
{
}



VOID CManagePos::AddOpenPos(string sBrokerKey, int nTicket, char cBuySellTp, double dAvg, double dLots)
{
	TPosInfo* pos = NULL;
	char cSide = cBuySellTp;
	if (cSide == __ALPHA::DEF_BUY)
		pos = &m_l;
	else
		pos = &m_s;


	strcpy(pos->zBrokerKey,sBrokerKey.c_str());
	pos->cSide[0] = cSide;
	pos->dAvg = dAvg;
	pos->nTicket = nTicket;
	pos->dLots = dLots;
	pos->nLastProfitPts = 0;

	//m_bPosOpened = TRUE;

	LOGGING(INFO, TRUE, "[%s]AddOpenPos (%s)(%c)(Ticket:%d)(Avg:%.5f)(Lots:%.2f)(TargetOpen:%d)(TargetClose:%d)",
		m_symbolInfo.sSymbol.c_str(), sBrokerKey.c_str(), cSide, nTicket, dAvg, dLots, m_symbolInfo.nTargetPtsOpening, m_symbolInfo.nTargetPtsClosing);
}


BOOL CManagePos::CheckProfit(
	string sBrokerKey, 
	double dBid, 
	double dAsk
)
{
	TPosInfo* pos = NULL;
	if (sBrokerKey.compare( m_l.zBrokerKey)==0 )
	{
		pos = &m_l;
		//pos->dLastTick = dBid;
		pos->nLastProfitPts = (int)((dBid - pos->dAvg) / m_symbolInfo.dPtsSize);
	}
	else if (sBrokerKey.compare(m_s.zBrokerKey) == 0)
	{
		pos = &m_s;
		//pos->dLastTick = dAsk;
		pos->nLastProfitPts = (int)((pos->dAvg - dAsk) / m_symbolInfo.dPtsSize);
	}
	else
	{
		return FALSE;
	}

	int nSum = m_l.nLastProfitPts + m_s.nLastProfitPts;
	//if (nSum > 0)
	//{
	//	LOGGING(INFO, TRUE, "[%s]Profit > 0 [L](%s)(PL:%d) [S](%s)(PL:%d) ==> %d",
	//		m_symbolInfo.sSymbol.c_str(),
	//		m_l.zBrokerKey, m_l.nLastProfitPts,
	//		m_s.zBrokerKey, m_s.nLastProfitPts,
	//		nSum
	//	);

	//}
	if (nSum < m_symbolInfo.nTargetPtsClosing)
		return FALSE;

	LOGGING(INFO, TRUE, "[%s]CalcProfits Fire [L](%s)(PL:%d) [S](%s)(PL:%d) ==> %d>%d",
		m_symbolInfo.sSymbol.c_str(),
		m_l.zBrokerKey, m_l.nLastProfitPts,
		m_s.zBrokerKey, m_s.nLastProfitPts,
		nSum, m_symbolInfo.nTargetPtsOpening
	);
	return TRUE;
}

VOID CManagePos::GetPosInfo(_Out_ TPosInfo* pLong, _Out_ TPosInfo* pShort)
{
	CopyMemory(pLong, &m_l, sizeof(TPosInfo));
	CopyMemory(pShort, &m_s, sizeof(TPosInfo));
}


VOID CManagePos::ResetPosInfo()
{
	m_l.Reset();
	m_s.Reset();
}