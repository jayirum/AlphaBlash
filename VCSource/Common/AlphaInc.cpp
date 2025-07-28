#include "AlphaInc.h"
#include <stdio.h>
#include "Util.h"



#ifdef _UNICODE
void __ALPHA::ComposeEAConfigFileName(_In_ char* pzDir, _Out_ wchar_t* pwzFileName)
{
	wchar_t wzDir[1024] = { 0, };
	A2U(pzDir, wzDir);

	if (pzDir[_tcslen(wzDir) - 1] == '\\')
		_stprintf(pwzFileName, TEXT("%s%s"), wzDir, TEXT(CONFIG_FILE));
	else
		_stprintf(pwzFileName, TEXT("%s\\%s"), wzDir, TEXT(CONFIG_FILE));
}
#endif