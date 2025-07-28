#pragma once

#pragma warning( disable : 26495)
#include "../Common/AlphaInc.h"
#include "../CommonAnsi/LogMsg.h"
#include <deque>
#include <memory>
#include <ta_libc.h>
using namespace std;

/*
* ORDSTATUS_OPEN_TRIGGERED  - Sending Open order to MT4
* ORDSTATUS_OPEN_MT4        - receive response of open order from MT4
* ORDSTATUS_CLOSE_TRIGGERED - Sending Close order to MT4
* ORDSTATUS_CLOSE_MT4       - receive response of close order from MT4
*/
enum EN_ORD_STATUS { ORDSTATUS_NONE = 0, ORDSTATUS_OPEN_TRIGGERED, ORDSTATUS_OPEN, ORDSTATUS_CLOSE_TRIGGERED, ORDSTATUS_CLOSE };
enum EN_FIND_SHAPE { FIND_NONE = 0, FIND_ELBOW_R, FIND_ELBOW_L, FIND_NOSE_HIGH };
void LOGGING(LOGMSG_TP tp, BOOL bMain, BOOL bPrintConsole, const char* pMsg, ...);



const int MAX_CANDLE	= 100;
const int MAX_EMA		= MAX_CANDLE;
const int MAX_RSI		= MAX_CANDLE;
const int	MAX_MAB		= MAX_CANDLE;		
const int	MAX_MID		= MAX_CANDLE;
const int	MAX_MBB		= MAX_CANDLE;	// 모든 max 값은 같게 한다. 코딩의 복잡함을 피하기 위해, 약간의 성능저하는 받아들인다.
const int PERIOD_FOR_EMA	=50;
const int PERIOD_FOR_RSI	=13;
const int PERIOD_FOR_MA		= 34;
const int PERIOD_FOR_MAB	= 2;
const int PERIOD_FOR_MBB	= 7;

const int LEN_TIME		= 32;
const int IDX_OLDEST	= 0;

const int MAGIC_NO = 2021;


struct TimeChar
{
	char s[LEN_TIME];
};


struct TMabValues
{
	char	CandleTime[LEN_TIME];
	double	mab;
	double	mid;
	double	mbb;
};


struct TSendOrder
{
	char cBuySell[1];
	char zSendBuf[MAX_BUF];
};

struct TPoints
{
	int		idx;
	string	time;
	double	value;
};

//struct TSigFactors
//{
//	string	sCandleTime;
//	string	sTimeMT4;	//yyyy.mm.dd hh:mm:ss
//	double	dRsi;
//	double	dMid;
//	double	dMab;
//	double	dMbb;
//};
//
//typedef deque<TSigFactors*>		SIG_FACTORS_LIST;	// ascending by time


enum EN_SIGNALS { SIGNAL_NONE, SIGNAL_BUY, SIGNAL_SELL, SIGNAL_ERROR};