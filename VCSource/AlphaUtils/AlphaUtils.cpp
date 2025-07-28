// AlphaUtils.cpp : Defines the exported functions for the DLL application.
//

#define _ALPHA_UTILS_EXPORTS

#include "AlphaUtils.h"
#include "../CommonAnsi/ADOFunc.h"
#include "../CommonAnsi/Log.h"
#include "../CommonAnsi/Util.h"

//CLog		g_log;
//map<_tstring, _tstring>	g_mapMsg;

CDBPoolAdo* _pDBPool = NULL;


int	Alpha_DBOpen(char* pzSvrIpPort, char* pzUser, char* pzPwd, char* pzDBName, _Out_ char* pzMsg)
{
	if (_pDBPool == NULL)
	{
		_pDBPool = new CDBPoolAdo(pzSvrIpPort, pzUser, pzPwd, pzDBName);
	}
	if (!_pDBPool->Init(1))
	{
		sprintf(pzMsg, "Failed to open DB(%s, %s, %s)", pzSvrIpPort, pzUser, pzPwd);
		return -1;
	}
	return 0;
}

int	Alpha_DBExec(char* pzQ, _Out_ char* pzMsg)
{
	if (_pDBPool == NULL)
	{
		sprintf(pzMsg, "DB must be opened first");
		return E_DB_NOT_OPENED;
	}

	CDBHandlerAdo db(_pDBPool->Get());
	if (FALSE == db->ExecQuery(pzQ))
	{
		sprintf(pzMsg, "Failed to Exec.(%s)", db->GetError());
		return -1;
	}
	
	db->Close();

	return 0;
}

int	Alpha_DBClose()
{
	if (_pDBPool) delete _pDBPool;
	_pDBPool = NULL;

	return 0;
}


void AlphaUtils_OpenHomepage(_In_ char* pzWebsiteUrl)
{
	ShellExecuteA(NULL, "open", pzWebsiteUrl, NULL, NULL, 1);
}




int GetMsg(_In_ wchar_t* pwzFileName, _In_ char* pzErrCode, _Out_ wchar_t* pwzMsg)
{

	//if (g_mapMsg.size() == 0)
	//{
	//	if (CUtil::Load_MsgFile(pwzFileName, g_mapMsg) == false)
	//		return E_MSG_FILE;
	//}

	//wchar_t wzErrCode[512] = { 0, };
	//A2U(pzErrCode, wzErrCode);
	//_tstring wsCode = wzErrCode;
	//map<_tstring, _tstring>::iterator it = g_mapMsg.find(wsCode);
	//if (it == g_mapMsg.end())
	//{
	//	_stprintf(pwzMsg, TEXT("Unknown Error Code(%s)"), wzErrCode);
	//	return E_UNKNOWN_ERR_CODE;
	//}
	//_stprintf(pwzMsg, TEXT("[%s]%s"), wzErrCode, (*it).second.c_str());
	return ERR_OK;
}

int AlphaUtils_Get_MsgEx(_In_ char* pzDir, _In_ char* pzErrCode, _In_ char* pzLanguage, _Out_ wchar_t* pwzMsg)
{
	//char zFileName[_MAX_PATH] = { 0, };
	//char zFullName[_MAX_PATH] = { 0, };
	//if (strcmp(pzLanguage, __ALPHA::MT4_LANG_KOR) == 0)
	//	strcpy(zFileName, MSG_FILE_KO);
	////else if (strcmp(pzLanguage, __ALPHA::MT4_LANG_JAP) == 0)
	////	strcpy(zFileName, MSG_FILE_JAP);
	////else if (strcmp(pzLanguage, __ALPHA::MT4_LANG_CHINA) == 0)
	////	strcpy(zFileName, MSG_FILE_CHINA);
	//else
	//	strcpy(zFileName, MSG_FILE_EN);


	//if (pzDir[strlen(pzDir) - 1] == '\\')
	//	sprintf(zFullName, "%s%s", pzDir, zFileName);
	//else
	//	sprintf(zFullName, "%s\\%s", pzDir, zFileName);

	//wchar_t wzFileName[_MAX_PATH] = { 0, };
	//A2U(zFileName, wzFileName);

	return 0;// GetMsg(wzFileName, pzErrCode, pwzMsg);
}

int AlphaUtils_Get_Msg(_In_ char* pzDir, _In_ char* pzErrCode, _Out_ wchar_t* pwzMsg)
{
	//wchar_t wzFileName[_MAX_PATH] = { 0, };
	//char zFileName[_MAX_PATH] = { 0, };

	//if (pzDir[strlen(pzDir) - 1] == '\\') {
	//	sprintf(zFileName, "%s%s", pzDir, MSG_FILE_EN);
	//}
	//else {
	//	sprintf(zFileName, "%s\\%s", pzDir, MSG_FILE_EN);
	//}

	//A2U(zFileName, wzFileName);

	return 0;	// GetMsg(wzFileName, pzErrCode, pwzMsg);

}
