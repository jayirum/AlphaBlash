//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CDashBoardForm.h"
#include "uLocalCommon.h"
#include "CEAInfoManager.h"
#include "../../common/cutils.h"
#include "../../common/clogmsg.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvGrid"
#pragma link "AdvObj"
#pragma link "AdvUtil"
#pragma link "BaseGrid"
#pragma resource "*.dfm"
TfrmDashBoard *frmDashBoard;
//---------------------------------------------------------------------------
__fastcall TfrmDashBoard::TfrmDashBoard(TComponent* Owner)
	: TfrmBasic(Owner)
{
}

void __fastcall TfrmDashBoard::FormShow(TObject *Sender)
{
	gdEaInfo_Init();

	gdEaInfo_ReDraw();

	ResumeThread();

	_ThreadIds.Add(MyThreadId());

}

void   __fastcall TfrmDashBoard::gdEaInfo_Init()
{
	gdEAInfo->ColCount 	= GDEAINFO_COLCNT;
	gdEAInfo->RowCount	= 3;
	gdEAInfo->FixedCols	= 0;
	gdEAInfo->FixedRows	= 1;

	gdEAInfo->ColWidths[GDEAINFO_APPID] 	= 0;
	gdEAInfo->ColWidths[GDEAINFO_BROKER] 	= 150;
	gdEAInfo->ColWidths[GDEAINFO_ACC] 		= 80;
	gdEAInfo->ColWidths[GDEAINFO_LIVEDEMO]	= 50;
	gdEAInfo->ColWidths[GDEAINFO_IP] 		= 100;
	gdEAInfo->ColWidths[GDEAINFO_MAC] 		= 100;
	gdEAInfo->ColWidths[GDEAINFO_LOGON_MKTTIME] 		= 110;
	gdEAInfo->ColWidths[GDEAINFO_LOGON_LCOALTIME] 		= 110;

	gdEAInfo->Width = 	gdEAInfo->ColWidths[GDEAINFO_APPID] +
						gdEAInfo->ColWidths[GDEAINFO_BROKER] +
						gdEAInfo->ColWidths[GDEAINFO_ACC] +
						gdEAInfo->ColWidths[GDEAINFO_LIVEDEMO] +
						gdEAInfo->ColWidths[GDEAINFO_IP] +
						gdEAInfo->ColWidths[GDEAINFO_MAC] +
						gdEAInfo->ColWidths[GDEAINFO_LOGON_MKTTIME] +
						gdEAInfo->ColWidths[GDEAINFO_LOGON_LCOALTIME] +
						20;

	gdEAInfo->DefaultRowHeight = 20;
	gdEAInfo->Options << goFixedHorzLine;

	gdEAInfo->Cells[GDEAINFO_APPID][0] 		= "EA ID";
	gdEAInfo->Cells[GDEAINFO_BROKER][0] 	= "Broker";
	gdEAInfo->Cells[GDEAINFO_ACC][0] 		= "AccNo";
	gdEAInfo->Cells[GDEAINFO_LIVEDEMO][0]	= "Live/Demo";
	gdEAInfo->Cells[GDEAINFO_IP][0] 		= "IP Addr";
	gdEAInfo->Cells[GDEAINFO_MAC][0] 		= "Mac Addr";
	gdEAInfo->Cells[GDEAINFO_LOGON_MKTTIME][0]		= "MarketTime";
	gdEAInfo->Cells[GDEAINFO_LOGON_LCOALTIME][0] 	= "LocalTime";


}

void __fastcall TfrmDashBoard::FormCreate(TObject *Sender)
{
//  /
}


void TfrmDashBoard::Exec()
{
	while( Is_OnGoing(10) )
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			try
			{
				_Main(msg);
				delete ((CBlock*)msg.lParam);
			}
			catch(const char* e)
			{
				__log.log(ERR, "Exception:%s", e);
				frmMain->AddMsg(L"Exception happened", MSGTP_ERR);
			}
			catch(...)
			{
				frmMain->AddMsg(L"Exception happended", MSGTP_ERR);
				__log.log(ERR, "Exception:%s %s", __FILE__, __LINE__);
			}
		}
	}
}
void __fastcall TfrmDashBoard::_Main(_In_ MSG& msg)
{

	CProtoGet get;
	AnsiString sCode;
	CBlock* pData = (CBlock*)msg.lParam;

	int nFieldCnt = get.ParsingWithHeader(pData->get());

	if(nFieldCnt==0)
	{
		__log.log(ERR, "TfrmDashBoard received Wrong Packet:%s", pData->get());
		frmMain->AddMsg(L"Manager received wrong data.Contact the Support",MSGTP_ERR);
		return;
	}

	get.GetVal(FDS_CODE, &sCode);
	if( sCode==__ALPHA::CODE_COMMAND_BY_CODE )
	{
		AnsiString sCommCode;
		CHECK_BOOL(get.GetVal(FDS_COMMAND_CODE, &sCommCode), "No FDS_COMMAND_CODE");

		if(sCommCode==CMD_NOTI_LOGONOUT )
		{
			gdEaInfo_ReDraw();
		}
	}
	if( sCode==__ALPHA::CODE_BALANCE )
	{
		Balance_Exec(get);
	}
}

void __fastcall TfrmDashBoard::gdEaInfo_ReDraw()
{
	gdEaInfo_ClearForAdd(_eaInfo.Count() );

	_eaInfo.LoopStart();
	for( int i=0; i<_eaInfo.Count() ; i++ )
	{
		CEAInfo info;
		bool bExist = _eaInfo.LoopEAInfo(info);
		if(bExist==false)
			break;

		if( i == gdEAInfo->RowCount-1 )
			gdEAInfo->RowCount = i+2;

		gdEAInfo->Cells[GDEAINFO_APPID   ][i+1] = String(info.m_sAppId.c_str());
		gdEAInfo->Cells[GDEAINFO_BROKER  ][i+1] = String(info.m_sBroker.c_str());
		gdEAInfo->Cells[GDEAINFO_ACC     ][i+1] = String(info.m_sAccNo.c_str());
		gdEAInfo->Cells[GDEAINFO_LIVEDEMO][i+1] = String(info.m_sLiveDemo.c_str());
		gdEAInfo->Cells[GDEAINFO_IP      ][i+1] = String(info.m_sAppIp.c_str());
		gdEAInfo->Cells[GDEAINFO_MAC     ][i+1] = String(info.m_sAppMac.c_str());
		gdEAInfo->Cells[GDEAINFO_LOGON_MKTTIME  ][i+1] = String(info.m_sLogonMktTime.c_str());
		gdEAInfo->Cells[GDEAINFO_LOGON_LCOALTIME][i+1] = String(info.m_sLogonLocalTime.c_str());

		Balance_Subs_UnSubs(AnsiString(info.m_sAppId.c_str()), true);

	}
	_eaInfo.LoopEnd();
}

void __fastcall TfrmDashBoard::gdEaInfo_ClearForAdd(int nEACnt)
{
	if(nEACnt==0 )	nEACnt=1;
	gdEAInfo->RowCount = nEACnt+1;

	gdEAInfo->Rows[1]->Clear();
}


void __fastcall TfrmDashBoard::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------




void __fastcall TfrmDashBoard::gdEAInfoSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect)
{
	if (ARow==0) {
		return;
	}

	edtAppId->Text 		= gdEAInfo->Cells[GDEAINFO_APPID][ARow];
	edtBroker->Text 	= gdEAInfo->Cells[GDEAINFO_BROKER][ARow];
	edtAccNo->Text 		= gdEAInfo->Cells[GDEAINFO_ACC][ARow];
	edtLiveDemo->Text 	= gdEAInfo->Cells[GDEAINFO_LIVEDEMO][ARow];
	edtIP->Text 		= gdEAInfo->Cells[GDEAINFO_IP][ARow];
	edtMacAddr->Text 	= gdEAInfo->Cells[GDEAINFO_MAC][ARow];

	m_sLastAppId = edtAppId->Text;
}
//---------------------------------------------------------------------------
void __fastcall TfrmDashBoard::Balance_Subs_UnSubs(AnsiString AppId, bool bSubs)
{
	if( Trim(AppId).Length()==0)
		return;

	CBlock* pData = new CBlock();
	CProtoSet set;
	set.SetVal(FDS_CODE, 		   	__ALPHA::CODE_BALANCE);
	set.SetVal(FDS_USER_ID,			_CommonInfo.m_UserId);
	set.SetVal(FDS_KEY,				AppId.c_str());
	set.SetVal(FDS_FLOW_DIRECTION,	DIRECTION_TO_EA);
	set.SetVal(FDS_REGUNREG,		(bSubs)? DEF_REG : DEF_UNREG);
	set.SetVal(FDS_JUSTRELAY_YN,	"Y");

	int len = set.Complete(pData->get());

	TfrmBasic::RequestSendData(pData);
}



void __fastcall TfrmDashBoard::Balance_Exec(CProtoGet& get)
{
	string sAppId;
	double dBalance=0, dEquity=0, dFreeMgn=0, dProfit=0;
	CHECK_BOOL(get.GetVal(FDS_KEY, &sAppId), "No FDS_KEY");
	CHECK_BOOL(get.GetVal(FDD_BALANCE, &dBalance), "NO FDD_BALANCE");
	CHECK_BOOL(get.GetVal(FDD_EQUITY, &dEquity), "NO FDD_EQUITY");
	CHECK_BOOL(get.GetVal(FDD_FREE_MGN, &dFreeMgn), "NO FDD_FREE_MGN");
	CHECK_BOOL(get.GetVal(FDD_PROFIT, &dProfit), "NO FDD_PROFIT");

	if( String(sAppId.c_str()) != edtAppId->Text)
		return;

	CUtils util;
	char z[128];
	edtBalance->Text = String(util.FormatMoney(dBalance, 2, z));
	edtEquity->Text = String(util.FormatMoney(dEquity, 2, z));
	edtFreeMgn->Text = String(util.FormatMoney(dFreeMgn, 2, z));
	edtProfit->Text = String(util.FormatMoney(dProfit, 2, z));
}


void __fastcall TfrmDashBoard::LogOut_Request(AnsiString AppId)
{
	if(Trim(edtAppId->Text).Length()==0 )
	{
		__MsgBox_Err(L"Select EA from the table first");
		return;
	}

	CEAInfo info;
	if(!_eaInfo.GetEAInfo(AppId.c_str(), info))
		return;

	CBlock* pData = new CBlock();
	CProtoSet set;
	set.SetVal(FDS_CODE, 		   	__ALPHA::CODE_LOGOFF);
	set.SetVal(FDS_USER_ID,			_CommonInfo.m_UserId.c_str());
	set.SetVal(FDS_KEY,				info.m_sAppId.c_str());
	set.SetVal(FDS_BROKER,			info.m_sBroker.c_str());
	set.SetVal(FDS_FLOW_DIRECTION,	DIRECTION_TO_EA);

	int len = set.Complete(pData->get());

	TfrmBasic::RequestSendData(pData);
}
void __fastcall TfrmDashBoard::btnLogOutClick(TObject *Sender)
{
	if(Trim(edtAppId->Text).Length()==0 )
	{
		__MsgBox_Err(L"Select EA from the table first");
		return;
	}
	LogOut_Request(AnsiString(edtAppId->Text.c_str()));
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TfrmDashBoard::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	StopThread();

	_ThreadIds.Erase(MyThreadId());
}
//---------------------------------------------------------------------------

