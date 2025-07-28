// Log.h: interface for the CLogMsg class.
//
//////////////////////////////////////////////////////////////////////

#pragma once 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcpClient.h"
#include <windows.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include "BaseThread.h" //todo after completion - remove ../
#include <list>

#define DEF_LOG_LEN 4096


//enum LOGMSG_TP { LOGTP_SUCC, LOGTP_ERR, INFO,ERR, DEBUG_, ENTER_, NOTIFY,  LOGTP_FATAL = 9, DATA, DATA_DT};
enum LOGMSG_TP { LOGTP_SUCC, LOGTP_ERR, INFO, ERR, DEBUG_, ENTER_, ALERM_EMAIL, ALERM_TELEGRAM, ALERM_BOTH, LOGTP_FATAL, DATA, DATA_DT };

#define INFO_BUFFER_SIZE 32767
#define WM_LOGMSG_LOG	WM_USER + 555
#define WM_LOGMSG_NOTI	WM_USER + 556
#define TCP_TIMEOUT 10

typedef struct _ST_LOGMSG
{
	LOGMSG_TP	tp;
	char	msg[DEF_LOG_LEN];
	BOOL	bPrintConsole;
}ST_LOGMSG;

class CLogMsgPool
{
public:
	enum {MIN_POOL=20, MAX_POOL=50};
	CLogMsgPool();
	virtual ~CLogMsgPool();

	ST_LOGMSG* Get();
	VOID	Restore(ST_LOGMSG* p);

private:
	std::list<ST_LOGMSG*>	m_logPool;
	CRITICAL_SECTION	m_cs;

};


#define NOTI_STX		0x02
#define NOTI_ETX		0x03
#define NOTI_DELIMITER	0x01
//#define LEN_SRV_NAME	32
#define LEN_APP_NAME	32
#define LEN_MSG_BODY	128



struct NOTI_LOG
{
	char STX[1];
	char AlermTp[2];					//ALARM_EMAIL, ALARM_TELEGRAM, ALARM_BOTH
	char zAppName[LEN_APP_NAME];
	char zBody[LEN_MSG_BODY];
	char ETX[1];
};


class CSendMsg : public CBaseThread
{
public:
	CSendMsg();
	virtual ~CSendMsg();
	VOID	setNotifyServerIP(char* szIP) { strcpy(m_szNotifyServerIP, szIP); }
	VOID	setNotifyServerPort(int nPort) { m_nNotifyServerPort = nPort; }
	int		getMyThreadID() { return GetMyThreadID();}
	BOOL	fn_SendMessage(NOTI_LOG* pNoti, int nTimeOut);

	virtual VOID	ThreadFunc();
private:

	CRITICAL_SECTION m_cs;
	char	m_szMsg[DEF_LOG_LEN];
	char	m_szNotifyServerIP[_MAX_PATH];
	int		m_nNotifyServerPort;
	char	m_szAppName[DEF_LOG_LEN];
	CTcpClient *m_pMonitorClient;

	VOID	LOCK() { EnterCriticalSection(&m_cs); };
	VOID	UNLOCK() { LeaveCriticalSection(&m_cs); };
};



///////////////////////////////////////////////
// Noti Log 를 수신하는 측을 위해
// STX+LEN(4)+DATA  ==> LEN 에는 STX와 LEN 자체의 길이는 빠진다.
//#define MAX_BUFFER	4096

class CNotiLogBuffering
{
public:
	CNotiLogBuffering();
	~CNotiLogBuffering();

	int		AddPacket(char* pBuf);
	BOOL	GetOnePacket(_Out_ char* pOutBuf, _Out_ BOOL* pbContinue);
	char* GetErrMsg() { return m_msg; }
private:
	BOOL	GetOnePacketFn(_Out_ char* pOutBuf, _Out_ BOOL* pbContinue);

	//VOID	Erase(int nStartPos, int nLen);
	VOID	RemoveAll();

	VOID	Lock() { EnterCriticalSection(&m_cs); }
	VOID	Unlock() { LeaveCriticalSection(&m_cs); }


private:
	CRITICAL_SECTION	m_cs;
	char				m_buf[DEF_LOG_LEN];
	char				m_msg[1024];
	int					m_nBufLen;
};



class CLogMsg : public CBaseThread
{
public:
	CLogMsg();
	virtual ~CLogMsg();


	virtual VOID	ThreadFunc();	// RECV 를 위한 스레드

	BOOL	OpenLog(const char* psPath, const char* pFileName);
	BOOL	OpenLogEx(char* szApplicationName, char* psPath, char* pFileName, char* szIP, int nPort);
	BOOL	OpenLogWithAlerm(char* szApplicationName, char* psPath, char* pFileName);
	VOID	log(LOGMSG_TP tp, const char* pMsg, ...);
	VOID	log_print(LOGMSG_TP tp, const char* pMsg, ...);
	VOID	Log(LOGMSG_TP tp, const char* pMsg, BOOL bPrintConsole=TRUE);
	VOID	SendAlermEmail(const char* pMsg);
	VOID	SendAlermTelegram(const char* pMsg);
	VOID	SendAlermBoth(const char* pMsg);
	VOID	enter();

	VOID	Close();
	char	m_szFileName[_MAX_PATH];
private:
	VOID	SendAlerm(LOGMSG_TP tp, const char* pMsg);
	VOID	logMsgFunc(ST_LOGMSG* p);
	//VOID	GetComputerNameIntoString();
	BOOL	ReOpen();

	VOID	LOCK() { EnterCriticalSection(&m_cs); };
	VOID	UNLOCK() { LeaveCriticalSection(&m_cs); };
	//BOOL	isOpenLog(){ return (m_fd>0); };

//private:
	int		m_fd;
	char	m_szPath[_MAX_PATH];
	//char	m_szFileName[_MAX_PATH];
	char	m_szPureFileName[_MAX_PATH];
	char	m_szDate[8 + 1];
	char	m_szMsg[1024];
	//char	m_zSendSvrName[64];
	char	m_zSendAppName[64];
	char	m_szNotifyServerIP[64];
	int		m_nNotifyServerPort;

	CRITICAL_SECTION m_cs;

	CLogMsgPool* m_pool;

	CSendMsg m_csmNotifyThread;
};


BOOL __ReadAlermSvr_IpPort(_Out_ char* pzIP, _Out_ int* pnPort);