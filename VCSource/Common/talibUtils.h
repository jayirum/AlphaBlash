#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:4700)

#include <Windows.h>
#include <ta_libc.h>


enum TAUTIL_RET {
	TAUTIL_OK = 0
	,TAUTIL_OUTBUFF_SIZEOVER
	,TAUTIL_STARTIDX_OVER
	,TAUTIL_ENDIDX_OVER_OUTBUFFSIZE
	,TAUTIL_PERIOD_OVER
	,TAUTIL_INVALID_NUMBER
};

class CTALibBase
{
public:
	CTALibBase();
	//CTALibBase(int startIdx, int endIdx, int inBuffSize, int period, int outBegIdx, int outNbElement, int outBuffSize);

	virtual ~CTALibBase();

	//int		OutBegIdx()		{ return m_outBegIdx;}
	int		OutNbElement()	{ return m_outNbElement; }
	
	virtual BOOL	Calc(const double inReal[], double outReal[])  = 0;

	char* GetMsg() { return m_zMsg; }
protected:
	virtual TAUTIL_RET	Validate();
	

protected:

	int		m_startIdx;
	int		m_endIdx;
	int		m_inBuffSize;
	int		m_outBuffSize;
	int		m_period;

	int		m_outBegIdx;
	int		m_outNbElement;

	char	m_zMsg[512];
};
/*
m_startIdx : start index of the input
m_endIdx   : end index of the input
m_inBuffSize : size of the input buffer
m_outBuffSize: size of the output buffer
m_period     : the unit size for calculation
m_outBegIdx  : in the input buffer, the start idx to be covered period
m_outNbElement : the numbers of the output
*/



/*
 * TA_SMA - Simple Moving Average
 *
 * Input  = double
 * Output = double
 *
 * Optional Parameters
 * -------------------
 * optInTimePeriod:(From 2 to 100000)
 *    Number of period
 *
 *
 */
//TAUTIL_RETCode TA_SMA(int    startIdx,
//	int    endIdx,
//	const double inReal[],
//	int           optInTimePeriod, /* From 2 to 100000 */
//	int* outBegIdx,
//	int* outNBElement,
//	double        outReal[]);
class CTALibSma : public CTALibBase
{
public:
	CTALibSma(int startIdx, int endIdx, int inBuffSize, int period, int outBuffSize);
	virtual ~CTALibSma();

	virtual BOOL	Calc(const double inReal[], double outReal[]);
protected:
	TAUTIL_RET	Validate();

};



/*
 * TA_EMA - Exponential Moving Average
 *
 * Input  = double
 * Output = double
 *
 * Optional Parameters
 * -------------------
 * optInTimePeriod:(From 2 to 100000)
 *    Number of period
 *
 *
 */
//TA_RetCode TA_EMA(
//	int    startIdx,
//	int    endIdx,
//	const double inReal[],
//	int           optInTimePeriod, /* From 2 to 100000 */
//	int* outBegIdx,
//	int* outNBElement,
//	double        outReal[]);
class CTALibEma : public CTALibBase
{
public:
	CTALibEma(int startIdx, int endIdx, int inBuffSize, int period, int outBuffSize);
	virtual ~CTALibEma();

	virtual BOOL	Calc(const double inReal[], double outReal[]);
protected:
	TAUTIL_RET	Validate();

};



/*
 * TA_RSI - Relative Strength Index
 *
 * Input  = double
 * Output = double
 *
 * Optional Parameters
 * -------------------
 * optInTimePeriod:(From 2 to 100000)
 *    Number of period
 *
 *
 */
//TA_RetCode TA_RSI(
//	int    startIdx,
//	int    endIdx,
//	const double inReal[],
//	int           optInTimePeriod, /* From 2 to 100000 */
//	int* outBegIdx,
//	int* outNBElement,
//	double        outReal[]);
class CTALibRsi : public CTALibBase
{
public:
	CTALibRsi(int startIdx, int endIdx, int inBuffSize, int period, int outBuffSize);
	virtual ~CTALibRsi();

	virtual BOOL	Calc(const double inReal[], double outReal[]);
protected:
	TAUTIL_RET	Validate();

};



/*
 * TA_STDDEV - Standard Deviation
 *
 * Input  = double
 * Output = double
 *
 * Optional Parameters
 * -------------------
 * optInTimePeriod:(From 2 to 100000)
 *    Number of period
 *
 * optInNbDev:(From TA_REAL_MIN to TA_REAL_MAX)
 *    Nb of deviations
 *
 *
 */
//TA_RetCode TA_STDDEV(
//	int    startIdx,
//	int    endIdx,
//	const double inReal[],
//	int           optInTimePeriod, /* From 2 to 100000 */
//	double        optInNbDev, /* From TA_REAL_MIN to TA_REAL_MAX */
//	int* outBegIdx,
//	int* outNBElement,
//	double        outReal[]);
class CTALibStdDev : public CTALibBase
{
public:
	CTALibStdDev(int startIdx, int endIdx, int inBuffSize, int period, int outBuffSize);
	virtual ~CTALibStdDev();

	virtual BOOL	Calc(const double inReal[], double outReal[]);
protected:
	TAUTIL_RET	Validate();

};