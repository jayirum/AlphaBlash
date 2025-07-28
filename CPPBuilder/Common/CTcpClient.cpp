
#include "CTcpClient.h"
#include "AlphaProtocol.h"


CTcpClient::CTcpClient(char* pzName) :CBaseThread(pzName)
{
	m_sMyName 	= pzName;
	m_sock 		= INVALID_SOCKET;
	m_bConn 	= false;
	m_nSendTimeout = m_nRecvTimeout = 1;
}
CTcpClient::~CTcpClient()
{
	Disconnect();
}


bool CTcpClient::Init_Connect( char* pRemoteIP, int nPort, int nSendTimeout, int nRecvTimeout)
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return false;
	}

	if( m_sock == INVALID_SOCKET){
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			DumpErr("create socket", GetLastError() );
			return false;
		}
	}

	////	remote address
	m_sin.sin_family      = AF_INET;
    m_sin.sin_port        = htons(nPort);
    m_sin.sin_addr.s_addr = inet_addr(pRemoteIP);

	strcpy(m_zRemoteIP, pRemoteIP);
	m_nRemotePort = nPort;

	m_nSendTimeout = nSendTimeout;
	m_nRecvTimeout = nRecvTimeout;

	CBaseThread::m_bContinue = true;

	return Connect();
}

bool CTcpClient::ReConnect()
{
	if( m_sock != INVALID_SOCKET)
	{
		Disconnect();
	}

	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(m_sock == INVALID_SOCKET)
	{
		DumpErr("socket create", GetLastError() );
		return false;
	}


	if(connect(m_sock, (LPSOCKADDR)&m_sin, sizeof(m_sin)) == SOCKET_ERROR)
	{
		m_bConn = false;
		DumpErr("connect", GetLastError() );
		return false;
	}

	// send timeout
	setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&m_nSendTimeout, sizeof(m_nSendTimeout));
	setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&m_nRecvTimeout, sizeof(m_nRecvTimeout));

	m_sMsg.Format("Re-Connect Ok(%s)(%d)",ARRAYOFCONST((m_zRemoteIP, m_nRemotePort)));
	m_bConn	= true;

	return true;
}

bool CTcpClient::Connect()
{
	if( m_sock == INVALID_SOCKET)
	{
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			DumpErr("socket create", GetLastError() );
			return false;
		}
	}

	if(connect(m_sock, (LPSOCKADDR)&m_sin, sizeof(m_sin)) == SOCKET_ERROR)
	{
		m_bConn = false;
		DumpErr("connect", GetLastError() );
		//Disconnect();
		return false;
	}

	// send timeout
	setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&m_nSendTimeout, sizeof(m_nSendTimeout));
	setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&m_nRecvTimeout, sizeof(m_nRecvTimeout));

	m_sMsg.Format("connect ok(%s)(%d)",ARRAYOFCONST((m_zRemoteIP, m_nRemotePort)));
	m_bConn	= true;

	CBaseThread::ResumeThread() ;

	return true;
}


//// RECV 전용 스레드
void CTcpClient::ThreadExec()
{
	char zRecvBuff[__ALPHA::BUF_LEN];
	int nRecvLen;
	while (Is_TimeOfStop() == false)
	{
		if (!IsConnected())
		{
			Sleep(10);
			//Connect();
			if (!CBaseThread::m_bContinue)
				return;

			continue;
		}

		do
		{
			ZeroMemory(zRecvBuff, sizeof(zRecvBuff));
			nRecvLen = recv(m_sock, zRecvBuff, __ALPHA::BUF_LEN, 0);
			if (nRecvLen > 0)
			{
				m_buffer.Add(zRecvBuff, nRecvLen);
			}
			else if (nRecvLen == 0)
			{
				Disconnect();
				m_sMsg = "Connections has been closed by peer";
			}
			else if (nRecvLen == SOCKET_ERROR)
			{
				int nErr = GetLastError();
				if (nErr != WSAETIMEDOUT)
				{
					DumpErr("recv", nErr);
					if(
						nErr==WSAENETDOWN || nErr==WSAENETUNREACH || nErr==WSAECONNABORTED || nErr==WSAECONNRESET ||
						nErr==WSAENOTCONN || nErr==WSAESHUTDOWN || nErr==WSAECONNREFUSED || nErr==WSAEHOSTDOWN
					  )
					{
						Disconnect();
					}
					//m_bRecvErr = false;
					//Disconnect();
					//Sleep(5000);
				}
			}
		}
		while(nRecvLen>0);

	} // while
}


bool CTcpClient::GetOnePacket(_Out_ int* pnLen, _Out_ char* pOutBuf)
{
    return m_buffer.GetOnePacketLock(pnLen, pOutBuf);
}

int CTcpClient::SendData( char* pInBuf, int nBufLen,  int *o_ErrCode )
{
	if (!IsConnected())
	{
		if (!Connect())
		{
			Disconnect();
			return -1;
		}
	}


	int Ret;
	int nRetryCnt = 0, nRetryBlock=0;

	while(1)
	{
		Ret = send(m_sock, pInBuf, nBufLen, 0);
		if (Ret > 0)
		{
			*o_ErrCode = 0;
			return Ret;	/////// SUCCESS /////////
		}

		int nErr = GetLastError();
		if( nErr==WSAETIMEDOUT )
		{
			if( ++nRetryCnt <= DEF_SEND_RETRY)
				continue;

			*o_ErrCode = WSAETIMEDOUT;
			m_sMsg.Format("WSAETIMEDOUT %d회 반복으로 에러 리턴", ARRAYOFCONST((nRetryCnt)));
		}
		else if(nErr==WSAEWOULDBLOCK)
		{
			if( ++nRetryBlock <= DEF_SEND_RETRY)
				continue;

			*o_ErrCode = WSAEWOULDBLOCK;
			m_sMsg.Format("WSAEWOULDBLOCK %d회 반복으로 에러 리턴", ARRAYOFCONST((nRetryBlock)));
		}
		else
		{
			*o_ErrCode = nErr;
			m_sMsg.Format("Send Errr (%d)", ARRAYOFCONST((nErr)));
		}
		Disconnect();
		return -1;
	}

	return Ret;
}



void CTcpClient::Disconnect()
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
	m_bConn = false;

}


void CTcpClient::DumpErr( char* pSrc, int nErr )
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

	m_sMsg.Format("[%s] %s", ARRAYOFCONST(( pSrc, (char*)lpMsgBuf)) );
	LocalFree( lpMsgBuf );
}


bool CTcpClient::IsConnected()
{
	if( m_sock==INVALID_SOCKET )
		return false;

	return m_bConn;
}
