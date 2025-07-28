#include "CEma.h"
//#include "../CommonAnsi/talibUtils.h"
#include "../CommonAnsi/Util.h"
#include <assert.h>

extern TCHAR	g_zConfig[_MAX_PATH];

CEma::CEma(string sSymbol, _In_ CCandles* pCandles)
{
	m_sSymbol = sSymbol;
	m_pCandles = pCandles;

	ResetData();
}

CEma::~CEma()
{
	ResetData();
}


// [0-10:00][1-11:00][2-12:00].....[21-23:00]
// memmove ( src, dest, size)
void CEma::ShiftLeft()
{
	int size = sizeof(TimeChar) * (MAX_EMA - 1);	memcpy(&m_arrCandleTime[IDX_OLDEST].s, &m_arrCandleTime[IDX_OLDEST + 1].s, size);

	size = sizeof(TimeChar) * (MAX_EMA - 1);		memcpy(&m_arrMDTime[IDX_OLDEST].s, &m_arrMDTime[IDX_OLDEST + 1].s, size);

	size = sizeof(double) * (MAX_EMA - 1);			memcpy(&m_arrEma[IDX_OLDEST], &m_arrEma[IDX_OLDEST + 1], size);
}



BOOL CEma::Calc_Save_Ema(char* pzCandleTime, char* pzMDTime)
{
	if ( ! m_pCandles->Is_EnoughCandles())
	{
		return FALSE;
	}

	int idx = 0;
	if (Is_Fulled())
	{
		// compare the candle time to decide whether incoming data is NEWER or not
		if (strcmp(m_arrCandleTime[IDX_LATEST_EMA].s, pzCandleTime) != 0)
		{
			ShiftLeft();
		}
		idx = IDX_LATEST_EMA;
	}
	else
	{
		idx = m_nCurrCnt;
		m_nCurrCnt++;
	}

	memcpy(&m_arrCandleTime[idx].s, pzCandleTime, LEN_TIME);
	memcpy(&m_arrMDTime[idx].s, pzMDTime, LEN_TIME);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Calculate EMA - with latest 50 Candles
	double inReal[PERIOD_FOR_EMA + 1];
	double outReal[PERIOD_FOR_EMA + 1];
	int candleIdx = MAX_CANDLE - (PERIOD_FOR_EMA+1);	// copy the latest data
	memcpy(&inReal[0], &m_pCandles->m_arrClose[candleIdx], (PERIOD_FOR_EMA+1)*sizeof(double));

	int startIdx	= 0;
	int endIdx		= PERIOD_FOR_EMA;
	int outBegIdx	= 0, outNbElement = 0;

	TA_RetCode ret = TA_EMA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, &outBegIdx, &outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS || outNbElement ==0)
	{
		assert(0);
		//TODO sprintf(m_zMsg, "[TA_EMA] error code:%d", ret);
		return FALSE;
	}

	// update the latest ema value on the latest item.
	m_arrEma[idx] = outReal[outNbElement-1];
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return TRUE;
}

void CEma::ResetData()
{
	for (int k = 0; k < MAX_EMA; k++)
	{
		ZeroMemory(&m_arrCandleTime[k], LEN_TIME);
		ZeroMemory(&m_arrMDTime[k], LEN_TIME);
		m_arrEma[k] = -1;
	}
	m_nCurrCnt = 0;
}

void CEma::ShowAll()
{
	string buf = CUtil::stringFormat("[EMA-%s]", m_sSymbol.c_str());
	for (int k = m_nCurrCnt - 1; k >= 0; k--)
	{
		char z[128];
		char cHyphen = 0x00;
		if (k != 0)
			cHyphen = '-';

		sprintf(z, "[(%d)%s,%.5f]%c",
			k,
			m_arrCandleTime[k].s,
			m_arrEma[k],
			cHyphen
		);

		buf += z;
	}

	LOGGING(INFO, TRUE, FALSE, buf.c_str());
}