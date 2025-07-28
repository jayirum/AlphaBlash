#pragma once

#include <curl/curl.h>

class CTelegramSender
{
public:
	CTelegramSender();
	~CTelegramSender();

public:
	BOOL	Initialize();
	VOID	DeInitialize();

	VOID	SetTelegramInfo(const char* pzUrl, const char* pzToken, const char* pzChatID);
	BOOL	SendTelegram(const char* pzSendData);
	char* GetMsg() { return m_zMsg; }
private:
	CURL*	m_curl;
	char	m_zUrl[1024];
	char	m_zToken[256];
	char	m_zChatID[256];
	char	m_zMsg[1024];
};

