#include "SymbolPair.h"


CSymbolPair::CSymbolPair()
{
}


CSymbolPair::~CSymbolPair()
{
}



int CSymbolPair::Add(string sMasterSymbol, string sSlaveSymbol)
{
	m_mapPair[sMasterSymbol] = sSlaveSymbol;
	return (int)__ALPHA::RET_OK;
}

int	CSymbolPair::Get(_In_ string sMasterSymbol, char* _Out_ pzSlaveSymbol)
{
	map<string, string>::iterator it = m_mapPair.find(sMasterSymbol);

	if (it == m_mapPair.end())
		return (int)__ALPHA::RET_ERR;

	strcpy(pzSlaveSymbol, (*it).second.c_str());
	return (int)__ALPHA::RET_OK;
}