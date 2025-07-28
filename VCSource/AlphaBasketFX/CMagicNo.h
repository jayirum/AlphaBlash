#pragma once

#include <Windows.h>
#include <stdio.h>

class CMagicNo
{
public:
	CMagicNo();
	~CMagicNo();

	long	getMagicNo();
private:
	long	m_lSeed;
	char	m_zToday[32];
	CRITICAL_SECTION	m_cs;
};

