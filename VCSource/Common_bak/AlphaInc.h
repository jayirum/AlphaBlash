#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:4700)
#pragma warning(disable:4703)
#pragma warning(disable:4267)
#pragma warning(disable:4477)
#pragma warning( disable : 4786 )
#pragma warning( disable : 4819 )
#pragma warning( disable : 4996 )
#pragma warning(disable:26812)
#pragma warning(disable:6386)
#pragma warning(disable:6101)

#include <Windows.h>
#include <string>
#include <tchar.h>
using namespace std;

#ifndef _In_
	#define _In_
#endif
#ifndef _Out_
	#define _Outn_
#endif
#ifndef _InOut_
	#define _InOut_
#endif

//DEF-REGISTRY
#define	ALPHABLASH_ROOT			"Software\\ALPHABLASH"
#define	ALPHABLASH_DIRECTORY	"Software\\ALPHABLASH\\DIRECTORY"
#define	ALPHABLASH_ALERMSVR		"Software\\ALPHABLASH\\ALERMSVR"

namespace __ALPHA
{
#ifdef _UNICODE
	void ComposeEAConfigFileName(_In_ char* pzDir, _Out_ wchar_t* pwzFileName);
#endif

	enum EN_RET_VAL { RET_ERR=-1, RET_SKIP, RET_OK};

	enum CLIENT_TP	{ MASTR=0, COPIER};
	#define MC_TP_MASTER	'M'
	#define MC_TP_COPIER	'C'

	enum PUBSCOPE_TP {
		ALLCOPIERS_UNDER_ONEMASTER,	//	All slaves under one master id
		ONECOPIER_WITH_ID,			//	one Specific Slave
		ALLMASTERS,					//	All Masters
		ONEMASTER_WITH_ID,			//	one specific master
		ALL
	};


	enum ORD_ACTION {
		ORD_ACTION_NONTRADE_SYMBOL=-1,
		ORD_ACTION_NONE = 0,
		ORD_ACTION_OPEN,			// Pending주문 또는 포지션 오픈
		ORD_ACTION_CLOSE_FULL,		// 포지션 전체 청산
		ORD_ACTION_CLOSE_PARTIAL,	// 포지션 부분 청산
		ORD_ACTION_CHANGE,			// OpenPrice, SL, TP, Expiry
		ORD_ACTION_DELETED,			// pending 삭제
		ORD_ACTION_FAILED = 90
	};

	enum IDX_CHG_ACTION {
		IDX_CHG_OPEN_PRC = 0,
		IDX_CHG_SL,
		IDX_CHG_TP,
		IDX_CHG_EXPIRY
	};


#define MAX_TIMEFRAMES_CNT	9
	enum EN_TIMEFRAMES {
		IDX_1M = 0,
		IDX_5M,
		IDX_15M,
		IDX_30M,
		IDX_1H,
		IDX_4H,
		IDX_1D,
		IDX_1W,
		IDX_1MN
	};
#define TIMEFRAME_M1    "M1"
#define TIMEFRAME_M5    "M5"
#define TIMEFRAME_M15   "M15"
#define TIMEFRAME_M30   "M30"
#define TIMEFRAME_H1    "H1"
#define TIMEFRAME_H4    "H4"
#define TIMEFRAME_D1    "D1"
#define TIMEFRAME_W1    "W1"
#define TIMEFRAME_MN1   "MN1"

	#define CHG_ACTION_SIZE	4

	#define URL_WEBSITE	"http://119.205.238.151/login"
	#define CONFIG_FILE	"AlphaBlash.ini"
	#define MSG_FILE_EN		"AlphaMsg_en.ini"
	#define MSG_FILE_KO		"AlphaMsg_ko.ini"
	#define MSG_FILE_JAP	"AlphaMsg_jap.ini"
	#define MSG_FILE_CHINA	"AlphaMsg_china.ini"

	const char MT4_LANG_KOR[] = "Korean";
	const char MT4_LANG_ENG[] = "English";
	const char MT4_LANG_JAP[] = "Japanese";
	const char MT4_LANG_CHINA[] = "Chinese";

	const char BUY_SIDE = 'B';
	const char SELL_SIDE = 'S';
	const char FLAT_SIDE = '0';

	#define QDATA_TP_CONFIG	1
	#define QDATA_TP_NOTI	2

	#define SYMBOL_CONFIG_CNT	20

	#define DEF_YES		true
	#define	DEF_NO		false

	#define DEF_DBEXEC_SP		'S'
	#define DEF_DBEXEC_QUERY	'Q'

	const int SIZE_SYMBOL	= 10;
	const int SIZE_USER_ID	= 20;
	const int SIZE_ACCNO	= 15;
	const int LEN_SYMBOL	= SIZE_SYMBOL;
	const int LEN_USERID	= SIZE_USER_ID;
	const int LEN_ACCNOT	= SIZE_ACCNO;
	const int LEN_PRC		= 10;
	const int LEN_BUF = 4096;
	const int BUF_LEN = LEN_BUF;
	const char DEF_BUY		= 'B';
	const char DEF_SELL		= 'S';
	const int FIELD_LEN = 32;
	const int LEN_FIELD = FIELD_LEN;
	const int DOT_CNT = 5;
	const int LEN_CODE_SIZE = 4;
	


	#define WM_ORDER_SEND	WM_USER + 411
	#define WM_RECEIVE_DATA	WM_USER + 412
	#define WM_LOGON		WM_USER + 413
	#define WM_MARKET_DATA	WM_USER + 414
	#define WM_POSITION_DATA	WM_USER + 415
	#define WM_ORDER_OPEN	WM_USER + 416
	#define WM_ORDER_CLOSE	WM_USER + 417
	#define WM_CONFIG_UPDATED	WM_USER + 418
	#define WM_LOGON_AUTH		WM_USER + 419




	//////////////////////////////////////////////////////////////////////////
	//
	// Must sync with BPProtocol.mqh
	//
	const char SYS_MT4	[] = "MT4";
	const char SYS_NINJA[] = "NINJA";
	const char SYS_ZORRO[] = "ZORRO";

	#define PACKET_CODE_SIZE 4
	#define PACKET_HEADER_SIZE	 10	// STX134=0074SOH

	const char CODE_REG_MASTER		[]	= "1001";
	const char CODE_REG_COPIER		[]	= "1002";
	const char CODE_MASTER_ORDER	[]	= "1003";
	const char CODE_PUBLISH_ORDER	[]	= "1004";
	const char CODE_PING			[]	= "1005";
	const char CODE_LOGOFF			[]	= "1006";
	const char CODE_LOGON			[]	= "1007";
	const char CODE_LOGON_MASTER	[]	= "1008";	// SLAVE 에게 전달
	const char CODE_LOGOFF_MASTER	[]	= "1009";	// SLAVE 에게 전달
	const char CODE_PING_LOGOFF		[]	= "1010";	// Log Off by ping condition
	const char CODE_COPIER_ORDER	[]  = "1011";
	const char CODE_USER_LOG		[]	= "1012";
	const char CODE_OPEN_ORDERS		[]	= "1013";
	const char CODE_ONLINE_COPIERS	[]	= "1014";
	const char CODE_CONFIG_SYMBOL	[]	= "1015";
	const char CODE_CONFIG_GENERAL	[]	= "1016";
	const char CODE_DUP_LOGON		[]	= "1017";
	const char CODE_OFFLINE_COPIERS	[]	= "1018";
	const char CODE_COPIER_UNSUBS	[]	= "1019";
	const char CODE_EA_MESSAGE		[]	= "1020";
	const char CODE_COPIER_LIST		[]	= "1021";
	const char CODE_RESET_MCTP		[]	= "1022";
	const char CODE_ORDER_OPEN		[]	= "1023";
	const char CODE_ORDER_CLOSE		[]	= "1024";
	const char CODE_HISTORY_CANDLES []  = "1025";
	const char CODE_LOGON_AUTH		[] = "1026";
	const char CODE_COMMAND_BY_CODE [] = "1027";

	const char CODE_MARKET_DATA		[] = "8001";
	const char CODE_POSITION		[] = "8002";
	const char CODE_SYMBOL_SPEC		[] = "8003";
	const char CODE_CANDLE_DATA		[] = "8004";

	const char CODE_PUBLISH_TEST	[]	= "9001";
	const char CODE_RETURN_ERROR	[]	= "9002";
	const char CODE_REG_ROUTER		[] = "9991";
	const char CODE_UNREG_ROUTER	[] = "9992";

	const char TP_COMMAND	= 'C';
	const char TP_ORDER		= 'O';
	const char TP_REG		= 'R';
	const char TP_UNREG		= 'U';
	const char TP_MASTER	= 'M';
	const char TP_COPIER	= 'C';
	
	//#define LEN_CHANNEL_NM	128
	//#define LOCAL_RELAY_CHANNEL	"BA_LOCALRELAY"

	//+--------------------------------------------------------------------
	//+	Field
	//+--------------------------------------------------------------------

#define DEF_DELI			0x01
#define DEF_STX				0x02
#define DEF_ETX				0x03
#define DEF_DELI_COLUMN		0x05
#define DEF_DELI_RECORD	    0x06
//#define DEF_DELI_ARRAY      0x06
#define DEF_ENTER			0x0A
#define DEF_PACKETLEN_SIZE	4

//sprintf(temp, "%c%d=%0*d%c", DEF_STX, FDS_PACK_LEN, DEF_PACKETLEN_SIZE, len, DEF_DELI);
// STX+134=0195+0x01
#define DEF_HEADER_SIZE 10	// STX, 134=0000, DELI

#define FDS_CODE				101
#define FDS_COMMAND_CODE		102		// CommandCodes.h
#define FDS_SYS					103   //  MT4, 
#define FDS_TM_HEADER			104   //  yyyymmdd_hhmmssmmm
#define FDS_SUCC_YN				105
#define FDS_MSG 				106

#define FDS_SYMBOL				107
#define FDS_NOTI_MSG			108   // 
#define FDS_MASTERCOPIER_TP		109	// M:MASTER, C:COPIER
#define FDS_OPEN_TM				110
#define FDS_CLOSE_TM			111
#define FDS_COMMENTS			112
#define FDS_LIVEDEMO			113
#define FDS_USERID_MINE			114
#define FDS_USERID_MASTER		115
#define FDS_LOGONOFF_MSG		116
#define FDS_MASTER_LOGON_YN		117
#define FDS_USER_LOG			118
#define FDS_ACCNO_MY			119
#define FDS_ACCNO_MINE			FDS_ACCNO_MY
#define FDS_ACCNO_MASTER		120
#define FDS_BROKER				121
#define FDS_USER_NICK_NM		122
#define FDS_USER_PASSWORD		123
#define FDS_RELAY_IP			124
#define FDS_RELAY_PORT			125
#define FDS_CONFIG_DATA			126
#define FDS_EXPIRY				127
#define FDS_ARRAY_TICKET		128
#define FDS_MT4_TICKET			129
#define FDS_LAST_ACTION_MT4_TM	130
#define FDS_OPEN_GMT			131
#define FDS_LAST_ACTION_GMT		132
#define FDS_MASTER_TICKET       133
#define FDS_PACK_LEN		    134
#define FDS_ARRAY_SYMBOL		135
#define FDS_COPY_TP			    136
#define FDS_COPY_OPEN_YN	    137
#define FDS_COPY_ENTRY_YN       FDS_COPY_OPEN_YN
#define FDS_COPY_CLOSE_YN	    138
#define FDS_COPY_SL_YN			139
#define FDS_COPY_TP_YN			140
#define FDS_COPY_PENDING_YN		141
#define FDS_ORD_BUY_YN			142
#define FDS_ORD_BUY_LIMIT_YN	143
#define FDS_ORD_BUY_STOP_YN		144
#define FDS_ORD_SELL_YN			145
#define FDS_ORD_SELL_LIMIT_YN	146
#define FDS_ORD_SELL_STOP_YN	147
#define FDS_VOL_EQTYRATIO_MULTIPLE_YN	148
#define FDS_ORD_ACTION_CHG      149
#define FDS_MAXLOT_ONEORDER_YN	150
#define FDS_MAXLOT_TOTORDER_YN	151
#define FDS_MAX_SLPG_YN			152
#define FDS_MARGINLVL_LIMIT_YN	153
#define FDS_TIMEFILTER_YN		154
#define FDS_MARKETORD_YN		155
#define FDS_LIMITORD_YN			156
#define FDS_STOPORD_YN			157
#define FDS_CLOSE_GMT			158
#define FDS_CLIENT_IP			159
#define FDS_USERID_COPIER		160
#define FDS_ACCNO_COPIER		161
#define FDS_SITEAUTOLOGON_KEY	162
#define FDS_WEBSITE_URL			163
#define FDS_TR_PORT				164
#define FDS_ORD_POS_TP			165	//O:ORDER, P:POSITION
#define FDS_ORDER_GROUPKEY   166
#define FDS_KEEP_ORGTICKET_YN    167
#define FDS_MT4_TICKET_CLOSING   168
#define FDS_ORD_ACTION_SUB_PARTIAL_YN   169
#define FDS_ORD_SIDE	170
#define FDS_KEY			171
#define FDS_REGUNREG	172   //  R, U
#define FDS_DATA				173
#define FDS_TERMINAL_NAME		174
#define FDS_ARRAY_DATA			175
#define FDS_CLIENT_SOCKET_TP	176
#define FDS_MARKETDATA_TIME     177		//yyyy.mm.dd hh:mm:ss
#define FDS_TIMEFRAME			178		//M1/M5/M15/M30/H1/H4/D1/W1/MN1
#define FDS_CANDLE_TIME			179
#define FDS_MT4_TICKET_ORG		180
#define FDS_CLR_TP				181
#define FDS_CLIENT_TP			182
#define FDS_TERMINAL_PATH		183
#define FDS_MAC_ADDR			184
#define FDS_USER_ID				185
#define FDS_COMPUTER_NM			186
#define FDS_DATASVR_IP       187
#define FDS_DATASVR_PORT     188

#define FDS_ROUTE_YN			191
#define FDS_BIZ_CODE			192


#define FDN_ORD_TYPE		500 // OP_BUY, OP_SELL, OP_BUYLIMIT, OP_SELLLIMIT, OP_BUYSTOP, OP_SELLSTOP
#define FDN_PUBSCOPE_TP		501	
#define FDN_OPEN_TM			502
#define FDN_CLOS_TM			503	
#define FDN_ERR_CODE		504
#define FDN_RSLT_CODE		FDN_ERR_CODE
#define FDN_ORD_ACTION		505
#define FDN_ARRAY_SIZE		506	// 내부에서 ARRAY 로 데이터를 전달할 때
#define FDN_SLTP_TP				507
#define FDN_COPY_VOL_TP			508
#define FDN_MARGINLVL_LIMIT_ACTION	509
#define FDN_TIMEFILTER_H	510
#define FDN_TIMEFILTER_M	511
#define FDN_BROKER_IDX		512
#define FDN_DECIMAL			513
#define FDN_SYMBOL_IDX		514
#define FDN_SIDE_IDX		515
#define FDN_RECORD_CNT		516
#define FDN_SUBS_STATUS		517
#define FDN_TERMINAL_IDX	518
#define FDN_ORDER_CMD	    519   // MT4 CMD - OP_BUY, OP_SELL, OP_BUYLIMIT...
#define FDN_MAGIC_NO		520
#define FDN_DATA_CNT        521
#define FDN_TICKET			522
#define FDN_APP_TP			523
#define FDN_CMD_CODE		524
#define FDN_PACKET_SEQ		599

#define FDD_OPEN_PRC		700
#define FDD_CLOSE_PRC		701  
#define FDD_LOTS			702
#define FDD_SLPRC			703
#define FDD_TPPRC			704
#define FDD_PROFIT			705
#define FDD_SWAP			706
#define FDD_CMSN			707
#define FDD_VOL_MULTIPLIER_VAL	708
#define FDD_VOL_FIXED_VAL		709
#define FDD_VOL_EQTYRATIO_MULTIPLE_VAL	710
#define FDD_MAXLOT_ONEORDER_VAL	711
#define FDD_MAXLOT_TOTORDER_VAL	712
#define FDD_MAX_SLPG_VAL		713
#define FDD_MARGINLVL_LIMIT_VAL	714
#define FDD_CLOSE_LOTS          715
#define FDD_EQUITY				716
#define FDD_PIP_SIZE			717
#define FDD_BID					718
#define FDD_ASK					719
#define FDD_SPREAD				720
#define FDD_HIGH_PRC			721
#define FDD_LOW_PRC				722


	//
	// Must sync with BPProtocol.mqh
	//
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//
	// Error Code - Must save on MsgCode.ini 
	//

#define ERR_OK						0
#define E_OK						0
#define ERR_UNDEFINED			9999		// Undefined Error Code
#define E_UNDEFINED				9999		// Undefined Error Code


// business
#define E_MASTER_NOT_CONN		2001	//	마스터가 온라인이 아닙니다.
#define E_NO_EMAIL				2002	//	이메일이 없습니다.
#define E_NO_MASTER_COPIER_LINK	2003	//	마스터-카피어 관계가 없습니다..
#define E_NOEXIST_USERID		2004	//	USERID 가 없습니다.
#define E_NO_MT4ACC				2005	//	MT4 Account 가 없습니다.
#define E_USERID_DUP			2006	//	USERID 가 이미 존재합니다.
#define	E_CANNOTFIND_ORD		2007	//	해당 주문이 없습니다.
#define	E_NOT_OUR_ORDTYPE		2008	//	주문타입 오류 입니다.
#define	E_WRONG_PASSWORD		2009	//	비밀번호 오류 입니다.
#define	E_NON_ACCEPTABLE_STATUS	2010	//	로그인 불가 계정입니다.
#define	E_NON_ACLLOCATED_SERVER	2011	//	해당 마스터는 서버가 배정되지 않았습니다.
#define	E_NON_LOGON				2012	//	온라인 상태가 아닙니다.
#define	E_TERMINATE				2013	//	종료되었습니다.
#define	E_TIMEOUT				2014	//	타임아웃 입니다.
#define E_UNKNOWN_ERR_CODE		2015	//	정의되지 않은 오류코드 입니다.
#define E_NO_SYMBOLS_CONFIG		2016	//	종목설정을 먼저 하셔야 합니다.
#define E_MT4ACC_TAKEN			2017	//	이 MT4 Account 는 이미 사용중 입니다.

// MT4
#define	E_PLACEORDER_MT4		3001	//	MT4 주문 중 오류가 발생했습니다.
#define	E_GET_MT4INFO			3002	//	MT4 조회 중 오류가 발생했습니다.
#define	E_CNFG_ORDERLOTS		3003	//	설정 - 주문수량 오류 입니다.
#define	E_CNFG_SYMBOL			3004	//	설정 - Symbol 오류 입니다.
#define	E_CNFG_MAXSLIPPAGE		3005	//	설정 - 슬리피지 오류 입니다.
#define	E_CNFG_ORDPRICE			3006	//	설정 - 주문가격 오류 입니다.
#define	E_NOT_READY_TRADE		3007	//	주문 실행 준비가 안됐습니다.
#define	E_CANNOT_CONN_SVR		3008	//	Alpha 서버로 연결 오류 입니다.
#define	E_LOAD_EXTERN_LIB		3009	//	라이브러리 로드 오류 입니다.
#define	E_PUBLISH_OPENORD		3010	//	진입주문 Publishing 에 실패했습니다.
#define	E_PUBLISH_CLOSEORD		3011	//	청산주문 Publishing 에 실패했습니다.
#define	E_FUNCTION_NOT_PREPARED	3012	//	기능이 구현되지 않았습니다.
#define	E_ORDERSELECT			3013	//	MT4 주문 조회 중 오류 입니다.
#define	E_OUTOFINDEX			3014	//	MT4 에서 INDEX 초과 오류 입니다.
#define	E_LOGOUT_FROM_SVR		3015	//	서버에 의해서 로그아웃 되었습니다.
#define	E_DISCONN_FROM_SVR		3016	//	서버에서 연결을 종료했습니다.
#define  E_WRONG_SYMBOL          3017	//	잘못된 심볼입니다.
#define  E_WRONG_TICKET          3018	//	잘못된 TICKET NO. 입니다.
#define  E_WRONG_TICK_INFO	      3019	// 잘못된 TICK 정보 입니다.

// socket error
#define E_WSA_STARTUP	1001
#define E_CREATE_SOCK	1002
#define E_CONNECT		1003
#define E_NON_CONNECT	1004
#define E_SEND			1005
#define E_RECV			1006
#define E_MAX_SOCK_ERR	1007
#define E_INVALID_SOCK	1008

// packet error
#define E_NOCODE				1101	//	패킷상 코드가 없습니다.
#define E_NO_CODE				E_NOCODE
#define E_INVALIDE_MASTERCOPIER	1102	//	패킷의 마스터/카피어 구분이 잘못됐습니다.
#define E_NO_USERID				1103	//	패킷에 USERID 가 없습니다.
#define E_INVALID_CODE			1104	//	패킷 코드가 유효하지 않습니다.
#define E_NO_MASTER_ID			1105	//	패킷에 마스터ID 가 없습니다.
#define E_NO_ACNTNO				1106	//	패킷에 MT4 계좌번호가 없습니다.
#define E_NO_MASTER_ACCNO		1107	//	패킷에 마스터의 MT4 계좌번호가 없습니다.
#define E_NO_FIELD				1108	//	패킷에 데이터가 누락되었습니다.
#define E_NO_NICK_NAME			1109	//	패킷에 NICK NAME 이 없습니다.


// system & db
#define E_SYS_DB_EXCEPTION		1201	//데이터베이스 처리 중 예외가 발생했습니다.
#define E_READ_CONFIG			1202	//설정파일 조회 오류 입니다.
#define E_MSG_FILE				1203	//메세지 파일 조회 오류 입니다.
#define E_DB_NOT_OPENED			1204	//DB is not opened yet.	


/*
*	MT4 CMD 
*/
enum {
	ORDER_TYPE_BUY = 0
	, ORDER_TYPE_SELL
	, ORDER_TYPE_BUY_LIMIT
	, ORDER_TYPE_SELL_LIMIT
	, ORDER_TYPE_BUY_STOP
	, ORDER_TYPE_SELL_STOP
	, ORDER_TYPE_BUY_STOP_LIMIT
	, ORDER_TYPE_SELL_STOP_LIMIT
};

//int getMT4Cmd_MarketBuy();
//int getMT4Cmd_MarketSell();
//char* getMT4CmdDesc(int nCmd, char* pOut);
//bool IsBuyOrder(int nCmd);
//char* getBuySellString(int nCmd, char* pOut);
//
//char* enMasterKey(const char* id, const char* accno, char* out);
//void deMasterKey(const char* key, /*out*/ char* id, /*out*/char* accno);
//char* Now(char* pBuf);
//BOOL IsMaster(string sTp);
//int TimeFrameIdx(const char* pzTimeFrame);


}