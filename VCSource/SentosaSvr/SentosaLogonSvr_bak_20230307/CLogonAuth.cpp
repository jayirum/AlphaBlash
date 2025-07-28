#include "Main.h"
#include "CLogonAuth.h"
#include <ctime>
#include <cstdlib>
#include "../../Common/AlphaProtocol.h"
#include "CConfig.h"

extern CConfig	g_config;

CLogonAuth::CLogonAuth():CBaseThread(FALSE)
{
    m_bFinished = FALSE;
    CBaseThread::ResumeThread();
}

CLogonAuth::~CLogonAuth()
{
    StopThread();    
}


VOID CLogonAuth::Execute(TPacket* pPacket)
{
    PostThreadMessage(m_dwThreadID, WM_LOGON_AUTH, (WPARAM)0, (LPARAM)pPacket);
}



void CLogonAuth::ThreadFunc()
{
    while (m_bContinue)
    {
        Sleep(1);
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            BOOL bDelete = TRUE;
            if (msg.message == WM_LOGON_AUTH)
            {
                TPacket* p = (TPacket*)msg.lParam;
                Logon_Auth(p->sock, p->sClientIp, p->packet.c_str(), p->packet.size());
            }

            delete (TPacket*)msg.lParam;
        } // while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    } // while(pThis->m_bRun)

    return;
}



char* CLogonAuth::ComposeAppId(char* pzUserId, char* pzBroker, char* pzAcc)
{
    sprintf(m_zAppId, "%s_%.2s_%s", pzUserId, pzBroker, pzAcc);
    return m_zAppId;
}

BOOL CLogonAuth::Logon_Auth(SOCKET sock, string sClientIp, const char* pLoginData, int nDataLen)
{
    CProtoGet get;
    int nFieldCnt=0;
    int nInnerArrCnt = 0;
    nFieldCnt = get.ParsingWithHeader((char*)pLoginData, &nInnerArrCnt);
    if (nFieldCnt==0)
    {
        LOGGING(ERR, TRUE, "No Data(%s)", pLoginData);
        return FALSE;
    }
    int res = 0;
    char zBrokerName[128] = { 0 };
    char zAccNo[512] = { 0 };
    char zMac[128] = { 0 };
    char zUserId[128] = { 0 };
    char zPwd[128] = { 0 };
    char zLiveDemo[128] = { 0 };
    char zSockTp[128] = { 0 };
    EN_APP_TP enAppTp;

    res += get.GetVal(FDS_BROKER, zBrokerName);
    res += get.GetVal(FDS_ACCNO_MINE, zAccNo);
    res += get.GetVal(FDS_USER_ID, zUserId);
    res += get.GetVal(FDS_USER_PASSWORD, zPwd);
    res += get.GetVal(FDS_MAC_ADDR, zMac);
    res += get.GetVal(FDS_LIVEDEMO, zLiveDemo);
    
    int nAppTp = get.GetValN(FDN_APP_TP);
    enAppTp = (EN_APP_TP)nAppTp;

    LOGGING(INFO, TRUE, "(%c)(UserID:%s)(Pwd:%s)(Broker:%s)(AccNo:%s)(Mac:%s)(LiveDemo:%s)",
        zSockTp[0], zUserId, zPwd, zBrokerName, zAccNo, zMac, zLiveDemo);
    //TODO 

    //LOGGING(INFO, TRUE, "[LOGIN](%5.5s)(%c)(Socket:%d)", zBrokerKey, zClientSockTp[0], sock);

    if (res < 6)
    {
        char zMsg[] = "Logon_auth packet is not correct";
        LOGGING(ERR, TRUE, zMsg);
        ReturnError(sock, __ALPHA::CODE_LOGON_AUTH, 9999, zMsg);
        return FALSE;
    }

    {
        ////////////////////////////////////////////////////////////////////////////////
        //
        //	TODO. Authentication via DB
        //	TODO. Block dual logon
        //
        ////////////////////////////////////////////////////////////////////////////////
    }

    char zSendBuff[__ALPHA::LEN_BUF] = { 0, };
    CProtoSet	set;
    set.Begin();
    set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON_AUTH);
    set.SetVal(FDS_SUCC_YN, "Y");
    set.SetVal(FDS_RELAY_IP,    g_config.getRelaySvrIP());
    set.SetVal(FDS_RELAY_PORT,  g_config.getRelaySvrPortS());
    set.SetVal(FDS_DATASVR_IP,  g_config.getDataSvrIP());
    set.SetVal(FDS_DATASVR_PORT, g_config.getDataSvrPortS());
    set.SetVal(FDS_KEY, ComposeAppId(zUserId, zBrokerName, zAccNo));
    int nLen = set.Complete(zSendBuff, (enAppTp==APPTP_MANAGER));

    LOGGING(INFO, TRUE, "[Return Login Auth to Client](%s)", zSendBuff);
    RequestSendIO(sock, zSendBuff, nLen);

    // Only client receiving socket is added ==> To transfer data to client
    //if (zClientSockTp[0] == DEF_CLIENT_SOCKTP_RECV)
    //{
    //	LOGGING(INFO, FALSE, "[LOGIN-3]%d", sock);
    //	AddList_ClientRecvSock(zAppId, sock);
    //}
    return TRUE;
}