#include "CSaveMD.h"
#include <process.h>
#include "../CommonAnsi/Util.h"
#include "../Common/AlphaInc.h"
#include "../Common/AlphaProtocolUni.h"

extern TCHAR	g_zConfig[_MAX_PATH];

CSaveMD::CSaveMD(string sSymbol)
{
	m_sSymbol	= sSymbol;
	m_csv		= NULL;
	m_bDie		= FALSE;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &Thread_Main, this, CREATE_SUSPENDED, &m_unThreadId);
}

CSaveMD::~CSaveMD()
{
	if (m_csv) delete m_csv;
	m_bDie = TRUE;
}


BOOL CSaveMD::Initialize()
{
	char zTemp[128] = { 0, };
	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("APP_CONFIG"), zTemp);	
	m_nTimeoutSaveSec = atoi(zTemp);

	CUtil::GetConfig(g_zConfig, TEXT("APP_CONFIG"), TEXT("DIR_CSV"), zTemp);
	m_csv = new CCsvFile(m_sSymbol, zTemp);

	::ResumeThread(m_hThread);
	return TRUE;
}

void CSaveMD::ReceiveProcess(const char* pRecvData, const int nRecvLen)
{
	char* pData = new char[nRecvLen];
	memcpy(pData, pRecvData, nRecvLen);
	PostThreadMessage(m_bDie, WM_RECEIVE_DATA, (WPARAM)nRecvLen, (LPARAM)pData);
}


unsigned WINAPI CSaveMD::Thread_Main(LPVOID lp)
{
	CSaveMD* p = (CSaveMD*)lp;
	while(!p->m_bDie)
	{ 
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			BOOL bDelete = TRUE;
			if (msg.message == WM_RECEIVE_DATA)
			{
				if (msg.message != WM_RECEIVE_DATA)
					continue;

				int nRecvLen = (int)msg.wParam;
				char* pRecvData = (char*)msg.lParam;

				p->Save_OnMemory(pRecvData, nRecvLen);

				delete pRecvData;
			}
		}

		p->Save_OnCSV();
	}
	return 0;
}

// write every (x) second
VOID CSaveMD::Save_OnCSV()
{
	SYSTEMTIME st; 
	char zDate[32], zTime[32];
	GetLocalTime(&st);
	sprintf(zDate, "%04d.%02d.%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(zTime, "%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); // hh:mm:ss.mmm
	int nSec = atoi( CUtil::stringFormat("%.2s", zTime+6).c_str());

	if (nSec % m_nTimeoutSaveSec != 0)
		return;

	for (m_itMD = m_mapMD.begin(); m_itMD != m_mapMD.end(); m_itMD++)
	{
		TMarketData* p = (TMarketData*)((*m_itMD).second);
		m_csv->WriteData(zDate, zTime, p->sTimeMT4.data(), (*m_itMD).first.data(), p->sBid.data(), p->sAsk.data());
	}
}

BOOL CSaveMD::Save_OnMemory(char* pRecvData, int nRecvLen)
{
	CProtoGet get;
	if (!get.ParsingWithHeader(pRecvData, nRecvLen))
	{
		//TODO LOGGING(ERR, TRUE, TRUE, "[Candle_Process]parsing error(%s)", get.GetMsg());
		return FALSE;
	}

	char zBroker[128] = { 0 }, zMDTime[32] = { 0 };
	double dBid, dAsk;

	get.GetVal(FDS_KEY, zBroker);
	get.GetVal(FDS_MARKETDATA_TIME, zMDTime);
	dBid = get.GetValD(FDD_BID);
	dAsk = get.GetValD(FDD_ASK);

	TMarketData* pMD = NULL;
	if (!FindMap(zBroker))
		pMD = new TMarketData;
	else
		pMD = (*m_itMD).second;

	pMD->sBid = CUtil::stringFormat("%.5f", dBid);
	pMD->sAsk = CUtil::stringFormat("%.5f", dAsk);
	pMD->sTimeMT4 = zMDTime;

	return TRUE;
}

BOOL CSaveMD::FindMap(string sBroker)
{
	m_itMD = m_mapMD.find(sBroker);
	return (m_itMD != m_mapMD.end());
}