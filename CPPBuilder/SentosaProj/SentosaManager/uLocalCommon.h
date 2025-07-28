//---------------------------------------------------------------------------

#ifndef uLocalCommonH
#define uLocalCommonH
//---------------------------------------------------------------------------


#define __VERSION	"1.0"

#include <System.hpp>
#include "AdvGrid.hpp"
#include "AdvObj.hpp"
#include "AdvUtil.hpp"
#include "../../common/AlphaInc.h"
#include <process.h>
#include <set>
using namespace std;


enum EN_FORM_MODE  {FORM_MODAL=0, FORM_MDI, FORM_MDI_MAX};

enum EN_Q  { Q_RECV, Q_SEND };

enum EN_APP_TP { APPTP_EA, APPTP_MANAGER };

enum EN_MSG_TP { MSGTP_INFO, MSGTP_ERR, MSGTP_OUTER=9999 };


#define SOCKTP_RELAY_R	"R"
#define SOCKTP_RELAY_S	"S"

#define TIMEOUT_SENDMSG  1000	//MS

#define ONEPACKET_SIZE	256

#define INTERVAL_MULTI_ORDER	1000
#define INTERVAL_SINGLE_ORDER	500

// for DashBoard Grid
enum EN_GD_EAINFO {
	GDEAINFO_APPID=0,
	GDEAINFO_BROKER,
	GDEAINFO_ACC,
	GDEAINFO_LIVEDEMO,
	GDEAINFO_IP,
	GDEAINFO_MAC,
	GDEAINFO_LOGON_MKTTIME,
	GDEAINFO_LOGON_LCOALTIME
};
#define GDEAINFO_COLCNT    8


enum EN_GD_MD {
	GDMD_CHKBOX,
	GDMD_APPID,
	GDMD_BROKER,
	GDMD_SYMBOL,
	GDMD_BID,
	GDMD_ASK,
	GDMD_SPREAD,
	GDMD_LOCAL_TIME,
	GDMD_MKT_TIME
};
#define GDMD_COLCNT 9


enum EN_GD_BALANCE {
	GDBAL_APPID,
	GDBAL_BROKER,
	GDBAL_ACC,
	GDBAL_BALANCE,
	GDBAL_EQUITY,
	GDBAL_PROFITS
};
#define GDBAL_COLCNT 6

enum EN_GD_POS {
	GDPOS_TICKET,
	GDPOS_SYMBOL,
	GDPOS_TYPE,
	GDPOS_OPENPRC,
	GDPOS_VOL,
	GDPOS_PROFIT,
	GDPOS_SL,
	GDPOS_TP,
	GDPOS_MAGIC,
	GDPOS_OPENTIME
};
#define GDPOS_COLCNT	10


enum EN_GD_ORD {
	GDORD_TICKET,
	GDORD_SYMBOL,
	GDORD_TYPE,
	GDORD_OPENPRC,
	GDORD_VOL,
	GDORD_SL,
	GDORD_TP,
	GDORD_MAGIC,
	GDORD_OPENTIME
};
#define GDORD_COLCNT	9


#define CB_CLOSEALL_BYSYMBOL 	0
#define CB_CLOSEALL_PROFIT 		1
#define CB_CLOSEALL_LOSS		2
#define CB_CLOSEALL_ALL			3

#define CB_CLOSEONE_TICKET	0
#define CB_CLOSEONE_BYSYMBOL	1
#define CB_CLOSEONE_MAGIC	2
#define CB_CLOSEONE_PROFIT	3
#define CB_CLOSEONE_LOSS	4
#define CB_CLOSEONE_ALL		5


#define CB_DELETEALL_BYSYMBOL 	0
#define CB_DELETEALL_ALL		1


#define CB_DELETEONE_TICKET		0
#define CB_DELETEONE_BYSYMBOL	1
#define CB_DELETEONE_MAGIC		2
#define CB_DELETEONE_ALL		3



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class CCommonInfo
{
public:
	CCommonInfo()
	{
		m_bAuthSuccess = false;
	}
	~CCommonInfo(){}

	AnsiString	m_UserId;
	AnsiString 	m_Pwd;
	AnsiString  m_AuthSvrIp;
	AnsiString	m_AuthSvrPort;
	AnsiString	m_RelaySvrIp;
	AnsiString 	m_RelaySvrPort;
	AnsiString 	m_DataSvrIp;
	AnsiString 	m_DataSvrPort;
	AnsiString 	m_AppId;
	AnsiString 	m_sSendTimeout;
	AnsiString 	m_sRecvTimeout;
	bool 		m_bAuthSuccess;


	bool Is_AuthDone() { return m_bAuthSuccess; }

	void Initialize();

	void ComposeAppId(AnsiString sInputUserId) {
		char id[128];
        sInputUserId = sInputUserId.UpperCase();
		sprintf(id, "%s_%.5s_%s", sInputUserId.c_str(), "MANAGER", "MANAGER");
		m_AppId = id;
	}
};

class CThreadId
{
public:
	CThreadId(){ InitializeCriticalSection(&m_cs); }
	~CThreadId(){ DeleteCriticalSection(&m_cs); }

	void Add(DWORD id);
	void Erase(DWORD id);
	void LoopBegin();
	bool Get(_Out_ DWORD& id);
	void LoopEnd();
private:
	set<DWORD>	m_set;
	set<DWORD>::iterator m_it;
	CRITICAL_SECTION	m_cs;
	bool				m_bLoopStart;
};


///////////////////////////////////////////////////////////
extern CCommonInfo 	_CommonInfo;
extern CThreadId	_ThreadIds;

void __Grid_Clear(TAdvStringGrid* grid);
bool __Grid_IsEmpty(TAdvStringGrid* grid);
void __Grid_DelRow(TAdvStringGrid* grid, int nDeletingIdx);
bool __Grid_Search(TAdvStringGrid* grid, String sData, int nColIdx, _Out_ int *pnFound);


///////////////////////////////////////////////////////////
void __MsgBox_Err(String sMsg);
void __MsgBox_Warn(String sMsg);
bool __MsgBox_Confirm(String sMsg);

#endif
