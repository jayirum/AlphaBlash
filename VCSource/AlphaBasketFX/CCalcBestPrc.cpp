#include "CCalcBestPrc.h"
#include "../CommonAnsi/LogMsg.h"
#include "../CommonAnsi/Util.h"

extern CLogMsg g_debug;
extern TCHAR	g_zConfig[_MAX_PATH];

CCalcBestPrc::CCalcBestPrc()
{	
	m_bRecvFirstMDAllBrokers = FALSE;
	m_bAddBrokerDone = FALSE;

	char zTemp[1024] = { 0, };
	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("CUTOFF_TIME_RATE"), zTemp);
	m_dCutOffTimeRate = atof(zTemp);
	if (m_dCutOffTimeRate <= 0)
		m_dCutOffTimeRate = 0.5;

}


CCalcBestPrc::~CCalcBestPrc()
{

}

void CCalcBestPrc::Set_ISymbol(int iSymbol)
{
	m_iSymbol = iSymbol;
}

/*
	map<BROKER_KEY, TMarketData*>						m_mapMD;	//

*/
//
//VOID CCalcBestPrc::AddBroker(_In_ vector<TBroker*>& vecBroker)
//{
//	for (int i = 0; i < (int)vecBroker.size(); i++)
//	{
//		TBroker* pBroker = vecBroker.at(i);
//
//		map<BROKER_KEY, TMarketData*>::iterator it = m_mapMD.find(pBroker->brokerKey);
//		if (it == m_mapMD.end())
//		{
//			TMarketData* pMD = new TMarketData;
//			ZeroMemory(pMD, sizeof(TMarketData));
//
//			strcpy(pMD->brokerKey, pBroker->brokerKey);
//
//			m_mapMD[pMD->brokerKey] = pMD;
//
//			//g_debug.log(INFO, "[CalcBestPrc-AddBroker](%s)", pzBrokerKey);
//		}
//		
//		//g_debug.log(INFO, "[AddBroker:%d](%s)", m_iSymbol, pMD->brokerKey);
//	}
//	m_bAddBrokerDone = TRUE;
//}


// Begin called in BOOL CDataHandler::Add_BrokerWhenLogin(char* pzBrokerKey, char* pzBrokerName)
VOID CCalcBestPrc::AddBroker(_In_ char* pzBrokerKey)
{
	map<BROKER_KEY, TMarketData*>::iterator it = m_mapMD.find(string(pzBrokerKey));
	if (it == m_mapMD.end())
	{
		TMarketData* pMD = new TMarketData;
		ZeroMemory(pMD, sizeof(TMarketData));

		strcpy(pMD->brokerKey, pzBrokerKey);

		m_mapMD[pMD->brokerKey] = pMD;

		//g_debug.log(INFO, "[CalcBestPrc-AddBroker](%s)", pzBrokerKey);
	}

	if(m_mapMD.size()==m_nBrokerCntTodo )
		m_bAddBrokerDone = TRUE;
}



BOOL CCalcBestPrc::GetLastBidAsk(char* pzBrokerKey, _Out_ char* pzBid, _Out_ char* pzAsk, _Out_ char* pzMDTime)
{
	map<BROKER_KEY, TMarketData*>::iterator itBroker = m_mapMD.find(string(pzBrokerKey));
	if (itBroker == m_mapMD.end())
		return FALSE;

	TMarketData* pBidAsk = (*itBroker).second;
	strcpy(pzBid, pBidAsk->bid);
	strcpy(pzAsk, pBidAsk->ask);
	strcpy(pzMDTime, pBidAsk->zLastMDTime);
	
	return TRUE;
}

/*
	map<BROKER_KEY, TMarketData*>						m_mapMD;	//
	map<BIDPRC, vector<TMarketData*>, greater<BIDPRC> >	m_mapBid;	// bid(descending), brokerKey
	map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>		m_mapAsk;	// ask(ascending), brokerKey
*/
__ALPHA::EN_RET_VAL CCalcBestPrc::Update_LatestMarketData(char* pzBrokerKey, char* pzNewBid, char* pzNewAsk, double dSpread, char* pzMDTime)
{
	map<BROKER_KEY, TMarketData*>::iterator itBroker = m_mapMD.find(string(pzBrokerKey));
	if (itBroker == m_mapMD.end())
	{
		sprintf(m_zMsg, "UpdateMD error.(Broker:%s)hasn't added.", pzBrokerKey);
		return __ALPHA::RET_ERR;
	}

	TMarketData* pBidAsk = (*itBroker).second;
	strcpy(pBidAsk->brokerKey, pzBrokerKey);
	strcpy(pBidAsk->bid, pzNewBid);
	strcpy(pBidAsk->ask, pzNewAsk);
	pBidAsk->spread = dSpread;
	strcpy(pBidAsk->zLastMDTime, pzMDTime);

	m_mapMD[string(pzBrokerKey)] = pBidAsk;

	//TODO
	//PrintMD();

	return __ALPHA::RET_OK;
}

void CCalcBestPrc::PrintMD()
{
	int cnt = 0;
	char buf[1024];
	for (map<BROKER_KEY, TMarketData*>::iterator itBroker = m_mapMD.begin(); itBroker != m_mapMD.end(); itBroker++)
	{
		TMarketData* p = (*itBroker).second;
		sprintf(buf, "[%d](%s)(bid:%.5f)(ask:%.5f)(time:.%s)", cnt++, (*itBroker).first.c_str(), p->bid, p->ask, p->zLastMDTime);
		g_debug.Log(INFO, buf, TRUE);
	}
}

//double CCalcBestPrc::GetSpread(int iSymbol, char* pzBrokerKey)
//{
//	double dSpread = -1;
//	map<ISYMBOL, map<BROKER_KEY, TMarketData*>>::iterator it = m_mapMD.find(iSymbol);
//	if (it != m_mapMD.end())
//	{
//		map<BROKER_KEY, TMarketData*> mapBroker = (*it).second;
//		map<BROKER_KEY, TMarketData*>::iterator itBroker = mapBroker.find(string(pzBrokerKey));
//		if (itBroker != mapBroker.end())
//			dSpread = (*itBroker).second->spread;
//	}
//	return dSpread;
//}





// map<BROKER_KEY, TMarketData*>						m_mapMD;	//
BOOL CCalcBestPrc::Check_AllBrokers_SendFirstMD()
{
	if (m_bRecvFirstMDAllBrokers)
		return TRUE;

	if (m_bAddBrokerDone == FALSE)
		return FALSE;

	int nCnt = 0;
	for (map<BROKER_KEY, TMarketData*>::iterator itBroker = m_mapMD.begin(); itBroker != m_mapMD.end(); itBroker++)
	{
		TMarketData* p = (*itBroker).second;
		if (atof(p->bid) > 0 && atof(p->ask) > 0)
			nCnt++;
		else
		{
			char zMsg[128];
			sprintf(zMsg, "[MD is 0](iSymbol:%d)(%s)(%s)(%s)", m_iSymbol, (*itBroker).first.c_str(), p->bid, p->ask);
			g_debug.Log(INFO, zMsg, FALSE);
		}
	}

	if (nCnt == (int)m_mapMD.size())
	{
		m_bRecvFirstMDAllBrokers = TRUE;
	}

	return m_bRecvFirstMDAllBrokers;
}


BOOL CCalcBestPrc::GetCutOffTime(_Out_ char* pzCutOffTime)
{
	if (GetBrokerCnt() < (UINT)m_nBrokerCntTodo)
		return FALSE;

	set<MD_TIME, greater<MD_TIME> > setTime;
	for (map<BROKER_KEY, TMarketData*>::iterator itBroker = m_mapMD.begin(); itBroker != m_mapMD.end(); itBroker++) // 모든 broker
	{
		TMarketData* pBrokerMD = (*itBroker).second;
		setTime.insert(pBrokerMD->zLastMDTime);
	}

	// BID, ASK MAP 에서 시간이 늦은 것은 제거 (총 broker 의 half)
	int nTopLatest = (int)(setTime.size() * m_dCutOffTimeRate);
	if (nTopLatest < 2)
		nTopLatest = 2;

	int nLoop = 0;
	for (set<MD_TIME>::iterator itSet = setTime.begin(); itSet != setTime.end(); itSet++)
	{
		if (++nLoop == nTopLatest)
		{
			strcpy(pzCutOffTime, (*itSet).c_str());
			return TRUE;;
		}
	}

	return FALSE;
}

BOOL CCalcBestPrc::Is_LaterThanCutOffTime(_In_ char* pzTime)
{
	char zCutOffTime[32] = { 0 };
	if (!GetCutOffTime(zCutOffTime))
		return FALSE;

	return (strcmp(pzTime, zCutOffTime) >= 0);
}
/*
	map<BROKER_KEY, TMarketData*>						m_mapMD;	//
	map<BIDPRC, vector<TMarketData*>, greater<BIDPRC> >	m_mapBid;	// bid(descending), brokerKey
	map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>		m_mapAsk;	// ask(ascending), brokerKey

	This function is called only when CDataHandler::Is_ReadyTrade() is true;
*/

__ALPHA::EN_RET_VAL CCalcBestPrc::CalcBestPrc(
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
)
{
	if (FALSE == Check_AllBrokers_SendFirstMD())
		return __ALPHA::RET_SKIP;

	
	//
	// 모든 broker 를 돌면서 해당 broker 들의 BID, ASK 를 각각 BID MAP, ASK MAP 에 담아서 자동 정렬이 되게 한다.
	// 다만, 같은 가격이 있을 수 있으므로, 각 MAP 의 SECOND VALUE 는 다시 map 에 저장한다. - 최신데이터가 가장 처음
	//

	char zCutOffTime[32] = { 0 };
	if(!GetCutOffTime(zCutOffTime))
		return __ALPHA::RET_SKIP;


	// map sorted by price
	
	//typedef map< MD_TIME, TMarketData*, greater<MD_TIME>>			INMAP_MDTIME;
	//typedef map< MD_TIME, TMarketData*, greater<MD_TIME>>::iterator	IT_INMAP_MDTIME;
	map<BIDPRC, vector<TMarketData*>, greater<BIDPRC> >	mapBid;	// bid(descending), brokerKey
	map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>		mapAsk;	// ask(ascending), brokerKey

	//char temp[512];
	for (map<BROKER_KEY, TMarketData*>::iterator itBroker = m_mapMD.begin(); itBroker != m_mapMD.end(); itBroker++) // 모든 broker
	{
		Sleep(0);
		TMarketData* pBrokerMD = (*itBroker).second;

		// compare time
		if (strcmp(pBrokerMD->zLastMDTime, zCutOffTime) < 0)
			continue;

		//sprintf(temp, "[iSymbol:%d][%5.5s](Time:%s)(cutOff:%s)(bid:%s)(ask:%s)", 
		//				m_iSymbol, (*itBroker).first.c_str(), pBrokerMD->zLastMDTime, zCutOffTime, pBrokerMD->bid, pBrokerMD->ask);
		//g_debug.Log(INFO, temp, FALSE);

		// BID MAP 에 저장
		vector< TMarketData*> vecBid;
		if (!mapBid.empty())
		{
			map<BIDPRC, vector<TMarketData*>, greater<BIDPRC> >::iterator it = mapBid.find(pBrokerMD->bid);
			if (it != mapBid.end()) // find
				vecBid = (*it).second;
		}
		vecBid.push_back(pBrokerMD);
		mapBid[pBrokerMD->bid] = vecBid;


		vector< TMarketData*> vecAsk;
		if ( !mapAsk.empty())
		{
			map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>::iterator it = mapAsk.find(pBrokerMD->ask);
			if (it != mapAsk.end()) 
				vecAsk = (*it).second;
		}
		vecAsk.push_back(pBrokerMD);
		mapAsk[pBrokerMD->ask] = vecAsk;

	}



	// BID MAP 의 첫번째 가격을 가져온다.
	// ASK MAP 의 BROKER 와 일치하지 않게 한다.

	__ALPHA::EN_RET_VAL retVal = __ALPHA::RET_SKIP;


	// 첫번째 bid 가격의 broker 들을 돌면서 
	// 다시 ask 를 돌면서 같은 broker 가 아닌 가장 낮은 가격을 가져오도록 loop 한다.

	string sBestBid = (*mapBid.begin()).first;
	vector< TMarketData*> vecBid = (*mapBid.begin()).second;

	TMarketData* pBidMD = NULL;

	// bid, ask 에 여러 broker 가 달려 있을 수 있다.
	for (int iBid = 0; iBid < (int)vecBid.size(); iBid++)
	{
		pBidMD = vecBid[iBid];

		for (map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>::iterator itAsk = mapAsk.begin(); itAsk != mapAsk.end(); itAsk++)
		{
			string sBestAsk = (*itAsk).first;
			vector< TMarketData*> vecAsk = (*itAsk).second;

			for (int iAsk = 0; iAsk < (int)vecAsk.size(); iAsk++)
			{
				TMarketData* pAskMD = vecAsk[iAsk];

				if (strcmp(pBidMD->brokerKey, pAskMD->brokerKey) != 0)
				{
					retVal = __ALPHA::RET_OK;

					strcpy(pzBestBid,			sBestBid.c_str());
					strcpy(pzBidBorkerKey,		pBidMD->brokerKey);
					strcpy(pzBestBidOpposite,	pBidMD->ask);
					strcpy(pzBidMDTime,			pBidMD->zLastMDTime);
					*pdBidderSpread =			pBidMD->spread;

					strcpy(pzBestAsk,			sBestAsk.c_str());
					strcpy(pzAskBrokerKey,		pAskMD->brokerKey);
					strcpy(pzBestAskOpposite,	pAskMD->bid);
					strcpy(pzAskMDTime,			pAskMD->zLastMDTime);
					*pdAskerSpread =			pAskMD->spread;

					break;
				}
			}
			if (retVal == __ALPHA::RET_OK)
				break;
		}
	}

	//if (retVal == __ALPHA::RET_ERR)
	//{
	//	sprintf(m_zMsg, "There is no other broker in AskSide[iSymbol:%d][BidBroker:%s][CutOff:%s]", 
	//		m_iSymbol, pBidMD->brokerKey, zCutOffTime);
	//}
	return (retVal);
}


/*
	map<BROKER_KEY, TMarketData*>						m_mapMD;	//
	map<BIDPRC, vector<TMarketData*>, greater<BIDPRC> >	m_mapBid;	// bid(descending), brokerKey
	map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>		m_mapAsk;	// ask(ascending), brokerKey
*/
//VOID CCalcBestPrc::PrintMap()
//{
//	for (map<BROKER_KEY, TMarketData*>::iterator it = m_mapMD.begin(); it != m_mapMD.end(); it++)
//	{
//		TMarketData* p = (*it).second;
//		sprintf(m_zMsg, "[MD](%5.5s)(BID:%s)(ASK:%s)(%.2f)", p->brokerKey, p->bid, p->ask, p->spread);
//		g_debug.Log(INFO, m_zMsg, FALSE);
//	}
//	
//	for (map<BIDPRC, vector<TMarketData*>, greater<BIDPRC> >::iterator itBid = m_mapBid.begin(); itBid != m_mapBid.end(); itBid++)
//	{
//		vector<TMarketData*> vecBid = (*itBid).second;
//		for (int i = 0; i < (int)vecBid.size(); i++)
//		{
//			TMarketData* p = vecBid.at(i);
//			sprintf(m_zMsg, "[BIDMAP-%s](%5.5s)(Opp(ASK):%s)(%.2f)", (*itBid).first.c_str(), p->brokerKey, p->ask, p->spread);
//			g_debug.Log(INFO, m_zMsg, FALSE);
//		}
//	}
//	for (map<ASKPRC, vector<TMarketData*>, less<ASKPRC>>::iterator itAsk = m_mapAsk.begin(); itAsk != m_mapAsk.end(); itAsk++)
//	{
//		vector<TMarketData*> vecAsk = (*itAsk).second;
//		for (int i = 0; i < (int)vecAsk.size(); i++)
//		{
//			TMarketData* p = vecAsk.at(i);
//			sprintf(m_zMsg, "[ASKMAP-%s](%5.5s)(Opp(BID):%s)(%.2f)", (*itAsk).first.c_str(), p->brokerKey, p->bid, p->spread);
//			g_debug.Log(INFO, m_zMsg, FALSE);
//		}
//	}
//}