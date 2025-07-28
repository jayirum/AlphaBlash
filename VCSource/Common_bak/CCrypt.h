#pragma once

#include <Windows.h>

const unsigned char CRYPT_KEY[] = "AlphaBlash";

class CCrypt
{
public:
	CCrypt();
	~CCrypt();

	BOOL Encrypt(
		_In_ char* pbPlaintext,
		_Out_ char* pbCipherText,
		_In_ DWORD dwDataSize,
		_In_ DWORD dwBufSize,
		_Out_ DWORD* pdwEncSize
	);


	BOOL Decrypt(
		_In_ char* pbCipherText,
		_Out_ char* pbPlaintext,
		_In_ DWORD dwEncDataSize,
		_Out_ DWORD* pdwDecSize
		);

	char* GetMsg() { return m_zMsg; }
private:
	DWORD	m_dwErrNo;
	char	m_zMsg[1024];
};
