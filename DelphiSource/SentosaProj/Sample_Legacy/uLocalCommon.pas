unit uLocalCommon;

interface

Uses
  System.sysutils, windows, Messages
  ;

type
  TPL_STATUS = (PL_NONE, PL_P1, TS_1, TS_2);

  TItemCnfg = class(TObject)
    iSymbol     : integer;
    keyBuy      : string;
    keySell     : string;
    brokerBuy   : string;
    brokerSell  : string;
    //WM_GRID_SAVECNFG  = WM_USER + 9391;
  end;

  TItemReset = class(TObject)
    iSymbol : integer;
    iSide   : integer;
    //WM_GRID_RESET  = WM_USER + 9392;
  end;

  TItemPos = class(TObject)
    iSymbol : integer;
    iSide   : integer;
    sTicket  : string;
    sOpenPrc  : string;
    sLots     : string;
    sPL       : string;
    sPlPip    : string;
    sClrTp    : string;
    //WM_GRID_POSITION  = WM_USER + 9393;
  end;

  TItemTsStatus = class(TObject)
    iSymbol : integer;
    iSide   : integer;
    sStatus : string;

    //WM_GRID_TSSTATUS  = WM_USER + 9394;
  end;

  TItemMD = class(TObject)
    iSymbol : integer;
    iSide   : integer;
    sClose  : string;
    sSpread : string;
    sPlPip  : string;

    //WM_GRID_REAL_MD   = WM_USER + 9395;
  end;

  TItemSpec = class(TObject)
    iSymbol : integer;
    iSide   : integer;
    sDecimal  : string;
    sPipSize  : string;

    //WM_GRID_SPEC      = WM_USER + 9396;
  end;

  TItemBestPrc = class(TObject)
    iSymbol : integer;
    iSide   : integer;
    sBestPip  : string;
    sCutPip   : string;
    //WM_GRID_BEST_PRC  = WM_USER + 9397;
  end;

  TItemSending = class(TObject)
    iSymbol : integer;
    iSide   : integer;
    bBoth   : boolean;
  end;



const
  MAX_SYMBOL  = 2;
  MAX_CTX     = 10;

  IDX_BUY  = 1;
  IDX_SELL = 2;

  Q_RECV = 0;
  Q_SEND = 1;

  CLRTP_NONE    = '0';
  CLRTP_OPEN    = '1';
  CLRTP_PARTIAL = '2';
  CLRTP_CLOSE   = '3';

  POS_KEY     = 0;
  POS_BROKER  = 1;
  POS_TICKET  = 2;
  POS_CLR_TP  = 3;
  POS_LOTS    = 4;
  POS_OPENPRC = 5;
  POS_NOWPRC  = 6;
  POS_SPREAD  = 7;
  POS_PL      = 8;
  POS_PL_PIP  = 9;
  POS_PL_STATUS   = 10;
  POS_TS_BEST     = 11;
  POS_TS_CUTPRC   = 12;
  POS_SENDING     = 13;
  POS_TS_LVL_1    = 14;
  POS_TS_LVL_2    = 15;
  POS_DECIMAL     = 16;
  POS_PIPSIZE     = 17;
  POS_PTSIZE      = 18;


  WM_GRID_SAVECNFG  = WM_USER + 9391;
  WM_GRID_RESET     = WM_USER + 9392;
  WM_GRID_POSITION  = WM_USER + 9393;
  WM_GRID_TSSTATUS  = WM_USER + 9394;
  WM_GRID_REAL_MD   = WM_USER + 9395;
  WM_GRID_SPEC      = WM_USER + 9396;
  WM_GRID_BEST_PRC  = WM_USER + 9397;
  WM_GRID_SENDING   = WM_USER + 9398;

  TIMEOUT_SENDMSG   = 500;

//type

//  TSymbolSpec = record
//    symbol  : string;
//    decimal : integer;
//    pipsize : double;
//    pipval  : double;
//  end;


var

  TRCVDATA : array of char;


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
