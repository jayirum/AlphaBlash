#pragma once

#include "CCandles.h"


//struct TEma
//{
//	string	sCandleTime;
//	string	sTimeMT4;	//yyyy.mm.dd hh:mm:ss
//	double	dEma;
//};
//
//typedef deque<TEma*>		EMA_LIST;	// ascending by time


const int IDX_LATEST_EMA = MAX_EMA - 1;

class CEma
{
public:
	CEma(string sSymbol, _In_ CCandles* pCandles);
	~CEma();

	BOOL	Calc_Save_Ema(char* pzCandleTime, char* pzMDTime);
	BOOL	Calc_Save_History();

	void	ShowAll();
	
	string& getMsg() { return m_sMsg; }
	int		getCount() { return (m_nCurrCnt); }
private:
	//int		ComposeArrayForSMA(_In_ __ALPHA::EN_TIMEFRAMES timeFrame, int nPeriod, _Out_ double arrCandles[]);
	void	ResetData();
private:
	void	ShiftLeft();
	BOOL	Is_Fulled() { return (m_nCurrCnt == (MAX_EMA)); }

public:
	TimeChar	m_arrCandleTime[MAX_EMA];	//yyyy.mm.dd hh:mm:ss
	TimeChar	m_arrMDTime[MAX_EMA];		//yyyy.mm.dd hh:mm:ss
	double		m_arrEma[MAX_EMA];

private:
	string		m_sSymbol;
	int			m_nCurrCnt;


	CCandles*	m_pCandles;

	string		m_sMsg;
};

