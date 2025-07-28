//---------------------------------------------------------------------------

#ifndef CChartHandlerH
#define CChartHandlerH
//---------------------------------------------------------------------------

/*
   map<appid, TPointList*> 는 한번 저장이 되면 삭제 하지 않고
   대신 TPointList 안의 내용을 지운다. (deque)

*/

#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <VCLTee.Series.hpp>
#include <list>
#include <map>
#include "../../common/AlphaInc.h"
using namespace std;


class CColor
{
public:
	CColor(){
		TColor arr[] = { clRed, clBlue,clGreen,clAqua,clPurple,clLime,clFuchsia, clYellow, clOlive};
		lstColor.insert(lstColor.end(),  arr, arr + sizeof(arr) / sizeof(arr[0]) );
	}

	~CColor(){}

	TColor	get() { TColor c = lstColor.front(); lstColor.pop_front(); return c;}
	void	set(TColor color) { lstColor.push_back(color);}

private:
	list<TColor>	lstColor;
};

//enum EN_SERIES_COLOR { SR_RED, SR_BLUE, SR_GREEN, SR_BROWN, SR_YELLOW } ;
typedef string	KEY_TIME;
typedef string 	KEY_APPID;

struct TPointUnit
{
	string	sAppId;
	string 	sBroker;
	string 	sSymbol;
	double 	dBid;
	double 	dAsk;
	string	sMktTime;
	string	sLocalTime;
	int 	nDecimalCnt;
	bool	bPainted;

	TPointUnit(){ bPainted=false; }
};


class CPointList
{
public:
	CPointList(){};
	~CPointList()
	{
		int cnt = lst.size();
		for( int i=0; i<cnt; i++ )
		{
			delete lst.front();
			lst.pop_front();
		}
	}

	list<TPointUnit*>	lst;
};

typedef map<KEY_TIME, CPointList* >      		MAP_TIME_POINTLIST;
typedef map<KEY_TIME, CPointList* >::iterator	IT_MAP_TIME_POINTLIST;
typedef map<KEY_TIME, CPointList* >::reverse_iterator	RIT_MAP_TIME_POINTLIST;

typedef map<KEY_APPID, TLineSeries*>					MAP_BROKER_SERIES;
typedef map<KEY_APPID, TLineSeries*>::iterator			IT_MAP_BROKER_SERIES;
typedef map<KEY_APPID, TLineSeries*>::reverse_iterator	RIT_MAP_BROKER_SERIES;



#define DEF_MINMAX_OFFSET	3
class CLastVal
{
public:
	string timeStamp;
	double dMinY;
	double dMaxY;

	CLastVal(){ Init(); }
	void Init() {timeStamp=""; dMinY=0; dMaxY=0; }
	bool Update(string sTime, double newVal, double dDecimalCnt)
	{
		timeStamp = sTime;
		bool bChange = false;
		if( dMinY==0 )
		{
			dMinY = newVal;
			bChange = true;
		}
		else
		{
			if( dMinY>newVal )
			{
				dMinY = newVal;
				bChange = true;
			}
		}

		if( dMaxY==0 )
		{
			dMaxY = newVal;
			bChange = true;
		}
		else
		{
			if( dMaxY<newVal )
			{
				dMaxY = newVal;
				bChange = true;
			}
		}

		if(bChange){
			double dNewMin = (dMinY * pow(10, dDecimalCnt)) - DEF_MINMAX_OFFSET;
			double dNewMax = (dMaxY * pow(10, dDecimalCnt)) + DEF_MINMAX_OFFSET;

			dMinY = dNewMin / pow(10, dDecimalCnt);
			dMaxY = dNewMax / pow(10, dDecimalCnt);
		}

		return bChange;
	}

	double getMin(){ return dMinY;}
	double getMax(){ return dMaxY;}
	char*  getTime(){ return (char*)timeStamp.c_str(); }
};

class CChartHandler
{
public:
	CChartHandler(TChart* pChart);
	~CChartHandler();

	void	__fastcall 	IF_InitChart();
	void    __fastcall  IF_NewData(string sAppId, string sBroker, string sSymbol, double dBid, double dAsk, string sMktTime, string sLocalTime, int nDecimalCnt);
	void	__fastcall	IF_RegBroker(string sAppId, string sBroker);

	void	__fastcall	IF_UnReg_AllBroker();
	//void	__fastcall 	IF_UnRegBroker (string sAppId);

	void	__fastcall	IF_TogglePrice(bool bUseAsk);
	void	__fastcall	IF_ToggleMktTime(bool bUseMktTime);

	void	__fastcall	IF_PageMove(bool bForward);


private:
//	bool	__fastcall PtList_Find(string sTimeStamp, bool bLock, _Out_ IT_MAP_TIME_POINTLIST& it);
//	void	__fastcall PtList_ResetOneBroker_ByChangeOptions(string sAppId, bool bSubs, bool bLock, bool bDelFromMap=false);
//	void    __fastcall PtList_Destroy();
//	void	__fastcall PtList_CopyPtList_ForDrawing(_Out_ list<TPointUnit*>& copyList);
//
//	void	__fastcall PtList_Lock(){ EnterCriticalSection(&m_csPtList);}
//	void	__fastcall PtList_UnLock(){ LeaveCriticalSection(&m_csPtList);}


	bool	__fastcall Series_Find(string sAppId, bool bLock, _Out_ IT_MAP_BROKER_SERIES& it);
	bool	__fastcall Series_BrokerExist(string sAppId);

	void    __fastcall Series_Delete_OneBroker(string sAppId, bool bLock);
	void    __fastcall Series_Delete_AllBrokers(bool bLock);

	void	__fastcall Series_Lock(){ EnterCriticalSection(&m_csSeries);}
	void	__fastcall Series_UnLock(){ LeaveCriticalSection(&m_csSeries);}


//	void	__fastcall	Lock() { EnterCriticalSection(&m_cs); }
//	void	__fastcall	UnLock() { LeaveCriticalSection(&m_cs); }

	void	__fastcall Set_LegendCnt();
private:
	static unsigned WINAPI ThreadDraw(LPVOID lp);

private:
	TChart*				m_chart;

//	MAP_TIME_POINTLIST	m_mapTimePtList;
//	CRITICAL_SECTION	m_csPtList;

	MAP_BROKER_SERIES	m_mapBrokerSeries;
	CRITICAL_SECTION	m_csSeries;

	HANDLE			m_hThrdDraw;
	unsigned int	m_dwThrdDraw;
	bool			m_bContinue;

	//long			m_lMaxPoints;
	long			m_lPointsPerPage;
	long			m_lCurrPointCnt;


	int 			m_nTimeoutDrawing;


	bool			m_bUseMktTime;
	bool			m_bUseAsk;

	CColor			m_color;

	CLastVal		m_lastVal;
};











#endif
