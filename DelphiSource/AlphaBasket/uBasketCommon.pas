unit uBasketCommon;

interface

Uses
  System.sysutils, windows, Messages
  ;


const
  MAX_BROKER  = 10;
  MAX_SYMBOL  = 30;
  MAX_CTX     = 10;
  MAX_THREAD  = 10;

  IDX_BUY  = 1;
  IDX_SELL = 2;

  Q_RECV = 0;
  Q_SEND = 1;

  MAX_SPREAD_PT = 5;

  CLRTP_NONE    = '0';
  CLRTP_OPEN    = '1';
  CLRTP_PARTIAL = '2';
  CLRTP_CLOSE   = '3';


  MD_BROKER = 0 ;
  MD_BID1 = 1;
  MD_ASK1 = 2;
  MD_BID2 = 3;
  MD_ASK2 = 4;
  MD_BID3 = 5;
  MD_ASK3 = 6;
  MD_BID4 = 7;
  MD_ASK4 = 8;
  MD_BID5 = 9;
  MD_ASK5 = 10;
  MD_BID6 = 11;
  MD_ASK6 = 12;
  MD_BID7 = 13;
  MD_ASK7 = 14;

  POS_SYMBOL  = 0;
  POS_SIDE    = 1;
  POS_VOL     = 2;
  POS_OPENPRC = 3;
  POS_NOWPRC  = 4;
  POS_PL      = 5;
  POS_PL_PIP  = 6;
  POS_PL_NET  = 7;
  POS_BROKER  = 8;
  POS_TM      = 9;
  POS_TICKET  = 10;


  WM_GRID_SAVECNFG  = WM_USER + 9391;
  WM_GRID_RESET     = WM_USER + 9392;
  WM_GRID_POSITION  = WM_USER + 9393;
  WM_GRID_TSSTATUS  = WM_USER + 9394;
  WM_GRID_REAL_MD   = WM_USER + 9395;
  WM_GRID_SPEC      = WM_USER + 9396;
  WM_GRID_BEST_PRC  = WM_USER + 9397;
  WM_GRID_SENDING   = WM_USER + 9398;

  TIMEOUT_SENDMSG   = 500;

type

  //////////////////////////////////////////////////////////////////////////////
  // Basket
  TConfig = record
    symbol      : string;
    TargetPt    : string;
    OpenPtGap   : string;
    MaxTradeCnt : integer;
    TradingEndTm : string;
    bRollOver     : boolean;
    bTradingOn    : boolean;

  end;


  TPos = record
    sOpenPrc    : string;
    sLots       : string;
    sCurrPrc    : string;
    sPL         : string;
    sPlPt       : string;
    sPlNet      : string;
    sBroker     : string;
    sOpenTm     : string;
    sTicket     : string;
  end;

  TSymbolSpec = record
    nDecimalCnt : integer;
    dPipSize    : double;
    nActiveSpreadPt : integer;
    nTSPtOpen   : integer;
    nTSPtClose  : integer;
    nCommPt     : integer;
    nTargetPt   : integer;
  end;

  TBestPrc = record
    bidPrc       : string;
    bidBrokerKey : string;
    askPrc       : string;
    askBrokerKey : string;

    nGapBidAsk   : integer;
    nGapMax_Open : integer;  // Once gap goes over the open condition(comm*2),
                            // update this value whenever new max value occurrs.

    nGapMax_Close : integer;  // Once gap hits the target pt,
                            // update this value whenever new max value occurrs.
  end;

  TData = record
    symbol : string;
    bOpen  : boolean;

    Spec    : TSymbolSpec;
    Bestprc : TBestPrc;

    Long    : TPos;
    Short   : TPos;
  end;


  PTBroker = ^TBroker;
  TBroker = record
    BrokerKey  : string;  // a value which is input into EA when EA runs.
    BrokerName : string;
    Balance : double;
    Equity  : double;
    FreeMgn : double;
    Lvg     : integer;
    bStartMD:boolean;
  end;


  // Basket
  //////////////////////////////////////////////////////////////////////////////




//  TItemTsStatus = class(TObject)
//    iSymbol : integer;
//    iSide   : integer;
//    sStatus : string;
//
//    //WM_GRID_TSSTATUS  = WM_USER + 9394;
//  end;

//  TItemMD = class(TObject)
//    iSymbol : integer;
//    iSide   : integer;
//    sClose  : string;
//    sSpread : string;
//    sPlPip  : string;
//  end;


//  TItemBestPrc = class(TObject)
//    iSymbol : integer;
//    iSide   : integer;
//    sBestPip  : string;
//    sCutPip   : string;
//    //WM_GRID_BEST_PRC  = WM_USER + 9397;
//  end;
//
//  TItemSending = class(TObject)
//    iSymbol : integer;
//    iSide   : integer;
//    bBoth   : boolean;
//  end;



//type

//  TSymbolSpec = record
//    symbol  : string;
//    decimal : integer;
//    pipsize : double;
//    pipval  : double;
//  end;


var

  //PROTOTYPE
  //g_symbol    : array [1..MAX_SYMBOL] of string;

  // config 정보를 담을 array
  g_arrConfig : array[1..MAX_SYMBOL] of TConfig;

  // position 정보
  //g_arrPos : array[1..MAX_SYMBOL] of TPos;

  // broker 정보
  //g_arrBroker : array[1..MAX_BROKER] of TBroker;

  TRCVDATA : array of char;

  g_nBrokerCnt : integer; // the number of the brokers which are included in trading


  function __SideDesc(side:integer):string;
  function __ClrTpDesc(clrTp:string):string;


implementation



function __ClrTpDesc(clrTp:string):string;
begin
  Result := 'N/A';
       if clrTp = CLRTP_OPEN    then Result := 'Open'
  else if clrTp = CLRTP_PARTIAL then Result := 'Partial'
  else if clrTp = CLRTP_CLOSE   then Result := 'Clr';

end;

function __SideDesc(side:integer):string;
begin
  Result := 'BUY';
  if side = IDX_SELL then
    Result := 'SELL';

end;

end.
