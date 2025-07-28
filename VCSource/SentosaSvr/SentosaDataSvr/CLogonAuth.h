#pragma once

#include "Main.h"
#include "../Common/Inc.h"
#include "../../Common/CMySqlHandler.h"
#include "../../Common/AlphaProtocol.h"
#include "../Common/ErrCodes.h"
#pragma warning(disable:4996)
class CLogonAuth 
{
public:
    CLogonAuth();
    ~CLogonAuth();

    BOOL Logon_Auth(_In_ CProtoGet& get, SOCKET sockClient, string sClientIp);
    
    BOOL    DBOpen();
    VOID    DBClose();
    BOOL    Is_Finished() { return m_bFinished; }
private:
    RET_SENTOSA  DB_Auth(
        char* pzUserId
        , char* pzPwd
        , char* pzAppId
        , EN_APP_TP enAppTp
        , char* pzBrokerName
        , char* pzAccNo
        , char* pzLiveDemo
        , char* pzClientIp
        , char* pzMac
        , char* pzMarketTime);
    RET_SENTOSA  Send_ReturnInfo_FromDB(char* pzUserID, EN_APP_TP enAppTp);
    char* ComposeAppId(char* pzUserId, char* pzBroker, char* pzAcc);

private:
    
    char                m_zAppId[128];
    BOOL                m_bFinished;
    CMySqlHandler*      m_dbHandler;

    std::string         m_sRelaySvrIP;
    int                 m_nRelaySvrPort;
    char                m_zVersion[128];
    char                m_zMsg[1024];

};

