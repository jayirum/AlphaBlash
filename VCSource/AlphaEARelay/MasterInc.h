#pragma once
#include "Main.h"

#define COPIER_ID	string
enum { EN_ONLINE, EN_OFFLINE };

struct SESSION_TM
{
	SOCKET	sock;
	DWORD	dwLastPing;
	int		nPingCnt;
	string	sAccNo;
	char	UserID[128];
	string	sNickNm;
	//BOOL	bMaster;
	string	sIPAddr;
};
