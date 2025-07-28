/*
	The instance of this class is allocated to each symbol.
	That is, if this application handles 10 symbols, the number of the instance of this class is 10 as well.
*/

#pragma once

#include "Inc.h"
#include <Windows.h>
#include "CCandles.h"
#include "CEma.h"
#include "CComputeSignal.h"
#include "CMarketTimeHandler.h"
#include <string>
using namespace std;



struct TSymbolSpec
{
	int		nSymbolIdx;
	string	sSymbol;
	double	dPipSize;
	int		nDecimalCnt;
	int		nAllowedSpreadPoint;
};

struct TTickData
{
	char	zBid[32];
	char	zAsk[32];
	int		nSpreadPt;
};


class CStrategyProc
{
public:
	CStrategyProc(int nSymbolIdx, string sSymbol, double dPipSize, int nDecimalCnt, int nAllowedSpreadPoint, 
					unsigned int unOrderSendThreadId, unsigned int unDBThreadId, CMarketTimeHandler* marketTimeHandler);
	~CStrategyProc();

	
	void	ReceiveProcess(const char* pRecvData, int nDataLen);

private:
	static	unsigned WINAPI Thread_Proc(LPVOID lp);

	//void	MD_Process(char* pRecvData, int nDataLen);
	void	CandleHistory_Process(char* pRecvData, int nDataLen);
	void	Candle_Process(char* pRecvData, int nDataLen);
	void	RecvOrder_Open(char* pRecvData, int nDataLen);
	void	RecvOrder_Close(char* pRecvData, int nDataLen);

	void	StrategyProcess();

	VOID	SendOrder(int iSymbol, char* pzSymbol, char cBuySell);

private:

	HANDLE			m_hThread;
	unsigned int	m_unThread;
	BOOL			m_bThreadRun;
	unsigned int	m_unOrderSendThreadId;
	unsigned int	m_unDBThreadId;

	CCandles		*m_candle;
	CEma			*m_ema;
	CComputeSignal* m_computeSignal;
	CMarketTimeHandler* m_marketTimeHandler;
	
	TSymbolSpec		m_spec;
	TTickData		m_tick;

	EN_ORD_STATUS	m_ordStatus;
};

