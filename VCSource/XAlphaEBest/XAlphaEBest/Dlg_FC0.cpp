// Dlg_FC0.cpp : implementation file
//

#include "stdafx.h"
#include "XingAPI_Sample.h"
#include "../../CommonAnsi/IRUM_Common.h"
#include "../../CommonAnsi/MemPool.h"
//#include "../../CommonAnsi/ADOFunc.h"
#include "Dlg_FC0.h"

#include "FC0.h"	//KOSPI선물
#include "NC0.h"	//CME
#include "OVC.h"	//해외선물
#include <assert.h>

#include "../../CommonAnsi/Util.h"
#include "../../CommonAnsi/LogMsg.h"

extern CLogMsg	g_log;
extern char		g_zConfig[_MAX_PATH];
CMemPool		g_memPool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int g_nDebugCnt = 0;
/////////////////////////////////////////////////////////////////////////////
// CDlg_FC0 dialog

IMPLEMENT_DYNCREATE(CDlg_FC0, CDialog)

CDlg_FC0::CDlg_FC0(CWnd* pParent /*=NULL*/)	: CDialog(CDlg_FC0::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_FC0)
	//}}AFX_DATA_INIT
}


void CDlg_FC0::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_FC0)
	DDX_Control(pDX, IDC_LIST_SISEHOGA, m_lstMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_FC0, CDialog)
	//{{AFX_MSG_MAP(CDlg_FC0)
	ON_BN_CLICKED(IDC_BUTTON_UNADVISE, OnButtonUnadvise)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY  ()
	ON_BN_CLICKED( IDC_BUTTON_REQUEST,				OnButtonRequest	    )
	ON_MESSAGE	 ( WM_USER + XM_RECEIVE_REAL_DATA,	OnXMReceiveRealData	)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_FC0 message handlers

BOOL CDlg_FC0::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitCtrls();

	m_bContinue = TRUE;

	// Listn & Pub class
	m_Pub = new CListenNPublsh;
	if (m_Pub->Initialize() == FALSE)
		return FALSE;

	// 시세처리 스레드
	HANDLE hRecv = (HANDLE)_beginthreadex(NULL, 0, &RecvDataProc, this, CREATE_SUSPENDED, &m_unRecvProc);
	
	//	종목정보 가져오기
	if( !LoadSymbolsIni() ){
		return	FALSE;
	}
	
	ResumeThread(hRecv); 
	CloseHandle(hRecv);
	

	OnButtonRequest();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}




BOOL CDlg_FC0::LoadSymbolsIni()
{
	char val[128];
	CUtil::GetConfig(g_zConfig, "SYMBOLS", "SYMBOL_CNT", val);
	int nCnt = atoi(val);
	if (nCnt <= 0)
	{
		strcpy(m_zMsg, "종목코드점검필요. ini 파일을 확인하세요");
		MessageBox(m_zMsg);
		g_log.log(NOTIFY, m_zMsg);
		return FALSE;
	}

	for (int i = 1; i < nCnt + 1; i++)
	{
		char zKey[32], zVal[32];
		sprintf(zKey, "SYMBOL%d", i);
		CUtil::GetConfig(g_zConfig, "SYMBOLS", zKey, zVal);
		CUtil::TrimAll(zVal, strlen(zVal));
		m_lstSymbol.push_back(zVal);
	}	
	return TRUE;
}
void CDlg_FC0::OnDestroy()
{
	m_bContinue = FALSE;

	delete m_Pub;

	OnButtonUnadvise();

	CDialog::OnDestroy();
}

/***************************************
	시세 요청
***************************************/
void CDlg_FC0::OnButtonRequest() 
{	
	std::list<CString>::iterator it;
	for( it= m_lstSymbol.begin(); it!= m_lstSymbol.end(); it++ )
	{
		CString sStk = *it;
		AdviseData(sStk, EN_ADVISE);
	}

}


//--------------------------------------------------------------------------------------
// 컨트롤 초기화
//--------------------------------------------------------------------------------------
void CDlg_FC0::InitCtrls()
{
	//-------------------------------------------------------------------------
	// 선물 체결 용
// 	m_ctrlOutBlock.InsertColumn( 0, "필드  ", LVCFMT_LEFT, 150 );
// 	m_ctrlOutBlock.InsertColumn( 1, "데이터", LVCFMT_LEFT, 200 );
// 
// 	int nRow = 0;
// 	m_ctrlOutBlock.InsertItem( nRow++, "체결시간          " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "전일대비구분      " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "전일대비          " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "등락율            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "현재가            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "시가              " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "고가              " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "저가              " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "체결구분          " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "체결량            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "누적거래량        " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "누적거래대금      " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "매도누적체결량    " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "매도누적체결건수  " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "매수누적체결량    " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "매수누적체결건수  " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "체결강도          " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "매도호가1         " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "매수호가1         " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "미결제약정수량    " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "KOSPI200지수      " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "이론가            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "괴리율            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "시장BASIS         " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "이론BASIS         " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "미결제약정증감    " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "장운영정보        " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "전일동시간대거래량" );
// 	m_ctrlOutBlock.InsertItem( nRow++, "단축코드          " );
// 
// 	//-------------------------------------------------------------------------
// 	// 선물 호가 용
// 	m_ctrlOutBlock_H.InsertColumn( 0, "필드  ", LVCFMT_LEFT, 150 );
// 	m_ctrlOutBlock_H.InsertColumn( 1, "데이터", LVCFMT_LEFT, 200 );
// 	
// 	nRow = 0;
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "호가시간          " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가1         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가2         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가3         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가4         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가5         " );	
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가1         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가2         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가3         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가4         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가5         " );
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가건수1     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가건수2     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가건수3     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가건수4     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가건수5     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가총건수    " );	
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가건수1     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가건수2     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가건수3     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가건수4     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가건수5     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가총건수    " );
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가수량1     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가수량2     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가수량3     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가수량4     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가수량5     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매도호가총수량    " );	
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가수량1     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가수량2     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가수량3     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가수량4     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가수량5     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "매수호가총수량    " );	
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "단축코드          " );

	SetDlgItemText(IDC_BUTTON_TEST, "FILE TEST Start");	// skeo 2011-01-17
}

//--------------------------------------------------------------------------------------
// 데이터 Advise
//--------------------------------------------------------------------------------------
void CDlg_FC0::AdviseData(CString sStk, int nTp)
{
	TCHAR	szTrCode[32];
	int		nSize;

	if (sStk.Left(3) == "101") 
	{
		// 주간선물
		strcpy(szTrCode, ETK_REAL_SISE_F);
		FC0_InBlock st;
		nSize = sizeof(st.futcode);
	}
	else 
	{	// 해외선물
		strcpy(szTrCode, ETK_REAL_SISE_FU);
		OVC_InBlock st;
		nSize = sizeof(st.symbol);
	}


	CString stk;
	stk.Format("%-*.*s", nSize, nSize, (LPCSTR)sStk);
	BOOL bSuccess;
	if (nTp == EN_ADVISE)
	{
		//-----------------------------------------------------------
		// 데이터 전송
		bSuccess = g_iXingAPI.AdviseRealData(
			GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
			szTrCode,					// TR 번호
			stk,						// 종목코드
			nSize						// 종목코드 길이
		);
	}
	else
	{
		bSuccess = g_iXingAPI.UnadviseRealData(
			GetSafeHwnd(),				// 데이터를 받을 윈도우, XM_RECEIVE_REAL_DATA 으로 온다.
			szTrCode,					// TR 번호
			sStk,						// 종목코드
			nSize						// 종목코드 길이
		);
	}
	//-----------------------------------------------------------
	// 에러체크
	if( bSuccess == FALSE )
	{
		if(nTp==EN_ADVISE)	sprintf(m_zMsg,  "[%s]시세 요청 실패", (LPCSTR)stk );
		else				sprintf(m_zMsg, "[%s]시세 중지 요청 실패", (LPCSTR)stk);
		g_log.log(NOTIFY, m_zMsg);
	}
	else
	{
		if (nTp == EN_ADVISE)	sprintf(m_zMsg, "[%s]시세 요청 성공", (LPCSTR)stk);
		else					sprintf(m_zMsg, "[%s]시세 중지 요청 성공", (LPCSTR)stk);
		g_log.log(INFO, m_zMsg);
	}
	m_lstMsg.InsertString(0, m_zMsg);
	
}



//--------------------------------------------------------------------------------------
// REAL 데이터를 받음
//--------------------------------------------------------------------------------------
LRESULT CDlg_FC0::OnXMReceiveRealData( WPARAM wParam, LPARAM lParam )
{
	char msg[4096] = {0,};
	char szSendBuf[1024] = {0,};
	//char szSendBuf_toFRONT[MAX_SENDSIZE] = {0,};
	char cStk;
	BOOL bHoga = FALSE;	
	BOOL bFind = FALSE;
	BOOL bDongsi = FALSE;
	
	char* pCopiedData = NULL;
	int nDataLen = 0;

	LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;

	//////////////////////////////////////////////////////////////////////////
	//	해외선물체결(시세)
	if( strcmp( pRealPacket->szTrCode, ETK_REAL_SISE_FU ) == 0 )
	{
		cStk = 'F';
		
		OVC_OutBlock* p = (OVC_OutBlock*)(pRealPacket->pszData);

		if (++g_nDebugCnt > 1000) {
			g_nDebugCnt = 0;
			g_log.log(INFO, "[해외시세](TR:%s)(%.8s)(%.6s)(%.15s)", pRealPacket->szTrCode, p->symbol, p->kortm, p->curpr);
		}

		nDataLen = pRealPacket->nDataLength;
		pCopiedData = g_memPool.get();
		memcpy(pCopiedData, pRealPacket->pszData, pRealPacket->nDataLength);
		PostThreadMessage(m_unRecvProc, WM_MD_OV_FUT, nDataLen, (LPARAM)pCopiedData);
	}

	///////////////////////////////////////////////////////////////////////////
	//	KOSPI선물 시세
	if (strcmp(pRealPacket->szTrCode, ETK_REAL_SISE_F) == 0)
	{
		FC0_OutBlock* p = (FC0_OutBlock*)(pRealPacket->pszData);	//	DrdsSC0.h
		//g_log.log(INFO, "[KOSPI시세](TR:%s)(%.8s)(%.6s)(%.15s)",pRealPacket->szTrCode, p->futcode, p->chetime, p->price);

		nDataLen = pRealPacket->nDataLength;
		pCopiedData = g_memPool.get();
		memcpy(pCopiedData, pRealPacket->pszData, pRealPacket->nDataLength);
		PostThreadMessage(m_unRecvProc, WM_MD_KOSPI_FUT, nDataLen, (LPARAM)pCopiedData);
	}

	// KOSPI 야간
	if (strcmp(pRealPacket->szTrCode, ETK_REAL_SISE_F_CME) == 0)
	{
		NC0_OutBlock* p = (NC0_OutBlock*)(pRealPacket->pszData);	//	DrdsSC0.h
		//g_log.log(INFO, "[KOSPI야간시세](TR:%s)(%.8s)(%.6s)(%.6s)",
		//	pRealPacket->szTrCode, p->futcode, p->chetime, p->price);

		nDataLen = pRealPacket->nDataLength;
		pCopiedData = g_memPool.get();
		memcpy(pCopiedData, pRealPacket->pszData, pRealPacket->nDataLength);
		PostThreadMessage(m_unRecvProc, WM_MD_KOSPI_CME, nDataLen, (LPARAM)pCopiedData);
	}
			
	return 0L;
}



unsigned WINAPI CDlg_FC0::RecvDataProc(LPVOID lp)
{
	CDlg_FC0* pThis = (CDlg_FC0*)lp;
	while (pThis->m_bContinue)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_MD_KOSPI_CME:
			case WM_MD_KOSPI_FUT:
			case WM_MD_OV_FUT:
				pThis->m_Pub->SendData(msg.message, (char*)msg.lParam, msg.wParam);
				g_memPool.release((char*)msg.lParam);
				break;
			default:
				//TODO. LOGGING
				break;
			}
		}
	}

	return 0;
}

void CDlg_FC0::OnButtonUnadvise() 
{
	// TODO: Add your control notification handler code here
	//	선물 시세/호가 해제
	std::list<CString>::iterator it;
	for( it=m_lstSymbol.begin(); it!= m_lstSymbol.end(); it++ )
	{
		CString sStk = *it;
		AdviseData(sStk, EN_UNADVISE);
	}
}



