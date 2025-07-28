#pragma warning(disable:4996)

#include "CConfig.h"
#include "../../Common/AlphaInc.h"
#include "../../Common/Util.h"
#include "Inc.h"


CConfig::CConfig()
{
}

CConfig::~CConfig()
{
	DeInitialize();
}

VOID CConfig::Initialize(string sFileName)
{
	strcpy(m_zFileName,sFileName.c_str());
}



EN_CNFG_RET CConfig::ReLoad_AppConfig(BOOL bInit)
{
	if (bInit == FALSE && !Is_Updated(SEC_APP_CONFIG))
		return CNFG_IDLE;

	if (!CUtil::SetConfigValue(getFileName(), SEC_APP_CONFIG, DEF_UPDATED_KEY, "N"))
		return CNFG_ERR;

	ZeroMemory(&m_appConfig, sizeof(m_appConfig));

	{
		CUtil::GetConfig(getFileName(), TEXT("APP_CONFIG"), TEXT("LISTEN_IP"), m_appConfig.zListenIP);
		if (m_appConfig.zListenIP[0] == NULL)	return CNFG_ERR;
	}

	{
		CUtil::GetConfig(getFileName(), TEXT("APP_CONFIG"), TEXT("LISTEN_PORT"), m_zMsg);
		if (m_zMsg[0] == NULL)	return CNFG_ERR;
		m_appConfig.nListenPort = atoi(m_zMsg);
	}

	{
		CUtil::GetConfig(getFileName(), TEXT("APP_CONFIG"), TEXT("TIMEOUT_READ_CONFIG_SEC"), m_zMsg);
		if (m_zMsg[0] == NULL)	return CNFG_ERR;
		m_appConfig.nTimeoutReadConfig = atoi(m_zMsg);
	}
	

	return CNFG_SUCC;
}

//
EN_CNFG_RET CConfig::ReLoad_DBInfo(BOOL bInit)
{
	if (bInit == FALSE && !Is_Updated(SEC_DB_INFO))
		return CNFG_IDLE;

	if (!CUtil::SetConfigValue(getFileName(), SEC_DB_INFO, DEF_UPDATED_KEY, "N"))
		return CNFG_ERR;

	ZeroMemory(&m_dbInfo, sizeof(m_dbInfo));

	{
		CUtil::GetConfig(getFileName(), SEC_DB_INFO, TEXT("DB_IP"), m_dbInfo.zIP);
		if (m_dbInfo.zIP[0] == NULL)	return CNFG_ERR;
	}
	{
		CUtil::GetConfig(getFileName(), SEC_DB_INFO, TEXT("DB_PORT"), m_zMsg);
		if (m_zMsg[0] == NULL)	return CNFG_ERR;
		m_dbInfo.nPort = atoi(m_zMsg);
	}

	{
		CUtil::GetConfig(getFileName(), SEC_DB_INFO, TEXT("DB_ID"), m_dbInfo.zID);
		if (m_dbInfo.zID[0] == NULL)	return CNFG_ERR;
	}

	{
		CUtil::GetConfig(getFileName(), SEC_DB_INFO, TEXT("DB_PWD"), m_dbInfo.zPwd);
		if (m_dbInfo.zPwd[0] == NULL)	return CNFG_ERR;
	}

	{
		CUtil::GetConfig(getFileName(), SEC_DB_INFO, TEXT("DB_NAME"), m_dbInfo.zDbName);
		if (m_dbInfo.zDbName[0] == NULL)	return CNFG_ERR;
	}

	{
		CUtil::GetConfig(getFileName(), SEC_DB_INFO, TEXT("DB_POOL_CNT"), m_zMsg);
		if (m_zMsg[0] == NULL)	return CNFG_ERR;
		m_dbInfo.nPoolCnt = atoi(m_zMsg);
	}
	return CNFG_SUCC;
}

VOID CConfig::DeInitialize()
{
	
}

BOOL CConfig::Is_Updated(char* pzSec)
{
	char zUpdated[128] = { 0 };
	CUtil::GetConfig(getFileName(), pzSec, DEF_UPDATED_KEY, zUpdated);
	return (zUpdated[0] == 'Y');
}

char* CConfig::getVal(char* pzSec, char* pzKey, _Out_ char* pzData)
{
	CUtil::GetConfig(getFileName(), pzSec, pzKey, pzData);
	if (pzData[0] == NULL)
	{
		return NULL;
	}
	return pzData;
}

EN_CNFG_RET	CConfig::ReLoad_ConfigInfo(BOOL bInit)
{
	EN_CNFG_RET ret1, ret2, ret3;
	ret1 = ReLoad_AppConfig(bInit);
	ret2 = CNFG_SUCC;	// ReLoad_ServersInfo(bInit);
	ret3 = ReLoad_DBInfo(bInit);

	if (ret1 == CNFG_ERR || ret2 == CNFG_ERR || ret3 == CNFG_ERR)
		return CNFG_ERR;

	if (ret1 == CNFG_SUCC || ret2 == CNFG_SUCC || ret3 == CNFG_SUCC)
		return CNFG_SUCC;

	return CNFG_IDLE;
}


//EN_CNFG_RET CConfig::ReLoad_ServersInfo(BOOL bInit)
//{
//	if (bInit == FALSE && !Is_Updated(SEC_SERVERS_INFO))
//		return CNFG_IDLE;
//
//	if (!CUtil::SetConfigValue(getFileName(), SEC_SERVERS_INFO, DEF_UPDATED_KEY, "N"))
//		return CNFG_ERR;
//
//	ZeroMemory(&m_serversInfo, sizeof(m_serversInfo));
//
//	{
//		CUtil::GetConfig(getFileName(), SEC_SERVERS_INFO, TEXT("RELAYSVR_IP"), m_serversInfo.zRelaySvrIp);
//		if (m_serversInfo.zRelaySvrIp[0] == NULL)	return CNFG_ERR;
//	}
//
//	{
//		CUtil::GetConfig(getFileName(), SEC_SERVERS_INFO, TEXT("RELAYSVR_PORT"), m_zMsg);
//		if (m_zMsg[0] == NULL)	return CNFG_ERR;
//		m_serversInfo.nRelaySvrPort = atoi(m_zMsg);
//	}
//
//	{
//		CUtil::GetConfig(getFileName(), SEC_SERVERS_INFO, TEXT("DATASVR_IP"), m_serversInfo.zDataSvrIp);
//		if (m_serversInfo.zDataSvrIp[0] == NULL)	return CNFG_ERR;
//	}
//
//	{
//		CUtil::GetConfig(getFileName(), SEC_SERVERS_INFO, TEXT("DATASVR_PORT"), m_zMsg);
//		if (m_zMsg[0] == NULL)	return CNFG_ERR;
//		m_serversInfo.nDataSvrPort = atoi(m_zMsg);
//	}
//	
//
//	return CNFG_SUCC;
//}
//
