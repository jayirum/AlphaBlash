// talibTester.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <windows.h>
#include <stdio.h>
#include <ta_libc.h>

#if 0
#define MAX_CANDLE 10
#define PERIOD_FOR_EMA 3

double m_arrClose[] = { 100,10,500,1000,1,10000,200,30000,2,300 };

int main()
{
    int startIdx = 0;// MAX_CANDLE - PERIOD_FOR_EMA;
    int endIdx = MAX_CANDLE - 1;
    int inBuffSize = MAX_CANDLE;
    int outBegIdx = 0, outNbElement = 0;

    double outReal[10];// [PERIOD_FOR_EMA] ;
    double inReal[MAX_CANDLE];

    int nCopySize = sizeof(double) * MAX_CANDLE;
    memcpy(&inReal, &m_arrClose[0], nCopySize);

    //TA_RetCode ret = TA_EMA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, &outBegIdx, &outNbElement, &outReal[0]);
    TA_RetCode ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);
    if (ret != TA_SUCCESS)
    {
        //TODO sprintf(m_zMsg, "[TA_EMA] error code:%d", ret);
        return FALSE;
    }


    startIdx = 1; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 2; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 3; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 4; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 5; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 6; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 7; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 8; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 9; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], PERIOD_FOR_EMA, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    startIdx = 0; ZeroMemory(&outReal, sizeof(double) * 10);
    ret = TA_MA(startIdx, endIdx, &inReal[0], 9, TA_MAType_SMA, &outBegIdx, &outNbElement, &outReal[0]);

    int a = 0;
}
#endif 




/*
TA_RetCode TA_MA( int          startIdx,
                  int          endIdx,
                  const double inReal[],
                  int          optInTimePeriod,
                  int          optInMAType,
                  int         *outBegIdx,
                  int         *outNbElement,
                  double       outReal[],
                )

    # The output will be calculated only for the range specified by startIdx to endIdx.

    # One or more output are finally specified. In that example there is only one output which is outReal (the parameters outBegIdx and outNbElement are always specified once before the list of outputs).
*/


int main()
{
    TA_Real    inReal[] = { 11.25,12.35,14.5,18.45,12.75,15.35,13.05,16.10,12.20,11.65,13.25,15.30,14.85,16.15,19.05,21.45,17.55 };    //17

    TA_Real    outReal[17];
    
    TA_Integer outBeg;
    TA_Integer outNbElement;
    

    int startIdx = 0;
    int endIdx = 16;
    int period = 16;

    /* ... initialize your closing price here... */
    TA_RetCode retCode = TA_RSI(startIdx, endIdx, inReal, period, &outBeg, &outNbElement, outReal);
    
    return 0;
}


