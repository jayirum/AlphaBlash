unit uMain;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs
  , XAlphaPacket, sSkinProvider, sSkinManager,
  Vcl.StdCtrls, sComboBox, Vcl.ExtCtrls, sPanel, CommonUtils, sLabel, sButton,
  sEdit, AdvUtil, Vcl.Grids, AdvObj, BaseGrid, AdvGrid, Vcl.Buttons, sBitBtn,
  uNotify, ThdTimer,
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdThreadComponent,
  IdExceptionCore

  ;

const
  MAX_MASTERS_CNT = 10;

type

  TMasterInfo = record
    sID           : string;
    loginTp       : string;  // I, O
    nLastCntrNo   : integer;
  end;

  TfmMain = class(TForm)
    pnlBody: TsPanel;
    pnlBottom: TsPanel;
    cbMsg: TsComboBox;
    sLabel2: TsLabel;
    edCntrTm: TsEdit;
    sLabel4: TsLabel;
    sLabel5: TsLabel;
    sLabel6: TsLabel;
    sLabel7: TsLabel;
    sLabel9: TsLabel;
    sLabel10: TsLabel;
    edCntrPrc: TsEdit;
    edCntrQty: TsEdit;
    edType: TsEdit;
    pnlGrid: TsPanel;
    gdCntr: TAdvStringGrid;
    edCntrNo: TsEdit;
    edSide: TsEdit;
    sLabel3: TsLabel;
    edPl: TsEdit;
    sLabel16: TsLabel;
    sLabel17: TsLabel;
    sLabel18: TsLabel;
    sLabel19: TsLabel;
    sLabel20: TsLabel;
    sLabel21: TsLabel;
    sLabel22: TsLabel;
    sPanel1: TsPanel;
    sLabel1: TsLabel;
    edMasterId: TEdit;
    edStk: TEdit;
    edCmsn: TsEdit;
    edBfQty: TsEdit;
    edAfQty: TsEdit;
    edBfAvg: TsEdit;
    edAfAvg: TsEdit;
    edLvg: TsEdit;
    pnlTop: TPanel;
    cbMasterIDs: TComboBox;
    pnlLoginTp: TPanel;
    edPwd: TsEdit;
    btnConn: TButton;
    lblLastLogonoffTime: TLabel;
    btnCntrHist: TButton;
    chkFixMaster: TCheckBox;
    IdTCPClient1: TIdTCPClient;
    IdThreadComponent1: TIdThreadComponent;
    btnCheck: TButton;
    edClrTp: TsEdit;
    Splitter1: TSplitter;
    tmrSetIDCombo: TThreadedTimer;
    tmrFlicker: TThreadedTimer;

    function  Initialize():boolean;
    
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);

    procedure AddMsg(msg:string);
    procedure btnConnClick(Sender: TObject);

    function EndMainByDisconn():boolean;
    procedure btnCntrHistClick(Sender: TObject);
    procedure cbMasterIDsChange(Sender: TObject);
    procedure tmrSetIDComboTimer(Sender: TObject);


    procedure AppExceptionHandler(sender:TObject; E:Exception);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure IdTCPClient1Connected(Sender: TObject);
    procedure IdTCPClient1Disconnected(Sender: TObject);
    procedure IdThreadComponent1Run(Sender: TIdThreadComponent);
    procedure btnCheckClick(Sender: TObject);
    procedure edPwdKeyPress(Sender: TObject; var Key: Char);
    procedure IdThreadComponent1Exception(Sender: TIdThreadComponent;
      AException: Exception);
    procedure IdThreadComponent1Stopped(Sender: TIdThreadComponent);
    procedure IdThreadComponent1Terminate(Sender: TIdThreadComponent);
    procedure tmrFlickerTimer(Sender: TObject);

  public
      m_bPwdOk : boolean;

  private
    { Private declarations }

    procedure Proc_Packet();
    procedure Proc_LogOnOff();
    procedure Proc_Cntr(bHistory:boolean);
    procedure Proc_Msg();

    function  GetIdxOfMasterId(sMasterID:string):integer;
    function  IsExistsID(sMasterID:string):boolean;

    procedure ClearCntrGrid();
    FUNCTION  IsMasterFixed():boolean;
    function  GetFixedIdx():integer;

  private
    m_sRcvPack      : string;
    m_sPwd          : string;
    m_arrMasters    : array[0..MAX_MASTERS_CNT-1] of TMasterInfo;
    m_nCurrMastersCnt   : integer;

    m_currComboIdx  : integer;

    ////////////////////////////////////
    ///  Socket 관련
  private
    function  ConnectSvr():boolean;
    procedure SendData(sData:string; len:integer);
  private
    m_sSvrIp      : string;
    m_nSvrPort    : integer;
    m_bConnected  : boolean;

    m_CnfgName  : string;

    
  end;


  TPacketThrd = class(TThread)
    procedure Execute();override;
  end;





var
  fmMain: TfmMain;

implementation

const
  I_NO        = 0;
  I_MASTER_ID = 1;
  I_CNTR_NO = 2;
  I_TM      = 3;
  I_STK     = 4;
  I_SIDE    = 5;
  I_QTY     = 6;
  I_PRC     = 7;
  I_TYPE    = 8;
  I_CLR_TP  = 9;
  I_PL      = 10;
  I_CMSN    = 11;
  I_BF_QTY  = 12;
  I_AF_QTY  = 13;
  I_BF_AVG  = 14;
  I_AF_AVG  = 15;
  I_LVG     = 16;
var
  _packetThrd : TPacketThrd;
  _bClosing   : boolean;

{$R *.dfm}



function _LoginTp(tp:string):string;
begin
  Result := '로그인';
  if tp='O' then
    Result := '로그아웃';

end;



function _ClrTp(tp:string):string;
begin
  Result := '진입';
  if tp='2' then Result := '일부청산'
  else if tp='3' then Result :='청산'
  else if tp='4' then Result :='역전';
end;



function  TfmMain.ConnectSvr():boolean;
var
  sPort : string;
begin

  m_bConnected := False;
  
  Result := True;
  
  m_CnfgName := __Get_CFGFileName();

  m_sSvrIp    := __Get_CFGFile('SERVER', 'IP', '', False, m_CnfgName);
  sPort       := __Get_CFGFile('SERVER', 'PORT', '', false, m_CnfgName);
  m_nSvrPort := strtointdef(sPort,0);

  if (m_sSvrIp='') or (m_nSvrPort=0) then
  begin
    showmessage('SERVER 정보 없음');
    Result := False;
    exit;
  end;


  IdTCPClient1.ReadTimeout := 100;  //millisecond
  IdTCPClient1.Host  := m_sSvrIp;
  IdTCPClient1.Port  := m_nSvrPort;
  IdTCPClient1.Connect;

end;

procedure TfmMain.edPwdKeyPress(Sender: TObject; var Key: Char);
begin
if key = #13 then
  begin
    if length( trim(edPwd.Text) )>0 then
      btnConnClick(sender);
  end;
end;

FUNCTION  TfmMain.IsMasterFixed():boolean;
begin
  Result := (chkFixMaster.Checked = True);
end;

function  TfmMain.GetFixedIdx():integer;
begin
  Result := cbMasterIDs.ItemIndex;
end;

procedure TfmMain.btnCheckClick(Sender: TObject);
begin
  pnlLoginTp.color  := clWhite;
  pnlLoginTp.Font.Color := clblack;

  edMasterId.Color  := clwhite;
  edMasterId.Font.Color := clblack;

  edStk.Color       := clwhite;
  edStk.Font.Color := clblack;

  edSide.Color      := clwhite;
  edSide.Font.Color := clblack;

  edCntrQty.Color   := clwhite;
  edCntrQty.Font.Color := clblack;

  edClrTp.Color   := clwhite;
  edClrTp.Font.Color := clblack;
end;


procedure TfmMain.btnConnClick(Sender: TObject);
var
  sPwd : string;
  sPacket : string;
begin
    
  sPacket := CODE_PWD + edPwd.text;

  if IdTCPClient1.Connected=False then
    ConnectSvr;

  SendData(sPacket, sPacket.Length);


end;

procedure TfmMain.cbMasterIDsChange(Sender: TObject);
begin

  if IsMasterFixed() then
  begin
    if m_currComboIdx <> cbMasterIDs.ItemIndex then
    begin
      Showmessage('먼저 [ID고정]을 해제 하세요');
      cbMasterIDs.ItemIndex :=  m_currComboIdx;
      exit;
    end;
  end;

  pnlLoginTp.Caption := _LoginTp( m_arrMasters[cbMasterIds.ItemIndex].loginTp);

  m_currComboIdx := cbMasterIDs.ItemIndex;

  ClearCntrGrid();
end;



procedure TfmMain.ClearCntrGrid();
var
  i : integer;
begin
  for i := gdCntr.RowCount-1 downto 2 do
    gdCntr.RemoveNormalRow(i);

  for i := 0 to gdCntr.ColCount-1 do
      gdCntr.Cells[i, 1] := '';


end;

procedure TfmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  //IdTCPClient1.Disconnect;
end;

procedure TfmMain.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
begin
  _bClosing := True;

  if idThreadComponent1.Active then
    idThreadComponent1.Terminate;

  if _packetThrd<>NIL then
    _packetThrd.Terminate;

end;


function TfmMain.Initialize:boolean;
begin
  __PacketQ := CPacketQueue.Create;

  if ConnectSvr = false then
  begin
    Result := false;
    exit;
  end;
end;


procedure TfmMain.FormCreate(Sender: TObject);
begin

  _packetThrd := NIL;
  __PacketQ   := NIL;

  Application.OnException := AppExceptionHandler;

  pnlLoginTp.Color := clwhite;

  if Initialize()=false then
  begin
    exit;
  end;

  m_bPwdOk := False;

  m_nCurrMastersCnt := 0;

end;

procedure TfmMain.FormShow(Sender: TObject);
begin

  _bClosing     := False;
  _packetThrd   := TPacketThrd.Create();
  

  edCntrNo.Text := '0';

  edPwd.Text := '';
  edPwd.SetFocus;

end;



procedure TfmMain.AddMsg(msg:string);
begin
    cbMsg.Items.Insert(0, format('[%s] %s', [__NowHMS(), msg]));
    cbMsg.ItemIndex := 0;
end;


function TfmMain.EndMainByDisconn():boolean;
begin
  Result := false;

  if IdTCPClient1.Connected=true then
    exit;

  if fmMain.m_bPwdOk=True then
    __ShowAlarm(TP_SIREN, '서버와의 연결이 끊어졌습니다.다시 실행하세요.')
  else
    showmessage('서버와의 연결이 끊어졌습니다. 다시 실행하세요.');

  Result := true;
end;

procedure TPacketThrd.Execute();
var
  ret : integer;
begin

    while (not terminated) AND (_bClosing=False) do
    begin
      Sleep(10);

      if fmMain.EndMainByDisconn()=true then
        exit;


      fmMain.m_sRcvPack := '';
      ret := __PacketQ.GetOnePacket(fmMain.m_sRcvPack);

      if __PacketQ.IsFailedToGet(ret) then
      begin
        fmMain.AddMsg('Error!!!! GetOnePacket');
        continue;
      end;

      if __PacketQ.IsGet(ret) then
      begin
        Synchronize( fmMain.Proc_Packet);
        //fmMain.AddMsg(fmMain.m_sRcvPack);
      end;


    end;

end;


procedure TfmMain.Proc_Packet();
var
  sCode : string;
begin

  sCode := __PacketQ.GetCode(m_sRcvPack);

  if sCode = CODE_LOGONOFF then
  begin
    if m_bPwdOk=True then
      Proc_LogOnOff()
  end
  else if sCode = CODE_CNTR THEN
  begin
    if m_bPwdOk=True then
      Proc_Cntr(False)
  end
  else if sCode = CODE_CNTR_HIST THEN
  begin
    if m_bPwdOk=True then
      Proc_Cntr(True)
  end
  else if sCode = CODE_MSG then
    Proc_Msg();


end;


procedure TfmMain.tmrFlickerTimer(Sender: TObject);
begin
  tmrFlicker.Enabled := False;

  btnCheckClick(sender);
end;

procedure TfmMain.tmrSetIDComboTimer(Sender: TObject);
begin
    cbMasterIDs.ItemIndex := 0;
    m_currComboIdx        := 0;
    pnlLoginTp.Caption    := _LoginTp(m_arrMasters[0].loginTp);
    tmrSetIDCombo.Enabled   := False;
end;


procedure TfmMain.Proc_Msg();
var
  sRetCode : string;
  sMsg     : string;
  bSucc    : boolean;
begin

  sRetCode := Copy(m_sRcvPack, 3, 2);


  if sRetCode = RETCODE_PWD_WRONG then
    __ShowMsg('오류', '비밀번호 오류')
  else if sRetCode = RETCODE_CNTR_NODATA then
    AddMsg('체결내역이 없습니다.')
  else if sRetCode=RETCODE_PWD_OK then
    m_bPwdOk := True;

end;




procedure TfmMain.IdTCPClient1Connected(Sender: TObject);
begin
  IdThreadComponent1.Active  := True;

  m_bConnected := True;
end;

procedure TfmMain.IdTCPClient1Disconnected(Sender: TObject);
begin
  m_bConnected := False;
  if not IdThreadComponent1.Terminated then
    IdThreadComponent1.Terminate

end;

procedure TfmMain.IdThreadComponent1Exception(Sender: TIdThreadComponent;
  AException: Exception);
begin
  Application.Terminate;
end;

procedure TfmMain.IdThreadComponent1Run(Sender: TIdThreadComponent);
var
    s : string;
begin


  //try
    // ... read message from server
    s := '';
    s := IdTCPClient1.IOHandler.ReadLn();
    if IdTCPClient1.IOHandler.ReadLnTimedout = False then
    begin
      if (IdTCPClient1.Connected) and (length(s)>0) then
        __PacketQ.Add(s);
    end;

    if IdTCPClient1.Connected=False then
      IdThreadComponent1.Terminate;
      //showmessage('read timeout');

//  except
//      on E : EIdNotConnected do
//      begin
//         if _bClosing=False then
//           ShowMessage('서버와의 연결 Closed-1(EIdNotConnected). 새로 실행해주세요');
//
//        m_bConnected := false;
//        IdThreadComponent1.Active := false;
//      end;
//      on E : EIdClosedSocket do
//      begin
//         if _bClosing=False then
//           ShowMessage('서버와의 연결 Closed-2(EIdClosedSocket). 새로 실행해주세요');
//
//        m_bConnected := false;
//        IdThreadComponent1.Active := false;
//      end;
//      ELSE
//      BEGIN
//
//        m_bConnected := false;
//        IdThreadComponent1.Active := false;
//
//        if _bClosing=False then
//          ShowMessage('통신예외 발생. 새로 실행해주세요')
//        else
//          Application.terminate;
//      end;
//
//      exit;
//
//  end;



end;

procedure TfmMain.IdThreadComponent1Stopped(Sender: TIdThreadComponent);
begin
  //showmessage('IdThreadComponent1Stopped');
end;

procedure TfmMain.IdThreadComponent1Terminate(Sender: TIdThreadComponent);
begin
  if IdTCPClient1.Connected then
    IdTCPClient1.Disconnect;
end;

function  TfmMain.IsExistsID(sMasterID:string):boolean;
begin
  Result := (GetIdxOfMasterId(sMasterID)>-1);
end;

function  TfmMain.GetIdxOfMasterId(sMasterID:string):integer;
var
  i       : integer;
  bFound  : boolean;
begin

  bFound := False;
  for i := 0 to m_nCurrMastersCnt do
  begin

    if sMasterID = m_arrMasters[i].sID then
    begin
      bFound := TRUE;
      break;
    end;
  end;

  if bFound then
    Result := i
  else
    Result := -1;

end;


procedure TfmMain.Proc_LogOnOff();
var

  oldDataYn
  ,masterId
  ,Tm
  ,loginTp
  ,masterNm   : string;

  iPos    : integer;

  bSound  : boolean;
  alarmTp : integer;

  idxMaster : integer;
  bChanged  : boolean;
  bFirstData : boolean;
begin


  oldDataYn := copy(m_sRcvPack, 3, 1);
  masterId  := Trim(copy(m_sRcvPack, 4, 20));
  Tm        := copy(m_sRcvPack, 24, 12);
  loginTp   := copy(m_sRcvPack, 36, 1);
  masterNm  := Trim(copy(m_sRcvPack, 37, 20));

  if IsMasterFixed() then
  begin
    if masterId <> cbMasterIDs.Items[cbMasterIDs.ItemIndex] then
    begin
      AddMsg(format('[%s] ID고정 사용중이므로 로그온/아웃 처리하지 않음', [masterId]));
      exit;
    end;
  end;
                                          

  bSound      := false;
  bChanged    := False;
  bFirstData  := false;

  idxMaster := GetIdxOfMasterId(masterId);

  // 첫 데이터
  if idxMaster = -1 then
  begin
    m_arrMasters[m_nCurrMastersCnt].sID     := masterId;
    m_arrMasters[m_nCurrMastersCnt].loginTp := loginTp;

    cbMasterIDs.Items.Add(masterId);
    //cbMasterIDs.ItemIndex := 0; //m_nCurrMastersCnt;

    Inc(m_nCurrMastersCnt);

    bChanged    := True;
    bFirstData  := True;

    if tmrSetIDCombo.Enabled = False then
    begin
      tmrSetIDCombo.Interval := 1000;
      tmrSetIDCombo.Enabled := True;
    end;


  end
  else
  begin
    // 이미 있는 ID 인데, 지난 데이터가 들어오면 SKIP
    if oldDataYn='Y' then
    begin
        exit;
    end;

    IF loginTp <> m_arrMasters[idxMaster].loginTp then
    begin
      bChanged := True;
      if oldDataYn <> 'Y' then
        bSound := true;
    end;

  end;

  if (bChanged=True) and (bFirstData=False) then
  Begin
    m_arrMasters[idxMaster].loginTp := loginTp;
    pnlLoginTp.Caption              := _LoginTp(loginTp);
    lblLastLogonoffTime.Caption     := Tm;

    cbMasterIDs.ItemIndex := idxMaster;

    pnlLoginTp.Color      := clred;
    pnlLoginTp.Font.Color := clyellow;

    tmrFlicker.Interval := 30000;
    tmrFlicker.Enabled  := True;


  End;

  if loginTp='I' then
    AddMsg(format('[Log On]%s', [masterId]))
  else
    AddMsg(format('[Log Off]%s', [masterId]));


  if bSound then
  begin
    if loginTp='I' then
      alarmTp := TP_LOGON
    else
      alarmTp := TP_LOGOFF;

    __ShowAlarm(alarmTp, masterId);
  end;

end;



function _OrdTp(tp:string):string;
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



function _Side(side:string):string;
begin

  Result := '매수';
  if side='S' then
    Result := '매도';
end;

procedure TfmMain.Proc_Cntr(bHistory:boolean);
var
  //oldDataYn  : string;
  masterId  : string;
  cntrNo  : string;
  stkCd   : string;
  bsTp    : string;
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

  i       : integer;
  rowcnt  : integer;
  bSound  : boolean;

  alarmTp : integer;
  idxMaster : integer;

  nColorChanged : integer;

  sSideExplain, sTypeExplain, sClrTpExplain : string;
begin

  masterId  := UPPERCASE( Trim(copy(m_sRcvPack, 4,20)));
  cntrNo  := Trim(copy(m_sRcvPack, 24,5));
  stkCd   := Trim(copy(m_sRcvPack, 29,10));
  bsTp    := copy(m_sRcvPack, 39,1);
  cntrQty := Trim(copy(m_sRcvPack, 40,3));
  cntrPrc := Trim(copy(m_sRcvPack, 43,10));
  clrPl   := Trim(copy(m_sRcvPack, 53,10));
  cmsn    := Trim(copy(m_sRcvPack, 63,5));
  clrTp   := copy(m_sRcvPack, 68, 1);
  bf_nclrQty  := Trim(copy(m_sRcvPack, 69,3));
  af_nclrQty  := Trim(copy(m_sRcvPack, 72,3));
  bf_avgPrc   := Trim(copy(m_sRcvPack, 75,10));
  af_avgPrc   := Trim(copy(m_sRcvPack, 85,10));
  bf_amt      := Trim(copy(m_sRcvPack, 95,10));
  af_amt      := Trim(copy(m_sRcvPack, 105,10));
  ordTp       := Trim(copy(m_sRcvPack, 115,2));
  tradeTm     := Trim(copy(m_sRcvPack, 117,12));
  lvg         := Trim(copy(m_sRcvPack, 129,2));

  idxMaster := GetIdxOfMasterId(masterId);
                                
  sSideExplain  := _Side(bsTp);
  sTypeExplain  := _OrdTp( ordTp);
  sClrTpExplain := _ClrTp(clrTp);
  
  if IsMasterFixed() then
  begin
    if masterId <> cbMasterIDs.Items[cbMasterIDs.ItemIndex] then
    begin
      AddMsg(format('[%s] ID고정 사용중이므로 체결 처리하지 않음', [masterId]));
      exit;
    end;
  end;

  
  if bHistory = FALSE then
  begin
    if strtoint(edCntrNo.Text)<>strtoint(cntrno) then
      bSound := true;    

     // mark the items changed
    nColorChanged := 0;
    if edMasterId.Text <> masterID then
    begin
      edMasterId.Color := clBlack;
      edMasterId.Font.Color := clYellow;
      nColorChanged := nColorChanged + 1;
    end;
    if edStk.Text <> stkcd then
    begin
      edStk.Color := clBlack;
      edStk.Font.Color := clYellow;
      nColorChanged := nColorChanged + 1;
    end;
    if edside.Text <> sSideExplain then
    begin
      edside.Color := clBlack;
      edside.Font.Color := clYellow;
      nColorChanged := nColorChanged + 1;
    end;
    if strtointdef(edCntrQty.Text, 0) <> strtoint(cntrQty) then
    begin
      edCntrQty.Color := clBlack;
      edCntrQty.Font.Color := clYellow;
      nColorChanged := nColorChanged + 1;
    end;
    if edClrTp.Text <> sClrTpExplain then
    begin
      edClrTp.Color := clBlack;
      edClrTp.Font.Color := clYellow;
      nColorChanged := nColorChanged + 1;
    end;

    if nColorChanged>0 then
    begin
      tmrFlicker.Interval := 30000;
      tmrFlicker.Enabled  := True;
    end;


    edMasterId.Text := masterId;
    edCntrTm.Text := tradeTm;
    edCntrNo.Text :=  cntrNo;
    edStk.Text    :=  stkCd;
    edSide.Text   :=  sSideExplain;
    edCntrQty.Text:=  cntrQty;
    edCntrPrc.Text:= cntrPrc ;
    edpl.Text     := clrPl;
    edCmsn.Text   :=  cmsn;
    edClrTp.Text  :=  sClrTpExplain;
    edBfQty.Text  :=  bf_nclrQty;
    edAfQty.Text  :=  af_nclrQty;
    edBfAvg.Text  :=  bf_avgPrc;
    edAfAvg.Text  := af_avgPrc;
    edType.Text   := sTypeExplain;
    edCntrTm.Text := tradeTm;
    edLvg.Text    := lvg;

    m_arrMasters[idxMaster].nLastCntrNo := strtoint(cntrNo);

    AddMsg(format('[체결수신](%s)(번호:%s)',[masterId, cntrNo]));

  end; // if bHistory = FALSE then

  
  if bSound then
  begin
    if bsTp='S' then alarmTp := TP_SELL;
    if bsTp='B' then alarmTp := TP_BUY;

    __ShowAlarm(alarmTp, masterId);
  end;


  //
  // grid
  //

  //if m_arrMasters[idxMaster].nLastCntrNo >= strtoint(cntrNo) then
  //  exit;

  rowcnt := gdCntr.RowCount;

  if rowcnt=2 then
  begin
    if gdCntr.Cells[I_CNTR_NO, 1]<>''  then
    begin
      gdCntr.InsertRows(1, 1);
      rowcnt := 2;
    end
    else
      rowcnt := 1;
  end
  else
  begin
    gdCntr.InsertRows(1, 1);
    rowcnt := rowcnt + 1;
  end;

  gdCntr.Cells[I_NO, 1]         := INTTOSTR(rowcnt);
  gdCntr.Cells[I_MASTER_ID, 1]  := masterId;
  gdCntr.Cells[I_CNTR_NO, 1]  := cntrNo;
  gdCntr.Cells[I_TM, 1]       := tradeTm;
  gdCntr.Cells[I_STK, 1]      := stkCd;
  gdCntr.Cells[I_SIDE, 1]     := sSideExplain;
  gdCntr.Cells[I_QTY, 1]      := cntrQty;
  gdCntr.Cells[I_PRC, 1]      := cntrPrc;
  gdCntr.Cells[I_TYPE, 1]     := sTypeExplain;
  gdCntr.Cells[I_CLR_TP, 1]   := sClrTpExplain;
  gdCntr.Cells[I_PL, 1]       := clrPl;
  gdCntr.Cells[I_CMSN, 1]     := cmsn;
  gdCntr.Cells[I_BF_QTY, 1] := bf_nclrQty;
  gdCntr.Cells[I_AF_QTY, 1] := af_nclrQty;
  gdCntr.Cells[I_BF_AVG, 1] := bf_avgPrc;
  gdCntr.Cells[I_AF_AVG, 1] := af_avgPrc;
  gdCntr.Cells[I_LVG, 1]    := lvg;


end;


procedure TfmMain.AppExceptionHandler(sender:TObject; E:Exception);  
begin
  showmessage('AppException');
  application.Terminate;
  //Application.ShowException(E);

    
end;


                                      
procedure TfmMain.btnCntrHistClick(Sender: TObject);
var
  sMasterId : string;
  sPacket   : string;
begin

  if m_bPwdOk=False then
  begin
    showmessage('먼저 비밀번호 승인을 해주세요');
    exit;
  end;

  if cbMasterIDs.ItemIndex=-1 then
  begin
    showmessage('조회할 ID 를 먼저 선택해 주세요');
    cbMasterIDs.SetFocus;
    exit;
  end;

  ClearCntrGrid;

  m_arrMasters[cbMasterIDs.ItemIndex].nLastCntrNo := 0;

  sPacket := CODE_CNTR_HIST + Trim(cbMasterIDs.Items[cbMasterIDs.ItemIndex]);

//  if IdTCPClient1.Connected=False then
//  begin
//    showmessage('서버와의 접속 오류. 재접속 하세요');
//    exit;
//  end;

  SendData(sPacket, sPacket.Length);

  AddMsg('체결내역 요청 전송...');

end;

procedure TfmMain.SendData(sData:string; len:integer);
begin

  try
    IdTCPClient1.IOHandler.Write(sdata);
  except
    __ShowWarning('통시오류', '데이터전송 실패');
    exit;
  end;

end;

end.
