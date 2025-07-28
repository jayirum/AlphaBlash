


//---------------------------------------------------------------------------

#pragma hdrstop

#include "CEAInfoManager.h"
#include <System.hpp>
#include "../../common/CTimeUtils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


CEAInfoManager	_eaInfo;


CEAInfoManager::CEAInfoManager()
{
	m_bLoopStarted = false;
	InitializeCriticalSection(&m_cs);
}

CEAInfoManager::~CEAInfoManager()
{
	Clear();

	DeleteCriticalSection(&m_cs);
}


void CEAInfoManager::ReadKeyAll(char* msg)
{
	EnterCriticalSection(&m_cs);
	for( IT_MAP_EAINFO it=m_map.begin(); it!=m_map.end(); ++it )
	{
		char s[128];
		sprintf(s, ", key=%s", ((*it).first).c_str());
		strcat(msg,s);
	}
	LeaveCriticalSection(&m_cs);
}

void CEAInfoManager::AddEA(string sAppId, string sAppIp, string sAppMac, string sBroker,
	string sAccNo, string sLiveDemo, string sLogonMktTime)
{
	EnterCriticalSection(&m_cs);
	__try
	{
		RemoveEA(sAppId, false);

		CTimeUtils util;
		AnsiString sLocal = util.DateTime_yyyy_mm_dd__hh_mm_ssA();
		CEAInfo* p = new  CEAInfo(sAppId, sAppIp, sAppMac, sBroker, sAccNo, sLiveDemo, sLogonMktTime, sLocal.c_str());
		m_map[sAppId] = p;
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}
}
void CEAInfoManager::RemoveEA(string sAppId, bool bLock)
{
	if(bLock) EnterCriticalSection(&m_cs);
	__try
	{
		CEAInfo* p = NULL;
		IT_MAP_EAINFO it = m_map.find(sAppId);
		if( it!=m_map.end() ){
			p = (*it).second;
			delete p;
			m_map.erase(it);
		}
	}
	__finally
	{
		if(bLock) LeaveCriticalSection(&m_cs);
	}
}


void CEAInfoManager::Clear()
{
	EnterCriticalSection(&m_cs);

	for( IT_MAP_EAINFO it=m_map.begin(); it!=m_map.end(); it++)
	{
		delete (*it).second;
	}
	m_map.clear();
	LeaveCriticalSection(&m_cs);
}

// return true : exist, else no exist
bool CEAInfoManager::GetEAInfo(string sAppId, _Out_ CEAInfo& info)
{
	EnterCriticalSection(&m_cs);
	__try
	{
		IT_MAP_EAINFO it = m_map.find(sAppId);
		if(it==m_map.end())
			return false;

		info = *(*it).second;
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
    }
	return true;
}


bool CEAInfoManager::Is_Exist(string sAppId, bool  bLock)
{
	bool bExist = true;
	if(bLock) EnterCriticalSection(&m_cs);
	IT_MAP_EAINFO it = m_map.find(sAppId);
	if(it==m_map.end())
		bExist = false;
	if(bLock) LeaveCriticalSection(&m_cs);

	return bExist;
}

void CEAInfoManager::LoopStart()
{
	EnterCriticalSection(&m_cs);
	m_sLoopKey = "";
	m_bLoopStarted = true;
}


void CEAInfoManager::LoopEnd()
{
	LeaveCriticalSection(&m_cs);
	m_bLoopStarted = false;
}


//
//// return true : infos exist
//// bNext : whether next data exists
bool CEAInfoManager::LoopEAInfo(_Out_ CEAInfo& info)
{
	IT_MAP_EAINFO it;
	if( m_sLoopKey.empty() )
	{
		it = m_map.begin();
	}
	else
	{
		it = m_map.find(m_sLoopKey);
		int away = distance(it, m_map.end() );
		if( away==1 )
			return false;

		++it;
	}

	m_sLoopKey = (*it).first;
	info = *(*it).second;
	return true;
}

