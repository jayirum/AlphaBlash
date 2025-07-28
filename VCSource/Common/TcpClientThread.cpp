// TcpSock.cpp: implementation of the CTcpClientThread class.
//
//////////////////////////////////////////////////////////////////////

#include "TcpClientThread.h"
#include "IRUM_Common.h" //todo after completion - remove ../NEW/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTcpClientThread::CTcpClientThread() :CBaseThread("TCP")
{
	//strcpy(m_zMyName, pzName);
	m_sock = INVALID_SOCKET;
	m_bConn = FALSE;
}
CTcpClientThread::~CTcpClientThread()
{
	End();

}

VOID CTcpClientThread::End()
{
	Disconnect();
}

BOOL CTcpClientThread::Begin(void* callbackClassPtr, RECV_CALLBACK callBack, char* pRemoteIP, int nPort, int nTimeOut)
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return FALSE;
	}

	if( m_sock == INVALID_SOCKET){
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			DumpErr("create socket", GetLastError() );
			return FALSE;
		}
	}

	////	remote address
	m_sin.sin_family      = AF_INET;
    m_sin.sin_port        = htons(nPort);
    m_sin.sin_addr.s_addr = inet_addr(pRemoteIP);

	strcpy(m_zRemoteIP, pRemoteIP);
	m_nRemotePort = nPort;

	m_nTimeout = nTimeOut;

	m_callbackClassPtr = callbackClassPtr;
	m_outerCallback = callBack;

	CBaseThread::m_bContinue = TRUE;

	return Connect();
}


BOOL CTcpClientThread::Connect()
{
	if( m_sock == INVALID_SOCKET)
	{
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			DumpErr("socket create", GetLastError() );	
			return FALSE;
		}
	}

	if(connect(m_sock, (LPSOCKADDR)&m_sin, sizeof(m_sin)) == SOCKET_ERROR)
	{
		m_bConn = FALSE;
		DumpErr("connect", GetLastError() );
		Disconnect();
		return FALSE;
	}

	// send timeout
	setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&m_nTimeout, sizeof(m_nTimeout));
	setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&m_nTimeout, sizeof(m_nTimeout));

	sprintf(m_zMsg, "connect ok(%s)(%d)", m_zRemoteIP, m_nRemotePort);
	m_bConn  =TRUE;
	return TRUE;
}


// RECV 전용 스레드
VOID CTcpClientThread::ThreadFunc()
{
	RecvThread();
}

VOID CTcpClientThread::RecvThread()
{
	char zRecvBuff[LEN_BUFF_SIZE];

	while (Is_TimeOfStop() == FALSE)
	{
		if (!IsConnected())
		{
			Sleep(1000);
			Connect();
			if (!CBaseThread::m_bContinue)
				return;

			continue;
		}
		ZeroMemory(zRecvBuff, LEN_BUFF_SIZE);
		int nRet = recv(m_sock, zRecvBuff, LEN_BUFF_SIZE, 0);
		if (nRet > 0)
		{
			m_outerCallback(m_callbackClassPtr, TRUE, nRet, zRecvBuff, "");
		}
		else if (nRet == 0)
		{
			Disconnect();
		}
		else if (nRet == SOCKET_ERROR)
		{
			int nErr = GetLastError();
			if (nErr != WSAETIMEDOUT)
			{
				DumpErr("recv", nErr);
				m_outerCallback(m_callbackClassPtr, FALSE, nErr, "", m_zMsg);
				Disconnect();
				Sleep(5000);
			}
		}
	} // while

	printf(" CTcpClientThread::RecvThread() end....\n");
}

INT CTcpClientThread::SendData( char* pInBuf, int nBufLen,  int *o_ErrCode )
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
		Ret = send(m_sock, pInBuf, nBufLen, 0);
		if (Ret > 0) {
			*o_ErrCode = 0;
			//printf("<SEND:%d>(%.*s)\n", Ret, Ret, pInBuf);
			break;
		}
		
		int nErr = GetLastError();
		if( nErr==WSAETIMEDOUT )
		{
			if( ++nRetryCnt > DEF_SEND_RETRY)
			{
				*o_ErrCode = WSAETIMEDOUT;
				sprintf( m_zMsg, "WSAETIMEDOUT %d회 반복으로 에러 리턴", nRetryCnt);
				printf("%s\n", m_zMsg);
				Disconnect();
				return -1;
			}
			continue;
		}
		else if(nErr==WSAEWOULDBLOCK)
		{
			if( ++nRetryBlock > DEF_SEND_RETRY)
			{
				*o_ErrCode = WSAETIMEDOUT;
				sprintf( m_zMsg, "WSAEWOULDBLOCK %d회 반복으로 에러 리턴", nRetryBlock);
				printf("%s\n", m_zMsg);
				Disconnect();
				return -1;
			}
			continue;
		}
		else
		{
			*o_ErrCode = nErr;
			sprintf( m_zMsg, "Send Errr (%d)", nErr);
			printf("%s\n", m_zMsg);
			Disconnect();
			return -1;
		}
	}
	
	return Ret;
}

VOID CTcpClientThread::Set_IPPort(char* psIP, int nPort)
{
	m_sin.sin_family = AF_INET;
	m_sin.sin_port = htons(nPort);
	m_sin.sin_addr.s_addr = inet_addr(psIP);
}



VOID CTcpClientThread::Disconnect()
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

VOID CTcpClientThread::DumpErr( char* pSrc, int nErr )
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

	sprintf( m_zMsg, "[%s] %s", pSrc, (char*)lpMsgBuf );
	LocalFree( lpMsgBuf );
}


BOOL CTcpClientThread::IsConnected()
{
	if( m_sock==INVALID_SOCKET )
		return FALSE;

	return m_bConn;
}