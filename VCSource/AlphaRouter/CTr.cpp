#include "Main.h"
#include "CTr.h"
#include "../Common/util.h"
#include "../Common/AlphaInc.h"


CTr::CTr()
{
	InitializeCriticalSection(&m_cs);
}

CTr::~CTr()
{
	Lock();
	m_setSock.clear();
	UnLock();
	DeleteCriticalSection(&m_cs);
}

BOOL CTr::RegUnreg(SOCKET sock, const char* pzCode)
{
	BOOL bRet = TRUE;

	Lock();
	__try
	{
		__try
		{
			RegUnregIn(sock, pzCode);
		}
		__except (ReportException(GetExceptionCode(), TEXT("CTr::RegUnreg"), m_wzMsg))
		{
			bRet = FALSE;
		}
	}
	__finally
	{
		UnLock();
	}
	return bRet;
}

void CTr::RegUnregIn(SOCKET sock, const char* pzCode)
{
	if( strcmp(pzCode, __ALPHA::CODE_REG_ROUTER)==0)
		m_setSock.insert(sock);
	if (strcmp(pzCode, __ALPHA::CODE_UNREG_ROUTER) == 0)
		m_setSock.erase(sock);

}

VOID CTr::SendData(char* pRecvData, int nRecvLen)
{
	Lock();
	__try
	{
		SendDataIn(pRecvData, nRecvLen);
	}
	__finally
	{
		UnLock();
	}
}


VOID CTr::SendDataIn(char* pRecvData, int nRecvLen)
{
	set<SOCKET>::iterator it;
	for (it = m_setSock.begin(); it != m_setSock.begin(); it++)
	{
		RequestSendIO((*it), pRecvData, nRecvLen);
	}
}



VOID CTr::RequestSendIO(SOCKET sock, char* pSendBuf, int nSendLen)
{

	BOOL  bRet = TRUE;
	DWORD dwOutBytes = 0;
	DWORD dwFlags = 0;
	IO_CONTEXT* pSend = NULL;

	try {
		pSend = new IO_CONTEXT;

		ZeroMemory(pSend, sizeof(IO_CONTEXT));
		CopyMemory(pSend->buf, pSendBuf, nSendLen);
		//	pSend->sock	= sock;
		pSend->wsaBuf.buf = pSend->buf;
		pSend->wsaBuf.len = nSendLen;
		pSend->context = CTX_RQST_SEND;

		int nRet = WSASend(sock
			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
			, 1					// wsaBuf 포인터 갯수
			, &dwOutBytes		// 전송된 바이트 수
			, dwFlags
			, &pSend->overLapped	// overlapped 포인터
			, NULL);
		if (nRet == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				bRet = FALSE;
			}
		}
		//printf("WSASend ok..................\n");
	}
	catch (...) {
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;

	return;
}
