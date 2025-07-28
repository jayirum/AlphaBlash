
#include "Main.h"
#include "Publisher.h"
#include "../Common/MemPool.h"
#include "../Common/IRUM_Common.h"
#include "../Common/AlphaProtocolUni.h"
#include "../Common/LogMsg.h"

extern CMemPool	g_memPool;
extern CLogMsg	g_log;

CPublisher::CPublisher(CSync* pSync, string sMasterId, string sMasterAcc)
{
	m_sMasterId = sMasterId;
	m_sMasterAcc = sMasterAcc;
	m_pSync = pSync;
	InitializeCriticalSection(&m_cs);

	ResumeThread();
}

CPublisher::~CPublisher()
{
	m_bContinue = FALSE;
	StopThread();

	Clear_AllCopiers();

	DeleteCriticalSection(&m_cs);
}

void CPublisher::Clear_AllCopiers()
{
	map < string, COPIER_INFO*>::iterator	it;
	EnterCriticalSection(&m_cs);
	for (it = m_mapCopiers.begin(); it != m_mapCopiers.end();)
	{
		delete (*it).second;
		it = m_mapCopiers.erase(it);
	}
	m_mapCopiers.clear();
	LeaveCriticalSection(&m_cs);
}

void CPublisher::AddCopier(SOCKET sock, string sCopierId, string sCopierAcc)
{
	COPIER_INFO* pCopier = new COPIER_INFO;
	pCopier->sock = sock;
	pCopier->sCopierId = sCopierId;
	pCopier->sCopierAcc = sCopierAcc;

	m_mapCopiers[sCopierId] = pCopier;
}

VOID CPublisher::_Main()
{
	g_log.log(INFO, "[%s's Publisher(%d)]starts....", m_sMasterId.c_str(), GetMyThreadID());
	while (Is_TimeOfStop()==FALSE)
	{
		if (m_bContinue == FALSE)
			break;

		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_PUBLISH_ORD)
			{
				Publish_Order((char*)msg.lParam);
				m_pSync->ReleaseRef();
				g_memPool.release((char*)msg.lParam);
			}
			if (msg.message == WM_DIE)
				break;
		}
	}

	g_log.log(INFO, "[%s's Publisher(%d)]ends....", m_sMasterId.c_str(), GetMyThreadID());
}

void CPublisher::Publish_Order(char* pOrdData)
{
	char		temp[128] = { 0, };
	char		zSendBuf[MAX_BUF] = { 0, };
	int			nSendLen;
	CProtoSet	ProtoSet;

	map < string, COPIER_INFO*>::iterator it;
	for (it = m_mapCopiers.begin(); it != m_mapCopiers.end(); it++)
	{
		string sID = (*it).first;
		COPIER_INFO* pCopier = (*it).second;

		ProtoSet.CopyFromRecvData(pOrdData);
		ProtoSet.SetVal(FDS_SYS, "RELAY");
		ProtoSet.SetVal(FDS_TM_HEADER, __ALPHA::Now(temp));
		ProtoSet.SetVal(FDN_PUBSCOPE_TP, __ALPHA::ALLCOPIERS_UNDER_ONEMASTER);
		ProtoSet.SetVal(FDS_USERID_MINE, sID);
		ProtoSet.SetVal(FDS_ACCNO_MY, pCopier->sCopierAcc);
		ProtoSet.SetVal(FDS_ACCNO_MASTER, m_sMasterAcc);
		//ProtoSet.SetVal(FDS_BROKER, AccountInfoString(ACCOUNT_COMPANY));

		ProtoSet.SetVal(FDN_PACKET_SEQ, __IncPacketSeq());

		nSendLen = ProtoSet.Complete(zSendBuf);
		RequestSendIO(pCopier->sock, sID, zSendBuf, nSendLen);

		g_log.log(INFO, "[Publish_Order](%d)[%s](%.*s)\n", pCopier->sock, sID.c_str(), nSendLen, zSendBuf);
	}
}

void CPublisher::RequestSendIO(SOCKET sock, string sID, char* pSendBuf, int nSendLen)
{
	BOOL  bRet = TRUE;
	DWORD dwOutBytes = 0;
	DWORD dwFlags = 0;
	IO_CONTEXT* pSend = NULL;

	COMPLETION_KEY* pCK = new COMPLETION_KEY;
	ZeroMemory(pCK, sizeof(COMPLETION_KEY));
	pCK->sock = sock;
	pCK->sUserID = sID;

	try {
		pSend = new IO_CONTEXT;

		ZeroMemory(pSend, sizeof(IO_CONTEXT));
		CopyMemory(pSend->buf, pSendBuf, nSendLen);
		//	pSend->sock	= sock;
		pSend->wsaBuf.buf = pSend->buf;
		pSend->wsaBuf.len = nSendLen;
		pSend->context = CTX_RQST_SEND;

		int nRet = WSASend(pCK->sock
			, &pSend->wsaBuf	// wsaBuf 배열의 포인터
			, 1					// wsaBuf 포인터 갯수
			, &dwOutBytes		// 전송된 바이트 수
			, dwFlags
			, &pSend->overLapped	// overlapped 포인터
			, NULL);
		if (nRet == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				bRet = FALSE;
			}
		}
	}
	catch (...) {
		bRet = FALSE;
	}
	if (!bRet)
		delete pSend;

	return;
}