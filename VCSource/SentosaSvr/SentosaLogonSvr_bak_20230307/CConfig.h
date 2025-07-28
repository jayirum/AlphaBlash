#pragma once

#include <Windows.h>
#include <map>
#include <string>
#include <vector>
using namespace std;



#define SEC_APP_CONFIG		"APP_CONFIG"
#define SEC_SERVERS_INFO	"SERVERS_INFO"
#define SEC_DB_INFO			"DB_INFO"
#define DEF_UPDATED_KEY		"UPDATED"

enum EN_CNFG_RET { CNFG_ERR = -1, CNFG_SUCC, CNFG_IDLE };


struct TAppConfig
{
	char	zListenIP[128];
	int 	nListenPort;
	int 	nWorkerThreadCnt;
	double 	dCutOffTimeRate;
	int		nTimeoutReadConfig;
};

struct TServersInfo
{
	char	zRelaySvrIp[128];
	int		nRelaySvrPort;
	char	zDataSvrIp[128];
	int		nDataSvrPort;
};

struct TDBInfo
{
	char	zAddress[128];
	char	zID[128];
	char	zPwd[128];
	char	zDbName[128];
	int		nPoolCnt;
};

class CConfig
{
public:
	CConfig();
	~CConfig();
	
	VOID 	Initialize(string sFileName);
	EN_CNFG_RET	ReLoad_ConfigInfo(BOOL bInit = FALSE);

private:
	VOID	DeInitialize();
	BOOL	Is_Updated(char* pzSec);
	EN_CNFG_RET ReLoad_AppConfig(BOOL bInit = FALSE);
	EN_CNFG_RET ReLoad_ServersInfo(BOOL bInit = FALSE);
	EN_CNFG_RET ReLoad_DBInfo(BOOL bInit = FALSE);

public:
	char*	getFileName() { return m_zFileName;}
	char*	getVal(char* pzSec, char* pzKey, _Out_ char* pzData);

	int		getTimeoutReadCnfg() { return m_appConfig.nTimeoutReadConfig; }
	char*	getListenIP() { return m_appConfig.zListenIP; }
	int		getListenPort() { return m_appConfig.nListenPort; }
	char*	getListenPortS() { sprintf(m_zMsg, "%d", m_appConfig.nListenPort); return m_zMsg; }
	int		getWorkThreadCnt() { return m_appConfig.nWorkerThreadCnt; }

	char*	getRelaySvrIP() { return m_serversInfo.zRelaySvrIp; }
	int		getRelaySvrPort() { return m_serversInfo.nRelaySvrPort; }
	char*	getRelaySvrPortS() { sprintf(m_zMsg, "%d", m_serversInfo.nRelaySvrPort); return m_zMsg; }
	
	char*	getDataSvrIP() { return m_serversInfo.zDataSvrIp; }
	int		getDataSvrPort() { return m_serversInfo.nDataSvrPort; }
	char*	getDataSvrPortS() { sprintf(m_zMsg, "%d", m_serversInfo.nDataSvrPort); return m_zMsg; }


private:	
	char 						m_zFileName[MAX_PATH];
	TAppConfig					m_appConfig;
	TServersInfo				m_serversInfo;
	TDBInfo						m_dbInfo;
	char 						m_zMsg[1024];
};
