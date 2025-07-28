#include "PythonEmailSender.h"
#include <stdio.h>
#include "../CommonAnsi/Util.h"
#include "../CommonAnsi/LogMsg.h"

extern CLogMsg		g_log;

CPythonEmailSender::CPythonEmailSender()
{
	m_PyFileName = NULL;
	m_PyMudle = NULL;
	m_PyFunc = NULL;

	
	ZeroMemory(m_zPyFileName, sizeof(m_zPyFileName));
	ZeroMemory(m_zPyFuncName, sizeof(m_zPyFuncName));
	ZeroMemory(m_zSenderAcc, sizeof(m_zSenderAcc));
	ZeroMemory(m_zSenderDevicePwd, sizeof(m_zSenderDevicePwd));
	ZeroMemory(m_zReceiverAcc, sizeof(m_zReceiverAcc));
	ZeroMemory(m_zMsg, sizeof(m_zMsg));
}

CPythonEmailSender::~CPythonEmailSender()
{}


void CPythonEmailSender::SetEmailInfo(TCHAR* lpPyFileName, TCHAR* lpFuncName, TCHAR* lpSenderAcc, TCHAR* lpSenderDevicePwd, TCHAR* lpReceiverAcc)
{
	strcpy(m_zPyFileName, lpPyFileName);
	strcpy(m_zPyFuncName, lpFuncName);
	strcpy(m_zSenderAcc, lpSenderAcc);
	strcpy(m_zSenderDevicePwd, lpSenderDevicePwd);
	strcpy(m_zReceiverAcc, lpReceiverAcc);
}
BOOL CPythonEmailSender::Initialize()
{
	Init_Python();

	m_PyFileName = PyUnicode_DecodeFSDefault(m_zPyFileName);	// _tcslen(m_zPyFileName));
	if (m_PyFileName == NULL)
	{
		sprintf(m_zMsg, "Error in getting python file name(%s)", m_zPyFileName);
		return FALSE;
	}

	m_PyMudle = PyImport_Import(m_PyFileName);
	if (m_PyMudle == NULL)
	{
		sprintf(m_zMsg, "Error in Importing python module(%s)", m_zPyFileName);
		return FALSE;
	}

	m_PyFunc = PyObject_GetAttrString(m_PyMudle, (char*) m_zPyFuncName);
	if (sprintf == NULL)
	{
		sprintf(m_zMsg, "Error in getting function(%s)", m_zPyFuncName);
		return FALSE;
	}

	if (PyCallable_Check(m_PyFunc) == 0)
	{
		sprintf(m_zMsg, "%s is not callable", m_zPyFuncName);
		return FALSE;
	}

	return TRUE;
}

BOOL CPythonEmailSender::Init_Python()
{
	Py_Initialize();
	return TRUE;
}

void CPythonEmailSender::DeInit_Python()
{
	Py_XDECREF(m_PyFileName);
	Py_XDECREF(m_PyMudle);
	Py_XDECREF(m_PyFunc);
	Py_FinalizeEx();
}

VOID CPythonEmailSender::DeInitialize()
{
	DeInit_Python();
}

BOOL CPythonEmailSender::SendEmail(char* lpTitle, char* lpBody)
{
	PyObject* pArgs = NULL;
	PyObject* pValue = NULL;
	BOOL bRet = TRUE;
	__try
	{
		__try
		{
			if (Initialize() == FALSE) 
			{
				g_log.Log(ERR, m_zMsg);
				return FALSE;
			}

			// sender, senderdevicepwd, receiver, title, body
			pArgs = PyTuple_New(5);

			pValue = PyUnicode_FromString(m_zSenderAcc);
			PyTuple_SetItem(pArgs, 0, pValue);

			pValue = PyUnicode_FromString(m_zSenderDevicePwd);
			PyTuple_SetItem(pArgs, 1, pValue);

			pValue = PyUnicode_FromString(m_zReceiverAcc);
			PyTuple_SetItem(pArgs, 2, pValue);

			wchar_t wTitle[128] = { 0, };
			int nLen = MultiByteToWideChar(CP_ACP, 0, lpTitle, -1, NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, lpTitle, -1, wTitle, nLen);
			pValue = PyUnicode_FromWideChar(wTitle, -1);
			PyTuple_SetItem(pArgs, 3, pValue);

			wchar_t wBody[128] = { 0, };
			nLen = MultiByteToWideChar(CP_ACP, 0, lpBody, -1, NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, lpBody, -1, wBody, nLen);
			pValue = PyUnicode_FromWideChar(wBody, -1);
			PyTuple_SetItem(pArgs, 4, pValue);

			pValue = PyObject_CallObject(m_PyFunc, pArgs);
			if (pValue == NULL) 
			{
				sprintf(m_zMsg, "Error in Sending Email(acc:%s)(pwd:%s)(receiver:%s)", m_zSenderAcc, m_zSenderDevicePwd, m_zReceiverAcc);
				g_log.Log(ERR, m_zMsg);
				bRet = FALSE;
			}
			else
			{
				//g_log.log(INFO, "[SendEmail](%s)(%.80s)", lpTitle, lpBody);
			}
		}
		__except(ReportException(GetExceptionCode(), "SendEmail", (char*)m_zMsg))
		{
			g_log.log(ERR, "SendEmail Exception:%s", m_zMsg);
			bRet = FALSE;
		}
	}
	__finally
	{
		Py_XDECREF(pArgs);
		Py_XDECREF(pValue);
		DeInitialize();
	}
	
	return bRet;
}