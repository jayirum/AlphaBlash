#ifndef __LOG_MSG_H__
#define __LOG_MSG_H__

#include "AlphaInc.h"
#include "CBaseThread.h" //todo after completion - remove ../
#include "CTimeUtils.h"
#include <list>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <share.h>

using namespace std;

enum LOGMSG_TP { LOGTP_SUCC, LOGTP_ERR, INFO, ERR, DEBUG_, ENTER_, ALERM_EMAIL, ALERM_TELEGRAM, ALERM_BOTH, LOGTP_FATAL, DATA, DATA_DT };

struct ST_LOGMSG
{
	LOGMSG_TP	tp;
	char	msg[__ALPHA::LEN_BUF];
	bool	bPrintConsole;

	ST_LOGMSG()	{ ZeroMemory(msg,sizeof(msg)); bPrintConsole=false; }
};

class CLogMsgPool
{
public:
	enum {MIN_POOL=20, MAX_POOL=50};
	CLogMsgPool();
	virtual ~CLogMsgPool();

	ST_LOGMSG* Get();
	void	Restore(ST_LOGMSG* p);

private:
	std::list<ST_LOGMSG*>	m_logPool;
	CRITICAL_SECTION	m_cs;

};



class CLogMsg : public CBaseThread
{
public:
	CLogMsg();
	virtual ~CLogMsg();


   virtual void	ThreadExec();

	bool	OpenLog(String sPath, String sFileName, String sToday="");

	void	log(LOGMSG_TP tp, const char* pMsg, ...);
	void	Log(LOGMSG_TP tp, const char* pMsg, bool bPrintConsole=false);
	void	enter();

	void	Close();
	char	m_szFileName[_MAX_PATH];
private:
	//void	SendAlerm(LOGMSG_TP tp, const char* pMsg);
	void	logMsgFunc(ST_LOGMSG* p);
	//void	GetComputerNameIntoString();
	bool	ReOpen();

	void	LOCK() { EnterCriticalSection(&m_cs); };
	void	UNLOCK() { LeaveCriticalSection(&m_cs); };
	//bool	isOpenLog(){ return (m_fd>0); };

private:

	int		m_fd;
	String	m_sPath	;
	String 	m_sFullFileName;
	String 	m_sPureFileName;
	String	m_sDate;
	//String	m_sMsg;
	char 	m_zMsg[__ALPHA::LEN_BUF];
	CRITICAL_SECTION m_cs;

	char 	m_zBuff[__ALPHA::LEN_BUF];
	char 	m_zBuffTmp[__ALPHA::LEN_BUF];

	CLogMsgPool* 	m_pool;
	CTimeUtils      m_timeUtil;

	//CSendMsg m_csmNotifyThread;
};
//---------------------------------------------------------------------------
extern CLogMsg __log;
//---------------------------------------------------------------------------


//bool __ReadAlermSvr_IpPort(_Out_ char* pzIP, _Out_ int* pnPort);



//
//#define NOTI_STX		0x02
//#define NOTI_ETX		0x03
//#define NOTI_DELIMITER	0x01
////#define LEN_SRV_NAME	32
//#define LEN_APP_NAME	32
//#define LEN_MSG_BODY	128
//
//
//
//struct NOTI_LOG
//{
//	char STX[1];
//	char AlermTp[2];					//ALARM_EMAIL, ALARM_TELEGRAM, ALARM_BOTH
//	char zAppName[LEN_APP_NAME];
//	char zBody[LEN_MSG_BODY];
//	char ETX[1];
//};
//
//
//class CSendMsg : public CBaseThread
//{
//public:
//	CSendMsg();
//	virtual ~CSendMsg();
//	void	setNotifyServerIP(char* szIP) { strcpy(m_szNotifyServerIP, szIP); }
//	void	setNotifyServerPort(int nPort) { m_nNotifyServerPort = nPort; }
//	int		getMyThreadID() { return GetMyThreadID();}
//	bool	fn_SendMessage(NOTI_LOG* pNoti, int nTimeOut);
//
//	virtual void	ThreadFunc();
//private:
//
//	CRITICAL_SECTION m_cs;
//	char	m_szMsg[DEF_LOG_LEN];
//	char	m_szNotifyServerIP[_MAX_PATH];
//	int		m_nNotifyServerPort;
//	char	m_szAppName[DEF_LOG_LEN];
//	CTcpClient *m_pMonitorClient;
//
//	void	LOCK() { EnterCriticalSection(&m_cs); };
//	void	UNLOCK() { LeaveCriticalSection(&m_cs); };
//};
//
//
//
/////////////////////////////////////////////////
//// Noti Log 를 수신하는 측을 위해
//// STX+LEN(4)+DATA  ==> LEN 에는 STX와 LEN 자체의 길이는 빠진다.
////#define MAX_BUFFER	4096
//
//class CNotiLogBuffering
//{
//public:
//	CNotiLogBuffering();
//	~CNotiLogBuffering();
//
//	int		AddPacket(char* pBuf);
//	bool	GetOnePacket(_Out_ char* pOutBuf, _Out_ bool* pbContinue);
//	char* GetErrMsg() { return m_msg; }
//private:
//	bool	GetOnePacketFn(_Out_ char* pOutBuf, _Out_ bool* pbContinue);
//
//	//void	Erase(int nStartPos, int nLen);
//	void	RemoveAll();
//
//	void	Lock() { EnterCriticalSection(&m_cs); }
//	void	Unlock() { LeaveCriticalSection(&m_cs); }
//
//
//private:
//	CRITICAL_SECTION	m_cs;
//	char				m_buf[DEF_LOG_LEN];
//	char				m_msg[1024];
//	int					m_nBufLen;
//
#endif