//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CLogonForm.h"
#include "../../Common/CUtils.h"
#include "../../Common/AlphaProtocol.h"
#include "CMainForm.h"
#include <Vcl.Dialogs.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"



TfrmLogon *frmLogon;
//---------------------------------------------------------------------------
__fastcall TfrmLogon::TfrmLogon(TComponent* Owner)
	: TForm(Owner)
{
	m_bThreadContinue 	= false;
	m_hThreadRecv		= NULL;
	m_dwThreadID		= 0;
	m_sockAuth			= NULL;
	m_bCancel			= false;
}
//---------------------------------------------------------------------------



void __fastcall TfrmLogon::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmLogon::FormCreate(TObject *Sender)
{
	_CommonInfo.Initialize();

	m_bThreadContinue = true;
	m_hThreadRecv = (HANDLE)_beginthreadex(NULL, 0, &Thread_Recv, this, CREATE_SUSPENDED, &m_dwThreadID);
	
}

bool __fastcall TfrmLogon::Initialize()
{
	CIniFile ini;
	AnsiString sIniFileName 	= ini.GetCnfgFileName();
	_CommonInfo.m_AuthSvrIp 	= ini.GetVal("SERVER_INFO", "LOGON_AUTH_IP");
	_CommonInfo.m_AuthSvrPort 	= ini.GetVal("SERVER_INFO", "LOGON_AUTH_PORT");
	_CommonInfo.m_sSendTimeout 	= ini.GetVal("SERVER_INFO", "SEND_TIMEOUT");
	_CommonInfo.m_sRecvTimeout 	= ini.GetVal("SERVER_INFO", "RECV_TIMEOUT");

	if (
		(sIniFileName.Length()==0) ||
		(_CommonInfo.m_AuthSvrIp.Length()==0) ||
		(_CommonInfo.m_AuthSvrPort.Length()==0)
		)
	{
		pnlMsg->Caption = "Wrong Auth Server IP/Port. Check Ini file";
		return false;
	}

	if(m_sockAuth)
	{
		delete m_sockAuth;
	}

	m_sockAuth = new CTcpClient("AUTH");
	if( !m_sockAuth->Init_Connect(_CommonInfo.m_AuthSvrIp.c_str(), _CommonInfo.m_AuthSvrPort.ToInt()
						,_CommonInfo.m_sSendTimeout.ToInt(), _CommonInfo.m_sRecvTimeout.ToInt() )
	)
	{
		pnlMsg->Caption = "Socket Init error:" + m_sockAuth->GetMsg();
		return false;
	}

	ResumeRecvProcThread();

	return true;
}

bool 	__fastcall TfrmLogon::Connect()
{
	bool bRes = m_sockAuth->Connect();
	if(!bRes )
	{
		pnlMsg->Caption = "Connect error:"+m_sockAuth->GetMsg() ;
	}
	return bRes;
}

void __fastcall TfrmLogon::DeInitialize()
{
	m_bThreadContinue = false;
	if(m_hThreadRecv) CloseHandle(m_hThreadRecv);
	if(m_sockAuth)	delete m_sockAuth;
}


unsigned WINAPI TfrmLogon::Thread_Recv(LPVOID lp)
{
	char zBuf[__ALPHA::LEN_BUF]={0};

	TfrmLogon* p = (TfrmLogon*)lp;

	while(p->m_bThreadContinue)
	{
		Sleep(1);

		ZeroMemory(zBuf, sizeof(zBuf));
		int nLen = 0;
		bool bContinue;
		do
		{
			if( !p->m_sockAuth->IsConnected())
			{
				p->pnlMsg->Caption = "Connection to Server is not established. try to Reconnect...";
				p->m_sockAuth->ReConnect();
				continue;
			}
			bContinue = p->m_sockAuth->GetOnePacket(&nLen, zBuf);
			if( nLen==0 )
				break;

			p->pnlMsg->Caption = "[1]Get one packet";
			CProtoGet get;
			int nFieldCnt = get.ParsingWithHeader(zBuf);
			if(nFieldCnt==0)
			{
				p->pnlMsg->Caption = "Wrong Packet:"+String(zBuf);
				break;
			}

			p->pnlMsg->Caption = "[2]Parsing";
			if(get.Is_Success()==false)
			{
				AnsiString sMsg; get.GetVal(FDS_MSG, &sMsg);
				p->pnlMsg->Caption = "Get Failure:"+String(sMsg);
				break;
			}

			try
			{
				p->pnlMsg->Caption = "[3]before get value";

				CHECK_BOOL(get.GetVal(FDS_RELAY_IP, &_CommonInfo.m_RelaySvrIp), "Get RELAY IP error");
				CHECK_BOOL(get.GetVal(FDS_RELAY_PORT, &_CommonInfo.m_RelaySvrPort), "Get RELAY PORT error");
				CHECK_BOOL(get.GetVal(FDS_DATASVR_IP, &_CommonInfo.m_DataSvrIp), "Get FDS_DATASVR IP error");
				CHECK_BOOL(get.GetVal(FDS_DATASVR_PORT, &_CommonInfo.m_DataSvrPort), "Get FDS_DATASVR PORT error");

				_CommonInfo.m_UserId 	= AnsiString(p->edtUserID->Text).UpperCase();
				_CommonInfo.m_Pwd		= AnsiString(p->edtPwd->Text).UpperCase();
				_CommonInfo.m_bAuthSuccess = true;

				AnsiString sVersion;
				CHECK_BOOL(get.GetVal(FDS_VERSION, &sVersion), "Get FDS_VERSION  error");
				if( sVersion != __VERSION )
				{
					sprintf(p->m_zMsg, "Version is not matched(Current:%s)(ShouldBe:%s)", __VERSION, sVersion.c_str() );
					__MsgBox_Err(p->m_zMsg);
					return 0;
                }

				frmMain->AddMsg(L"Succeeded in Authenticating", MSGTP_INFO) ;

				p->pnlMsg->Caption = "[4]before close";


				// Close Logon Form
				//p->Close() ;
				p->tmrClose->Enabled = true;
				return 0;
			}
			catch(const char* e)
			{
				p->pnlMsg->Caption = e;
				break;
			}
		}while(bContinue);

	}

	return 0;
}
void __fastcall TfrmLogon::btnLogonClick(TObject *Sender)
{
	if( !Initialize() )
		return;

	SendLogAuthData();
}


void 	__fastcall TfrmLogon::SendLogAuthData()
{
	if( m_sockAuth->IsConnected()==false )
		return;

	CProtoSet set;
	set.Begin();
	set.SetVal(FDS_CODE,  			__ALPHA::CODE_LOGON_AUTH);
	set.SetVal(FDN_APP_TP, 			APPTP_MANAGER);
	set.SetVal(FDS_BROKER, 			"BROKER");
	set.SetVal(FDS_ACCNO_MINE, 		"ACCNO");
	set.SetVal(FDS_USER_ID, 		AnsiString(edtUserID->Text));
	set.SetVal(FDS_USER_PASSWORD,	AnsiString(edtPwd->Text));

	_CommonInfo.ComposeAppId(AnsiString(edtUserID->Text));
	set.SetVal(FDS_KEY, _CommonInfo.m_AppId);

	char zMac[32]={0};
	CUtils util;
	set.SetVal(FDS_MAC_ADDR, util.GetMacAddr(zMac));
	set.SetVal(FDS_LIVEDEMO, "N");

	char zBuf[__ALPHA::LEN_BUF] = {0};
	int nBufLen = set.Complete(zBuf, false);

	int nErrCode = 0;
	if(m_sockAuth->SendData(zBuf, nBufLen, &nErrCode)<0)
	{
		pnlMsg->Caption = "Send Error:" + m_sockAuth->GetMsg();
		return;
	}
	pnlMsg->Caption = "Send Auth OK";
}


//---------------------------------------------------------------------------

void __fastcall TfrmLogon::btnCancelClick(TObject *Sender)
{
	int result = Application->MessageBox(L"Do you want to close SentosaManager?", L"Confirmation",MB_YESNO);
	if( result==IDYES) {
		m_bCancel = true;
		Close();
	}

}
//---------------------------------------------------------------------------

void __fastcall TfrmLogon::Timer1Timer(TObject *Sender)
{
	Timer1->Enabled = false;

	edtPwd->Text = L"1111";

	btnLogonClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmLogon::tmrCloseTimer(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

