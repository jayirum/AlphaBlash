#pragma once

#include "Main.h"
#include "../Common/Inc.h"
#include <map>
#include "../../Common/BaseThread.h"
#include "../../Common/AlphaProtocol.h"

struct TAppInfo
{
    string sUserId;
    string sBroker;
    string sAccNo;
    string sAppId;
    SOCKET sockMain;
    SOCKET sockSend;
    string sClientIp;
    string sMacAddr;
    string sLiveDemo;
    string sLogonMktTime;
    EN_APP_TP enAppTp;
    BOOL bLogonCompleted;

    TAppInfo()
    {
        sockMain = sockSend = INVALID_SOCKET;
        bLogonCompleted = FALSE;
    }
    
};

typedef map<TYPE_APP_ID, TAppInfo*>		    MAP_APP;
typedef map<TYPE_APP_ID, TAppInfo*>::iterator IT_MAP_APP;


class CAppManager : public CBaseThread
{
public:
    CAppManager();
    ~CAppManager();

    VOID Execute(char* pCode, char* pzRecvData);

    BOOL Is_MainSock_AlreadyLogOn(string sAppId, SOCKET sockApp, EN_APP_TP enAppTp, char cSockTp, BOOL bLock);
    string Add_AppInfo(string sUserId, EN_APP_TP enAppTp, string sAppId,
        string sBroker, string sAccNo, char cSockTp, SOCKET sockApp,
        string sClientIp, string sMacAddr, string sLiveDemo, string sMktTime, BOOL bAlreadLogOn,
        _Out_ SOCKET& sockPrevSession);

    void    Remove_App_OnlyForRecvSocket( string sAppId, SOCKET sockClosing, char cSockTp);
    SOCKET  Get_RecvSocket_Of_EA(string sAppId);
    //SOCKET  Get_RecvSocket_Of_Suites() { return (!m_pSuitesInfo) ? INVALID_SOCKET : m_pSuitesInfo->sockSuitesSock; }

    VOID    NoticeManager_EALogon(string sAppId, SOCKET sock, EN_APP_TP enAppTp);
    VOID    NoticeEA_ManagerLogOnOff(bool bLogon);
    EN_APP_TP   GetAppTp(string sAppId);
    void    ReturnClose_Of_DupLogon(SOCKET sockCurrSession);
private:
    void    NoticeManager_OneEALogOnOff(TAppInfo* pApp, EN_LOGONOUT enOnOut);
    void    Command_Manager_EA(_In_ CProtoGet& get);

    void    ByPass_Manager_EA(_In_ CProtoGet& get);
    void    ByPass_EA_Manager(_In_ string& sData, int nTotLen);

    void    MD_Manager_EA(_In_ CProtoGet& get);
    void    MD_EA_Manager(_In_ string& sData, int nTotLen);

    void    Logout_Manager_EA(_In_ CProtoGet& get);
    

    
    BOOL    Is_CurrSocket_Closing(SOCKET sockCurr, SOCKET sockClosing) { return (sockCurr == sockClosing); }
    BOOL    Is_ManagerConnected() { return (m_pManagerInfo != NULL); }
    //VOID Initialize();
    void    ThreadFunc();
    void    DeInitialize();
    BOOL    Find_App(string sAppId, IT_MAP_APP& it);
    TAppInfo* Find_App(string sAppId);
    VOID    GetReady();
    BOOL    Is_ManagerApp(EN_APP_TP enAppTp)    { return (enAppTp == APPTP_MANAGER); }
    BOOL    Is_AlreadyReady()                   { return m_bReady; }
    int     AppCount()                          { return m_mapApp.size(); }
    VOID    LockApp()                           { EnterCriticalSection(&m_csApp); }
    VOID    UnlockApp()                         { LeaveCriticalSection(&m_csApp); }

private:
    string              m_sUserId;
    TAppInfo			* m_pManagerInfo;
	MAP_APP				m_mapApp;
	CRITICAL_SECTION	m_csApp;
    BOOL                m_bReady;
};

