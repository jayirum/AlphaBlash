#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4700)
#pragma warning(disable:4703)
#pragma warning(disable:4267)
#pragma warning(disable:4477)

#include <memory.h>
#include <assert.h> 

#ifdef	_FBI_DEBUG
#define debugCode(code_fragment){code_fragment;}
#else
#define debugCode(code)
#endif


/*
종목 : MAX 50개
차트종류 : 5개 (1분,5분,10분,30분,60분)
==> 종목당 차트 5개 = 50* 5 = 250	==> GROUP_CNT

차트당 MAX : 1440 개 (1분차트 고려)	==> STRUCT_CNT
==> 250 * 1440 * 45(sizeof(struct))
GROUP KEY :
종목 - GROUP KEY : 50개
차트 - STRUCT KEY :

GROUP KEY : CLK71
STRUCT KEY : 1분, 5분, 10분, 30분, 60분 (01MIN, 05MIN, 10MIN, 30MIN, 60MIN_

*/

//DEF-REGISTRY
#define	IRUM_ROOT			"Software\\IRUMSYSTEMS"
#define	IRUM_DIRECTORY		"Software\\IRUMSYSTEMS\\DIRECTORY"

#define	BLASHPF_ROOT		"Software\\BULLASH"
#define	BLASHPF_DIRECTORY	"Software\\BULLASH\\DIRECTORY"

#define	ALPHABLASH_ROOT		"Software\\ALPHABLASH"
#define	ALPHABLASH_DIRECTORY	"Software\\ALPHABLASH\\DIRECTORY"

///////////////////////////////////////////////////////////
//DEF-LEN
#define	LEN_SYMBOL	7			//FDAXU17
#define	LEN_SYMBOL_NM	32			//FDAXU17
#define	LEN_ARTC	7			//FDAXU17
#define LEN_PRC		10
#define LEN_SEQNO	10
#define	LEN_SHM_GROUP_KEY	9	//CLK17+01  // GROUP KEY (종목+charttp)
#define	LEN_GROUP_KEY		LEN_SHM_GROUP_KEY	//CLK7+1  // GROUP KEY (종목+charttp)
#define	LEN_CHART_NM		12	// yyyymmddhhmm
#define	LEN_SHM_STRUCT_KEY	LEN_CHART_NM	// // STRUCT KEY (차트이름) 0000, 0005, 0010
#define LEN_INDICHART_NM	12	//yyyymmddhhmm
#define LEN_QTY				10
#define LEN_PL				10
#define LEN_ACNT_NO	11
#define LEN_ORD_NO	10
//#define LEN_ORD_QTY	10
#define LEN_INDICHART_NM	12	//yyyymmddhhmm
#define	LEN_STRAT_COMP_ID	10
#define LEN_STRAT_ID		10
#define LEN_BUFF_SIZE		1024
#define LEN_USERID			16
#define LEN_PACKET_CODE		6
#define SIZE_PACKET_LEN		4
#define LEN_MONEY			12
#define LEN_TICKSIZE		10
#define LEN_DOTCNT			2
#define LEN_RSLT_CODE		5
#define LEN_BUF				1024

//DEF-BUY, DEF-SELL
#define CD_BUY	'B'
#define CD_SELL	'S'

//DEF-OPEN, DEF-CLOSE
#define CD_OPEN		'O'
#define CD_CLOSE	'C'


//DEF-ORD_TP
#define CD_ORD_TP_LIMIT		'1' // 지정가
#define CD_ORD_TP_MARKET	'2' // 시장가
#define CD_ORD_TP_SL		'3' // 손절
#define CD_ORD_TP_PT		'4' // 익절
#define CD_ORD_TP_SLPT		'5'	// 손절+익절
#define CD_ORD_TP_MIT		'6'

#define ORDTP2NAME(cd,out){\
	if(cd==CD_ORD_TP_LIMIT)	sprintf(out,"지정");	\
	if(cd==CD_ORD_TP_MARKET)sprintf(out,"시장");	\
	if(cd==CD_ORD_TP_SL)	sprintf(out,"손절");	\
	if(cd==CD_ORD_TP_PT)	sprintf(out,"익절");	\
	if(cd==CD_ORD_TP_SLPT)	sprintf(out,"손익");	\
	if(cd==CD_ORD_TP_MIT)	sprintf(out,"예약");	\
}


//DEF-ORD_PROC
#define CD_ORD_PROC_NEW		'1'	//신규
#define CD_ORD_PROC_MDFY	'2'	//정정
#define CD_ORD_PROC_CNCL	'3'	//취소
#define CD_ORD_PROC_RJCT	'4'	//거부
#define	CD_ORD_PROC_SYMBOL_CNCL	'A'	//종목취소, 
#define CD_ORD_PROC_SYMBOL_CLR	'B'	//종목청산
#define	CD_ORD_PROC_ALL_CNCL	'C'	//전종목취소, 
#define CD_ORD_PROC_ALL_CLR		'D'	//전종목청산

#define ORDPROC2NAME(cd,out){\
	if(cd==CD_ORD_PROC_NEW)			sprintf(out,"신규");	\
	if(cd==CD_ORD_PROC_MDFY)		sprintf(out,"정정");	\
	if(cd==CD_ORD_PROC_CNCL)		sprintf(out,"취소");	\
	if(cd==CD_ORD_PROC_RJCT)		sprintf(out,"거부");	\
	if(cd==CD_ORD_PROC_SYMBOL_CNCL)	sprintf(out,"종취");	\
	if(cd==CD_ORD_PROC_SYMBOL_CLR)	sprintf(out,"종청");	\
	if(cd==CD_ORD_PROC_ALL_CNCL)	sprintf(out,"전취");	\
	if(cd==CD_ORD_PROC_ALL_CLR)		sprintf(out,"전청");	\
}

//DEF-CANDLE
//#define DEF_CANDLE_PLUS		'1'		//LEGACY
//#define DEF_CANDLE_MINUS	'2'		//LEGACY
#define DEFINE_NO_CHART		"NONE"	//LEGACY
#define CD_PLUS_CANDLE		'+'
#define CD_MINUS_CANDLE		'-'
#define CD_OPENSIG_MINUS	'1'
#define CD_OPENSIG_PLUS		'2'



//DEF-PACKET CODES. LEGACY
#define CD_SIG_OPENPRC		"SS0001"
#define CD_SIG_ASSIST		"SS0002"
#define CD_SIG_MACROSS		"SS0003"
#define CD_SIG_SIDEWAY		"SS0004"
#define CD_SIG_OPEN_ASSIST	"SS0005"
#define CD_SIG_OPEN_ASSIST2	"SS0006"
#define CD_STRAT_ORD		"SO0001"
#define CD_C_ORDER			"SO0001"
#define CD_C_NCLR			"ST0001"
#define CD_C_REMAIN_ORD		"ST0002"

//DEF-PACKET CODES. API<->내부
#define CDAPI_ORD_RQST		"100101"
#define	CDAPI_ORD_ACPT	"100201"
#define CDAPI_ORD_REAL	"100301"
#define CDAPI_CNTR_REAL	"100302"
#define CDAPI_ERROR		"999999"

//기타
#define CD_CHART_QRY	"300001"

// RESULT CODE
#define RSLT_SUCCESS			"00000"
#define RSLT_CHART_NOTFOUND		"10000"

#define DEF_EOL				0x0a	// delphi ipworks 와 통신하기 위해


#define MDRELAY_KFUT_TICK		"1"
#define MDREALY_KFUT_CME_TICK	"2"
#define MDRELAY_OV_TICK			"3"

//DEF-CROSS
// 골든크로스 (단기>장기), 데드크로스(단기<장기)
enum CROSS_TP { NONE_CROSS = 0, GOLDEN_CROSS, DEAD_CROSS };
#define CROSS2NAME(cross,out){\
	if(cross==GOLDEN_CROSS) strcpy(out,"골든");		\
	if (cross == DEAD_CROSS) strcpy(out, "데드");	\
	if (cross == NONE_CROSS) strcpy(out, "노크");	\
}

//DEF-SMA
#define	SMA_SHORTEST_CNT	5	// 단기 SMA 갯수
#define	SMA_SHORT_CNT	10	// 단기 SMA 갯수
#define	SMA_LONG_CNT	20	// 장기 SMA 갯수

#define	FORMAT_PRC(prc,dotcnt,out) { sprintf(out, "%0*.*f", LEN_PRC, dotcnt, prc); } // 000000012.12
#define FORMAT_PRC_STR(prc,len, out){ sprintf(out, "%*.*s", len, len, prc);} // "       12.12"	
#define FORMAT_SEQNO(seqno,out) { sprintf(out, "%0*ld",LEN_SEQNO, seqno);}
#define GET_SHM_NM(artc,out){ sprintf(out, "Global\\%s_SHM", artc);}
#define GET_SHM_LOCK_NM(artc,out){ sprintf(out, "Global\\%s_MUTEX", artc);}
#define GET_ARTC_CODE(symbol,out){ sprintf(out,"%.*s", strlen(symbol) -2 , symbol);}// CLQ7,6EQ7
#define FORMAT_USERID(id,out) { sprintf(out, "%*.*s", LEN_USERID, LEN_USERID, id);}
#define FORMAT_SYMBOL(symbol) { CUtil::TrimAll(symbol, strlen(symbol));}

//DEF_WM CODE
#define WM_CHART_ALL_KILL	WM_USER + 800
#define WM_CHART_DATA		WM_USER + 801
#define WM_MANAGER_NOTIFY	WM_USER + 802
#define WM_MANAGER_NOTIFY_SOCK	WM_USER + 803
#define WM_NOTI_MATCH		WM_USER + 804
#define WM_SAVE_CHART		WM_USER + 805
#define WM_STRAT_LOGGING	WM_USER + 806
#define WM_SAVE_MATCH		WM_USER + 807
#define WM_SEND_STRATEGY	WM_USER + 808	// Send signal to client
#define WM_RECV_CLIENT		WM_USER + 809	// recv data from client - TR, real
#define WM_RECV_API_MD		WM_USER + 810	// recv data from client - TR, real
#define WM_SENDORD_API		WM_USER + 811
#define WM_MARKET_CLOSE		WM_USER + 812
#define WM_RECV_API_ORD		WM_USER + 813
#define WM_RECV_API_CNTR	WM_USER + 814
#define WM_SAVE_API_ORD		WM_USER + 815
#define WM_CLOSE_POSITION	WM_USER + 816
#define WM_PASS_DATA		WM_USER + 817
#define WM_LOGON			WM_USER + 818
#define WM_TERMINATE		WM_USER + 819
#define WM_SAVE_ORDER		WM_USER + 820
#define WM_SAVE_USERLOG		WM_USER + 821
#define WM_LOGOUT			WM_USER + 822
#define WM_MD_OV_FUT		WM_USER + 823	// OVERSEAS FUTURES
#define WM_MD_KOSPI_FUT		WM_USER + 824
#define WM_MD_KOSPI_CME		WM_USER + 825


#define WM_DIE				WM_USER + 999


//DEF-SHART SHM, DEF-CHARTSHM
#define CHART_TP_CNT		1				//10	// 1분, 5분, ...  20
#define MAX_CHART_TP_CNT	CHART_TP_CNT
#define MAX_SYMBOL_PER_ARTC	1				//2		//	품목당 2개의 월물(종목)에 대해서만 지원한다.
#define MAX_GROUPKEY_CNT	MAX_SYMBOL_PER_ARTC * CHART_TP_CNT
#define MAX_CHART_UNIT_CNT	2880	// 1440			//	한 GROUP당 (한종목, 한차트당 'CLU7 00' 1분차트 * 60 * 24)
#define MAX_ARTC_CNT		10				//	10개의 품목만 
#define GET_TICKCHART_NM(seq,out){ sprintf(out, "%0*d", LEN_CHART_NM, seq);}
#define GET_GROUP_KEY(symbol, CHART_TP, out) { sprintf(out, "%-*.*s%02d", LEN_SYMBOL, LEN_SYMBOL, symbol, CHART_TP) ; }

enum CHARTNAME_TYPE { CHARTNAME_TP_NEAR, CHARTNAME_TP_FAR };


enum CHART_TP {
	TP_1MIN = 0,
	TP_3MIN,
	TP_5MIN,
	TP_10MIN,
	TP_15MIN,
	TP_20MIN,
	//TP_30MIN, 
	TP_60MIN,
	TP_120MIN,
	TP_DAY,
	TP_WEEK,
	TP_MON,
	TP_TICK1,
	TP_TICK2,
	TP_TICK5,
	TP_TICK10,
	TP_TICK20,
	TP_TICK30,
	TP_TICK40,
	TP_TICK60
};
#define CHART_TP_S(tp,out){\
	if(tp==TP_1MIN) strcpy(out,"1분");		\
	if (tp == TP_3MIN) strcpy(out, "3분");	\
	if (tp == TP_5MIN) strcpy(out, "5분");	\
	if (tp == TP_10MIN) strcpy(out, "10분");	\
	if (tp == TP_15MIN) strcpy(out, "15분");	\
	if (tp == TP_20MIN) strcpy(out, "20분");	\
	if (tp == TP_60MIN) strcpy(out, "60분");	\
	if (tp == TP_120MIN) strcpy(out, "120분");	\
	if (tp == TP_DAY) strcpy(out, "일간");	\
	if (tp == TP_WEEK) strcpy(out, "주간");	\
}


enum TIME_SECTION {
	SECTION_START,	// TRADE 시작 : 07:00:00 ~ 23:59:59
	SECTION_NEXT	// 다음날     : 00:00:00 ~ 06:59:59
};
TIME_SECTION __GetTimeSection(const char* psTime);	// hh:mm:ss
char* __GetDate_ByTimeSection(const char* psCompTime, _Out_ char* psDate, bool bWithColon=true);	// date:yyyymmdd

// PostThreadMessage 를 위한 char wrapping structure
class TAG_BUF
{
public:
	TAG_BUF() { memset(buf, 0x00, sizeof(buf)); }
	~TAG_BUF() {}
	char buf[2048];
	int bufsize;
};

/*
SHM - 품목코드로 (6E)
GROUP_KEY - 종목코드+CHART_TP
*/
typedef struct _ST_SHM_CHART_HEADER
{
	char LastChartNm[LEN_CHART_NM];
	char Reserved2[10];
	char Reserved3[10];
	char Reserved4[10];
	char Reserved5[10];
}ST_SHM_CHART_HEADER;

typedef struct _ST_SHM_CHART_UNIT
{
	char	Nm[LEN_CHART_NM];		//8,8 KEY. chart name. hhmm		0000, 0005, 0010
	char	prevNm[LEN_CHART_NM];	//8,16 previous EY
	char	gb[1];					//4,17 +:양봉(plus candel), -:음봉(minus candle)
	char	open[LEN_PRC];			//
	char	high[LEN_PRC];			//
	char	low[LEN_PRC];			//
	char	close[LEN_PRC];			//
	char	cntr_qty[LEN_QTY];
	char	dotcnt[5];				//5,100
									//char	seq[LEN_SEQNO];			//10,110 시간저장한다.
	char	sma_short[LEN_PRC];		//20,130 단기short term SMA(Simple Moving Average). 10
	char	sma_long[LEN_PRC];		//20,150 장기long term SMA(Simple Moving Average). 20
	char	sma_shortest[LEN_PRC];	//20,170 단기short term SMA(Simple Moving Average). 5
	char	Reserved[40];			//원래 50bytes 였다가, stk_cd 에 떼어줌
	char	stk_cd[10];
	
}ST_SHM_CHART_UNIT;	// 
					//#define LEN_CHART_WITHOUT_STRAT	sizeof(ST_SHM_CHART_UNIT) - STRATEGY_CNT

typedef struct _ST_SHM_LAST_CHART
{
	char	Nm[LEN_CHART_NM];
	char	Reserved[50];
}ST_SHM_LAST_CHART;

// 차트를 전달하기 위한 소켓용 버퍼
typedef struct _ST_CHART_PACKET
{
	char	STX[1];
	char	Len[SIZE_PACKET_LEN];
	char	Code[LEN_PACKET_CODE];		// CD_CHART_QRY
	char	RsltCode[LEN_RSLT_CODE];
	ST_SHM_CHART_UNIT	chart;
	char	ETX[1];
}ST_CHART_PACKET;

/*
지표를 위한 차트
*/
typedef struct _ST_SHM_INDICATOR
{
	char	IndiNm[LEN_INDICHART_NM];	// KEY. yyyymmddhhmm		0000, 0005, 0010
	char	prevNm[LEN_INDICHART_NM];	// 직전KEY
	char	sma_short[LEN_PRC];		// 단기 SMA(Simple Moving Average). 10
	char	sma_long[LEN_PRC];		// 장기 SMA(Simple Moving Average). 20
	char	dotcnt[5];
}ST_SHM_INDICATOR;




// ChartMaker 에게 전송할 패킷
//typedef struct _ST_PACK2CHART
//{
//	char	Date[8];
//	char	Time[8];
//	char	ShortCode[LEN_SYMBOL];
//	char	Close[LEN_PRC];
//	char	CntrQty[LEN_QTY];
//	char	DecLen[5];				// 소숫점 자릿수
//}ST_PACK2CHART;

// ChartMaker 에게 전송할 패킷
typedef struct _ST_PACK2CHART_EX
{
	char	STX[1];
	char	Len[SIZE_PACKET_LEN];	// 전체 길이
	char	Date[8];
	char	Time[8];
	char	ShortCode[LEN_SYMBOL];
	char	Close[LEN_PRC];
	char	CntrQty[LEN_QTY];	
	char	DecLen[5];				// 소숫점 자릿수
	char	ETX[1];
}ST_PACK2CHART_EX;


/*
fldAccountNo
fldSymbol
잔고수량: pBAL_BALANCE_QTY
평균단가: pBAL_EXEC_AVG
방향: pBAL_SIDE
수익배수: pBAL_VIP_PL_MONEY
*/
struct ST_MINI_AFTER_CNTR
{
	char zUserId[32];
	char zAccountNo[32];	// fldAccountNo
	char zSymbol[32];	// fldSymbol
	double dPosQty;		// fldBeableOptQty
	double dAvgPrc;		// fldStrikePrice
	char zSide[2];		// fldFutFlag
	int nPLMultiple;	// fldLeverageType
	char zAvgPrc[32];	// 
	char zSLPrc[32];	// PLMultipleCut 에서 set
	double dSLtickCnt;	// PLMultipleCut 에서 set
	char zFiredCurrPrc[32];
	bool bIsClosedInfo;	// 청산정보인가?
};

#pragma pack(push, 1)
// 시세패킷
struct TFutExec2
{
	char issue[9];
	double gap;
	double cup;
	double sip;
	double hip;
	double lip;
	int vol;
	double amt;
	int time;
	char side[1 + 1];
	char ydiffSign[1 + 1];
	char chgrate[6 + 1];
	//int execvol;
}; 
#pragma pack(pop)

struct TFutExecRelay
{
	char issue[9];
	double gap;
	double cup;
	double sip;
	double hip;
	double lip;
	int vol;
	double amt;
	int time;
	char side[1 + 1];
	char ydiffSign[1 + 1];
	char chgrate[6 + 1];
	char prodTp[1];			// #define MD_KFUT_TICK		"1"
							//#define MD_KFUT_CME_TICK	"2"
							//#define MD_OV_TICK		"3"
	char eol[1];
};

struct TFutExecRelayS
{
	char issue[9];
	char gap[15];
	char cup[15];
	char sip[15];
	char hip[15];
	char lip[15];
	char vol[15];
	char amt[15];
	char time[11];
	char side[2];
	char ydiffSign[2];
	char chgrate[7];
	char prodTp[1];			// #define MD_KFUT_TICK		"1"
							//#define MD_KFUT_CME_TICK	"2"
							//#define MD_OV_TICK		"3"
	char eol[1];
};



struct SMPACK_FX_EXEC
{
	char issue[9 + 1];
	char gap[11];
	char cup[11];
	char sip[11];
	char hip[11];
	char lip[11];
	char vol[11];
	char amt[11];
	char time[11];
	char side[2];
	char ydiffSign[2];
	char chgrate[7];
	char execvol[11];
};



#pragma pack(push, 1)
// delphi 패킷
struct TTradeOpenInterest
{
	char account_no[15];   // 계좌번호
	char symbol[21];
	char side[2];
	double balance_qty;	//: real; // 잔고수량
	double clear_qty;	// : real; // 청산가능수량 ( 잔고수량 - 주문수량 )
	double exec_avg;	// : real; // 평균가
	double book_amt;	// : real; // 잔고금액(매매금액)
	double pl_money;	// : real; // 실현손익
	char update_time[24];
	char user_id[17];   // ID
	int product;                // Product
	int PLMultiple;
};
#pragma pack(pop)


struct ST_CLR_NOTIFY
{
	char Account[15];
	char Symbol[21];
};
   
// real-time strategy packet from SERVER to CLIENT
typedef struct _ST_STRAT_REAL_CLIENT
{
	char	Len[SIZE_PACKET_LEN];	// 전체 길이
	char	Symbol[LEN_SYMBOL];
	char	GroupKey[LEN_GROUP_KEY];
	char	ChartNm[LEN_CHART_NM];
	char	StratID[32];
	char	StratPrc[LEN_PRC];
	char	ApiDT[8];
	char	ApiTM[8];	//hh:mm:ss
	char	Note[100];
	char	EOL[1];
	
}ST_STRAT_REAL_CLIENT;


/*
// MUBFISH 전략발동. 주문패킷

//DEF-BUY, DEF-SELL
#define CD_BUY	'B'
#define CD_SELL	'S'


//DEF-ORD_TP
#define CD_ORD_TP_LIMIT		'1' // 지정가
#define CD_ORD_TP_MARKET	'2' // 시장가
#define CD_ORD_TP_SL		'3' // 손절
#define CD_ORD_TP_PT		'4' // 익절
#define CD_ORD_TP_SLPT		'5'	// 손절+익절
#define CD_ORD_TP_MIT		'6'
*/
typedef struct _ST_MF_STRAT_ORD
{
	char	Symbol[LEN_SYMBOL];
	char	StratID[32];
	char	ClrTp[1];				// CD_OPEN,CD_CLOSE
	char	Side[1];				// CD_BUY, CD_SELL
	char	OpenPrc[LEN_PRC];
	char	BasePrc[LEN_PRC];
	char	CurrPrc[LEN_PRC];
	char	MaxPLPrc[LEN_PRC];		// 진입 후 최고좋은 가격
	char	OrdPrc[LEN_PRC];
	char	EntryPrc[LEN_PRC];
	//char	MaxPL[LEN_PL];			// 최고가격의 이익
	//char	CurrPL[LEN_PL];			// 현재가격의 이익
	char	OrdProtTp[1];			// CD_ORD_TP_MARKET, CD_ORD_TP_LIMIT
	char	OrdTM[12];				// hh:mm:ss.mmm
	char	ApiDT[8];
	char	ApiTM[8];	//hh:mm:ss
	char	Note[256];
}ST_MF_STRAT_ORD;


//API 에게 전송할 주문 패킷
typedef struct _ST_API_ORD_RQST
{
	char	Code[LEN_PACKET_CODE];	//CD_ORD_RQST
	char	Symbol[LEN_SYMBOL];		//space trailing
	char	OrdPrcTp[1];			// 신규,정정,취소
	char	Side[1];				//CD_BUY / CD_SELL
	char	OrdTp[1];				//CD_ORD_TP_LIMIT, ...
	char	OrdPrc[LEN_PRC];		//시장가인 경우 "0      " 
	char	OrdQty[LEN_QTY];
	char	OrgOrdNo[LEN_ORD_NO];
	char	OrgPrc[LEN_PRC];
	char	UUID[36];				//내부주문번호
	char	Date[8];
	char	TM[9];	//hhmmssmmm
	char	EOL[1];
}ST_API_ORD_RQST;


//API 의 에러 (API->)
typedef struct _ST_API_ERROR
{
	char	STX[1];
	char	Len[SIZE_PACKET_LEN];
	char	Code[LEN_PACKET_CODE];	//999999
	char	ApiMsgCd[10];
	char	ApiMsg[128];
	char	Date[8];
	char	UUID[36];
	char	TM[9];	//hhmmssmmm};
	char	ETX[1];
}ST_API_ERROR;

//API 에서 수신 - 주문접수
typedef struct _ST_API_ORD_ACPT
{
	char	STX[1];
	char	Len[SIZE_PACKET_LEN];
	char	Code[LEN_PACKET_CODE];	//100101
	char	Symbol[LEN_SYMBOL];
	char	OrdNo[LEN_ORD_NO];
	char	OrdPrc[LEN_PRC];
	char	OrdQty[LEN_QTY];
	char	OrdProcTp[1];
	char	RjctTp[1];		//0:정상 1:거부
	char	UUID[36];
	char	ApiDT[8];
	char	ApiTM[9];		// HH:MM:SS
	char	ETX[1];		//	
}ST_API_ORD_RESPONSE;

//API 에서 수신 - 주문REAL
typedef struct _ST_API_ORD_REAL
{
	char	STX[1];
	char	Len[SIZE_PACKET_LEN];
	char	Code[LEN_PACKET_CODE];
	char	Symbol[LEN_SYMBOL];
	char	OrdNo[LEN_ORD_NO];
	char	Side[1];
	char	OrdPrc[LEN_PRC];
	char	OrdQty[LEN_QTY];
	char	RemnQty[LEN_QTY];
	char	OrgOrdNo[LEN_ORD_NO];
	char	UUID[36];
	char	ApiDT[8];
	char	ApiTM[9];		// HH:MM:SS
	char	ETX[1];		//	

}ST_API_ORD_REAL;//123


//API 에서 수신 - 체결REAL
typedef struct _ST_API_CNTR_REAL
{
	char	STX[1];
	char	Len[SIZE_PACKET_LEN];
	char	Code[LEN_PACKET_CODE];
	char	Symbol[LEN_SYMBOL];
	char	OrdNo[LEN_ORD_NO];
	char	Side[1];
	char	OrdPrc[LEN_PRC];
	char	OrdQty[LEN_QTY];
	char	CntrPrc[LEN_PRC];
	char	CntrQty[LEN_QTY];
	char	EngagedAmt[LEN_MONEY];	//약정금액
	char	Cmsn[LEN_MONEY];
	char	ApiOrdDT[8];
	char	ApiOrdTM[9];		// HH:MM:SS
	char	ApiCntrDT[8];
	char	ApiCntrTM[9];		// HH:MM:SS
	char	ETX[1];		//	

}ST_API_CNTR_REAL; // 128

// 관리자 작업통보 소켓 버퍼
typedef struct _ST_MANAGER_SOCKNOTIFY
{
	char AcntNo[11];
	char Symbol[5];
	char OrdTp[1];	// 1:NEW, 2:전체취소, 3:전체청산
	char PrcTp[1];	// 1:지정가, 2:시장가
	char Side[1];	// 1:BUY, 2:SELL
	char Prc[10];
}ST_MANAGER_SOCKNOTIFY;

// 관리자 작업 통보
typedef struct _ST_MANAGER_NOTIFY
{
	int nSeqNo;
	char zAcntNo[LEN_ACNT_NO + 1];
	int nWorkTp;
	char zWorkVal[64];
	char zWorkNote[64];
	char zAdminID[64];
}ST_MANAGER_NOTIFY;




typedef struct _ABOTLOG_NO1
{
	char	zSymbol[32];
	char	zStratID[32];
	char	FireYN[1];		//Y-발동, N-발동하지 않음(단지 정보성)
	char	OpenClose[1];	//O, C
	char	PLTp[1];		// P, L
	char	BsTp[1];		//B, S
	char	zOrdQty[32];
	char	zCurrPrc[32];
	char	zStratPrc[32];
	char	zOpenPrc[32];
	char	zEntryPrc[32];
	char	zEntryPercent[32];
	char	zCross_1min[64];
	char	zCross_3min[64];
	char	zCross_5min[64];
	char	zMaxPrc[32];		// 익절 max
	char	zPtClrTriggerPercent[10];
	char	zApiTM[32];
	char	zMsg[256];
}ABOTLOG_NO1;



typedef struct _ABOTLOG_NO3
{
	char	zSymbol[32];
	char	zStratID[32];
	char	FireYN[1];		//Y-발동, N-발동하지 않음(단지 정보성)
	char	OpenClose[1];	//O, C
	char	PLTp[1];		// P, L
	char	BsTp[1];		//B, S
	char	zOrdQty[32];
	char	zCurrPrc[32];
	char	zStratPrc[32];
	char	zOpenPrc[32];
	char	zEntryPrc[32];
	char	zEntryPercent[32];
	char	zCrossMsg[128];
	char	zMaxPrc[32];		// 익절 max
	char	zPtClrTriggerPercent[10];
	char	zApiTM[32];
	char	zMsg[256];
}ABOTLOG_NO3;





namespace _IRUM
{
	const int MEM_PRE_ALLOC = 100;
	const int MEM_MAX_ALLOC = 10000;
	const int MEM_BLOCK_SIZE = 512;
}


//bool ir_isbrokerKR(char* psBroker);
//bool ir_isbrokerHD(char* psBroker);
//char* ir_cvtcode_HD_KR(char* pzhdCode, char* pzOut);	// CLQ17 -> CLQ7
char* ir_cvtcode_uro_6e(char* pzIn, char* pzOut);
char* ir_cvtcode_6e_uro(char* pzIn, char* pzOut);
//char* pack_header_fields(char* pData, char* pzName, char* pzOut);
int TFutExec_to_SMPACK_FX_EXEC(const void* pIn, void* pOut);

char* __ComposeChartName(char* date, char* time, int chartTp, int nameTp, char* out);
char* __ComposeChartName_1min(char* date, char* time, int nameTp, char* out);
