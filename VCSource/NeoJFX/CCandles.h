#pragma once


/*

	NOTE
		m_lstCandles sorts data in ascending way  ==> MT4 Candle order
		- [0] - oldest, [49] - latest
		- [0-10:00][1-11:00][2-12:00].....[21-23:00]

		https://docs.google.com/spreadsheets/d/1dSaQ0y7xBTI0VIr1JkNf_X5iFR412MQkh328HDzKAU4/edit#gid=2064698057
*/


#include "Inc.h"



#define		IDX_LATEST	(MAX_CANDLE-1)


class CCandles
{
public:
	CCandles();
	~CCandles();

	VOID	SetSymbol(string sSymbol) { m_sSymbol = sSymbol; }
	BOOL	Calc_Save_CandleData(char* pzSymbol, char* pzTimeFrame, char* pzCandleTime, char* pzMDTime, char* pzClose, BOOL* pbNewCandle, BOOL bHistory);
	//BOOL	ComposeArray_For_TALib(const int nPeriod, _Out_ double outCandles[], _In_ int shiftLeft=0);
	BOOL	CandleTime(const int idx, _Out_ char* pCandleTime);
	
	//BOOL	GetSingleSMA(__ALPHA::EN_TIMEFRAMES timeFrame, int nPeriod, _Out_ double* pSMA);
	void	ResetData();
	void	ShowwAllCandles();

	
	string& getSymbol()			{ return m_sSymbol; }
	int		getCount()			{ return (m_nCurrCnt); }
	string& getMsg()			{ return m_sMsg; }
	BOOL	Is_EnoughCandles()	{ return (m_nCurrCnt == (MAX_CANDLE)); }
	BOOL	Is_RecvFirstNewCandle() { return m_bRecvFirstNewCandle; }
public:
	TimeChar	m_arrCandleTime	[MAX_CANDLE];	//yyyy.mm.dd hh:mm:ss
	TimeChar	m_arrMDTime		[MAX_CANDLE];		//yyyy.mm.dd hh:mm:ss
	double		m_arrClose		[MAX_CANDLE];


private:
	void	ShiftLeft();	
	BOOL	Is_Fulled() { return (m_nCurrCnt == (MAX_CANDLE)); }
private:
	string		m_sSymbol;
	int			m_nCurrCnt;
	BOOL		m_bRecvFirstNewCandle;

	string		m_sMsg;
	CRITICAL_SECTION	m_cs;
};

