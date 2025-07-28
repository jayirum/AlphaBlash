#pragma once

/*
link python38.lib;
add directory
	C:\Users\JAYKIM\AppData\Local\Programs\Python\Python38-32\include;
	C:\Users\JAYKIM\AppData\Local\Programs\Python\Python38-32\libs


UNICODE
*/
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <Windows.h>
#include <wchar.h>

#pragma comment(lib, "python38.lib")


class CPythonEmailSender
{

public:
	CPythonEmailSender();
	~CPythonEmailSender();
	
	void SetEmailInfo(TCHAR* lpPyFileName, TCHAR* lpFuncName, TCHAR* lpSenderAcc, TCHAR* lpSenderDevicePwd, TCHAR* lpReceiverAcc);

	BOOL	SendEmail(char* lpTitle, char* lpBody);
	char* GetMsg() { return m_zMsg; }

	char* Receiver() { return m_zReceiverAcc; }

private:
	BOOL	Initialize();
	VOID	DeInitialize();

	BOOL	Init_Python();
	void	DeInit_Python();
private:
	PyObject*	m_PyFileName;
	PyObject*	m_PyMudle;
	PyObject*	m_PyFunc;


	char	m_zPyFileName[128];
	char	m_zPyFuncName[128];
	char	m_zSenderAcc[128];
	char	m_zSenderDevicePwd[128];
	char	m_zReceiverAcc[128];

	char	m_zMsg[512];
};

