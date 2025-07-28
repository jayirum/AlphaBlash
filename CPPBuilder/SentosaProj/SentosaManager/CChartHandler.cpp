//---------------------------------------------------------------------------

#pragma hdrstop

#include "CChartHandler.h"
#include "CMainForm.h"
#include <process.h>
#include "../../common/clogmsg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


#define CHART_MARGIN_TOP 2

CChartHandler::CChartHandler(TChart* pChart)
{
	m_chart = pChart;
	InitializeCriticalSection(&m_csSeries);
	m_bUseMktTime 	= false;
	m_bUseAsk		= false;
	m_lCurrPointCnt	= 0;
}

CChartHandler::~CChartHandler()
{
	Series_Delete_AllBrokers(true);
	DeleteCriticalSection(&m_csSeries);
}

void __fastcall CChartHandler::IF_InitChart()
{

	// read config file
	{
		//TODO
		//m_lMaxPoints 		= 100;
		m_lPointsPerPage	= 100;
		m_nTimeoutDrawing	= 5000;
    }
	// for performance
	{
		m_chart->View3D = false;

		// When using only a single thread, disable locking:
		m_chart->Canvas->ReferenceCanvas->Pen->OwnerCriticalSection = NULL;

		// Set axis calculations in "fast mode".
		m_chart->Axes->FastCalc = true;

		m_chart->Walls->Back->Gradient->Visible = false;

		m_chart->AutoRepaint = true;  // if false need repaint or refresh

		m_chart->MaxPointsPerPage = m_lPointsPerPage;

		m_chart->Page = 0;

	}


	m_chart->MarginTop = CHART_MARGIN_TOP;
//	m_chart->Title->Text->Add("[ Price Comparision ]");
//	m_chart->Title->Font->Size 	= 10;
//	m_chart->Title->Font->Color = clBlue;
//	m_chart->Title->Font->Style << fsBold;
//	m_chart->Title->Alignment = taLeftJustify;

	m_chart->Axes->Right->Visible = true;
	m_chart->Axes->Left->Visible = false;

	m_chart->Axes->Right->AxisValuesFormat = L"#,##0.####0";
	m_chart->Axes->Right->Automatic = false;
	//m_chart->Axes->Right->SetMinMax(0.9, 1.9);


	m_chart->Axes->Bottom->DateTimeFormat = L"dd/mmm/yy hh:mm:ss";
	m_chart->Axes->Bottom->Increment = DateTimeStep[ dtFiveSeconds ];
	m_chart->Axes->Bottom->MinorTickCount = 4;

	m_chart->Walls->Left->Visible = false;
	m_chart->Walls->Bottom->Visible = false;

	m_chart->Legend->Title->Caption = "Brokers";
	//TODO m_chart->Legend->MaxNumRows =2;
	m_chart->Legend->LegendStyle = lsSeries;//  (lsAuto, lsSeries, lsValues, lsLastValues, lsSeriesGroups);
	m_chart->Legend->Left = m_chart->Left;
	m_chart->Legend->Top  = m_chart->Top - m_chart->Legend->Height;


	m_bContinue = true;
	m_hThrdDraw = (HANDLE)_beginthreadex(NULL, 0, &ThreadDraw, this, 0, &m_dwThrdDraw);


}

void __fastcall CChartHandler::Set_LegendCnt()
{
	m_chart->Legend->MaxNumRows =  m_mapBrokerSeries.size();
}



void	__fastcall	CChartHandler::IF_UnReg_AllBroker()
{
	Series_Delete_AllBrokers(true);
}


void	__fastcall	CChartHandler::IF_RegBroker(string sAppId, string sBroker)
{
	Series_Lock();

	IT_MAP_BROKER_SERIES it;
	if( !Series_Find(sAppId, false, it) )
	{
		TLineSeries* series = new TLineSeries (m_chart);
		series->Title			= String().sprintf(L"%.7s", String(sBroker.c_str()));
		series->XValues->Order 	= loNone;
		series->VertAxis 		= aRightAxis;
		series->HorizAxis 		= aBottomAxis;
		series->Color			= m_color.get();
		series->LinePen->OwnerCriticalSection = NULL;

		m_chart->AddSeries(series);
		//
		m_mapBrokerSeries[sAppId] = series;
		//
		frmMain->Memo1->Lines->Insert(0, L"ADDSeries");
	}
	Series_UnLock();
}

void    __fastcall  CChartHandler::IF_NewData
(string sAppId, string sBroker, string sSymbol, double dBid, double dAsk, string sMktTime, string sLocalTime, int nDecimalCnt)
{
	__log.log(INFO, "[IF_NewData]Incoming Data(%s)(%s)(%s)(%f)", sAppId.c_str(), sBroker.c_str(), sLocalTime.c_str(), dBid );
    IT_MAP_BROKER_SERIES it;
	if( !Series_Find(sAppId, true, it) )
	{
		IF_RegBroker(sAppId,sBroker);
	}


		string sTimeStamp = (m_bUseMktTime)? sMktTime : sLocalTime;
		__log.log(INFO, "[IF_NewData]sTimeStamp(%s) Local(%s)", sTimeStamp.c_str(), sLocalTime.c_str() );

		TPointUnit* p 	= new TPointUnit;
		p->sAppId 	= sAppId;
		p->sBroker	= sBroker;
		p->sSymbol	= sSymbol;
		p->dBid		= dBid;
		p->dAsk		= dAsk;
		p->sMktTime	= sMktTime;
		p->sLocalTime = sLocalTime;
		p->nDecimalCnt = nDecimalCnt;


		//TODO
//		while( m_lCurrPointCnt >= m_lMaxPoints)
//		{
//			CPointList* pList = (*m_mapTimePtList.begin()).second;
//			m_lCurrPointCnt -= pList->lst.size();
//			delete pList;
//
//			m_mapTimePtList.erase( m_mapTimePtList.begin() );
//		}


	PostThreadMessage(m_dwThrdDraw, WM_CHART_DRAW, (WPARAM)0, (LPARAM)p);

}



void	__fastcall	CChartHandler::IF_ToggleMktTime(bool bUseMktTime)
{
	m_bUseMktTime = bUseMktTime;
}


void    __fastcall CChartHandler::Series_Delete_AllBrokers( bool bLock)
{
	if(bLock) Series_Lock();

	try{
		for( IT_MAP_BROKER_SERIES it= m_mapBrokerSeries.begin(); it!=m_mapBrokerSeries.end(); ++it )
		{
			TLineSeries* series = (*it).second;
			series->Clear();
			delete series;
		}
		m_mapBrokerSeries.clear();
	}
	__finally{
		if(bLock) Series_UnLock();
    }
}

void    __fastcall CChartHandler::Series_Delete_OneBroker(string sAppId, bool bLock)
{
	if(bLock) Series_Lock();

	try{
		IT_MAP_BROKER_SERIES it;
		if(Series_Find(sAppId, !bLock, it))
		{
			TLineSeries* series = (*it).second;
			m_color.set(series->Color);
			series->Clear();
			delete series;

			m_mapBrokerSeries.erase(it);
		}
	}
	__finally{
		if(bLock) Series_UnLock();
    }
}

void	__fastcall	CChartHandler::IF_TogglePrice(bool bUseAsk)
{
	m_bUseAsk = bUseAsk;
}


bool __fastcall CChartHandler::Series_Find(string sAppId, bool bLock, _Out_ IT_MAP_BROKER_SERIES& it)
{
	if(bLock) Series_Lock();
	__try
	{
		it = m_mapBrokerSeries.find(sAppId);
	}
	__finally{
	if(bLock) Series_UnLock();
	}
	return (it!=m_mapBrokerSeries.end());
}




unsigned WINAPI CChartHandler::ThreadDraw(LPVOID lp)
{
	CChartHandler* p = (CChartHandler*)lp;
	bool bAddToSeries = false;
	while(p->m_bContinue)
	{
		Sleep(p->m_nTimeoutDrawing);

		MSG msg;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message!=WM_CHART_DRAW)
				continue;

			if( p->m_mapBrokerSeries.empty() )
				continue;


			TPointUnit* pUnit = (TPointUnit*)msg.lParam;

			IT_MAP_BROKER_SERIES itMap;

			for( itMap=p->m_mapBrokerSeries.begin(); itMap!=p->m_mapBrokerSeries.end(); ++itMap )
			{
				string	sAppId 		= (*itMap).first;
				TLineSeries* series	= (*itMap).second;

				string sTimeStamp = (p->m_bUseMktTime)? pUnit->sMktTime : pUnit->sLocalTime;
				double dPrc 	  = (p->m_bUseAsk)? 	pUnit->dAsk		: pUnit->dBid;

				if( p->m_lastVal.Update(sTimeStamp, dPrc, pUnit->nDecimalCnt) )
				{
					p->m_chart->Axes->Right->SetMinMax( p->m_lastVal.getMin(), p->m_lastVal.getMax());
				}

				if( sAppId == pUnit->sAppId )
				{
					series->Add(dPrc, String(sTimeStamp.c_str()));
					__log.log(INFO, "[DRAW] Add Series. (%s) Time(%s) Prc(%f)", pUnit->sBroker.c_str(), sTimeStamp.c_str(), dPrc);
				}
				else
				{
					series->AddNull(dPrc);
					__log.log(INFO, "[DRAW] Add Null. (%s) Time(%s) Prc(%f)", pUnit->sBroker.c_str(), sTimeStamp.c_str(), dPrc);
                }

			}
			delete pUnit;

		} // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	}

    return 0;
}

void	__fastcall	CChartHandler::IF_PageMove(bool bForward)
{
	if(bForward)
		m_chart->NextPage();
	else
		m_chart->PreviousPage();
}
