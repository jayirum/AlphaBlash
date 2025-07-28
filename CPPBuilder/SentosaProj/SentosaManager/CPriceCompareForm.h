//---------------------------------------------------------------------------

#ifndef CPriceCompareFormH
#define CPriceCompareFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "CBasicForm.h"
#include "../../common/alphaProtocol.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <VCLTee.Chart.hpp>
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>

#include <deque>
#include <map>
using namespace std;

#include "CChartHandler.h"
//---------------------------------------------------------------------------

typedef string 	APP_ID;

#define	DEF_CHECKED		1
#define	DEF_UNCHECKED	0


enum EN_SUB_TP {TP_SUB, TP_UNSUB};
struct TMDInfo
{
	int 		gridRowIdx;

	string 		sAppId;
	string 		sBroker;
	string 		sSymbol;
	double		dBid;
	double		dAsk;
	double		dSpread;
	string 		sMktTime;
	string		sLocalTime;
	int 		nDecimal;

	TCheckBox*	chk;
	//bool		bCheckedForBackup;
	TMDInfo()
	{
		gridRowIdx 	= -1;
		chk			= NULL;

	}
};

class TfrmPriceCompare : public TfrmBasic
{
__published:	// IDE-managed Components
	TShape *Shape1;
	TComboBox *cbSymbols;
	TLabel *Label2;
	TStringGrid *gdMD;
	TEdit *edtSymbol;
	TPanel *Panel1;
	TChart *Chart1;
	TCheckBox *chkUseMktTime;
	TCheckBox *chkUseAsk;
	TButton *btnSubs;
	TButton *btnPrev;
	TButton *btnNext;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall cbSymbolsChange(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall gdMDSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall chkUseMktTimeClick(TObject *Sender);
	void __fastcall btnSubsClick(TObject *Sender);
	void __fastcall btnPrevClick(TObject *Sender);
	void __fastcall btnNextClick(TObject *Sender);


protected:
	virtual void 	Exec();
	void __fastcall _Main(_In_ MSG& msg);

	void   	__fastcall	LoadSymbols();

	void 	__fastcall 	gdMD_Init();
	void	__fastcall  gdMD_UpdatePrice(CProtoGet& get);
	void 	__fastcall 	gdMD_EA_ReListUp();

	void 	__fastcall	DrawChart();
	void 	__fastcall  RequestSubUnsub(AnsiString sAppId, string sSubCmd);

	//void	__fastcall	OnClick_CheckBoxOfGrid(TObject *Sender);
	void	__fastcall	ReSubscribe();

	// delete AppId from map that doesn't exist in the _eaInfo
	void	__fastcall	Del_UnRegAppId_fromMap();

public:		// User declarations
	__fastcall TfrmPriceCompare(TComponent* Owner);

private:
	TMDInfo*  __fastcall MapGrid_Idx(string sAppId, _Out_ int* idx);
	void __fastcall MapGrid_Add_Idx_ChkBox(string sAppId, int rowIdx,TMDInfo* pMDBak);
	void __fastcall MapGrid_Clear();

	map<APP_ID, TMDInfo*>	m_mapGridIdx;  	//sAppId
	CRITICAL_SECTION		m_csGridIdx;

	CChartHandler			*m_chartHandler;

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPriceCompare *frmPriceCompare;
//---------------------------------------------------------------------------



#endif
