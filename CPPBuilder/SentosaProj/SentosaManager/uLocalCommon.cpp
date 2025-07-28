//---------------------------------------------------------------------------

#pragma hdrstop

#include <Vcl.Forms.hpp>
#include "uLocalCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

CCommonInfo _CommonInfo;
CThreadId	_ThreadIds;

void CCommonInfo::Initialize()
{
	m_UserId 		= "";
	m_Pwd 			= "";
	m_RelaySvrIp 	= "";
	m_RelaySvrPort 	= "";
	m_DataSvrIp 	= "";
	m_DataSvrPort 	= "";
	m_AppId 		= "";
	m_bAuthSuccess 	= false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool __Grid_Search(TAdvStringGrid* grid, String sData, int nColIdx, _Out_ int *pnFound)
{
	*pnFound = -1;

	int nCurrRowCnt = grid->RowCount;

	for( int k = 1; k<nCurrRowCnt; k++ )
	{
		if( grid->Cells[nColIdx][k]==sData )
		{
			*pnFound = k;
			break;
		}
	}
	return (*pnFound>-1);
}

void __Grid_Clear(TAdvStringGrid* grid)
{
	grid->RowCount = 2;
	grid->Rows[1]->Clear();
}

bool __Grid_IsEmpty(TAdvStringGrid* grid)
{
	if( grid->RowCount > 2 )
		return false;

	return (grid->Cells[0][1].Length()==0 );
}

void __Grid_DelRow(TAdvStringGrid* grid, int nDeletingIdx)
{
	if( nDeletingIdx >= grid->RowCount )
		return;

	for( int i=nDeletingIdx; i<grid->RowCount-1; i++ )
	{
		if( grid->Cells[i+1][0].Length()>0)
			grid->Rows[i]->Assign(grid->Rows[i+1]);
	}

	if( (grid->RowCount==2) &&  ( grid->Cells[1][1].Length()>0) )
	{
		grid->Rows[1]->Clear();
	}
	else
	{
		grid->RowCount -= 1;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void __MsgBox_Err(String sMsg)
{
	Application->MessageBox(sMsg.c_str(), L"[SentosaManager]Error!!!",MB_OK | MB_ICONERROR);
}
void __MsgBox_Warn(String sMsg)
{
	Application->MessageBox(sMsg.c_str(), L"[SentosaManager]Warning",MB_OK | MB_ICONWARNING);
}

bool __MsgBox_Confirm(String sMsg)
{
	 int rslt = Application->MessageBox(sMsg.c_str(), L"[SentosaManager]Confirm",MB_OKCANCEL | MB_ICONQUESTION);
	 return (rslt==IDOK);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CThreadId::Add(DWORD id)
{
	EnterCriticalSection(&m_cs);
	m_set.insert(id);
	LeaveCriticalSection(&m_cs);
}
void CThreadId::Erase(DWORD id)
{
	EnterCriticalSection(&m_cs);
	m_set.erase(id);
	LeaveCriticalSection(&m_cs);
}
void CThreadId::LoopBegin()
{
	EnterCriticalSection(&m_cs);
}
bool CThreadId::Get(_Out_ DWORD& id)
{
	if( m_set.empty() )
		return false;

	bool bExist = false;
	if(	m_bLoopStart == false)
	{
		m_it = m_set.begin();
		m_bLoopStart = true;
		id = (*m_it);
		bExist = true;
	}
	else
	{
		int away = distance(m_it, m_set.end() );
		if( away>1 )
		{
			m_it++;
			id = (*m_it);
			bExist = true;
		}
		else
		{
			bExist = false;
		}
	}
	return bExist;
}

void CThreadId::LoopEnd()
{
	m_bLoopStart = false;
	LeaveCriticalSection(&m_cs);
}

