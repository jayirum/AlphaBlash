
#define ALPHA_LOGIN_EXPORTS


#include "../Common/TcpClient.h"
#include "TrInterface.h"
#include "../Common/Util.h"
#include "../Common/Log.h"
#include "../Common/IRExcept.h"
#include "../Common/AlphaProtocolUni.h"

CLog	g_log;
bool	g_bLog;

bool IsLogging()
{
	return g_bLog;
}


CTcpClient	g_tcp;

int LoginAPI_UniTest(_Out_ wchar_t* pwzMsg)
{
	//MessageBoxW(NULL, pwzMsg, TEXT("test"), 64);
	_tcscpy(pwzMsg, TEXT("저것은 유니코드 테스트 입니다. That is unicode test"));
	return ERR_OK;
}

int LoginAPI_Init(char* pzConfigDir, _Out_ char* pzErrMsg)
{
	wchar_t wzIp[128] = { 0, };
	int nPort, nSendTimeout, nRecvTimeout;

	bool ret = Get_SvrInfo(pzConfigDir, wzIp, &nPort, &nSendTimeout, &nRecvTimeout);
	if (ret == false) {
		sprintf(pzErrMsg, "Read Config Error" );
		return E_READ_CONFIG;
	}

	//g_log.OpenLog(pzConfigDir, "LoginEngine");
	//g_log.log("Initialzie");
		
	ret = g_tcp.Initialize(wzIp, nPort, nSendTimeout, nRecvTimeout);
	if (ret == false)
	{
		sprintf(pzErrMsg, "TR Socket Initialize(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}

	char zIp[128] = { 0, };	U2A(wzIp, zIp);
	sprintf(pzErrMsg, "Connect OK(IP:%s)(Prt:%d)(SendTimeout:%d)(RecvTimeout:%d)", zIp, nPort, nSendTimeout, nRecvTimeout);
	return ERR_OK;
}


bool Get_SvrInfo(char* pzDir, _Out_ wchar_t* pzIp, _Out_ int* pnPort, _Out_ int* pnSendTimeout, _Out_ int* pnRecvTimeout)
{
	wchar_t zFileName[_MAX_PATH] = { 0, };
	__ALPHA::ComposeEAConfigFileName(pzDir, zFileName);
	

	wchar_t wzVal[128] = { 0, };

	// IP
	if(CUtil::GetConfig(zFileName, TEXT("LOGINSERVER_INFO"), TEXT("IP"), wzVal) ==NULL)
	{
		return false;
	}
	_tcscpy(pzIp, wzVal);

	// PORT
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("LOGINSERVER_INFO"), TEXT("PORT"), wzVal) == NULL)
	{
		return false;
	}
	*pnPort = _ttoi(wzVal);

	// SENDTIMEOUT
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("LOGINSERVER_INFO"), TEXT("SENDTIMEOUT"), wzVal) == NULL)
	{
		return false;
	}
	*pnSendTimeout = _ttoi(wzVal);

	// RECVTIMEOUT
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("LOGINSERVER_INFO"), TEXT("RECVTIMEOUT"), wzVal) == NULL)
	{
		return false;
	}
	*pnRecvTimeout = _ttoi(wzVal);

	return true;
}



void LoginAPI_DeInit()
{
	g_tcp.DeInitialize();
}

void LoginAPI_Disconnect()
{
	g_tcp.Disconnect();
}



int LoginAPI_SendData(char* pSendData, int nSendSize, _Out_ char* pzErrMsg)
{
	int nSentSize = g_tcp.SendData(pSendData, nSendSize);
	if (nSentSize <0 )
	{
		sprintf(pzErrMsg, "Send(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}
	
	return ERR_OK;
}

int LoginAPI_Login_Master(char* pzUserID, char* pzPwd, char* pzMCTp, char* pzMT4Acc, char* pzBroker, char* pzLiveDemo,
	_Out_ char* pzRelayIp, _Out_ int* pnRelayPort, _Out_ int* pnTrPort, _Out_ char* pzNickName, _Out_ char* pzWebUrl,
	_Out_ char* pzErrMsg)
{
	char zTime[32];
	char zSendBuff[256];
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pzUserID);
	set.SetVal(FDS_USER_PASSWORD, pzPwd);
	set.SetVal(FDS_MASTERCOPIER_TP, "M");
	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);
	set.SetVal(FDS_BROKER, pzBroker);
	set.SetVal(FDS_LIVEDEMO, pzLiveDemo);

	int nLen = set.Complete(zSendBuff);
	if (nLen < 0)
		return ERR_UNDEFINED;

	int nSentLen = g_tcp.SendData(zSendBuff, nLen);
	if (nSentLen<0)
	{
		sprintf(pzErrMsg, "[SendData error]%s", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}

	char zRecvBuf[1024] = { 0, };
	nLen = g_tcp.RecvData(zRecvBuf, sizeof(zRecvBuf));
	if (nLen <= 0)
	{
		sprintf(pzErrMsg, "[RecvData error]%s", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}
	
	CProtoGet protoGet;
	string sCode;
	string sRelayIp, sRelayPort, sNickName, sMasterID, sMasterAcc, sMasterTickets, sTrPort;
	int nRsltCode;

	protoGet.Parsing(zRecvBuf, nLen);

	try
	{
		ASSERT_BOOL2(protoGet.GetVal(FDN_RSLT_CODE, &nRsltCode), E_NO_FIELD, TEXT("Receive data but there is no Result Code"));
		if (nRsltCode != ERR_OK)
		{
			sprintf(pzErrMsg, "Rslt Error:%d", nRsltCode);
			return nRsltCode;
		}

		ASSERT_BOOL2(protoGet.GetCode(sCode), E_NOCODE, TEXT("Receive data but there is no Code"));
		ASSERT_BOOL2(protoGet.GetVal(FDS_RELAY_IP, &sRelayIp), E_NO_FIELD, TEXT("FDS_RELAY_IP is not in the packet"));
		ASSERT_BOOL2(protoGet.GetVal(FDS_RELAY_PORT, &sRelayPort), E_NO_FIELD, TEXT("FDS_RELAY_PORT is not in the packet"));
		ASSERT_BOOL2(protoGet.GetVal(FDS_TR_PORT, &sTrPort), E_NO_FIELD, TEXT("FDS_TR_PORT is not in the packet"));
		ASSERT_BOOL2(protoGet.GetVal(FDS_USER_NICK_NM, &sNickName), E_NO_FIELD, TEXT("FDS_USER_NICK_NM is not in the packet"));
		protoGet.GetVal(FDS_USERID_MASTER, &sMasterID);
		protoGet.GetVal(FDS_ACCNO_MASTER, &sMasterAcc);
		protoGet.GetVal(FDS_ARRAY_TICKET, &sMasterTickets);
		protoGet.GetVal(FDS_WEBSITE_URL, pzWebUrl);

		strcpy(pzRelayIp, sRelayIp.c_str());
		*pnRelayPort = atoi(sRelayPort.c_str());
		*pnTrPort = atoi(sTrPort.c_str());
		strcpy(pzNickName, sNickName.c_str());
	}
	catch (CIRExcept& e)
	{
		strcpy(pzErrMsg,e.GetMsg());
		return e.GetCode();
	}
	return ERR_OK;
}


int LoginAPI_Login_Copier(char* pzUserID, char* pzPwd, char* pzMCTp, char* pzMT4Acc, char* pzBroker, char* pzLiveDemo,
	_Out_ char* pzRelayIp, _Out_ int* pnRelayPort, _Out_ int* pnTrPort, _Out_ char* pzNickName,
	_Out_ char* pzMasterID, _Out_ char* pzMasterAcc, _Out_ char* pzWebUrl,
	_Out_ char* pzErrMsg)
{
	*pzRelayIp = 0;
	*pnRelayPort = 0;
	*pzNickName = 0;
	*pzMasterID = 0;
	*pzMasterAcc = 0;
	*pzErrMsg = 0;

	char zTime[32];
	char zSendBuff[256];
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pzUserID);
	set.SetVal(FDS_USER_PASSWORD, pzPwd);
	set.SetVal(FDS_MASTERCOPIER_TP, "C");
	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);
	set.SetVal(FDS_BROKER, pzBroker);
	set.SetVal(FDS_LIVEDEMO, pzLiveDemo);

	int nLen = set.Complete(zSendBuff);
	int nSentLen;
	if (g_tcp.SendData(zSendBuff, nLen, (int*)&nSentLen) < 0)
	{
		sprintf(pzErrMsg, "Send Error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}
	char zRecvBuf[1024] = { 0, };
	nLen = g_tcp.RecvData(zRecvBuf, sizeof(zRecvBuf));
	if (nLen <=0 )
	{
		sprintf(pzErrMsg, "Recv Error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}

	CProtoGet protoGet;
	string sCode;
	string sRelayIp, sRelayPort, sNickName, sMasterID, sMasterAcc, sMasterTickets, sTrPort;
	int nRsltCode;

	//g_log.log("[RECV](%s)", zRecvBuf);
	protoGet.Parsing(zRecvBuf, nLen);

	
	try
	{
		ASSERT_BOOL2(protoGet.GetVal(FDN_RSLT_CODE, &nRsltCode), E_NO_FIELD, TEXT("Receive data but there is no Result Code"));
		if (nRsltCode != ERR_OK)
		{
			sprintf(pzErrMsg, "Rslt Error:%d", nRsltCode);
			//g_log.log("After GetVal-0:%s", pzErrMsg);
			return nRsltCode;
		}

		ASSERT_BOOL2(protoGet.GetCode(sCode), E_NOCODE, TEXT("Receive data but there is no Code"));
		ASSERT_BOOL2(protoGet.GetVal(FDS_RELAY_IP, &sRelayIp), E_NO_FIELD, TEXT("FDS_RELAY_IP is not in the packet"));
		ASSERT_BOOL2(protoGet.GetVal(FDS_RELAY_PORT, &sRelayPort), E_NO_FIELD, TEXT("FDS_RELAY_PORT is not in the packet"));
		ASSERT_BOOL2(protoGet.GetVal(FDS_TR_PORT, &sTrPort), E_NO_FIELD, TEXT("FDS_TR_PORT is not in the packet"));
		ASSERT_BOOL2(protoGet.GetVal(FDS_USER_NICK_NM, &sNickName), E_NO_FIELD, TEXT("FDS_USER_NICK_NM is not in the packet"));

		protoGet.GetVal(FDS_USERID_MASTER, &sMasterID);
		protoGet.GetVal(FDS_ACCNO_MASTER, &sMasterAcc);
		protoGet.GetVal(FDS_ARRAY_TICKET, &sMasterTickets);
		protoGet.GetVal(FDS_WEBSITE_URL, pzWebUrl);

		strcpy(pzRelayIp, sRelayIp.c_str());
		*pnRelayPort = atoi(sRelayPort.c_str());
		*pnTrPort = atoi(sTrPort.c_str());
		strcpy(pzNickName, sNickName.c_str());
		strcpy(pzMasterID, sMasterID.c_str());
		strcpy(pzMasterAcc, sMasterAcc.c_str());
	}
	catch (CIRExcept & e)
	{
		strcpy(pzErrMsg, e.GetMsg());
		return e.GetCode();
	}
	return ERR_OK;
}



//
//void LoginAPI_ServerInfo(char* pzServerIp, int nServerPort)
//{
//	g_tcp.SetSvrInfo(pzServerIp, nServerPort);
//}


void LoginAPI_GetMsg(char* pzMsg)
{
	strcpy(pzMsg, g_tcp.GetMsg());
}

//bool LoginAPI_HasErrHappened(char* pzMsg)
//{
//	if (g_tcpThread.Has_ErrorHappened(pzMsg) == TRUE)
//		return true;
//
//	return false;
//}

bool LoginAPI_IsConnected()
{
	return g_tcp.IsConnected();
}