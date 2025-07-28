#pragma once


#pragma warning( disable : 26495)
#include <winsock2.h>
#include "../Common/AlphaInc.h"
#include <map>


#define SERVICENAME		TEXT("AlphaAlermSvr")
#define DISPNAME		TEXT("AlphaAlermSvr")
#define DESC			TEXT("Alerm Relay Server - telegram and email")
#define EXENAME			TEXT("AlphaAlermSvr.exe")
#define __APP_VERSION	TEXT("v1.0")



enum { CTX_DIE = 990, /*CTX_MT4PING,*/ CTX_RQST_SEND, CTX_RQST_RECV };
enum { CK_TYPE_NORMAL, CK_TYPE_COMMUNICATION };

#define CVT_SOCKET(sock,out) { sprintf(out, "%d", sock);}

typedef struct _IO_CONTEXT
{
	WSAOVERLAPPED	overLapped;
	WSABUF			wsaBuf;
	char			buf[MAX_BUF];
	int				context;
}IO_CONTEXT;
#define CONTEXT_SIZE sizeof(IO_CONTEXT)


typedef struct _COMPLETION_KEY
{
	SOCKET	sock;
	//string	sUserID;
	LONG	refcnt;
} COMPLETION_KEY;
#define CONTEXT_SIZE sizeof(IO_CONTEXT)

struct RECV_DATA
{
	SOCKET	sock;
	char	data[MAX_BUF];
	int		len;
};