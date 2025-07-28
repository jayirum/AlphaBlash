#pragma once

#include <Windows.h>

class CSync
{
public:
	CSync();
	~CSync();

	void	AddRef();
	void	ReleaseRef();
	BOOL	IsCleared();

private:
	CRITICAL_SECTION	m_cs;
	int					m_nRefCnt;
};

