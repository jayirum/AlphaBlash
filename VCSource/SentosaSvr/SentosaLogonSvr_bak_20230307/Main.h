#pragma once


#pragma warning( disable : 26495)

#include <winsock2.h>
#include <map>
#include "../../Common/AlphaInc.h"

#define SERVICENAME		TEXT("SentosaSvr_v1")
#define DISPNAME		TEXT("SentosaSvr_v1")
#define DESC			TEXT("SentosaSvr_v1")
#define EXENAME			TEXT("SentosaSvr_v1.exe")
#define __APP_VERSION	TEXT("v1.0")



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
	string	sUserID;
	bool	bBeingUsed;
	string  sClientIp;

	COMPLETION_KEY()
	{
		sock = INVALID_SOCKET;
		bBeingUsed = false;
	}

	void	TurnOn_BeingUsed() { bBeingUsed = true; }
	void	TurnOff_BeingUsed() { bBeingUsed = false; }
	bool	Is_BeingUsed() { return bBeingUsed; }

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

struct TPacket
{
	//COMPLETION_KEY* pCK;
	SOCKET	sock;
	string	sUserID;
	//LONG	refcnt;
	string  sClientIp;
	string			packet;
};

void	RequestSendIO(SOCKET sock, const char* pSendBuf, int nSendLen);
void 	RequestRecvIO(COMPLETION_KEY* pCK);
void	ReturnError(SOCKET sock, const char* pCode, int nErrCode, char* pzMsg);