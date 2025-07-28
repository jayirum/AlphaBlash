//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "About.h"
#include "CLogMsg.h"
#include "CUtils.h"
#include <System.StrUtils.hpp>

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent *Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CreateMDIChild(String Name)
{
	TMDIChild *Child;

	//--- create a new MDI child window ----
	Child = new TMDIChild(Application);
	Child->Caption = Name;
	if (FileExists (Name))
		Child->Memo1->Lines->LoadFromFile(Name);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileNew1Execute(TObject *Sender)
{
	CreateMDIChild("NONAME" + IntToStr(MDIChildCount + 1));
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileOpen1Execute(TObject *Sender)
{
	if (OpenDialog->Execute())
		CreateMDIChild(OpenDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::HelpAbout1Execute(TObject *Sender)
{
	AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileExit1Execute(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------


void SplitStringTest()
{
	AnsiString s = "123;456;789;";
	TStringDynArray arr = SplitString(s, L";");

	MainForm->Memo1->Lines[0].Add(L"-------[123;456;789;]-------");
	for( int i=0; i<arr.Length; i++ )
	{
		MainForm->Memo1->Lines[0].Add( arr[i]);
	}

	s = "123;456;789";
	TStringDynArray arr2 = SplitString(s, L";");

	MainForm->Memo1->Lines[0].Add(L"-------[123;456;789]-------");
	for( int i=0; i<arr2.Length; i++ )
	{
		MainForm->Memo1->Lines[0].Add( arr2[i]);
	}

	s = "123;456;789;*";
	TStringDynArray arr3 = SplitString(s, L";");

	MainForm->Memo1->Lines[0].Add(L"-------[123;456;789;*]-------");
	for( int i=0; i<arr3.Length; i++ )
	{
		MainForm->Memo1->Lines[0].Add( arr3[i]);
	}

}


#define DELI  0x3B
void SplitStringTest2()
{
	AnsiString ansi = AnsiChar(65);

	String sDeli = StringOfChar(AnsiChar(DELI),1);

		AnsiString s = "123;456;789;";
	TStringDynArray arr = SplitString(s,sDeli);

	MainForm->Memo1->Lines[0].Add(L"-------[123;456;789;]-------");
	for( int i=0; i<arr.Length; i++ )
	{
		MainForm->Memo1->Lines[0].Add( arr[i]);
	}

	s = "123;456;789";
	TStringDynArray arr2 = SplitString(s, sDeli);

	MainForm->Memo1->Lines[0].Add(L"-------[123;456;789]-------");
	for( int i=0; i<arr2.Length; i++ )
	{
		MainForm->Memo1->Lines[0].Add( arr2[i]);
	}

	s = "123;456;789;*";
	TStringDynArray arr3 = SplitString(s, sDeli);

	MainForm->Memo1->Lines[0].Add(L"-------[123;456;789;*]-------");
	for( int i=0; i<arr3.Length; i++ )
	{
		MainForm->Memo1->Lines[0].Add( arr3[i]);
	}



}

void __fastcall TMainForm::Button1Click(TObject *Sender)
{

	SplitStringTest2();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button2Click(TObject *Sender)
{
	MainForm->Memo1->Clear() ;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
	gdEAInfo->ColCount 	= 6;
	gdEAInfo->RowCount	= 2;
	gdEAInfo->FixedCols	= 0;
	gdEAInfo->FixedRows	= 1;
	gdEAInfo->Cells[0][0] 		= "EA ID";
	gdEAInfo->Cells[1][0] 	= "Broker";
	gdEAInfo->Cells[2][0] 		= "AccNo";
	gdEAInfo->Cells[3][0]	= "Live/Demo";
	gdEAInfo->Cells[4][0] 		= "On/Off";
	gdEAInfo->Cells[5][0] 		= "IP Addr";
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::btnSendMessageClick(TObject *Sender)
{
	SendMessage(Application->MainForm->MDIChildren[0]->Handle, WM_MY_MSG, (WPARAM)0, (LPARAM)0);
}
//---------------------------------------------------------------------------

