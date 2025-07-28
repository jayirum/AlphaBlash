unit uCommonDef;


interface

USES
  System.sysutils, windows, Messages
  ;



const

  B_SIREN = TRUE;

  MAX_MASTERS_CNT = 10;
  MAX_STK         = 3;


  POS_MASTER  = 0;
  POS_ARTC    = 1;
  POS_STATUS  = 2;
  POS_SIDE    = 3;
  POS_QTY     = 4;
  POS_TM      = 5;
  POS_AVG     = 6;
  POS_NOWPRC  = 7;
  POS_PL_TICK = 8;
  POS_TS_STATUS = 9;
  POS_TS_BEST   = 10;
  POS_TS_SLSHIFT  = 11;
  POS_TS_CUT      = 12;
  POS_SL_PRC          = 13;
  POS_SL_TICK         = 14;
  POS_OPEN_TICKCOUNT  = 15;

  DIFF_PRC    = 0;
  DIFF_TICK   = 1;

  CNTR_SEQ      = 0;
  CNTR_ID       = 1;
  CNTR_TM       = 2;
  CNTR_STK      = 3;
  CNTR_SIDE     = 4;
  CNTR_CLR_TP   = 5;
  CNTR_PRC      = 6;
  CNTR_QTY      = 7;
  CNTR_PL_TICK  = 8;
  CNTR_PL       = 9;
  CNTR_ORD_TP   = 10;
  CNTR_LVG      = 11;
  CNTR_CNTR_NO  = 12;

  TICK_ARTC = 0;
  TICK_PRC  = 1;
  TICK_TM   = 2;


  POS_STATUS_OPEN  = '진입';
  POS_STATUS_NONE  = '무포';

  CLR_TP_OPEN     = '1';
  CLR_TP_PARTIAL  = '2';
  CLR_TP_CLR      = '3';
  CLR_TP_RVS      = '4';

  SIDE_BUY  = 'B';
  SIDE_SELL = 'S';


  LOGIN_IN  = 'I';
  LOGIN_OUT = 'O';


  // PosGrid 를 그리기 위한 메세지
  WM_GRID_REAL_MD   = WM_USER + 9191;
  WM_GRID_TS_STATUS = WM_USER + 9192;
  WM_GRID_POSITION  = WM_USER + 9193;
  WM_GRID_POSCLR    = WM_USER + 9194;
  WM_GRID_POSINSERT = WM_USER + 9195;
  WM_GRID_CNTR      = WM_USER + 9196;

  TIMEOUT_SENDMSG = 500;


  SL_NONE   = 'NONE';
  SL_SINGLE = 'SINGLE';
  SL_MULTI  = 'MULTI';
  SL_NONE_DESC   = 'SL미설정';
  SL_SINGLE_DESC = 'SL단일설정';
  SL_MULTI_DESC  = 'SL복수설정';

type

  RET_TS        = (RET_CHANGE, RET_NON_CHANGE);
  UPDATE_POS_TP = (POS_MD, POS_POS, POS_FULL);
  //TTS_STATUS     = (TS_NONE, TS_SKIPCLR, TS_SLSHIFT, TS_CUT);
  TTS_STATUS     = (TS_NONE, TS_SLSHIFT, TS_CUT);


  TItemMD = class(TObject)
    idxPrcGrid  : integer;
    idxPosGrid  : integer;
    artc        : string;
    close       : string;
    time        : string;
  end;

  TItemPos = class(TObject)
    idx       : integer;
    masterId  : string;
    artc      : string;
    tsSLShift : string;
    tsCutPrc  : string;
    slCutPrc  : string;
    slTick    : string;
  end;

  TItemTSStatus = class(TObject)
    idx       : integer;
    tsStatus  : string;
    tsBestPrc : double;
    slPrc     : double;
    slTick    : double;
  end;

  TItemCntr = class(TObject)
    idx       : integer;
    posStatus : string;
    side      : string;
    tm        : string;
    qty       : string;
    avg       : string;
  end;


   TMASTER_ORD = record
    masterId  : string;
    cntrNo  : string;
    //stkCd   : string;
    artc    : string;
    side    : string;
    cntrQty : string;
    cntrPrc : string;
    clrPl   : string;
    cmsn    : string;
    clrTp   : string;
    bf_nclrQty  : string;
    af_nclrQty  : string;
    bf_avgPrc   : string;
    af_avgPrc   : string;
    bf_amt      : string;
    af_amt      : string;
    ordTp       : string;
    tradeTm     : string;
    lvg         : string;
  end;


  TMYPOS = RECORD
    artc    : string;
    status  : string;
    sideTp  : string;
    avg     : string;
    qty     : string;
    tm      : string;
    lastTickTm : string;
  END;

  TSISE = RECORD
    artc : array[0..5] of char;
    close: array[0..9] of char;
  end;

//  TTSSETTING = RECORD
//    bTS         : boolean;
//    tsStartTick : double;
//    tsStepTick  : double;
//    tsCutRate   : double;
//
//    slTp        : string; // SL_NONE, SL_SINGLE, SL_MULTI
//    slTickA     : double;
//    slTickB     : double;
//  end;



  function __OrdTp(tp:string):string;
  function __Side(side:string):string;
  function __SideTp(side:string):string;
  function __LoginTp(tp:string):string;
  function __ClrTp(tp:string):string;
  function __TickSize(stk:string):double;
  function __DotCnt(stk:string):integer;
  function __PrcFmtD(stk:string;prc:double):string;
  function __PrcFmt(stk:string;prc:string):string;
  function __MoneyFmt(amt:string):string;
  function __MoneyFmtD(amt:double):string;
  function __IsSameSymbol(rcvStk:string; artc:string):boolean;
  function __ExtractArtcCd(stk:string):string;
  function __Artc(stk:string):string;
  function __CalcTimeGapSec(startTick:double):integer;
  function __CvtCommaAmt(amt:string):integer;
  function __IsSamePrc(prc1:double; prc2:double; dotcnt:integer):boolean;
  function __SlTp(desc:string):string;
  function __SlTpDesc(tp:string):string;
  function __Status_Desc(status:TTS_STATUS):string;

var
  __hookX, __hookY : integer;

implementation



function __Status_Desc(status:TTS_STATUS):string;
BEGIN
    Result := inttostr(
                    Ord(status)
             );
END;

function __OrdTp(tp:string):string;
var
  i : integer;
begin

  i := strtoint(tp);

  case i of
  1: Result := '시장가';
  2: Result := '지정가';
  9: Result := '종목청산';
  10: Result := '전체청산';
  12: Result := '장마감청산';
  14: Result := '로스컷청산';
  15: Result := '관리자청산';
  17: Result := 'SL손절';
  18: Result := 'SL익절';
  end;
end;



function __SideTp(side:string):string;
begin
  Result := '';
  if side='매도' then
    Result := SIDE_SELL
  else if side='매수' then
    Result := SIDE_BUY;

end;


function __Side(side:string):string;
begin

  Result := '';
  if side=SIDE_SELL then
    Result := '매도'
  else if side=SIDE_BUY then
  Result := '매수';
end;

function __LoginTp(tp:string):string;
begin
  Result := '';

  if tp=LOGIN_OUT then
    Result := '로그아웃'
  else if tp=LOGIN_IN then
    Result := '로그인';

end;



function __ClrTp(tp:string):string;
begin
  Result := '';
  if tp=CLR_TP_PARTIAL then Result := '일부청산'
  else if tp=CLR_TP_CLR then Result :='청산'
  else if tp=CLR_TP_RVS then Result :='역전'
  else if tp=CLR_TP_OPEN then Result :='진입';
end;


function __TickSize(stk:string):double;
begin

       if pos(stk,'CL')>0 then  Result := 0.01
  else if pos(stk,'GC')>0 then  Result := 0.1
  else if pos(stk,'ES')>0 then  Result := 0.25
  else if pos(stk,'HSI')>0 then Result := 1
  else if pos(stk,'SCN')>0 then Result := 0.5
  else if pos(stk,'NQ')>0 then  Result := 0.25
  else if pos(stk,'URO')>0 then Result := 0.00005
  else if pos(stk,'6E')>0 then  Result := 0.00005
  else if pos(stk,'JY')>0 then  Result := 0.5
  else if pos(stk,'6J')>0 then  Result := 0.5
  else if pos(stk,'BP')>0 then  Result := 0.0001
  else if pos(stk,'6B')>0 then  Result := 0.0001
  else if pos(stk,'AD')>0 then  Result := 0.0001
  else if pos(stk,'6A')>0 then  Result := 0.0001
  else if pos(stk,'YM')>0 then  Result := 1
  else if pos(stk,'101')>0 then Result := 0.05
  else                          Result := 1;

end;


function __DotCnt(stk:string):integer;
begin

       if pos(stk,'CL')>0 then  Result := 2//0.01
  else if pos(stk,'GC')>0 then  Result := 1//0.1
  else if pos(stk,'ES')>0 then  Result := 2//0.25
  else if pos(stk,'HSI')>0 then Result := 0//1
  else if pos(stk,'SCN')>0 then Result := 1//0.5
  else if pos(stk,'NQ')>0 then  Result := 2//0.25
  else if pos(stk,'URO')>0 then Result := 5//0.00005
  else if pos(stk,'6E')>0 then  Result := 5//0.00005
  else if pos(stk,'JY')>0 then  Result := 1//0.5
  else if pos(stk,'6J')>0 then  Result := 1//0.5
  else if pos(stk,'BP')>0 then  Result := 4//0.0001
  else if pos(stk,'6B')>0 then  Result := 4//0.0001
  else if pos(stk,'AD')>0 then  Result := 4//0.0001
  else if pos(stk,'6A')>0 then  Result := 4//0.0001
  else if pos(stk,'YM')>0 then  Result := 0//1
  else if pos(stk,'101')>0 then Result := 2//0.05
  else                          Result := 0;//1;

end;

function __Artc(stk:string):string;
begin
  Result := __ExtractArtcCd(stk);
end;

function __ExtractArtcCd(stk:string):string;
begin

       if pos('CL',stk)>0 then  Result := 'CL'
  else if pos('GC',stk)>0 then  Result := 'GC'
  else if pos('ES',stk)>0 then  Result := 'ES'
  else if pos('HSI',stk)>0 then Result := 'HSI'
  else if pos('SCN',stk)>0 then Result := 'SCN'
  else if pos('NQ',stk)>0 then  Result := 'NQ'
  else if pos('URO',stk)>0 then Result := '6E'
  else if pos('6E',stk)>0 then  Result := '6E'
  else if pos('JY',stk)>0 then  Result := '6J'
  else if pos('6J',stk)>0 then  Result := '6J'
  else if pos('BP',stk)>0 then  Result := '6B'
  else if pos('6B',stk)>0 then  Result := '6B'
  else if pos('AD',stk)>0 then  Result := '6A'
  else if pos('6A',stk)>0 then  Result := '6A'
  else if pos('YM',stk)>0 then  Result := 'YM'
  else if pos('101',stk)>0 then Result := '101'
  else
    Result := '   ';
    //Result := 'N/A';

end;

function __PrcFmtD(stk:string;prc:double):string;
begin
       if pos('CL',stk)>0 then  Result := formatfloat('#.#0', prc)//0.01
  else if pos('GC',stk)>0 then  Result := formatfloat('#.#0', prc)//1//0.1
  else if pos('ES',stk)>0 then  Result := formatfloat('#.#0', prc)//2//0.25
  else if pos('HSI',stk)>0 then Result := formatfloat('#0', prc)//0//1
  else if pos('SCN',stk)>0 then Result := formatfloat('#.0', prc)//1//0.5
  else if pos('NQ',stk)>0 then  Result := formatfloat('#.#0', prc)//2//0.25
  else if pos('URO',stk)>0 then Result := formatfloat('#.####0', prc)//5//0.00005
  else if pos('6E',stk)>0 then  Result := formatfloat('#.####0', prc)//5//0.00005
  else if pos('JY',stk)>0 then  Result := formatfloat('#.0', prc)//1//0.5
  else if pos('6J',stk)>0 then  Result := formatfloat('#.0', prc)//1//0.5
  else if pos('BP',stk)>0 then  Result := formatfloat('#.###0', prc)//4//0.0001
  else if pos('6B',stk)>0 then  Result := formatfloat('#.###0', prc)//4//0.0001
  else if pos('AD',stk)>0 then  Result := formatfloat('#.###0', prc)//4//0.0001
  else if pos('6A',stk)>0 then  Result := formatfloat('#.###0', prc)//4//0.0001
  else if pos('YM',stk)>0 then  Result := formatfloat('#0', prc)//0//1
  else if pos('101',stk)>0 then Result := formatfloat('#.#0', prc)//2//0.05
  else                          Result := formatfloat('#0', prc);//0//1

end;

function __IsSameSymbol(rcvStk:string; artc:string):boolean;
begin

  if (Pos('6E', rcvStk)>0) or (Pos('URO', rcvStk)>0) then
  begin
    if (artc = '6E') OR (artc='URO') then
      Result := True;
  end;

end;


function __PrcFmt(stk:string;prc:string):string;
var
  dPrc : double;
begin

    dPrc := strtofloatdef(prc,0);
    Result := __PrcFmtD(stk, dPrc);

end;

function __MoneyFmt(amt:string):string;
begin
  Result := __MoneyFmtD(strtofloatdef(amt,0));
end;


function __MoneyFmtD(amt:double):string;
begin
  Result := formatfloat('#,##0', amt);
end;


function __CalcTimeGapSec(startTick:double):integer;
var
  gap : double;
begin
  gap := GetTickCount() - startTick;

  Result := round( gap / 1000);

end;


// 1,234 ==> 1234
function __CvtCommaAmt(amt:string):integer;
var
  sPure : string;
begin

  sPure := StringReplace(amt, ',', '', [rfReplaceall]);

  Result := strtointdef(sPure, 0);

end;

function __IsSamePrc(prc1:double; prc2:double; dotcnt:integer):boolean;
var
  s1, s2:string;
begin

  if dotcnt=0 then
  begin
      s1 := formatfloat('#0', prc1);
      s2 := formatfloat('#0', prc2);
  end
  else if dotcnt=1 then
  begin
      s1 := formatfloat('#.0', prc1);
      s2 := formatfloat('#.0', prc2);
  end
  else if dotcnt=2 then
  begin
      s1 := formatfloat('#.#0', prc1);
      s2 := formatfloat('#.#0', prc2);
  end
  else if dotcnt=3 then
  begin
      s1 := formatfloat('#.##0', prc1);
      s2 := formatfloat('#.##0', prc2);
  end
  else if dotcnt=4 then
  begin
      s1 := formatfloat('#.###0', prc1);
      s2 := formatfloat('#.###0', prc2);
  end
  else if dotcnt=5 then
  begin
      s1 := formatfloat('#.####0', prc1);
      s2 := formatfloat('#.####0', prc2);
  end;

  Result := (s1=s2);
end;


function __SlTp(desc:string):string;
begin

        if desc = SL_NONE_DESC then Result := SL_NONE
  else  if desc = SL_SINGLE_DESC then Result := SL_SINGLE
  else  if desc = SL_MULTI_DESC then Result := SL_MULTI
  else  Result := SL_NONE;
end;


function __SlTpDesc(tp:string):string;
BEGIN
        if tp = SL_NONE then Result := SL_NONE_DESC
  else  if tp = SL_SINGLE then Result :=SL_SINGLE_DESC
  else  if tp = SL_MULTI then Result :=SL_MULTI_DESC
  else  Result := SL_NONE_DESC;
END;



end.
