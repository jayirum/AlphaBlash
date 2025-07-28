unit uTrailingStop;

interface

uses
  System.Classes, system.SysUtils, winapi.windows
  , uTwoWayCommon
  ;


type

  RET_TS        = (RET_CHANGE, RET_NON_CHANGE);
  TS_STATUS     = (TS_NONE, TS_LVL_1, TS_LVL_2);
  UPDATE_POS_TP = (POS_MD, POS_POS, POS_FULL);

  TInfo = record
    master    : string;
    
    // 품목정보
    artc      : string;
    ticksize : double;
    dotcnt   : integer;

    // 현재가
    lastprc       : double;
    nowprc        : double;

    // 포지션 정보
    side: string;
    avg : double;
    qty : double;

    // 계산값
    bBeingCut           : boolean;
    prcLvl_1    : double;
    prcLvl_2    : double;
    prcLvl_3    : double;
    prcTSbest   : double;     // 현재까지 최고 가격(현재가변경시)
    prcSLcut    : double;
    tsStatus    : TS_STATUS;
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

    procedure Update_CurrPrc(artc:string; prc:string);                             // 시세스레드(tickthrd) 에서 시세 전달
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

    // TS Status desc
    function Status_Desc(status:TS_STATUS):string;


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
  uMain, uPrcList, uSettingTs, uOrdThrd, unotify
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


function TTS.Calc_PrcAwayTick(side:string; basePrc:double;                     // 첫익절 또는 NextBasePrc 계산
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
  Result := ((m[idx].avg>0) and (m[idx].qty>0) and (m[idx].side<>''));
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
begin

  Result := False;

  if ExistPosition(idx) = false then
    exit;

  if __IsTSChecked(idx) then
  begin
    Result := True;

    m[idx].prcLvl_1 := Calc_PrcAwayTick( m[idx].side,
                                            m[idx].avg,
                                            __Level_Tick(idx, 1),
                                            m[idx].ticksize
                                            );

    // level 2 먼저
    m[idx].prcLvl_2 := Calc_PrcAwayTick( m[idx].side,
                                            m[idx].avg,
                                            __Level_Tick(idx, 2),
                                            m[idx].ticksize
                                           );

    // level 3
    m[idx].prcLvl_3  := Calc_PrcAwayTick(m[idx].side,
                                            m[idx].avg,
                                            __Level_Tick(idx, 3),
                                            m[idx].ticksize
                                            );
  end;

  if __IsSLChecked(idx) then
  begin
    Result := True;

    m[idx].prcSLcut := Calc_PrcAwayTick(m[idx].side,
                                        m[idx].avg,
                                        __SLTick(idx),
                                        m[idx].ticksize,
                                        true)
  end;

end;

// main 에서 check box check 할때
procedure TTS.Init_TS(idx:integer);
var
  dNowPrc : double;
  artc    : string;
  bTSCheked,
  bSLCheked : boolean;
begin

  //--------------------------------------------------------------------------//
  artc := fmMain.m_setting[idx].artcCd.Text;

  // master id
  m[idx].master    := fmMain.m_setting[idx].master.Text;

  // 품목정보
  m[idx].artc      := artc;
  m[idx].ticksize  := __TickSize(m[idx].artc);
  m[idx].dotcnt    := __DotCnt(m[idx].artc);

  dNowPrc := strtofloatdef(__prcList.Prc(artc),0);

  bTSCheked := __IsTSChecked(idx);
  bSLCheked := __IsSLChecked(idx);

  if (bTSCheked=False) then
  begin
    m[idx].bBeingCut   := false;
    m[idx].prcLvl_1    := 0;
    m[idx].prcLvl_2    := 0;
    m[idx].prcLvl_3    := 0;
    m[idx].prcTSbest   := 0;
    m[idx].tsStatus    := TS_NONE;
  end;

  if (bSLCheked=False) then
  BEGIN
    m[idx].prcSLcut := 0;
  END;

  if (bTSCheked=False) and (bSLCheked=False) then
  begin
    m[idx].side := '';
    m[idx].avg  := 0;
    m[idx].qty  := 0;
    m[idx].lastprc := 0;
    m[idx].nowprc  := 0;

    Update_PosGrid (idx, POS_FULL);

    exit;
  end;



  // 포지션 정보
  m[idx].side      := __SideTp(fmMain.gdPosMine.cells[POS_SIDE, idx]);
  m[idx].avg       := strtofloatdef(fmMain.gdPosMine.cells[POS_AVG, idx],0);
  m[idx].qty       := strtofloatdef(fmMain.gdPosMine.cells[POS_QTY, idx],0);


  //--------------------------------------------------------------------------//
  // 포지션이 있으면 TSPrc 와 SLPrc 재계산
  if Calc_TSPrc_SLPrc(idx) = True Then
  begin

    if IsTouched_BasePrc(idx, m[idx].side, m[idx].prcLvl_1, dNowPrc)=true then
    begin
      fmMain.AddMsg('ERR! 첫익절(Level2) 터치. Level 2 틱을 다시 설정하세요.',true);
      exit;
    end;
  end;

  if dNowPrc>0 then
  begin
    // 현재가 정보
    m[idx].lastprc := m[idx].nowprc;
    m[idx].nowprc  := dNowPrc;

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
  m[idx].lastprc      := 0;
  m[idx].nowprc       := 0;

  // 포지션정보
  m[idx].side      := '';
  m[idx].avg       := 0;
  m[idx].qty       := 0;


  // 계산값
  m[idx].bBeingCut  := false;
  m[idx].prcLvl_1   := 0;
  m[idx].prcLvl_2   := 0;
  m[idx].prcLvl_3   := 0;
  m[idx].prcTSbest  := 0;
  m[idx].prcSLcut   := 0;
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
begin

  Result  := RET_NON_CHANGE;
  bClrOrd := False;

  if Ord(m[idx].tsStatus) < Ord(TS_LVL_2)  then
    exit;


  if m[idx].tsStatus = TS_LVL_2 then
  begin
    dTick     := __OffSet_Tick(idx, Ord(TS_LVL_2));
    dLevelPrc := m[idx].prcLvl_2;
  end
  else
  if m[idx].tsStatus = TS_LVL_3 then
  begin
    dTick     := __OffSet_Tick(idx, Ord(TS_LVL_3));
    dLevelPrc := m[idx].prcLvl_3;
  end
  else exit
      ;


  if m[idx].side = SIDE_BUY then
  BEGIN
    if (m[idx].prcTSbest > m[idx].nowprc) then  //and (m[idx].lastprc > m[idx].nowprc) then
    begin

      dCutPrc := m[idx].prcTSBest - ( dTick * m[idx].ticksize);

      if dCutPrc > m[idx].nowprc then
      begin
        bClrOrd := True;
        msg := format('[매수TS컷-Level:%d][Best:%f][Lvl:%f] (cutprc:%f)>(현재가:%f)(평단:%f)(PLTick:%f)',
                      [
                        Ord(m[idx].tsStatus), dLevelPrc,
                        m[idx].prcTSBest, dCutPrc,
                        m[idx].nowprc,
                        m[idx].avg, (m[idx].nowprc-m[idx].avg)/m[idx].ticksize
                      ]
                      );
      end;

    end;

  END;

  if m[idx].side = SIDE_SELL then
  BEGIN
    if (m[idx].prcTSbest < m[idx].nowprc) then  //and (m[idx].lastprc > m[idx].nowprc) then
    begin

      dCutPrc := m[idx].prcTSBest + ( dTick * m[idx].ticksize);

      if dCutPrc < m[idx].nowprc then
      begin
        bClrOrd := True;
        msg := format('[매도TS컷-Level:%d][Best:%f][Lvl:%f] (cutprc:%f)<(현재가:%f)(평단:%f)(PLTick:%f)',
                      [
                        Ord(m[idx].tsStatus), dLevelPrc,
                        m[idx].prcTSBest, dCutPrc,
                        m[idx].nowprc,
                        m[idx].avg, (m[idx].avg-m[idx].nowprc)/m[idx].ticksize
                      ]
                      );

      end;

    end;

  END;

  if bClrOrd then
  begin

    m[idx].bBeingCut := true;

    //주문처리
    __ordThrd[idx].Clear_TS_SL(m[idx].artc,idx,m[idx].side,'TS익절');

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
begin

  Result := RET_NON_CHANGE;

  // 청산 중
  if m[idx].bBeingCut=true then
    exit;

  // 포지션이 없으면 하지 않는다.
  if ExistPosition(idx)=False then
    exit;

  if __IsSLChecked(idx)=false then
    exit;


  // LONG   : SLPRC > NOWPRC
  // SHORT  : SLPRC < NOWPRC
  bCut := False;
  if m[idx].side = SIDE_BUY then
  BEGIN
    if (m[idx].prcSLcut > m[idx].nowprc) then
    begin
      msg := format('[손절-매수](%s)(Tick:%f)(SLPrc:%f)>(now:%f)(평단:%f)(PLTick:%f)',
                            [
                            m[idx].artc, __SLTick(idx),
                            m[idx].prcSLcut, m[idx].nowprc,
                            m[idx].avg, (m[idx].nowprc-m[idx].avg)/m[idx].ticksize
                            ]);
      bCut := True;
    end;
  END;

  if m[idx].side = SIDE_SELL then
  BEGIN
    if (m[idx].prcSLcut < m[idx].nowprc) then
    begin
      msg := format('[손절-매도](%s)(Tick:%f)(SLPrc:%f)<(now:%f)(평단:%f)(PLTick:%f)',
                            [
                            m[idx].artc, __SLTick(idx),
                            m[idx].prcSLcut, m[idx].nowprc,
                            m[idx].avg, (m[idx].avg-m[idx].nowprc)/m[idx].ticksize
                            ]
                            );
      bCut := True;
    end;
  END;

  if bCut then
  begin
    m[idx].bBeingCut := True;

    //주문처리
    __ordThrd[idx].Clear_TS_SL(m[idx].artc,idx,m[idx].side,'손절');
    fmMain.AddMsg(msg, B_SIREN);
    Clear_TS(idx);
  end;

end;



// 새로운 시세 들어온 경우 처리
function TTS._Main_TS_MarketData(artc:string; newprc:string; idx:integer):RET_TS;
var
  dNewprc : double;
  nChangeCnt : integer;
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


  dNewprc := strtofloatdef(newprc,0);

  // 가격변화가 없으면 하지 않는다.
  if __IsSamePrc(m[idx].nowprc, dNewprc, m[idx].dotcnt)=true then
  begin
    exit;
  end;

  nChangeCnt := 0;

  // lastprc, nowprc 갱신
  if m[idx].lastprc <> m[idx].nowprc then
    nChangeCnt := nChangeCnt + 1;

  m[idx].lastprc := m[idx].nowprc;
  m[idx].nowprc  := dNewPrc;

  //--------------------------------------------------------------------------//
  // TS 각 PRC 를 건드리는지 점검
  if Calc_TS_Status(idx) = True then
  begin
    m[idx].prcTSbest := m[idx].nowprc;

    Update_PosGrid(idx, POS_MD);

    Result := RET_CHANGE;

    exit;

  end;


  //--------------------------------------------------------------------------//
  // 수익실현 컷(청산) 점검

  if _ProfitCut(idx) = RET_CHANGE then
    exit;


  //--------------------------------------------------------------------------//
  // bestprice 조정

  if m[idx].side = SIDE_BUY then
  begin
    if m[idx].nowprc > m[idx].prcTSbest then
    begin
      m[idx].prcTSbest  := m[idx].nowprc;
      nChangeCnt := nChangeCnt + 1;
    end;
  end;

  if m[idx].side = SIDE_SELL then
  begin
    if m[idx].nowprc < m[idx].prcTSbest then
    begin
      m[idx].prcTSbest  := m[idx].nowprc;
      nChangeCnt := nChangeCnt + 1;
    end;

  end;


  if nChangeCnt > 0 then
    Result := RET_CHANGE;

  Update_PosGrid(idx, POS_MD);

end;


{
  TS_STATUS = (TS_NONE, TS_LVL_1, TS_LVL_2, TS_LVL_3);

  - 뒤로 가지 못한다.

  fmMain.AddMsg(format('[첫익절터치](익절:%f)(현재가:%f)', [m[idx].prcTSstart, m[idx].nowprc] ));
}
function TTS.Calc_TS_Status(idx:integer):boolean;
var
  prevStatus  : TS_STATUS;
  dTick       : double;
  base        : double;
begin
  base := 0;
  prevStatus := m[idx].tsStatus;

  if m[idx].side=SIDE_BUY then
  BEGIN

    if prevStatus = TS_NONE  then
    begin

      if m[idx].nowprc > m[idx].prcLvl_3 then
      begin
        m[idx].tsStatus := TS_LVL_3;
        base    := m[idx].prcLvl_3;
        dTick   := __Level_Tick(idx, 3);
      end
      else if m[idx].nowprc > m[idx].prcLvl_2 then
      begin
        m[idx].tsStatus := TS_LVL_2;
        base    := m[idx].prcLvl_2;
        dTick   := __Level_Tick(idx, 2);
      end
      else if m[idx].nowprc > m[idx].prcLvl_1 then
      begin
        m[idx].tsStatus := TS_LVL_1;
        base    := m[idx].prcLvl_1;
        dTick   := __Level_Tick(idx, 1);
      end
      ;
    end;

    if prevStatus = TS_LVL_1  then
    begin

      if m[idx].nowprc > m[idx].prcLvl_3 then
      begin
        m[idx].tsStatus := TS_LVL_3;
        base    := m[idx].prcLvl_3;
        dTick   := __Level_Tick(idx, 3);
      end
      else if m[idx].nowprc > m[idx].prcLvl_2 then
      begin
        m[idx].tsStatus := TS_LVL_2;
        base    := m[idx].prcLvl_2;
        dTick   := __Level_Tick(idx, 2);
      end
      ;

    end;

    if prevStatus = TS_LVL_2  then
    begin

      if m[idx].nowprc > m[idx].prcLvl_3 then
      begin
        m[idx].tsStatus := TS_LVL_3;
        base    := m[idx].prcLvl_3;
        dTick   := __Level_Tick(idx, 3);
      end
      ;
    end;

    if base > 0 then
      fmMain.AddMsg(format('[%s 터치][Tick:%f] (%f) > (%f)',
                        [
                        Status_Desc(m[idx].tsStatus), dTick,  m[idx].nowprc, base
                        ]
                        )
                  );
  END;


  if m[idx].side=SIDE_SELL then
 BEGIN

    if prevStatus = TS_NONE  then
    begin

      if m[idx].nowprc < m[idx].prcLvl_3 then
      begin
        m[idx].tsStatus := TS_LVL_3;
        base    := m[idx].prcLvl_3;
        dTick   := __Level_Tick(idx, 3);
      end
      else if m[idx].nowprc < m[idx].prcLvl_2 then
      begin
        m[idx].tsStatus := TS_LVL_2;
        base    := m[idx].prcLvl_2;
        dTick   := __Level_Tick(idx, 2);
      end
      else if m[idx].nowprc < m[idx].prcLvl_1 then
      begin
        m[idx].tsStatus := TS_LVL_1;
        base    := m[idx].prcLvl_1;
        dTick   := __Level_Tick(idx, 1);
      end
      ;
    end;

    if prevStatus = TS_LVL_1  then
    begin

      if m[idx].nowprc < m[idx].prcLvl_3 then
      begin
        m[idx].tsStatus := TS_LVL_3;
        base    := m[idx].prcLvl_3;
        dTick   := __Level_Tick(idx, 3);
      end
      else if m[idx].nowprc< m[idx].prcLvl_2 then
      begin
        m[idx].tsStatus := TS_LVL_2;
        base    := m[idx].prcLvl_2;
        dTick   := __Level_Tick(idx, 2);
      end
      ;

    end;

    if prevStatus = TS_LVL_2  then
    begin

      if m[idx].nowprc < m[idx].prcLvl_3 then
      begin
        m[idx].tsStatus := TS_LVL_3;
        base    := m[idx].prcLvl_3;
        dTick   := __Level_Tick(idx, 3);
      end
      ;
    end;

    if base > 0 then
      fmMain.AddMsg(format('[%s 터치][Tick:%f] (%f) < (%f)',
                        [
                        Status_Desc(m[idx].tsStatus), dTick, m[idx].nowprc, base
                        ]
                        )
                  );
  END;

  Result :=(prevStatus <> m[idx].tsStatus);

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
    m[idx].qty := strtofloatdef(fmMain.gdPosMine.cells[POS_QTY,idx] ,0);

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

  m[idx].side      := __SideTp(fmMain.gdPosMine.cells[POS_SIDE, idx]);
  m[idx].avg       := strtofloatdef(fmMain.gdPosMine.cells[POS_AVG, idx],0);
  m[idx].qty       := strtofloatdef(fmMain.gdPosMine.cells[POS_QTY, idx],0);


  // 포지션이 변경되면, 신규이거나 물타기 이거나 평단이 바뀌므로
  // 전부 새로 reset 해야 한다.
  m[idx].bBeingCut   := False;
  m[idx].prcLvl_1    := 0;
  m[idx].prcLvl_2    := 0;
  m[idx].prcLvl_3    := 0;
  m[idx].prcTSbest   := 0;
  m[idx].prcSLcut    := 0;
  m[idx].tsStatus    := TS_NONE;

  // 각 level price, sl price 재계산
  Calc_TSPrc_SLPrc(idx);

  Result := RET_CHANGE;

  Update_PosGrid(idx, POS_POS);


  fmMain.AddMsg(format('진입.TS계산(평단:%f)(Level1:%f)(Level2:%f)(Level3:%f)(SL:%f)',
                    [
                    m[idx].avg, m[idx].prcLvl_1, m[idx].prcLvl_2, m[idx].prcLvl_3, m[idx].prcSLcut
                    ]
                    ));

end;

procedure TTS.Update_CurrPrc(artc:string; prc:string);
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
begin

  if idx < 0 then
    exit;

  // 포지션 변화에 의한 변화
  if (tp=POS_FULL) OR (tp=POS_POS) then
  BEGIN
    fmMain.gdPosMine.Cells[POS_MASTER,   idx] := fmMain.m_setting[idx].masterID.Text;
    fmMain.gdPosMine.Cells[POS_ARTC,     idx] := fmMain.m_setting[idx].artcCd.Text;
    fmMain.gdPosMine.Cells[POS_TS_LVL_1, idx] := __PrcFmtD(m[idx].artc, m[idx].prcLvl_1);
    fmMain.gdPosMine.Cells[POS_TS_LVL_2, idx] := __PrcFmtD(m[idx].artc, m[idx].prcLvl_2);
    fmMain.gdPosMine.Cells[POS_TS_LVL_3, idx] := __PrcFmtD(m[idx].artc, m[idx].prcLvl_3);
    fmMain.gdPosMine.Cells[POS_SL_PRC, idx]   := __PrcFmtD(m[idx].artc, m[idx].prcSLcut);

  END;

  // 시세 변화에 의한 변화
  if (tp=POS_FULL) OR (tp=POS_MD) then
  BEGIN
    fmMain.gdPosMine.Cells[POS_TS_STATUS,idx] := IntToStr( Ord(m[idx].tsStatus) );
    fmMain.gdPosMine.Cells[POS_TS_BEST, idx] := __PrcFmtD(m[idx].artc, m[idx].prcTSbest);

//    if m[idx].prcTSbest>0 then
//    begin
//      prc := Calc_CutPrc(m[idx].side, m[idx].prcTSbest, __OffsetA(idx), m[idx].ticksize);
//      fmMain.gdPosMine.Cells[POS_TS_CUTPRC1, idx] := __PrcFmtD(m[idx].artc, prc);
//
//      prc := Calc_CutPrc(m[idx].side, m[idx].prcTSbest, __OffsetB(idx), m[idx].ticksize);
//      fmMain.gdPosMine.Cells[POS_TS_CUTPRC2, idx] := __PrcFmtD(m[idx].artc, prc);
//    end;
  end;

end;

//procedure TTS.Repaint_PosGrid();
//var
//  d : double;
//begin
//
////  if m_chgIdx < 0 then
////    exit;
////
////  fmMain.gdPosMine.Cells[POS_TS_START, m_chgIdx] := __PrcFmtD(m[m_chgIdx].artc, m[m_chgIdx].prcTSstart);
////
////  fmMain.gdPosMine.Cells[POS_TS_START_TOUCHED,m_chgIdx] := IntToStr(m[m_chgIdx].mktStatus);
////
////  fmMain.gdPosMine.Cells[POS_TS_BEST, m_chgIdx] := __PrcFmtD(m[m_chgIdx].artc, m[m_chgIdx].prcTSbest);
////  fmMain.gdPosMine.Cells[POS_TS_RANGE1, m_chgIdx] := __PrcFmtD(m[m_chgIdx].artc, m[m_chgIdx].prcTSrange);
////  fmMain.gdPosMine.Cells[POS_SL_PRC, m_chgIdx]    := __PrcFmtD(m[m_chgIdx].artc, m[m_chgIdx].prcSLcut);
////
////  d := Calc_CutPrc(m[idx)
////
////  fmMain.gdPosMine.Cells[POS_TS_CUTPRC1, m_chgIdx] := __PrcFmtD(m[m_chgIdx].artc, m[m_chgIdx].prcTScutA);
////  fmMain.gdPosMine.Cells[POS_TS_CUTPRC2, m_chgIdx] := __PrcFmtD(m[m_chgIdx].artc, m[m_chgIdx].prcTScutB);
////
////  m_chgIdx := -1;
//
//end;



function TTS.IsTS_Running(idx:integer):boolean;
begin

  Result := False;

  if __IsTSChecked(IDX)=False then
    exit;

  if m[idx].tsStatus = TS_NONE then
  begin
    exit;
  end
  else
  if m[idx].tsStatus = TS_LVL_1 then
  BEGIN
    if m[idx].side = SIDE_BUY then
    BEGIN
      Result := (m[idx].avg < m[idx].nowprc);
    END;

    if m[idx].side = SIDE_SELL then
    BEGIN
      Result := (m[idx].avg > m[idx].nowprc);
    END;
  END
  else
  begin
    Result := True;
  end;



end;


function TTS.Status_Desc(status:TS_STATUS):string;
BEGIN
    Result := inttostr(
                    Ord(status)
             );

END;


function TTS.Calc_PLTick(idx:integer; nowPrc:string):string;
var
  pl : double;
begin
  Result := '';

  if ExistPosition(idx)=False then
    exit;

  pl := (strtofloatdef(nowPrc,0) - m[idx].avg) / m[idx].ticksize;

  if m[idx].side = SIDE_SELL then
    pl := pl * -1.0;

  Result := formatfloat('#0.#', pl);
end;

end.
