//---------------------------------------------------------------------------

#ifndef CMainFormH
#define CMainFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ToolWin.hpp>
#include "../../common/CTcpClient.h"
#include "uLocalCommon.h"


//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu;
	TMenuItem *mnuFile;
	TMenuItem *itemClose;
	TMenuItem *mnuMD;
	TMenuItem *itemPriceComare;
	TToolBar *tbAtiveWindows;
	TShape *Shape1;
	TComboBox *cbMsg;
	TTimer *tmrLogonAuth;
	TButton *btnDashBoard;
	TTimer *tmrDashBoard;
	TMemo *Memo1;
	TButton *btnPriceCompare;
	TButton *btnPosOrd;
	TTimer *tmrMsgRed;
	TPanel *Panel1;
	TButton *btnLogon;
	TButton *btnCloseApp;
	TTimer *tmrTerminate;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall tmrLogonAuthTimer(TObject *Sender);
	void __fastcall btnDashBoardClick(TObject *Sender);
	void __fastcall tmrDashBoardTimer(TObject *Sender);
	void __fastcall itemCloseClick(TObject *Sender);
	void __fastcall btnPriceCompareClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall btnPosOrdClick(TObject *Sender);
	void __fastcall tmrMsgRedTimer(TObject *Sender);
	void __fastcall btnCloseAppClick(TObject *Sender);
	void __fastcall tmrTerminateTimer(TObject *Sender);
	void __fastcall btnLogonClick(TObject *Sender);
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);

	void	AddMsg(String s, EN_MSG_TP msgTp);
	void	Set_AlreadySubRequest(){ m_bAlreadySub = true; }
	bool	Is_AlreadySubRequest(){ return m_bAlreadySub; }


protected:
	bool __fastcall SendLogon_ToRelay(AnsiString sSockTp);
	void __fastcall DeInitialize();

	void  __fastcall Initialize();
	static unsigned WINAPI Thread_Recv_RSock(LPVOID lp);
	static unsigned WINAPI Thread_Recv_SSock(LPVOID lp);
	static unsigned WINAPI Thread_Logon(LPVOID lp);
	static unsigned WINAPI Thread_ComboMsg(LPVOID lp);

	void 	__fastcall DeliverData_AllForms(string sCode, char* pzData, int nDataLen, UINT uMessage);

	virtual void 	__fastcall WndProc(TMessage& Message);
	void 			__fastcall SendData_To_Relay(TMessage& Message);

	bool  __fastcall Is_LogonCompleted() { return (m_nLogonSockCnt>=2); }
	void  __fastcall Load_DashBoard();

	void __fastcall Request_EALogonInfo();
	bool __fastcall Update_EAInfo_By_OnOff(_In_ char* pzRecvData);

private:	// User declarations
	CTcpClient*		m_sockRecv;
	CTcpClient*		m_sockSend;
	HANDLE			m_hRecvThrd_RSock, m_hSendSockThrd, m_hMsgThrd, m_hLogonThrd;
	unsigned int	m_dwRecvThrd_RSock, m_dwSendSockThrd, m_dwMsgThrd, m_dwLogonThrd;

	bool			m_bThreadContinue;
	int				m_nLogonSockCnt;
	bool			m_bAlreadySub;

	char 			m_zMsg[1024];

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
