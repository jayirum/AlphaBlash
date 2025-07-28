// TcpSock.cpp: implementation of the CTcpClient class.
//
//////////////////////////////////////////////////////////////////////

#include "TcpClient.h"
#include "IRUM_Common.h" //todo after completion - remove ../NEW/
#include "AlphaInc.h"
#include "Util.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTcpClient::CTcpClient() 
{
	//_tcscpy(m_zMyName, pzName);
	m_sock = INVALID_SOCKET;
	m_bConn = FALSE;
	m_bRecvErr = FALSE;
}
CTcpClient::~CTcpClient()
{
	End();

}

VOID CTcpClient::End()
{
	Disconnect();
}

BOOL CTcpClient::Initialize(char* pRemoteIP, int nPort, int nSendTimeOut, int nRecvTimeOut)
{
	TCHAR wzIp[128] = { 0, };
	A2U(pRemoteIP, wzIp);
	return Begin(wzIp, nPort, nSendTimeOut, nRecvTimeOut);
}

BOOL CTcpClient::Begin( TCHAR* pRemoteIP, int nPort, int nSendTimeOut, int nRecvTimeOut)
{
	m_nSendTimeOut = nSendTimeOut;
	m_nRecvTimeOut = nRecvTimeOut;

	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		m_nErrCode = E_WSA_STARTUP;
		return FALSE;
	}

	if( m_sock == INVALID_SOCKET){
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			m_nErrCode = E_CREATE_SOCK;
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


BOOL CTcpClient::Connect()
{
	if( m_sock == INVALID_SOCKET){
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			m_nErrCode = E_CREATE_SOCK;
			DumpErr(TEXT("socket create"), GetLastError() );	
			return FALSE;
		}
	}

	if(connect(m_sock, (LPSOCKADDR)&m_sin, sizeof(m_sin)) == SOCKET_ERROR)
	{
		m_nErrCode = E_CONNECT;
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

INT CTcpClient::SendData(char* pInBuf, int nBufLen)
{
	int nErr;
	return SendData(pInBuf, nBufLen, &nErr);
}

INT CTcpClient::SendData( char* pInBuf, int nBufLen,  int *o_ErrCode )
{
	if (!IsConnected())
	{
		if (!Connect()) {
			Disconnect();
			return -1;
		}
	}

	
	int Ret;
	int nRetryCnt = 0, nRetryBlock=0;
	
	while(1)
	{
		Ret = send(m_sock, (const char*)pInBuf, nBufLen*2, 0);
		if (Ret > 0) 
		{
			*o_ErrCode = 0;
			break;
		}
		
		int nErr = GetLastError();
		if( nErr==WSAETIMEDOUT )
		{
			if( ++nRetryCnt > DEF_SEND_RETRY)
			{
				m_nErrCode = E_SEND;
				*o_ErrCode = WSAETIMEDOUT;
				_stprintf( m_wzMsg, TEXT("WSAETIMEDOUT %d회 반복으로 에러 리턴"), nRetryCnt);
				Disconnect();
				return -1;
			}
			continue;
		}
		else if(nErr==WSAEWOULDBLOCK)
		{
			if( ++nRetryBlock > DEF_SEND_RETRY)
			{
				m_nErrCode = E_SEND;
				*o_ErrCode = WSAETIMEDOUT;
				_stprintf( m_wzMsg, TEXT("WSAEWOULDBLOCK %d회 반복으로 에러 리턴"), nRetryBlock);
				Disconnect();
				return -1;
			}
			continue;
		}
		else
		{
			m_nErrCode = E_SEND;
			*o_ErrCode = nErr;
			_stprintf( m_wzMsg, TEXT("Send Errr (%d)"), nErr);
			Disconnect();
			return -1;
		}
	}
	
	return Ret;
}

VOID CTcpClient::SetIP_Port(TCHAR* psIP, int nPort)
{
	m_sin.sin_family = AF_INET;
	m_sin.sin_port = htons(nPort);
	m_sin.sin_addr.s_addr = _tinet_addr(psIP);
}
//VOID CTcpClient::SetNagle(BOOL bOn)
//{
//	if (m_sock == INVALID_SOCKET)
//		return;
//
//	DWORD value = 1;
//	if(bOn==FALSE)
//		setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (TCHAR*) &value, sizeof(value));
//}


//
//int	 CTcpClient::GetOneRecvedPacket(TCHAR* pOutBuf)
//{
//	if (!pOutBuf)
//		return -1;
//
//	return m_pktHandler.GetOnePkt(pOutBuf);
//}
int CTcpClient::RecvData(char* pOutBuf, int nBufLen)
{
	int nErr;
	return RecvData( pOutBuf, nBufLen, &nErr);
}

int CTcpClient::RecvData( char* pOutBuf, int nBufLen, int *o_ErrCode)
{
	int Ret = recv(m_sock, pOutBuf, nBufLen, 0);
	if( Ret == SOCKET_ERROR)
	{
		int nErr = GetLastError();
		if (nErr == WSAETIMEDOUT) {
			return 0;
		}
		else
		{
			m_nErrCode = E_RECV;
			*o_ErrCode = nErr;
			DumpErr(TEXT("recv"), nErr);
			Disconnect();
			return -1;
		}
	}
	else if(Ret==0){
		Disconnect();
		return 0;
	}

	return Ret;
}



VOID CTcpClient::Disconnect()
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

VOID CTcpClient::DumpErr( TCHAR* pSrc, int nErr )
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


BOOL CTcpClient::IsConnected()
{
	if( m_sock==INVALID_SOCKET )
		return FALSE;

	return m_bConn;
}

char* CTcpClient::GetMsg()
{
	ZeroMemory(m_zMsg, sizeof(m_zMsg));
	U2A(m_wzMsg, m_zMsg);
	return m_zMsg;
}