#pragma once


#pragma warning(disable:4996)

#include "../Common/AlphaInc.h"
#include <map>
#include <string>
using namespace std;

class CSymbolPair
{
public:
	CSymbolPair();
	~CSymbolPair();

	int	Add(string sMasterSymbol, string sSlaveSymbol);
	int	Get(_In_ string sMasterSymbol, char* _Out_ pzSlaveSymbol);

private:
	map<string, string>	m_mapPair;
};

