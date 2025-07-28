#include "CCrypt.h"
#include <mbstring.h>

CCrypt::CCrypt()
{}

CCrypt::~CCrypt()
{

}

BOOL CCrypt::Encrypt(
	_In_ char* pbPlaintext,
	_Out_ char* pbCipherText,
	_In_ DWORD dwDataSize,
	_In_ DWORD dwBufSize,
	_Out_ DWORD* pdwEncSize
	)
{
	HCRYPTPROV  hProv = NULL;
	HCRYPTKEY   hKey = NULL;
	HCRYPTHASH  hHash = NULL;

	BOOL bRet = TRUE;

	memcpy(pbCipherText, pbPlaintext, dwDataSize);

	try
	{
		if (CryptAcquireContext(&hProv,
			NULL,
			NULL,
			PROV_RSA_FULL,
			CRYPT_VERIFYCONTEXT) == FALSE)
			throw GetLastError();

		if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == FALSE)
			throw GetLastError();

		if (CryptHashData(hHash, CRYPT_KEY, _mbslen(CRYPT_KEY), 0) == FALSE)
			throw GetLastError();

		if (CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey) == FALSE)
			throw GetLastError();

		if (CryptEncrypt(hKey, 0, TRUE, 0, (UCHAR*)pbCipherText, &dwDataSize, dwBufSize) == FALSE)
			throw GetLastError();
		*pdwEncSize = dwBufSize;
	}
	catch (const DWORD dwLastError)
	{
		m_dwErrNo = dwLastError;
		bRet = FALSE;
	}

	if (hKey)
		CryptDestroyKey(hKey);

	if (hHash)
		CryptDestroyHash(hHash);

	if (hProv)
		CryptReleaseContext(hProv, 0);

	return bRet;
}


BOOL CCrypt::Decrypt(
	char* pbCipherText,
	char* pbPlaintext,
	DWORD dwEncDataSize,
	_Out_ DWORD* pdwDecSize
	)
{
	HCRYPTPROV  hProv = NULL;
	HCRYPTKEY   hKey = NULL;
	HCRYPTHASH  hHash = NULL;

	DWORD bRet = TRUE;

	memcpy(pbPlaintext, pbCipherText, dwEncDataSize);

	try
	{
		if (CryptAcquireContext(&hProv,
			NULL,
			NULL,
			PROV_RSA_FULL,
			CRYPT_VERIFYCONTEXT) == FALSE)
			throw GetLastError();

		if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == FALSE)
			throw GetLastError();

		if (CryptHashData(hHash, CRYPT_KEY, _mbslen(CRYPT_KEY), 0) == FALSE)
			throw GetLastError();

		if (CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey) == FALSE)
			throw GetLastError();

		if (CryptDecrypt(hKey, 0, TRUE, 0, (UCHAR*)pbPlaintext, &dwEncDataSize) == FALSE)
			throw GetLastError();
		*pdwDecSize = dwEncDataSize;
	}
	catch (const DWORD dwLastError)
	{
		m_dwErrNo = dwLastError;
		bRet = FALSE;
	}

	if (hKey)
		CryptDestroyKey(hKey);

	if (hHash)
		CryptDestroyHash(hHash);

	if (hProv)
		CryptReleaseContext(hProv, 0);

	return bRet;
}