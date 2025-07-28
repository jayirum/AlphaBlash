#pragma warning(disable:4996)
#pragma warning(disable:26812)
#pragma warning(disable:6386)
#pragma warning(disable:6101)

#include "CTelegramSender.h"
#include "../CommonAnsi/Util.h"

CTelegramSender::CTelegramSender()
{
	m_curl = NULL;
	
	ZeroMemory(m_zUrl, sizeof(m_zUrl));
	ZeroMemory(m_zToken, sizeof(m_zToken));
	ZeroMemory(m_zChatID, sizeof(m_zChatID));
	ZeroMemory(m_zMsg, sizeof(m_zMsg));
}

CTelegramSender::~CTelegramSender()
{
	DeInitialize();
}


BOOL CTelegramSender::Initialize()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	return TRUE;
}

VOID CTelegramSender::DeInitialize()
{
	curl_global_cleanup();
}

VOID CTelegramSender::SetTelegramInfo(const char* pzUrl, const char* pzToken, const char* pzChatID)
{
	strcpy(m_zUrl, pzUrl);
	CUtil::RTrim(m_zUrl, strlen(m_zUrl));
	
	strcpy(m_zToken, pzToken);
	if (m_zToken[strlen(m_zToken) - 1] == '/')
		m_zToken[strlen(m_zToken) - 1] = 0x00;

	strcpy(m_zChatID, pzChatID);

}

//curl_easy_setopt(curl, CURLOPT_URL, "https://api.telegram.org/bot1856600757:AAE7xk6woeFbAfflTtD0RgceSJKoq54DqKU/sendMessage?chat_id=-510382981&text=This message has sent from vc++");
	
BOOL CTelegramSender::SendTelegram(const char* pzSendData)
{
	m_curl = curl_easy_init();
	if (m_curl == NULL)
	{
		sprintf(m_zMsg, "cUrl init error");
		return FALSE;
	}

	CURLcode res;
	char zBuffer[1024];
	sprintf(zBuffer, "%s/bot%s/sendMessage?chat_id=%s&text=%s", m_zUrl, m_zToken, m_zChatID, pzSendData);
	curl_easy_setopt(m_curl, CURLOPT_URL, zBuffer);
	
	/* Now specify the POST data */
	curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, FALSE);

	res = curl_easy_perform(m_curl);

	/* Check for errors */
	if (res != CURLE_OK)
	{
		sprintf(m_zMsg, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
	}
	if (m_curl)	curl_easy_cleanup(m_curl);
	m_curl = NULL;

	return (res == CURLE_OK);
}