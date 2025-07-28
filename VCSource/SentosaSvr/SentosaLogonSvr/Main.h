#pragma once


#pragma warning( disable : 26495)

#include <winsock2.h>
#include <map>
#include <list>
#include "../../Common/AlphaInc.h"

#define SERVICE_NAME		TEXT("SentosaLogonSvr")
#define SERVICE_DISPNAME	TEXT("SentosaLogonSvr")
#define SERVICE_DESC		TEXT("SentosaLogonSvr")
#define EXENAME				TEXT("SentosaLogonSvr.exe")
#define __APP_VERSION		TEXT("v2.0")



enum { CTX_DIE = 990, /*CTX_MT4PING,*/ CTX_RQST_SEND, CTX_RQST_RECV };
enum { CK_TYPE_NORMAL, CK_TYPE_COMMUNICATION };

#define CVT_SOCKET(sock,out) { sprintf(out, "%d", sock);}



typedef struct _IO_CONTEXT
{
	WSAOVERLAPPED	overLapped;
	WSABUF			wsaBuf;
	char			buf[__ALPHA::LEN_BUF];
	int				context;
	DWORD           dwIoSize;
}IO_CONTEXT;
#define CONTEXT_SIZE sizeof(IO_CONTEXT)


struct COMPLETION_KEY
{
	SOCKET	sock;
	std::string	sUserID;
	std::string  sClientIp;
	std::string	sAppId;
	char	cSockTp;	//DEF_SOCKTP_R, DEF_SOCKTP_S
	long	lRefCnt;
	COMPLETION_KEY()
	{
		sock = INVALID_SOCKET;
		lRefCnt  = 0;
	}

	void	AddRefer() { InterlockedIncrement(&lRefCnt); }
	void	Release() { InterlockedDecrement(&lRefCnt); }
	bool	Is_BeingUsed() { return (lRefCnt>0); }

} ;
#define CONTEXT_SIZE sizeof(IO_CONTEXT)

//struct TDeliveryItem
//{
//	COMPLETION_KEY** ppCK;
//	char	packet[MAX_BUF];
//};

struct RECV_DATA
{
	SOCKET	sock;
	char	data[__ALPHA::LEN_BUF];
	int		len;
};

class TPacket
{
public:
	TPacket(COMPLETION_KEY* p, char* pzPacket, int len)
	{
		pCK = p;
		packet = std::string(pzPacket, len);
	};

	COMPLETION_KEY* pCK;
	std::string	packet;
	void* pAny;
};


void	ReturnError(SOCKET sock, const char* pCode, int nErrCode, char* pzMsg);
void	RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen);
void 	RequestRecvIO(SOCKET sock);
