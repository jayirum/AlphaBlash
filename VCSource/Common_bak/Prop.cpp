#include <windows.h>
#include <winreg.h>
#include "Prop_Uni.h"
#include <stdio.h>
#include <tchar.h>


//##ModelId=42046BE201C2
CProp::CProp()
{
	m_hKey = NULL;
	memset(m_szErrMsg, 0x00, sizeof(m_szErrMsg));
}

       
CProp::CProp(HKEY hKey, TCHAR* lpszSub, BOOL bQueryOnly /* = TRUE */)
{
	m_hKey = NULL;
	memset(m_szErrMsg, 0x00, sizeof(m_szErrMsg));

	SetBaseKey(hKey, lpszSub, bQueryOnly);
}

//##ModelId=42046BE201CC
CProp::~CProp()
{
	Close();
}

//##ModelId=42046BE2024F
void CProp::Close() {
	if(m_hKey) 
		RegCloseKey(m_hKey);
	m_hKey = NULL;
}


//##ModelId=42046BE201CE
/**	\brief	KEY로 사용할 위치를 설정

	\param	hKey		KEY
	\param	lpszSub		Section 이름
	\param	bQueryOnly	읽기 전용

	\return	BOOL	
*/
BOOL CProp::SetBaseKey(HKEY hKey, TCHAR* lpszSub, BOOL bQueryOnly) 
{
	unsigned long lResult;
	REGSAM samDesired;
	if(bQueryOnly)
		samDesired = KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS;
	else 
		samDesired = KEY_ALL_ACCESS;

	lResult = RegOpenKeyEx(hKey, lpszSub, 0, samDesired, &m_hKey);
	if(lResult != ERROR_SUCCESS){
		DumpErr(lResult);
		return FALSE;
	}

	return TRUE;
}

//##ModelId=42046BE201E0
/**	\brief	숫자형 값 읽기 (LONG type)

	\param	lpszName	Label 이름

	\return	long
*/
long CProp::GetLongValue(TCHAR* lpszName) 
{
	unsigned long lValue, lResult, lSize;
	lSize = sizeof(lValue);
	DWORD dwType;
	lResult = RegQueryValueEx(m_hKey, lpszName, NULL, &dwType, (LPBYTE)&lValue, (LPDWORD)&lSize);
	if(lResult != ERROR_SUCCESS) return -1;
	return lValue;
}

//##ModelId=42046BE201E2
/**	\brief	문자형 값 읽기

	\param	lpszName	Label이름

	\return	TCHAR	
*/
TCHAR *CProp::GetValue(TCHAR* lpszName) 
{
	unsigned long lResult, lSize;
	TCHAR szValue[256];
	lSize = sizeof(szValue);
	DWORD dwType;
	lResult = RegQueryValueEx(m_hKey, lpszName, NULL, &dwType, (LPBYTE)szValue, (LPDWORD)&lSize);
	if(lResult != ERROR_SUCCESS) {
		DumpErr(lResult);
		_tcscpy(szReturn, TEXT(""));
		return szReturn;
	}
	_tcscpy(szReturn, szValue);
	return szReturn;
}


//##ModelId=42046BE201FE
/**	\brief	Label 존재여부 확인

	\param	lpszName	Label이름

	\return	BOOL
*/
BOOL CProp::IsExistValue(TCHAR* lpszName) 
{
	unsigned long lResult, lSize;
	TCHAR szValue[256];
	lSize = sizeof(szValue);
	DWORD dwType;
	lResult = RegQueryValueEx(m_hKey, lpszName, NULL, &dwType, (LPBYTE)szValue, (LPDWORD)&lSize);
	if(lResult != ERROR_SUCCESS) 
	{
		DumpErr(lResult);
		return FALSE;
	}
	return TRUE;
}

//##ModelId=42046BE20208
/**	\brief	숫자형 값 설정(Long type)

	\param	lpszName	Label명
	\param	lValue		값

	\return	BOOL
*/
BOOL CProp::SetValue(TCHAR* lpszName, long lValue) {
	unsigned long lResult;
	long value = lValue;
	unsigned long lSize = sizeof(value);

	lResult = RegSetValueEx(m_hKey, lpszName, 0, REG_DWORD, (BYTE const *)&value, (DWORD)lSize);
	if(lResult != ERROR_SUCCESS) {
		DumpErr(lResult);
		return FALSE;
	}
	return TRUE;
}

//##ModelId=42046BE2021C
/**	\brief	문자형 값 설정

	\param	lpszName	Label명
	\param	lpszValue	값

	\return	BOOL	
*/
BOOL CProp::SetValue(TCHAR* lpszName, TCHAR* lpszValue) {
	TCHAR szValue[256];
	lstrcpy(szValue, (TCHAR *)lpszValue);
	unsigned long lSize = sizeof(szValue);
	unsigned long lResult;

	lResult = RegSetValueEx(m_hKey, lpszName, 0, REG_SZ, (BYTE const *)szValue, lSize);
	if(lResult != ERROR_SUCCESS) {
		DumpErr(lResult);
		return FALSE;
	}
	return TRUE;
}

//##ModelId=42046BE20226
/**	\brief	Label 삭제

	\param	lpszName	Label명

	\return	BOOL
*/
BOOL CProp::DeleteValue(TCHAR* lpszName) {
	LONG lResult = RegDeleteValue(m_hKey, lpszName);
	if(lResult != ERROR_SUCCESS) 
	{
		DumpErr(lResult);
		return FALSE;
	}
	return TRUE;
}

//##ModelId=42046BE20230
/**	\brief	Section의 포함된 Label및 값 리스트 얻기

	\param	lIndex	a parameter of type long
	\param	lpszName	a parameter of type TCHAR*
	\param	lpszValue	a parameter of type TCHAR*

	\return	BOOL
*/
BOOL CProp::EnumStringValue(long lIndex, TCHAR* lpszName, TCHAR* lpszValue) {
	TCHAR szName[1000], szValue[1000];
	DWORD dwNameSize = sizeof(szName);
	DWORD dwValueSize = sizeof(szValue);
	DWORD dwType;
	DWORD dwIndex = (DWORD)lIndex;
	
	LONG lResult = RegEnumValue(m_hKey, dwIndex, szName, &dwNameSize, NULL, &dwType, (LPBYTE)szValue, &dwValueSize);
	if(lResult != ERROR_SUCCESS)
	{
		return FALSE;
	}
	if(lpszName) 
		lstrcpy((TCHAR *)lpszName, szName);
	if(lpszValue)
		lstrcpy((TCHAR *)lpszValue, szValue);
	return TRUE;
}

//##ModelId=42046BE20245
BOOL CProp::EnumKey(long lIndex, TCHAR* lpszName) {
	TCHAR szName[1000];
	DWORD dwNameSize = sizeof(szName);
	DWORD dwIndex = (DWORD)lIndex;
	
	LONG lResult = RegEnumKey(m_hKey, dwIndex, szName, dwNameSize);
	if(lResult != ERROR_SUCCESS)
	{
		return FALSE;
	}
	lstrcpy((TCHAR *)lpszName, szName);
	return TRUE;
}

BOOL CProp::CreateRegKey(TCHAR* lpszName)
{
	ULONG dwDisposition = REG_OPENED_EXISTING_KEY;
	REGSAM samDesired = KEY_ALL_ACCESS;
	HKEY hKey;

	//Need the name of the key to open
	if(!lpszName)
		return FALSE;

	LONG lResult = RegCreateKeyEx(m_hKey, lpszName, 0, NULL, REG_OPTION_NON_VOLATILE, samDesired, NULL, &hKey, &dwDisposition);

	if(lResult != ERROR_SUCCESS)
	{
		DumpErr(lResult);
		return FALSE;
	}
	lResult = RegCloseKey(hKey);

	return TRUE;
}

BOOL CProp::DeleteKey(TCHAR* strKeyName)
{
	LONG lResult = RegDeleteKey(m_hKey, strKeyName);
	if(lResult != ERROR_SUCCESS)
	{
		DumpErr(lResult);
		return FALSE;
	}

	return TRUE;
}


TCHAR* CProp::GetErrorMsg()
{
	return m_szErrMsg;
}

VOID CProp::DumpErr(LONG lErrNo)
{
	TCHAR* lpMsgBuf = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
				  NULL,
				  lErrNo,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPTSTR)&lpMsgBuf,
				  0,
				  NULL );
	_stprintf(m_szErrMsg, TEXT("[%d]%s"), lErrNo, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

BOOL CProp::EnumValue(LONG lIndex, TCHAR* lpszName, VARIANT *pvValue)
{
	TCHAR szName[1000], szValue[1000];
	DWORD dwNameSize = sizeof(szName);
	DWORD dwValueSize = sizeof(szValue);
	DWORD dwType;
	DWORD dwIndex = (DWORD)lIndex;
	
	LONG lResult = RegEnumValue(m_hKey, dwIndex, szName, &dwNameSize, NULL, &dwType, (LPBYTE)szValue, &dwValueSize);
	if(lResult != ERROR_SUCCESS)
	{
		return FALSE;
	}
	if(lpszName) 
		lstrcpy((TCHAR *)lpszName, szName);
	VariantInit(pvValue);
	switch (dwType)
	{
	case REG_BINARY:
	case REG_DWORD:
		VariantCopy(pvValue, &_variant_t(*((LONG*)szValue)));
		break;
	default:
		VariantCopy(pvValue, &_variant_t(szValue));
		break;
	}
	
	return TRUE;
}
