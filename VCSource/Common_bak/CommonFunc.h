#pragma once
#pragma warning(disable:4996)

#include <Windows.h>
#include <tchar.h>

BOOL Comm_DBSave_TraceOrder(void* pDBPool, const char* pOrdData, int nDataLen, _Out_ wchar_t* pzMsg);
BOOL Comm_Compose_ConfigSymbol_Master(void* pDBPool, const char* pzUserID, const char* pzMT4Acc,
									_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg);
BOOL Comm_Compose_ConfigSymbol_MasterW(void* pDBPool, const wchar_t* pzUserID, const wchar_t* pzMT4Acc,
									_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg);


BOOL Comm_Compose_ConfigSymbol_Copier(void* pDBPool, const char* pzUserID, const char* pzMT4Acc, 
									_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg);
BOOL Comm_Compose_ConfigSymbol_CopierW(void* pDBPool, const wchar_t* pzUserID, const wchar_t* pzMT4Acc,
									_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg);



BOOL Comm_Compose_ConfigGeneral_Master
(void* pDBPool, const char* pzUserID, const char* pzMT4Acc, _Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg);
BOOL Comm_Compose_ConfigGeneral_MasterW(void* pDBPool, const wchar_t* pzUserID, const wchar_t* pzMT4Acc,
									_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg);

BOOL Comm_Compose_ConfigGeneral_Copier(void* pDBPool, const char* pzUserID, const char* pzMT4Acc, 
									_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg);
BOOL Comm_Compose_ConfigGeneral_CopierW(void* pDBPool, const wchar_t* pzUserID, const wchar_t* pzMT4Acc,
									_Out_ char* pzRslt, _Out_ int* pnRslt, _Out_ TCHAR* pzErrMsg);