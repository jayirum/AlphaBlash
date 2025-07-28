#include "CCrypt.h"
#include <stdio.h>
#include "AlphaInc.h"
#include "Util.h"

CCrypt::CCrypt()
{

}

CCrypt::~CCrypt()
{

}

int CCrypt::Enc(char* src, char* dest,  int len)
{
	int i;
	int key = KEY;

	if (!src || !dest || len <= 0) {
		sprintf(m_zMsg, "Wrong Parameter");
		return -1;
	}

	for ( i = 0; i < len; i++)
	{
		dest[i] = src[i] ^ key >> 8;
		if (dest[i] == 0x02)
		{
			sprintf(m_zMsg, "[Encrypt]Shift operating convert[%d](%c) to STX(%s)", i, src[i], &src[i]);
			CUtil::Assert(m_zMsg);
			return -1;
		}

		key = (dest[i] + key) * C1 + C2;
	}

	return i;
}


int CCrypt::Dec(char* src, char* dest, int len)
{
	int i;
	char prevBlock;
	int key = KEY;

	for (i = 0; i < len; i++)
	{
		prevBlock = src[i];
		dest[i] = src[i] ^ key >> 8;
		key = (prevBlock + key) * C1 + C2;
	}

	return i;
}