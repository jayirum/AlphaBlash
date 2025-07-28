#pragma once

#include "Main.h"
#include "Inc.h"
#include <map>
#include "../../Common/BaseThread.h"

struct TLoginInfo
{
    string sUserId;
    string sBroker;
    string sAccNo;
    string sAppId;
    //string sClientIp;
    //string sClientMac;
    SOCKET sockEARecv;
    SOCKET sockEASend;
    SOCKET sockSuitesSock;
    EN_APP_TP enAppTp;

    
};

typedef map<TYPE_APP_ID, TLoginInfo*>		    MAP_APP;
typedef map<TYPE_APP_ID, TLoginInfo*>::iterator IT_MAP_APP;


class CLogonAuth : public CBaseThread
{
public:
    CLogonAuth();
    ~CLogonAuth();

    VOID Execute(TPacket* pPacket);
    BOOL Logon_Auth(SOCKET sock, string sClientIp, const char* pLoginData, int nDataLen);
    
    BOOL    Is_Finished() { return m_bFinished; }
private:
    char* ComposeAppId(char* pzUserId, char* pzBroker, char* pzAcc);

    //VOID Initialize();
    void ThreadFunc();
    
private:
    
    char                m_zAppId[128];
    BOOL                m_bFinished;


};

