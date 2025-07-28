#include "CMySqlHandler.h"


CMySqlHandler::CMySqlHandler()
{
	m_sess = NULL;
}

CMySqlHandler::~CMySqlHandler()
{
	CloseDB();
}

bool CMySqlHandler::Initialize(std::string sSvrIP, int nSvrPort, std::string sUserId, std::string sPwd, std::string	sDBName)
{
	m_sSvrIp = sSvrIP;
	m_nSvrPort = nSvrPort;
	m_sUserId = sUserId;
	m_sPwd = sPwd;
	m_sDBName = sDBName;

	return true;
}

bool CMySqlHandler::OpenDB()
{	
	if (m_sSvrIp.size() == 0)
	{
		sprintf(m_zMsg, "Please call Initialize() first");
		return false;
	}

	try
	{
		CloseDB();
		m_sess = new mysqlx::Session(m_sSvrIp, m_nSvrPort, m_sUserId, m_sPwd, m_sDBName);
	}
	catch (const mysqlx::Error& err)
	{
		sprintf(m_zMsg, "DBOpen Err:%s", err.what());
		return false;
	}
	return true;
}

void CMySqlHandler::CloseDB()
{
	if (m_sess) {
		m_sess->close();
		delete m_sess;
	}
}

bool CMySqlHandler::IsConned()
{
	if (m_sess == NULL )
		return false;
	return true;
}


Rs* CMySqlHandler::Execute(char* pzQ)
{
	Rs *pRs = new Rs;

	if (Exec(pzQ, pRs) == false)
	{
		pRs->setInValid();
	}
	return pRs;
}

bool CMySqlHandler::Exec(char* pzQ, _Out_ Rs* rs)
{
	try
	{
		if (!IsConned())
		{
			sprintf(m_zMsg, "Please Open DB first");
			return false;
		}

		mysqlx::SqlResult sqlRes = m_sess->sql(pzQ).execute();
		rs->setRecordCnt(sqlRes.count());

		if (rs->getRecordCnt() == 0)
			return true;

		int nColCnt = sqlRes.getColumnCount();
		for (int i = 0; i < nColCnt; i++)
		{
			std::string sName = sqlRes.getColumn(i).getColumnLabel();
			std::transform(sName.begin(), sName.end(), sName.begin(), ::toupper);
			rs->setColumnIdx(sName, i);
		}
		if (sqlRes.hasData())
		{
			do
			{
				mysqlx::Row row = sqlRes.fetchOne();
				if (row.isNull())
					break;
				
				rs->setRsValue(row);
			} while (true);
		}
	}
	catch (const mysqlx::Error& err)
	{
		sprintf(m_zMsg, "Exec Err:%s-%s", err.what(), pzQ);
		return false;
	}
	return true;
}
