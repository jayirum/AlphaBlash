#pragma once

#pragma warning( disable : 26495)
#include "../Common/AlphaInc.h"
#include "../CommonAnsi/LogMsg.h"
#include <deque>
#include <memory>
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



struct TCandle
{
	string sSymbol;
	string sTimeFrame;
	string sCandleTime;	//yyyy.mm.dd hh:mm:ss
	string sMDTime;		//yyyy.mm.dd hh:mm:ss
	char zOpen[32];
	char zHigh[32];
	char zLow[32];
	char zClose[32];
};

typedef deque<TCandle*>		CANDLE_LIST;	// ascending by time


struct TPoints
{
	int		idx;
	string	time;
	double	value;
};

struct TSigFactors
{
	string	sTimeMT4;	//yyyy.mm.dd hh:mm:ss
	double	dRsi;
	double	dMid;
	double	dMab;
	double	dMbb;
};

typedef deque<TSigFactors*>		SIG_FACTORS_LIST;	// ascending by time


enum EN_SIGNALS { SIGNAL_NONE, SIGNAL_BUY, SIGNAL_SELL, SIGNAL_ERROR};