#pragma once

#include <Windows.h>
#include <string>
#include "Inc.h"
using namespace std;

struct TPosInfo
{
	char	zBrokerKey[128];
	int		nTicket;
	char	cSide[1];	//B,S
	double	dAvg;
	double	dLots;
	//double	dLastTick;
	int 	nLastProfitPts;
	//double	dPointSize;

	void Reset() {
		zBrokerKey[0] = NULL;
		nTicket = 0;
		dAvg = 0;
		dLots = 0;
		nLastProfitPts = 0;
	}
};


class CManagePos
{
public:
	CManagePos();
	~CManagePos();

	VOID	UpdateSymbolInfo(_In_ TSymbolInfo* pInfo) { CopyMemory(&m_symbolInfo, pInfo, sizeof(TSymbolInfo)); }
	VOID	AddOpenPos(string sBrokerKey, int nTicket, char cBuySellTp, double dAvg, double dLots);
	BOOL	CheckProfit(string sBrokerKey, double dBid, double dAsk);
	VOID	GetPosInfo(_Out_ TPosInfo* pLong, _Out_ TPosInfo* pShort);
	VOID	ResetPosInfo();

	BOOL Is_PositionOpened() { return (m_l.nTicket>0 && m_s.nTicket>0); }
private:


	TSymbolInfo	m_symbolInfo;
	TPosInfo	m_l, m_s;	//long, short
	//int			m_nTargetPLOpen, m_nTargetPLClose;
	//BOOL		m_bPosOpened;
};

