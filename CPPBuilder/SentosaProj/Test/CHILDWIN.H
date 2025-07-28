//----------------------------------------------------------------------------
#ifndef ChildWinH
#define ChildWinH
//----------------------------------------------------------------------------
#include <Controls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <Classes.hpp>
#include <Windows.hpp>
#include <System.hpp>
#include <StdCtrls.hpp>

#define WM_MY_MSG WM_USER + 1212

//----------------------------------------------------------------------------
class TMDIChild : public TForm
{
__published:
	TMemo *Memo1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:
public:
	virtual __fastcall TMDIChild(TComponent *Owner);
	void __fastcall WndProc(TMessage& Message);
};
//----------------------------------------------------------------------------
#endif	
