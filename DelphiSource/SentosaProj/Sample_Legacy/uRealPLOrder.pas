unit uRealPLOrder;

interface

uses
  System.Classes, uAlphaProtocol, windows, system.SysUtils
  ,ProtoGetU, ProtoSetU, uTwoWayCommon, winapi.messages, vcl.forms
  ;

type

  // receive login packet and return to mt4
  TRealPLOrder = class(TThread)
  protected
    procedure Execute();override;
    procedure _MainProc();

    function  Proc_OnePos(iSide:integer; dPLPip, dBasePip:double):boolean;
    function  Proc_TwoPos(dPLPipBuy, dPLPipSell, dBasePip:double):boolean;
    function  Cut_ProfitCut( iSide:integer; dPLPip:double):boolean;
    procedure Update_TSStatus( iSide:integer; dPLPip:double);
    procedure Order_Open(iSide:integer);
    procedure Order_Close(iPosSide:integer);
    procedure Update_BestPip(iSide:integer);

  private
    function Is_OverBasePip(bProfit:boolean; dPLPip:double; dBasePip:double):boolean;
    //function Calc_CutPrc(currStatus : TPL_STATUS; iSide:integer; dBestPrc:double):double;
    function Calc_CutPip(currStatus : TPL_STATUS; iSide:integer; dBestPip:double):double;
    procedure Mark_Sending(iSymbol, iSide:integer;bBoth:boolean);
  protected
    m_ThreadId  : cardinal;
    m_protoSet  : TProtoSet;

  public
    m_iSymbol   : integer;
  public
    constructor Create();
    destructor Destroy();override;
    function ThreadId():cardinal;

  end;


var
  __realPLOrd : array [1..MAX_SYMBOL] of TRealPLOrder;

  procedure __CreatRealPLOrder(iSymbol:integer);
  procedure __DeployMD(iSymbol:integer);

implementation

uses
  uQueueEx, fmMainU, uCtrls, CommonUtils, uPostThread
  ;



{
  iSymbol 은 1부터
  array 는 0 부터
}
procedure __CreatRealPLOrder(iSymbol:integer);
begin

  BEGIN
    if Assigned(__realPLOrd[iSymbol]) then
    begin
      if __realPLOrd[iSymbol].m_iSymbol = iSymbol then
        exit
      else
        __realPLOrd[iSymbol].m_iSymbol := iSymbol;
    end
    else
    begin
      __realPLOrd[iSymbol] := TRealPLOrder.Create;
      __realPLOrd[iSymbol].m_iSymbol := iSymbol;
    end;
  END;

end;

procedure __DeployMD(iSymbol:integer);
var
  i : integer;
begin
  for i:=1 TO MAX_SYMBOL DO
  BEGIN
    if Assigned(__realPLOrd[i]) then
      PostThreadMessage(__realPLOrd[i].ThreadId(), WM_MD, 0, 0);

  END;
end;

constructor TRealPLOrder.Create;
begin
  m_protoSet  := TProtoSet.create;
  inherited;
end;

destructor TRealPLOrder.Destroy();
begin
  m_protoSet.Destroy;
  inherited;
end;

function TRealPLOrder.ThreadId():cardinal;
begin
  Result := m_ThreadId;
end;




procedure TRealPLOrder.Execute();
var
  M : Msg;
begin

  m_ThreadId := GetCurrentThreadId();

  while (not terminated) do //and (m_bcontinue) do
  begin

    Sleep(10);

    while PeekMessage(M, 0, 0, 0, PM_REMOVE) do
    begin
      if M.Message = WM_QUIT then Break;

      // if 포지션 ==> 딱히 할게 없다.

      // if 시세
      if M.Message = WM_MD then
      begin

        _MainProc();

      end;
    end;

  end;

end;



procedure TRealPLOrder._MainProc();
var
  dPlPipSell
  ,dPlPipBuy
  ,dBasePip  : double;
  bLong, bShort : boolean;
  nPosCnt       : integer;
  //bBeingCutBuy,
  //bBeingCutSell : boolean;
  bCut : boolean;
begin


  dPlPipSell  := 0;
  dPlPipBuy   := 0;
  nPosCnt     := 0;
  bLong       := False;
  bShort      := False;

  dBasePip := __BaseSpreadPip(m_iSymbol);

  //--------------------------------------------------------------------------//
  if (__gdExistPosition(m_iSymbol, IDX_BUY)=True) And
     (__gdIsSending(m_iSymbol, IDX_BUY)=False)   then
  BEGIN
    nPosCnt := nPosCnt+1;
    bLong   := True;
    dPlPipBuy  := __gdCurrPLPip(m_iSymbol, IDX_BUY);
  END;

  if (__gdExistPosition(m_iSymbol, IDX_SELL)=True) and
     (__gdIsSending(m_iSymbol, IDX_SELL)=False)   then
  BEGIN
    nPosCnt := nPosCnt+1;
    bShort  := True;
    dPlPipSell  := __gdCurrPLPip(m_iSymbol, IDX_SELL);
  END;

  if nPosCnt=0 then
    exit;

  if nPosCnt=1 then
  BEGIN
    if bLong=True then
      bCut := Proc_OnePos(IDX_BUY, dPlPipBuy, dBasePip)
    else
      bCut := Proc_OnePos(IDX_SELL, dPlPipSell, dBasePip)
    ;

    if bCut then
      exit;
  END;

  if nPosCnt=2 then
  begin
    if Proc_TwoPos(dPlPipBuy, dPlPipSell, dBasePip )=true then
      exit;
  end;

  if bLong then
  begin
    if __gdIsSending(m_iSymbol, IDX_BUY) = false then
    begin
      Update_TSStatus(IDX_BUY, dPlPipBuy);
      Update_BestPip(IDX_BUY);
    end;
  end;

  if bShort then
  BEGIN
    if __gdIsSending(m_iSymbol, IDX_SELL) = false then
    begin
      Update_TSStatus(IDX_SELL, dPlPipSell);
      Update_BestPip(IDX_SELL);
    end;
  END;


end;


procedure TRealPLOrder.Update_BestPip(iSide:integer);
var
  //dNowPrc       : double;
  dNowPip       : double;
  tsStatus      : TPL_STATUS;
  dCurrBestPip  : double;
  dNewBestPip   : double;
  dCutPip       : double;
  //msg         : string;
  itemBest      : TItemBestPrc;
  dwRslt        : DWORD;
BEGIN
  tsStatus := __gdPLStatusTp(m_iSymbol, iSide);
  if Ord(tsStatus) < Ord(TS_1) then
    exit;

  //dNowPrc   := __gdNowPrc(m_iSymbol, iSide);
  dNowPip   := __gdCurrPLPip(m_iSymbol, iSide);

  dCurrBestPip := __gdTS_BestPip(m_iSymbol, iSide);
  dNewBestPip  := 0;
  dCutPip      := 0;


  if dNowPip > dCurrBestPip then
    dNewBestPip := dNowPip;

  if dCurrBestPip = 0 then
    dNewBestPip := dNowPip;

  if dNewBestPip > 0 then
  begin
    itemBest := TItemBestPrc.Create;
    itemBest.iSymbol  := m_iSymbol;
    itemBest.iSide    := iSide;
    itemBest.sBestPip := __FmtPip( dNewBestPip );

    dCutPip  := Calc_CutPip(tsStatus, iSide, dNewBestPip);
    itemBest.sCutPip  := __FmtPip(dCutPip);


    SendMessageTimeOut(Application.MainForm.Handle,
                                WM_GRID_BEST_PRC,
                                wParam(LongInt(sizeof(itemBest))),
                                Lparam(LongInt(itemBest)),
                                SMTO_ABORTIFHUNG,
                                TIMEOUT_SENDMSG * 2 * 3,
                                dwRslt
                                );
  end;

END;

{
  포지션이 하나만 있는 경우.
  1) 해당 포지션 손실 중 => 반대방향 진입 결정
  2) 해당 포지션 이익 중 => TS 적용
}
function TRealPLOrder.Proc_OnePos(iSide:integer; dPLPip, dBasePip:double):boolean;
const
  IS_PROFIT = True;
begin

  Result := false;

  if __gdIsSending(m_iSymbol, iSide) = true then
    exit;

  // loss
  if dPLPip < 0 then
  begin

    if Is_OverBasePip ( not IS_PROFIT, dPLPip, dBasePip ) = True then
    begin

      Mark_Sending(m_iSymbol, iSide, True);

      fmMain.AddMsg(format('[OnePos.Base이상손실.반대방향진입](%s)(%s)(%s)(손실Pip:%f)(BasePip:%f)',
                            [
                              __Symbol(m_iSymbol),
                              __Key(m_iSymbol, iSide),
                              __SideDesc(iSide),
                              dPLPip,
                              dBasePip
                            ]
                            ));
      if iSide=IDX_BUY then
        Order_Open(IDX_SELL)
      else
        Order_Open(IDX_BUY)
      ;

      Result := true;
    end;

  end;

  // profit
  if dPLPip > 0 then
  begin
    Result := Cut_ProfitCut(iSide, dPLPip);
  end;

end;

{
  포지션이 두개인 경우.
  1) 손실 포지션이 base 이상이면 반대방향 청산
}
function TRealPLOrder.Proc_TwoPos(dPLPipBuy, dPLPipSell, dBasePip:double):boolean;
const
  IS_PROFIT = True;
begin

  Result := false;

  if __gdIsSending(m_iSymbol, IDX_BUY) = false then
  BEGIN
    if Is_OverBasePip(NOT IS_PROFIT, dPLPipBuy, dBasePip*2)=True  then
    begin
      fmMain.AddMsg(format('[TwoPos.Base이상손실.매수잔고청산](%s)(%s)(손실Pip:%f)(BasePip:%f)',
                              [
                                __Symbol(m_iSymbol),
                                __Key(m_iSymbol, IDX_BUY),
                                dPLPipBuy,
                                dBasePip
                              ]
                              ));
      Order_Close(IDX_BUY);
      Result := true;
      exit;
    end;
  END;


  if __gdIsSending(m_iSymbol, IDX_SELL) = false then
  BEGIN
    if Is_OverBasePip(NOT IS_PROFIT, dPLPipSell, dBasePip*2)=True  then
    begin
      fmMain.AddMsg(format('[TwoPos.Base이상손실.매도잔고청산](%s)(%s)(손실Pip:%f)(BasePip:%f)',
                              [
                                __Symbol(m_iSymbol),
                                __Key(m_iSymbol, IDX_SELL),
                                dPLPipSell,
                                dBasePip
                              ]
                              ));
      Order_Close(IDX_SELL);
      Result := true;
      exit;
    end;
  END;


end;


function TRealPLOrder.Calc_CutPip(currStatus : TPL_STATUS; iSide:integer; dBestPip:double):double;
var
  dOffSet : double;
begin

  Result := 0;

  if Ord(currStatus) < Ord(TS_1) then
    exit;


  // cutprice 는 best 에서 pip 만큼 나쁜 가격임.
  dOffSet := 0;
  if currStatus = TS_1 then
    dOffSet := __TS_OffSetPip(m_iSymbol, 1)
  else if currStatus = TS_2 then
    dOffSet := __TS_OffSetPip(m_iSymbol, 2)
  ;

  Result := dBestPip - dOffSet;
end;

{
  포지션은 1,
  TS1 또는 TS2 상태에서
  최고가 대비 OFFSET 만큼 떨어진 경우 수익컷
// return True : 변화가 있다.  
}
function TRealPLOrder.Cut_ProfitCut( iSide:integer; dPLPip:double):boolean;
var
  currStatus  : TPL_STATUS;
  //dOffSet     : double;

  dNowPip   : double;
  dBestPip  : double;
  dCutPip   : double;

  msg       : string;

begin

  Result := False;
  
  if dPLPip <= 0  then
    exit;

  currStatus := __gdPLStatusTp( m_iSymbol, iSide );
  if Ord(currStatus) < Ord(TS_1) then
    exit;

  //--------------------------------------------------------------------------//
  // 현재가가 best 보다 나쁠 때 수익컷을 점검하는 것이다.
  dNowPip   := __gdCurrPLPip(m_iSymbol, iSide);
  dBestPip  := __gdTS_BestPip(m_iSymbol, iSide);

  if dBestPip <=0 then
    exit;

  if  dNowPip >= dBestPip then
    exit;


  //--------------------------------------------------------------------------//
  // cutprice 는 best 에서 pip 만큼 나쁜 가격임.
  dCutPip   := Calc_CutPip(currStatus, iSide, dBestPip);

  // 현재가격이 cutprice 보다 나쁘면 청산
  if dNowPip < dCutPip then
  begin
    if (iSide=IDX_BUY) then
    begin
      msg := format('Long(NowPip:%.2f) <= (CutPip:%.2f)(BestPip:%.2f)', [dNowPip, dCutPip, dBestPip]);
      Result := True;
      Order_Close(IDX_BUY);
    end;

    if (iSide=IDX_SELL) then
    begin
      msg := format('Short(NowPip:%.2f) <= (CutPip:%.2f)(BestPip:%.2f)', [dNowPip, dCutPip, dBestPip]);
      Result := True;
      Order_Close(IDX_SELL);
    end;
  end;

  if Result then
  begin
    fmMain.AddMsg(format('[TS익절](%s)(%s)(%s)(%s)',
                            [
                              __Symbol(m_iSymbol),
                              __Key(m_iSymbol, iSide),
                              __SideDesc(iSide),
                              msg
                            ]
                            ));
  end;

end;



// 평가이익인 상황에서 TS Status 체크 & Update
procedure TRealPLOrder.Update_TSStatus( iSide:integer; dPLPip:double);
var
  currStatus  : TPL_STATUS;
  newStatus   : TPL_STATUS;
  dCurrPLPip  : double;
  msg         : string;

  itemTsStatus  : TItemTsStatus;
  dwRslt        : DWORD;

begin

  if dPLPip <=0 then
    exit;

  currStatus  := __gdPLStatusTP(m_iSymbol, iSide );
  newStatus   := currStatus;
  dCurrPLPip  := __gdCurrPLPip(m_iSymbol, iSide);

  // NONE, PL_P1 => TS1 OR TS2
  if Ord(currStatus) < Ord(TS_1) then
  BEGIN

    if dCurrPLPip >= __TS_LvlPip(m_iSymbol, 2) then
    begin
      newStatus := TS_2;
      msg := format('[TS Lvl-2 Touch from P1](CurrPip:%f)>=(Lvl2:%f)[%s]',
                    [dCurrPLPip, __TS_LvlPip(m_iSymbol, 2), __SideDesc(iSide)]);
    end

    else if dCurrPLPip >= __TS_LvlPip(m_iSymbol, 1) then
    begin
      newStatus := TS_1;
      msg := format('[TS Lvl-1 Touch from P1](CurrPip:%f)>=(Lvl2:%f)[%s]',
                    [dCurrPLPip, __TS_LvlPip(m_iSymbol, 1), __SideDesc(iSide)]);
    end;

  END

  else if Ord(currStatus) < Ord(TS_2) then
  BEGIN
    if dCurrPLPip >= __TS_LvlPip(m_iSymbol, 2) then
    begin
      newStatus := TS_2;
      msg := format('[TS Lvl-2 Touch from TS1](CurrPip:%f)>=(Lvl2:%f)[%s]',
                    [dCurrPLPip, __TS_LvlPip(m_iSymbol, 2), __SideDesc(iSide)]);
    end;
  END;

  if currStatus <> newStatus then
  begin
    itemTsStatus := TItemTsStatus.create;
    itemTsStatus.iSymbol  := m_iSymbol;
    itemTsStatus.iSide    := iSide;
    itemTsStatus.sStatus  := __PLStatusDesc(newStatus);

    SendMessageTimeOut(Application.MainForm.Handle,
                            WM_GRID_TSSTATUS,
                            wParam(LongInt(sizeof(itemTsStatus))),
                            Lparam(LongInt(itemTsStatus)),
                            SMTO_ABORTIFHUNG,
                            TIMEOUT_SENDMSG,
                            dwRslt
                            );
  end;

  if length(msg) > 0 then
    fmMain.AddMsg(msg);

end;




procedure TRealPLOrder.Mark_Sending(iSymbol, iSide:integer; bBoth:boolean);
var
  itemSending : TitemSending;
  dwRslt      : DWORD;
begin
  itemSending := TitemSending.create;
  itemSending.iSymbol := iSymbol;
  itemSending.iSide   := iSide;
  itemSending.bBoth   := bBoth;

  SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_Sending,
                              wParam(LongInt(sizeof(itemSending))),
                              Lparam(LongInt(itemSending)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG * 2 * 3,
                              dwRslt
                              );

end;


{
  주문에 필요한 데이터
  종목
  side
  lot
  진입/청산
}
procedure TRealPLOrder.Order_Open(iSide:integer);
VAR
  outstr : string;
begin

  m_protoSet.Start;
  m_protoSet.SetVal(FDS_CODE,     CODE_PUBLISH_ORDER);
  m_protoSet.SetVal(FDS_SYMBOL,   __Symbol(m_iSymbol));
  m_protoSet.SetVal(FDN_SIDE_IDX, iSide);
  m_protoSet.SetVal(FDD_LOTS,     __OpenLots(m_iSymbol));
  m_protoSet.SetVal(FDS_CLR_TP,   CLRTP_OPEN);

  m_protoSet.Complete(outstr);

  __QueueEx[Q_SEND].Add(CODE_PUBLISH_ORDER, __Key(m_iSymbol, iSide), outstr);

  fmMain.AddMsg(format('[Order_Open - Insert Q](Key:%s)(%s)',[__Key(m_iSymbol, iSide), outstr]));
end;

procedure TRealPLOrder.Order_Close(iPosSide:integer);
VAR

  outstr : string;
begin
  Mark_Sending(m_iSymbol, iPosSide, false);

  m_protoSet.Start;
  m_protoSet.SetVal(FDS_CODE,       CODE_PUBLISH_ORDER);
  m_protoSet.SetVal(FDS_SYMBOL,     __Symbol(m_iSymbol));
  m_protoSet.SetVal(FDN_SIDE_IDX,   iPosSide);
  m_protoSet.SetVal(FDD_LOTS,       __gdLots(m_iSymbol, iPosSide));
  m_protoSet.SetVal(FDS_CLR_TP,     CLRTP_CLOSE);
  m_protoSet.SetVal(FDS_MT4_TICKET, __gdTicket(m_iSymbol, iPosSide));

  m_protoSet.Complete(outstr);

  __QueueEx[Q_SEND].Add(CODE_PUBLISH_ORDER, __Key(m_iSymbol, iPosSide), outstr);

  fmMain.AddMsg(format('[Order_Close - Insert Q](Key:%s)(%s)',[__Key(m_iSymbol, iPosSide), outstr]));

end;



function TRealPLOrder.Is_OverBasePip(bProfit:boolean; dPLPip:double; dBasePip:double):boolean;
var
  signed : double;
begin

  signed := 1;
  if bProfit=False then
    signed := -1;

  Result := ( (dPLPip*signed) >= dBasePip );
end;



end.

