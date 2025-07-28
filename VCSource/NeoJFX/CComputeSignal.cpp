#include "CComputeSignal.h"
#include "../CommonAnsi/talibUtils.h"
#include "../CommonAnsi/Util.h"
#include <Windows.h>

extern TCHAR	g_zConfig[_MAX_PATH];

CComputeSignal::CComputeSignal(CCandles* pCandles)
{
	m_pCandles = pCandles;

	m_sSymbol = m_pCandles->getSymbol();
	
	char temp[128]; 
	CUtil::GetConfig(g_zConfig, "STRATEGY", "MAB_SIGNAL_THRESHOLD", temp);
	m_dMabSignalThreshold = atof(temp);

	CUtil::GetConfig(g_zConfig, "STRATEGY", "SHAPE_VALID_THRESHOLD", temp);
	m_dShapeValidThreshold = atof(temp);

	ResetData();
}


CComputeSignal::~CComputeSignal()
{
	ResetData();
}

void CComputeSignal::ResetData()
{
	for (int k = 0; k < MAX_RSI; k++)
	{
		ZeroMemory(&m_rsiCandleTime[k], sizeof(TimeChar));
		m_rsiValue[k] = 0;
	}
	for (int j = 0; j < MAX_MAB; j++)
	{
		ZeroMemory(m_arrMabValues[j].CandleTime, sizeof(m_arrMabValues[j].CandleTime));
		m_arrMabValues[j].mab = 0;
		m_arrMabValues[j].mid = 0;
		m_arrMabValues[j].mbb = 0;
	}
}
//
//BOOL CComputeSignal::Calc_Save_SigFactors(string sCandleTime, string sTimeMT4)
//{
//	// if the time is new
//	if(m_lstSigFactors.size()==0 ||
//		m_lstSigFactors[m_lstSigFactors.size() - 1]->sCandleTime != sCandleTime
//	)
//	{
//		TSigFactors* p = new TSigFactors;
//		p->sCandleTime	= sCandleTime;
//		m_lstSigFactors.push_back(p);
//	}
//	int lastIdx = m_lstSigFactors.size() - 1;
//	m_lstSigFactors[lastIdx]->sTimeMT4 = sTimeMT4;
//
//	// Rsi
//	if (!Calc_Save_Rsi(lastIdx))
//	{
//		return FALSE;
//	}
//
//	//TODO
//	return TRUE;
//
//	std::auto_ptr<double> arrRsi(new double(m_nMaxRsiCnt));
//
//	int nArrCnt = ComposeRsiArray_For_TALib(arrRsi.get());
//
//	if( !Calc_Save_Mid(lastIdx, arrRsi.get()))	return FALSE;
//	if( !Calc_Save_Mab(lastIdx, arrRsi.get()))	return FALSE;
//	if( !Calc_Save_Mbb(lastIdx, arrRsi.get()))	return FALSE;
//
//
//	if (m_lstSigFactors.size() >= (UINT)m_nMaxElementCnt)
//	{
//		for (int size = m_lstSigFactors.size(); size < m_nMaxElementCnt; )
//		{
//			delete (*m_lstSigFactors.begin());
//		}	m_lstSigFactors.pop_front();
//	}
//
//	return TRUE;
//}

VOID CComputeSignal::ShowAll()
{
	string buf;
	//for (int k = m_lstSigFactors.size() - 1; k >= 0; k--)
	//{
	//	char z[128];
	//	char cHyphen = 0x00;
	//	if (k != 0)
	//		cHyphen = '-';

	//	sprintf(z, "[%s,%.5f]%c",
	//		m_lstSigFactors[k]->sCandleTime.c_str(), 
	//		m_lstSigFactors[k]->dRsi,
	//		cHyphen
	//	);

	//	buf += z;
	//}

	LOGGING(INFO, TRUE, FALSE, buf.c_str());
}


BOOL CComputeSignal::Calc_History_SigFactors()
{
	if (!m_pCandles->Is_EnoughCandles())
	{
		return FALSE;
	}

	////////////////////////////////////////////////////
	// CANDLE HISTORY 로 부터 RSI 의 HISTORY 를 구성한다.
	//
	for (int k = MAX_CANDLE - 1; k > -1; k--)
	{
		Calc_Save_Rsi(NULL, TRUE, k);
	}
	
	for (int k = MAX_CANDLE - 1; k > -1; k--)
	{
		Calc_Save_MabValues(NULL, TRUE, k);
	}
	showMabValues();
	
	return TRUE;
}

/*
	r=rsi(close,13)
	ma=sma(r,34)
	offs = (1.6185 * stddev(r,34))
	mid = ((ma+offs)+(ma-offs))/2
	mab = sma(r,13);
	mbb = sma(r,7);
*/
BOOL CComputeSignal::Calc_Save_MabValues(char* pzCandleTime, BOOL bHistoryData, int nHistoryMaxIdx)
{
	if (!Is_RsiFulled())
	{
		assert(0);
		return FALSE;
	}

	if (!bHistoryData)
	{
		if (strlen(m_arrMabValues[IDX_LATEST_RSI].CandleTime) == 0)
		{
			assert(0);
		}

		// compare the candle time to decide whether incoming data is NEWER or not
		if (strcmp(m_arrMabValues[IDX_LATEST_RSI].CandleTime, pzCandleTime) != 0)
		{
			ShiftLeft_MabValues();
		}
	}

	int nCopyRsiCnt = PERIOD_FOR_MA + 1;
	double inReal[PERIOD_FOR_MA + 1];
	double outReal[PERIOD_FOR_MA + 1];
	ZeroMemory(inReal, sizeof(double) * nCopyRsiCnt);
	ZeroMemory(outReal, sizeof(double) * nCopyRsiCnt);

	int startIdx = 0;
	int endIdx = PERIOD_FOR_MA;
	int outBegIdx = 0;
	int outNbElement = 0;

	////////////////////////////////////////////////////////////////////////////
	// Copy RSI values for input 
	int startRsi = 0;
	if (bHistoryData)
		startRsi = nHistoryMaxIdx - (PERIOD_FOR_MA + 1);
	else
		startRsi = MAX_RSI - (PERIOD_FOR_MA + 1);	// copy the latest data

	memcpy(&inReal[0], &m_rsiValue[startRsi], nCopyRsiCnt * sizeof(double));


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///
	///		1) MID 
	TA_RetCode ret = TA_SMA(startIdx, endIdx, &inReal[0], PERIOD_FOR_MA, &outBegIdx, &outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS || outNbElement < 0)
	{
		//delete[] inReal;
		//delete[] outReal;
		assert(0);
		return FALSE;
	}
	double dMa = outReal[outNbElement - 1];


	int optInNbDev = 1;	//default value

	startIdx = 0;
	endIdx = PERIOD_FOR_MA;
	outBegIdx = 0;
	outNbElement = 0;

	ZeroMemory(outReal, sizeof(double) * nCopyRsiCnt);

	ret = TA_STDDEV(startIdx, endIdx, &inReal[0], PERIOD_FOR_MA, optInNbDev, &outBegIdx, &outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS || outNbElement < 0)
	{
		//delete[] inReal;
		//delete[] outReal;
		assert(0);
		return FALSE;
	}
	double dStdev = outReal[outNbElement - 1];

	double dOffs = 1.6185 * dStdev;

	double dMid = ((dMa + dOffs) + (dMa - dOffs)) / 2;
	//
	//	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	2) MAB
	startIdx = 0;
	endIdx = PERIOD_FOR_MA;
	outBegIdx = 0;
	outNbElement = 0;

	ZeroMemory(outReal, sizeof(double) * nCopyRsiCnt);

	ret = TA_SMA(startIdx, endIdx, &inReal[0], PERIOD_FOR_MAB, &outBegIdx, &outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS || outNbElement < 0)
	{
		//delete[] inReal;
		//delete[] outReal;
		assert(0);
		return FALSE;
	}
	double dMab = outReal[outNbElement - 1];
	//
	//	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	3) MBB
	startIdx = 0;
	endIdx = PERIOD_FOR_MA;
	outBegIdx = 0;
	outNbElement = 0;

	ZeroMemory(outReal, sizeof(double) * nCopyRsiCnt);

	ret = TA_SMA(startIdx, endIdx, &inReal[0], PERIOD_FOR_MBB, &outBegIdx, &outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS || outNbElement < 0)
	{
		//delete[] inReal;
		//delete[] outReal;
		assert(0);
		return FALSE;
	}
	double dMbb = outReal[outNbElement - 1];

	char zCandleTime[LEN_TIME] = { 0 };
	int idx = 0;
	if (bHistoryData)
	{
		m_pCandles->CandleTime(nHistoryMaxIdx, zCandleTime);
		idx = nHistoryMaxIdx;
	}
	else
	{
		idx = IDX_LATEST_MAB;
		strcpy(zCandleTime, pzCandleTime);
	}
	strcpy(m_arrMabValues[idx].CandleTime, zCandleTime);
	m_arrMabValues[idx].mid = dMid;
	m_arrMabValues[idx].mab = dMab;
	m_arrMabValues[idx].mbb = dMbb;

	return TRUE;
}



VOID CComputeSignal::showRsi()
{
	string buf = CUtil::stringFormat("[RSI   -%s]", m_sSymbol.c_str());
	for (int k = MAX_RSI - 1; k >= 0; k--)
	{
		char z[128];
		char cHyphen = 0x00;
		if (k != 0)
			cHyphen = '-';

		sprintf(z, "[(%d)%s,%.5f]%c",
			k,
			m_rsiCandleTime[k].s+11,
			m_rsiValue[k],
			cHyphen
		);

		buf += z;
	}

	LOGGING(INFO, TRUE, FALSE, buf.c_str());
}


VOID CComputeSignal::showMabValues()
{
	string buf = CUtil::stringFormat("[MABs  -%s]", m_sSymbol.c_str());
	for (int k = MAX_RSI - 1; k > -1; k--)
	{
		char z[256];
		char cHyphen = 0x00;
		if (k != 0)
			cHyphen = '-';

		sprintf(z, "[(%d)%s(%.5f)(%.5f)(%.5f)]%c",
			k,
			m_arrMabValues[k].CandleTime + 11,
			m_arrMabValues[k].mab,
			m_arrMabValues[k].mbb,
			m_arrMabValues[k].mid,
			cHyphen
		);

		buf += z;

		if (k == 50)
			break;
	}

	LOGGING(INFO, TRUE, FALSE, buf.c_str());
}


void CComputeSignal::ShiftLeft_Rsi()
{
	int size = sizeof(TimeChar) * (MAX_RSI - 1);	
	memcpy(&m_rsiCandleTime[IDX_OLDEST], &m_rsiCandleTime[IDX_OLDEST + 1], size);

	size = sizeof(double) * (MAX_RSI - 1);
	memcpy(&m_rsiValue[IDX_OLDEST], &m_rsiValue[IDX_OLDEST + 1], size);
}

void CComputeSignal::ShiftLeft_MabValues()
{
	int size = sizeof(TMabValues) * (MAX_MAB - 1);
	memcpy(&m_arrMabValues[IDX_OLDEST], &m_arrMabValues[IDX_OLDEST + 1], size);
}


// Being calculated based on the latest 13 candles
// Must be stored at least 34 r values
BOOL CComputeSignal::Calc_Save_Rsi(char* pzCandleTime, BOOL bHistoryData, int nHistoryMaxIdx)
{
	if (!m_pCandles->Is_EnoughCandles())
	{
		assert(0);
		return FALSE;
	}

	if (!bHistoryData)
	{
		if (strlen(m_rsiCandleTime[IDX_LATEST_RSI].s) == 0)
		{
			assert(0);
		}

		// compare the candle time to decide whether incoming data is NEWER or not
		if (strcmp(m_rsiCandleTime[IDX_LATEST_RSI].s, pzCandleTime) != 0)
		{
			ShiftLeft_Rsi();
		}
	}

	int nCopyCandleCnt = PERIOD_FOR_RSI + 1;
	double inReal[PERIOD_FOR_RSI + 1];
	double outReal[PERIOD_FOR_RSI + 1];

	int startCandle = 0;
	if (bHistoryData)
		startCandle = nHistoryMaxIdx - (PERIOD_FOR_RSI + 1);
	else
		startCandle = MAX_CANDLE - (PERIOD_FOR_RSI + 1);	// copy the latest data

	// Copy candle data for RSI input
	ZeroMemory(inReal, sizeof(double) * nCopyCandleCnt);
	ZeroMemory(outReal, sizeof(double) * nCopyCandleCnt);
	memcpy(&inReal[0], &m_pCandles->m_arrClose[startCandle], nCopyCandleCnt * sizeof(double));

	int startIdx	= 0;
	int endIdx		= PERIOD_FOR_RSI;
	int outBegIdx	= 0;
	int outNbElement= 0;


	TA_RetCode ret = TA_RSI(startIdx, endIdx, &inReal[0], PERIOD_FOR_RSI, &outBegIdx, &outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS || outNbElement < 0)
	{
		//delete[] inReal;
		//delete[] outReal;
		assert(0);
		return FALSE;
	}

	char zCandleTime[LEN_TIME] = { 0 };
	int idx;
	if (bHistoryData)
	{		
		m_pCandles->CandleTime(nHistoryMaxIdx, zCandleTime);
		idx = nHistoryMaxIdx;
	}
	else
	{
		strcpy(zCandleTime, pzCandleTime);
		idx = IDX_LATEST_RSI;
	}
	m_rsiValue[idx] = outReal[outNbElement - 1];	// 최신 데이터를 rsi 의 뒤부터 채운다.
	memcpy(m_rsiCandleTime[idx].s, zCandleTime, LEN_TIME);

	return TRUE;
}





//struct TSigFactors
//{
//	__ALPHA::EN_TIMEFRAMES	timeFrame;
//	char	zTimeMT4[32];	//yyyy.mm.dd hh:mm:ss
//	double	dRsi;
//	double	dMid;
//	double	dMab;
//	double	dMbb;
//};
//typedef deque<TSigFactors*>		ELEMENT_LIST;	// ascending by time
EN_SIGNALS CComputeSignal::ComputeSignal()
{
	EN_SIGNALS ret = SIGNAL_NONE;
		
	// Condition-1 for BUY
	//	(new mab > 50)&
	//	(new mab > new mid)&
	//	(new mab > new mdd)
	if (( m_arrMabValues[IDX_LATEST_MAB].mab > m_dMabSignalThreshold)				&&
		( m_arrMabValues[IDX_LATEST_MAB].mab > m_arrMabValues[IDX_LATEST_MAB].mbb)	&&
		( m_arrMabValues[IDX_LATEST_MAB].mab > m_arrMabValues[IDX_LATEST_MAB].mid)
		)
	{
		LOGGING(INFO,TRUE,FALSE, "[SIGNAL-B-1]mab(%f)>(%f) && mab>mbb(%f) && mab>mid(%f)",
			m_arrMabValues[IDX_LATEST_MAB].mab,
			m_dMabSignalThreshold,
			m_arrMabValues[IDX_LATEST_MAB].mbb,
			m_arrMabValues[IDX_LATEST_MAB].mid
		);
		// Condition-2 - looping
		if (ComputeSignal_Buy())
			ret = SIGNAL_BUY;
	}

	// Condition-1 for SELL
	//	(new mab < 50)&
	//	(new mab < new mid)&
	//	(new mab < new mdd)
	if ((m_arrMabValues[IDX_LATEST_MAB].mab < m_dMabSignalThreshold) &&
		(m_arrMabValues[IDX_LATEST_MAB].mab < m_arrMabValues[IDX_LATEST_MAB].mbb) &&
		(m_arrMabValues[IDX_LATEST_MAB].mab < m_arrMabValues[IDX_LATEST_MAB].mid)
		)
	{
		LOGGING(INFO, TRUE, FALSE, "[SIGNAL-S-1]mab(%f)<(%f) && mab<mbb(%f) && mab<mid(%f)",
			m_arrMabValues[IDX_LATEST_MAB].mab,
			m_dMabSignalThreshold,
			m_arrMabValues[IDX_LATEST_MAB].mbb,
			m_arrMabValues[IDX_LATEST_MAB].mid
		);
		// Condition-2 - looping
		if (ComputeSignal_Sell())
			ret = SIGNAL_SELL;
	}

	return ret;


}

BOOL CComputeSignal::ComputeSignal_Buy()
{
	CFind_W_Shape shape;
	if (!shape.FindShape(&m_arrMabValues[0], MAX_MAB))
	{
		return FALSE;
	}

	LOGGING(INFO, TRUE, FALSE, "[SIGNAL-B-2] ElbowR(idx:%d)(%f) ElbowL(idx:%d)(%f) Nose(idx:%d)(%f)",
		shape.Idx_ElbowR(), shape.ElbowR(),
		shape.Idx_ElbowL(), shape.ElbowL(),
		shape.Idx_Nose(), shape.Nose()
	);

	for (int i = shape.Idx_ElbowR(); i >= shape.Idx_ElbowL(); i--)
	{
		LOGGING(INFO, TRUE, FALSE,
			"[SIGNAL-B-3][%d](mab:%f)(mbb:%f)(mid:%f)",
			i, m_arrMabValues[i].mab, m_arrMabValues[i].mbb, m_arrMabValues[i].mid
		);
	}


	//	1) 2 elbows, nose < 50
	//	2) 2 elbows, nose < mid
	//	3) mbb < mid

	// 1) 
	if (shape.ElbowR() < m_dShapeValidThreshold &&
		shape.ElbowL() < m_dShapeValidThreshold &&
		shape.Nose() < m_dShapeValidThreshold
		)
	{
		// 2) 
		if (shape.ElbowR() < m_arrMabValues[shape.Idx_ElbowR()].mid &&
			shape.ElbowL() < m_arrMabValues[shape.Idx_ElbowL()].mid &&
			shape.Nose() < m_arrMabValues[shape.Idx_Nose()].mid
			)
		{
			// 3)
			int nMatchedCnt = 0;
			for (int i = shape.Idx_ElbowR(); i >= shape.Idx_ElbowL(); i--)
			{
				if (m_arrMabValues[i].mbb < m_arrMabValues[i].mid)
					nMatchedCnt++;
			}

			if (nMatchedCnt == (shape.Idx_ElbowR() - shape.Idx_ElbowL()+1))
			{
				LOGGING(INFO, TRUE, TRUE, "BUY SIGNAL!!!");
					return TRUE;
			}
		}
	}
	return FALSE;
}



BOOL CComputeSignal::ComputeSignal_Sell()
{
	CFind_M_Shape shape;
	if (!shape.FindShape(&m_arrMabValues[0], MAX_MAB))
	{
		return FALSE;
	}

	LOGGING(INFO, TRUE, FALSE, "[SIGNAL-S-2] ElbowR(idx:%d)(%f) ElbowL(idx:%d)(%f) Nose(idx:%d)(%f)",
		shape.Idx_ElbowR(), shape.ElbowR(),
		shape.Idx_ElbowL(), shape.ElbowL(),
		shape.Nose(), shape.Nose()
	);

	for (int i = shape.Idx_ElbowR(); i >= shape.Idx_ElbowL(); i--)
	{
		LOGGING(INFO, TRUE, FALSE,
			"[SIGNAL-S-3][%d](mab:%f)(mbb:%f)(mid:%f)",
			i, m_arrMabValues[i].mab, m_arrMabValues[i].mbb, m_arrMabValues[i].mid
		);
	}


	//	1) 2 elbows, nose > 50
	//	2) 2 elbows, nose > mid
	//	3) mbb > mid

	// 1) 
	if (shape.ElbowR() > m_dShapeValidThreshold &&
		shape.ElbowL() > m_dShapeValidThreshold &&
		shape.Nose() > m_dShapeValidThreshold
		)
	{
		// 2) 
		if (shape.ElbowR() > m_arrMabValues[shape.Idx_ElbowR()].mid &&
			shape.ElbowL() > m_arrMabValues[shape.Idx_ElbowL()].mid &&
			shape.Nose() > m_arrMabValues[shape.Idx_Nose()].mid
			)
		{
			// 3)
			int nMatchedCnt = 0;
			for (int i = shape.Idx_ElbowR(); i >= shape.Idx_ElbowL(); i--)
			{
				if (m_arrMabValues[i].mbb > m_arrMabValues[i].mid)
					nMatchedCnt++;
			}
			if (nMatchedCnt == (shape.Idx_ElbowR() - shape.Idx_ElbowL() + 1))
			{
				LOGGING(INFO, TRUE, TRUE, "SELL SIGNAL!!!");
				return TRUE;
			}
		}
	}
	return FALSE;
}

