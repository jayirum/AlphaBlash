//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CPriceCompareForm.h"
#include "../../common/CTimeUtils.h"
#include "../../common/CUtils.h"
#include "../../common/CLogMsg.h"
#include "CEAInfoManager.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmPriceCompare *frmPriceCompare;



//---------------------------------------------------------------------------
__fastcall TfrmPriceCompare::TfrmPriceCompare(TComponent* Owner)
	: TfrmBasic(Owner)
{
	InitializeCriticalSection(&m_csGridIdx);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPriceCompare::FormShow(TObject *Sender)
{
	LoadSymbols();

	gdMD_Init();

	m_chartHandler = new CChartHandler(Chart1);
	m_chartHandler->IF_InitChart() ;

	ResumeThread();

	_ThreadIds.Add(MyThreadId());
}
//---------------------------------------------------------------------------


void   __fastcall TfrmPriceCompare::gdMD_Init()
{
	gdMD->ColCount 	= GDMD_COLCNT;
	gdMD->RowCount	= 2;
	gdMD->FixedCols	= 1;
	gdMD->FixedRows	= 1;

	gdMD->ColWidths[GDMD_CHKBOX] 	= 20;		//HIDDEN
	gdMD->ColWidths[GDMD_APPID] 	= 0;		//HIDDEN
	gdMD->ColWidths[GDMD_BROKER] 	= 110;
	gdMD->ColWidths[GDMD_SYMBOL]	= 70;
	gdMD->ColWidths[GDMD_BID] 		= 70;
	gdMD->ColWidths[GDMD_ASK]		= 70;
	gdMD->ColWidths[GDMD_SPREAD] 	= 40;
	gdMD->ColWidths[GDMD_MKT_TIME] 	= 80;
	gdMD->ColWidths[GDMD_LOCAL_TIME]= 80;

	gdMD->Width =       gdMD->ColWidths[GDMD_CHKBOX] +
						gdMD->ColWidths[GDMD_APPID] +
						gdMD->ColWidths[GDMD_BROKER] +
						gdMD->ColWidths[GDMD_SYMBOL] +
						gdMD->ColWidths[GDMD_BID] +
						gdMD->ColWidths[GDMD_ASK] +
						gdMD->ColWidths[GDMD_SPREAD] +
						gdMD->ColWidths[GDMD_MKT_TIME] +
						gdMD->ColWidths[GDMD_LOCAL_TIME] +
						20;

	gdMD->DefaultRowHeight = 20;
	gdMD->Options << goFixedHorzLine;

	//gdMD->Cells[GDMD_CHKBOX][0] 	= CreateCheckBoxInGrid(0);
	gdMD->Cells[GDMD_APPID][0] 		= "APP ID";
	gdMD->Cells[GDMD_BROKER][0] 	= "Broker";
	gdMD->Cells[GDMD_SYMBOL][0] 	= "Symbol";
	gdMD->Cells[GDMD_BID][0] 		= "Bid";
	gdMD->Cells[GDMD_ASK][0]		= "Ask";
	gdMD->Cells[GDMD_SPREAD][0] 	= "Spread";
	gdMD->Cells[GDMD_MKT_TIME][0] 	= "Market TM";
	gdMD->Cells[GDMD_LOCAL_TIME][0]	= "Local TM";


	gdMD_EA_ReListUp();
}


//
//void	__fastcall	TfrmPriceCompare::OnClick_CheckBoxOfGrid(TObject *Sender)
//{
//	//EnterCriticalSection(&m_csGridIdx);
//
//	__try
//	{
//		if( cbSymbols->ItemIndex < 0 )
//		{
//			return;
//		}
//		bool bSymbolChanged = false;
//		if(cbSymbols->Tag!=cbSymbols->ItemIndex)
//		{
//			bSymbolChanged = true;
//			cbSymbols->Tag = cbSymbols->ItemIndex;
//
//			//
//			m_chartHandler->IF_ChangeSymbol();
//			//
//		}
//
//		for( map<string, TMDInfo*>::iterator it=m_mapGridIdx.begin(); it!=m_mapGridIdx.end(); ++it)
//		{
//			string sAppId 	= (*it).first.c_str();
//			TMDInfo* pInfo 	= (*it).second;
//
//			TCheckBox* pChk 	= pInfo->chk;
//
//			if( pChk->Checked )
//			{
//				if( bSymbolChanged || (!bSymbolChanged && pChk->Tag==DEF_UNCHECKED) ){
//					RequestSubUnsub(AnsiString(sAppId.c_str()),  CMD_MD_SUB);
//					frmMain->Set_AlreadySubRequest();
//				}
//
//				pChk->Tag = DEF_CHECKED;
//			}
//			else
//			{
//				if( bSymbolChanged || (!bSymbolChanged && pChk->Tag==DEF_CHECKED) )
//				{
//					RequestSubUnsub(AnsiString(sAppId.c_str()),  CMD_MD_UNSUB);
//
//					gdMD->Cells[GDMD_SYMBOL    ][pInfo->gridRowIdx] = L"Unsubscribe";
//					gdMD->Cells[GDMD_BID       ][pInfo->gridRowIdx] = L"";
//					gdMD->Cells[GDMD_ASK       ][pInfo->gridRowIdx] = L"";
//					gdMD->Cells[GDMD_SPREAD    ][pInfo->gridRowIdx] = L"";
//					gdMD->Cells[GDMD_MKT_TIME  ][pInfo->gridRowIdx] = L"";
//					gdMD->Cells[GDMD_LOCAL_TIME][pInfo->gridRowIdx] = L"";
//
//					m_chartHandler->IF_ToggleSubscribe(false, sAppId);
//				}
//				pChk->Tag = DEF_UNCHECKED;
//			}
//			m_mapGridIdx[sAppId] = pInfo;
//		}
//	}
//	__finally{
//		//LeaveCriticalSection(&m_csGridIdx);
//	}
//}


void TfrmPriceCompare::Exec()
{
    while( Is_OnGoing(10) )
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			try
			{
				EnterCriticalSection(&m_csGridIdx);
				_Main(msg);
				delete ((CBlock*)msg.lParam);
				LeaveCriticalSection(&m_csGridIdx);
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

void __fastcall TfrmPriceCompare::_Main(_In_ MSG& msg)
{

	if(msg.message ==WM_EA_LOGONOFF )
	{
		gdMD_EA_ReListUp();
		//OnClick_CheckBoxOfGrid(this);
		return;
	}

	CProtoGet get;
	String sMsg;
	CBlock* pData = (CBlock*)msg.lParam;
	int nFieldCnt = get.ParsingWithHeader(pData->get());
	if(nFieldCnt==0)
	{
		__log.log(ERR, "TfrmPriceCompare received Wrong Packet:%s", pData->get());
		frmMain->AddMsg(L"Manager received the wrong data", MSGTP_ERR);
		return;
	}

	AnsiString sCode;
	get.GetVal(FDS_CODE, &sCode);

	if( sCode==__ALPHA::CODE_MARKET_DATA )
	{
		gdMD_UpdatePrice(get);
	}

}




void	__fastcall	TfrmPriceCompare::Del_UnRegAppId_fromMap()
{
	for(map<APP_ID, TMDInfo*>::iterator it=m_mapGridIdx.begin(); it!=m_mapGridIdx.end();)
	{
		bool bNeedLock = false;
		if( _eaInfo.Is_Exist ((*it).first, bNeedLock)==true ) {
			++it;
			continue;
		}

		delete (*it).second->chk;
		delete (*it).second;

		//m_chartHandler->IF_UnRegBroker((*it).first);

		it = m_mapGridIdx.erase(it);
	}
}

//
void __fastcall TfrmPriceCompare::gdMD_EA_ReListUp()
{
	// delete AppId from map that doesn't exist in the _eaInfo
	Del_UnRegAppId_fromMap();


	gdMD->RowCount = (_eaInfo.Count()==0)?1:_eaInfo.Count()+1;

	_eaInfo.LoopBegin();
	for( int i=0; i<_eaInfo.Count() ; i++ )
	{
		CEAInfo info;
		bool bExist = _eaInfo.LoopEAInfo(info);
		if(bExist==false)
			break;

		int rowIdx = i + 1;
		if( rowIdx == gdMD->RowCount-1 )
			gdMD->RowCount = rowIdx+1;

		gdMD->Cells[GDMD_APPID     ][rowIdx] = String(info.m_sAppId.c_str());
		gdMD->Cells[GDMD_BROKER    ][rowIdx] = String(info.m_sBroker.c_str());

		TMDInfo* pMD = NULL;
		map<APP_ID, TMDInfo*>::iterator itFind = m_mapGridIdx.find(info.m_sAppId);
		if(itFind!=m_mapGridIdx.end())
			pMD = (*itFind).second;

		if(pMD==NULL) // new
		{
			gdMD->Cells[GDMD_SYMBOL    ][rowIdx] = String("Unsubscribe");
			gdMD->Cells[GDMD_MKT_TIME  ][rowIdx] = String("");
			gdMD->Cells[GDMD_LOCAL_TIME][rowIdx] = String("");
			gdMD->Cells[GDMD_BID       ][rowIdx] = String("");
			gdMD->Cells[GDMD_ASK       ][rowIdx] = String("");
			gdMD->Cells[GDMD_SPREAD    ][rowIdx] = String("");
		}
		else
		{
			gdMD->Cells[GDMD_SYMBOL    ][rowIdx] = (pMD->chk->Checked)? String(pMD->sSymbol.c_str()): String("Unsubscribe");
			gdMD->Cells[GDMD_MKT_TIME  ][rowIdx] = (pMD->chk->Checked)? String(pMD->sMktTime.c_str()): String("");
			gdMD->Cells[GDMD_LOCAL_TIME][rowIdx] = (pMD->chk->Checked)? String(pMD->sLocalTime.c_str()):String("");

			char bid[32]={0},ask[32]={0},spread[32]={0};
			if( pMD->chk->Checked )
			{
				if(pMD->dBid>0)	sprintf(bid, "%.*f", pMD->nDecimal, pMD->dBid);
				if(pMD->dAsk>0)	sprintf(ask, "%.*f", pMD->nDecimal, pMD->dAsk);
				if(pMD->dSpread>0)	sprintf(spread, "%.0f", pMD->dSpread);
			}
			gdMD->Cells[GDMD_BID       ][rowIdx] = String(bid);
			gdMD->Cells[GDMD_ASK       ][rowIdx] = String(ask);
			gdMD->Cells[GDMD_SPREAD    ][rowIdx] = String(spread);
		}

		MapGrid_Add_Idx_ChkBox(info.m_sAppId.c_str(), rowIdx, pMD);
	}
	_eaInfo.LoopEnd();

}

void __fastcall TfrmPriceCompare::gdMD_UpdatePrice(CProtoGet &get)
{
	//EnterCriticalSection(&m_csGridIdx);

	__try
	{
		if(cbSymbols->ItemIndex<0)
			return;

		string sAppId, sSymbol, sMktTime, sLocalTime, sBroker;
		double dBid, dAsk, dSpread;
		int nDecimal;

		try
		{
			CHECK_BOOL(get.GetVal(FDS_KEY, &sAppId), 				"No FDS_KEY");
			CHECK_BOOL(get.GetVal(FDS_SYMBOL, &sSymbol), 			"No FDS_SYMBOL");
			CHECK_BOOL(get.GetVal(FDS_BROKER, &sBroker), 			"No FDS_BROKER");
			CHECK_BOOL(get.GetVal(FDD_BID, &dBid ), 				"No FDD_BID");
			CHECK_BOOL(get.GetVal(FDD_ASK, &dAsk), 					"No FDD_ASK");
			CHECK_BOOL(get.GetVal(FDD_SPREAD, &dSpread), 			"No FDD_SPREAD");
			CHECK_BOOL(get.GetVal(FDS_MARKETDATA_TIME, &sMktTime), "No FDS_MARKETDATA_TIME");
			CHECK_BOOL(get.GetVal(FDN_DECIMAL, &nDecimal), 			"No FDN_DECIMAL");
		}
		catch(const char* e)
		{
			__log.log(ERR, "[MarketData]%s", e);
			frmMain->AddMsg(L"Market Data is wrong", MSGTP_ERR);
			return;
		}
		CTimeUtils time; sLocalTime = time.Time_hhmmssA().c_str();

		TMDInfo* pInfo;
		int rowIdx = 0;
		map<string, TMDInfo*>::iterator it = m_mapGridIdx.find(sAppId);
		if( it==m_mapGridIdx.end() )
		{
			return;
		}
		else
		{
			pInfo = (*it).second;
			rowIdx = pInfo->gridRowIdx;

			// update map info
			pInfo->sAppId 	= sAppId;
			pInfo->sBroker  = sBroker;
			pInfo->sSymbol	= sSymbol;
			pInfo->dBid		= dBid;
			pInfo->dAsk		= dAsk;
			pInfo->dSpread	= dSpread;
			pInfo->sMktTime	= sMktTime;
			pInfo->sLocalTime	= sLocalTime;
			pInfo->nDecimal		= nDecimal;

			m_mapGridIdx[(*it).first] = pInfo;

			//
			if( frmMain->Is_AlreadySubRequest() )
				m_chartHandler->IF_NewData(sAppId,sBroker,sSymbol,dBid,dAsk,sMktTime,sLocalTime, nDecimal );
			//
		}

		if( pInfo->chk->Checked )
		{
			gdMD->Cells[GDMD_APPID   	][rowIdx] = String(sAppId.c_str());
			gdMD->Cells[GDMD_BROKER  	][rowIdx] = String(sBroker.c_str());
			gdMD->Cells[GDMD_SYMBOL  	][rowIdx] = String(sSymbol.c_str());
			gdMD->Cells[GDMD_LOCAL_TIME	][rowIdx] = String(sLocalTime.c_str());
			gdMD->Cells[GDMD_MKT_TIME  	][rowIdx] = String(sMktTime.c_str());

			char z[32];
			sprintf(z, "%.*f", nDecimal, dBid); 	gdMD->Cells[GDMD_BID][rowIdx] = String(z);
			sprintf(z, "%.*f", nDecimal, dAsk); 	gdMD->Cells[GDMD_ASK][rowIdx] = String(z);
			sprintf(z, "%.0f", dSpread); 	gdMD->Cells[GDMD_SPREAD][rowIdx] = String(z);
		}
		else
		{
			//gdMD->Cells[GDMD_MKT_TIME  	][rowIdx] = L"Unsubscribe";
		}
	}
	__finally{
		//LeaveCriticalSection(&m_csGridIdx);
	}

}

void 	__fastcall TfrmPriceCompare::RequestSubUnsub(AnsiString sAppId, string sSubCmd)
{
	CBlock* pData = new CBlock();
	CProtoSet set;
	set.SetVal(FDS_CODE, 		   	__ALPHA::CODE_COMMAND_BY_CODE);
	set.SetVal(FDS_COMMAND_CODE,	sSubCmd);
	set.SetVal(FDS_SYMBOL, 			AnsiString(cbSymbols->Text.c_str()));
	set.SetVal(FDS_USER_ID,			_CommonInfo.m_UserId);
	set.SetVal(FDS_KEY,				sAppId.c_str());
	set.SetVal(FDS_FLOW_DIRECTION,	DIRECTION_TO_MGR);

	int len = set.Complete(pData->get());

	TfrmBasic::RequestSendData(pData);
}


void   	__fastcall	TfrmPriceCompare::LoadSymbols()
{
	cbSymbols->AddItem("EURUSD",NULL);
	cbSymbols->AddItem("EURGBP",NULL);
	cbSymbols->AddItem("EURJPY",NULL);
	cbSymbols->AddItem("GBPUSD",NULL);
	cbSymbols->AddItem("GBPJPY",NULL);
	cbSymbols->AddItem("AUDUSD",NULL);
	cbSymbols->AddItem("GBPJPY",NULL);
	cbSymbols->AddItem("NZDUSD",NULL);
	cbSymbols->AddItem("NZDJPY",NULL);
	cbSymbols->AddItem("USDJPY",NULL);
	cbSymbols->AddItem("USDCAD",NULL);
	cbSymbols->AddItem("USDCHF",NULL);

	cbSymbols->ItemIndex = -1;
}



void __fastcall TfrmPriceCompare::cbSymbolsChange(TObject *Sender)
{
//	edtSymbol->Text = cbSymbols->Text;
//
//	OnClick_CheckBoxOfGrid(Sender);
}
//---------------------------------------------------------------------------



void 	__fastcall TfrmPriceCompare::DrawChart()
{
   

}



void __fastcall TfrmPriceCompare::MapGrid_Clear()
{
	EnterCriticalSection(&m_csGridIdx);
	for(map<string, TMDInfo*>::iterator it=m_mapGridIdx.begin(); it!=m_mapGridIdx.end(); ++it)
	{
		delete (*it).second->chk;
		delete (*it).second;
	}
	m_mapGridIdx.clear();
	LeaveCriticalSection(&m_csGridIdx);
}

TMDInfo*  __fastcall TfrmPriceCompare::MapGrid_Idx(string sAppId, _Out_ int* idx)
{
	TMDInfo *pRes = NULL;

	map<string, TMDInfo*>::iterator it = m_mapGridIdx.find(sAppId);
	if( it==m_mapGridIdx.end() )
	{
		*idx = -1;
	}
	else{
		pRes = (*it).second;
		*idx = pRes->gridRowIdx;
	}

	return pRes;
}

void __fastcall TfrmPriceCompare::MapGrid_Add_Idx_ChkBox(string sAppId, int rowIdx, TMDInfo* pMDInfo)
{
	bool bCurrChkecked = false;


	bool bCreateNew = (pMDInfo==NULL);
	if(bCreateNew)
	{
		pMDInfo = new TMDInfo;
	}
	else
	{
		bCurrChkecked = pMDInfo->chk->Checked;
	}

	pMDInfo->gridRowIdx = rowIdx;

	static int _iLeft 	= 	gdMD->Left + 5;
	static int _iTopBase= 	gdMD->Top  + 4;
	static int _iHeight	= 16;
	static int _iWidth 	= 16;
	static int _iTopMargin = 4;

	if( !bCreateNew )
		delete pMDInfo->chk;

	pMDInfo->chk 			= new TCheckBox(this);
	pMDInfo->chk->Parent 	= this;
	pMDInfo->chk->Left 		= _iLeft;
	pMDInfo->chk->Height  	= _iHeight;
	pMDInfo->chk->Width		= _iWidth;
	pMDInfo->chk->Top  		= _iTopBase + (rowIdx*_iHeight) + (rowIdx*_iTopMargin);
	//pMDInfo->chk->OnClick	= OnClick_CheckBoxOfGrid;
	pMDInfo->chk->Checked	= bCurrChkecked;
	pMDInfo->chk->Tag		= ( bCreateNew )? false : (!bCurrChkecked);	// reverse for action of sub/unsub

	m_mapGridIdx[sAppId] = pMDInfo;

}

void __fastcall TfrmPriceCompare::FormClose(TObject *Sender, TCloseAction &Action)
{
	MapGrid_Clear();
	DeleteCriticalSection(&m_csGridIdx);
	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPriceCompare::gdMDSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect)
{
//	if( ACol!=GDMD_CHKBOX )
//		return;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPriceCompare::chkUseMktTimeClick(TObject *Sender)
{
	m_chartHandler->IF_ToggleMktTime(chkUseMktTime->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPriceCompare::btnSubsClick(TObject *Sender)
{
	bool bContinue = __MsgBox_Confirm(L"Cancel the current subscription and start new one?");
	if(!bContinue )
		return;


}
//---------------------------------------------------------------------------


void	__fastcall	TfrmPriceCompare::ReSubscribe()
{

	//EnterCriticalSection(&m_csGridIdx);

	__try
	{
		if( cbSymbols->ItemIndex < 0 )
		{
			__MsgBox_Err(L"Please select the symbol first");
			return;
		}

		for( map<string, TMDInfo*>::iterator it=m_mapGridIdx.begin(); it!=m_mapGridIdx.end(); ++it)
		{
			string sAppId 	= (*it).first.c_str();
			TMDInfo* pInfo 	= (*it).second;

			TCheckBox* pChk 	= pInfo->chk;

			if( pChk->Checked )
			{
				RequestSubUnsub(AnsiString(sAppId.c_str()),  CMD_MD_SUB);
				frmMain->Set_AlreadySubRequest();
			}
			else
			{
				RequestSubUnsub(AnsiString(sAppId.c_str()),  CMD_MD_UNSUB);

				gdMD->Cells[GDMD_SYMBOL    ][pInfo->gridRowIdx] = L"Unsubscribe";
				gdMD->Cells[GDMD_BID       ][pInfo->gridRowIdx] = L"";
				gdMD->Cells[GDMD_ASK       ][pInfo->gridRowIdx] = L"";
				gdMD->Cells[GDMD_SPREAD    ][pInfo->gridRowIdx] = L"";
				gdMD->Cells[GDMD_MKT_TIME  ][pInfo->gridRowIdx] = L"";
				gdMD->Cells[GDMD_LOCAL_TIME][pInfo->gridRowIdx] = L"";
			}
			m_mapGridIdx[sAppId] = pInfo;
		}

        m_chartHandler->IF_UnReg_AllBroker();
	}
	__finally{
		//LeaveCriticalSection(&m_csGridIdx);
	}
}
void __fastcall TfrmPriceCompare::btnPrevClick(TObject *Sender)
{
	bool bForward = false;
	//IF_PageMove(bForward);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPriceCompare::btnNextClick(TObject *Sender)
{
	bool bForward = true;
   //	IF_PageMove(bForward);
}
//---------------------------------------------------------------------------

