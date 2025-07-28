// TcpSock.cpp: implementation of the CRouterClient class.
//
//////////////////////////////////////////////////////////////////////

#include "CRouterClient.h"
#include "IRUM_Common.h" //todo after completion - remove ../NEW/
#include "AlphaInc.h"
#include "Util.h"
#include "AlphaProtocolUni.h"
#include "AlphaProtoSetEx.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRouterClient::CRouterClient() 
{
	m_sock = INVALID_SOCKET;
	m_bConn = FALSE;
}
CRouterClient::~CRouterClient()
{
	End();

}

VOID CRouterClient::End()
{
	Disconnect();
}

BOOL CRouterClient::Initialize(char* pRemoteIP, int nPort, int nSendTimeOut, int nRecvTimeOut)
{
	TCHAR wzIp[128] = { 0, };
	A2U(pRemoteIP, wzIp);
	return Begin(wzIp, nPort, nSendTimeOut, nRecvTimeOut);
}

BOOL CRouterClient::Begin( TCHAR* pRemoteIP, int nPort, int nSendTimeOut, int nRecvTimeOut)
{
	m_nSendTimeOut = nSendTimeOut;
	m_nRecvTimeOut = nRecvTimeOut;

	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		DumpErr(TEXT("WSAStartup"), GetLastError());
		return FALSE;
	}

	if( m_sock == INVALID_SOCKET)
	{
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			DumpErr(TEXT("create socket"), GetLastError() );
			return FALSE;
		}
	}

	////	remote address
	m_sin.sin_family      = AF_INET;
    m_sin.sin_port        = htons(nPort);
    m_sin.sin_addr.s_addr = _tinet_addr((const TCHAR*)pRemoteIP);

	_tcscpy(m_zRemoteIP, pRemoteIP);
	m_nRemotePort = nPort;

	return Connect();
}


BOOL CRouterClient::Connect()
{
	if( m_sock == INVALID_SOCKET){
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			DumpErr(TEXT("socket create"), GetLastError() );	
			return FALSE;
		}
	}

	if(connect(m_sock, (LPSOCKADDR)&m_sin, sizeof(m_sin)) == SOCKET_ERROR)
	{
		m_bConn = FALSE;
		DumpErr(TEXT("connect"), GetLastError() );
		Disconnect();
		return FALSE;
	}

	// send timeout
	setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&m_nSendTimeOut, sizeof(m_nSendTimeOut));
	setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&m_nRecvTimeOut, sizeof(m_nRecvTimeOut));

	_stprintf(m_wzMsg, TEXT("connect ok(%s)(%d)"), m_zRemoteIP, m_nRemotePort);
	m_bConn  =TRUE;
	return TRUE;
}


BOOL CRouterClient::RegRouter(const char* pzBizCode)
{
	char zSendBuf[MAX_BUF] = { 0 };
	CProtoSetEx set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_REG_ROUTER);
	set.SetVal(FDS_ROUTE_YN, "Y");
	int len = set.Complete(zSendBuf);

	int nSendLen = 0;
	return SendData(zSendBuf, len, &nSendLen);
}

BOOL CRouterClient::UnRegRouter(const char* pzBizCode)
{
	char zSendBuf[MAX_BUF] = { 0 };
	CProtoSetEx set;
	set.Begin();
	set.SetVal(FDS_CODE, __ALPHA::CODE_UNREG_ROUTER);
	set.SetVal(FDS_ROUTE_YN, "Y");
	int len = set.Complete(zSendBuf);

	int nSendLen = 0;
	return SendData(zSendBuf, len, &nSendLen);
}


BOOL CRouterClient::SendData( char* pInBuf, int nBufLen,  int *pnSendLen )
{
	*pnSendLen = 0;


	if (!IsConnected())
	{
		if (!Connect()) {
			Disconnect();
			return FALSE;
		}
	}

	
	int Ret;
	int nRetryCnt = 0, nRetryBlock=0;
	
	while(1)
	{
		Ret = send(m_sock, (const char*)pInBuf, nBufLen*2, 0);
		if (Ret > 0) 
		{
			*pnSendLen = Ret;
			return TRUE;;
		}
		
		int nErr = GetLastError();
		if( nErr==WSAETIMEDOUT )
		{
			if( ++nRetryCnt > DEF_SEND_RETRY)
			{
				_stprintf( m_wzMsg, TEXT("WSAETIMEDOUT %d회 반복으로 에러 리턴"), nRetryCnt);
				return FALSE;
			}
			continue;
		}
		else if(nErr==WSAEWOULDBLOCK)
		{
			if( ++nRetryBlock > DEF_SEND_RETRY)
			{
				_stprintf( m_wzMsg, TEXT("WSAEWOULDBLOCK %d회 반복으로 에러 리턴"), nRetryBlock);
				return FALSE;
			}
			continue;
		}
		else
		{
			DumpErr(TEXT("Send error"), GetLastError());
			return FALSE;
		}
	}
	
	return Ret;
}

VOID CRouterClient::SetIP_Port(TCHAR* psIP, int nPort)
{
	m_sin.sin_family = AF_INET;
	m_sin.sin_port = htons(nPort);
	m_sin.sin_addr.s_addr = _tinet_addr(psIP);
}
//VOID CRouterClient::SetNagle(BOOL bOn)
//{
//	if (m_sock == INVALID_SOCKET)
//		return;
//
//	DWORD value = 1;
//	if(bOn==FALSE)
//		setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (TCHAR*) &value, sizeof(value));
//}



int CRouterClient::RecvData( char* pOutBuf, int nBufLen, int *pnRecvLen)
{
	*pnRecvLen = 0;

	int Ret = recv(m_sock, pOutBuf, nBufLen, 0);
	if (Ret == SOCKET_ERROR)
	{
		int nErr = GetLastError();
		if (nErr == WSAETIMEDOUT)
		{
			return TRUE;
		}
		DumpErr(TEXT("recv"), GetLastError());
		return FALSE;
	}

	*pnRecvLen = Ret;

	return TRUE;
}



VOID CRouterClient::Disconnect()
{
	if (m_sock != INVALID_SOCKET) {
		struct linger ling;
		ling.l_onoff = 1;   // 0 ? use default, 1 ? use new value
		ling.l_linger = 0;  // close session in this time
		setsockopt(m_sock, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));
		//-We can avoid TIME_WAIT on both of client and server side as we code above.
		closesocket(m_sock);
	}
	m_sock = INVALID_SOCKET;
	m_bConn = FALSE;
}

VOID CRouterClient::DumpErr( TCHAR* pSrc, int nErr )
{
	LPVOID lpMsgBuf=NULL;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		nErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );

	_stprintf( m_wzMsg, TEXT("[%s] %s"), pSrc, (TCHAR*)lpMsgBuf );
	LocalFree( lpMsgBuf );
}


BOOL CRouterClient::IsConnected()
{
	if( m_sock==INVALID_SOCKET )
		return FALSE;

	return m_bConn;
}

char* CRouterClient::GetMsg()
{
	ZeroMemory(m_zMsg, sizeof(m_zMsg));
	U2A(m_wzMsg, m_zMsg);
	return m_zMsg;
}