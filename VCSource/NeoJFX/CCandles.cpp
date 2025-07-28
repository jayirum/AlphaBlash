#include "CCandles.h"
#include "../CommonAnsi/talibUtils.h"
#include "../CommonAnsi/Util.h"
#include "Inc.h"

extern TCHAR	g_zConfig[_MAX_PATH];

CCandles::CCandles()
{
	InitializeCriticalSection(&m_cs);
	ResetData();
}

CCandles::~CCandles()
{
	ResetData();
	DeleteCriticalSection(&m_cs);
}

void CCandles::ResetData()
{
	for (int k = 0; k < MAX_CANDLE; k++)
	{
		ZeroMemory(&m_arrCandleTime[k], LEN_TIME);
		ZeroMemory(&m_arrMDTime[k], LEN_TIME);
		m_arrClose		[k] = -1;
	}
	m_nCurrCnt = 0;
	m_bRecvFirstNewCandle = FALSE;;
}

// [0-10:00][1-11:00][2-12:00].....[21-23:00]
// memmove ( src, dest, size)
void CCandles::ShiftLeft()
{
	LOCK_CS(m_cs);
	
	int size = sizeof(TimeChar) * (MAX_CANDLE - 1);		memcpy(&m_arrCandleTime[IDX_OLDEST],	&m_arrCandleTime[IDX_OLDEST+1],	size);

	size = sizeof(TimeChar) * (MAX_CANDLE - 1);			memcpy(&m_arrMDTime	[IDX_OLDEST],	&m_arrMDTime[IDX_OLDEST+1], size);

	size = sizeof(double) * (MAX_CANDLE - 1);			memcpy(&m_arrClose[IDX_OLDEST],	&m_arrClose[IDX_OLDEST+1], size);

	UNLOCK_CS(m_cs);
}

// descending( [0]-lastest, [49]-oldest)
BOOL CCandles::Calc_Save_CandleData(char* pzSymbol, char* pzTimeFrame, char* pzCandleTime, char* pzMDTime, char* pzClose, BOOL* pbNewCandle, BOOL bHistory)
{
	*pbNewCandle = FALSE;

	if (m_sSymbol == "")
	{
		LOGGING(INFO, TRUE, TRUE, "SetSymbol must be called before");
		return FALSE;
	}

	if (Is_Fulled())
	{
		// compare the candle time to decide whether incoming data is NEWER or not
		if ( strcmp(m_arrCandleTime[IDX_LATEST].s, pzCandleTime)!=0 )
		{
			ShiftLeft();
			*pbNewCandle = TRUE;
			m_bRecvFirstNewCandle = TRUE;
		}
		memcpy(&m_arrCandleTime[IDX_LATEST].s, pzCandleTime, LEN_TIME);
		memcpy(&m_arrMDTime[IDX_LATEST].s, pzMDTime, LEN_TIME);

		m_arrClose[IDX_LATEST] = atof(pzClose);
	}
	else
	{
		memcpy(&m_arrCandleTime[m_nCurrCnt].s, pzCandleTime, LEN_TIME);
		memcpy(&m_arrMDTime[m_nCurrCnt].s, pzMDTime, LEN_TIME);

		m_arrClose[m_nCurrCnt] = atof(pzClose);
		
		m_nCurrCnt++;
	}

	if (bHistory) m_bRecvFirstNewCandle = FALSE;

	return TRUE;
}




//struct TCandle
//{
//	string sTimeFrame;
//	string sTimeMT4;	//yyyy.mm.dd hh:mm:ss
//	char zOpen[32];
//	char zHigh[32];
//	char zLow[32];
//	char zClose[32];
//};
void CCandles::ShowwAllCandles()
{
	LOCK_CS(m_cs);

	string buf = CUtil::stringFormat("[CANDLE-%s]", m_sSymbol.c_str());
	for (int k = m_nCurrCnt-1; k >= 0; k--)
	{
		char z[128];
		char cHyphen=0x00;
		if (k != 0)
			cHyphen = '-';

		sprintf(z, "[(%d)%.8s,%.8s,%.5f]%c", 
			k,
			m_arrCandleTime[k].s+11,
			m_arrMDTime[k].s+11,
			m_arrClose[k],
			cHyphen
			);

		buf += z;
	}

	UNLOCK_CS(m_cs);
	
	LOGGING(INFO, TRUE, FALSE, buf.c_str());
}

// period means the candle numbers to calculate sma
// input candles array with ascending ==> output has also ascending.
//BOOL CCandles::GetSingleSMA(__ALPHA::EN_TIMEFRAMES timeFrame, int nPeriod, _Out_ double* pSMA)
//{
//	int startIdx	= 0;
//	int endIdx		= nPeriod - 1;
//	int inBuffSize	= nPeriod;
//
//	smartptrDbl inReal(inBuffSize);
//	smartptrDbl outReal(inBuffSize);
//
//
//	int nArrCnt = ComposeArray_For_TALib(nPeriod, inReal.get(), 0);
//
//	CTALibSma	sma(startIdx, endIdx, inBuffSize, nPeriod, inBuffSize);
//
//	if (!sma.Calc(inReal.get(), outReal.get()))
//	{
//		//TODO. LOGGING
//		return FALSE;
//	}
//
//	if (sma.OutNbElement() != 1)
//	{
//		//TODO.
//		return FALSE;
//	}
//	*pSMA = outReal.get()[0];
//
//	return TRUE;
//}

// 
// 
// -
// -[0 - 10:00][1 - 11:00][2 - 12:00].....[21 - 23:00]
// 0-1-2-3-4-5-6-7-8-9, nPeriod=5  ==> m_lstCandles[5]~m_lstCandles[9] : 5 data  ( count(10) - period(5) = 5)


//
///*
//* Copy out double array for TA Libraries
//* candle 오른쪽을 기준을 삼아서, period 만큼 떨어진 왼쪽 부터 copy 하여 반환한다. (오른쪽이 최신이므로)
//* [0] - oldest, [49] - latest
//* ex) [0][1][2][3][4][5][6][7][8][9], period=6 이면  [9]로 부터 왼쪽으로 6만큼 떨어진 [4]~[9] 까지 copy 한다.
//*     (9-6+1 = 4)
//* shift 가 있는 경우는 기준이 되는 오른쪽을 하나씩 왼쪽으로 shift 한다.
//* ex) [0][1][2][3][4][5][6][7][8][9], period=6, shift=2 이면 [7]을 기준으로 해서 7-6+1=2, [2]~[7] 까지 copy 한다.
//*/
//BOOL	CCandles::ComposeArray_For_TALib(const int nPeriod, _Out_ double outCandles[], int nShiftLeft)
//{
//	if (MAX_CANDLE < nPeriod)
//	{
//		m_sMsg = CUtil::stringFormat("Candles(%d) are less than period(%d)", (m_nCurrCnt), nPeriod);
//		return FALSE;
//	}
//	
//	int nRightBaseIdx = (MAX_CANDLE - 1) - nShiftLeft;
//	int nLeftStartIdx = nRightBaseIdx - nPeriod + 1;
//	if (nLeftStartIdx < 0)
//		return FALSE;
//
//	int nCopySize = sizeof(double) * nPeriod;
//
//	LOCK_CS(m_cs);
//
//	memcpy(&outCandles[0], &m_arrClose[nLeftStartIdx], nCopySize);
//
//
//	UNLOCK_CS(m_cs);
//	return TRUE;
//}
//

BOOL CCandles::CandleTime(const int idx, _Out_ char* pCandleTime)
{
	if (idx < 0 || idx >= MAX_CANDLE)
		return FALSE;

	memcpy(pCandleTime, &m_arrCandleTime[idx], LEN_TIME);
	
	return TRUE;
}