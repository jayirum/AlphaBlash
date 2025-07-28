#pragma once

/*
	Save MarketData on Memory and write the latest date on the csv file regularly.
*/

#include <Windows.h>
#include "CCsvFile.h"
#include <map>

using namespace std;

struct TMarketData
{
	string	sBid;
	string	sAsk;
	string  sTimeMT4;
};

class CSaveMD
{
public:
	CSaveMD(string sSymbol);
	~CSaveMD();

	BOOL	Initialize();
	void	ReceiveProcess(const char* pRecvData, const int nRecvLen);
private:
	static	unsigned WINAPI Thread_Main(LPVOID lp);
	BOOL	Save_OnMemory(char* pRecvData, int nRecvLen);
	VOID	Save_OnCSV();
	BOOL	FindMap(string sBroker);
private:
	HANDLE		m_hThread;
	UINT		m_unThreadId;
	int			m_nTimeoutSaveSec;
	string		m_sSymbol;
	CCsvFile	*m_csv;
	BOOL		m_bDie;

	map<string, TMarketData*>				m_mapMD;	// broker, data
	map<string, TMarketData*>::iterator		m_itMD;
};

