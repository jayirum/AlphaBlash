//---------------------------------------------------------------------------

#ifndef CLogonFormH
#define CLogonFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "../../Common/CTcpClient.h"
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include "uLocalCommon.h"

//---------------------------------------------------------------------------
class TfrmLogon : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlMsg;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *edtUserID;
	TEdit *edtPwd;
	TButton *btnLogon;
	TButton *btnCancel;
	TTimer *Timer1;
	TTimer *tmrClose;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall btnLogonClick(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall tmrCloseTimer(TObject *Sender);
public:		// User declarations
	__fastcall TfrmLogon(TComponent* Owner);

public:
	static unsigned WINAPI Thread_Recv(LPVOID lp);
    bool 	Is_Cancelled() { return m_bCancel; }
private:

	bool 	__fastcall Initialize();
	bool 	__fastcall Connect();
	void 	__fastcall DeInitialize();
	void 	__fastcall SendLogAuthData();
	void	__fastcall ResumeRecvProcThread() { ::ResumeThread(m_hThreadRecv); }

private:	// User declarations
	HANDLE			m_hThreadRecv;
	unsigned int	m_dwThreadID;
	bool 			m_bThreadContinue;

	CTcpClient		*m_sockAuth;
	bool 			m_bCancel;
	char 			m_zMsg[1024];

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmLogon *frmLogon;
//---------------------------------------------------------------------------
#endif
