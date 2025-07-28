// Dlg_FC0.cpp : implementation file
//

#include "stdafx.h"
#include "XingAPI_Sample.h"
#include "../../CommonAnsi/IRUM_Common.h"
#include "../../CommonAnsi/MemPool.h"
//#include "../../CommonAnsi/ADOFunc.h"
#include "Dlg_FC0.h"

#include "FC0.h"	//KOSPI����
#include "NC0.h"	//CME
#include "OVC.h"	//�ؿܼ���
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

	// �ü�ó�� ������
	HANDLE hRecv = (HANDLE)_beginthreadex(NULL, 0, &RecvDataProc, this, CREATE_SUSPENDED, &m_unRecvProc);
	
	//	�������� ��������
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
		strcpy(m_zMsg, "�����ڵ������ʿ�. ini ������ Ȯ���ϼ���");
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
	�ü� ��û
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
// ��Ʈ�� �ʱ�ȭ
//--------------------------------------------------------------------------------------
void CDlg_FC0::InitCtrls()
{
	//-------------------------------------------------------------------------
	// ���� ü�� ��
// 	m_ctrlOutBlock.InsertColumn( 0, "�ʵ�  ", LVCFMT_LEFT, 150 );
// 	m_ctrlOutBlock.InsertColumn( 1, "������", LVCFMT_LEFT, 200 );
// 
// 	int nRow = 0;
// 	m_ctrlOutBlock.InsertItem( nRow++, "ü��ð�          " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "���ϴ�񱸺�      " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "���ϴ��          " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�����            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "���簡            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�ð�              " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "��              " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "����              " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "ü�ᱸ��          " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "ü�ᷮ            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�����ŷ���        " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�����ŷ����      " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�ŵ�����ü�ᷮ    " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�ŵ�����ü��Ǽ�  " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�ż�����ü�ᷮ    " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�ż�����ü��Ǽ�  " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "ü�ᰭ��          " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�ŵ�ȣ��1         " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�ż�ȣ��1         " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�̰�����������    " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "KOSPI200����      " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�̷а�            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "������            " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "����BASIS         " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�̷�BASIS         " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�̰�����������    " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "������        " );
// 	m_ctrlOutBlock.InsertItem( nRow++, "���ϵ��ð���ŷ���" );
// 	m_ctrlOutBlock.InsertItem( nRow++, "�����ڵ�          " );
// 
// 	//-------------------------------------------------------------------------
// 	// ���� ȣ�� ��
// 	m_ctrlOutBlock_H.InsertColumn( 0, "�ʵ�  ", LVCFMT_LEFT, 150 );
// 	m_ctrlOutBlock_H.InsertColumn( 1, "������", LVCFMT_LEFT, 200 );
// 	
// 	nRow = 0;
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "ȣ���ð�          " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ��1         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ��2         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ��3         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ��4         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ��5         " );	
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ��1         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ��2         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ��3         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ��4         " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ��5         " );
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ���Ǽ�1     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ���Ǽ�2     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ���Ǽ�3     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ���Ǽ�4     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ���Ǽ�5     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ���ѰǼ�    " );	
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ���Ǽ�1     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ���Ǽ�2     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ���Ǽ�3     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ���Ǽ�4     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ���Ǽ�5     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ���ѰǼ�    " );
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ������1     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ������2     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ������3     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ������4     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ������5     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ŵ�ȣ���Ѽ���    " );	
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ������1     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ������2     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ������3     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ������4     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ������5     " );
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�ż�ȣ���Ѽ���    " );	
// 
// 	m_ctrlOutBlock_H.InsertItem( nRow++, "�����ڵ�          " );

	SetDlgItemText(IDC_BUTTON_TEST, "FILE TEST Start");	// skeo 2011-01-17
}

//--------------------------------------------------------------------------------------
// ������ Advise
//--------------------------------------------------------------------------------------
void CDlg_FC0::AdviseData(CString sStk, int nTp)
{
	TCHAR	szTrCode[32];
	int		nSize;

	if (sStk.Left(3) == "101") 
	{
		// �ְ�����
		strcpy(szTrCode, ETK_REAL_SISE_F);
		FC0_InBlock st;
		nSize = sizeof(st.futcode);
	}
	else 
	{	// �ؿܼ���
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
		// ������ ����
		bSuccess = g_iXingAPI.AdviseRealData(
			GetSafeHwnd(),				// �����͸� ���� ������, XM_RECEIVE_REAL_DATA ���� �´�.
			szTrCode,					// TR ��ȣ
			stk,						// �����ڵ�
			nSize						// �����ڵ� ����
		);
	}
	else
	{
		bSuccess = g_iXingAPI.UnadviseRealData(
			GetSafeHwnd(),				// �����͸� ���� ������, XM_RECEIVE_REAL_DATA ���� �´�.
			szTrCode,					// TR ��ȣ
			sStk,						// �����ڵ�
			nSize						// �����ڵ� ����
		);
	}
	//-----------------------------------------------------------
	// ����üũ
	if( bSuccess == FALSE )
	{
		if(nTp==EN_ADVISE)	sprintf(m_zMsg,  "[%s]�ü� ��û ����", (LPCSTR)stk );
		else				sprintf(m_zMsg, "[%s]�ü� ���� ��û ����", (LPCSTR)stk);
		g_log.log(NOTIFY, m_zMsg);
	}
	else
	{
		if (nTp == EN_ADVISE)	sprintf(m_zMsg, "[%s]�ü� ��û ����", (LPCSTR)stk);
		else					sprintf(m_zMsg, "[%s]�ü� ���� ��û ����", (LPCSTR)stk);
		g_log.log(INFO, m_zMsg);
	}
	m_lstMsg.InsertString(0, m_zMsg);
	
}



//--------------------------------------------------------------------------------------
// REAL �����͸� ����
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
	//	�ؿܼ���ü��(�ü�)
	if( strcmp( pRealPacket->szTrCode, ETK_REAL_SISE_FU ) == 0 )
	{
		cStk = 'F';
		
		OVC_OutBlock* p = (OVC_OutBlock*)(pRealPacket->pszData);

		if (++g_nDebugCnt > 1000) {
			g_nDebugCnt = 0;
			g_log.log(INFO, "[�ؿܽü�](TR:%s)(%.8s)(%.6s)(%.15s)", pRealPacket->szTrCode, p->symbol, p->kortm, p->curpr);
		}

		nDataLen = pRealPacket->nDataLength;
		pCopiedData = g_memPool.get();
		memcpy(pCopiedData, pRealPacket->pszData, pRealPacket->nDataLength);
		PostThreadMessage(m_unRecvProc, WM_MD_OV_FUT, nDataLen, (LPARAM)pCopiedData);
	}

	///////////////////////////////////////////////////////////////////////////
	//	KOSPI���� �ü�
	if (strcmp(pRealPacket->szTrCode, ETK_REAL_SISE_F) == 0)
	{
		FC0_OutBlock* p = (FC0_OutBlock*)(pRealPacket->pszData);	//	DrdsSC0.h
		//g_log.log(INFO, "[KOSPI�ü�](TR:%s)(%.8s)(%.6s)(%.15s)",pRealPacket->szTrCode, p->futcode, p->chetime, p->price);

		nDataLen = pRealPacket->nDataLength;
		pCopiedData = g_memPool.get();
		memcpy(pCopiedData, pRealPacket->pszData, pRealPacket->nDataLength);
		PostThreadMessage(m_unRecvProc, WM_MD_KOSPI_FUT, nDataLen, (LPARAM)pCopiedData);
	}

	// KOSPI �߰�
	if (strcmp(pRealPacket->szTrCode, ETK_REAL_SISE_F_CME) == 0)
	{
		NC0_OutBlock* p = (NC0_OutBlock*)(pRealPacket->pszData);	//	DrdsSC0.h
		//g_log.log(INFO, "[KOSPI�߰��ü�](TR:%s)(%.8s)(%.6s)(%.6s)",
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
	//	���� �ü�/ȣ�� ����
	std::list<CString>::iterator it;
	for( it=m_lstSymbol.begin(); it!= m_lstSymbol.end(); it++ )
	{
		CString sStk = *it;
		AdviseData(sStk, EN_UNADVISE);
	}
}



