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


enum LOGMSG_TP { LOGTP_SUCC, LOGTP_ERR, INFO,ERR, DEBUG_, ENTER_, NOTIFY,  LOGTP_FATAL = 9, DATA, DATA_DT};

#define INFO_BUFFER_SIZE 32767
#define WM_LOGMSG_LOG	WM_USER + 555
#define WM_LOGMSG_NOTI	WM_USER + 556
#define TCP_TIMEOUT 10

typedef struct _ST_LOGMSG
{
	LOGMSG_TP	tp;
	wchar_t	msg[DEF_LOG_LEN];
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
#define LEN_SRV_NAME	32
#define LEN_APP_NAME	32
#define LEN_BODY_NAME	512

struct NOTI_LOG
{
	wchar_t STX[1];
	wchar_t zSrvName[LEN_SRV_NAME];
	wchar_t zAppName[LEN_APP_NAME];
	wchar_t zBody[LEN_BODY_NAME];
	wchar_t ETX[1];
};
class CSendMsg : public CBaseThread
{
public:
	CSendMsg();
	virtual ~CSendMsg();
	VOID	setNotifyServerIP(wchar_t* szIP) { _tcscpy(m_szNotifyServerIP, szIP); }
	VOID	setNotifyServerPort(int nPort) { m_nNotifyServerPort = nPort; }
	int		getMyThreadID() { return GetMyThreadID();}
	BOOL	fn_SendMessage(NOTI_LOG* pNoti, int nTimeOut);

	virtual VOID	ThreadFunc();
private:

	CRITICAL_SECTION m_cs;
	wchar_t	m_szMsg[DEF_LOG_LEN];
	wchar_t	m_szNotifyServerIP[_MAX_PATH];
	int		m_nNotifyServerPort;
	wchar_t	m_szAppName[DEF_LOG_LEN];
	CTcpClient *m_pMonitorClient;

	VOID	LOCK() { EnterCriticalSection(&m_cs); };
	VOID	UNLOCK() { LeaveCriticalSection(&m_cs); };
};



///////////////////////////////////////////////
// Noti Log 를 수신하는 측을 위해
// STX+LEN(4)+DATA  ==> LEN 에는 STX와 LEN 자체의 길이는 빠진다.
#define MAX_BUFFERING	4096

class CNotiLogBuffering
{
public:
	CNotiLogBuffering();
	~CNotiLogBuffering();

	int		AddPacket(wchar_t* pBuf);
	BOOL	GetOnePacket(_Out_ wchar_t* pOutBuf, _Out_ BOOL* pbContinue);
	wchar_t* GetErrMsg() { return m_msg; }
private:
	BOOL	GetOnePacketFn(_Out_ wchar_t* pOutBuf, _Out_ BOOL* pbContinue);

	//VOID	Erase(int nStartPos, int nLen);
	VOID	RemoveAll();

	VOID	Lock() { EnterCriticalSection(&m_cs); }
	VOID	Unlock() { LeaveCriticalSection(&m_cs); }


private:
	CRITICAL_SECTION	m_cs;
	wchar_t				m_buf[MAX_BUFFERING];
	wchar_t				m_msg[1024];
	int					m_nBufLen;
};



class CLogMsg : public CBaseThread
{
public:
	CLogMsg();
	virtual ~CLogMsg();


	virtual VOID	ThreadFunc();	// RECV 를 위한 스레드

	BOOL	OpenLog(const wchar_t* psPath, const wchar_t* pFileName);
	BOOL	OpenLogEx(wchar_t* psSvrName, wchar_t* psPath, wchar_t* pFileName, wchar_t* szIP, int nPort, wchar_t* szApplicationName);
	VOID	logW(LOGMSG_TP, const wchar_t* pMsg, ...);
	VOID	log(LOGMSG_TP, const char* pMsg, ...);
	VOID	Log(LOGMSG_TP, const wchar_t* pMsg);
	VOID	enter();
	//VOID	setNotifyServerIP(wchar_t* szIP) { strcpy(m_szNotifyServerIP, szIP);m_csmNotifyThread.setNotifyServerIP(m_szNotifyServerIP); }
	//VOID	setNotifyServerPort(int nPort) { m_nNotifyServerPort = nPort; m_csmNotifyThread.setNotifyServerPort(m_nNotifyServerPort); }
	//VOID	setNotifyServerOwnHostName() { GetComputerNameIntoString(); }
	//VOID	setNotifyOwnApplicationName(wchar_t* szApplicationName) { strcpy(m_zSendAppName, szApplicationName); }

	VOID	Close();
	wchar_t	m_szFileName[_MAX_PATH];
private:
	VOID	logMsg(ST_LOGMSG* p);
	//BOOL	fn_SendMessage(wchar_t* szMessage, int nTimeOut);
	VOID	GetComputerNameIntoString();
	BOOL	ReOpen();



	//VOID	log(wchar_t* pMsg);
	//VOID	WriteErr(int nErrCode, wchar_t* pMsg, ...);
	VOID	LOCK() { EnterCriticalSection(&m_cs); };
	VOID	UNLOCK() { LeaveCriticalSection(&m_cs); };
	//BOOL	isOpenLog(){ return (m_fd>0); };

//private:
	FILE	*m_fd;
	wchar_t	m_szPath[_MAX_PATH];
	//wchar_t	m_szFileName[_MAX_PATH];
	wchar_t	m_szPureFileName[_MAX_PATH];
	wchar_t	m_szDate[8 + 1];
	wchar_t	m_szMsg[1024];
	wchar_t	m_zSendSvrName[64];
	wchar_t	m_zSendAppName[64];
	wchar_t	m_szNotifyServerIP[64];
	int		m_nNotifyServerPort;

	CRITICAL_SECTION m_cs;

	CLogMsgPool* m_pool;

	CSendMsg m_csmNotifyThread;
};
