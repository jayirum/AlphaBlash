unit uOrdThrd;

interface

USES
  Messages,windows,System.Classes, System.SysUtils, vcl.dialogs, vcl.forms
  ,uCommonDef
  ;


type
    TORDERING = record
    artcCd   : string;
    side    : string;
    clrTp   : string;
    qty     : string;
    prc     : string;
    pl_tick : double;
    tm      : string;
    tm_open : double;
    x,y     : integer;
    succ    : boolean;
  end;


  TOrdThrd = class(TThread)
  private
    m_idx       : integer;
    m_myOrd       : TORDERING;
    m_masterOrd  : TMASTER_ORD;
    m_thrdId    : integer;
    m_lstData : TList;
    m_cs      : TRTLCriticalSection;
  protected
    procedure Execute();override;

    function  Copy_And_UpdatePos():boolean;
    function  Copy_MasterOpen():boolean;
    function  Copy_MasterPartial():boolean;
    function  Copy_MasterClr():boolean;
    procedure Copy_ClickOuterOrder();

    function  Calc_AvgPrc():double;

    Procedure InsertCntr_Mine();
    //Procedure InsertCntr_Master();

    procedure MasterOrd_UpdatePosGrid();
    procedure DiffGrid_Update();

    //function  IsScalping():boolean;

  public
    constructor Create;
    destructor Destroy; override;
    procedure SetIdx(idx:integer);
    function GetThreadId():integer;

    procedure AddData(var vMasterOrd:TMASTER_ORD);
    PROCEDURE Clean_Pos();

    procedure Clear_TS_SL(artc:string;idx: Integer; posSide:string; cutTp:string);


  end;


VAR
  __ordThrd : array[1..MAX_STK] of TOrdThrd;


implementation

  uses uMain, CommonUtils, uNotify, uPrcList, uTrailingStop, uSettingTS;

constructor TOrdThrd.Create;
begin
  inherited;
  m_idx := -1;
  InitializeCriticalSection(m_cs);
  m_lstData := TList.Create;
end;

destructor TOrdThrd.Destroy;
begin
  inherited;
  DeleteCriticalSection(m_cs);
end;

procedure TOrdThrd.SetIdx(idx: Integer);
begin
  m_idx := idx;
end;


function TOrdThrd.GetThreadId():integer;
begin
  Result := m_thrdId;
end;


PROCEDURE TOrdThrd.Clean_Pos();
begin
  fmMain.PosGrid_Clear(m_idx);


  //fmMain.gdPosMaster.cells[POS_ARTC, m_idx]    := '';
  fmMain.gdPosMaster.cells[POS_STATUS, m_idx] := POS_STATUS_NONE;
  fmMain.gdPosMaster.cells[POS_SIDE, m_idx]   := '';
  fmMain.gdPosMaster.cells[POS_AVG, m_idx]    := '';
  fmMain.gdPosMaster.cells[POS_QTY, m_idx]    := '';
  fmMain.gdPosMaster.cells[POS_TM, m_idx]     := '';

end;

procedure TOrdThrd.AddData(var vMasterOrd:TMASTER_ORD);
var
  masterOrd : ^TMASTER_ORD;
begin


  GetMem(masterOrd, sizeof(TMASTER_ORD));

  CopyMemory(masterOrd, Addr(vMasterOrd), sizeof(TMASTER_ORD));

  EnterCriticalSection(m_cs);
  m_lstData.Add(masterOrd);
  LeaveCriticalSection(m_cs);

end;

procedure TOrdThrd.Execute;
var
  //rcvmsg : TMSG;
  //nSize  : integer;
  pRcv   : ^TMASTER_ORD;
begin

  //showmessage('thread start:'+inttostr(m_idx));

  m_thrdId := GetCurrentThreadId();

  while not terminated and fmMain.m_bTerminate=false do
  BEGIN
    Sleep(100);
    //while PeekMessage(&rcvmsg, HWND(NIL), WPARAM(Addr(nSize)), LPARAM(Addr(pRcv)), PM_REMOVE)=TRUE do
    //BEGIN

    EnterCriticalSection(m_cs);
    if m_lstData.Count = 0 then
    begin
      LeaveCriticalSection(m_cs);
      continue;
    end;

    pRcv := m_lstData.Items[0];
    ZeroMemory(@m_masterOrd, sizeof(m_masterOrd));
    CopyMemory(@m_masterOrd, pRcv, sizeof(m_masterOrd));
    FreeMem(pRcv);
    m_lstData.Delete(0);
    LeaveCriticalSection(m_cs);


    if fmMain.m_setting[m_idx].master.ItemIndex = 0 then
      continue;

    // ������ �ֹ��� �� �������� �ֹ��ΰ�
    if fmMain.m_setting[m_idx].master.text <> m_masterOrd.masterId then
      continue;

    if fmMain.m_setting[m_idx].artcCd.Text <> m_masterOrd.artc then
      continue;

    // COPY �� �����Ǿ��ִ°�
    if fmMain.m_setting[m_idx].macro.Checked=false then
    begin
      /// �������� ������ ����
      Synchronize(MasterOrd_UpdatePosGrid);
      continue;
    end;

    ZeroMemory(@m_myOrd, sizeof(m_myOrd));

    if NOt Copy_And_UpdatePos() then
      continue;

    /// Macro Order
    Synchronize(Copy_ClickOuterOrder);

    /// ���� ü�᳻��
    Synchronize(InsertCntr_Mine);

    /// �������� ������ ����
    Synchronize(MasterOrd_UpdatePosGrid);

    /// diff
    Synchronize(DiffGrid_Update);

     //
  // TS ó��
  //
  if (m_myOrd.clrTp<>'')   then
    __ts.Update_Pos(m_masterOrd.masterId, m_myOrd.artcCd, m_myOrd.clrTp);


  END;

end;




// True : �ֹ��� ����. False : �ֹ��� �ȳ���.
function  TOrdThrd.Copy_And_UpdatePos():boolean;
begin

  Result := False;

  // �������� �ֹ��� ����
  if m_masterOrd.clrTp = CLR_TP_OPEN then
  BEGIN
    Result := Copy_MasterOpen();
    //fmMain.gdPosMine.Invalidate;
    //Sleep(100);
  END
  else if m_masterOrd.clrTp = CLR_TP_PARTIAL then
  BEGIN
    Result := Copy_MasterPartial();
  END
  else if (m_masterOrd.clrTp = CLR_TP_CLR) then
  BEGIN
    Result := Copy_MasterClr();
  END
  else if (m_masterOrd.clrTp = CLR_TP_RVS) then
  begin
    Result := Copy_MasterClr();
    if Result=True then
    begin
      ZeroMemory(@m_myOrd, sizeof(m_myOrd));
      Result := Copy_MasterOpen();
    end;

  end;


  //TODO fmMain.gdPosMine.Invalidate;


//  //
//  // TS ó��
//  //
//  if (m_myOrd.clrTp<>'')   then
//    __ts.Update_Pos(m_masterOrd.masterId, m_myOrd.artcCd, m_myOrd.clrTp);

  m_myOrd.succ := Result;

end;

function TOrdThrd.Calc_AvgPrc():double;
var
  dCurrQty
  ,dCurrAmt
  ,dNewQty
  ,dNewAmt : double;
begin
// curr amt
  dCurrQty := strtofloatdef(fmMain.gdPosMine.Cells[POS_QTY,m_idx],0);
  dCurrAmt := dCurrQty * strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG,m_idx], 0);

  // incoming amt
  dNewQty := strtofloatdef(fmMain.m_setting[m_idx].qty.Text,0);
  dNewAmt := dNewQty
              * strtofloatdef(__prcList.Prc(m_myOrd.artcCd), 0);

  Result := (dCurrAmt+dNewAmt) / (dCurrQty+dNewQty);
end;

function  TOrdThrd.Copy_MasterOpen():boolean;
var
  msg    : string;
  bRvs   : boolean;

  dAvgPrc   : double;
  itemCntr  : TItemCntr;
  dwRslt    : DWORD;
begin
  Result  := False;
  bRvs    := False;

  m_myOrd.side := m_masterOrd.side;
  if fmMain.m_setting[m_idx].rvs.Checked then
  begin
    bRvs := True;
    if m_masterOrd.side=SIDE_BUY then m_myOrd.side := SIDE_SELL
    else                          m_myOrd.side := SIDE_BUY;
  end;


  // ���� ����
  if (fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = POS_STATUS_NONE) or
    (fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = '')
   then
  BEGIN
    fmMain.AddMsg( format('[�ű�����-%s]������ �ű� Add(%s)',[m_masterOrd.artc, m_myOrd.side]));

    // �ʴ�Ÿ�� �����ϱ� ����
    m_myOrd.tm_open := GetTickCount();

  END;

  // ���� ����
  if fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = POS_STATUS_OPEN then
  begin

    if fmMain.m_setting[m_idx].addPos.Checked=false then
    begin
      fmMain.AddMsg('��Ÿ�� �� ���');
      exit;
    end;

    if (fmMain.gdPosMine.Cells[POS_SIDE, m_idx] <> __Side(m_masterOrd.side)) and
        (bRvs=false)
     then
    begin
      msg := format(
              'ERR![��Ÿ�����-%s] �����ǹ���(%s)�� �������ֹ�����(%s)�� �ٸ���',
                [m_masterOrd.artc, fmMain.gdPosMine.Cells[POS_SIDE, m_idx], __Side(m_masterOrd.side)]
                );
      fmMain.AddMsg(msg, B_SIREN);
      exit;
    end;
    fmMain.AddMsg( format('[������ ��Ÿ��-%s](%s)', [m_masterOrd.artc, m_myOrd.side]));

    // �ʴ�Ÿ�� �����ϱ� ����
    m_myOrd.tm_open := GetTickCount();
  end;

  // MACRO ��ǥ
  if m_myOrd.side = SIDE_BUY then
  BEGIN
    m_myOrd.x := strtointdef(fmMain.m_setting[m_idx].buyx.text,-1);
    m_myOrd.y := strtointdef(fmMain.m_setting[m_idx].buyy.text,-1);
  END;
  if m_myOrd.side = SIDE_SELL then
  BEGIN
    m_myOrd.x := strtointdef(fmMain.m_setting[m_idx].sellx.text,-1);
    m_myOrd.y := strtointdef(fmMain.m_setting[m_idx].selly.text,-1);
  END;

  if (m_myOrd.x < 0) or (m_myOrd.y < 0) then
  begin
    msg := format('ERR![���Խ���-%s] ��ǥ�� ����(����%d)',[m_masterOrd.artc, m_idx]);
    fmMain.AddMsg(msg, B_SIREN);
    exit;
  end;

  m_myOrd.artcCd   := fmMain.gdPosMine.Cells[POS_ARTC, m_idx];
  m_myOrd.clrTp   := CLR_TP_OPEN;
  m_myOrd.pl_tick := 0;
  m_myOrd.tm      := __NowHMS();
  m_myOrd.qty     := fmMain.m_setting[m_idx].qty.Text;
  m_myOrd.prc     := __prcList.Prc(m_myOrd.artcCd);

  // calculate avg prc
  dAvgPrc := Calc_AvgPrc();
  ///////////////////////////


  // Positin Grid
  itemCntr := TItemCntr.Create;
  itemCntr.idx        := m_idx;
  itemCntr.posStatus  := POS_STATUS_OPEN;
  itemCntr.side       := __Side(m_myOrd.side);
  itemCntr.tm         := m_myOrd.tm;
  itemCntr.qty        := formatfloat('#.#',
                                    strtofloatdef(fmMain.gdPosMine.Cells[POS_QTY,m_idx],0)
                                    +
                                    strtofloatdef(fmMain.m_setting[m_idx].qty.Text,0)
                                    );
  itemCntr.avg        := __PrcFmtD(m_myOrd.artcCd, dAvgPrc);

  SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_CNTR,
                              wParam(LongInt(sizeof(itemCntr))),
                              Lparam(LongInt(itemCntr)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );
  Result := True;


end;


function  TOrdThrd.Copy_MasterClr():boolean;
var
  msg : string;
  d1stPrc, d2ndPrc : double;
BEGIN
  Result := false;

  if fmMain.IsAllowedClr(m_idx)=False then
  begin
    fmMain.AddMsg('Copying Close is not allowed');
    exit;
  end;

  m_myOrd.side := m_masterOrd.side;
  if fmMain.m_setting[m_idx].rvs.Checked then
  begin
    if m_masterOrd.side=SIDE_BUY then  m_myOrd.side := SIDE_SELL
    else                               m_myOrd.side := SIDE_BUY;
  end;

  if (fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = POS_STATUS_NONE) or
    (fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = '') then
  begin
    fmMain.AddMsg( format('ERR![û�����-%s]�����ʹ� û���ε� ���� �����̴�. Skip',[m_masterOrd.artc]));
    exit;
  end;

  if fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = POS_STATUS_OPEN then
  begin
    if (fmMain.gdPosMine.Cells[POS_SIDE, m_idx] = __Side(m_masterOrd.side)) and
        (fmMain.m_setting[m_idx].rvs.Checked = false)  then
    begin
      msg := format(
              'ERR![û�����-%s] �����ǹ���(%s)�� �������ֹ�����(%s)�� ����',
                [m_masterOrd.artc,fmMain.gdPosMine.Cells[POS_SIDE, m_idx], __Side(m_masterOrd.side)]
                );
      fmMain.AddMsg(msg);
      exit;
    end;
  end;

  // �ʴ�Ÿ ����
  if fmMain.IsScalping(m_idx) then
  begin
    fmMain.AddMsg(format('ERR![û�����-%s] ��Ÿ�ð� �� û��(����%d)',[m_masterOrd.artc, m_idx]));
    __Siren('��Ÿ�ð� �̳� û�� �߻�');

    // Master Position Update
    Synchronize(MasterOrd_UpdatePosGrid);

    exit;
  end;

  //TS ������ �������̸� û������ �ʴ´�.
  if __ts.IsTS_Running(m_idx) then
  begin
    fmMain.AddMsg(format('[û��SKIP-%s]TS Running ���̶� û�� ó������ �ʴ´�.',[m_masterOrd.artc]), B_SIREN);
    exit;
  end;

  // û���ư ��ǥ
  m_myOrd.x := strtointdef(fmMain.m_setting[m_idx].clrx.text,-1);
  m_myOrd.y := strtointdef(fmMain.m_setting[m_idx].clry.text,-1);

  if (m_myOrd.x < 0) or (m_myOrd.y < 0) then
  begin
    fmMain.AddMsg(format('ERR![û�����-%s] û���ư ��ǥ�� ����(����%d)',[m_masterOrd.artc, m_idx]), B_SIREN);
    exit;
  end;

  m_myOrd.artcCd  := fmMain.gdPosMine.Cells[POS_ARTC, m_idx];
  m_myOrd.clrTp   := CLR_TP_CLR;
  m_myOrd.qty     := fmMain.gdPosMine.Cells[POS_QTY, m_idx];
  m_myOrd.prc     := __prcList.Prc(m_myOrd.artcCd);
  m_myOrd.tm      := __NowHMS();


  d1stPrc:=0;
  d2ndPrc:=0;

  // pl tick => FOR CNTR GRID
  if m_myOrd.side=SIDE_BUY then
  begin
    d1stPrc := strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG,m_idx],0);
    d2ndprc := strtofloatdef(m_myOrd.prc,0);
  end
  else if m_myOrd.side = SIDE_SELL then
  begin
    d1stPrc := strtofloatdef(m_myOrd.prc,0);
    d2ndprc := strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG,m_idx],0);
  end;

  m_myOrd.pl_tick := d1stPrc - d2ndPrc;
  m_myOrd.pl_tick := m_myOrd.pl_tick / __TickSize(m_myOrd.artcCd);

  //
  fmMain.PosGrid_Clear(m_idx);

  __ts.Clear_TS(m_idx);
  //

  Result := True;

END;


// TrailingStop �����忡�� û���ų��
procedure TOrdThrd.Clear_TS_SL(artc:string; idx:Integer;posSide:string; cutTp:string);
var
  msg : string;
  d1stPrc, d2ndPrc : double;
BEGIN

  if (fmMain.gdPosMine.Cells[POS_STATUS, m_idx]=POS_STATUS_NONE) or
    (fmMain.gdPosMine.Cells[POS_STATUS, m_idx]='') then
  BEGIN
    msg := format('[% ����-%s]�����̴�.',[ cutTp, artc]);
    fmMain.AddMsg(msg);
    exit;
  END;

  if posSide=SIDE_BUY then
    m_myOrd.side := SIDE_SELL
  else
    m_myOrd.side := SIDE_BUY;

  m_myOrd.x := strtointdef(fmMain.m_setting[m_idx].clrx.text,-1);
  m_myOrd.y := strtointdef(fmMain.m_setting[m_idx].clry.text,-1);

  if (m_myOrd.x < 0) or (m_myOrd.y < 0) then
  begin
    fmMain.AddMsg(format('ERR![%����-%s]û���ư ��ǥ�� ����(����%d)',[cutTp, artc, m_idx]), B_SIREN);
    exit;
  end;

  m_myOrd.artcCd  := fmMain.gdPosMine.Cells[POS_ARTC, m_idx];
  m_myOrd.clrTp   := CLR_TP_CLR;
  m_myOrd.qty     := fmMain.gdPosMine.Cells[POS_QTY, m_idx];
  m_myOrd.prc     := __prcList.Prc(m_myOrd.artcCd);
  m_myOrd.tm      := __NowHMS();

  // Position Grid
  // pl tick
  d1stPrc:=0;
  d2ndPrc:=0;
  if m_myOrd.side=SIDE_BUY then
  begin
    d1stPrc := strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG,m_idx],0);
    d2ndprc := strtofloatdef(m_myOrd.prc,0);
  end
  else if m_myOrd.side = SIDE_SELL then
  begin
    d1stPrc := strtofloatdef(m_myOrd.prc,0);
    d2ndprc := strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG,m_idx],0);
  end;

  m_myOrd.pl_tick := d1stPrc - d2ndPrc;
  m_myOrd.pl_tick := m_myOrd.pl_tick / __TickSize(m_myOrd.artcCd);

  // Macro �ֹ�
  Synchronize(Copy_ClickOuterOrder);

  // Position Grid
  fmMain.PosGrid_Clear(idx);

  // ü�᳻��Grid
  Synchronize(InsertCntr_Mine);

  fmMain.AddMsg(format('[%s-%s] ����', [cutTp, artc]));

END;



function  TOrdThrd.Copy_MasterPartial():boolean;
var
  msg : string;
  qty : integer;
  currqty : integer;

  d1stPrc, d2ndPrc : double;
  ItemCntr : TItemCntr;
  dwRslt   : DWORD;
begin

  Result := false;

  if fmMain.IsAllowedClr(m_idx)=False then
  begin
    fmMain.AddMsg('Copying Close is not allowed');
    exit;
  end;


  m_myOrd.side := m_masterOrd.side;
  if fmMain.m_setting[m_idx].rvs.Checked then
  begin
    if m_masterOrd.side=SIDE_BUY then  m_myOrd.side := SIDE_SELL
    else                               m_myOrd.side := SIDE_BUY;
  end;

  // ��������
  if (fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = POS_STATUS_NONE) or
    (fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = '' ) then
  begin
    fmMain.AddMsg(format('ERR![�Ϻ�û�����-%s] �����ʹ� �Ϻ�û���ε� ���� �����̴�',[m_masterOrd.artc]), B_SIREN);
    exit;
  end;

  // ���Ի���
  if fmMain.gdPosMine.Cells[POS_STATUS, m_idx] = POS_STATUS_OPEN then
  begin
    if (fmMain.gdPosMine.Cells[POS_SIDE, m_idx] = __Side(m_masterOrd.side)) and
               (  fmMain.m_setting[m_idx].rvs.Checked = false ) then
    begin
      msg := format(
              'ERR![�Ϻ�û�����-%s] �����ǹ���(%s)�� �������ֹ�����(%s)�� ����',
                [m_masterOrd.artc, fmMain.gdPosMine.Cells[POS_SIDE, m_idx], __Side(m_masterOrd.side)]
                );
      fmMain.AddMsg(msg, B_SIREN);
      exit;
    end;
  end;


  //TS ������ �������̸� û������ �ʴ´�.
  if __ts.IsTS_Running(m_idx) then
  begin
    fmMain.AddMsg(format('[û��SKIP-%s]TS Running ���̶� �Ϻ�û�� ó������ �ʴ´�.',[m_masterOrd.artc]), B_SIREN);
    exit;
  end;

  // �����ʹ� �Ϻ�û���ε� ���� ����û���̸� ó������ �ʴ´�.
  currqty := strtointdef(fmMain.gdPosMine.Cells[POS_QTY, m_idx],0);
  if currqty = strtointdef(m_masterOrd.cntrQty,0) then
  begin
      fmMain.AddMsg(format('[�Ϻ�û��SKIP-%s]�����ʹ� �Ϻ�û���ε� ���� ����û��.skip',[m_masterOrd.artc]));
      exit;
  end;

    // �ʴ�Ÿ ����
  if fmMain.IsScalping(m_idx) then
  begin
    fmMain.AddMsg(format('ERR![�Ϻ�û�����-%s] ��Ÿ�ð� �� û��(����%d)',[m_masterOrd.artc, m_idx]));
    __Siren('��Ÿ�ð� �̳� �Ϻ�û�� �߻�');

    // Master Position Update
    Synchronize(MasterOrd_UpdatePosGrid);

    exit;
  end;


  m_myOrd.artcCd  := fmMain.gdPosMine.Cells[POS_ARTC, m_idx];
  m_myOrd.clrTp   := CLR_TP_PARTIAL;
  m_myOrd.qty     := m_masterOrd.cntrQty;
  m_myOrd.prc     := __prcList.Prc(m_myOrd.artcCd);
  m_myOrd.tm      := __NowHMS();


  // ���尡��ư ��ǥ
  if m_myOrd.side = SIDE_BUY then
  BEGIN
    m_myOrd.x := strtointdef(fmMain.m_setting[m_idx].buyx.text,-1);
    m_myOrd.y := strtointdef(fmMain.m_setting[m_idx].buyy.text,-1);
  END;
  if m_myOrd.side = SIDE_SELL then
  BEGIN
    m_myOrd.x := strtointdef(fmMain.m_setting[m_idx].sellx.text,-1);
    m_myOrd.y := strtointdef(fmMain.m_setting[m_idx].selly.text,-1);
  END;

  if (m_myOrd.x < 0) or (m_myOrd.y < 0) then
  begin
    fmMain.AddMsg(format('ERR![�Ϻ�û�����-%s] �ż��ŵ���ư ��ǥ�� ����',[m_masterOrd.artc]), B_SIREN);
    exit;
  end;


  // pl tick : FOR CNTR GRID
  if m_myOrd.side=SIDE_BUY then
  begin
    d1stPrc := strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG,m_idx],0);
    d2ndprc := strtofloatdef(m_myOrd.prc,0);
  end
  else if m_myOrd.side = SIDE_SELL then
  begin
    d1stPrc := strtofloatdef(m_myOrd.prc,0);
    d2ndprc := strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG,m_idx],0);
  end;

  m_myOrd.pl_tick := d1stPrc - d2ndPrc;
  m_myOrd.pl_tick := m_myOrd.pl_tick / __TickSize(m_myOrd.artcCd);

  //
  qty := strtointdef(fmMain.gdPosMine.Cells[POS_QTY,m_idx],0);
  qty := qty - strtoint(m_masterOrd.cntrQty);


   // Positin Grid
  itemCntr := TItemCntr.Create;
  itemCntr.idx        := m_idx;
  itemCntr.qty        := inttostr(qty);

  SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_CNTR,
                              wParam(LongInt(sizeof(itemCntr))),
                              Lparam(LongInt(itemCntr)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );

  Result := True;
end;


procedure TOrdThrd.Copy_ClickOuterOrder();
begin
  SetCursorPos(m_myOrd.x, m_myOrd.y);

  mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); //press left button
  mouse_event(MOUSEEVENTF_LEFTUP,0, 0, 0, 0); //release left button

  fmMain.AddMsg(format('[%s] Macro �ֹ� ����(%d)(%d)',[m_myOrd.artcCd, m_myOrd.x, m_myOrd.y]));
end;

//
procedure TOrdThrd.InsertCntr_Mine();
var
  rowcnt : integer;
begin
  rowcnt := fmMain.gdCntrMine.RowCount;

  if rowcnt=2 then
  begin
    if fmMain.gdCntrMine.Cells[CNTR_SEQ, 1]<>''  then
    begin
      fmMain.gdCntrMine.InsertRows(1, 1);
      rowcnt := 2;
    end
    else
      rowcnt := 1;
  end
  else
  begin
    fmMain.gdCntrMine.InsertRows(1, 1);
    rowcnt := rowcnt + 1;
  end;

  fmMain.gdCntrMine.Cells[CNTR_SEQ, 1]      := INTTOSTR(rowcnt);
  fmMain.gdCntrMine.Cells[CNTR_ID, 1]       := fmMain.m_setting[m_idx].master.Text;
  fmMain.gdCntrMine.Cells[CNTR_TM, 1]       := m_myOrd.tm;
  fmMain.gdCntrMine.Cells[CNTR_STK, 1]      := m_myOrd.artcCd;
  fmMain.gdCntrMine.Cells[CNTR_SIDE, 1]     := __Side(m_myOrd.side);
  fmMain.gdCntrMine.Cells[CNTR_CLR_TP, 1]   := __ClrTp(m_myOrd.clrtp);
  fmMain.gdCntrMine.Cells[CNTR_PRC, 1]      := __PrcFmt( m_myOrd.artcCd, m_myOrd.prc);
  fmMain.gdCntrMine.Cells[CNTR_QTY, 1]      := m_myOrd.qty;
  fmMain.gdCntrMine.Cells[CNTR_PL_TICK, 1]  := formatfloat('#0.#', m_myOrd.pl_tick);

  fmMain.gdCntrMine.Cells[CNTR_PL, 1]       := __MoneyFmtD(
                                              m_myOrd.pl_tick *
                                              strtofloatdef(fmMain.m_setting[m_idx].tickval.text,1.0)
                                              );
end;


procedure TOrdThrd.MasterOrd_UpdatePosGrid();

begin

  // Master ����
  if m_masterOrd.clrtp = CLR_TP_OPEN THEN
  BEGIN
    if fmMain.gdPosMaster.cells[POS_STATUS, m_idx] = POS_STATUS_OPEN THEN
    BEGIN
      if fmMain.gdPosMaster.cells[POS_SIDE, m_idx] <> __Side(m_masterOrd.side) then
      begin
        //fmMain.AddMsg(format('[%s] Master ������.Side �� �ٸ���. �����.',[m_masterOrd.artc]));
        fmMain.gdPosMaster.cells[POS_QTY, m_idx] := m_masterOrd.cntrQty;
      end
      else
      begin
        //qty := strtointdef(fmMain.gdPosMaster.cells[POS_QTY, m_idx],0) + strtointdef(m_masterOrd.cntrQty,0);
        fmMain.gdPosMaster.cells[POS_QTY, m_idx] := m_masterOrd.af_nclrQty;

        fmMain.gdPosMaster.Cells[POS_AVG, m_idx] := __PrcFmt(m_masterOrd.artc, m_masterOrd.af_avgPrc);
      end;
    END
    ELSE
    begin
      fmMain.gdPosMaster.cells[POS_QTY, m_idx] := m_masterOrd.af_nclrQty;
      fmMain.gdPosMaster.Cells[POS_AVG, m_idx] := __PrcFmt(m_masterOrd.artc, m_masterOrd.cntrPrc);
    end;

    fmMain.gdPosMaster.cells[POS_STATUS, m_idx]  := POS_STATUS_OPEN;
    fmMain.gdPosMaster.Cells[POS_SIDE, m_idx]    := __Side(m_masterOrd.side);
    fmMain.gdPosMaster.Cells[POS_TM, m_idx]      := m_masterOrd.tradeTm;

  END;

  // Master û��/�Ϻ�û
  if (m_masterOrd.clrtp = CLR_TP_PARTIAL) or (m_masterOrd.clrtp = CLR_TP_CLR) THEN
  BEGIN
    if (fmMain.gdPosMaster.cells[POS_STATUS, m_idx] = POS_STATUS_NONE) or
      (fmMain.gdPosMaster.cells[POS_STATUS, m_idx] = '') THEN
    BEGIN
      //fmMain.AddMsg(format('ERR![%s] Master ������. �����ε� û���� ���Դ�',[m_masterOrd.artc]));
      EXIT;
    END;

    if fmMain.gdPosMaster.Cells[POS_SIDE, m_idx] = __Side(m_masterOrd.side) then
    BEGIN
      //fmMain.AddMsg( format('ERR![%s] Master ������.û���ֹ��ε� �����ǰ� ü���� SIDE�� ����.',[m_masterOrd.artc]));
      EXIT;
    END;

    if m_masterOrd.clrtp = CLR_TP_PARTIAL THEN
    BEGIN
      //qty := strtointdef(fmMain.gdPosMaster.cells[POS_QTY, m_idx],0) - strtointdef(m_masterOrd.cntrQty,0);
      fmMain.gdPosMaster.cells[POS_QTY, m_idx] := m_masterOrd.af_nclrQty;
    END
    ELSE if m_masterOrd.clrtp = CLR_TP_CLR then
    BEGIN
      fmMain.gdPosMaster.cells[POS_STATUS, m_idx]  := POS_STATUS_NONE;
      fmMain.gdPosMaster.Cells[POS_SIDE, m_idx]    := '';
      fmMain.gdPosMaster.Cells[POS_AVG, m_idx]     := '';
      fmMain.gdPosMaster.Cells[POS_TM, m_idx]      := '';
      fmMain.gdPosMaster.cells[POS_QTY, m_idx]     := '';
    END;
  END;

end;


procedure TOrdThrd.DiffGrid_Update();
VAR
  gap:double;
begin

  if fmMain.gdPosMine.Cells[POS_STATUS, m_idx] <> fmMain.gdPosMaster.Cells[POS_STATUS, m_idx] then
  begin
    //fmMain.AddMsg(format('[����%d] �� ������ �����ǻ��°� �ٸ���.',[m_idx]) );
    exit;
  end;


  if fmMain.gdPosMine.Cells[POS_SIDE, m_idx] = SIDE_BUY then
  begin
    gap :=  strtofloatdef(fmMain.gdPosMaster.Cells[POS_AVG, m_idx],0) -
            strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG, m_idx],0);
  end
  else
  begin
    gap :=  strtofloatdef(fmMain.gdPosMine.Cells[POS_AVG, m_idx],0) -
            strtofloatdef(fmMain.gdPosMaster.Cells[POS_AVG, m_idx],0);
  end;

  fmMain.gdDiff.Cells[DIFF_PRC, m_idx]  := __PrcFmtD(m_myOrd.artcCd,gap);
  fmMain.gdDiff.Cells[DIFF_TICK, m_idx] := formatfloat(
                                          '#0.#0',
                                          gap/ __TickSize(m_myOrd.artcCd)
                                          );

end;
//
//function  TOrdThrd.IsScalping():boolean;
//var
//  openTm    : double;
//  GapSec    : integer;
//  sBaseSec  : string;
//  bRslt     : boolean;
//begin
//
//  openTm  := strtofloatdef(fmMain.gdPosMine.Cells[POS_OPEN_TICKCOUNT, m_idx] ,0);
//
//  GapSec  := __CalcTimeGapSec(openTm) ;
//
//  sBaseSec := fmMain.m_setting[m_idx].scalping.Items[fmMain.m_setting[m_idx].scalping.ItemIndex];
//
//  bRslt  := (GapSec < strtointdef(sBaseSec,0));
//  Result := bRslt;
//end;

//
//procedure TOrdThrd.InsertCntr_Master;
//var
//  rowcnt : integer;
//  dClrPl : double;
//  dTickSize : double;
//begin
//    // ü�᳻��
//  rowcnt := fmMain.gdCntrMaster.RowCount;
//
//  if rowcnt=2 then
//  begin
//    if fmMain.gdCntrMaster.Cells[CNTR_CNTR_NO, 1]<>''  then
//    begin
//      fmMain.gdCntrMaster.InsertRows(1, 1);
//      rowcnt := 2;
//    end
//    else
//      rowcnt := 1;
//  end
//  else
//  begin
//    fmMain.gdCntrMaster.InsertRows(1, 1);
//    rowcnt := rowcnt + 1;
//  end;
//
//  fmMain.gdCntrMaster.Cells[CNTR_SEQ, 1]     := INTTOSTR(rowcnt);
//  fmMain.gdCntrMaster.Cells[CNTR_ID, 1]      := m_masterOrd.masterId;
//  fmMain.gdCntrMaster.Cells[CNTR_CNTR_NO, 1] := m_masterOrd.cntrNo;
//  fmMain.gdCntrMaster.Cells[CNTR_TM, 1]      := m_masterOrd.tradeTm;
//  fmMain.gdCntrMaster.Cells[CNTR_STK, 1]     := m_masterOrd.artc;
//  fmMain.gdCntrMaster.Cells[CNTR_SIDE, 1]    := __Side(m_masterOrd.side);
//  fmMain.gdCntrMaster.Cells[CNTR_QTY, 1]     := m_masterOrd.cntrQty;
//  fmMain.gdCntrMaster.Cells[CNTR_PRC, 1]     := m_masterOrd.cntrPrc;
//  fmMain.gdCntrMaster.Cells[CNTR_ORD_TP, 1]  := __OrdTp(m_masterOrd.ordTp);
//  fmMain.gdCntrMaster.Cells[CNTR_CLR_TP, 1]  := __ClrTp(m_masterOrd.clrTp);
//  fmMain.gdCntrMaster.Cells[CNTR_PL, 1]      := __MoneyFmt( m_masterOrd.clrPl);
//  fmMain.gdCntrMaster.Cells[CNTR_LVG, 1]     := m_masterOrd.lvg;
//
//  dClrPl := strtofloatdef(m_masterOrd.clrPl,0);
//
//  dTickSize := strtofloatdef(fmMain.m_setting.tickval[m_idx].Text,1);
//  fmMain.gdCntrMaster.Cells[CNTR_PL_TICK, 1] := FloatToStr(dClrPl / dTickSize);
//
//end;



end.
