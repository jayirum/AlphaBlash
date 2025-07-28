#include "CUtils.h"

#include <System.hpp>
#include <System.StrUtils.hpp>
#include <Iphlpapi.h>
#include <Assert.h>
#include <deque>
using namespace std;
#pragma comment(lib, "iphlpapi.lib")



///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
void __CheckBool(bool res, char* msg)
{
	if(!res)
		throw msg;
}


///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////

String CUtils::GetPureModuleName()
{
	String res;
    wchar_t wzModule[_MAX_PATH]={0};
	GetModuleFileName(NULL, wzModule, _MAX_PATH);
	String sName = wzModule;

	TStringDynArray arr = SplitString(sName, L"\\");

	res = arr[arr.Length -1];

	return res;
}


char*  CUtils::GetMacAddr(_Out_ char* pMac)
{
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	char *mac_addr = new char[18];

	AdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL) {
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		free(mac_addr);
		return NULL; // it is safe to call free(NULL)
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(AdapterInfo);
		AdapterInfo = (IP_ADAPTER_INFO *) malloc(dwBufLen);
		if (AdapterInfo == NULL)
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			free(mac_addr);
			return NULL;
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
	{
		// Contains pointer to current adapter info
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do
		{
			// technically should look at pAdapterInfo->AddressLength
			//   and not assume it is 6.
			sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
			pAdapterInfo->Address[0], pAdapterInfo->Address[1],
			pAdapterInfo->Address[2], pAdapterInfo->Address[3],
			pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			// print them all, return the last one.
			// return mac_addr;

			printf("\n");
			pAdapterInfo = pAdapterInfo->Next;
		} while(pAdapterInfo);
	}
	free(AdapterInfo);

	strcpy(pMac, mac_addr);
	delete[] mac_addr;
	return pMac; // caller must free.
}



char*	CUtils::FormatMoney(double dMoney, int nDotCnt, _Out_ char* buf)
{
	//ZeroMemory(buf, bufsize);
	char org[128]={0}, points[128]={0}, decimal[128]={0};
	sprintf(org, "%.*f", nDotCnt, dMoney);

	char* pPoint = strchr(org, '.');
	if(pPoint)
	{
		sprintf(points, "%.*s", strlen(pPoint), pPoint);
		*pPoint = NULL;
	}

	strcpy( decimal, org);

	deque<char> deq;
	int len = strlen(decimal);
	int nAdd = 0;
	for( int k=len-1; k>-1; k-- )
	{
		deq.push_front(decimal[k]);
		nAdd++;


		if(nAdd==3){
			deq.push_front(',');
			nAdd = 0;
		}
	}
	if( deq[0]==',' )
		deq.pop_front();

	for( int i=0; i<deq.size(); i++) {
		buf[i] = deq[i];
		buf[i+1] = 0;
	}

	strcat(buf, points);

	return buf;
}



////////////////////////////////////////////////////////////////////////////
//
// CIniFile
//
////////////////////////////////////////////////////////////////////////////

AnsiString CIniFile::GetCnfgFileName(AnsiString sExtension/*="ini"*/)
{
	wchar_t wzModule[_MAX_PATH]={0};
	GetModuleFileName(NULL, wzModule, _MAX_PATH);
	AnsiString sModuleName = AnsiString(wzModule);

	// replace exe with ini (d:\\proj\\myapp.exe ==> d:\\proj\\myapp.ini )
	AnsiString sName;
	sName.sprintf("%.*s", sModuleName.Length()-3, sModuleName);
	m_sIniFile = AnsiString(sName + sExtension);

	return m_sIniFile;
}


AnsiString CIniFile::GetVal(AnsiString sSec, AnsiString sKey)
{
	char zVal[1024]={0};
	DWORD dwRet = GetPrivateProfileStringA(sSec.c_str(), sKey.c_str(), NULL, zVal, 1024, m_sIniFile.c_str());
	if (dwRet == 0)
		return "";

	// remove comments after "//"
	// except the case of [http://] or [https://]

	char* http = strstr(zVal, "http://");
	char* https = strstr(zVal, "https://");

	if (!http && !https)
	{
		char* pComment = strstr(zVal, "//");
		if (pComment)	*(pComment) = 0x00;

		// tab
		char* pTab = strstr(zVal, "\t");
		if (pTab)	*(pTab) = 0x00;
	}
	else
	{
		if( http )
		{
			char* pComment = strstr(http+6, "//");
			if (pComment)
				*(pComment) = 0x00;
		}
		if (https)
		{
			char* pComment = strstr(https + 7, "//");
			if (pComment)
				*(pComment) = 0x00;
		}
	}
	return zVal;
}


bool CIniFile::SetVal(AnsiString sSec, AnsiString sKey, AnsiString sVal)
{
	return WritePrivateProfileStringA(sSec.c_str(), sKey.c_str(), sVal.c_str(), m_sIniFile.c_str());
}

