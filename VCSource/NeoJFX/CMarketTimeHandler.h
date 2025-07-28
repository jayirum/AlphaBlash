#pragma once

#pragma warning(disable:4996)
#include <Windows.h>

#define DEF_MARKETCLOSE_TIME "05:30"

class CMarketTimeHandler
{
public:
	CMarketTimeHandler();
	~CMarketTimeHandler();


	void	Set_MarketTime(char* pzTradeStart, char* pzTradeEnd, char* pzMarketClrUseYN, char* pzMarketClrTime);

	void	Check_NowTime(_Out_ BOOL *bTimeToMarketClr);

	BOOL	Is_InTradingTime() { return m_bIsTradingTime; }
	BOOL	IsToday_Weekend();
private:
	char*	Get_MarketCloseTime() { return DEF_MARKETCLOSE_TIME;}

private:
	char	m_zTime_TradeStart[32];			// hh:mm
	char	m_zTime_TradeEnd[32];			// hh:mm
	char	m_zTime_MarketCloseClr[32];		// hh:mm

	BOOL	m_bUseMarketCloseClr;	
	BOOL	m_bIsTradingTime;
	BOOL	m_bAlreadyFired_MarketClr;
};

