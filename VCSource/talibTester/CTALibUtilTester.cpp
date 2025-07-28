#if 0

#include "../CommonAnsi/talibUtils.h"
#include <memory>

#include "../CommonAnsi/Util.h"

char org[] = "abc;defg;hijkl;";

int main()
{
    CSplit split;
    char deli = ';';
    string* res = NULL;
    res = split.Split(org, deli);
    int num = split.Count();;
    split.Clear();

    vector<string> vec;
    split.Split(org, deli, vec);
    getchar();
}
#endif


#if 0

#include <windows.h>
#include <stdio.h>
#include <ta_libc.h>

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

#define TOTAL_CNT 20
int main()
{
    TA_Real    outReal[TOTAL_CNT] = { -1, };
    TA_Real    closePrice[TOTAL_CNT] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 };

    TA_Integer outBeg;
    TA_Integer outNbElement;


    int period = 20;
    int startIdx = 0;
    int endIdx = period-1;

    /* ... initialize your closing price here... */
    TA_RetCode retCode = TA_MA(
        startIdx,
        endIdx,
        &closePrice[0],
        period,
        TA_MAType_SMA,
        &outBeg,
        &outNbElement,
        &outReal[0]
    );

    printf("startIdx:%d, endIdx:%d, period:%d, outBeg:%d, outNb:%d\n",
        startIdx, endIdx, period, outBeg, outNbElement);
    for (int i = 0; i < outNbElement; i++)
        printf("Day %d = %f\n", outBeg + i, outReal[i]);


    //TA_RetCode TA_RSI(int    startIdx,
    //    int    endIdx,
    //    const double inReal[],
    //    int           optInTimePeriod, /* From 2 to 100000 */
    //    int* outBegIdx,
    //    int* outNBElement,
    //    double        outReal[]);


    //TA_Real    closePrice1[TOTAL_CNT] = { 100,102,131,114,105,136,127,148,191,90,110,112,213,141,159,216,147,158,199,220 };

    //TA_RetCode retCode = TA_RSI(
    //    startIdx,
    //    endIdx,
    //    &closePrice1[0],
    //    period,
    //    &outBeg,
    //    &outNbElement,
    //    &outReal[0]
    //);

    //printf("startIdx:%d, endIdx:%d, period:%d, outBeg:%d, outNb:%d\n",
    //    startIdx, endIdx, period, outBeg, outNbElement);
    //for (int i = 0; i < outNbElement; i++)
    //    printf("Day %d = %f\n", outBeg + i, outReal[i]);


    return 0;
}

#endif


#if 0

void memory_test()
{
    std::auto_ptr<int> arr(new int(10240));

    //int* arr = new int[10240];
}

#define TOTAL_CNT 20
int main()
{
   

    TA_Real    outReal[TOTAL_CNT] = { -1, };
    TA_Real    closePrice[TOTAL_CNT] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 };

    int outBeg=0;
    int outNbElement=0;


    int startIdx = 0;
    int endIdx = 9;
    int period = 10;
    

    //int startIdx, int endIdx, int inBuffSize, int period, int outBegIdx, int outNbElement, int outBuffSize
    
    CTALibSMA	sma(startIdx, endIdx, TOTAL_CNT, period, TOTAL_CNT);
    if (!sma.Calc(&closePrice[0], &outReal[0]))
    {
        getchar();
    }
    outBeg = sma.OutBegIdx();
    outNbElement = sma.OutNbElement();

    getchar();
}
#endif

#if 0
#include <deque>
using namespace std;

struct TCandle
{
	char zClose[32];
};

deque<TCandle*> m_arrCandles;

int	ComposeArrayForSMA( int nPeriod, _Out_ double arrCandles[])
{
	int nLoop = 0;
	for (auto rit = m_arrCandles.crbegin(); rit != m_arrCandles.crend(); ++rit)
	{
		arrCandles[nLoop] = atof((*rit)->zClose);

		if (++nLoop == nPeriod)
			break;
	}

	return nLoop;
}

int main()
{
	for (double i = 1; i < 100; i++)
	{
		TCandle* p = new TCandle;
		sprintf(p->zClose, "%.5f", i);
		m_arrCandles.push_back(p);
	}

	int nPeriod = 10;
	int startIdx = 0;
	int endIdx = nPeriod - 1;
	int inBuffSize = nPeriod;

	std::auto_ptr<double> inReal(new double(inBuffSize));
	std::auto_ptr<double> outReal(new double(inBuffSize));


	int nArrCnt = ComposeArrayForSMA(nPeriod, inReal.get());

	CTALibSMA	sma(startIdx, endIdx, inBuffSize, nPeriod, inBuffSize);

	if (!sma.Calc(inReal.get(), outReal.get()))
	{
		//TODO. LOGGING
		return FALSE;
	}

	if (sma.OutBegIdx() != nPeriod-1 || sma.OutNbElement() != 1)
	{
		//TODO.
		return FALSE;
	}
	double result = outReal.get()[0];

	return 0;
}

#endif