//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CMainForm.h"
#include "../../common/AlphaInc.h"
#include "../../common/CUtils.h"
#include "../../common/CTimeUtils.h"
#include "CLogonForm.h"
#include "CBasicform.h"
#include "CDashBoardForm.h"
#include "CPriceCompareform.h"
#include "CEAInfoManager.h"
#include "CPosOrdForm.h"
#include "uLocalCommon.h"
#include "../../common/clogmsg.h"


extern unsigned int	m_dwPosOrdThreadId;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	m_sockRecv = NULL;
	m_sockSend = NULL;

	m_hRecvThrd_RSock 	= NULL;
	m_hSendSockThrd 	= NULL;
	m_hLogonThrd		= NULL;

	m_dwRecvThrd_RSock 	= 0;
	m_dwSendSockThrd	= 0;
	m_dwLogonThrd		= 0;


	m_bThreadContinue = false;

	m_nLogonSockCnt = 0;

	m_bAlreadySub	= false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormShow(TObject *Sender)
{
	tmrLogonAuth->Interval = 500;
	tmrLogonAuth->Enabled  = true;

	char path[_MAX_PATH]={0};
	GetCurrentDirectoryA(_MAX_PATH, path);
	__log.OpenLog(path, "SentosaManager.exe");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrLogonAuthTimer(TObject *Sender)
{
	tmrLogonAuth->Enabled = false;

	////////////////////////////////////////////
	// Logon Form
	frmLogon = new TfrmLogon(Application);
	frmLogon->ShowModal();

	bool bCancelled = frmLogon->Is_Cancelled();
	delete frmLogon;
	/////////////////////////////////////////////////

	if(bCancelled){
		Close();
		return;
	}

	if( !_CommonInfo.Is_AuthDone() )
		return;

	m_bThreadContinue = true;

	Initialize();

	SendLogon_ToRelay(SOCKTP_RELAY_S);
	SendLogon_ToRelay(SOCKTP_RELAY_R);

	Request_EALogonInfo();
}

void  __fastcall TfrmMain::Initialize()
{
	m_sockRecv = new CTcpClient("R-Sock");
	m_sockSend = new CTcpClient("S-Sock");

	m_hRecvThrd_RSock 	= (HANDLE)_beginthreadex(NULL, 0, &Thread_Recv_RSock, this, 0, &m_dwRecvThrd_RSock);
	m_hSendSockThrd 	= (HANDLE)_beginthreadex(NULL, 0, &Thread_Recv_SSock, this, 0, &m_dwSendSockThrd);
	m_hMsgThrd 			= (HANDLE)_beginthreadex(NULL, 0, &Thread_ComboMsg, this, 0, &m_dwMsgThrd);
	m_hLogonThrd 		= (HANDLE)_beginthreadex(NULL, 0, &Thread_Logon, this, 0, &m_dwLogonThrd);
}

void __fastcall TfrmMain::DeInitialize()
{
	m_bThreadContinue = false;

	SAFE_DELETE( m_sockRecv );
	SAFE_DELETE( m_sockSend );
	SAFE_CLOSE( m_hRecvThrd_RSock );
	SAFE_CLOSE( m_hSendSockThrd );
	SAFE_CLOSE( m_hMsgThrd );
	SAFE_CLOSE( m_hLogonThrd );
}

bool __fastcall TfrmMain::SendLogon_ToRelay(AnsiString sSockTp)
{
	CTcpClient* sock;
	if( sSockTp==SOCKTP_RELAY_R )
	{
		sock = m_sockRecv;
    }
	else
	{
		sock = m_sockSend;
	}

	if( !sock->Init_Connect(_CommonInfo.m_RelaySvrIp.c_str(),
							_CommonInfo.m_RelaySvrPort.ToInt(),
							_CommonInfo.m_sSendTimeout.ToInt(),
							_CommonInfo.m_sRecvTimeout.ToInt()
							)
	  )
	{
		 AddMsg(L"Sock init error:"+sock->GetMsg(), MSGTP_ERR);
		 return false;
	}

	char zBuf[__ALPHA::LEN_BUF] = {0};
	char zMac[32]={0};
	CUtils util;
	CProtoSet set;

	set.Begin();
	set.SetVal(FDS_CODE,  			__ALPHA::CODE_LOGON);
	set.SetVal(FDN_APP_TP, 			APPTP_MANAGER);
	set.SetVal(FDS_BROKER, 			"BROKER");
	set.SetVal(FDS_ACCNO_MINE, 		"ACCNO");
	set.SetVal(FDS_USER_ID, 		_CommonInfo.m_UserId);
	set.SetVal(FDS_USER_PASSWORD, 	_CommonInfo.m_Pwd);

	set.SetVal(FDS_MAC_ADDR, util.GetMacAddr(zMac));
	set.SetVal(FDS_LIVEDEMO, "N");
	set.SetVal(FDS_KEY, _CommonInfo.m_AppId);
	set.SetVal(FDS_CLIENT_SOCKET_TP, sSockTp);


	int nBufLen = set.Complete(zBuf, false);
	int nErrCode = 0;
	if(sock->SendData(zBuf, nBufLen, &nErrCode)<0)
	{
		AddMsg("Failed to Send Logon Data:"+sock->GetMsg(), MSGTP_ERR);
		return false;
	}
	return true;
}

void  __fastcall TfrmMain::Load_DashBoard()
{
	tmrDashBoard->Interval 	= 500;
	tmrDashBoard->Enabled	= true;
}


unsigned WINAPI TfrmMain::Thread_Recv_RSock(LPVOID lp)
{
	TfrmMain* p = (TfrmMain*)lp;
	char zBuf[__ALPHA::LEN_BUF]={0};
	CProtoUtils protUtils;

	while (p->m_bThreadContinue)
	{
		Sleep(1);


		int nLen = 0;
		bool bContinue;
		do
		{
			if( !p->m_sockRecv->IsConnected())
			{
				//TODO
//				p->m_sockRecv->ReConnect();
//				continue;

				p->AddMsg (L"Connection to Server is not established. try to Reconnect...", MSGTP_ERR);
				__log.log(ERR,"Connection to Server is not established. try to Reconnect...");
				__MsgBox_Err(L"You lost connection to Server. Please re-logon later");
				p->tmrTerminate->Enabled = true;
				return 0;
			}

			ZeroMemory(zBuf, sizeof(zBuf));
			bContinue = p->m_sockRecv->GetOnePacket(&nLen, zBuf);
			if( nLen==0 )
				break;


			char zCode[32]={0};
			if(!protUtils.GetCode(zBuf, _Out_ zCode))
			{
				__log.log(ERR,"No PacketCode:%s", zBuf);
				p->AddMsg (L"Wrong Packet. Contact the Support", MSGTP_ERR);
				continue;
			}

			UINT uMessage = WM_RECEIVE_DATA;

			string sCode(zCode);

			if(sCode==__ALPHA::CODE_MESSAGE)
			{
				CBlock *pData = new CBlock();
				memcpy(pData->get(), zBuf, nLen);

				PostThreadMessage(p->m_dwMsgThrd, WM_MSG, (WPARAM)MSGTP_OUTER, (LPARAM)pData);
				continue;
            }

			if( sCode ==__ALPHA::CODE_COMMAND_BY_CODE )
			{
				char zCmd[32]={0};
				protUtils.GetCommandCode(zBuf, zCmd);
				if( strcmp(zCmd,CMD_NOTI_LOGONOUT)==0 )
				{
					p->Update_EAInfo_By_OnOff(zBuf);
				}
				uMessage = WM_EA_LOGONOFF;
			}
			if (sCode==__ALPHA::CODE_DUP_LOGON)
			{
				AnsiString sMsg;
				CProtoGet get; get.ParsingWithHeader(zBuf);
				get.GetVal(FDS_MSG, &sMsg);
				p->AddMsg(String(sMsg), MSGTP_ERR);
				__MsgBox_Warn(String(sMsg));

				p->tmrTerminate->Enabled = true;
				return 0;
			}
            //
			p->DeliverData_AllForms(sCode, zBuf, nLen, uMessage);
			//


			if( !p->Is_LogonCompleted() )
			{
				CBlock *pData = new CBlock();
				memcpy(pData->get(), zBuf, nLen);
				PostThreadMessage(p->m_dwLogonThrd, uMessage, (WPARAM)SOCKTP_RELAY_R, (LPARAM)pData);
			}

		}while(bContinue);
    }
	return 0;
}

void 	__fastcall TfrmMain::DeliverData_AllForms(string sCode, char* pzData, int nDataLen, UINT uMessage)
{
	DWORD id;

	_ThreadIds.LoopBegin();
	while( _ThreadIds.Get(id) )
	{
		CBlock* pData = new CBlock();
		memcpy(pData->get(), pzData, nDataLen);

		PostThreadMessage(id, uMessage, (WPARAM)nDataLen, (LPARAM)pData);
//		if(sCode==   __ALPHA::CODE_POSORD)
//		{
//			__log.log(INFO, "[1-0][POSORD](%s)\n", pzData);
//		}
	}
	_ThreadIds.LoopEnd();
}




bool __fastcall TfrmMain::Update_EAInfo_By_OnOff(_In_ char* pzRecvData)
{
	CProtoGet get;
	int nFieldCnt = get.ParsingWithHeader(pzRecvData);

	string sAppId;
	string sRegUnreg;
	bool res = true;
	try
	{
		CHECK_BOOL(get.GetVal(FDS_KEY, &sAppId), "No FDS_KEY");
		CHECK_BOOL(get.GetVal(FDS_REGUNREG, &sRegUnreg), "No FDS_REGUNREG");

		if(sRegUnreg.compare(DEF_UNREG)==0 )
		{
			_eaInfo.RemoveEA(sAppId, true);
		}
		else
		{
			string sBroker, sAcc, sIp, sMac, sLiveDemo, sTime;
			CHECK_BOOL(get.GetVal(FDS_BROKER, &sBroker), "No FDS_BROKER_NAME");
			CHECK_BOOL(get.GetVal(FDS_ACCNO_MINE, &sAcc), "No FDS_ACCNO_MINE");
			CHECK_BOOL(get.GetVal(FDS_CLIENT_IP, &sIp), "No FDS_CLIENT_IP");
			CHECK_BOOL(get.GetVal(FDS_MAC_ADDR, &sMac), "No FDS_MAC_ADDR");
			CHECK_BOOL(get.GetVal(FDS_LIVEDEMO, &sLiveDemo), "No FDS_LIVEDEMO");
			CHECK_BOOL(get.GetVal(FDS_TIME, &sTime), "No FDS_TIME");


			_eaInfo.AddEA(sAppId, sIp, sMac, sBroker, sAcc, sLiveDemo, sTime);

			//frmMain->Memo1->Lines[0].Add(L"ADD EA:"+String(sBroker.c_str()));
		}
	}
	catch(const char* e)
	{
		__log.log(ERR, "Exception:%s", e);
		frmMain->AddMsg(L"Exception happened", MSGTP_ERR);
		res = false;
	}
	return res;
}



unsigned WINAPI TfrmMain::Thread_Recv_SSock(LPVOID lp)
{
	TfrmMain* p = (TfrmMain*)lp;


	while (p->m_bThreadContinue)
	{
		Sleep(1);

		if( p->Is_LogonCompleted() )
		{
		}
		else
		{
			char zBuf[__ALPHA::LEN_BUF]={0};
			int nLen = 0;
			bool bContinue;
			do
			{
				bContinue = p->m_sockSend->GetOnePacket(&nLen, zBuf);
				if( nLen==0 )
					break;

				if( p->Is_LogonCompleted() )
				{
					// terminate thread
					return  0;
					//
				}

				CBlock *pData = new CBlock();
				memcpy(pData->get(), zBuf, nLen);
				PostThreadMessage(p->m_dwLogonThrd, WM_RECEIVE_DATA, (WPARAM)SOCKTP_RELAY_S, (LPARAM)pData);

			}while(bContinue);
		}
	}

	return 0;
}


unsigned WINAPI TfrmMain::Thread_Logon(LPVOID lp)
{
	TfrmMain* p = (TfrmMain*)lp;
	String sMsg;
	while (p->m_bThreadContinue)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			CBlock *pData 	= (CBlock*)msg.lParam;
			char* pSockTp   = (char*)msg.wParam;
			CProtoGet get;

			int nFieldCnt = get.ParsingWithHeader(pData->get());
			delete pData;

			if(nFieldCnt==0)
			{
				__log.log(ERR, "[Thread_Logon]Wrong Packet:%s", pData->get());
				p->AddMsg("Wrong Protocol.Contact suppport", MSGTP_ERR);

				continue;
			}

			AnsiString sCode;
			get.GetVal(FDS_CODE, &sCode);

			if( sCode ==__ALPHA::CODE_LOGON )
			{
				if(get.Is_Success()==false)
				{
					__log.log(ERR, "Received Logon Error(%s)", pData->get());
					p->AddMsg("Logon Failed", MSGTP_ERR);
					continue;
				}
				p->m_nLogonSockCnt++;
				if( p->Is_LogonCompleted() )
				{
					p->AddMsg(L"Log on Completed!", MSGTP_INFO);
					p->Load_DashBoard();
				}
			}
			else if (sCode==__ALPHA::CODE_DUP_LOGON)
			{
				AnsiString sMsg;
				get.GetVal(FDS_MSG, &sMsg);
				p->AddMsg(String(sMsg), MSGTP_ERR);

				frmMain->Close();
				return 0;
			}
		}//while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    }
	return 0;
}



void __fastcall TfrmMain::WndProc(TMessage& Message)
{
	if(Message.Msg == WM_REQUEST_SENDDATA )
	{
		SendData_To_Relay(Message);
	}
	else
	{
		TForm::WndProc(Message);
	}
}

void __fastcall TfrmMain::SendData_To_Relay(TMessage& Message)
{
	String sMsg;
	CBlock *pData 	= (CBlock*)Message.LParam;
	int err;
	m_sockSend->SendData(pData->get(), pData->size(), &err );
	__log.log(INFO, "SendData:%s",pData->get());

	delete pData;
}


unsigned WINAPI TfrmMain::Thread_ComboMsg(LPVOID lp)
{
	TfrmMain* p = (TfrmMain*)lp;
	CTimeUtils time;
	while (p->m_bThreadContinue)
	{
		Sleep(1);
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message!=WM_MSG)
				continue;

			String sMsg;
			bool bErr = false;
			if(MSGTP_OUTER == (int)msg.wParam)
			{
				CBlock* pBlock = (CBlock*)msg.lParam;
				CProtoGet get; get.ParsingWithHeader(pBlock->get());
				char zData[256]={0};
				char zSuccYN[32]={0};
				get.GetVal(FDS_NOTI_MSG, zData);
				get.GetVal(FDS_SUCC_YN, zSuccYN);
				bErr = (zSuccYN[0]=='Y')? false:true;

				sMsg = String().Format("[%s]%s %s",
							ARRAYOFCONST(( time.Time_hhmmssmmm(), (bErr)?"[ERROR]":"" , String(zData)))
							);
				delete pBlock;
			}
			else
			{
				if( MSGTP_ERR ==  (int)msg.wParam)
					bErr = true;
				String *sData = (String*)msg.lParam;

				sMsg = String().Format("[%s]%s %s",
							ARRAYOFCONST(( time.Time_hhmmssmmm(), (bErr)?"[ERROR]":"" , *sData))
							);

				delete (String*)msg.lParam;
			}

			if(bErr){
				p->cbMsg->Color = clRed;
			}
			else{
				p->cbMsg->Color = 0x00FFFF9B;
			}

			p->cbMsg->Items->Insert(0, sMsg);
			p->cbMsg->ItemIndex = 0;
			p->tmrMsgRed->Enabled = true;

			__log.log(INFO, AnsiString(sMsg).c_str());
		}
	}
	return 0;
}

void TfrmMain::AddMsg(String s, EN_MSG_TP msgTp)
{
	String *msg = new String(s);
	PostThreadMessage(m_dwMsgThrd, WM_MSG, (WPARAM)msgTp, (LPARAM)msg);
}
void __fastcall TfrmMain::btnDashBoardClick(TObject *Sender)
{
	frmDashBoard = (TfrmDashBoard*)__Create_ChildForm(__classid(TfrmDashBoard), L"TfrmDashBoard", frmDashBoard, FORM_MDI_MAX);

}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrDashBoardTimer(TObject *Sender)
{
	tmrDashBoard->Enabled = false;
	btnDashBoardClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::itemCloseClick(TObject *Sender)
{
	frmMain->Close();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::Request_EALogonInfo()
{
	CBlock* pData = new CBlock();
	CProtoSet set;
	set.SetVal(FDS_CODE, __ALPHA::CODE_COMMAND_BY_CODE);
	set.SetVal(FDS_COMMAND_CODE, CMD_NOTI_LOGONOUT);
	set.SetVal(FDS_USER_ID, _CommonInfo.m_UserId);
	int len = set.Complete(pData->get());

	DWORD dwRslt;
	SendMessageTimeout(frmMain->Handle,
									WM_REQUEST_SENDDATA,
									(WPARAM)pData->size(),
									(LPARAM)pData,
									SMTO_ABORTIFHUNG,
									TIMEOUT_SENDMSG,
									&dwRslt
									);
}
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	m_bThreadContinue = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnPosOrdClick(TObject *Sender)
{
	frmPosOrd = (TfrmPosOrd*)__Create_ChildForm(__classid(TfrmPosOrd), L"TfrmPosOrd", frmPosOrd, FORM_MDI_MAX);
}


void __fastcall TfrmMain::btnPriceCompareClick(TObject *Sender)
{
	frmPriceCompare = (TfrmPriceCompare*)__Create_ChildForm(__classid(TfrmPriceCompare), L"TfrmPriceCompare", frmPriceCompare, FORM_MDI_MAX);
}
void __fastcall TfrmMain::tmrMsgRedTimer(TObject *Sender)
{
	tmrMsgRed->Enabled = false;
	cbMsg->Color		= clWhite;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnCloseAppClick(TObject *Sender)
{
	frmMain->Close() ;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrTerminateTimer(TObject *Sender)
{
	frmMain->Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnLogonClick(TObject *Sender)
{
	tmrLogonAuthTimer(Sender);
}
//---------------------------------------------------------------------------

