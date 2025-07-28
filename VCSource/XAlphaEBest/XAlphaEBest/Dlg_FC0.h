#if !defined(AFX_DLG_FC0_H__02C7D1E4_2C21_46E3_891F_7256BFB82BF2__INCLUDED_)
#define AFX_DLG_FC0_H__02C7D1E4_2C21_46E3_891F_7256BFB82BF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_FC0.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_FC0 dialog


/***************************************************************************
	KOSPI200선물, 해외선물 시세
***************************************************************************/
#include "ListenNPublish.h"
#include <list>


#define ETK_REAL_SISE_F			"FC0"
#define ETK_REAL_SISE_O			"OC0"
#define ETK_REAL_HOGA_F			"FH0"
#define ETK_REAL_HOGA_O			"OH0"

#define ETK_REAL_HOGA_FU		"OVH"	// 해외선물호가
#define ETK_REAL_SISE_FU		"OVC"	// 해외선물시세

//CME
#define ETK_REAL_SISE_F_CME		"NC0"
#define ETK_REAL_HOGA_F_CME		"NH0"

#define MSG_RE_CONNECT		WM_USER + 889

#define DEF_TARGET_CNT	1

enum {EN_ADVISE=0, EN_UNADVISE};

class CDlg_FC0 : public CDialog
{
	DECLARE_DYNCREATE( CDlg_FC0 )
// Construction
public:
	CDlg_FC0(CWnd* pParent = NULL);   // standard constructor

	
// Dialog Data
	//{{AFX_DATA(CDlg_FC0)
	enum { IDD = IDD_FC0 };
	CListBox	m_lstMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_FC0)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	//////////////////////////////////////////////////////////////////////////
	//	USER FUNCTION
	BOOL	LoadSymbolsIni();	
	static	unsigned WINAPI RecvDataProc(LPVOID lp);
	
private:
	CListenNPublsh			*m_Pub;	
	std::list<CString>		m_lstSymbol;

	unsigned	m_unRecvProc;
	BOOL		m_bContinue;
	char		m_zMsg[1024];

protected:
	CString				m_strCode, m_strCode_H;
	void				InitCtrls();
	void				AdviseData(CString sStk, int nTp);
	//void				UnadviseData(CString sStk);

	// Generated message map functions
	//{{AFX_MSG(CDlg_FC0)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonUnadvise();
	//}}AFX_MSG
	afx_msg void		OnDestroy();
	afx_msg void		OnButtonRequest		();
	afx_msg	LRESULT		OnXMReceiveRealData	( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_FC0_H__02C7D1E4_2C21_46E3_891F_7256BFB82BF2__INCLUDED_)
