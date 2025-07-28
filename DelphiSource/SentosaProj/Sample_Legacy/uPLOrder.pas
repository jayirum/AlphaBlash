unit uPLOrder;

interface

uses
  System.Classes, uAlphaProtocol, windows, system.SysUtils
  ,ProtoGetU, ProtoSetU, uTwoWayCommon
  ;

type

  // receive login packet and return to mt4
  TPLOrder = class(TThread)
  protected
    procedure Execute();override;
    procedure _MainProc();

    function  Cut_Losscut_OneSide(iSide:integer; dPLPip, dBaseSpread:double):boolean;
    function  Cut_Losscut_PLSum(nPosCnt:integer; dPLPipBuy, dPLPipSell, dBaseSpread:double):boolean;
    function  Cut_ProfitCut( iSide:integer; dPLPip:double):boolean;

    procedure Calc_LossStatus(iSide:integer; dPLPip, dBaseSpread:double);
    procedure Calc_TSStatus( iSide:integer; dPLPip:double);


  protected
    m_ThreadId  : cardinal;
    m_protoSet  : TProtoSet;

  public
    m_iSymbol   : integer;
  public
    constructor Creator();
    destructor Destroy();override;
    function ThreadId():cardinal;

  end;


var
  __plOrd : array [1..MAX_SYMBOL] of TPLOrder;

  procedure __CreatPLOrder(iSymbol:integer);

implementation

uses
  uQueue, fmMainU, uCtrls, CommonUtils
  ;


{
  iSymbol �� 1����
  array �� 0 ����
}
procedure __CreatPLOrder(iSymbol:integer);
var
  i : integer;
begin
  for i := 1 to MAX_SYMBOL do
  BEGIN
    if Assigned(__plOrd[i]) then
    begin
      if __plOrd[i].m_iSymbol = iSymbol then
        exit
      else
        __plOrd[i].m_iSymbol := iSymbol;
    end
    else
    begin
      __plOrd[i] := TPLOrder.Creator;
      __plOrd[i].m_iSymbol := iSymbol;
    end;
  END;



end;

constructor TPLOrder.Creator;
begin
  m_protoSet  := TProtoSet.create;
end;

destructor TPLOrder.Destroy();
begin
  m_protoSet.Destroy;
  inherited;
end;

function TPLOrder.ThreadId():cardinal;
begin
  Result := m_ThreadId;
end;




procedure TPLOrder.Execute();
begin

  m_ThreadId := GetCurrentThreadId();

  while (not terminated) do //and (m_bcontinue) do
  begin


    //TODO
    // PeekMessage
    // �ü�, ������ ���� ����Ǹ� �޼��� �޴´�.

    // if ������ ==> ���� �Ұ� ����.

    // if �ü�
    _MainProc();

  end;

end;



procedure TPLOrder._MainProc();
var
  dPlPipSell
  ,dPlPipBuy
  ,dBaseSpread  : double;
  nPosCnt       : integer;
  nChangeCnt    : integer;
begin

  dPlPipSell :=0; dPlPipBuy:=0;  nPosCnt:=0;

  dBaseSpread := __BaseSpreadPip(m_iSymbol);

  //--------------------------------------------------------------------------//
  // LossCut ����

  if (__ExistPosition(m_iSymbol, IDX_BUY)=True) And
     (__IsBeingCut(m_iSymbol, IDX_BUY)=False)   then
  BEGIN
    nPosCnt := nPosCnt + 1;
    dPlPipBuy  := __PLPip(m_iSymbol, IDX_BUY);
  END;

  if (__ExistPosition(m_iSymbol, IDX_SELL)=True) and
     (__IsBeingCut(m_iSymbol, IDX_SELL)=False)   then
  BEGIN
    nPosCnt := nPosCnt + 1;
    dPlPipSell  := __PLPip(m_iSymbol, IDX_SELL);
  END;

  if nPosCnt=0 then
    exit;

  nChangeCnt := 0;
  
  // ���� ���� �� ����
  if Cut_Losscut_OneSide(IDX_BUY, dPlPipBuy, dBaseSpread)=True then
    nChangeCnt := nChangeCnt + 1;
  if Cut_Losscut_OneSide(IDX_SELL, dPlPipSell, dBaseSpread)=True then
    nChangeCnt := nChangeCnt + 1;
    
  if Cut_Losscut_PLSum(nPosCnt, dPlPipBuy, dPlPipSell, dBaseSpread)=True then
    nChangeCnt := nChangeCnt + 1;

  if nChangeCnt > 0 then
    exit;

    
  // ���� ���� �� ����
  if Cut_ProfitCut(IDX_BUY, dPLPipBuy)=True then
    nChangeCnt := nChangeCnt + 1;
    
  if Cut_ProfitCut(IDX_SELL, dPLPipSell)=True then
    nChangeCnt := nChangeCnt + 1;

  // PL STATUS ��� �� Update
  Calc_LossStatus(IDX_BUY, dPLPipBuy, dBaseSpread);
  Calc_LossStatus(IDX_SELL, dPlPipSell, dBaseSpread);

  // TS update
  Calc_TSStatus(IDX_BUY, dPLPipBuy);
  Calc_TSStatus(IDX_SELL, dPlPipSell);


end;


// return : ��ȭ�� �ִ�.
function TPLOrder.Cut_Losscut_OneSide(iSide:integer; dPLPip, dBaseSpread:double):boolean;
begin

  Result := False;

  // �ս��� BaseSpread*2 �̻��̴�. (���밪)
  if (dPLPip < 0) and ((dPLPip*-1) >= dBaseSpread*2) then
  begin
    Result := True;
    
    //TODO. ����
    //exit;
  end;

end;


// return True : ��ȭ�� �ִ�.
function TPLOrder.Cut_Losscut_PLSum(nPosCnt:integer; dPLPipBuy, dPLPipSell, dBaseSpread:double):boolean;
var
  dPLSum : double;
begin

  Result := False;
  
  //--------------------------------------------------------------------------//
  // �ս� ���� base �� �Ѿ���� ����

  dPlSum := dPlPipBuy + dPlPipSell;

  if (dPlSum < 0) and ((dPlSum*-1) >= dBaseSpread)  then
  begin
    //CASE-1. �������� ���� ��� �ս� ������ ����
    if nPosCnt=2 then
    begin
      Result := True;
      // �ս������� ����
      exit;
    end;

    //CASE-2. �������� �ϳ��� ��� �ݴ������� ����
    if nPosCnt=1 then
    begin
      Result := True;
      exit;
    end;
  end;
end;



{
  TS1 �Ǵ� TS2 �ǵ� ���¿���
  �ְ� ��� OFFSET ��ŭ ������ ��� ������
// return True : ��ȭ�� �ִ�.  
}
function TPLOrder.Cut_ProfitCut( iSide:integer; dPLPip:double):boolean;
var
  currStatus  : TPL_STATUS;
  dOffSet     : double;
  dCutPip     : double;

  dNowPrc   : double;
  dBestPrc  : double;
  dCutPrc   : double;

begin

  Result := False;
  
  if dPLPip <= 0  then
    exit;

  currStatus := __PLStatusTp( m_iSymbol, iSide );
  if Ord(currStatus) < Ord(TS_1) then
    exit;

  if currStatus = TS_1 then
    dOffSet := __TS_OffSetPip(m_iSymbol, 1)
  else if currStatus = TS_2 then
    dOffSet := __TS_OffSetPip(m_iSymbol, 2)
  ;

  dNowPrc   := __NowPrc(m_iSymbol, iSide);
  dBestPrc  := __TS_BestPrc(m_iSymbol, iSide);

  // ���簡�� best ���� ���� �� �������� �����ϴ� ���̴�.
  if (iSide=IDX_BUY) AND ( dNowPrc >= dBestPrc) then
    exit;

  if (iSide=IDX_SELL) AND ( dNowPrc <= dBestPrc) then
    exit;
  
  // cutprice �� best ���� pip ��ŭ ���� ������.
  if currStatus = TS_1 then
    dCutPrc   := __AwayPrc(m_iSymbol, iSide, dBestPrc, dOffSet, False)
  else if currStatus = TS_2 then
    dCutPrc   := __AwayPrc(m_iSymbol, iSide, dBestPrc, dOffSet, False)
  else
  begin
    fmMain.AddMsg('Cut_ProfitCut err-1', false, true);
    exit;
  end
  ;

  // ���簡���� cutprice ���� ���ڸ� û��
  if (iSide=IDX_BUY) AND ( dNowPrc <= dCutPrc) then
  begin
    Result := True;
    //TODO. ����
  end;

  if (iSide=IDX_SELL) AND ( dNowPrc >= dCutPrc) then
  begin            
    Result := True;
    //TODO. ����
  end;



end;



// �������� ��Ȳ���� TS Status üũ & Update
procedure TPLOrder.Calc_TSStatus( iSide:integer; dPLPip:double);
var
  dNowPrc     : double;
  currStatus  : TPL_STATUS;
  newStatus   : TPL_STATUS;
  dCurrPLPip  : double;
  dLvlPip     : double;
  msg         : string;
begin

  if dPLPip <=0 then
    exit;

  currStatus  := __PLStatusTP( __PLStatus(m_iSymbol, iSide) );
  newStatus   := currStatus;
  dCurrPLPip  := __PLPip(m_iSymbol, iSide);

  // NONE, PL_P1 => TS1 OR TS2
  if Ord(currStatus) < Ord(TS_1) then
  BEGIN

    if dCurrPLPip >= __TS_LvlPip(m_iSymbol, 2) then
    begin
      newStatus := TS_2;
      msg := format('[TS Lvl-2 Touch from P1](CurrPip:%f)>=(Lvl2:%f)[%s]',[dCurrPLPip, dLvlPip, __SideDesc(iSide)]);
    end
    else if dCurrPLPip >= __TS_LvlPip(m_iSymbol, 1) then
    begin
      newStatus := TS_1;
      msg := format('[TS Lvl-1 Touch from P1](CurrPip:%f)>=(Lvl2:%f)[%s]',[dCurrPLPip, dLvlPip, __SideDesc(iSide)]);
    end;
  END
  else if Ord(currStatus) < Ord(TS_2) then
  BEGIN
    dLvlPip := __TS_LvlPip(m_iSymbol, 2);
    if dCurrPLPip >= dLvlPip then
    begin
      newStatus := TS_2;
      msg := format('[TS Lvl-2 Touch from TS1](CurrPip:%f)>=(Lvl2:%f)[%s]',[dCurrPLPip, dLvlPip, __SideDesc(iSide)]);
    end;
  END;

  if currStatus <> newStatus then
  begin
    __ctrls[m_iSymbol].gdPos.Cells[POS_PL_STATUS, iSide] := __PLStatusDesc(newStatus);
    __ctrls[m_iSymbol].gdPos.Cells[POS_TS_BEST,   iSide] := __FmtPip(dCurrPLPip);
  end;

end;



{
  �򰡼ս��� ��� LOSS STATUS ��� �� UPDATE
}
procedure TPLOrder.Calc_LossStatus(iSide:integer; dPLPip, dBaseSpread:double);
var
  currStatus  : TPL_STATUS;
  status      : TPL_STATUS;
begin

  if dPLPip >= 0 then
    exit;

  currStatus := __PLStatusTp( __PLStatus(m_iSymbol, iSide) );

  // ���� TS �����̸� �սǷ� �����Ű�� �ʴ´�.(�ݴ�� ���� �ʴ´�.)
  if Ord(currStatus) >= Ord(TS_1) then
    exit;


       if (dBaseSpread) > (dPLPip*-1)  then  status := PL_L1
  else if (dBaseSpread) <= (dPLPip*-1) then  status := PL_L2
  ;

  __ctrls[m_iSymbol].gdPos.Cells[POS_PL_STATUS, iSide] := __PLStatusDesc(status);
end;


end.

