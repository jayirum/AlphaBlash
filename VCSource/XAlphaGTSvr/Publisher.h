#pragma once


#include "../Common/BaseThread.h"
#include "Sync.h"
#include <string>
using namespace std;

struct COPIER_INFO
{
	SOCKET sock;
	string sCopierId;
	string sCopierAcc;
};

class CPublisher : public CBaseThread
{
public:
	CPublisher(CSync* pSync, string sMasterId, string sMasterAcc);
	virtual ~CPublisher();
	void	AddCopier(SOCKET sock, string sCopierId, string sCopierAcc);
	VOID	ThreadFunc() { _Main(); };
	
private:
	void	_Main();
	void	Publish_Order(char* pOrdData);
	void	Clear_AllCopiers();
	void	RequestSendIO(SOCKET sock, string sID, char* pSendBuf, int nSendLen);
private:
	map < string, COPIER_INFO*>			m_mapCopiers;		// SlaveID, TIME
	CRITICAL_SECTION					m_cs;
	CSync*								m_pSync;
	string								m_sMasterId, m_sMasterAcc;
};

