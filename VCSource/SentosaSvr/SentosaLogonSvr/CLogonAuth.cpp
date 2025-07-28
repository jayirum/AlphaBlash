#include "CLogonAuth.h"

#include "../../Common/AlphaProtocol.h"
#include "../Common/CConfig.h"

extern CConfig	g_config;

CLogonAuth::CLogonAuth()
{
    m_bFinished = FALSE;
    m_dbHandler = NULL;
}

CLogonAuth::~CLogonAuth()
{
    DBClose();
}


char* CLogonAuth::ComposeAppId(char* pzUserId, char* pzBroker, char* pzAcc)
{
    sprintf(m_zAppId, "%s_%.5s_%s", pzUserId, pzBroker, pzAcc);
    return m_zAppId;
}


RET_SENTOSA  CLogonAuth::DB_Auth(
    char* pzUserId
    , char* pzPwd
    , char* pzAppId
    , EN_APP_TP enAppTp
    , char* pzBrokerName
    , char* pzAccNo
    , char* pzLiveDemo
    , char* pzClientIp
    , char* pzMac
    , char* pzMarketTime)

{
    char zQ[1024];
    sprintf(zQ, "Call sp_logon("
        "'%s'"  //i_user_id	varchar(20)
        ",'%s'" //i_user_pwd	varchar(20)
        ",'%s'" //i_app_id	varchar(20)
        ",'%s'" //i_app_tp	char(1)
        ",'%s'" //i_broker_nm	varchar(50)
        ",'%s'" //i_acc_no
        ",'%s'" //i_livedemo	 	char(1)
        ",'%s'" //i_logon_ip	 	varchar(15)
        ",'%s'" //i_logon_mac 	varchar(20)
        ",'%s'" //i_logontime_broker	varchar(21)
        ",'%s'" //i_svr_tp			char(1)	-- // 'A':AUTHSVR, 'R':RELAYSVR, 'D':DATASVR)
        ",'%s'" //i_dup_logon_yn
        ");"
        ,pzUserId
        ,pzPwd
        ,pzAppId
        ,AppTp_S(enAppTp)
        ,pzBrokerName
        ,pzAccNo
        ,pzLiveDemo
        ,pzClientIp
        ,pzMac
        ,pzMarketTime
        ,DEF_SVRTP_AUTH
        ,"N"
    );
    auto_ptr<Rs> rs(m_dbHandler->Execute(zQ));
    int ret_code;
    std::string ret_msg;
    if (!rs->Is_Successful_ExcutingSP(ret_code, ret_msg))
    {
        LOGGING(ERR, TRUE, "[DB_Auth]%s(%s)", ret_msg.c_str(), zQ);
        strcpy(m_zMsg, "ERR_EXCEPTION_DB");
        return ERR_EXCEPTION_DB;
    }
    if (ret_code != ERR_SUCCESS)
    {
        LOGGING(ERR, TRUE, "[DB_Auth]SP return error(%s)", ret_msg.c_str());
        strcpy(m_zMsg, ret_msg.c_str());
        return (RET_SENTOSA)ret_code;
    }
    return ERR_SUCCESS;
}

BOOL CLogonAuth::Logon_Auth(_In_ CProtoGet& get, SOCKET sockClient, string sClientIp)
{
    int res = 0;
    char zBrokerName[128] = { 0 };
    char zAccNo[512] = { 0 };
    char zMac[128] = { 0 };
    char zUserId[128] = { 0 };
    char zPwd[128] = { 0 };
    char zAppId[128] = { 0 };
    char zLiveDemo[128] = { 0 };
    char zMarketTime[128] = { 0 };
    EN_APP_TP enAppTp;

    res += get.GetVal(FDS_BROKER, zBrokerName);
    res += get.GetVal(FDS_ACCNO_MINE, zAccNo);
    res += get.GetVal(FDS_USER_ID, zUserId);
    res += get.GetVal(FDS_USER_PASSWORD, zPwd);
    res += get.GetVal(FDS_MAC_ADDR, zMac);
    res += get.GetVal(FDS_LIVEDEMO, zLiveDemo);
    res += get.GetVal(FDS_KEY, zAppId);
    res += get.GetVal(FDS_TIME, zMarketTime);
    
    int nAppTp = get.GetValN(FDN_APP_TP);
    enAppTp = (EN_APP_TP)nAppTp;

    LOGGING(INFO, TRUE, "(UserID:%s)(Pwd:%s)(Broker:%s)(AccNo:%s)(Mac:%s)(LiveDemo:%s)",
        zUserId, zPwd, zBrokerName, zAccNo, zMac, zLiveDemo);
    if (res < 6)
    {
        char zMsg[] = "Logon_auth packet is not correct";
        LOGGING(ERR, TRUE, zMsg);
        ReturnError(sockClient, __ALPHA::CODE_LOGON_AUTH, ERR_INCORRECT_DATA, "ERR_INCORRECT_DATA");
        return FALSE;
    }

    if (!DBOpen())
    {
        ReturnError(sockClient, __ALPHA::CODE_LOGON_AUTH, ERR_DBOPEN_ERROR, "ERR_DBOPEN_ERROR");
        return  FALSE;
    }

    // Authentication via DB // 
    RET_SENTOSA retCode = DB_Auth(zUserId, zPwd, zAppId, enAppTp, zBrokerName,
                                    zAccNo, zLiveDemo, (char*)sClientIp.c_str(), zMac, zMarketTime);
    if (retCode != ERR_SUCCESS)
    {
        ReturnError(sockClient, __ALPHA::CODE_LOGON_AUTH, (int)retCode, m_zMsg);
        return  FALSE;
    }

    retCode = Send_ReturnInfo_FromDB(zUserId, enAppTp);
    if (retCode!=ERR_SUCCESS)
    {
        ReturnError(sockClient, __ALPHA::CODE_LOGON_AUTH, (int)retCode, m_zMsg);
        return  FALSE;
    }
    char zSendBuff[__ALPHA::LEN_BUF] = { 0, };
    CProtoSet	set;
    set.Begin();
    set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON_AUTH);
    set.SetVal(FDS_SUCC_YN, "Y");
    set.SetVal(FDS_RELAY_IP,    m_sRelaySvrIP.c_str());
    set.SetVal(FDS_RELAY_PORT, m_nRelaySvrPort);
    set.SetVal(FDS_DATASVR_IP, m_sRelaySvrIP.c_str());
    set.SetVal(FDS_DATASVR_PORT, m_nRelaySvrPort);
    set.SetVal(FDS_KEY, zAppId);
    set.SetVal(FDS_VERSION, m_zVersion);
    int nLen = set.Complete(zSendBuff, (enAppTp==APPTP_MANAGER));

    LOGGING(INFO, TRUE, "[Return Login Auth to Client](%s)", zSendBuff);
    RequestSendIO(sockClient, zSendBuff, nLen);

    return TRUE;
}

RET_SENTOSA  CLogonAuth::Send_ReturnInfo_FromDB(char* pzUserID, EN_APP_TP enAppTp)
{
    char zQ[1024];
    sprintf(zQ, "Call sp_logon_svr_version_info("
        "'%s'"  //i_user_id	varchar(20)
        ",'%s'" //i_app_tp	char(1)
        ");"
        , pzUserID
        , AppTp_S(enAppTp)
    );
    auto_ptr<Rs> rs(m_dbHandler->Execute(zQ));
    if (!rs->IsValid())
    {
        LOGGING(ERR, TRUE, "DB EXEC Error(%s)(%s)", m_dbHandler->GetMsg(), zQ);
        strcpy(m_zMsg, "ERR_EXCEPTION_DB");
        return ERR_EXCEPTION_DB;
    }
    if (rs->getRecordCnt() == 0)
    {
        LOGGING(ERR, TRUE, "No Recordsets");
        strcpy(m_zMsg, "ERR_NO_RECORDSETS");
        return ERR_NO_RECORDSETS;
    }
    int ret_code;
    std::string ret_msg;
    if (!rs->getInt(0, "RET_CODE", &ret_code) || !rs->getString(0, "RET_MSG", &ret_msg))
    {
        LOGGING(ERR, TRUE, "No RET_CODE or RET_MSG");
        strcpy(m_zMsg, "ERR_EXCEPTION_DB");
        return ERR_EXCEPTION_DB;
    }
    if (ret_code != ERR_SUCCESS)
    {
        LOGGING(ERR, TRUE, "SP return error(%s)", ret_msg.c_str());
        strcpy(m_zMsg, ret_msg.c_str());
        return (RET_SENTOSA)ret_code;
    }

    double dVersion;
    if (
        !rs->getString(0, "RELAYSVR_IP", &m_sRelaySvrIP) ||
        !rs->getInt(0, "RELAYSVR_PORT", &m_nRelaySvrPort) ||
        !rs->getDbl(0, "VERSION", &dVersion)
        )
    {
        LOGGING(ERR, TRUE, "GetData Error.Check the column name");
        strcpy(m_zMsg, "ERR_INCORRECT_DATA");
        return ERR_INCORRECT_DATA;
    }
    sprintf(m_zVersion, "%.1f", dVersion);
    return ERR_SUCCESS;
}

BOOL CLogonAuth::DBOpen()
{
    m_dbHandler = new CMySqlHandler();
    m_dbHandler->Initialize(g_config.getDBIp(), g_config.getDBPort(), g_config.getDBUserId(),
        g_config.getDBUserPwd(), g_config.getDBName());

    if (!m_dbHandler->OpenDB())
    {
        LOGGING(ERR, TRUE, "DB Open Error(%s)", m_dbHandler->GetMsg());
        return FALSE;
    }
    return TRUE;
}

VOID CLogonAuth::DBClose()
{
    if (m_dbHandler)
        delete m_dbHandler;
    m_dbHandler = NULL;
}