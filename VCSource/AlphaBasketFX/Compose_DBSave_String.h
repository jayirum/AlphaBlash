#pragma once

#include "../Common/AlphaInc.h"

void AlphaBasket_Save_CloseMT4(int iSymbol, _In_ TData* pData, _Out_ char* pzBuffer);
void AlphaBasket_Save_CloseTriggered(int iSymbol, _In_ TData* pData, BOOL bMarketClose, _Out_ char* pzBuffer);
void AlphaBasket_Save_OpenMT4(int iSymbol, _In_ TData* pData, _Out_ char* pzBuffer);
void AlphaBasket_Save_OpenTriggered(int iSymbol, _In_ TData* pData, _Out_ char* pzBuffer);
void AlphaBasket_Error(int iSymbol, char cBuySellTp, _In_ TData* pData, _In_ char* pzErrMsg, _Out_ char* pzBuffer);