#pragma once

#pragma warning(disable:4996)
#include <mysqlx/xdevapi.h>
#pragma warning(disable:4996)
#pragma comment(lib, "mysqlcppconn8.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")
#include <string>
#include <map>
#include <vector>
#include <algorithm>



class Rs
{
public:
	Rs() { m_valid = true; };
	~Rs() {	Clear();}

	void setRecordCnt(int cnt) { m_nRsCnt = cnt; }
	int getRecordCnt() { return m_nRsCnt;}
	void setColumnIdx(std::string colNm, int idx)
	{
		m_mapColIdx[colNm] = idx;
	}
	void setRsValue(_In_ mysqlx::Row& refRow)
	{
		m_vecRows.push_back(refRow);

	}
	void Clear()
	{
		m_vecRows.clear();
		m_mapColIdx.clear();
	}

	bool getString(int rowIdx, std::string sColName, _Out_ std::string* psValue)
	{
		std::string sFieldName = sColName;
		std::transform(sFieldName.begin(), sFieldName.end(), sFieldName.begin(), ::toupper);

		mysqlx::Value val;
		if (!getVal(rowIdx, sFieldName, val))
			return false;
		*psValue = std::string(val);
		return true;
	}
	bool getInt(int rowIdx, std::string sColName, _Out_ int* pnValue)
	{
		std::string sFieldName = sColName;
		std::transform(sFieldName.begin(), sFieldName.end(), sFieldName.begin(), ::toupper);

		mysqlx::Value val;
		if (!getVal(rowIdx, sColName, val))
			return false;
		*pnValue = int(val);
		return true;
	}
	bool getDbl(int rowIdx, std::string sColName, _Out_ double* pdValue)
	{
		std::string sFieldName = sColName;
		std::transform(sFieldName.begin(), sFieldName.end(), sFieldName.begin(), ::toupper);

		mysqlx::Value val;
		if (!getVal(rowIdx, sColName, val))
			return false;
		*pdValue = double(val);
		return true;
	}

	bool Has_RetCode_RetMsg()
	{
		int ret_code = 0;
		std::string ret_msg;
		if (!getInt(0, "RET_CODE", &ret_code) || !getString(0, "RET_MSG", &ret_msg))
			return false;

		return true;
	}
	bool Is_Successful_ExcutingSP(_Out_ int &ret_code, _Out_ std::string& sMsg)
	{
		if (!IsValid())
		{
			sMsg = "No valid Recordsets";
			return false;
		}
		
		if (!getInt(0, "RET_CODE", &ret_code))
		{
			sMsg = "There is no [RET_CODE]";
			return false;
		}
		if (!getString(0, "RET_MSG", &sMsg)) {
			sMsg = "There is no [RET_MSG]";
			return false;
		}
		
		return (ret_code==0);
	}

	char* GetMsg() { return m_zMsg; }
	bool IsValid() { return m_valid; }
	void setInValid() { m_valid = false; }
private:
	bool getVal(int rowIdx, std::string sColName, _Out_ mysqlx::Value& val)
	{
		if (rowIdx >= (int)m_vecRows.size()) {
			sprintf(m_zMsg, "Out of Index");
			return false;
		}

		std::map< std::string, int>::iterator it = m_mapColIdx.find(sColName);
		if (it == m_mapColIdx.end()) {
			sprintf(m_zMsg, "[%s] is not a correct column name", sColName.c_str());
			return false;
		}
		int nColIdx = (*it).second;

		val = m_vecRows[rowIdx].get(nColIdx);
		return true;
	}

private:
	int m_nRsCnt;
	std::map< std::string, int>	m_mapColIdx;	// column name, index
	std::vector< mysqlx::Row>	m_vecRows;
	char						m_zMsg[256];
	bool						m_valid;
};

class CMySqlHandler
{
public:
	
	CMySqlHandler();
	~CMySqlHandler();

	
	bool Initialize(std::string sSvrIP, int nSvrPort, std::string sUserId, std::string sPwd, std::string	sDBName);
	bool OpenDB();
	bool IsConned(); 
	void CloseDB();

	Rs* Execute(char* pzQ);
	char* GetMsg() { return m_zMsg; }

private:
	bool Exec(char* pzQ, _Out_ Rs* rs);
	
protected:
	mysqlx::Session* m_sess;
	mysqlx::SqlResult res;

	std::string	m_sSvrIp;
	int		m_nSvrPort;
	std::string	m_sUserId;
	std::string	m_sPwd;
	std::string	m_sDBName;

	char	m_zMsg[1024];
};

