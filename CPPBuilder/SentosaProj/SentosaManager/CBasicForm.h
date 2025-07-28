//---------------------------------------------------------------------------

#ifndef CBasicFormH
#define CBasicFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "uLocalCommon.h"
#include <Vcl.ExtCtrls.hpp>

#include "CMainForm.h"


//---------------------------------------------------------------------------
class TfrmBasic : public TForm
{
__published:	// IDE-managed Components
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmBasic(TComponent* Owner);

protected:

	void	RequestSendData(CBlock* pData);

public:
	void 	ShowForm(EN_FORM_MODE enFormMode);

	inline void StopThread()
	{
		if(!m_hThread || !m_hDie )	return;

		SetEvent(m_hDie);
		if (WaitForSingleObject(m_hThread, 3000) != WAIT_OBJECT_0){
			DWORD dwExitCode = 0;
			TerminateThread(m_hThread, dwExitCode);
		}
		CloseHandle(m_hDie);
		CloseHandle(m_hThread);
		m_hDie = m_hThread = NULL;
	};

	inline unsigned int	MyThreadId()
	{
		return m_dwThreadID;
	};

protected:
	virtual void Exec()=0;

	static unsigned WINAPI ThreadProc(LPVOID lp)
	{
		TfrmBasic* p = (TfrmBasic*)lp;
		p->Exec();
		return 0;
	};

	inline void ResumeThread()
	{
		::ResumeThread(m_hThread);
	};


	inline bool	Is_TimeOfStop(int nTime=10)
	{
		return ( WaitForSingleObject(m_hDie, nTime)==WAIT_OBJECT_0);
	};

	inline bool	Is_OnGoing(int nTime=10)
	{
		return (!Is_TimeOfStop(nTime));
	};
protected:
	HANDLE			m_hThread;
	HANDLE			m_hDie;
	unsigned int	m_dwThreadID;


};
//---------------------------------------------------------------------------
extern PACKAGE TfrmBasic *frmBasic;
//---------------------------------------------------------------------------

void* __Create_ChildForm(TComponentClass formClass, String sClassName, void* pFormInstance,  EN_FORM_MODE enFormMode);







#endif
