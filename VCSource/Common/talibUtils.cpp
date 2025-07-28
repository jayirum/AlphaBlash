#include "talibUtils.h"

//ENUM_BEGIN(RetCode)
///*      0 */  ENUM_DEFINE(TA_SUCCESS, Success),            /* No error */
///*      1 */  ENUM_DEFINE(TA_LIB_NOT_INITIALIZE, LibNotInitialize), /* TA_Initialize was not sucessfully called */
///*      2 */  ENUM_DEFINE(TA_BAD_PARAM, BadParam), /* A parameter is out of range */
///*      3 */  ENUM_DEFINE(TA_ALLOC_ERR, AllocErr), /* Possibly out-of-memory */
///*      4 */  ENUM_DEFINE(TA_GROUP_NOT_FOUND, GroupNotFound),
///*      5 */  ENUM_DEFINE(TA_FUNC_NOT_FOUND, FuncNotFound),
///*      6 */  ENUM_DEFINE(TA_INVALID_HANDLE, InvalidHandle),
///*      7 */  ENUM_DEFINE(TA_INVALID_PARAM_HOLDER, InvalidParamHolder),
///*      8 */  ENUM_DEFINE(TA_INVALID_PARAM_HOLDER_TYPE, InvalidParamHolderType),
///*      9 */  ENUM_DEFINE(TA_INVALID_PARAM_FUNCTION, InvalidParamFunction),
///*     10 */  ENUM_DEFINE(TA_INPUT_NOT_ALL_INITIALIZE, InputNotAllInitialize),
///*     11 */  ENUM_DEFINE(TA_OUTPUT_NOT_ALL_INITIALIZE, OutputNotAllInitialize),
///*     12 */  ENUM_DEFINE(TA_OUT_OF_RANGE_START_INDEX, OutOfRangeStartIndex),
///*     13 */  ENUM_DEFINE(TA_OUT_OF_RANGE_END_INDEX, OutOfRangeEndIndex),
///*     14 */  ENUM_DEFINE(TA_INVALID_LIST_TYPE, InvalidListType),
///*     15 */  ENUM_DEFINE(TA_BAD_OBJECT, BadObject),
///*     16 */  ENUM_DEFINE(TA_NOT_SUPPORTED, NotSupported),
///*   5000 */  ENUM_DEFINE(TA_INTERNAL_ERROR, InternalError) = 5000,
///* 0xFFFF */  ENUM_DEFINE(TA_UNKNOWN_ERR, UnknownErr) = 0xFFFF



CTALibBase::CTALibBase()
{
	m_startIdx		= -1;
	m_endIdx		= -1;
	m_inBuffSize	= -1;
	m_outBuffSize	= 0;
	m_period		= -1;

	m_outBegIdx		= -1;
	m_outNbElement	= -1;

	ZeroMemory(m_zMsg, sizeof(m_zMsg));
}

//CTALibBase::CTALibBase(int startIdx, int endIdx, int inBuffSize, int period, int outBegIdx, int outNbElement, int outBuffSize)
//{
//	m_startIdx		= startIdx;
//	m_endIdx		= endIdx;
//	m_inBuffSize	= inBuffSize;
//	m_period		= period;
//	m_outBegIdx		= outBegIdx;
//	m_outNbElement	= outNbElement;
//	m_outBuffSize	= outBuffSize;
//}

CTALibBase::~CTALibBase()
{}



TAUTIL_RET CTALibBase::Validate()
{
	if (m_startIdx < 0 || m_endIdx < 1 || m_inBuffSize < 1 || m_outBuffSize < 1 || m_period < 2)
		return TAUTIL_INVALID_NUMBER;

	// outbuffer <= inpub buffer
	if (m_outBuffSize > m_inBuffSize)
	{
		return TAUTIL_OUTBUFF_SIZEOVER;
	}

	// startIdx <= endIdx
	if (m_startIdx > m_endIdx)
	{
		return TAUTIL_STARTIDX_OVER;
	}

	//TODO
	//if (m_endIdx > m_outBuffSize)
	//{
	//	return TAUTIL_ENDIDX_OVER_OUTBUFFSIZE;
	//}

	//if (m_period > m_inBuffSize || m_period > m_endIdx + 1 || m_period > m_outBuffSize)
	//{
	//	return TAUTIL_PERIOD_OVER;
	//}

	return TAUTIL_OK;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//			CTALibSma
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTALibSma::CTALibSma(int startIdx, int endIdx, int inBuffSize, int period, int outBuffSize):CTALibBase()
{
	m_startIdx		= startIdx;
	m_endIdx		= endIdx;
	m_inBuffSize	= inBuffSize;
	m_period		= period;
	m_outBuffSize	= outBuffSize;
}
CTALibSma::~CTALibSma()
{}


TAUTIL_RET CTALibSma::Validate()
{
	return CTALibBase::Validate();
}

//TAUTIL_RETCode TA_SMA(int    startIdx,
//	int    endIdx,
//	const double inReal[],
//	int           optInTimePeriod, /* From 2 to 100000 */
//	int* outBegIdx,
//	int* outNBElement,
//	double        outReal[]);

BOOL CTALibSma::Calc(const double inReal[], double outReal[])
{
	if (Validate() != TAUTIL_OK)
		return FALSE;

	TA_RetCode ret = TA_SMA(m_startIdx, m_endIdx, &inReal[0], m_period, &m_outBegIdx, &m_outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS)
	{
		sprintf(m_zMsg, "[TA_SMA] error code:%d", ret);
		return FALSE;
	}

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//			CTALibEma
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTALibEma::CTALibEma(int startIdx, int endIdx, int inBuffSize, int period, int outBuffSize) :CTALibBase()
{
	m_startIdx		= startIdx;
	m_endIdx		= endIdx;
	m_inBuffSize	= inBuffSize;
	m_period		= period;
	m_outBuffSize	= outBuffSize;

	if (m_endIdx >= m_inBuffSize)
		m_endIdx = m_inBuffSize - 1;

	if (m_period > m_inBuffSize)
		m_period = m_inBuffSize;

}
CTALibEma::~CTALibEma()
{}


TAUTIL_RET CTALibEma::Validate()
{
	return CTALibBase::Validate();
}

//TA_RetCode TA_EMA(
//	int    startIdx,
//	int    endIdx,
//	const double inReal[],
//	int           optInTimePeriod, /* From 2 to 100000 */
//	int* outBegIdx,
//	int* outNBElement,
//	double        outReal[]);

BOOL CTALibEma::Calc(const double inReal[], double outReal[])
{
	if (Validate() != TAUTIL_OK)
		return FALSE;


	TA_RetCode ret = TA_EMA(m_startIdx, m_endIdx, &inReal[0], m_period, &m_outBegIdx, &m_outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS)
	{
		sprintf(m_zMsg, "[TA_EMA] error code:%d", ret);
		return FALSE;
	}

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//			CTALibRsi
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTALibRsi::CTALibRsi(int startIdx, int endIdx, int inBuffSize, int period, int outBuffSize) :CTALibBase()
{
	m_startIdx		= startIdx;
	m_endIdx		= endIdx;
	m_inBuffSize	= inBuffSize;
	m_period		= period;
	m_outBuffSize	= outBuffSize;
}
CTALibRsi::~CTALibRsi()
{}


TAUTIL_RET CTALibRsi::Validate()
{
	return CTALibBase::Validate();
}

//TA_RetCode TA_RSI(
//	int    startIdx,
//	int    endIdx,
//	const double inReal[],
//	int           optInTimePeriod, /* From 2 to 100000 */
//	int* outBegIdx,
//	int* outNBElement,
//	double        outReal[]);

BOOL CTALibRsi::Calc(const double inReal[], double outReal[])
{
	if (Validate() != TAUTIL_OK)
		return FALSE;

	TA_RetCode ret = TA_RSI(m_startIdx, m_endIdx, &inReal[0], m_period, &m_outBegIdx, &m_outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS)
	{
		sprintf(m_zMsg, "[TA_RSI] error code:%d", ret);
		return FALSE;
	}

	return TRUE;
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//			CTALibRsi
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CTALibStdDev::CTALibStdDev(int startIdx, int endIdx, int inBuffSize, int period, int outBuffSize) :CTALibBase()
{
	m_startIdx = startIdx;
	m_endIdx = endIdx;
	m_inBuffSize = inBuffSize;
	m_period = period;
	m_outBuffSize = outBuffSize;
}
CTALibStdDev::~CTALibStdDev()
{}


TAUTIL_RET CTALibStdDev::Validate()
{
	return CTALibBase::Validate();
}

//TA_RetCode TA_STDDEV(
//	int    startIdx,
//	int    endIdx,
//	const double inReal[],
//	int           optInTimePeriod, /* From 2 to 100000 */
//	double        optInNbDev, /* From TA_REAL_MIN to TA_REAL_MAX */
//	int* outBegIdx,
//	int* outNBElement,
//	double        outReal[]);

BOOL CTALibStdDev::Calc(const double inReal[], double outReal[])
{
	if (Validate() != TAUTIL_OK)
		return FALSE;

	int optInNbDev = 1;	//default value
	TA_RetCode ret = TA_STDDEV(m_startIdx, m_endIdx, &inReal[0], m_period, optInNbDev, &m_outBegIdx, &m_outNbElement, &outReal[0]);
	if (ret != TA_SUCCESS)
	{
		sprintf(m_zMsg, "[TA_RSI] error code:%d", ret);
		return FALSE;
	}

	return TRUE;
}