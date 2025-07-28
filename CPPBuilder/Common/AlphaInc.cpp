#include "AlphaInc.h"
//#include <stdio.h>
//#include "Util.h"



unsigned int ReportException(DWORD dExitCode, const char* psPos, _Out_ char* pzMsgBuff) // 20120510
{
	switch (dExitCode)
	{
	case EXCEPTION_ACCESS_VIOLATION: 		sprintf(pzMsgBuff, "[%s]EXCEPTION_ACCESS_VIOLATION", psPos);		break;
	case EXCEPTION_BREAKPOINT: 				sprintf(pzMsgBuff, "[%s]EXCEPTION_BREAKPOINT", psPos);		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT: 	sprintf(pzMsgBuff, "[%s]EXCEPTION_DATATYPE_MISALIGNMENT", psPos);		break;
	case EXCEPTION_SINGLE_STEP: 			sprintf(pzMsgBuff, "[%s]EXCEPTION_SINGLE_STEP", psPos);		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: 	sprintf(pzMsgBuff, "[%s]EXCEPTION_ARRAY_BOUNDS_EXCEEDED", psPos);		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND: 	sprintf(pzMsgBuff, "[%s]EXCEPTION_FLT_DENORMAL_OPERAND", psPos);		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: 		sprintf(pzMsgBuff, "[%s]EXCEPTION_FLT_DIVIDE_BY_ZERO", psPos);		break;
	case EXCEPTION_FLT_INEXACT_RESULT: 		sprintf(pzMsgBuff, "[%s]EXCEPTION_FLT_INEXACT_RESULT", psPos);		break;
	case EXCEPTION_FLT_INVALID_OPERATION: 	sprintf(pzMsgBuff, "[%s]EXCEPTION_FLT_INVALID_OPERATION", psPos);		break;
	case EXCEPTION_FLT_OVERFLOW: 			sprintf(pzMsgBuff, "[%s]EXCEPTION_FLT_OVERFLOW", psPos);		break;
	case EXCEPTION_FLT_STACK_CHECK: 		sprintf(pzMsgBuff, "[%s]EXCEPTION_FLT_STACK_CHECK", psPos);		break;
	case EXCEPTION_FLT_UNDERFLOW: 			sprintf(pzMsgBuff, "[%s]EXCEPTION_FLT_UNDERFLOW", psPos);		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO: 		sprintf(pzMsgBuff, "[%s]EXCEPTION_INT_DIVIDE_BY_ZERO", psPos);		break;
	case EXCEPTION_INT_OVERFLOW: 			sprintf(pzMsgBuff, "[%s]EXCEPTION_INT_OVERFLOW", psPos);		break;
	case EXCEPTION_PRIV_INSTRUCTION: 		sprintf(pzMsgBuff, "[%s]EXCEPTION_PRIV_INSTRUCTION", psPos);		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:sprintf(pzMsgBuff, "[%s]EXCEPTION_NONCONTINUABLE_EXCEPTION", psPos);		break;
	default:sprintf(pzMsgBuff, "[except code:%d]undefined error", dExitCode); break;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}


//
//BOOL __ALPHA::IsMaster(string sTp)
//{
//	return (sTp.at(0) == __ALPHA::TP_MASTER);
//}
//
//
//char* __ALPHA::enMasterKey(const char* id, const char* accno, char* out)
//{
//	//_stprintf(out, "%*.*s%*.*s", SIZE_USER_ID, SIZE_USER_ID, id, SIZE_ACCNO, SIZE_ACCNO, accno);
//	sprintf(out, "%.*s", SIZE_USER_ID, id);
//	return out;
//}
//
//void __ALPHA::deMasterKey(const char* key, /*out*/ char* id, /*out*/char* accno)
//{
//	sprintf(id, "%.*s", SIZE_USER_ID, key);
//
//	sprintf(accno, "%.*s", SIZE_ACCNO, key + SIZE_USER_ID);
//}
//
//
//int __ALPHA::getMT4Cmd_MarketBuy()
//{
//	return ORDER_TYPE_BUY;
//}
//
//int __ALPHA::getMT4Cmd_MarketSell()
//{
//	return ORDER_TYPE_SELL;
//}

/*
ORDER_TYPE_BUY = 0
, ORDER_TYPE_SELL
, ORDER_TYPE_BUY_LIMIT
, ORDER_TYPE_SELL_LIMIT
, ORDER_TYPE_BUY_STOP
, ORDER_TYPE_SELL_STOP
, ORDER_TYPE_BUY_STOP_LIMIT
, ORDER_TYPE_SELL_STOP_LIMIT
*/
//char* __ALPHA::getMT4CmdDesc(int nCmd, char* pOut)
//{
//	switch (nCmd)
//	{
//	case ORDER_TYPE_BUY: strcpy(pOut, "BUY_MARKET"); break;
//	case ORDER_TYPE_SELL:strcpy(pOut, "SELL_MARKET"); break;
//	case ORDER_TYPE_BUY_LIMIT:strcpy(pOut, "BUY_LIMIT"); break;
//	case ORDER_TYPE_SELL_LIMIT: strcpy(pOut, "SELL_LIMIT"); break;
//	case ORDER_TYPE_BUY_STOP: strcpy(pOut, "BUY_STOP"); break;
//	case ORDER_TYPE_SELL_STOP: strcpy(pOut, "SELL_STOP"); break;
//	case ORDER_TYPE_BUY_STOP_LIMIT: strcpy(pOut, "BUY_STOPLIMIT"); break;
//	case ORDER_TYPE_SELL_STOP_LIMIT: strcpy(pOut, "SELL_STOPLIMIT"); break;
//	}
//	return pOut;
//}
//
//
//bool __ALPHA::IsBuyOrder(int nCmd)
//{
//	if (nCmd == ORDER_TYPE_BUY ||
//		nCmd == ORDER_TYPE_BUY_LIMIT ||
//		nCmd == ORDER_TYPE_BUY_STOP ||
//		nCmd == ORDER_TYPE_BUY_STOP_LIMIT
//		)
//	{
//		return TRUE;
//	}
//
//	return FALSE;
//}
//
//
//char* __ALPHA::getBuySellString(int nCmd, char* pOut)
//{
//	if (nCmd == ORDER_TYPE_BUY ||
//		nCmd == ORDER_TYPE_BUY_LIMIT ||
//		nCmd == ORDER_TYPE_BUY_STOP ||
//		nCmd == ORDER_TYPE_BUY_STOP_LIMIT
//		)
//	{
//		strcpy(pOut, "BUY");
//	}
//	else
//		strcpy(pOut, "SELL");
//
//	return pOut;
//}


//
//
//int __ALPHA::TimeFrameIdx(const char* pzTimeFrame)
//{
//	int nIdx = -1;
//	if (strcmp(pzTimeFrame, TIMEFRAME_M1) == 0)	nIdx = IDX_1M;
//	else if (strcmp(pzTimeFrame, TIMEFRAME_M5) == 0)	nIdx = IDX_5M;
//	else if (strcmp(pzTimeFrame, TIMEFRAME_M15) == 0)	nIdx = IDX_1M;
//	else if (strcmp(pzTimeFrame, TIMEFRAME_M30) == 0)	nIdx = IDX_30M;
//	else if (strcmp(pzTimeFrame, TIMEFRAME_H1) == 0)	nIdx = IDX_1H;
//	else if (strcmp(pzTimeFrame, TIMEFRAME_H4) == 0)	nIdx = IDX_4H;
//	else if (strcmp(pzTimeFrame, TIMEFRAME_D1) == 0)	nIdx = IDX_1D;
//	else if (strcmp(pzTimeFrame, TIMEFRAME_W1) == 0)	nIdx = IDX_1W;
//	else if (strcmp(pzTimeFrame, TIMEFRAME_MN1) == 0)	nIdx = IDX_1MN;
//
//	return nIdx;
//}
//
//#ifdef _UNICODE
//void __ALPHA::ComposeEAConfigFileName(_In_ char* pzDir, _Out_ wchar_t* pwzFileName)
//{
//	wchar_t wzDir[1024] = { 0, };
//	A2U(pzDir, wzDir);
//
//	if (pzDir[_tcslen(wzDir) - 1] == '\\')
//		_stprintf(pwzFileName, TEXT("%s%s"), wzDir, TEXT(CONFIG_FILE));
//	else
//		_stprintf(pwzFileName, TEXT("%s\\%s"), wzDir, TEXT(CONFIG_FILE));
//}
//#endif
