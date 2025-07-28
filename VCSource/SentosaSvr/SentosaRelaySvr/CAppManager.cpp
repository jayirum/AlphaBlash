#include "Main.h"
#include "CAppManager.h"
#include <ctime>
#include <cstdlib>
#include "../../Common/UTIL.h"
#include "../../Common/TimeUtils.h"
#include "../Common/ErrCodes.h"
#include "../Common/Inc.h"

extern CProtoGetList	g_listProtoGet;

CAppManager::CAppManager()
{
    InitializeCriticalSection(&m_csApp);
    m_bReady = FALSE;
    m_pManagerInfo = NULL;
}

CAppManager::~CAppManager()
{
    StopThread();
    DeInitialize();
    DeleteCriticalSection(&m_csApp);
}



void CAppManager::DeInitialize()
{
    LockApp();
    for (IT_MAP_APP it = m_mapApp.begin(); it != m_mapApp.end(); it++)
    {
        delete (*it).second;
    }
    m_mapApp.clear();
    UnlockApp();

    m_pManagerInfo = NULL;
}


BOOL CAppManager::Find_App(string sAppId, IT_MAP_APP& it)
{
    it = m_mapApp.find(sAppId);
    return (it != m_mapApp.end());
}

TAppInfo* CAppManager::Find_App(string sAppId)
{
    TAppInfo* pApp = NULL;

    IT_MAP_APP it = m_mapApp.find(sAppId);
    if (it == m_mapApp.end())
        return NULL;

    return (*it).second;
}

EN_APP_TP CAppManager::GetAppTp(string sAppId)
{
    EN_APP_TP tp;
    TAppInfo* pApp = Find_App(sAppId);
    if (pApp != NULL)
    {
        tp = pApp->enAppTp;
    }
    UnlockApp();
    return tp;
}

SOCKET  CAppManager::Get_RecvSocket_Of_EA(string sAppId)
{
    SOCKET sock = INVALID_SOCKET;
    LockApp();

    TAppInfo* pApp = Find_App(sAppId);
    if (pApp != NULL)
    {
        sock = pApp->sockMain;
    }
    UnlockApp();

    return sock;
}




VOID CAppManager::GetReady()
{
    if (Is_AlreadyReady())   return;

    if (
        //TODO (AppCount() > 1) &&
        (m_pManagerInfo != NULL)
        )
    {
        CBaseThread::ResumeThread();
        m_bReady = TRUE;
    }

}

string CAppManager::Add_AppInfo(string sUserId, EN_APP_TP enAppTp, string sAppId,
    string sBroker, string sAccNo, char cSockTp, SOCKET sockApp, string sClientIp, 
    string sMacAddr, string sLiveDemo, string sMktTime, BOOL bAlreadLogOn, _Out_ SOCKET& sockPrevSession)
{
    TAppInfo* pApp  = NULL;
    BOOL bCreate    = FALSE;
    sockPrevSession = INVALID_SOCKET;

    LockApp();

    pApp = Find_App(sAppId);
    if (pApp == NULL)
    {
        bCreate = TRUE;
        m_sUserId = sUserId;

        pApp = new TAppInfo;
        pApp->sUserId = sUserId;
        pApp->sBroker = sBroker;
        pApp->sAccNo = sAccNo;
        pApp->sAppId = sAppId;
        pApp->sockMain = INVALID_SOCKET;
        pApp->sockSend = INVALID_SOCKET;
        pApp->sLiveDemo = sLiveDemo;
        pApp->sClientIp = sClientIp;
        pApp->sMacAddr = sMacAddr;
        pApp->enAppTp = enAppTp;
        pApp->sLogonMktTime = sMktTime;
        pApp->bLogonCompleted = FALSE;
    }

    if (cSockTp == DEF_CLIENT_SOCKTP_RECV)
    {
        if (!bCreate && pApp->bLogonCompleted && bAlreadLogOn) {
            sockPrevSession = pApp->sockMain;
        }

        pApp->sockMain = sockApp;
        LOGGING(INFO, TRUE, "Recv Socket Add:%d", pApp->sockMain);

        if (pApp->sockSend != INVALID_SOCKET)
            pApp->bLogonCompleted = TRUE;
    }
    else
    {
        pApp->sockSend = sockApp;
        if (pApp->sockMain != INVALID_SOCKET)
            pApp->bLogonCompleted = TRUE;
    }

    if (Is_ManagerApp(enAppTp))
    {
        m_pManagerInfo = pApp;
    }

    m_mapApp[pApp->sAppId] = pApp;

    GetReady();
    UnlockApp();

    return pApp->sAppId;
}

BOOL CAppManager::Is_MainSock_AlreadyLogOn(string sAppId, SOCKET sockApp, EN_APP_TP enAppTp, char cSockTp, BOOL bLock)
{
    if (cSockTp != DEF_CLIENT_SOCKTP_RECV)
        return FALSE;

    if(bLock) LockApp();
    TAppInfo* pApp = Find_App(sAppId );
    if (bLock) UnlockApp();

    if (pApp == NULL)
        return FALSE;

    if (!pApp->bLogonCompleted)
        return FALSE;

    if (pApp->sockMain == INVALID_SOCKET)  
        return FALSE;

    if (pApp->sockMain == sockApp)
        return FALSE;

    return TRUE;
}


void CAppManager::ReturnClose_Of_DupLogon(SOCKET sockCurrSession)
{
    CProtoSet set;
    set.Begin();
    set.SetVal(FDS_CODE, __ALPHA::CODE_DUP_LOGON);
    set.SetVal(FDN_ERR_CODE, ERR_DUALSESSION_DISALLOWED);
    set.SetVal(FDS_MSG, "Double session is not allowed. Close this session for new incoming session");

    char zBuff[__ALPHA::LEN_BUF] = { 0 };
    int size = set.Complete(zBuff, FALSE);

    RequestSendIO(sockCurrSession, zBuff, size);
}


void CAppManager::Remove_App_OnlyForRecvSocket(string sAppId, SOCKET sockClosing, char cSockTp)
{
    if (cSockTp == DEF_CLIENT_SOCKTP_SEND)
        return;

    BOOL bDupLogon = FALSE;
    TAppInfo* pApp = NULL;
    IT_MAP_APP it;

    LockApp();

    if (Find_App(sAppId, it))
    {
        pApp = (*it).second;
        if (Is_CurrSocket_Closing(pApp->sockMain, sockClosing)==FALSE)
        {
            bDupLogon = TRUE;
            LOGGING(INFO, TRUE, "[RemoveApp]Don't Remove App as this is DupLogon Closing(prev:%d)(curr:%d)", sockClosing, pApp->sockMain);
        }
        else
        {
            if (Is_ManagerApp(pApp->enAppTp))
            {
                NoticeEA_ManagerLogOnOff(false);
                m_pManagerInfo = NULL;
            }
            else
            {
                NoticeManager_OneEALogOnOff( pApp, EN_LOGOUT);
            }

            delete (*it).second;
            m_mapApp.erase(it);
            LOGGING(INFO, TRUE, "[RemoveApp]Remove App because of closing(socket:%d)", sockClosing, pApp->sockMain);
        }
    }

    UnlockApp();

}


VOID    CAppManager::NoticeEA_ManagerLogOnOff(bool bLogon)
{
    char zSendBuff[__ALPHA::LEN_BUF] = { 0, };
    CProtoSet set;
    set.Begin();
    set.SetVal(FDS_CODE,    __ALPHA::CODE_COMMAND_BY_CODE);
    set.SetVal(FDS_COMMAND_CODE, CMD_NOTI_LOGONOUT);
    set.SetVal(FDN_APP_TP,  APPTP_MANAGER);

    string sReg = (bLogon) ? DEF_REG : DEF_UNREG;
    set.SetVal(FDS_REGUNREG, sReg);
    
    int nLen = set.Complete(zSendBuff, TRUE);

    LOGGING(INFO, TRUE, "Noti Log Manager Log On/off(%s) to EAs(%s)", sReg.c_str(), zSendBuff);
    for (IT_MAP_APP it = m_mapApp.begin(); it != m_mapApp.end(); ++it)
    {
        TAppInfo* pApp = (*it).second;
        if (pApp->enAppTp == APPTP_MANAGER)
            continue;
        RequestSendIO(pApp->sockMain, zSendBuff, nLen);
    }
}

VOID CAppManager::NoticeManager_EALogon(string sAppId, SOCKET sock, EN_APP_TP enAppTp)
{
    if (Is_ManagerConnected() == FALSE)    return;

    IT_MAP_APP it; 
    
    LockApp();    
    if (Find_App(sAppId, it) == FALSE) { UnlockApp(); return;}

    TAppInfo* pApp = (*it).second;
    enAppTp = pApp->enAppTp;
    UnlockApp();

    // Only recv socket is used for recognizing LogOn
    if (sock == pApp->sockSend)  return;

    if (enAppTp == APPTP_EA)
    {
        NoticeManager_OneEALogOnOff(pApp, EN_LOGON);
    }
    else if (enAppTp == APPTP_MANAGER)
    {// MANAGER will request after it logon

        LockApp();
        int nEACnt = 0;
        for (IT_MAP_APP it = m_mapApp.begin(); it != m_mapApp.end(); ++it)
        {
            TAppInfo* p = (*it).second;
            if (p->enAppTp == APPTP_MANAGER)
                continue;

            NoticeManager_OneEALogOnOff( p, EN_LOGON);
            nEACnt++;
        }
        UnlockApp();

        if (nEACnt == 0)
        {
            NoticeManager_OneEALogOnOff(m_pManagerInfo, EN_LOGON);
        }
    }
}

void CAppManager::NoticeManager_OneEALogOnOff( TAppInfo* pApp, EN_LOGONOUT enOnOut)
{
    if (m_pManagerInfo == NULL) return;

    LockApp();
    int nEACnt = m_mapApp.size() - 1;
    UnlockApp();
    
    if (nEACnt == 0)
        return;

    char zSendBuff[__ALPHA::LEN_BUF] = { 0, };
    CProtoSet set;
    set.Begin();
    set.SetVal(FDS_CODE,        __ALPHA::CODE_COMMAND_BY_CODE);
    set.SetVal(FDN_COUNT, nEACnt);
    set.SetVal(FDS_COMMAND_CODE, CMD_NOTI_LOGONOUT);
    set.SetVal(FDS_BROKER, pApp->sBroker.c_str());
    set.SetVal(FDS_ACCNO_MINE, pApp->sAccNo.c_str());
    set.SetVal(FDS_KEY, pApp->sAppId.c_str());
    set.SetVal(FDS_CLIENT_IP, pApp->sClientIp.c_str());
    set.SetVal(FDS_MAC_ADDR, pApp->sMacAddr.c_str());
    set.SetVal(FDS_LIVEDEMO, pApp->sLiveDemo.c_str());
    set.SetVal(FDS_REGUNREG, (enOnOut == EN_LOGON) ? DEF_REG : DEF_UNREG);
    set.SetVal(FDS_TIME, (pApp->sLogonMktTime.size()>0)? pApp->sLogonMktTime.c_str():"-");

    int nLen = set.Complete(zSendBuff, TRUE);

    LOGGING(INFO, TRUE, "[Noti Log On/Out to Suites(%s)", zSendBuff);
    RequestSendIO(m_pManagerInfo->sockMain, zSendBuff, nLen);
}


VOID CAppManager::Execute(char* pCode, char* pzRecvData)
{
    //TRecvData* pData = new TRecvData(pzRecvData);
    string* pData = new string(pzRecvData);
    CProtoUtils util;
    if (util.Is_JustRelay(pzRecvData))
    {
        PostThreadMessage(m_dwThreadID, WM_JUST_BYPASS, (WPARAM)pData->size(), (LPARAM)pData);
    }
    else
    {
        if (strcmp(pCode, __ALPHA::CODE_MARKET_DATA) == 0)
        {
            PostThreadMessage(m_dwThreadID, WM_MARKET_DATA, (WPARAM)pData->size(), (LPARAM)pData);
        }
        else if (strcmp(pCode, __ALPHA::CODE_COMMAND_BY_CODE) == 0)
        {
            PostThreadMessage(m_dwThreadID, WM_COMMAND_CODE, (WPARAM)pData->size(), (LPARAM)pData);
        }
    }

}



void CAppManager::ThreadFunc()
{
    string sDirection;
    while (m_bContinue)
    {
        Sleep(1);
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            BOOL bDelete = TRUE;
            string sData = *(string*)msg.lParam;
            delete (string*)msg.lParam; 
            if (msg.message == WM_MARKET_DATA)
            {
                MD_EA_Manager(sData, (int)msg.wParam);
            }
            else if (msg.message == WM_COMMAND_CODE)
            {
                CProtoGet get;
                get.ParsingWithHeader((char*)sData.c_str());
                get.GetVal(FDS_FLOW_DIRECTION, &sDirection);
                if (sDirection.compare(DIRECTION_TO_EA) == 0) {
                    Command_Manager_EA(get);
                    g_listProtoGet.Add(get);
                }
            }
            else if (msg.message == WM_JUST_BYPASS)
            {
                CProtoGet get;
                get.ParsingWithHeader((char*)sData.c_str());
                get.GetVal(FDS_FLOW_DIRECTION, &sDirection);
                if (sDirection.compare(DIRECTION_TO_EA) == 0) {
                    ByPass_Manager_EA(get);
                    g_listProtoGet.Add(get);
                }
                else {
                    ByPass_EA_Manager(sData, (int)msg.wParam);
                }
            }
            
           
            
        } // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    } // while(pThis->m_bRun)

    return;
}

void    CAppManager::Logout_Manager_EA(_In_ CProtoGet& get)
{
    LockApp();
    int nEACnt = m_mapApp.size() - 1;
    UnlockApp();

    if (nEACnt == 0)
        return;

    string sAppId, sBroker;
    get.GetVal(FDS_KEY, &sAppId);
    get.GetVal(FDS_BROKER, &sBroker);
    LockApp();
    for (IT_MAP_APP it = m_mapApp.begin(); it != m_mapApp.end(); ++it)
    {
        TAppInfo* pApp = (*it).second;
        if (pApp->sAppId == sAppId)
        {
            LOGGING(INFO, TRUE, "[Send Logout to EA](APP ID:%s)(Broker:%s)", sAppId.c_str(), sBroker.c_str());
            RequestSendIO(pApp->sockMain, get.GetOrgData(), get.OrgDataSize());
        }
    }
    UnlockApp();
}

void    CAppManager::ByPass_Manager_EA(_In_ CProtoGet& get)
{
    LockApp();
    int nEACnt = m_mapApp.size() - 1;
    UnlockApp();

    if (nEACnt == 0)
        return;

    string sAppId;
    get.GetVal(FDS_KEY, &sAppId);
    LockApp();
    for (IT_MAP_APP it = m_mapApp.begin(); it != m_mapApp.end(); ++it)
    {
        TAppInfo* pApp = (*it).second;
        if (pApp->enAppTp == APPTP_MANAGER)
            continue;
        if (pApp->sAppId != sAppId)
            continue;

        LOGGING(INFO, TRUE, "[Send Command for Balance to(%d) EA-%s]%s", pApp->sockMain, pApp->sAppId.c_str(), get.GetOrgData());
        RequestSendIO(pApp->sockMain, get.GetOrgData(), get.OrgDataSize());
    }
    UnlockApp();

}
void    CAppManager::ByPass_EA_Manager(_In_ string& sData, int nTotLen)
{
    if (!m_pManagerInfo)  return;
    RequestSendIO(m_pManagerInfo->sockMain, sData.c_str(), nTotLen);
}



void CAppManager::Command_Manager_EA(_In_ CProtoGet& get)
{
    string sCommandCode;

    try
    {
        //int n;
        CHECK_BOOL(get.GetVal(FDS_COMMAND_CODE, &sCommandCode), "No FDS_COMMAND_CODE");
        if (sCommandCode.compare(CMD_NOTI_LOGONOUT) == 0)
        {
            if (!m_pManagerInfo)  return;
            NoticeManager_EALogon(m_pManagerInfo->sAppId, m_pManagerInfo->sockMain, APPTP_MANAGER);
        }
        else if (sCommandCode.compare(CMD_MD_SUB) == 0 || sCommandCode.compare(CMD_MD_UNSUB) == 0)
        {
            MD_Manager_EA(get);
        }
    }
    catch (const char* e)
    {
        LOGGING(ERR, TRUE, "[CommandProc]%s", e);
        return;
    }
}

void CAppManager::MD_Manager_EA(_In_ CProtoGet& get)
{
    LockApp();
    int nEACnt = m_mapApp.size() - 1;
    UnlockApp();

    if (nEACnt == 0)
        return;

    string sAppId;
    get.GetVal(FDS_KEY, &sAppId);
    LockApp();
    for (IT_MAP_APP it = m_mapApp.begin(); it != m_mapApp.end(); ++it)
    {
        TAppInfo* pApp = (*it).second;
        if (pApp->enAppTp == APPTP_MANAGER)
            continue;
        if (pApp->sAppId != sAppId)
            continue;

        LOGGING(INFO, TRUE, "[Send Command to(%d) EA-%s]%s", pApp->sockMain, pApp->sAppId.c_str(), get.GetOrgData());
        RequestSendIO(pApp->sockMain, get.GetOrgData(), get.OrgDataSize());
    }
    UnlockApp();
}


void    CAppManager::MD_EA_Manager(_In_ string& sData, int nTotLen)
{
    if (!m_pManagerInfo)  return;
    RequestSendIO(m_pManagerInfo->sockMain, sData.c_str(), nTotLen);
}

