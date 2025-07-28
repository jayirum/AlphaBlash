#include "CMarketTimeHandler.h"
#include <stdio.h>
#include "../CommonAnsi/Util.h"

CMarketTimeHandler::CMarketTimeHandler()
{
	m_bIsTradingTime			= FALSE;
	m_bAlreadyFired_MarketClr	= FALSE;
	m_bUseMarketCloseClr		= FALSE;
}

CMarketTimeHandler::~CMarketTimeHandler()
{

}


void CMarketTimeHandler::Set_MarketTime(char* pzTradeStart, char* pzTradeEnd, char* pzMarketClrUseYN, char* pzMarketClrTime)
{
	strcpy(m_zTime_TradeStart,		pzTradeStart);
	strcpy(m_zTime_TradeEnd,		pzTradeEnd);
	strcpy(m_zTime_MarketCloseClr,	pzMarketClrTime);
	m_bUseMarketCloseClr = (pzMarketClrUseYN[0] == 'Y') ? TRUE : FALSE;
	
}


//
// check weekends
//
BOOL CMarketTimeHandler::IsToday_Weekend()
{
	if (CUtil::Is_Weekend())
		return TRUE;

	return FALSE;
}

void CMarketTimeHandler::Check_NowTime(_Out_ BOOL* bTimeToMarketClr)
{
	SYSTEMTIME st;
	char zNow[64];
	GetLocalTime(&st);
	sprintf(zNow, "%02d:%02d", st.wHour, st.wMinute);


	//
	// check trading time
	//
	if (strcmp(m_zTime_TradeStart, zNow) >= 0 &&
		strcmp(zNow, m_zTime_TradeEnd) < 0
		)
	{
		m_bIsTradingTime = TRUE;
	}
	else
	{
		m_bIsTradingTime = FALSE;
	}

	//
	// MarketCloseClear
	//
	if (m_bUseMarketCloseClr)
	{
		// check market close time
		// 04:00~06:00
		if (!m_bAlreadyFired_MarketClr)
		{
			if (strcmp(zNow, m_zTime_MarketCloseClr) >= 0 && strcmp(zNow, m_zTime_MarketCloseClr) < 0)
			{
				*bTimeToMarketClr			= TRUE;
				m_bAlreadyFired_MarketClr	= TRUE;
			}
			else
			{
				m_bAlreadyFired_MarketClr = FALSE;
			}
		}
	}
}

