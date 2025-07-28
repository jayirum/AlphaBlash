#pragma once

/*
	NOTE

*/

#include "Inc.h"
#include "CCandles.h"
#include "CFindWShape.h"
#include "CFindMShape.h"
#include <string>
#include "Inc.h"

using namespace std;

const int IDX_LATEST_RSI = MAX_RSI - 1;
const int IDX_LATEST_MAB = MAX_MAB - 1;




class CComputeSignal
{
public:
	CComputeSignal(CCandles* pCandles);
	~CComputeSignal();

	BOOL Calc_History_SigFactors();
	
	//BOOL Calc_Save_SigFactors(string sCandleTime, _In_ string sTimeMT4);
	BOOL Calc_Save_Rsi(char* pzCandleTime, BOOL bHistoryData=FALSE, int nHistoryMaxIdx=0);
	BOOL Calc_Save_MabValues(char* pzCandleTime, BOOL bHistoryData = FALSE, int nHistoryMaxIdx=0);

	EN_SIGNALS	ComputeSignal();
	
	VOID	ShowAll();
	VOID	showRsi();
	VOID	showMabValues();
private:

	VOID	ShiftLeft_Rsi();
	VOID	ShiftLeft_MabValues();

	BOOL	Is_RsiFulled() { return (strlen(m_rsiCandleTime[IDX_LATEST_RSI].s)>0); }

	BOOL ComputeSignal_Buy();
	BOOL ComputeSignal_Sell();
	BOOL Find_W_Shape();
	BOOL Find_M_Shape();

	

private:
	void	ResetData();
	
private:
	
	TimeChar		m_rsiCandleTime[MAX_RSI];
	double			m_rsiValue[MAX_RSI];
	TMabValues		m_arrMabValues[MAX_MAB];

	CCandles*		m_pCandles;
	double			m_dMabSignalThreshold;
	double			m_dShapeValidThreshold;

	string			m_sSymbol;
};

