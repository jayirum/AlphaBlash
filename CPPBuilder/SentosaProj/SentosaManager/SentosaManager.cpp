//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("CLogonForm.cpp", frmLogon);
USEFORM("CBasicForm.cpp", frmBasic);
USEFORM("CDashBoardForm.cpp", frmDashBoard);
USEFORM("CMainForm.cpp", frmMain);
USEFORM("CPosOrdForm.cpp", frmPosOrd);
USEFORM("CPriceCompareForm.cpp", frmPriceCompare);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
