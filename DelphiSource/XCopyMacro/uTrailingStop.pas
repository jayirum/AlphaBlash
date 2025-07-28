unit uTrailingStop;

interface

uses
  System.Classes, system.SysUtils, winapi.windows
  , uCommonDef, VCL.dialogs, VCL.Forms
  ;


type


  TInfo = record
    master    : string;
    
    // 품목정보
    artc      : string;
    ticksize : double;
    dotcnt   : integer;

    // 현재가
    //lastprc       : double;
    //nowprc        : double;

    // 포지션 정보
    //side: string;
    //avg : double;
    //qty : double;

    // 계산값
    bBeingCut   : boolean;
    //prcSkipClr  : double;
    prcSLShift  : double;
    prcTSCut    : double;
    prcSLCut    : double;
    tsStatus    : TTS_STATUS;
    prcTSbest   : double;     // 현재까지 최고 가격(현재가변경시)
//    prcLvl_1    : double;
//    prcLvl_2    : double;
//    prcLvl_3    : double;
  end;

  TQDATA = RECORD
    code  : string;
    master: string;
    artc  : string;
    prc   : string;
    clrTp : string;
  END;

  TTS = class(TThread)
  public
    constructor Create;
    destructor Destroy; override;

    procedure Init_TS(idx:integer);                                             // 설정 check
    procedure Clear_TS(idx:integer);                                            // 설정 uncheck

    procedure Update_Tick(artc:string; prc:string);                             // 시세스레드(tickthrd) 에서 시세 전달
    procedure Update_Pos(masterId, artc:string; clrTp:string);                            // 주문스레드(ordthrd) 에서 체결 전달

    function  TsCount():integer;                                                // TS 설정한 종목 수

    //procedure Update_TSsetting(idx:integer);
    function  GetIdxForPos(masterId, artc:string):integer;                      

    function IsTS_Running(idx:integer):boolean;

    function Calc_PLTick(idx:integer; nowPrc:string):string;
    
  private
    function  _Main_TS_MarketData(artc:string; newprc:string; idx:integer):RET_TS;            // 수신받은 시세로 TS 관련 계산
    function  _Main_TS_Pos(masterId, artc:string; clrTp:string; var idx:integer):RET_TS;                    // 수신받은 체결정보로 TS 관련 계산
    function  _Main_SL_MarketData(artc:string; newprc:string; idx:integer):RET_TS; // SL 점검

    // 수익컷을 수행한다.
    function  _ProfitCut(idx:integer):RET_TS;                            // 수익컷 계산 및 처리

  private // Util

    // tick 만큼 떨어긴 가격 구하기
    function  Calc_PrcAwayTick(side:string; basePrc:double;                     // 첫익절 또는 NextBasePrc 계산
                              tickCnt:double; tickSize:double; bSL:boolean=false):double;

    // 현재가가 특정 가격 over/touch 했는지 점검
    function  IsTouched_BasePrc(idx:integer; side:string;                           // BasePrc 를 현재가격이 터치하는지여부
                              basePrc:double; nowPrc:double):boolean;

    // 포지션avg 이용 TSPRICE 와 SLPRICE 재계산
    function Calc_TSPrc_SLPrc(idx:integer):boolean;

    // 현재가격을 이용해서 TS STATUS 재계산
    function Calc_TS_Status(idx:integer):boolean;

    // 포지션이 존재하는가
    function  ExistPosition(idx:integer):boolean;

    //procedure Repaint_PosGrid();
    procedure Update_PosGrid(idx:integer;tp:UPDATE_POS_TP);

    // shift SL price
    function ShiftSLPrc(idx:integer):double;


  public //Grid 단축 함수들
    function PosSide(idx:integer):string;
    function PosAvg(idx:integer):double;
    function PosQty(idx:integer):double;
    function PosNowPrc(idx:integer):double;

  protected
    procedure Execute();override;
  private
    m_nSetCnt     : array[1..MAX_STK] of integer; // TS 설정한 종목 갯수
    m_listRcvData : TList;
    m_cs          : TRTLCriticalSection;
    //m_chgIdx      : integer;
  public
    m        : array[1..MAX_STK] of TInfo;

  end;

var
  __ts : TTS;

  //__myPos : array[1..MAX_STK] of TMYPOS;

implementation

uses
  uMain, uPrcList, uSettingTs, uOrdThrd, unotify, commonutils
  ;

const
  EVENT_TICK  = 'TICK';
  EVENT_POS   = 'POS';


constructor TTS.Create;
var
  i : integer;
begin
  inherited;

  InitializeCriticalSection(m_cs);
  m_listRcvData := TList.Create;

  for i := 1 to MAX_STK do
  begin
    Clear_TS(i);
  end;

end;


destructor TTS.Destroy;
begin
  inherited;
  FreeAndNil(m_listRcvData);
  DeleteCriticalSection(m_cs);
end;



procedure TTS.Execute;
var
  data  : ^TQDATA;
  retTs : RET_TS;
  idx   : integer;
  //nChanged : integer;
begin

  while NOT Terminated do
  begin
      Sleep(10);

      EnterCriticalSection(m_cs);
      if m_listRcvData.Count=0 then
      begin
        LeaveCriticalSection(m_cs);
        continue;
      end;

      data := m_listRcvData.Items[0];
      m_listRcvData.Delete(0);
      LeaveCriticalSection(m_cs);

      idx := -1;
      //nChanged := 0;
      if data.code=EVENT_TICK then
      BEGIN
        for idx := 1 to MAX_STK do
        BEGIN
          if m[idx].artc = data.artc then
          begin
            retTs := _Main_TS_MarketData(data.artc, data.prc, idx);

            retTs := _Main_SL_MarketData(data.artc, data.prc, idx);

          end;
        END;

      END
      else if data.code=EVENT_POS then
      BEGIN
        retTs := _Main_TS_Pos(data.master, data.artc, data.clrtp, idx);
      END;

      //TODO
//      if nChanged > 0 then
//      begin
//        m_chgIdx := idx;
//        Synchronize(Repaint_PosGrid);
//      end;

      Dispose(data);
  end;
end;



function TTS.PosSide(idx:integer):string;
var
  sideDesc : string;
begin

  sideDesc := fmMain.gdPosMine.Cells[POS_SIDE, idx];

  Result := 'B';
  if sideDesc = '매도' then
    Result := 'S';

end;

function TTS.PosAvg(idx:integer):double;
begin
  Result := strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG, idx],0);
end;

function TTS.PosQty(idx:integer):double;
begin
  Result := strtofloatdef(fmMain.gdPosMine.Cells[POS_QTY, idx],0);
end;

function TTS.PosNowPrc(idx:integer):double;
begin
  Result := strtofloatdef(fmMain.gdPosMine.Cells[POS_NOWPRC, idx],0);
end;


// 첫익절 또는 NextBasePrc 계산
function TTS.Calc_PrcAwayTick(side:string; basePrc:double;
                              tickCnt:double; tickSize:double; bSL:boolean=false):double;
begin
  if side = SIDE_BUY then
  begin
    if bSL then
      Result := basePrc - (tickCnt * ticksize)
    else
      Result := basePrc + (tickCnt * ticksize);
  end
  else if side = SIDE_SELL then
  begin
    if bSL then
      Result := basePrc + (tickCnt * ticksize)
    else
      Result := basePrc - (tickCnt * ticksize);
  end
  else
    Result := 0;
end;

function TTS.IsTouched_BasePrc(idx:integer; side:string; basePrc:double; nowPrc:double):boolean;
var
  currPrc : double;
begin

  if nowPrc=0 then  currPrc := strtofloatdef(__prcList.Prc(m[idx].artc),0)
  else              currPrc := nowPrc;

  Result := false;

  if side = SIDE_BUY then
  begin
    if basePrc <= currPrc then
      Result := true;
  end;
  if side = SIDE_SELL then
  begin
    if basePrc >= currPrc then
      Result := true;
  end;
end;

function TTS.ExistPosition(idx:integer):boolean;
begin
  Result := False;

  if ( fmMain.gdPosMine.Cells[POS_STATUS, idx]=POS_STATUS_OPEN) and
     ( strtointdef(fmMain.gdPosMine.Cells[POS_QTY, idx],0) > 0 ) and
     ( PosAvg(idx) > 0 ) then
  begin
    Result := True;
  end;


end;

//procedure TTS.Update_CurRate(idx:integer;rate:string);
//begin
//  EnterCriticalSection(m_cs);
//
//  try
//    m[idx].cut_rate := strtofloatdef(rate,0)/100.;
//  finally
//    LeaveCriticalSection(m_cs);
//  end;
//
//end;


function  TTS.TsCount():integer;
var
  i : integer;
BEGIN

  Result :=0;
  for i := 1 to MAX_STK do
    Result := Result + m_nSetCnt[i];

END;



function TTS.Calc_TSPrc_SLPrc(idx:integer):boolean;
VAR
  s : string;
begin

  Result := False;

  if ExistPosition(idx) = false then
    exit;

  if __IsTSChecked(idx) then
  begin
    Result := True;

//    m[idx].prcSkipClr := Calc_PrcAwayTick( PosSide(idx),
//                                            PosAvg(idx),
//                                            __TSLevel_Tick(idx, TS_SKIPCLR),
//                                            m[idx].ticksize
//                                            );

    m[idx].prcSLShift := Calc_PrcAwayTick( PosSide(idx),
                                            PosAvg(idx),
                                            __TSLevel_Tick(idx, TS_SLSHIFT),
                                            m[idx].ticksize
                                            );

    m[idx].prcTSCut := Calc_PrcAwayTick( PosSide(idx),
                                            PosAvg(idx),
                                            __TSLevel_Tick(idx, TS_CUT),
                                            m[idx].ticksize
                                           );

    m[idx].prcSLCut := Calc_PrcAwayTick( PosSide(idx),
                                            PosAvg(idx),
                                            __SLTick(idx),
                                            m[idx].ticksize,
                                            true // SL
                                           );
  end;

  if PosAvg(idx) <=0 then
  BEGIN
  s := 'SL 이상';
  fmMain.AddMsg(s);

  END;

end;

// main 에서 check box check 할때
procedure TTS.Init_TS(idx:integer);
var
  dNowPrc : double;
  artc    : string;
  bTSCheked : boolean;
begin


  //--------------------------------------------------------------------------//
  artc := fmMain.m_setting[idx].artcCd.Text;
  if artc='' then
  begin
    fmMain.AddMsg('ID 와 종목을 먼저 선택하세요',false, true);
    exit;
  end;
  // master id
  m[idx].master    := fmMain.m_setting[idx].master.Text;

  // 품목정보
  m[idx].artc      := artc;
  m[idx].ticksize  := __TickSize(m[idx].artc);
  m[idx].dotcnt    := __DotCnt(m[idx].artc);

  dNowPrc := strtofloatdef(__prcList.Prc(artc),0);

  bTSCheked := __IsTSChecked(idx);

  if (bTSCheked=False) then
  begin
    m[idx].bBeingCut   := false;
    //m[idx].prcSkipClr  := 0;
    m[idx].prcSLShift  := 0;
    m[idx].prcTSCut    := 0;
    m[idx].prcSLCut    := 0;
    m[idx].prcTSbest   := 0;
    m[idx].tsStatus    := TS_NONE;

    Update_PosGrid (idx, POS_FULL);

    exit;
  end;


  //--------------------------------------------------------------------------//
  // 포지션이 있으면 TSPrc 와 SLPrc 재계산
  if Calc_TSPrc_SLPrc(idx) = True Then
  begin

    if IsTouched_BasePrc(idx, PosSide(idx), m[idx].prcSLShift, dNowPrc)=true then
    begin
      fmMain.AddMsg('ERR! S/L Shift 가격터치. 다시 설정하세요.',true);
      exit;
    end;

    m[idx].tsStatus   := TS_NONE;
    m[idx].prcTSbest  := 0;

  end;

  if dNowPrc>0 then
  begin
    // 현재가 정보
//    m[idx].lastprc := m[idx].nowprc;
//    m[idx].nowprc  := dNowPrc;

//    if m[idx].side=SIDE_BUY then
//    BEGIN
//      if dNowPrc > m[idx].prcTSbest then
//        m[idx].prcTSbest := dNowPrc;
//    END;
//
//    if m[idx].side=SIDE_SELL then
//    BEGIN
//      if dNowPrc < m[idx].prcTSbest then
//        m[idx].prcTSbest := dNowPrc;
//    END;

  end;

  inc(m_nSetCnt[idx]);

  Update_PosGrid (idx, POS_FULL);

end;


// main 에서 check box un check 할때
procedure TTS.Clear_TS(idx:integer);
begin

  //현재가
//  m[idx].lastprc      := 0;
//  m[idx].nowprc       := 0;
//
//  // 포지션정보
//  m[idx].side      := '';
//  m[idx].avg       := 0;
//  m[idx].qty       := 0;


  // 계산값
  m[idx].bBeingCut  := false;
  //m[idx].prcSkipClr := 0;
  m[idx].prcSLShift := 0;
  m[idx].prcTSCut   := 0;
  m[idx].prcTSbest  := 0;
  m[idx].prcSLCut   := 0;
  m[idx].tsStatus   := TS_NONE;

  dec(m_nSetCnt[idx]);
  if m_nSetCnt[idx]<0 then
    m_nSetCnt[idx] := 0;
end;



function  TTS.GetIdxForPos(masterId, artc:string):integer;
var
  idx  : integer;
  find : boolean;
begin

  find := false;
  for idx := 1 to MAX_STK do
  begin
    if (m[idx].master = masterId) and (m[idx].artc = artc) then
    begin
      find := true;
      break;
    end;
  end;

  if not find then
  begin
    Result := -1;
    exit;
  end;

  Result := idx;
end;


{
  Level 2 부터 점검한다.

}

function  TTS._ProfitCut(idx:integer):RET_TS;
var
  dCutPrc   : double;
  bClrOrd   : boolean;
  msg       : string;
  dTick     : double;
  dLevelPrc : double;

  side      : string;
  avg,nowprc : double;
begin

  Result  := RET_NON_CHANGE;
  bClrOrd := False;

  if Ord(m[idx].tsStatus) < Ord(TS_CUT)  then
    exit;

  dTick     := __OffSet_Tick(idx);
  dLevelPrc := m[idx].prcTSCut;

  side    := PosSide(idx);
  avg     := PosAvg(idx);
  nowprc  := PosNowPrc(idx);


  if side = SIDE_BUY then
  BEGIN
    if (m[idx].prcTSbest > nowprc) then
    begin

      dCutPrc := m[idx].prcTSBest - ( dTick * m[idx].ticksize);

      if dCutPrc >= nowprc then
      begin

        // 단타점검
        if fmMain.IsScalping(idx) then
        begin
          fmMain.AddMsg('[익절실패] 단타방지');
          exit;
        end;

        bClrOrd := True;
        msg := format('[매수TS컷-Level:%d][Best:%f] (cutprc:%f)>(현재가:%f)(평단:%f)(PLTick:%f)',
                      [
                        Ord(m[idx].tsStatus),
                        m[idx].prcTSBest,
                        dCutPrc,
                        nowprc,
                        avg,
                        (nowprc-avg)/m[idx].ticksize
                      ]
                      );
      end;

    end;

  END;

  if side = SIDE_SELL then
  BEGIN
    if (m[idx].prcTSbest < nowprc) then
    begin

      dCutPrc := m[idx].prcTSBest + ( dTick * m[idx].ticksize);

      if dCutPrc <= nowprc then
      begin
              // 단타점검
        if fmMain.IsScalping(idx) then
        begin
          fmMain.AddMsg('[익절실패] 단타방지');
          exit;
        end;


        bClrOrd := True;
        msg := format('[매도TS컷-Level:%d][Best:%f] (cutprc:%f)<(현재가:%f)(평단:%f)(PLTick:%f)',
                      [
                        Ord(m[idx].tsStatus),
                        m[idx].prcTSBest,
                        dCutPrc,
                        nowprc,
                        avg,
                        (avg-nowprc)/m[idx].ticksize
                      ]
                      );

      end;

    end;

  END;

  if bClrOrd then
  begin

    m[idx].bBeingCut := true;

    //주문처리
    __ordThrd[idx].Clear_TS_SL(m[idx].artc, idx, side,'TS익절');

    fmMain.AddMsg(msg, B_SIREN);

    // 포지션정보 초기화
    Clear_TS(idx);

    Result := RET_CHANGE;
  end;

end;




function  TTS._Main_SL_MarketData(artc:string; newprc:string; idx:integer):RET_TS; // SL 점검
var
  bCut    : boolean;
  dNewprc : double;
  msg     : string;

  side      : string;
  avg,nowprc : double;
  dPltick    : double;
begin

  Result := RET_NON_CHANGE;

  // 청산 중
  if m[idx].bBeingCut=true then
    exit;

  if __IsTSChecked(idx)=false then
    exit;


  // 포지션이 없으면 하지 않는다.
  if ExistPosition(idx)=False then
    exit;




  // 아직 SL 계산전이면 넘어간다.
  if m[idx].prcSLCut <=0 then
  begin
    //fmMain.AddMsg('SL계산전!!!');
    exit;
  end;

  side    := PosSide(idx);
  avg     := PosAvg(idx);
  nowprc  := PosNowPrc(idx);


  // LONG   : SLPRC > NOWPRC
  // SHORT  : SLPRC < NOWPRC
  bCut := False;
  if side = SIDE_BUY then
  BEGIN
    if (m[idx].prcSLCut >= nowprc) then
    begin
      dPlTick := (nowprc - avg)/m[idx].ticksize;

      msg := format('[손절-매수](%s)(SLPrc:%f) > (NowPrc:%f),(평단:%f)(SLTick:%f)(PLTick:%f)',
                            [
                            m[idx].artc,
                            m[idx].prcSLCut,
                            nowprc,
                            avg,
                            __SLTick(idx),
                            dPlTick
                            ]);
      fmMain.AddMsg(msg);
//      if dPlTick < __SLTick(idx) then
//      begin
//        bCut := False;
//        fmMain.AddMsg(format('[손절-매수실패] 손익틱(%f) < SLTick(%f)', [dPlTick, __SLTick(idx)]));
//      end
//      else
        bCut := True;
    end;
  END;

  if side = SIDE_SELL then
  BEGIN
    if (m[idx].prcSLCut <= nowprc) then
    begin
      dPlTick := (avg - nowprc)/m[idx].ticksize;

            msg := format('[손절-매도](%s)(SLPrc:%f) < (NowPrc:%f),(평단:%f)(SLTick:%f)(PLTick:%f)',
                            [
                            m[idx].artc,
                            m[idx].prcSLCut,
                            nowprc,
                            avg,
                            __SLTick(idx),
                            dPlTick
                            ]);
      fmMain.AddMsg(msg);
//      if dPlTick < __SLTick(idx) then
//      begin
//        bCut := False;
//        fmMain.AddMsg(format('[손절-매도실패] 손익틱(%f) < SLTick(%f)', [dPlTick, __SLTick(idx)]));
//      end
//      else
        bCut := True;

    end;
  END;

  if bCut then
  begin
    m[idx].bBeingCut := True;

    //주문처리
    __ordThrd[idx].Clear_TS_SL(m[idx].artc, idx, side,'손절');
    Clear_TS(idx);
  end;

end;



// 새로운 시세 들어온 경우 처리
function TTS._Main_TS_MarketData(artc:string; newprc:string; idx:integer):RET_TS;
var
  nowprc  : double;
  dNewprc : double;
  nChangeCnt : integer;
  side    : string;
begin

  Result := RET_NON_CHANGE;

  // 청산 중
  if m[idx].bBeingCut=true then
    exit;

  // 포지션이 없으면 하지 않는다.
  if Not ExistPosition(idx) then
    exit;

  if __IsTSChecked(idx)=false then
    exit;

  // TS 값이 아직 계산 전이면 넘어간다.
  if //(m[idx].prcSkipClr <=0) or
     (m[idx].prcSLShift <=0) or
     (m[idx].prcTSCut <=0) then
  begin
    fmMain.AddMsg('TS 계산전!!!');
    exit;
  end;


  nowprc  := PosNowPrc(idx);
  dNewprc := strtofloatdef(newprc,0);

  // 가격변화가 없으면 하지 않는다.
//  if __IsSamePrc( nowprc, dNewprc, m[idx].dotcnt)=true then
//  begin
//    exit;
//  end;


  //--------------------------------------------------------------------------//
  // TS 각 PRC 를 건드리는지 점검
  if Calc_TS_Status(idx) = True then
  begin

    // TS_CUT 만 BestPrice 필요
    if m[idx].tsStatus = TS_CUT then
      m[idx].prcTSbest  := nowprc;

    Update_PosGrid(idx, POS_MD);

    Result := RET_CHANGE;

    exit;

  end;


  //--------------------------------------------------------------------------//
  // 수익실현 컷(청산) 점검

  if _ProfitCut(idx) = RET_CHANGE then
    exit;


  //--------------------------------------------------------------------------//
  // bestprice 조정 => TS_CUT 일때만

  nChangeCnt := 0;
  if m[idx].tsStatus = TS_CUT then
  BEGIN

    side    := PosSide(idx);

    if side = SIDE_BUY then
    begin
      if nowprc > m[idx].prcTSbest then
      begin
        m[idx].prcTSbest  := nowprc;
        Inc(nChangeCnt);
      end;
    end;

    if side = SIDE_SELL then
    begin
      if nowprc < m[idx].prcTSbest then
      begin
        m[idx].prcTSbest  := nowprc;
        Inc(nChangeCnt);
      end;

    end;

  END;

  if nChangeCnt > 0 then
    Result := RET_CHANGE;

  Update_PosGrid(idx, POS_MD);

end;


{
  TTS_STATUS     = (TS_NONE, TS_SLSHIFT, TS_CUT);

  - 한번 터치 하면 뒤로 가지 못한다.
}
function TTS.Calc_TS_Status(idx:integer):boolean;
var
  prevStatus  : TTS_STATUS;
  dTick       : double;
  base        : double;

  side        : string;
  nowprc      : double;
begin
  base := 0;
  prevStatus := m[idx].tsStatus;

  side    := PosSide(idx);
  nowprc  := PosNowPrc(idx);


  if side=SIDE_BUY then
  BEGIN

    if prevStatus = TS_NONE  then
    begin

      if nowprc >= m[idx].prcTSCut then
      begin
        m[idx].tsStatus := TS_CUT;
        base            := m[idx].prcTSCut;
        dTick           := __TSLevel_Tick(idx, TS_CUT);
      end
      else if nowprc >= m[idx].prcSLShift then
      begin
        m[idx].tsStatus := TS_SLSHIFT;
        base            := m[idx].prcSLShift;
        dTick           := __TSLevel_Tick(idx, TS_SLSHIFT);

        //SL SHIFT
        m[idx].prcSLCut := ShiftSLPrc(idx);
      end
//      else if nowprc >= m[idx].prcSkipClr then
//      begin
//        m[idx].tsStatus := TS_SKIPCLR;
//        base            := m[idx].prcSkipClr;
//        dTick           := __TSLevel_Tick(idx, TS_SKIPCLR);
//      end;
      ;
    end;


//    if prevStatus = TS_SKIPCLR  then
//    begin
//
//      if nowprc >= m[idx].prcTSCut then
//      begin
//        m[idx].tsStatus := TS_CUT;
//        base            := m[idx].prcTSCut;
//        dTick           := __TSLevel_Tick(idx, TS_CUT);
//      end
//      else if nowprc >= m[idx].prcSLShift then
//      begin
//        m[idx].tsStatus := TS_SLSHIFT;
//        base            := m[idx].prcSLShift;
//        dTick           := __TSLevel_Tick(idx, TS_SLSHIFT);
//
//        //SL SHIFT
//        m[idx].prcSLCut := ShiftSLPrc(idx);
//      end;
//      ;
//    end;

    if prevStatus = TS_SLSHIFT  then
    begin

      if nowprc >= m[idx].prcTSCut then
      begin
        m[idx].tsStatus := TS_CUT;
        base            := m[idx].prcTSCut;
        dTick           := __TSLevel_Tick(idx, TS_CUT);
      end ;

    end;

  END

 ELSE if side=SIDE_SELL then
 BEGIN

    if prevStatus = TS_NONE  then
    begin

      if nowprc <= m[idx].prcTSCut then
      begin
        m[idx].tsStatus := TS_CUT;
        base    := m[idx].prcTSCut;
        dTick   := __TSLevel_Tick(idx, TS_CUT);
      end
      else if nowprc <= m[idx].prcSLShift then
      begin
        m[idx].tsStatus := TS_SLSHIFT;
        base    := m[idx].prcSLShift;
        dTick   := __TSLevel_Tick(idx, TS_SLSHIFT);

        //SL SHIFT
        m[idx].prcSLCut := ShiftSLPrc(idx);

      end
//      else if nowprc <= m[idx].prcSkipClr then
//      begin
//        m[idx].tsStatus := TS_SKIPCLR;
//        base    := m[idx].prcSkipClr;
//        dTick   := __TSLevel_Tick(idx, TS_SKIPCLR);
//
//        //SL SHIFT
//        m[idx].prcSLCut := ShiftSLPrc(idx);
//
//      end
      ;
    end;

//    if prevStatus = TS_SKIPCLR  then
//    begin
//
//      if nowprc <= m[idx].prcTSCut then
//      begin
//        m[idx].tsStatus := TS_CUT;
//        base    := m[idx].prcTSCut;
//        dTick   := __TSLevel_Tick(idx, TS_CUT);
//      end
//      else if nowprc <= m[idx].prcSLShift then
//      begin
//        m[idx].tsStatus := TS_SLSHIFT;
//        base    := m[idx].prcSLShift;
//        dTick   := __TSLevel_Tick(idx, TS_SLSHIFT);
//
//        //SL SHIFT
//        m[idx].prcSLCut := ShiftSLPrc(idx);
//
//      end
//      ;
//    end;

    if prevStatus = TS_SLSHIFT  then
    begin

      if nowprc <= m[idx].prcTSCut then
      begin
        m[idx].tsStatus := TS_CUT;
        base    := m[idx].prcTSCut;
        dTick   := __TSLevel_Tick(idx, TS_CUT);
      end
      ;

    end;

  END;

  if base > 0 then
    fmMain.AddMsg(format('[%s 터치][Tick:%f] (%f) < (%f)',
                      [
                      __Status_Desc(m[idx].tsStatus), dTick, nowprc, base
                      ]
                      )
                );
  Result :=(prevStatus <> m[idx].tsStatus);

end;


// shift SL price to the profit direction
function TTS.ShiftSLPrc(idx:integer):double;
var
  dShiftTick  : double;
  side        : string;
begin

  side        := PosSide(idx);
  dShiftTick  := __TSLevel_Tick(idx, TS_SLSHIFT);

  Result := Calc_PrcAwayTick(side, m[idx].prcSLCut, dShiftTick, m[idx].ticksize, False)

end;


{
// 새로운 거래가 들어온 경우
  1. 진입이 들어온 경우
  2. 청산(일부/전부) 는 점검할 필요 없다.
}
function TTS._Main_TS_Pos(masterId, artc:string; clrTp:string; var idx:integer ):RET_TS;
//var
//  dNowPrc : double;
begin

  Result := RET_NON_CHANGE;

  idx := GetIdxForPos(masterId, artc);
  if idx<0 then
  begin
    exit;
  end;

  if m[idx].bBeingCut=true then
    exit;


  // 부분청산은 수량만 감소시키고
  // 전부청산이면 정보를 삭제한다.
  if (clrTp = CLR_TP_PARTIAL) then
  BEGIN
    //m[idx].qty := strtofloatdef(fmMain.gdPosMine.cells[POS_QTY,idx] ,0);

    Update_PosGrid(idx, POS_POS);

    exit;
  END;

  if (clrTp=CLR_TP_CLR)  then
  begin
    Clear_TS(idx);

    Update_PosGrid(idx, POS_POS);

    Result := RET_CHANGE;

    exit;

  end;


  //dNowPrc := strtofloatdef(__prcList.Prc(artc),0);

//  m[idx].side      := __SideTp(fmMain.gdPosMine.cells[POS_SIDE, idx]);
//  m[idx].avg       := strtofloatdef(fmMain.gdPosMine.cells[POS_AVG, idx],0);
//  m[idx].qty       := strtofloatdef(fmMain.gdPosMine.cells[POS_QTY, idx],0);


  // 포지션이 변경되면, 신규이거나 물타기 이거나 평단이 바뀌므로
  // 전부 새로 reset 해야 한다.
  m[idx].bBeingCut   := False;
  //m[idx].prcSkipClr  := 0;
  m[idx].prcSLShift  := 0;
  m[idx].prcTSCut    := 0;
  m[idx].prcSLCut    := 0;
  m[idx].prcTSbest   := 0;
  m[idx].tsStatus    := TS_NONE;

  //
  // 각 level price, sl price 재계산
  Calc_TSPrc_SLPrc(idx);
  //

  Result := RET_CHANGE;

  Update_PosGrid(idx, POS_POS);


  fmMain.AddMsg(format('진입(평단:%f)(TS_S/L Shift:%f)(TS Cut:%f)(SL:%f)',
                    [
                    PosAvg(idx), m[idx].prcSLShift, m[idx].prcTSCut, m[idx].prcSLCut
                    ]
                    ));

end;

procedure TTS.Update_Tick(artc:string; prc:string);
var
  data : ^TQDATA;
begin
  New(data);
  data.code := EVENT_TICK;
  data.artc := artc;
  data.prc  := prc;
  EnterCriticalSection(m_cs);
  m_listRcvData.Add(data);
  LeaveCriticalSection(m_cs);
end;

procedure TTS.Update_Pos(masterId, artc:string; clrTp:string);
var
  data : ^TQDATA;
begin
  New(data);
  data.code   := EVENT_POS;
  data.master := masterId;
  data.artc   := artc;
  data.clrTp  := clrTp;
  EnterCriticalSection(m_cs);
  m_listRcvData.Add(data);
  LeaveCriticalSection(m_cs);
end;


procedure TTS.Update_PosGrid(idx:integer;tp:UPDATE_POS_TP);
var
  ItemPos : TItemPos;
  ItemTSStatus : TItemTSStatus;
  dwRslt : dword;
begin

  if idx < 0 then
    exit;

  // 포지션 변화에 의한 변화
  if (tp=POS_FULL) OR (tp=POS_POS) then
  BEGIN
    ItemPos := TItemPos.Create;
    ItemPos.idx       := idx;
    ItemPos.masterId  := fmMain.m_setting[idx].masterID.Text;
    ItemPos.artc      := fmMain.m_setting[idx].artcCd.Text;
    ItemPos.tsSLShift := floattostr(__TSLevel_Tick(idx, TS_SLSHIFT));
    ItemPos.tsCutPrc  := floattostr(__TSLevel_Tick(idx, TS_CUT));
    ItemPos.slCutPrc  := floattostr(m[idx].prcSLCut);
    ItemPos.slTick    := '-'+floattostr(__SLTick(idx));

    SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_POSITION,
                              wParam(LongInt(sizeof(ItemPos))),
                              Lparam(LongInt(ItemPos)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );
  END;


  // 시세 변화에 의한 변화
  if (tp=POS_FULL) OR (tp=POS_MD) then
  BEGIN

    ItemTSStatus := TItemTSStatus.Create;

    ItemTSStatus.idx       := idx;
    ItemTSStatus.tsStatus  := IntToStr( Ord(m[idx].tsStatus));

    ItemTSStatus.tsBestPrc  := 0;
    ItemTSStatus.slPrc      := 0;
    ItemTSStatus.slTick     := 0;

    if ExistPosition(idx) then
    begin

      if m[idx].prcTSbest > 0 then
      begin
        if PosSide(idx) = SIDE_SELL then
          ItemTSStatus.tsBestPrc := (PosAvg(idx) - m[idx].prcTSbest) / m[idx].ticksize
        else
          ItemTSStatus.tsBestPrc := (m[idx].prcTSbest - PosAvg(idx)) / m[idx].ticksize;

        fmMain.edtDebug.Text := formatfloat('#0.#',ItemTSStatus.tsBestPrc);
      end;

      if m[idx].prcSLCut > 0 then
      begin
        ItemTSStatus.slPrc := m[idx].prcSLCut;

        if PosSide(idx)=SIDE_BUY then
          ItemTSStatus.slTick := (m[idx].prcSLCut - PosAvg(idx)) / m[idx].ticksize
        else
          ItemTSStatus.slTick := (PosAvg(idx) - m[idx].prcSLCut) / m[idx].ticksize;
      end;

    end;

    SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_TS_STATUS,
                              wParam(LongInt(sizeof(ItemTSStatus))),
                              Lparam(LongInt(ItemTSStatus)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );
  end;

end;


// MASTER 청산 받았을 때 따라서 청산할지 여부 판단하기 위해
function TTS.IsTS_Running(idx:integer):boolean;
var
  side : string;
  nowprc  : double;
  avg     : double;
begin

  Result := False;

  if __IsTSChecked(IDX)=False then
    exit;

  side := PosSide(idx);
  avg  := PosAvg(idx);
  nowprc  := PosNowPrc(idx);

  if m[idx].tsStatus = TS_NONE then
  begin
    exit;
  end
  else

  if side = SIDE_BUY then
  BEGIN
    Result := (avg < nowprc);
  END;

  if side = SIDE_SELL then
  BEGIN
    Result := (avg > nowprc);
  END;


end;




function TTS.Calc_PLTick(idx:integer; nowPrc:string):string;
var
  pl : double;
begin
  Result := '';

  if ExistPosition(idx)=False then
    exit;

  if m[idx].ticksize <=0 then
  BEGIN
    Showmessage('0 으로 나누기 오류');
    EXIT;
  END;

  pl := (strtofloatdef(nowPrc,0) - PosAvg(idx)) / m[idx].ticksize;

  if PosSide(idx) = SIDE_SELL then
    pl := pl * -1.0;

  Result := formatfloat('#0.#', pl);
end;




end.
