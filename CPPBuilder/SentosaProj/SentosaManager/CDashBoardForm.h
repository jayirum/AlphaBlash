//---------------------------------------------------------------------------

#ifndef CDashBoardFormH
#define CDashBoardFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "CBasicForm.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include "../../common/alphaProtocol.h"
#include "AdvGrid.hpp"
#include "AdvObj.hpp"
#include "AdvUtil.hpp"
#include "BaseGrid.hpp"
//---------------------------------------------------------------------------
class TfrmDashBoard : public TfrmBasic
{
__published:	// IDE-managed Components
	TLabel *lblEaInfo;
	TShape *Shape1;
	TLabel *Label1;
	TPanel *pnlAppDetails;
	TLabel *Label2;
	TEdit *edtAppId;
	TLabel *Label3;
	TEdit *edtBroker;
	TLabel *Label4;
	TEdit *edtAccNo;
	TLabel *Label5;
	TEdit *edtIP;
	TLabel *Label6;
	TLabel *Label7;
	TEdit *edtMacAddr;
	TEdit *edtLogonTime;
	TButton *btnLogOut;
	TLabel *Label8;
	TEdit *edtLiveDemo;
	TEdit *edtBalance;
	TEdit *edtEquity;
	TEdit *edtFreeMgn;
	TEdit *edtProfit;
	TButton *btnOrderDetail;
	TAdvStringGrid *gdEAInfo;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall gdEAInfoSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall btnLogOutClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);


public:		// User declarations
	__fastcall TfrmDashBoard(TComponent* Owner);


	//virtual void __fastcall WndProc(TMessage& Message);

protected:
	virtual void 	Exec();
	void __fastcall	_Main(_In_ MSG& msg);

	void __fastcall gdEaInfo_ReDraw();
	void __fastcall gdEaInfo_Init();
	void __fastcall gdEaInfo_ClearForAdd(int nEACnt);

	void __fastcall Balance_Subs_UnSubs(AnsiString AppId, bool bSubs);
	void __fastcall Balance_UnSubs(AnsiString AppId);
	void __fastcall Balance_Exec(CProtoGet& get);


	void __fastcall LogOut_Request(AnsiString AppId);

private:	// User declarations

	String	m_sLastAppId;

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmDashBoard *frmDashBoard;
//---------------------------------------------------------------------------
#endif
