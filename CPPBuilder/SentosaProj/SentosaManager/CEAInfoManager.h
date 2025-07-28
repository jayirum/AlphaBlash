//---------------------------------------------------------------------------

#ifndef CEAInfoManagerH
#define CEAInfoManagerH
//---------------------------------------------------------------------------

#include <System.hpp>
#include "../../common/AlphaInc.h"
#include <string>
#include <map>
using namespace std;


class CEAInfo
{
public:
	CEAInfo(string sAppId, string sAppIp, string sAppMac, string sBroker, string sAccNo,
			string sLiveDemo, string sLogonMktTime, string sLogonLocalTime)
	{
		m_sAppId 	= sAppId;
		m_sAppIp 	= sAppIp;
		m_sAppMac	= sAppMac;
		m_sBroker	= sBroker;
		m_sAccNo	= sAccNo;
		m_sLiveDemo	= sLiveDemo;
		m_sLogonMktTime = sLogonMktTime;
		m_sLogonLocalTime =  sLogonLocalTime;
	}
	CEAInfo(){}
	CEAInfo(CEAInfo& ref){
		m_sAppId 	= ref.m_sAppId;
		m_sAppIp 	= ref.m_sAppIp;
		m_sAppMac	= ref.m_sAppMac;
		m_sBroker	= ref.m_sBroker;
		m_sAccNo	= ref.m_sAccNo;
		m_sLiveDemo	= ref.m_sLiveDemo;
		m_sLogonMktTime = ref.m_sLogonMktTime;
		m_sLogonLocalTime = ref.m_sLogonLocalTime;
	}
	CEAInfo& operator=(CEAInfo &ref){
		m_sAppId 	= ref.m_sAppId;
		m_sAppIp 	= ref.m_sAppIp;
		m_sAppMac	= ref.m_sAppMac;
		m_sBroker	= ref.m_sBroker;
		m_sAccNo	= ref.m_sAccNo;
		m_sLiveDemo	= ref.m_sLiveDemo;
		m_sLogonMktTime = ref.m_sLogonMktTime;
		m_sLogonLocalTime = ref.m_sLogonLocalTime;
	}
	~CEAInfo(){};

public:
	string	m_sAppId;
	string 	m_sAppIp;
	string 	m_sAppMac;
	string 	m_sBroker;
	string 	m_sAccNo;
	string	m_sLiveDemo;
	string	m_sLogonMktTime;
	string 	m_sLogonLocalTime;
};


typedef map<string, CEAInfo*>			MAP_EAINFO;   	// AppId
typedef map<string, CEAInfo*>::iterator	IT_MAP_EAINFO;



class CEAInfoManager
{
public:
	CEAInfoManager();
	~CEAInfoManager();

	void	AddEA(string sAppId, string sAppIp, string sAppMac, string sBroker,
						string sAccNo, string sLiveDemo, string sLogonMktTime);
	void 	RemoveEA(string sAppId, bool bLock);
	bool	GetEAInfo(string sAppId, _Out_ CEAInfo& info);
	int 	Count() { return m_map.size(); }

	void	LoopStart();
	void	LoopBegin() { LoopStart(); }
	bool	LoopEAInfo(_Out_ CEAInfo& info);
	void 	LoopEnd();
	void ReadKeyAll(char* msg);

	bool	Is_Exist(string sAppId, bool  bLock);
private:
	void Clear();
private:

	MAP_EAINFO			m_map;
	CRITICAL_SECTION	m_cs;

	bool				m_bLoopStarted;
	string				m_sLoopKey;
};

extern CEAInfoManager	_eaInfo;


#endif
