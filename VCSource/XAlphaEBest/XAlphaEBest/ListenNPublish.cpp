
#include <set>
#include "ListenNPublish.h"
#include "OVC.h"
#include "fc0.h"
#include "nc0.h"
#include "../../CommonAnsi/IRUM_Common.h"
#include "../../CommonAnsi/LogMsg.h"
#include "../../CommonAnsi/MemPool.h"
#include "../../CommonAnsi/Util.h"

extern CLogMsg	g_log;
extern char		g_zConfig[_MAX_PATH];
extern CMemPool		g_memPool;


CListenNPublsh::CListenNPublsh()
{
	m_sockListen = INVALID_SOCKET;
	m_hwsa		= NULL;
	m_bContinue = FALSE;
	m_nThreadCnt = 0;

	InitializeCriticalSection(&m_csThreadId);
}


CListenNPublsh::~CListenNPublsh()
{
	UnInitialize();
}

VOID CListenNPublsh::UnInitialize()
{
	CloseListenSock();
	m_bContinue = FALSE;
	DeleteCriticalSection(&m_csThreadId);
	
}

VOID CListenNPublsh::CloseListenSock()
{
	if (m_sockListen) {
		shutdown(m_sockListen, SD_SEND);
		closesocket(m_sockListen);
		m_sockListen = INVALID_SOCKET;
	}
	if (m_hwsa) {
		CloseHandle(m_hwsa);
		m_hwsa = NULL;
	}
}


BOOL CListenNPublsh::Initialize()
{
	char zVal[32];
	CUtil::GetConfig(g_zConfig, "LISTEN_INFO", "PORT", zVal);
	m_nListenPort = atoi(zVal);

	WORD	wVersionRequired;
	WSADATA	wsaData;

	//// WSAStartup
	wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		g_log.log(ERR, "WSAStartup error ");
		return FALSE;
	}

	//DumpWsaData(&wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		g_log.log(ERR, "RequiredVersion not Usable ");
		return FALSE;
	}


	m_sockListen = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockListen == INVALID_SOCKET)
	{
		SetSockErrMsg("socket create");
		return FALSE;
	}

	m_sock_addr.sin_family = AF_INET;
	m_sock_addr.sin_port = htons(m_nListenPort);
	m_sock_addr.sin_addr.s_addr = INADDR_ANY;

	BOOL opt = TRUE;
	int optlen = sizeof(opt);
	setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (const char far*) & opt, optlen);

	if (::bind(m_sockListen, (struct sockaddr*) & m_sock_addr, sizeof(m_sock_addr)) == SOCKET_ERROR)
	{
		SetSockErrMsg("bind");
		return FALSE;
	}

	if (listen(m_sockListen, SOMAXCONN) == SOCKET_ERROR)
	{
		SetSockErrMsg("listen");
		return FALSE;
	}

	m_hwsa = WSACreateEvent();
	if (WSAEventSelect(m_sockListen, m_hwsa, FD_ACCEPT)) {
		SetSockErrMsg("WSAEventSelect");
		return FALSE;
	}


	// client thread cnt	
	CUtil::GetConfig(g_zConfig, "LISTEN_INFO", "THREAD_CNT", zVal);
	m_nThreadCnt = atoi(zVal);
	int i = 0;
	for (i = 0; i < m_nThreadCnt; i++)
	{
		unsigned id;
		HANDLE h = (HANDLE)_beginthreadex(NULL, 0, &SendThread, this, 0, &id);

		EnterCriticalSection(&m_csThreadId);
		m_lstThreadId.push_back(id);
		LeaveCriticalSection(&m_csThreadId);
		CloseHandle(h);
	}


	m_bContinue = TRUE;

	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, &AcptThread, this, 0, &m_unListenThread);

	return TRUE;
}

unsigned WINAPI CListenNPublsh::AcptThread(LPVOID lp)
{
	CListenNPublsh* p = (CListenNPublsh*)lp;

	DWORD dw = 0;
	while (p->m_bContinue) 
	{
		dw = WSAWaitForMultipleEvents(1, &p->m_hwsa, TRUE, 100, FALSE);
		if (dw != WSA_WAIT_EVENT_0)
		{
			if (dw == WSA_WAIT_TIMEOUT)		continue;
			else 
			{
				p->SetSockErrMsg("WaitFor");
				Sleep(5000);
				continue;
			}
		}
		WSAResetEvent(p->m_hwsa);

		SOCKADDR_IN	sinClient;
		int	sinSize = sizeof(sinClient);
		SOCKET client = accept(p->m_sockListen, (LPSOCKADDR)&sinClient, &sinSize);
		if (client == INVALID_SOCKET) {
			p->SetSockErrMsg("accept");
			Sleep(5000);
			continue;
		}
		g_log.log(INFO, "Accept client [%d]", client);

		std::list<UINT>::iterator it;
		for (it = p->m_lstThreadId.begin(); it != p->m_lstThreadId.end(); it++)
			PostThreadMessage(*it, WM_RECV_CLIENT, 0, (LPARAM)client);

	} // while (p->m_bContiue) 
	
	return 0;
}

VOID CListenNPublsh::SendData(UINT message, char* pData, int nDataLen)
{
	EnterCriticalSection(&m_csThreadId);
	if (m_lstThreadId.empty())
	{
		LeaveCriticalSection(&m_csThreadId);
		g_log.log(ERR, "가능한 스레드가 없습니다.");
		return;
	}

	UINT id = *m_lstThreadId.begin();
	m_lstThreadId.pop_front();
	m_lstThreadId.push_back(id);
	LeaveCriticalSection(&m_csThreadId);

	char* pNewData = g_memPool.get();
	memcpy(pNewData, pData, nDataLen);
	PostThreadMessage(id, message, nDataLen, (LPARAM)pNewData);
}

int CListenNPublsh::SetSockErrMsg(const char* pzMsg)
{
	int nErr = GetLastError();
	char msg[1024];
	CUtil::FormatErrMsg(nErr, msg);
	sprintf(m_zMsg, "[%s][%d]%s", pzMsg, nErr, msg);
	g_log.log(LOGTP_ERR, "%s", m_zMsg);
	return nErr;
}

unsigned WINAPI CListenNPublsh::SendThread(LPVOID lp)
{
	g_log.log(INFO, "[SendThread])%d)", GetCurrentThreadId());
	CListenNPublsh* pThis = (CListenNPublsh*)lp;
	std::set<SOCKET> setSock;

	while (pThis->m_bContinue)
	{
		Sleep(1);

		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_RECV_CLIENT)
			{
				SOCKET client = (SOCKET)(SOCKET)msg.lParam;
				setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, (char*)&SEND_TIMEOUT_MS, sizeof(SEND_TIMEOUT_MS));
				setSock.insert(client);
			}
			else
			{
				if (msg.message == WM_MD_OV_FUT)
				{
					pThis->Send_OverseasFut((char*)msg.lParam, &setSock);
				}
				else if (msg.message == WM_MD_KOSPI_FUT)
				{
					pThis->Send_KospiFut((char*)msg.lParam, &setSock);
				}
				else if (msg.message == WM_DIE)
				{
					return 0;
				}
				else
				{
					//TODO. logging
				}

				g_memPool.release((char*)msg.lParam);
			}
		}
	}

	return 0;
}



BOOL CListenNPublsh::Send_KospiFut(char* pEbestData, std::set<SOCKET>* setSock)
{
	if (setSock->empty())
		return TRUE;

	_XAlpha::TTICK stSend;
	ZeroMemory(&stSend, sizeof(stSend));
	FC0_OutBlock* pEbest = (FC0_OutBlock*)pEbestData;

	memcpy(stSend.Code, CODE_TICK_KOSPI, sizeof(stSend.Code));

	char zSymbol[32];
	sprintf(zSymbol, "%.*s", sizeof(pEbest->futcode), pEbest->futcode);
	CUtil::RTrim(zSymbol, strlen(zSymbol));
	memcpy(stSend.stk, zSymbol, strlen(zSymbol));

	char z[32];
	double dVal;
	
	//dVal = CUtil::Str2D(pEbest->change, sizeof(pEbest->change));
	//sprintf(z, "%.2f", dVal *0.01);
	//memcpy(stSend.gap, z, strlen(z));

	dVal = CUtil::Str2D(pEbest->price, sizeof(pEbest->price));
	sprintf(z, "%.2f", dVal * 0.01);
	memcpy(stSend.close, z, strlen(z));

	//dVal = CUtil::Str2D(pEbest->open, sizeof(pEbest->open));
	//sprintf(z, "%.2f", dVal * 0.01);
	//memcpy(stSend.open, z, strlen(z));

	//dVal = CUtil::Str2D(pEbest->high, sizeof(pEbest->high));
	//sprintf(z, "%.2f", dVal * 0.01);
	//memcpy(stSend.high, z, strlen(z));

	//dVal = CUtil::Str2D(pEbest->low, sizeof(pEbest->low));
	//sprintf(z, "%.2f", dVal * 0.01);
	//memcpy(stSend.low, z, strlen(z));

	//memcpy(stSend.vol, pEbest->volume, sizeof(pEbest->volume));
	//memcpy(stSend.amt, pEbest->value, sizeof(pEbest->value));
	memcpy(stSend.time, pEbest->chetime, sizeof(pEbest->chetime));

	if (pEbest->cgubun[0] == '1' || pEbest->cgubun[0] == '-')
		stSend.side[0] = SIDE_BUY;
	else
		stSend.side[0] = SIDE_SELL;

	//stSend.ydiffSign[0] = pEbest->sign[0];
	//memcpy(stSend.chgrate, pEbest->change, sizeof(pEbest->change));

	stSend.Enter[0] = DEF_EOL;

	SendToClient(&stSend, setSock);

	//g_log.log(INFO, "[KOSPI](%.11s)(%.5f)(%.11s)", stSend.stk, stSend.close, stSend.time);
	return TRUE;
}


BOOL CListenNPublsh::Send_OverseasFut(char* pEbestData, std::set<SOCKET>* setSock)
{
	if (setSock->empty())
		return TRUE;

	_XAlpha::TTICK stSend;
	ZeroMemory(&stSend, sizeof(stSend));
	OVC_OutBlock* pEbest = (OVC_OutBlock*)pEbestData;

	ZeroMemory(&stSend, sizeof(stSend));

	char zSymbol[32];

	memcpy(stSend.Code, CODE_TICK_OV, sizeof(stSend.Code));
	sprintf(zSymbol, "%.*s", sizeof(pEbest->symbol), pEbest->symbol);
	CUtil::RTrim(zSymbol, strlen(zSymbol));
	memcpy(stSend.stk, zSymbol, strlen(zSymbol));

	//memcpy(stSend.gap, pEbest->ydiffpr, sizeof(pEbest->ydiffpr));
	memcpy(stSend.close, pEbest->curpr, sizeof(pEbest->curpr));
	//memcpy(stSend.open, pEbest->open, sizeof(pEbest->open));
	//memcpy(stSend.high, pEbest->high, sizeof(pEbest->high));
	//memcpy(stSend.low, pEbest->low, sizeof(pEbest->low));
	//memcpy(stSend.vol, pEbest->trdq, sizeof(pEbest->trdq));
	//stSend.ydiffSign[0] = pEbest->ydiffSign[0];
	//stSend.amt[0] = '0';
	//memcpy(stSend.chgrate, pEbest->chgrate, sizeof(pEbest->chgrate));
	memcpy(stSend.time, pEbest->kortm, sizeof(pEbest->kortm));

	if (pEbest->cgubun[0] == '1' || pEbest->cgubun[0] == '-')
		stSend.side[0] = SIDE_BUY;
	else
		stSend.side[0] = SIDE_SELL;

	stSend.Enter[0] = DEF_EOL;

	SendToClient(&stSend, setSock);

	//g_log.log(INFO, "[OVERSEA](%.8s)(%.15f)(%.11s)", stSend.stk, stSend.close, stSend.time);
	return TRUE;
}
			         
VOID CListenNPublsh::SendToClient(_XAlpha::TTICK* pSendData, std::set<SOCKET>* setSock)
{
	int nSendSize = sizeof(_XAlpha::TTICK);

	std::set<SOCKET>::iterator it;
	for (it = setSock->begin(); it != setSock->end(); )
	{
		SOCKET client = (*it);
		int nSend = send(client, (char*)pSendData, nSendSize, 0);
		if (nSend > 0) {
			it++;
			continue;
		}

		long nErr = GetLastError();
		if (nErr == WSAETIMEDOUT || nErr == WSAEWOULDBLOCK)
		{
			it++;
		}
		else
		{
			if (nErr >= WSAENETDOWN && nErr<= WSAESHUTDOWN)
			{
				CloseClient(client);
				it = setSock->erase(it);
			}
			else
				it++;
			SetSockErrMsg("send");
		}
	}

}

void CListenNPublsh::CloseClient(SOCKET sock)
{
	struct linger ling;
	ling.l_onoff = 1;   // 0 ? use default, 1 ? use new value
	ling.l_linger = 0;  // close session in this time
	setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));
	//-We can avoid TIME_WAIT on both of client and server side as we code above.
	closesocket(sock);
	sock = INVALID_SOCKET;
}