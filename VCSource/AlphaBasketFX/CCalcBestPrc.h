/*
	Each symbol has one instance of this class
*/

#pragma once

#include "Inc.h"
#include <map>
#include <vector>
#include <set>

using namespace std;

#define ISYMBOL		int
#define BROKER_KEY	string
#define BIDPRC		string
#define ASKPRC		string
#define MD_TIME		string

//typedef map< MD_TIME, TMarketData*, greater<MD_TIME>>			INMAP_MDTIME;
//typedef map< MD_TIME, TMarketData*, greater<MD_TIME>>::iterator	IT_INMAP_MDTIME;
//
//typedef map<BIDPRC, INMAP_MDTIME, greater<BIDPRC>>				MAP_BID;	// bid(descending), brokerKey
//typedef map<BIDPRC, INMAP_MDTIME, greater<BIDPRC>>::iterator	IT_MAP_BID;	// bid(descending), brokerKey
//
//typedef map<ASKPRC, INMAP_MDTIME, less<ASKPRC>>					MAP_ASK;	// ask(ascending), brokerKey
//typedef map<ASKPRC, INMAP_MDTIME, less<ASKPRC>>::iterator		IT_MAP_ASK;	// ask(ascending), brokerKey

struct TMarketData
{
	char brokerKey[32];
	char bid[__ALPHA::LEN_PRC + 1];
	char ask[__ALPHA::LEN_PRC + 1];
	double spread;
	char zLastMDTime[32];
};


class CCalcBestPrc
{
public:
	CCalcBestPrc();
	~CCalcBestPrc();
	
	void Set_ISymbol(int iSymbol);
	void Set_BroketCntTodo(int nCnt) { m_nBrokerCntTodo = nCnt; }
	VOID AddBroker(_In_ char* pzBrokerKey);
	
	//VOID AddBroker(_In_ vector<TBroker*>& vecBroker);
	//void AddBrokerDone() { m_bAddBrokerDone = TRUE; }

	__ALPHA::EN_RET_VAL Update_LatestMarketData(char* pzBrokerKey, char* pzNewBid, char* pzNewAsk, double dSpread, char* pzMDTime);
	
	__ALPHA::EN_RET_VAL CalcBestPrc(
		_Out_ char* pzBestBid
		, _Out_ char* pzBidBorkerKey
		, _Out_ char* pzBestBidOpposite
		, _Out_ double* pdBidderSpread
		, _Out_ char* pzBidMDTime

		, _Out_ char* pzBestAsk
		, _Out_ char* pzAskBrokerKey
		, _Out_ char* pzBestAskOpposite
		, _Out_ double* pdAskerSpread
		, _Out_ char* pzAskMDTime
	);
	
	BOOL	Check_AllBrokers_SendFirstMD();

	//	VOID PrintMap();

	char* GetMsg() { return m_zMsg; }
	UINT GetBrokerCnt() { return m_mapMD.size(); }
	BOOL GetLastBidAsk(char* pzBrokerKey, _Out_ char* pzBid, _Out_ char* pzAsk, _Out_ char* pzMDTime);
	BOOL Is_LaterThanCutOffTime(_In_ char* pzTime);
private:
	void PrintMD();
	BOOL CCalcBestPrc::GetCutOffTime(_Out_ char* pzCutOffTime);
public:
	int		m_iSymbol;
	int		m_nBrokerCntTodo;
	char	m_zMsg[1024];

	map<BROKER_KEY, TMarketData*>						m_mapMD;	// 
	//map<BIDPRC, vector<TMarketData*>, greater<BIDPRC> >	m_mapBid;	// bid(descending), brokerKey
	//map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>		m_mapAsk;	// ask(ascending), brokerKey

	BOOL	m_bRecvFirstMDAllBrokers;
	BOOL	m_bAddBrokerDone;
	double	m_dCutOffTimeRate;
};

/*
vector<map<BROKER_KEY, TMarketData*>>		m_mapMD;

 [FXMARKETS-TMarketData][FXOPEN-TMarketData][OANDA-TMarketData][PEPPERSTONE-TMarketData]
 

map<BIDPRC, vector<TMarketData*>, greater<BIDPRC> >	m_mapBid;
 [999-(TMarketData-TMarketData)][900-(TMarketData)][800-(TMarketData-TMarketData-TMarketData)]
	
map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>		m_mapAsk;
 [500-(TMarketData-TMarketData)][600-(TMarketData)][700-(TMarketData-TMarketData-TMarketData)]

 */

