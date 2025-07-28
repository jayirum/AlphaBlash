
#define ALPHA_TR_EXPORTS


#include "../Common/TcpClient.h"
#include "TrInterface.h"
#include "../Common/Util.h"

#include "../Common/IRExcept.h"
#include "../Common/AlphaProtocolUni.h"

extern CLog	g_log;
bool	g_bLog;

bool IsLogging()
{
	return g_bLog;
}


CTcpClient g_tcp;

int TRAPI_Init(char* pzPath, char* pzServerIp, int nServerPort, _Out_ char* pOutMsg)
{
	int nSendTimeout;
	int nRecvTimeout;
	Get_TrSvrInfo(pzPath, &nSendTimeout, &nRecvTimeout);

	//g_log.OpenLog(pzPath, "TREngine");

	bool ret = Get_TrSvrInfo(pzPath, &nSendTimeout, &nRecvTimeout);
	if (ret == false) {
		sprintf(pOutMsg, "Failed to read config file");
		return E_READ_CONFIG;
	}
		
	ret = g_tcp.Initialize(pzServerIp, nServerPort, nSendTimeout, nRecvTimeout);
	if (ret == false)
	{
		sprintf(pOutMsg, g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}
	return ERR_OK;
}


bool Get_TrSvrInfo(char* pzDir, _Out_ int* pnSendTimeout, _Out_ int* pnRecvTimeout)
{
	TCHAR zFileName[_MAX_PATH] = { 0, };
	__ALPHA::ComposeEAConfigFileName(pzDir, zFileName);

	TCHAR wzVal[128] = { 0, };

	// SENDTIMEOUT
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("TRSERVER_INFO"), TEXT("SENDTIMEOUT"), wzVal) == NULL)
	{
		return false;
	}
	*pnSendTimeout = _ttoi(wzVal);
	
	ZeroMemory(wzVal, sizeof(wzVal));
	if (CUtil::GetConfig(zFileName, TEXT("TRSERVER_INFO"), TEXT("RECVTIMEOUT"), wzVal) == NULL)
	{
		return false;
	}
	*pnRecvTimeout = _ttoi(wzVal);

	return true;
}

void TRAPI_DeInit()
{
	g_tcp.DeInitialize();
}

void TRAPI_Disconnect()
{
	g_tcp.Disconnect();
}



int TRAPI_SendData(char* pSendData, int nSendSize, _Out_ char* pzErrMsg)
{
	char SendBuf[MAX_BUF] = { 0, };
	int nErrCode;
	if (g_tcp.SendData(pSendData, nSendSize, &nErrCode) <0 )
	{
		sprintf("tcp send error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}
	
	return ERR_OK;
}




int TRAPI_ConfigSymbol(char* pzMCTp, char* pzUserID, char* pzMT4Acc, _Out_ int* pnArrSize, _Out_ char* pArrSymbols, _Out_ char* pzErrMsg)
{
	*pnArrSize = 0;
	*pArrSymbols = 0;
	*pzErrMsg = 0;

	char zTime[32];
	char zSendBuff[256];
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_CONFIG_SYMBOL);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pzUserID);
	set.SetVal(FDS_MASTERCOPIER_TP, pzMCTp);
	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);

	int nLen = set.Complete(zSendBuff);
	int nSentLen;
	if (g_tcp.SendData(zSendBuff, nLen, &nSentLen) == false)
	{
		sprintf(pzErrMsg, "Send Error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}

	char zRecvBuf[1024] = { 0, };
	nLen = g_tcp.RecvData(zRecvBuf, sizeof(zRecvBuf));
	if (nLen<0)
	{
		sprintf(pzErrMsg, "Recv Error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}

	CProtoGet protoGet;
	string sCode;
	string sSymbols;
	int nRsltCode;
	int nSymbolArraySize = 0;

	protoGet.Parsing(zRecvBuf, nLen);

	try
	{
		ASSERT_BOOL2(protoGet.GetVal(FDN_RSLT_CODE, &nRsltCode), E_NO_FIELD, TEXT("Receive data but there is no Result Code"));
		if (nRsltCode != ERR_OK)
		{
			sprintf(pzErrMsg, "Rslt Error:%d", nRsltCode);
			return nRsltCode;
		}

		ASSERT_BOOL2(protoGet.GetCode(sCode), E_NO_CODE, TEXT("Receive data but there is no Code"));
		nSymbolArraySize = protoGet.GetValN(FDN_ARRAY_SIZE);
		*pnArrSize = nSymbolArraySize;

		if (nSymbolArraySize <= 0)
		{
			//char zDebug[4097] = { 0, };
			//protoGet.SeeAllData(zDebug);
			sprintf(pzErrMsg, "[%d]Need to set symbols configuration", nSymbolArraySize);
			return E_NO_SYMBOLS_CONFIG;
		}

		ASSERT_BOOL2(protoGet.GetVal(FDS_ARRAY_SYMBOL, &sSymbols), E_NO_FIELD, TEXT("FDS_ARRAY_SYMBOL is not in the packet"));

		strcpy(pArrSymbols, sSymbols.c_str());
	}
	catch (CIRExcept & e)
	{
		sprintf(pzErrMsg, e.GetMsg());
		return e.GetCode();
	}
	return ERR_OK;
}


int TRAPI_ConfigGeneral(char* pzMCTp, char* pzUserID, char* pzMT4Acc, _Out_ char* pzTotalPacket, _Out_ int* pnPacketSize, _Out_ char* pzErrMsg)
{
	*pzTotalPacket = 0;
	*pnPacketSize = 0;
	*pzErrMsg = 0;

	char zTime[32];
	char zSendBuff[256];
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_CONFIG_GENERAL);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pzUserID);
	set.SetVal(FDS_MASTERCOPIER_TP, pzMCTp);
	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);

	int nLen = set.Complete(zSendBuff);
	int nSentLen;
	if (g_tcp.SendData(zSendBuff, nLen, &nSentLen) == false)
	{
		sprintf(pzErrMsg, "Send Error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}
	char zRecvBuf[1024] = { 0, };
	nLen = g_tcp.RecvData(zRecvBuf, sizeof(zRecvBuf));
	if (nLen<0)
	{
		sprintf(pzErrMsg, "Recv Error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}

	*pnPacketSize = nLen;
	sprintf(pzTotalPacket, "%.*s", nLen, zRecvBuf);
	return ERR_OK;
}

int TRAPI_OpenOrders_Master(char* pzUserID, char* pzMT4Acc, _Out_ int* pnArrSize, _Out_ char* pArrTickets, _Out_ char* pzErrMsg)
{
	*pnArrSize = 0;
	*pArrTickets = 0;
	*pzErrMsg = 0;

	char zTime[32];
	char zSendBuff[256];
	CProtoSet	set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_OPEN_ORDERS);
	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);

	__ALPHA::Now(zTime);
	set.SetVal(FDS_TM_HEADER, zTime);
	set.SetVal(FDS_USERID_MINE, pzUserID);
	set.SetVal(FDS_MASTERCOPIER_TP, "M");
	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);

	int nLen = set.Complete(zSendBuff);
	int nSentLen;
	if (g_tcp.SendData(zSendBuff, nLen, &nSentLen) == false)
	{
		sprintf(pzErrMsg, "Send Error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}
	char zRecvBuf[1024] = { 0, };
	nLen = g_tcp.RecvData(zRecvBuf, sizeof(zRecvBuf));
	if (nLen<0)
	{
		sprintf(pzErrMsg, "Recv Error(%s)", g_tcp.GetMsg());
		return g_tcp.GetErrCode();
	}

	CProtoGet protoGet;
	string sCode;
	string sTickets;
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

		ASSERT_BOOL2(protoGet.GetCode(sCode), E_NO_CODE, TEXT("Receive data but there is no Code"));
		*pnArrSize = protoGet.GetValN(FDN_ARRAY_SIZE);

		if(*pnArrSize>0)
			ASSERT_BOOL2(protoGet.GetVal(FDS_ARRAY_TICKET, &sTickets), E_NO_FIELD, TEXT("FDS_ARRAY_TICKET is not in the packet"));

		strcpy(pArrTickets, sTickets.c_str());
	}
	catch (CIRExcept & e)
	{
		sprintf(pzErrMsg, e.GetMsg());
		return e.GetCode();
	}
	return ERR_OK;
}



//
//void TRAPI_ServerInfo(char* pzServerIp, long nServerPort)
//{
//	g_tcp.SetSvrInfo(pzServerIp, nServerPort);
//}


void TRAPI_GetMsg(char* pzMsg)
{
	strcpy(pzMsg, g_tcp.GetMsg());
}

//bool TRAPI_HasErrHappened(char* pzMsg)
//{
//	if (g_tcpThread.Has_ErrorHappened(pzMsg) == TRUE)
//		return true;
//
//	return false;
//}

bool TRAPI_IsConnected()
{
	return g_tcp.IsConnected();
}


//long TRAPI_Login_Master(char* pzUserID, char* pzPwd, char* pzMCTp, char* pzMT4Acc, char* pzBroker, char* pzLiveDemo,
//	_Out_ char* pzRelayIp, _Out_ long* pnRelayPort, _Out_ char* pzNickName, _Out_ char* pzWebUrl,
//	_Out_ char* pzErrMsg)
//{
//	char zTime[32];
//	char zSendBuff[256];
//	CProtoSet	set;
//	set.Begin();
//	set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON);
//	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);
//
//	__ALPHA::Now(zTime);
//	set.SetVal(FDS_TM_HEADER, zTime);
//	set.SetVal(FDS_USERID_MINE, pzUserID);
//	set.SetVal(FDS_USER_PASSWORD, pzPwd);
//	set.SetVal(FDS_MASTERCOPIER_TP, "M");
//	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);
//	set.SetVal(FDS_BROKER, pzBroker);
//	set.SetVal(FDS_LIVEDEMO, pzLiveDemo);
//
//	long nLen = set.Complete(zSendBuff);
//	long nSentLen;
//	if (g_tcp.SendData(zSendBuff, nLen, &nSentLen) == false)
//	{
//		sprintf(pzErrMsg, "Send Error(%s)", g_tcp.GetMsg());
//		return ERR_SENDINFO_TO_SVR;
//	}
//	char zRecvBuf[1024] = { 0, };
//	if (g_tcp.RecvData(zRecvBuf, sizeof(zRecvBuf), &nLen) == false)
//	{
//		sprintf(pzErrMsg, "Recv Error(%s)", g_tcp.GetMsg());
//		return ERR_SVR_RECV_ERROR;
//	}
//
//	CProtoGet protoGet;
//	string sCode;
//	string sRelayIp, sRelayPort, sNickName, sMasterID, sMasterAcc, sMasterTickets;
//	int nRsltCode;
//
//	protoGet.Parsing(zRecvBuf, nLen);
//
//	try
//	{
//		ASSERT_BOOL2(protoGet.GetVal(FDN_RSLT_CODE, &nRsltCode), ERR_NO_FIELD, "Receive data but there is no Result Code");
//		if (nRsltCode != ERR_OK)
//		{
//			sprintf(pzErrMsg, "Rslt Error:%d", nRsltCode);
//			return nRsltCode;
//		}
//
//		ASSERT_BOOL2(protoGet.GetCode(sCode), ERR_NO_CODE, "Receive data but there is no Code");
//		ASSERT_BOOL2(protoGet.GetVal(FDS_RELAY_IP, &sRelayIp), ERR_NO_FIELD, "FDS_RELAY_IP is not in the packet");
//		ASSERT_BOOL2(protoGet.GetVal(FDS_RELAY_PORT, &sRelayPort), ERR_NO_FIELD, "FDS_RELAY_PORT is not in the packet");
//		ASSERT_BOOL2(protoGet.GetVal(FDS_USER_NICK_NM, &sNickName), ERR_NO_FIELD, "FDS_USER_NICK_NM is not in the packet");
//		protoGet.GetVal(FDS_USERID_MASTER, &sMasterID);
//		protoGet.GetVal(FDS_ACCNO_MASTER, &sMasterAcc);
//		protoGet.GetVal(FDS_ARRAY_TICKET, &sMasterTickets);
//		protoGet.GetVal(FDS_WEBSITE_URL, pzWebUrl);
//
//		strcpy(pzRelayIp, sRelayIp.c_str());
//		*pnRelayPort = atoi(sRelayPort.c_str());
//		strcpy(pzNickName, sNickName.c_str());
//	}
//	catch (CIRExcept & e)
//	{
//		sprintf(pzErrMsg, e.GetMsg());
//		return e.GetCode();
//	}
//	return ERR_OK;
//}
//
//
//long TRAPI_Login_Copier(char* pzUserID, char* pzPwd, char* pzMCTp, char* pzMT4Acc, char* pzBroker, char* pzLiveDemo,
//	_Out_ char* pzRelayIp, _Out_ long* pnRelayPort, _Out_ char* pzNickName,
//	_Out_ char* pzMasterID, _Out_ char* pzMasterAcc, _Out_ char* pzAutoKey,
//	_Out_ char* pzErrMsg)
//{
//	*pzRelayIp = 0;
//	*pnRelayPort = 0;
//	*pzNickName = 0;
//	*pzMasterID = 0;
//	*pzMasterAcc = 0;
//	*pzErrMsg = 0;
//
//	char zTime[32];
//	char zSendBuff[256];
//	CProtoSet	set;
//	set.Begin();
//	set.SetVal(FDS_CODE, __ALPHA::CODE_LOGON);
//	set.SetVal(FDS_COMMAND, __ALPHA::TP_COMMAND);
//
//	__ALPHA::Now(zTime);
//	set.SetVal(FDS_TM_HEADER, zTime);
//	set.SetVal(FDS_USERID_MINE, pzUserID);
//	set.SetVal(FDS_USER_PASSWORD, pzPwd);
//	set.SetVal(FDS_MASTERCOPIER_TP, "C");
//	set.SetVal(FDS_ACCNO_MINE, pzMT4Acc);
//	set.SetVal(FDS_BROKER, pzBroker);
//	set.SetVal(FDS_LIVEDEMO, pzLiveDemo);
//
//	long nLen = set.Complete(zSendBuff);
//	long nSentLen;
//	if (g_tcp.SendData(zSendBuff, nLen, &nSentLen) == false)
//	{
//		sprintf(pzErrMsg, "Send Error(%s)", g_tcp.GetMsg());
//		return ERR_SENDINFO_TO_SVR;
//	}
//	char zRecvBuf[1024] = { 0, };
//	if (g_tcp.RecvData(zRecvBuf, sizeof(zRecvBuf), &nLen) == false)
//	{
//		sprintf(pzErrMsg, "Recv Error(%s)", g_tcp.GetMsg());
//		return ERR_SVR_RECV_ERROR;
//	}
//
//	CProtoGet protoGet;
//	string sCode;
//	string sRelayIp, sRelayPort, sNickName, sMasterID, sMasterAcc, sMasterTickets;
//	int nRsltCode;
//
//	protoGet.Parsing(zRecvBuf, nLen);
//
//
//	try
//	{
//		ASSERT_BOOL2(protoGet.GetVal(FDN_RSLT_CODE, &nRsltCode), ERR_NO_FIELD, "Receive data but there is no Result Code");
//		if (nRsltCode != ERR_OK)
//		{
//			sprintf(pzErrMsg, "Rslt Error:%d", nRsltCode);
//			return nRsltCode;
//		}
//
//		ASSERT_BOOL2(protoGet.GetCode(sCode), ERR_NO_CODE, "Receive data but there is no Code");
//		ASSERT_BOOL2(protoGet.GetVal(FDS_RELAY_IP, &sRelayIp), ERR_NO_FIELD, "FDS_RELAY_IP is not in the packet");
//		ASSERT_BOOL2(protoGet.GetVal(FDS_RELAY_PORT, &sRelayPort), ERR_NO_FIELD, "FDS_RELAY_PORT is not in the packet");
//		ASSERT_BOOL2(protoGet.GetVal(FDS_USER_NICK_NM, &sNickName), ERR_NO_FIELD, "FDS_USER_NICK_NM is not in the packet");
//		protoGet.GetVal(FDS_USERID_MASTER, &sMasterID);
//		protoGet.GetVal(FDS_ACCNO_MASTER, &sMasterAcc);
//		protoGet.GetVal(FDS_ARRAY_TICKET, &sMasterTickets);
//		protoGet.GetVal(FDS_SITEAUTOLOGON_KEY, pzAutoKey);
//
//		strcpy(pzRelayIp, sRelayIp.c_str());
//		*pnRelayPort = atoi(sRelayPort.c_str());
//		strcpy(pzNickName, sNickName.c_str());
//		strcpy(pzMasterID, sMasterID.c_str());
//		strcpy(pzMasterAcc, sMasterAcc.c_str());
//	}
//	catch (CIRExcept & e)
//	{
//		sprintf(pzErrMsg, e.GetMsg());
//		return e.GetCode();
//	}
//	return ERR_OK;
//}