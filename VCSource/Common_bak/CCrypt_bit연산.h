#pragma once
#include <Windows.h>


const int C1 = 19283;
const int C2 = 91827;
const int KEY = 15609;

class CCrypt
{
public:
	CCrypt();
	~CCrypt();

	int Enc(_In_ char* src, _Out_ char* dest, int len);
	int Dec(_In_ char* src, _Out_ char* dest, int len);

	char* GetMsg() { return m_zMsg; }
private:
	char	m_zMsg[1024];
};

