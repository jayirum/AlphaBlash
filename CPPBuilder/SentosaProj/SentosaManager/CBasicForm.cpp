//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CBasicForm.h"
#include "../../common/AlphaInc.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmBasic *frmBasic;


void* __Create_ChildForm(TComponentClass  formClass, String sClassName, void* pFormInstance, EN_FORM_MODE enFormMode)
{
	bool bReUse = false;
	int i;
	for(i=0; i<Application->MainForm->MDIChildCount; i++)
	{
		if (Application->MainForm->MDIChildren[i]->ClassName() == sClassName)
		{
			Application->MainForm->MDIChildren[i]->Show();
			bReUse = true;
			break;
		}
	}
	if( bReUse )
		return (void*)Application->MainForm->MDIChildren[i];

	Application->CreateForm(formClass, &pFormInstance);
	((TfrmBasic*)pFormInstance)->ShowForm(enFormMode);


//	for(int i=0; i<Application->MainForm->MDIChildCount; i++)
//	{
//		frmMain->Memo1->Lines[0].Add(L"classname:"+Application->MainForm->MDIChildren[i]->ClassName());
//	}

	return pFormInstance;
}


//---------------------------------------------------------------------------
__fastcall TfrmBasic::TfrmBasic(TComponent* Owner)
	: TForm(Owner)
{
	m_dwThreadID = 0;
	m_hDie = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &ThreadProc, this, CREATE_SUSPENDED, &m_dwThreadID);
}


void TfrmBasic::ShowForm(EN_FORM_MODE enFormMode)
{
	switch(enFormMode)
	{
		case FORM_MODAL :
		{
			Visible     = False;
			FormStyle   = fsNormal;
			Position    = poMainFormCenter;
			BorderIcons << biSystemMenu;
			WindowState = wsNormal;
			ShowModal();
			break;
		}
		case FORM_MDI :
		{
			FormStyle   = fsMDIChild;
			WindowState = wsNormal;
			Visible     = True;
			break;
		}
		case FORM_MDI_MAX :
		{
			FormStyle   = fsMDIChild;
			WindowState = wsMaximized;
			Visible     = True;
		}
	}


}

void TfrmBasic::RequestSendData(CBlock* pData)
{
	//frmMain->SendData(pData);
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

void __fastcall TfrmBasic::FormClose(TObject *Sender, TCloseAction &Action)
{
	m_dwThreadID = 0;
	Action = caFree;
}
//---------------------------------------------------------------------------

void __fastcall TfrmBasic::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	StopThread();
}




