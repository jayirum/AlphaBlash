
unit uAlphaProtocol;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs,
  IniFiles, TypInfo, WinSock, SyncObjs, TlHelp32
  ;



type TKeyValue = record
  Key : string;
  Value : string;
end;


type
  PTRsValue = ^TRsValue;
  TRsValue = record
    idx   : integer;
    Key   : string;
    Value : string;
  end;


type TKeyValueArray = Array of TKeyValue;




const

  /////////////////////////////////////////////
  ///
  PACKET_CODE_SIZE  = 4;
  HEADER_SIZE       = 10; // STX(1), 139=XXXX(8), SOH(1)


  //SOH_BYTE  = #01;
  STX_BYTE  = #02;
  //DELI_ARRY = #14;
  ENTER_BYTE= #10;

  DEF_DELIMITER   = #01;
  DEF_STX         = #02;
  DEF_DELI_COLUMN = #05;
  DEF_DELI_ARRAY  = #06;

  


  /////////////////////////////////////////////
  ///  ERR CODE
  E_OK       = 0;
  E_NO_KEY   = 2018;
  E_EA_APP_UNMATCHED  = 2027; // EA와 App 간 조합이 일치하지 않습니다.
  E_NO_BROKER_IDX     = 1110; // 패킷에 BROKER INDEX 가 없습니다.

  /////////////////////////////////////////////
  ///  PACKET CODE
  CODE_REG_MASTER		= '1001';
  CODE_REG_SLAVE		= '1002';
  CODE_MASTER_ORDER	= '1003';
  CODE_PUBLISH_ORDER	= '1004';
  CODE_PING			      = '1005';
  CODE_LOGOFF			    = '1006';
  CODE_LOGON			    = '1007';
  CODE_LOGON_MASTER	  = '1008';
  CODE_LOGOFF_MASTER	= '1009';
  CODE_PING_LOGOFF	  = '1010';
  CODE_SLAVE_ORDER	  = '1011';
  CODE_USER_LOG		    = '1012';
  CODE_OPEN_ORDERS	  = '1013';
  CODE_ONLINE_COPIERS	= '1014';
  CODE_CONFIG_SYMBOL	= '1015';
  CODE_CONFIG_GENERAL	= '1016';
  CODE_DUP_LOGON		  = '1017';
  CODE_OFFLINE_COPIERS	= '1018';
  CODE_COPIER_UNSUBS		= '1019';
  CODE_EA_MESSAGE		    = '1020';
  CODE_COPIER_LIST			= '1021';
  CODE_RESET_MCTP       = '1022';
  CODE_RQST_MD_SUB      = '1023';
  CODE_RQST_MD_UNSUB    = '1024';
  CODE_HISTORY_CANDLES	= '1025';
  CODE_LOGON_AUTH       = '1026';

  CODE_MARKET_DATA	    = '8001';
  CODE_POSITION		      = '8002';
  CODE_SYMBOL_SPEC	    = '8003';
  CODE_CANDLE_DATA		  = '8004';

  /////////////////////////////////////////////
  ///  PACKET Fields

  FDS_COMMAND			= '102';   //  ORDER, COMMAND
  FDS_SYS				  = '103';   //  MT4,
  FDS_CODE			  = '101';
  FDS_TM_HEADER		= '104';   //  yyyymmdd_hhmmssmmm
  FDS_SUCC_YN	    = '105';
  FDS_MSG	        = '106';
  FDS_SYMBOL	    = '107';
  FDS_MASTERCOPIER_TP	= '109';
  FDS_OPENED_TM	  = '110';
  FDS_CLOSED_TM	  = '111';
  FDS_COMMENTS	  = '112';
  FDS_LIVEDEMO	  = '113';
  FDS_USERID_MINE	  = '114';
  FDS_USERID_MASTER	= '115';
  FDS_LOGONOFF_MSG	= '116';
  FDS_MASTER_LOGON_YN	= '117';
  FDS_USER_LOG	      = '118';
  FDS_ACCNO_MY	      = '119';
  FDS_ACCNO_MASTER	  = '120';
  FDS_BROKER			    = '121';
  FDS_USER_NICK_NM	  = '122';
  FDS_USER_PASSWORD	  = '123';
  FDS_RELAY_IP	      = '124';
  FDS_RELAY_PORT	    = '125';
  FDS_CONFIG_DATA	    = '126';
  FDS_EXPIRY	        = '127';
  FDS_ARRAY_TICKET	  = '128';
  FDS_MT4_TICKET	    = '129';
  FDS_LAST_ACTION_MT4_TM	= '130';
  FDS_OPEN_GMT	      = '131';
  FDS_LAST_ACTION_GMT	= '132';
  FDS_MASTER_TICKET	  = '133';
  FDS_PACK_LLEN       = '134';
  FDS_ARRAY_SYMBOL	  = '135';
  FDS_COPY_TP	        = '136';
  FDS_COPY_OPEN_YN	  = '137';
  FDS_COPY_CLOSE_YN	  = '138';
  FDS_COPY_SL_YN	    = '139';
  FDS_COPY_TP_YN	    = '140';
  FDS_COPY_PENDING_YN	= '141';
  FDS_ORD_BUY_YN	    = '142';
  FDS_ORD_BUY_LIMIT_YN	= '143';
  FDS_ORD_BUY_STOP_YN	  = '144';
  FDS_ORD_SELL_YN	      = '145';
  FDS_ORD_SELL_LIMIT_YN	= '146';
  FDS_ORD_SELL_STOP_YN	= '147';
  FDS_VOL_EQTYRATIO_MULTIPLE_YN	= '148';
  FDS_ORD_ACTION_CHG	    = '149';
  FDS_MAXLOT_ONEORDER_YN	= '150';
  FDS_MAXLOT_TOTORDER_YN	= '151';
  FDS_MAX_SLPG_YN	        = '152';
  FDS_MARGINLVL_LIMIT_YN	= '153';
  FDS_TIMEFILTER_YN	      = '154';
  FDS_MARKETORD_YN	      = '155';
  FDS_LIMITORD_YN	        = '156';
  FDS_STOPORD_YN	        = '157';
  FDS_CLOSE_GMT	          = '158';
  FDS_CLIENT_IP	          = '159';
  FDS_USERID_COPIER	      = '160';
  FDS_ACCNO_COPIER	      = '161';
  FDS_SITEAUTOLOGON_KEY	  = '162';
  FDS_WEBSITE_URL	        = '163';
  FDS_TR_PORT	            = '164';
  FDS_ORD_POS_TP	        = '165';
  FDS_ORDER_GROUPKEY	    = '166';
  FDS_KEEP_ORGTICKET_YN	  = '167';
  FDS_MT4_TICKET_CLOSING	= '168';
  FDS_ORD_ACTION_SUB_PARTIAL_YN	= '169';
  FDS_ORD_SIDE    = '170';
  FDS_KEY         = '171';
  FDS_REGUNREG    = '172';
  FDS_DATA		    = '173';
  FDS_TERMINAL_NAME = '174';
  FDS_ARRAY_DATA    = '175';
  FDS_CLIENT_SOCKET_TP	= '176';
  FDS_MARKETDATA_TIME   = '177';		//yyyy.mm.dd hh:mm:ss
  FDS_TIMEFRAME			    = '178';		//M1/M5/M15/M30/H1/H4/D1/W1/MN1
  FDS_CANDLE_TIME			  = '179';
  FDS_MT4_TICKET_ORG	  = '180';
  FDS_CLR_TP		        = '181';
  FDS_CLIENT_TP         = '182';
  FDS_TERMINAL_PATH     = '183';
  FDS_MAC_ADDR			    = '184';
  FDS_USER_ID				    = '185';
  FDS_COMPUTER_NM			  = '186';
  FDS_DATASVR_IP       = '187';
  FDS_DATASVR_PORT     = '188';
  FDS_ROUTE_YN			    = '191';
  FDS_BIZ_CODE			    = '192';

	FDN_ORD_TYPE        = '500'; // OP_BUY, OP_SELL, OP_BUYLIMIT, OP_SELLLIMIT, OP_BUYSTOP, OP_SELLSTOP
	FDN_PUBSCOPE_TP		  = '501';
	FDN_OPEN_TM         = '502';
	FDN_CLOS_TM         = '503';
  FDN_ERR_CODE	  = '504';
	FDN_RSLT_CODE   = FDN_ERR_CODE;
	FDN_ORD_ACTION  = '505';   //CHANGED_NONE = 0,
                            //CHANGED_OPEN,			// Pending주문 또는 포지션 오픈
                            //CHANGED_CLOSE_FULL,		// 포지션 전체 청산
                            //CHANGED_CLOSE_PARTIAL,	// 포지션 부분 청산
                            //CHANGED_ORDPRC,			// pending 주문 가격 변경
                            //CHANGED_SLPT_PRC,		// pending/position SL,TP 가격 변경
                            //CHANGED_DELETED			// pending 삭제
  FDN_ARRAY_SIZE  = '506';
  FDN_SLTP_TP			= '507';
	FDN_COPY_VOL_TP	= '508';
	FDN_MARGINLVL_LIMIT_ACTION	= '509';
	FDN_TIMEFILTER_H  = '510';
	FDN_TIMEFILTER_M	= '511';
  FDN_BROKER_IDX	= '512';
  FDN_DECIMAL	    = '513';
  FDN_SYMBOL_IDX	= '514';
  FDN_SIDE_IDX	  = '515';
  FDN_RECORD_CNT	= '516';
  FDN_SUBS_STATUS	= '517';
  FDN_TERMINAL_IDX=	'518';
  FDN_ORDER_CMD	  = '519';   // MT4 CMD - OP_BUY, OP_SELL, OP_BUYLIMIT...
  FDN_MAGIC_NO    = '520';
  FDN_DATA_CNT    = '521';
  FDN_TICKET      = '522';
  FDN_APP_TP			= '523';
  FDN_PACKET_SEQ	= '599';


  FDD_OPEN_PRC	  = '700';
  FDD_CLOSE_PRC	  = '701';
  FDD_LOTS	      = '702';
	FDD_SLPRC       = '703';
	FDD_TPPRC       = '704';
  FDD_PROFIT	    = '705';
	FDD_SWAP        = '706';
	FDD_CMSN        = '707';
	FDD_VOL_MULTIPLIER_VAL	= '708';
	FDD_VOL_FIXED_VAL		    = '709';
	FDD_VOL_EQTYRATIO_MULTIPLE_VAL	= '710';
	FDD_MAXLOT_ONEORDER_VAL	        = '711';
	FDD_MAXLOT_TOTORDER_VAL	        = '712';
	FDD_MAX_SLPG_VAL		            = '713';
	FDD_MARGINLVL_LIMIT_VAL	        = '714';
	FDD_CLOSE_LOTS                  = '715';
	FDD_EQUITY                      = '716';
  FDD_PIP_SIZE	  = '717';
  FDD_BID	        = '718';
  FDD_ASK	        = '719';
  FDD_SPREAD	    = '720';
  FDD_HIGH_PRC		= '721';
  FDD_LOW_PRC			= '722';

//var


implementation

//const

//var

end.


(*

---------------------------------------------------------
[ Login CLIENT->SERVER ]
BROKER_NAME   FDS_BROKER


---------------------------------------------------------
[ Login SERVER->SERVER ]

FDN_SYMBOL_IDX
FDN_SIDE_IDX

---------------------------------------------------------
[ SYMBOL SPEC CLIENT->SERVER : 8003  CODE_SYMBOL_SPEC ]

SYMBOL  : FDS_SYMBOL
SYMBOL_IDX  : FDN_SYMBOL_IDX
SIDE_IDX    : FDN_SIDE_IDX
DECIMAL : FDN_DECIMAL
PIPSIZE : FDD_PIP_SIZE

---------------------------------------------------------
[ MARKET DATA  CLIENT->SERVER : 8001  CODE_MARKET_DATA ]

SYMBOL  : FDS_SYMBOL
SYMBOL_IDX  : FDN_SYMBOL_IDX
SIDE_IDX    : FDN_SIDE_IDX
BID     : FDS_BID
ASK     : FDS_ASK
SPREAD  : FDS_SPREAD

---------------------------------------------------------
[ POSITION  CLIENT->SERVER : 8002  CODE_POSITION ]

SYMBOL      : FDS_SYMBOL
SYMBOL_IDX  : FDN_SYMBOL_IDX
SIDE_IDX    : FDN_SIDE_IDX
TICKET      : FDS_MT4_TICKET
ORG_TICKET  : FDS_MT4_TICKET_ORG
OPEN_PRC    : FDD_OPEN_PRC
LOTS        : FDD_LOTS
SIDE        : FDS_ORD_SIDE
PL          : FDD_PROFIT

---------------------------------------------------------
[ ORDER  SERVER->CLIENT : 1004  CODE_PUBLISH_ORDER ]

SYMBOL      : FDS_SYMBOL
CLR_TP      : FDS_CLR_TP // 진입, 청산
LOTS        : FDS_LOTS
SIDE        : FDS_ORD_SIDE

*)
