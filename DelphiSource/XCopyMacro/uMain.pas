unit uMain;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants,  Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs
  , XAlphaPacket, sSkinProvider, sSkinManager,
  Vcl.StdCtrls, sComboBox, Vcl.ExtCtrls, sPanel, CommonUtils,
  AdvUtil, Vcl.Grids, AdvObj, BaseGrid, AdvGrid, Vcl.Buttons,
  uNotify, ThdTimer,
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdThreadComponent,
  IdExceptionCore, uCommonDef, uPickOrdButton, IdGlobal, Idexception
  ,VCL.Themes // vcl hooks
  ,uOrdThrd, uPacketThrd, uRecvThrd, System.Classes, uTickThrd, uPrcList, MTLoggerU
  ;

//const


type

  TMasterInfo = record
    sID           : string;
    loginTp       : string;  // I, O
    nLastCntrNo   : integer;
  end;


  TSettings = record
    master    : ^TComboBox;
    masterID  : ^TEdit;
    artcNm    : ^TComboBox;
    artcCd    : ^TEdit;
    rvs       : ^TCheckBox;
    macro     : ^TCheckBox;
    buyx      : ^TEdit;
    buyy      : ^TEdit;
    sellx     : ^TEdit;
    selly     : ^TEdit;
    clrx      : ^TEdit;
    clry      : ^TEdit;
    //ticksize : ^TEdit;
    tickval  : ^TEdit;
    qty      : ^TEdit;
    scalping : ^TComboBox;
    addPos   : ^TCheckBox;
    clrYN    : ^TCheckBox;
  end;

  type
  TEditStyleHookColor = class(TEditStyleHook)
  private
    procedure UpdateColors;
  protected
    procedure WndProc(var Message: TMessage); override;
  public
    constructor Create(AControl: TWinControl); override;
  end;


  TfmMain = class(TForm)
    pnlBottom: TsPanel;
    cbMsg: TsComboBox;
    pnlTop: TPanel;
    cbMasterIDs: TComboBox;
    pnlLoginTp: TPanel;
    btnConn: TButton;
    lblLastLogonoffTime: TLabel;
    idTcpOrd: TIdTCPClient;
    tmrSetIDCombo: TThreadedTimer;
    tmrUnMarkChange: TThreadedTimer;
    edPwd: TEdit;
    tmrTryConn: TTimer;
    Label16: TLabel;
    IdTcpTick: TIdTCPClient;
    sbBgSetting: TScrollBox;
    GroupBox2: TGroupBox;
    Label10: TLabel;
    Label12: TLabel;
    Label42: TLabel;
    edbuyx1: TEdit;
    edbuyy1: TEdit;
    edsellx1: TEdit;
    edselly1: TEdit;
    edclrx1: TEdit;
    edclry1: TEdit;
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    cbStk1: TComboBox;
    cbMasterId1: TComboBox;
    chkRvs1: TCheckBox;
    edArtc1: TEdit;
    GroupBox5: TGroupBox;
    Label20: TLabel;
    Label22: TLabel;
    Label46: TLabel;
    edbuyx3: TEdit;
    edbuyy3: TEdit;
    edsellx3: TEdit;
    edselly3: TEdit;
    edclrx3: TEdit;
    edclry3: TEdit;
    Button7: TButton;
    Button8: TButton;
    Button9: TButton;
    cbStk3: TComboBox;
    cbMasterId3: TComboBox;
    chkRvs3: TCheckBox;
    edArtc3: TEdit;
    GroupBox3: TGroupBox;
    Label15: TLabel;
    Label17: TLabel;
    Label45: TLabel;
    edbuyx2: TEdit;
    edbuyy2: TEdit;
    edsellx2: TEdit;
    edselly2: TEdit;
    edclrx2: TEdit;
    edclry2: TEdit;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    cbStk2: TComboBox;
    cbMasterId2: TComboBox;
    chkRvs2: TCheckBox;
    edArtc2: TEdit;
    sbBgCntr: TScrollBox;
    gdCntrMaster: TAdvStringGrid;
    gdCntrMine: TAdvStringGrid;
    pnlSettingSummary: TPanel;
    chkMacroOrd1: TCheckBox;
    chkMacroOrd2: TCheckBox;
    chkMacroOrd3: TCheckBox;
    btnShowSetting: TButton;
    btnShowPrcGrid: TButton;
    btnShowPos: TButton;
    btnShowCntr: TButton;
    Panel10: TPanel;
    Label3: TLabel;
    Label7: TLabel;
    Label1: TLabel;
    Label2: TLabel;
    Label6: TLabel;
    Label5: TLabel;
    Label4: TLabel;
    Label34: TLabel;
    edClrTp: TEdit;
    edMasterId: TEdit;
    edStk: TEdit;
    edSide: TEdit;
    edCntrPrc: TEdit;
    edCntrTm: TEdit;
    edCntrQty: TEdit;
    edCntrNo: TEdit;
    btnCntrHist: TButton;
    sbBgPos: TPanel;
    gdPosMine: TAdvStringGrid;
    gdPosMaster: TAdvStringGrid;
    gdDiff: TAdvStringGrid;
    btnTS1: TButton;
    lbsTSstatus1: TLabel;
    btnTS3: TButton;
    lbsTSstatus3: TLabel;
    Label32: TLabel;
    cbPosSetQty1: TComboBox;
    Label35: TLabel;
    cbPosSetSide1: TComboBox;
    Label30: TLabel;
    cbPosSetQty2: TComboBox;
    Label37: TLabel;
    cbPosSetSide2: TComboBox;
    Label36: TLabel;
    cbPosSetQty3: TComboBox;
    Label41: TLabel;
    cbPosSetSide3: TComboBox;
    btnPosSet1: TButton;
    Label38: TLabel;
    edqty1: TEdit;
    Label29: TLabel;
    edtickval1: TEdit;
    Label39: TLabel;
    Label31: TLabel;
    edtickval2: TEdit;
    edqty2: TEdit;
    Label40: TLabel;
    edqty3: TEdit;
    edtickval3: TEdit;
    Label33: TLabel;
    Label28: TLabel;
    lbsTSstatus2: TLabel;
    btnTS2: TButton;
    lblCntrMaster: TLabel;
    lblCntrMine: TLabel;
    edMasterID_1: TEdit;
    edMasterID_2: TEdit;
    edMasterID_3: TEdit;
    Label19: TLabel;
    cbScalping1: TComboBox;
    cbScalping2: TComboBox;
    Label21: TLabel;
    cbScalping3: TComboBox;
    Label8: TLabel;
    edtDebug: TEdit;
    cbSignalStk1: TComboBox;
    cbSignalUpDown1: TComboBox;
    edtSignalPrc1: TEdit;
    cbSignalUpDown2: TComboBox;
    edtSignalPrc2: TEdit;
    cbSignalStk2: TComboBox;
    cbSignalUpDown3: TComboBox;
    edtSignalPrc3: TEdit;
    cbSignalStk3: TComboBox;
    tmrSignalAlarm: TThreadedTimer;
    chPopup: TCheckBox;
    chkMute1: TCheckBox;
    chkAddPos1: TCheckBox;
    chkAddPos2: TCheckBox;
    chkAddPos3: TCheckBox;
    Label9: TLabel;
    edtCalcPrcBase: TEdit;
    Label11: TLabel;
    cbCalcPrc: TComboBox;
    edtCalcPrcTick: TEdit;
    edtCalcPrcH: TEdit;
    edtCalcPrcL: TEdit;
    btnCalcPrc: TButton;
    Label13: TLabel;
    Label14: TLabel;
    edtTicker: TEdit;
    tmrTicker: TThreadedTimer;
    chkClrYN1: TCheckBox;
    chkClrYN2: TCheckBox;
    chkClrYN3: TCheckBox;



    function  Init_PacketThread():boolean;
    procedure InitComponents();
    
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);

    procedure AddMsg(msg:string; bSiren:boolean=false; bShow:boolean=false );
    procedure btnConnClick(Sender: TObject);

    function EndMainByDisconn():boolean;
    procedure btnCntrHistClick(Sender: TObject);
    procedure cbMasterIDsChange(Sender: TObject);
    procedure tmrSetIDComboTimer(Sender: TObject);


    procedure AppExceptionHandler(sender:TObject; E:Exception);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure idTcpOrdConnected(Sender: TObject);
    procedure idTcpOrdDisconnected(Sender: TObject);
    //procedure btnCheckClick(Sender: TObject);
    procedure edPwdKeyPress(Sender: TObject; var Key: Char);
    procedure tmrUnMarkChangeTimer(Sender: TObject);
    procedure GroupBox1Click(Sender: TObject);
    //procedure tmrInitTimer(Sender: TObject);
    procedure gdPosMineGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdPosMasterGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdDiffGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdCntrMineGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdCntrMasterGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure tmrTryConnTimer(Sender: TObject);
//    procedure btnSaveSettingsClick(Sender: TObject);
//    procedure chkSetting1Click(Sender: TObject);
//    procedure chkSetting2Click(Sender: TObject);
//    procedure chkSetting3Click(Sender: TObject);
    procedure gdCntrMasterGetCellColor(Sender: TObject; ARow, ACol: Integer;
      AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
    procedure gdCntrMineGetCellColor(Sender: TObject; ARow, ACol: Integer;
      AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
    procedure gdPosMineGetCellColor(Sender: TObject; ARow, ACol: Integer;
      AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
    procedure gdPosMasterGetCellColor(Sender: TObject; ARow, ACol: Integer;
      AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
    procedure gdDiffGetCellColor(Sender: TObject; ARow, ACol: Integer;
      AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure Button9Click(Sender: TObject);
    procedure Button8Click(Sender: TObject);
    procedure Button7Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    //procedure cbStk1Change(Sender: TObject);
    procedure cbMasterId1Change(Sender: TObject);
    procedure cbMasterId2Change(Sender: TObject);
    procedure cbMasterId3Change(Sender: TObject);
    procedure cbStk1Change(Sender: TObject);
    procedure cbStk2Change(Sender: TObject);
    procedure cbStk3Change(Sender: TObject);
    procedure IdTcpTickConnected(Sender: TObject);
    procedure btnPosSet1Click(Sender: TObject);
    //procedure btnPosSet2Click(Sender: TObject);
    //procedure btnPosSet3Click(Sender: TObject);
    procedure btnShowPrcGridClick(Sender: TObject);
    procedure btnShowSettingClick(Sender: TObject);
    procedure btnTS1Click(Sender: TObject);
    procedure btnTS2Click(Sender: TObject);
    procedure btnTS3Click(Sender: TObject);
    procedure chkMacroOrd1Click(Sender: TObject);
    procedure chkMacroOrd2Click(Sender: TObject);
    procedure chkMacroOrd3Click(Sender: TObject);
    procedure cbSignalStk1Change(Sender: TObject);
    procedure cbSignalStk2Change(Sender: TObject);
    procedure cbSignalStk3Change(Sender: TObject);
    procedure edtSignalPrc1Click(Sender: TObject);
    procedure edtSignalPrc2Click(Sender: TObject);
    procedure edtSignalPrc3Click(Sender: TObject);
    procedure tmrSignalAlarmTimer(Sender: TObject);
    procedure edtCalcPrcTickClick(Sender: TObject);
    procedure btnCalcPrcClick(Sender: TObject);

    procedure edtCalcPrcTickKeyPress(Sender: TObject; var Key: Char);
    procedure tmrTickerTimer(Sender: TObject);
//    procedure btnShowPosClick(Sender: TObject);
//    procedure btnShowCntrClick(Sender: TObject);


  public
    procedure Proc_Packet();
    procedure PosGrid_InsertStk(idx:integer; bReset:boolean);
    procedure PosGrid_Clear(idx:integer);

    function  IsScalping(idx:integer):boolean;
    procedure EnableSignalAlarmTimer();

    function  IsAllowedClr(idx:integer):boolean;

  private
    { Private declarations }

    procedure WndProc(var Message:TMessage);override;

    procedure Proc_LogOnOff();

    procedure Proc_Cntr(bHistory:boolean);
    procedure MasterOrd_Parsing();
    procedure MasterOrd_MarkChanged();
    procedure UnMark_MasterDataChanged();


    procedure Proc_Msg();

    function  GetIdxOfMasterId(sMasterID:string):integer;
    //function  IsExistsID(sMasterID:string):boolean;

    procedure Clear_MasterCntrGrid();
    //function  GetFixedIdx():integer;

    //function  ValidateSettings():boolean;



  public
    m_bPwdOk      : boolean;
    m_bTerminate  : boolean;
    m_sRcvPack    : string;
    m_setting     : array [1..MAX_STK] OF TSettings;
    m_rcv         : TMASTER_ORD;

  private
    m_arrMasters        : array[0..MAX_MASTERS_CNT-1] of TMasterInfo;
    m_nCurrMastersCnt   : integer;
    m_currComboIdx      : integer;

    ////////////////////////////////////////////////////////
    ///  Order Thread 관련
private
    //m_ordThrd       : array[1..MAX_STK] of TOrdThrd;
private
    procedure CreateOrdThreads();
    procedure PostMsg_OrdThrds();

    ////////////////////////////////////////////////////////
    // 보이고안보이고
    procedure ShowHideBg();
    procedure ShowHideTick();

private
    m_bShowTick     : boolean;
    m_bShowSetting  : boolean;
    m_log           : TMTLogger;
    tmrSignalAlarmCnt : integer;

    ////////////////////////////////////
    ///  Master 주문 수신 Socket 관련
  private
    function  Connect_OrdSvr():boolean;
    procedure SendData_OrdSvr(sData:string; len:integer);

    ////////////////////////////////////
    ///  시세 수신 Socket 관련
  private
    function  Connect_TickSvr():boolean;

    ///////////////////////////////////////////
    ///  mouse hooking관련
  private
    function InstallMouseHook: Boolean;
  public
    MouseHookHandle : hHook;

    // posgrid index
    //function  GetIdxOfPosGrid(artc:string):integer;

  private



    
  end;

var
  fmMain: TfmMain;
  __CnfgName  : string;

implementation

uses
  uPrcGrid, uControlSize, uTrailingStop, uSettingTS, uPos_Manually, uSignal;

type
  TWinControlH= class(TWinControl);

var
  _bMarkChanged : boolean;

{$R *.dfm}


// MOUSE HOOKING
function LowLevelMouseProc(nCode: Integer; wParam: wParam; lParam: lParam): LRESULT; stdcall;
begin

  Result := CallNextHookEx(fmMain.MouseHookHandle, nCode, wParam, lParam);

//  if wParam = WM_LButtonDOWN then
//  begin
//    GetCursorPos(pt);
//
//    if fmMain.chkSetting1.checked then
//    begin
//      fmMain.edx
//    end;
//
//    //fmMain.Edit1.Text := STRTOINT(pt.X);
////    fmMain.Edit2.Text := IntToStr(pt.Y);
//  end;


end;



function  TfmMain.IsAllowedClr(idx:integer):boolean;
begin
  Result := m_setting[idx].clrYN.Checked;
end;

function TfmMain.InstallMouseHook : Boolean;
begin
  Result := False;
  if fmMain.MouseHookHandle = 0 then
  begin
    fmMain.MouseHookHandle := SetWindowsHookEx(WH_MOUSE_LL, @LowLevelMouseProc, hInstance, 0);
    Result := fmMain.MouseHookHandle <> 0;
    if Result = FALSE then
    begin
      ShowMessage('Mouse Hook not installed, mouse tracking functionality disabled !');
    end;
  end;
end;

function  TfmMain.Connect_OrdSvr():boolean;
var
  sPort : string;
  sIp   : string;
begin

  Result := True;

  __CnfgName := __Get_CFGFileName();

  sIp    := __Get_CFGFile('SERVER', 'ORD_IP', '', False, __CnfgName);
  sPort  := __Get_CFGFile('SERVER', 'ORD_PORT', '', false, __CnfgName);

  if (sIp='') or (strtointdef(sPort,0)=0) then
  begin
    showmessage('SERVER 정보 없음');
    Result := False;
    exit;
  end;

  idTcpOrd.ReadTimeout := 100;  //millisecond
  idTcpOrd.Host  := sIp;
  idTcpOrd.Port  := strtointdef(sPort,0);

  if idTcpOrd.Connected=false then
    idTcpOrd.Connect;

end;


function  TfmMain.Connect_TickSvr():boolean;
var
  sPort : string;
  sIp   : string;
begin

  Result := True;

  __CnfgName := __Get_CFGFileName();

  sIp    := __Get_CFGFile('SERVER', 'TICK_IP', '', False, __CnfgName);
  sPort  := __Get_CFGFile('SERVER', 'TICK_PORT', '', false, __CnfgName);

  if (sIp='') or (strtointdef(sPort,0)=0) then
  begin
    showmessage('Tick SERVER 정보 없음');
    Result := False;
    exit;
  end;

  idTcpTick.ReadTimeout := 100;  //millisecond
  idTcpTick.Host  := sIp;
  idTcpTick.Port  := strtoint(sPort);

  AddMsg('Trying to connect Tick server');
  if idTcpTick.Connected=false then
    idTcpTick.Connect;

end;

procedure TfmMain.edPwdKeyPress(Sender: TObject; var Key: Char);
begin
if key = #13 then
  begin
    if length( trim(edPwd.Text) )>0 then
      btnConnClick(sender);
  end;
end;

procedure TfmMain.edtCalcPrcTickClick(Sender: TObject);
begin
  edtCalcPrcTick.text := '';

end;


procedure TfmMain.edtCalcPrcTickKeyPress(Sender: TObject; var Key: Char);
begin

  if key = #13 then
    btnCalcPrc.OnClick(sender);
end;

procedure TfmMain.edtSignalPrc1Click(Sender: TObject);
begin
   edtSignalPrc1.Text := '';
end;

procedure TfmMain.edtSignalPrc2Click(Sender: TObject);
begin
edtSignalPrc2.Text := '';
end;

procedure TfmMain.edtSignalPrc3Click(Sender: TObject);
begin
edtSignalPrc3.Text := '';
end;

//FUNCTION  TfmMain.IsMasterFixed():boolean;
//begin
//  Result := (chkFixMaster.Checked = True);
//end;


//function  TfmMain.GetFixedIdx():integer;
//begin
//  Result := cbMasterIDs.ItemIndex;
//end;
//

//
procedure TfmMain.UnMark_MasterDataChanged();
begin

  _bMarkChanged := false;

//  pnlLoginTp.color  := clWhite;
//  pnlLoginTp.Font.Color := clblack;
//
//  edMasterId.Color  := clwhite;
//  edMasterId.Font.Color := clblack;
//
//  edStk.Color       := clwhite;
//  edStk.Font.Color := clblack;
//
//  edSide.Color      := clwhite;
//  edSide.Font.Color := clblack;
//
//  edCntrQty.Color   := clwhite;
//  edCntrQty.Font.Color := clblack;
//
//  edClrTp.Color   := clwhite;
//  edClrTp.Font.Color := clblack;
end;


procedure TfmMain.btnConnClick(Sender: TObject);
var
  sPacket : string;
begin

  sPacket := CODE_PWD + edPwd.text;

  if idTcpOrd.Connected=False then
  begin
    //Connect_OrdSvr;
    showmessage('서버와 연결되지 않았습니다. 잠시 후 다시 시도하세요');
    exit;
  end;

  SendData_OrdSvr(sPacket, sPacket.Length);

  AddMsg('비밀번호 승인 전송');
end;


procedure TfmMain.btnPosSet1Click(Sender: TObject);
begin

  __ShowPosInputBox();

end;
//
//procedure TfmMain.btnPosSet2Click(Sender: TObject);
//VAR
//  idx : integer;
//begin
//
//  idx := 2;
//
//  gdPosMine.cells[POS_MASTER,  idx] := m_setting[idx].master.Text;
//  gdPosMine.cells[POS_ARTC,  idx] := m_setting[idx].artcCd.Text;
//  gdPosMine.cells[POS_STATUS,idx] := POS_STATUS_NONE;
//  gdPosMine.cells[POS_SIDE,  idx] := '';
//  gdPosMine.cells[POS_AVG,   idx] := '0';
//  gdPosMine.cells[POS_QTY,   idx] := '0';
//  gdPosMine.cells[POS_TM,    idx] := '';
//  gdPosMine.cells[POS_TS_LVL_1, idx] := '';
//  gdPosMine.cells[POS_TS_LVL_2, idx] := '';
//  gdPosMine.cells[POS_TS_LVL_3, idx] := '';
//  gdPosMine.cells[POS_TS_BEST,  idx] := '';
//  gdPosMine.cells[POS_SL_PRC,   idx] := '';
//
//
//  gdPosMaster.cells[POS_ARTC,  idx] := m_setting[idx].artcCd.Text;
//  gdPosMaster.cells[POS_STATUS,idx] := POS_STATUS_NONE;
//  gdPosMaster.cells[POS_SIDE,  idx] := '';
//  gdPosMaster.cells[POS_AVG,   idx] := '0';
//  gdPosMaster.cells[POS_QTY,   idx] := '0';
//  gdPosMaster.cells[POS_TM,    idx] := '';
//end;
//
//procedure TfmMain.btnPosSet3Click(Sender: TObject);
//VAR
//  idx : integer;
//begin
//
//  idx := 3;
//
//  gdPosMine.cells[POS_MASTER,  idx] := m_setting[idx].master.Text;
//  gdPosMine.cells[POS_ARTC,  idx] := m_setting[idx].artcCd.Text;
//  gdPosMine.cells[POS_STATUS,idx] := POS_STATUS_NONE;
//  gdPosMine.cells[POS_SIDE,  idx] := '';
//  gdPosMine.cells[POS_AVG,   idx] := '0';
//  gdPosMine.cells[POS_QTY,   idx] := '0';
//  gdPosMine.cells[POS_TM,    idx] := '';
//  gdPosMine.cells[POS_TS_LVL_1, idx] := '';
//  gdPosMine.cells[POS_TS_LVL_2, idx] := '';
//  gdPosMine.cells[POS_TS_LVL_3, idx] := '';
//  gdPosMine.cells[POS_TS_BEST,  idx] := '';
//  gdPosMine.cells[POS_SL_PRC,   idx] := '';
//
//
//  gdPosMaster.cells[POS_ARTC,  idx] := m_setting[idx].artcCd.Text;
//  gdPosMaster.cells[POS_STATUS,idx] := POS_STATUS_NONE;
//  gdPosMaster.cells[POS_SIDE,  idx] := '';
//  gdPosMaster.cells[POS_AVG,   idx] := '0';
//  gdPosMaster.cells[POS_QTY,   idx] := '0';
//  gdPosMaster.cells[POS_TM,    idx] := '';
//end;


procedure TfmMain.btnShowPrcGridClick(Sender: TObject);
begin
  m_bShowTick := not m_bShowTick;
  ShowHideTick();
end;


procedure TfmMain.btnShowSettingClick(Sender: TObject);
begin

  m_bShowSetting := not m_bShowSetting;

  ShowHideBg();

end;


procedure TfmMain.btnTS1Click(Sender: TObject);
begin
  if (m_setting[1].master.ItemIndex=0) or (m_setting[1].artcNm.ItemIndex=-1) then
  begin
    showmessage('ID와 종목을 먼저 선택하세요');
    exit;
  end;

  __CreateTSsetting(btnTS1.Left, btnTS1.Top, true);
end;

procedure TfmMain.btnTS2Click(Sender: TObject);
begin
  __CreateTSsetting(btnTS2.Left, btnTS2.Top, true);
end;

procedure TfmMain.btnTS3Click(Sender: TObject);
begin
  __CreateTSsetting(btnTS3.Left, btnTS3.Top, true);

end;


procedure TfmMain.PosGrid_Clear(idx:integer);
VAR
  dwRslt : DWORD;
begin

  SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_POSCLR,
                              wParam(idx),
                              Lparam(0),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );

end;


procedure TfmMain.PosGrid_InsertStk(idx:integer; bReset:boolean);
var
  itemidx   : integer;
  newMaster : string;
  newArtc   : string;
  nSameCnt  : integer;
  i         : integer;
  itemPos   : TItemPos;
  dwRslt    : DWORD;
begin

  if m_setting[idx].master.ItemIndex=0 then
  begin
    PosGrid_Clear(idx);
    exit;
  end;

  newMaster := m_setting[idx].master.Items[m_setting[idx].master.ItemIndex];

  itemidx     := m_setting[idx].artcNm.ItemIndex;
  newArtc := __Artc(m_setting[idx].artcNm.Items[itemidx]);

  // 이미 같은 조합이 있는지 점검한다.
  nSameCnt := 0;
  for i := 1 to MAX_STK do
  BEGIN
    if i = idx then
      continue;

    if (gdPosMine.Cells[POS_MASTER, i]=newMaster) and
       (gdPosMine.Cells[POS_ARTC, i]=newArtc)     then
    begin
      nSameCnt := nSameCnt + 1;
    end;

  END;

  if nSameCnt > 0 then
  begin
    Addmsg('이미 같은 조합이 존재합니다.', true);
    m_setting[idx].master.ItemIndex := 0;
    m_setting[idx].macro.Caption := '';
    exit;
  end;


  m_setting[idx].macro.Caption  := format('[%s-%s]Copy', [newMaster, newArtc]);


  m_setting[idx].masterID.Text := newMaster;
  m_setting[idx].artcCd.Text := newArtc;


  if bReset then
    PosGrid_Clear(idx);

  itemPos := TItemPos.Create;
  itemPos.idx       := idx;
  itemPos.masterId  := m_setting[idx].master.Items[m_setting[idx].master.ItemIndex];
  itemPos.artc      := m_setting[idx].artcCd.Text;

  SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_POSINSERT,
                              wParam(LongInt(sizeof(ItemPos))),
                              Lparam(LongInt(ItemPos)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );


  gdPosMaster.Cells[POS_ARTC, idx]    := m_setting[idx].artcCd.Text;
  gdPosMaster.Cells[POS_STATUS, idx]  := POS_STATUS_NONE;
  gdPosMaster.Cells[POS_QTY, idx]     := '0';
  gdPosMaster.Cells[POS_AVG, idx]     := '0';
  gdPosMaster.Cells[POS_TM,  idx]     := '';


end;

procedure TfmMain.Button1Click(Sender: TObject);
begin

  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edbuyx1.Text := inttostr(__hookX);
    edbuyy1.Text := inttostr(__hookY);
  end;
end;

procedure TfmMain.Button2Click(Sender: TObject);
begin
  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edsellx1.Text := inttostr(__hookX);
    edselly1.Text := inttostr(__hookY);
  end;
end;

procedure TfmMain.Button3Click(Sender: TObject);
begin
  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edclrx1.Text := inttostr(__hookX);
    edclry1.Text := inttostr(__hookY);
  end;
end;

procedure TfmMain.Button4Click(Sender: TObject);
begin
  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edbuyx2.Text := inttostr(__hookX);
    edbuyy2.Text := inttostr(__hookY);
  end;

end;

procedure TfmMain.Button5Click(Sender: TObject);
begin
  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edsellx2.Text := inttostr(__hookX);
    edselly2.Text := inttostr(__hookY);
  end;

end;

procedure TfmMain.Button6Click(Sender: TObject);
begin
  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edclrx2.Text := inttostr(__hookX);
    edclry2.Text := inttostr(__hookY);
  end;
end;

procedure TfmMain.Button7Click(Sender: TObject);
begin
  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edbuyx3.Text := inttostr(__hookX);
    edbuyy3.Text := inttostr(__hookY);
  end;

end;

procedure TfmMain.Button8Click(Sender: TObject);
begin
  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edsellx3.Text := inttostr(__hookX);
    edselly3.Text := inttostr(__hookY);
  end;
end;

procedure TfmMain.Button9Click(Sender: TObject);
begin
  __hookX := 0;
  __hookY := 0;

  __PickButton();

  if (__hookX>0) and (__hookY>0) then
  begin
    edclrx3.Text := inttostr(__hookX);
    edclry3.Text := inttostr(__hookY);
  end;
end;


procedure TfmMain.cbMasterId1Change(Sender: TObject);
var
  newID : string;
  idx : integer;
begin

  idx := 1;

  if m_setting[idx].master.ItemIndex=0 then
  begin
    m_setting[idx].masterID.text := '';
    PosGrid_Clear(idx);
    m_setting[idx].macro.Caption := '';
    exit;
  end;


  newID := m_setting[idx].master.Items[m_setting[idx].master.ItemIndex];
  if (newID = m_setting[idx].masterID.text) then
    exit;

  PosGrid_InsertStk(idx,true);
end;

procedure TfmMain.cbMasterId2Change(Sender: TObject);
var
  newID : string;
  idx : integer;
begin

  idx := 2;

  if m_setting[idx].master.ItemIndex=0 then
  begin
    m_setting[idx].masterID.text := '';
    PosGrid_Clear(idx);
    m_setting[idx].macro.Caption := '';
    exit;
  end;

  newID := m_setting[idx].master.Items[m_setting[idx].master.ItemIndex];
  if (newID = m_setting[idx].masterID.text) then
    exit;

  PosGrid_InsertStk(idx,true);
end;

procedure TfmMain.cbMasterId3Change(Sender: TObject);
var
  newID : string;
  idx : integer;
begin

  idx := 3;

  if m_setting[idx].master.ItemIndex=0 then
  begin
    m_setting[idx].masterID.text := '';
    PosGrid_Clear(idx);
    m_setting[idx].macro.Caption := '';
    exit;
  end;


  newID := m_setting[idx].master.Items[m_setting[idx].master.ItemIndex];
  if (newID = m_setting[idx].masterID.text) then
    exit;

  PosGrid_InsertStk(idx,true);
end;

procedure TfmMain.cbMasterIDsChange(Sender: TObject);
begin

//  if IsMasterFixed() then
//  begin
//    if m_currComboIdx <> cbMasterIDs.ItemIndex then
//    begin
//      Showmessage('먼저 [ID고정]을 해제 하세요');
//      cbMasterIDs.ItemIndex :=  m_currComboIdx;
//      exit;
//    end;
//  end;

  pnlLoginTp.Caption := __LoginTp( m_arrMasters[cbMasterIds.ItemIndex].loginTp);

  m_currComboIdx := cbMasterIDs.ItemIndex;

  //Clear_MasterCntrGrid();
end;


procedure TfmMain.cbSignalStk1Change(Sender: TObject);
begin
  if cbSignalStk1.ItemIndex=0 then
  begin
    edtSignalPrc1.Text := '';
  end;

end;

procedure TfmMain.cbSignalStk2Change(Sender: TObject);
begin
  if cbSignalStk2.ItemIndex=0 then
  begin
    edtSignalPrc2.Text := '';
  end;
end;

procedure TfmMain.cbSignalStk3Change(Sender: TObject);
begin
  if cbSignalStk3.ItemIndex=0 then
  begin
    edtSignalPrc3.Text := '';
  end;
end;

procedure TfmMain.cbStk1Change(Sender: TObject);
var
  idx     : integer;
  newArtc : string;
begin

  idx     := m_setting[1].artcNm.ItemIndex;
  newArtc := __Artc(m_setting[1].artcNm.Items[idx]);
  if (newArtc = edArtc1.Text) then
    exit;

  PosGrid_InsertStk(1,true);

end;

procedure TfmMain.cbStk2Change(Sender: TObject);
var
  idx     : integer;
  newArtc : string;
begin

  idx     := m_setting[2].artcNm.ItemIndex;
  newArtc := __Artc(m_setting[2].artcNm.Items[idx]);
  if newArtc = edArtc2.Text then
    exit;

  PosGrid_InsertStk(2,true);
end;

procedure TfmMain.cbStk3Change(Sender: TObject);
var
  idx     : integer;
  newArtc : string;
begin

  idx     := m_setting[3].artcNm.ItemIndex;
  newArtc := __Artc(m_setting[3].artcNm.Items[idx]);
  if newArtc = edArtc3.Text then
    exit;

  PosGrid_InsertStk(3,true);
end;

procedure TfmMain.chkMacroOrd1Click(Sender: TObject);
begin
  __ts.Init_TS(1);
end;

procedure TfmMain.chkMacroOrd2Click(Sender: TObject);
begin
  __ts.Init_TS(2);
end;

procedure TfmMain.chkMacroOrd3Click(Sender: TObject);
begin
  __ts.Init_TS(3);
end;

//function  TfmMain.ValidateSettings():boolean;
//var
//  i : integer;
//
//begin
//
//  Result := False;
//
//  if (m_setting[1].master.ItemIndex = 0) and
//     (m_setting[2].master.ItemIndex = 0) and
//     (m_setting[3].master.ItemIndex = 0)
//  then
//  begin
//      AddMsg('ERR! Copy 할 ID 를 선택하세요');
//      exit;
//  end;
//
//  for i := 1 to MAX_STK do
//  BEGIN
//
//    if (m_setting[i].master.ItemIndex > 0) and
//       (m_setting[i].artcCd.Text = '') then
//    begin
//      AddMsg('ERR! Copy 할 종목을 선택하세요');
//      m_setting[i].artcCd.SetFocus;
//      exit;
//    end;
//
//    if m_setting[i].macro.Checked then
//    begin
//      if (m_setting[i].buyx.text='') or (m_setting[i].buyy.text='') or
//         (m_setting[i].sellx.text='') or (m_setting[i].selly.text='') or
//         (m_setting[i].clrx.text='') or (m_setting[i].clry.text='')
//      then
//      begin
//        AddMsg('ERR! Macro주문을 위한 좌표를 설정하세요.');
//        exit;
//      end;
//    end;
//
//  END;
//
//  Result := true;
//
//end;


procedure TfmMain.Clear_MasterCntrGrid();
var
  i : integer;
begin
  for i := gdCntrMaster.RowCount-1 downto 2 do
    gdCntrMaster.RemoveNormalRow(i);

  for i := 0 to gdCntrMaster.ColCount-1 do
     gdCntrMaster.Cells[i, 1] := '';
end;

procedure TfmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  // MOUSE HOOKING
  UnhookWindowsHookEx(fmMain.MouseHookHandle);

  if assigned(m_log) then
    m_log.SetStop;

end;

procedure TfmMain.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
begin
  m_bTerminate := True;

  if __tickThrd<>nil then
  begin
    __tickThrd.Terminate;
    FreeandNil(__tickThrd);
  end;

  if __packetThrd<>NIL then
  BEGIN
    __packetThrd.Terminate;
    FreeAndNil(__packetThrd);
  end;

  if __PacketQ<>nil then
    FreeAndNil(__PacketQ);

  if __ts<>nil then
    FreeAndNil(__ts);


end;


function TfmMain.Init_PacketThread:boolean;
begin
  __PacketQ     := CPacketQueue.Create;
  __packetThrd  := TPacketThrd.Create();
  __RcvThrd     := TRecvThrd.Create(true);
  __tickThrd    := TTickThrd.Create(true);
  __ts          := TTS.create();

  Result := True;
end;

procedure TfmMain.IdTcpTickConnected(Sender: TObject);
begin
  AddMsg('시세서버 connected!');
  // TICK 처리 스레드
  __tickThrd.Resume;
end;


procedure TfmMain.ShowHideBg();
var
  minus_setting : integer;

begin

  if m_bShowSetting then
  begin
    minus_setting  := 0;
    btnShowSetting.Caption := '설정숨김';
  end
  else
  begin
    minus_setting  := BG_SETTING_H;
    btnShowSetting.Caption := '설정보임';
  end;

  sbBgSetting.Height  := BG_SETTING_H - minus_setting;
  sbBgPos.Height      := BG_POS_H;
  sbBgCntr.Height     := BG_CNTR_H;

  fmMain.Height := FORM_H - minus_setting;

end;



procedure TfmMain.ShowHideTick();
begin
  if m_bShowTick then
  begin
    btnShowPrcGrid.Caption := '시세 감추기';
    __ShowPrcGrid(fmMain.Left+fmMain.Width, fmMain.top);
  end
  else
  begin
    btnShowPrcGrid.Caption := '시세 보이기';
    __HidePrcGrid(fmMain.Left+fmMain.Width, fmMain.top);
  end;
end;



//function  TfmMain.GetIdxOfPosGrid(artc:string):integer;
//var
//  i : integer;
//begin
//
//  for i := 1 to MAX_STK do
//  BEGIN
//    if gdPosMine.Cells[POS_ARTC, i] = artc then
//    begin
//      Result := i;
//      exit;
//    end;
//  END;
//
//  Result := -1;
//
//end;

procedure TfmMain.InitComponents();
var
  i1,i2 : integer;
  cnt   : integer;
  s     : string;


begin

  m_bShowSetting  := true;
  m_bShowTick     := false;
  
  ShowHideBg();

  //tick grid
  __CreatePrcGrid(fmMain.Left + fmMain.Width, fmMain.top);



  // 내 체결내역 size
  gdCntrMine.ColWidths[CNTR_SEQ]      := 30;
  gdCntrMine.ColWidths[CNTR_ID]       := 0;
  gdCntrMine.ColWidths[CNTR_TM]       := 60;
  gdCntrMine.ColWidths[CNTR_STK]      := 45;
  gdCntrMine.ColWidths[CNTR_SIDE]     := 45;
  gdCntrMine.ColWidths[CNTR_CLR_TP]   := 55;
  gdCntrMine.ColWidths[CNTR_PRC]      := 55;
  gdCntrMine.ColWidths[CNTR_QTY]      := 35;
  gdCntrMine.ColWidths[CNTR_PL_TICK]  := 55;
  gdCntrMine.ColWidths[CNTR_PL]       := 0;
  gdCntrMine.ColWidths[CNTR_ORD_TP]   := 0;
  gdCntrMine.ColWidths[CNTR_LVG]      := 0;
  gdCntrMine.ColWidths[CNTR_CNTR_NO]  := 0;

//  gdCntrMine.Width := gdCntrMine.ColWidths[CNTR_SEQ]    +
//                      gdCntrMine.ColWidths[CNTR_ID]     +
//                      gdCntrMine.ColWidths[CNTR_TM]     +
//                      gdCntrMine.ColWidths[CNTR_STK]    +
//                      gdCntrMine.ColWidths[CNTR_SIDE]   +
//                      gdCntrMine.ColWidths[CNTR_CLR_TP] +
//                      gdCntrMine.ColWidths[CNTR_PRC]    +
//                      gdCntrMine.ColWidths[CNTR_QTY]    +
//                      gdCntrMine.ColWidths[CNTR_PL_TICK]+
//                      //gdCntrMine.ColWidths[CNTR_PL]
//                      //gdCntrMine.ColWidths[CNTR_ORD_TP] +
//                      //gdCntrMine.ColWidths[CNTR_LVG]
//                      30
//                      ;

  // 마스터체결내역 size
  gdCntrMaster.ColWidths[CNTR_SEQ]    := gdCntrMine.ColWidths[CNTR_SEQ];
  gdCntrMaster.ColWidths[CNTR_ID]     := 60;
  gdCntrMaster.ColWidths[CNTR_TM]     := gdCntrMine.ColWidths[CNTR_TM] + 25;
  gdCntrMaster.ColWidths[CNTR_STK]    := gdCntrMine.ColWidths[CNTR_STK];
  gdCntrMaster.ColWidths[CNTR_SIDE]   := gdCntrMine.ColWidths[CNTR_SIDE];
  gdCntrMaster.ColWidths[CNTR_CLR_TP] := gdCntrMine.ColWidths[CNTR_CLR_TP];
  gdCntrMaster.ColWidths[CNTR_PRC]    := gdCntrMine.ColWidths[CNTR_PRC];
  gdCntrMaster.ColWidths[CNTR_QTY]    := gdCntrMine.ColWidths[CNTR_QTY];
  gdCntrMaster.ColWidths[CNTR_PL_TICK]:= 0;
  gdCntrMaster.ColWidths[CNTR_PL]     := gdCntrMine.ColWidths[CNTR_PRC]+20;
  gdCntrMaster.ColWidths[CNTR_ORD_TP] := gdCntrMine.ColWidths[CNTR_CLR_TP];
  gdCntrMaster.ColWidths[CNTR_LVG]    := gdCntrMine.ColWidths[CNTR_SEQ];
  gdCntrMaster.ColWidths[CNTR_CNTR_NO]:= 0; //gdCntrMine.ColWidths[CNTR_ID];

  gdCntrMaster.Width := gdCntrMaster.ColWidths[CNTR_SEQ]    +
                        gdCntrMaster.ColWidths[CNTR_ID]     +
                        gdCntrMaster.ColWidths[CNTR_CNTR_NO]+
                        gdCntrMaster.ColWidths[CNTR_TM]     +
                        gdCntrMaster.ColWidths[CNTR_STK]    +
                        gdCntrMaster.ColWidths[CNTR_SIDE]   +
                        gdCntrMaster.ColWidths[CNTR_CLR_TP] +
                        gdCntrMaster.ColWidths[CNTR_PRC]    +
                        gdCntrMaster.ColWidths[CNTR_QTY]    +
                        //gdCntrMaster.ColWidths[CNTR_PL_TICK]+
                        gdCntrMaster.ColWidths[CNTR_PL]     +                        //gdCntrMaster.ColWidths[CNTR_ORD_TP] +
                        //gdCntrMaster.ColWidths[CNTR_LVG]
                        30;


  gdCntrMine.Width :=  gdCntrMaster.Width;

  lblCntrMaster.Left  := gdCntrMaster.Left + gdCntrMaster.Width + 10;
  lblCntrMine.Left    := lblCntrMaster.Left;

  // position grid
  gdPosMine.ColWidths[POS_OPEN_TICKCOUNT] := 0;

  gdPosMaster.ColWidths[POS_MASTER] := 0;
  gdPosMaster.ColWidths[POS_NOWPRC] := 0;
  gdPosMaster.ColWidths[POS_TS_STATUS] := 0;
  gdPosMaster.ColWidths[POS_TS_SLSHIFT] := 0;
  gdPosMaster.ColWidths[POS_TS_CUT]   := 0;
  gdPosMaster.ColWidths[POS_TS_BEST]  := 0;
  gdPosMaster.ColWidths[POS_SL_PRC]   := 0;
  gdPosMaster.ColWidths[POS_SL_TICK]   := 0;
  gdPosMaster.ColWidths[POS_OPEN_TICKCOUNT] := 0;


  // 설정항목들

  m_setting[1].master := @cbMasterId1;
  m_setting[2].master := @cbMasterId2;
  m_setting[3].master := @cbMasterId3;

  m_setting[1].masterID := @edMasterID_1;
  m_setting[2].masterID := @edMasterID_2;
  m_setting[3].masterID := @edMasterID_3;

  m_setting[1].artcNm := @cbStk1;
  m_setting[2].artcNm := @cbStk2;
  m_setting[3].artcNm := @cbStk3;

  m_setting[1].artcCd := @edArtc1;
  m_setting[2].artcCd := @edArtc2;
  m_setting[3].artcCd := @edArtc3;

  m_setting[1].rvs := @chkRvs1;
  m_setting[2].rvs := @chkRvs2;
  m_setting[3].rvs := @chkRvs3;

  m_setting[1].macro := @chkMacroOrd1;
  m_setting[2].macro := @chkMacroOrd2;
  m_setting[3].macro := @chkMacroOrd3;

  m_setting[1].buyx := @edbuyx1;
  m_setting[2].buyx := @edbuyx2;
  m_setting[3].buyx := @edbuyx3;
  m_setting[1].buyy := @edbuyy1;
  m_setting[2].buyy := @edbuyy2;
  m_setting[3].buyy := @edbuyy3;

  m_setting[1].sellx := @edsellx1;
  m_setting[2].sellx := @edsellx2;
  m_setting[3].sellx := @edsellx3;
  m_setting[1].selly := @edselly1;
  m_setting[2].selly := @edselly2;
  m_setting[3].selly := @edselly3;

  m_setting[1].clrx := @edclrx1;
  m_setting[2].clrx := @edclrx2;
  m_setting[3].clrx := @edclrx3;
  m_setting[1].clry := @edclry1;
  m_setting[2].clry := @edclry2;
  m_setting[3].clry := @edclry3;


  m_setting[1].tickval := @edtickval1;
  m_setting[2].tickval := @edtickval2;
  m_setting[3].tickval := @edtickval3;

  m_setting[1].qty := @edqty1;
  m_setting[2].qty := @edqty2;
  m_setting[3].qty := @edqty3;

  m_setting[1].scalping := @cbScalping1;
  m_setting[2].scalping := @cbScalping2;
  m_setting[3].scalping := @cbScalping3;

  m_setting[1].addPos := @chkAddPos1;
  m_setting[2].addPos := @chkAddPos2;
  m_setting[3].addPos := @chkAddPos3;

  m_setting[1].clrYN := @chkClrYN1;
  m_setting[2].clrYN := @chkClrYN2;
  m_setting[3].clrYN := @chkClrYN3;

  // ts setting
  __CreateTSsetting(0, 0, false);

  // signal
  __CreateSignal();

  // 시세list
  __prcList := TPRCLIST.CREATE;

  __CnfgName := __Get_CFGFileName();

  for i1 := 1 to MAX_STK do
  BEGIN
    m_setting[i1].master.AddItem('! Copy 해제 !', NIL);
    m_setting[i1].buyx.Text := '';
    m_setting[i1].buyy.Text := '';
    m_setting[i1].sellx.Text := '';
    m_setting[i1].selly.Text := '';
    m_setting[i1].rvs.Checked := false;

    //m_setting.stk[i].Alignment  := taCenter;
    m_setting[i1].buyx.Alignment := taCenter;
    m_setting[i1].buyy.Alignment := taCenter;
    m_setting[i1].sellx.Alignment := taCenter;
    m_setting[i1].selly.Alignment := taCenter;
    m_setting[i1].tickval.Alignment := taCenter;
    m_setting[i1].qty.Alignment := taCenter;


    // 거래 종목을 config 에서 읽어들인다.
    s   := __Get_CFGFile('STK', 'CNT', '3', False, __CnfgName);
    cnt := strtointdef(s,0);

    // sise signal
    __signal.AddStk('');

    for i2 := 1 to cnt do
    begin
      s     := __Get_CFGFile('STK', inttostr(i2), '', False, __CnfgName);
      m_setting[i1].artcNm.AddItem(s, nil);
      m_setting[i1].artcCd.Text := '';

      // 시세list 에 artc 추가
      __prcList.InsertArtc( __Artc(s) );

      // signal 에 추가
      __signal.AddStk(s);

      // 가격계산 툴
      cbCalcPrc.AddItem(__Artc(s), nil);
    end;

    m_setting[i1].scalping.AddItem('0',nil);
    m_setting[i1].scalping.AddItem('30',nil);
    m_setting[i1].scalping.AddItem('60',nil);
    m_setting[i1].scalping.AddItem('70',nil);
    m_setting[i1].scalping.AddItem('80',nil);
    m_setting[i1].scalping.AddItem('90',nil);
    m_setting[i1].scalping.AddItem('100',nil);
    m_setting[i1].scalping.AddItem('110',nil);
    m_setting[i1].scalping.AddItem('120',nil);
    m_setting[i1].scalping.AddItem('150',nil);
    m_setting[i1].scalping.AddItem('180',nil);
    m_setting[i1].scalping.ItemIndex := 3;


    // TS 체크박스 초기화
    s := format('TS%d_USE', [i1]);
    __Set_CFGFile('TSSL', s, 'N', False, __CnfgName);


  end;


  edPwd.Text := '';

  AddMsg('InitComponents');

end;


//procedure TfmMain.tmrInitTimer(Sender: TObject);
//begin
//
//  tmrInit.Enabled  := False;
//
//  //InitComponents();
//
//  Init_PacketThread();
//
//
//end;


procedure TfmMain.FormCreate(Sender: TObject);
begin

  //Application.OnException := AppExceptionHandler;
  m_log := TMTLogger.create(True);
  m_log.Initialize(GetCurrentDir(), ExtractFileName(Application.ExeName));

  __PacketQ     := nil;
  __packetThrd  := nil;
  __rcvThrd     := nil;
  __tickThrd    := nil;
  __ts          := nil;

  m_bPwdOk := False;

  m_nCurrMastersCnt := 0;

  InstallMouseHook();

end;

procedure TfmMain.FormShow(Sender: TObject);
begin

  m_bTerminate     := False;


  InitComponents();

  edPwd.SetFocus;

  tmrTryConn.Interval := 500;
  tmrTryConn.Enabled  := True;

end;

procedure TfmMain.CreateOrdThreads;
var
  i : integer;
begin

  for i := 1 to MAX_STK do
  BEGIN
    __ordThrd[i] := TOrdThrd.Create;
    __ordThrd[i].SetIdx(i);
  END;

end;



procedure TfmMain.gdCntrMineGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin

  if ARow=0 then
  begin
    HAlign := taCenter;
  end
  else
  begin

    if (ACol=CNTR_PRC)  OR
        (ACol=CNTR_QTY) OR
        (ACol=CNTR_PL_TICK) OR
        (ACol=CNTR_PL) OR
        (ACol=CNTR_LVG)
    then
    BEGIN
      HAlign := taRightJustify;
    END
    ELSE
    BEGIN
      HAlign := taCenter;
    END;
  end;
end;

procedure TfmMain.gdCntrMineGetCellColor(Sender: TObject; ARow, ACol: Integer;
  AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
var
  dPl : double;
begin
  if acol=CNTR_SIDE then
  BEGIN
    if gdCntrMine.cells[acol,arow]='매도' then
      afont.Color := clblue
    else if gdCntrMine.cells[acol,arow]='매수' then
      afont.Color := clred;
  END
  else if acol=CNTR_PL_TICK then
  BEGIN
    dPl := strtofloatdef(gdCntrMine.cells[acol,arow],0);
         if dPl > 0 then afont.Color := clred
    else if dPl < 0 then afont.Color := clblue;
  END;
  //else
    //afont.Color := clwhite;
end;

procedure TfmMain.gdDiffGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  if arow=0 then
    HAlign := taCenter
  else
    HAlign := taRightJustify;
end;

procedure TfmMain.gdDiffGetCellColor(Sender: TObject; ARow, ACol: Integer;
  AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
begin
//afont.Color := clwhite;
end;

procedure TfmMain.gdCntrMasterGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
 if ARow=0 then
  begin
    HAlign := taCenter;
  end
  else
  begin

    if (ACol=CNTR_PRC)  OR
        (ACol=CNTR_QTY) OR
        (ACol=CNTR_PL_TICK) OR
        (ACol=CNTR_PL) OR
        (ACol=CNTR_LVG)
    then
    BEGIN
      HAlign := taRightJustify;
    END
    ELSE
    BEGIN
      HAlign := taCenter;
    END;
  end;
end;

procedure TfmMain.gdCntrMasterGetCellColor(Sender: TObject; ARow, ACol: Integer;
  AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
var
  nPl : integer;
begin
 if acol=CNTR_SIDE then
  BEGIN
    if gdCntrMaster.cells[acol,arow]='매도' then
      afont.Color := clblue
    else if gdCntrMaster.cells[acol,arow]='매수' then
      afont.Color := clred;
  END
  else if acol=CNTR_PL then
  BEGIN
    nPl := __CvtCommaAmt(gdCntrMaster.cells[acol,arow]);
          if nPl > 0 then afont.Color := clred
    else  if nPl < 0 then afont.Color := clblue;
  END;
 // else
    //afont.Color := clwhite;
end;

procedure TfmMain.gdPosMasterGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin

  if arow=0 then
    HAlign := taCenter
  else
  begin
    if (ACol=POS_QTY) OR (ACol=POS_AVG) then
      HAlign := taRightJustify
    else
      HAlign := taCenter;
  end;
end;

procedure TfmMain.gdPosMasterGetCellColor(Sender: TObject; ARow, ACol: Integer;
  AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
begin
  if acol=POS_SIDE then
  begin
    if gdPosMine.Cells[acol,arow] = '매도' then
      afont.Color := clblue
    else if gdPosMine.Cells[acol,arow] = '매수' then
      afont.Color := clred;
  end;
 // else
   // afont.Color := clwhite;
end;

procedure TfmMain.gdPosMineGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin

  //TODO
//  if arow=0 then
//    HAlign := taCenter
//  else
//  begin
//    if (ACol=POS_QTY) OR (ACol=POS_AVG) then
//      HAlign := taRightJustify
//    else
//      HAlign := taCenter;
//  end;

end;

procedure TfmMain.gdPosMineGetCellColor(Sender: TObject; ARow, ACol: Integer;
  AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
begin

  if acol=POS_SIDE then
  begin
    if gdPosMine.Cells[acol,arow] = '매도' then
      afont.Color := clblue
    else if gdPosMine.Cells[acol,arow] = '매수' then
      afont.Color := clred;
  end;
//  else
//    afont.Color := clwhite;
end;


procedure TfmMain.AddMsg(msg:string; bSiren:boolean=false; bShow:boolean=false);
var
  sMsg : string;
begin
  sMsg := format('[%s] %s', [__NowHMS(), msg]);
  cbMsg.Items.Insert(0, sMsg);
  cbMsg.ItemIndex := 0;

  if bShow then
    showmessage(sMsg);

  m_log.log(INFO, msg);

  if bSiren then
    __Siren(msg);
end;


function TfmMain.EndMainByDisconn():boolean;
begin
  Result := false;

  if idTcpOrd.Connected=true then
    exit;

  AddMsg('주문서버 Disconnect', B_SIREN);

  Result := true;
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


procedure TfmMain.tmrUnMarkChangeTimer(Sender: TObject);
begin
  tmrUnMarkChange.Enabled := False;
  UnMark_MasterDataChanged();
  edClrTp.Repaint;
  edMasterId.Repaint;
  edStk.Repaint;
  edSide.Repaint;
  edCntrPrc.Repaint;
  edCntrQty.Repaint;
  edCntrTm.Repaint;
  edCntrNo.Repaint;
end;


procedure TfmMain.tmrSetIDComboTimer(Sender: TObject);
var
  i : integer;
begin
    tmrSetIDCombo.Enabled   := False;

    cbMasterIDs.ItemIndex := 0;
    m_currComboIdx        := 0;
    pnlLoginTp.Caption    := __LoginTp(m_arrMasters[0].loginTp);

    for i := 1 to MAX_STK do
      m_setting[i].master.ItemIndex := 0;
end;


procedure TfmMain.tmrSignalAlarmTimer(Sender: TObject);
begin
  if tmrSignalAlarmCnt = 2 then
  begin
    tmrSignalAlarm.Enabled := false;
  end;
  __SignalAlarm('', 0, '');
  Inc(tmrSignalAlarmCnt);
end;

procedure TfmMain.tmrTickerTimer(Sender: TObject);
begin
  tmrTicker.Enabled := false;
  edtTicker.Repaint;
end;

procedure TfmMain.tmrTryConnTimer(Sender: TObject);
begin

  tmrTryConn.Enabled := false;
  Connect_OrdSvr;


  CreateOrdThreads();

end;

procedure TfmMain.Proc_Msg();
var
  sRetCode : string;
  sMsg     : string;
  i1       : integer;
begin

  sRetCode := Copy(m_sRcvPack, 3, 2);


  if sRetCode = RETCODE_PWD_WRONG then
  BEGIN
    AddMsg('ERR! 비밀번호 오류');
  END
  else if sRetCode = RETCODE_CNTR_NODATA then
    AddMsg('체결내역이 없습니다.')
  else if sRetCode=RETCODE_PWD_OK then
  BEGIN
    m_bPwdOk := True;

    // 시세 수신
    // 시세GRID 에 INSERT
    for i1 := 0 to __prcList.Count()-1 do
      fmPrcGrid.gdTick.Cells[TICK_ARTC,i1] := __prcList.Artc(i1);

    // 시세서버 연결 및 데이터 수신
    Connect_TickSvr;
  END;

end;


procedure TfmMain.idTcpOrdConnected(Sender: TObject);
begin

  Init_PacketThread();

  __rcvThrd.Resume;

  AddMsg('주문서버 Connect!');

end;

procedure TfmMain.idTcpOrdDisconnected(Sender: TObject);
begin
//
end;


//function  TfmMain.IsExistsID(sMasterID:string):boolean;
//begin
//  Result := (GetIdxOfMasterId(sMasterID)>-1);
//end;

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


procedure TfmMain.GroupBox1Click(Sender: TObject);
begin
  _bMarkChanged := false;
end;

procedure TfmMain.Proc_LogOnOff();
var

  oldDataYn
  ,masterId
  ,Tm
  ,loginTp
  ,masterNm   : string;

  bSound  : boolean;
  alarmTp : integer;

  idxMaster : integer;
  bChanged  : boolean;
  bFirstData : boolean;

  i1,i2   : integer;
  bFound  : boolean;
begin


  oldDataYn := Uppercase(copy(m_sRcvPack, 3, 1));
  masterId  := Uppercase(Trim(copy(m_sRcvPack, 4, 20)));
  Tm        := copy(m_sRcvPack, 24, 12);
  loginTp   := copy(m_sRcvPack, 36, 1);


  masterNm  := Trim(copy(m_sRcvPack, 37, 20));
  //pName := Addr(m_sRcvPack[37]);
  //masterNm := __CharFunction(pName, 20);
  //edstk1.Text := masterNm;

  // setting 의 Master ID 콤보박스
  for i1 := 1 to MAX_STK do
  BEGIN
    bFound := false;
    for i2 := 0 to m_setting[i1].master.Items.Count do
    begin
      if masterId = m_setting[i1].master.Items[i2] then
      begin
        bFound := true;
      end;
    end;

    if Not bFound then
    begin
      m_setting[i1].master.AddItem(masterId, NIL);
    end;

  END;

//  if IsMasterFixed() then
//  begin
//    if masterId <> cbMasterIDs.Items[cbMasterIDs.ItemIndex] then
//    begin
//      AddMsg(format('[%s] ID고정 사용중이므로 로그온/아웃 처리하지 않음', [masterId]));
//      exit;
//    end;
//  end;
                                          

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
    pnlLoginTp.Caption              := __LoginTp(loginTp);
    lblLastLogonoffTime.Caption     := Tm;

    cbMasterIDs.ItemIndex := idxMaster;

    pnlLoginTp.Color      := clred;
    pnlLoginTp.Font.Color := clyellow;

    tmrUnMarkChange.Interval := 30000;
    tmrUnMarkChange.Enabled  := True;


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

    __ShowLogin(alarmTp, masterId, chPopup.Checked);
  end;

end;


procedure TfmMain.MasterOrd_Parsing();
var
  stk : string;
begin
  ZeroMemory(@m_rcv, sizeof(m_rcv));

  m_rcv.masterId  := UPPERCASE( Trim(copy(m_sRcvPack, 4,20)));
  m_rcv.cntrNo  := Trim(copy(m_sRcvPack, 24,5));

  stk := UpperCase(Trim(copy(m_sRcvPack, 29,10)));
  m_rcv.artc := __ExtractArtcCd(stk);

  m_rcv.side    := copy(m_sRcvPack, 39,1);
  m_rcv.cntrQty := Trim(copy(m_sRcvPack, 40,3));
  m_rcv.cntrPrc := Trim(copy(m_sRcvPack, 43,10));
  m_rcv.clrPl   := Trim(copy(m_sRcvPack, 53,10));
  m_rcv.cmsn    := Trim(copy(m_sRcvPack, 63,5));
  m_rcv.clrTp   := copy(m_sRcvPack, 68, 1);
  m_rcv.bf_nclrQty  := Trim(copy(m_sRcvPack, 69,3));
  m_rcv.af_nclrQty  := Trim(copy(m_sRcvPack, 72,3));
  m_rcv.bf_avgPrc   := Trim(copy(m_sRcvPack, 75,10));
  m_rcv.af_avgPrc   := Trim(copy(m_sRcvPack, 85,10));
  m_rcv.bf_amt      := Trim(copy(m_sRcvPack, 95,10));
  m_rcv.af_amt      := Trim(copy(m_sRcvPack, 105,10));
  m_rcv.ordTp       := Trim(copy(m_sRcvPack, 115,2));
  m_rcv.tradeTm     := Trim(copy(m_sRcvPack, 117,12));
  m_rcv.lvg         := Trim(copy(m_sRcvPack, 129,2));

  //Set_CurrStxIdx();

end;

procedure TfmMain.MasterOrd_MarkChanged();
begin

  _bMarkChanged := false;

   // mark the items changed
  if edMasterId.Text <> m_rcv.masterID then
    _bMarkChanged := true;

  if edStk.Text <> m_rcv.artc then
    _bMarkChanged := true;

  if edside.Text <> __Side(m_rcv.side) then
    _bMarkChanged := true;

  if strtointdef(edCntrQty.Text, 0) <> strtoint(m_rcv.cntrQty) then
    _bMarkChanged := true;

  if edClrTp.Text <> __ClrTp(m_rcv.clrTp) then
  begin
    if m_rcv.clrTp=CLR_TP_CLR then  edClrTp.Tag := 1
    else                            edClrTp.Tag := 0;
    _bMarkChanged := true;
  end;

  if _bMarkChanged then
  begin
    tmrUnMarkChange.Interval := 30000;
    tmrUnMarkChange.Enabled  := True;
  end;

end;



// DIFF GRID

procedure TfmMain.Proc_Cntr(bHistory:boolean);
var
  rowcnt  : integer;
  bSound  : boolean;

  alarmTp : integer;
  idxMaster : integer;

  //dClrPl,
  //dTickSize : double;

begin

  bSound := FALSE;

  // 패킷을 Parsing 해서 m_rcv 에 담는다.
  MasterOrd_Parsing();

  idxMaster := GetIdxOfMasterId(m_rcv.masterId);

  if bHistory = FALSE then
  begin

    // 각 주문 스레드에 신규주문이 들어왔음을 알리고, 주문처리하게 한다.
    PostMsg_OrdThrds();

    if strtointdef(edCntrNo.Text,0)<>strtoint(m_rcv.cntrno) then
      bSound := true;

    //
    MasterOrd_MarkChanged();

    edMasterId.Text := m_rcv.masterId;
    edCntrTm.Text   := m_rcv.tradeTm;
    edCntrNo.Text   := m_rcv.cntrNo;
    edStk.Text      := m_rcv.artc;
    edSide.Text     := __Side(m_rcv.side);
    edCntrQty.Text  := m_rcv.cntrQty;
    edCntrPrc.Text  := __Prcfmt(m_rcv.artc, m_rcv.cntrPrc) ;
    edClrTp.Text    := __ClrTp(m_rcv.clrTp);

    m_arrMasters[idxMaster].nLastCntrNo := strtoint(m_rcv.cntrNo);

    AddMsg(format('[체결수신](%s)(%s)(%s)(%s)(%s)',
                  [m_rcv.masterId, m_rcv.artc, edClrTp.Text, edSide.Text,edCntrPrc.Text]));

  end; // if bHistory = FALSE then

  
  if bSound then
  begin
    if m_rcv.side='S' then alarmTp := TP_SELL;
    if m_rcv.side='B' then alarmTp := TP_BUY;

    __ShowCntr(alarmTp, m_rcv.clrTp, m_rcv.masterId, chPopup.Checked);
  end;

  // 체결내역
  rowcnt := gdCntrMaster.RowCount;

  if rowcnt=2 then
  begin
    if gdCntrMaster.Cells[CNTR_CNTR_NO, 1]<>''  then
    begin
      gdCntrMaster.InsertRows(1, 1);
      rowcnt := 2;
    end
    else
      rowcnt := 1;
  end
  else
  begin
    gdCntrMaster.InsertRows(1, 1);
    rowcnt := rowcnt + 1;
  end;

  gdCntrMaster.Cells[CNTR_SEQ, 1]     := INTTOSTR(rowcnt);
  gdCntrMaster.Cells[CNTR_ID, 1]      := m_rcv.masterId;
  gdCntrMaster.Cells[CNTR_CNTR_NO, 1] := m_rcv.cntrNo;
  gdCntrMaster.Cells[CNTR_TM, 1]      := m_rcv.tradeTm;
  gdCntrMaster.Cells[CNTR_STK, 1]     := m_rcv.artc;
  gdCntrMaster.Cells[CNTR_SIDE, 1]    := __Side(m_rcv.side);
  gdCntrMaster.Cells[CNTR_QTY, 1]     := m_rcv.cntrQty;
  gdCntrMaster.Cells[CNTR_PRC, 1]     := __PrcFmt(m_rcv.artc, m_rcv.cntrPrc);
  gdCntrMaster.Cells[CNTR_ORD_TP, 1]  := __OrdTp(m_rcv.ordTp);
  gdCntrMaster.Cells[CNTR_CLR_TP, 1]  := __ClrTp(m_rcv.clrTp);
  gdCntrMaster.Cells[CNTR_PL, 1]      := __MoneyFmt( m_rcv.clrPl);
  gdCntrMaster.Cells[CNTR_LVG, 1]     := m_rcv.lvg;

  //dClrPl := strtofloatdef(m_rcv.clrPl,0);

end;


// ORD THREAD 들에게 패킷이 들어왔음을 알려준다.
procedure TfmMain.PostMsg_OrdThrds();
var
  i         : integer;
begin

  for i := 1 to MAX_STK do
  BEGIN
    if m_setting[i].master.ItemIndex = 0 then
      continue;

//    SetLength(masterOrd, nSize);
//    CopyMemory(@masterOrd[0], Addr(m_rcv), nSize);
//
//    nThreadId := m_ordThrd[i].GetThreadId();
//    PostThreadMessage(nthreadId, WM_COPY_ORD, LongInt(nSize), LongInt(masterOrd));

    __ordThrd[i].AddData(m_rcv);

  END;


end;


procedure TfmMain.AppExceptionHandler(sender:TObject; E:Exception);
begin
  showmessage('AppException');
  application.Terminate;
  //Application.ShowException(E);

    
end;


                                      
procedure TfmMain.btnCalcPrcClick(Sender: TObject);
var
  dBase,
  dTick,
  dtickSize : double;
  artc : string;
begin

  dBase := strtofloatdef(edtCalcPrcBase.Text, 0);
  if dBase <=0 then
    exit;

  dTick := strtofloatdef(edtCalcPrcTick.Text, 0);
  if dTick <=0 then
    exit;

  artc := cbCalcPrc.Items[cbCalcPrc.Itemindex];
  if artc='' then
    exit;

  dTickSize := __TickSize(artc);

  edtCalcPrcH.Text := __PrcFmtD(artc, dBase + (dTick * dTickSize));
  edtCalcPrcL.Text := __PrcFmtD(artc, dBase - (dTick * dTickSize));

end;

procedure TfmMain.btnCntrHistClick(Sender: TObject);
var
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

  //
  Clear_MasterCntrGrid;
  //

  m_arrMasters[cbMasterIDs.ItemIndex].nLastCntrNo := 0;

  sPacket := CODE_CNTR_HIST + Trim(cbMasterIDs.Items[cbMasterIDs.ItemIndex]);

  if idTcpOrd.Connected=False then
  begin
    showmessage('서버와의 접속 오류. 재접속 하세요');
    exit;
  end;

  SendData_OrdSvr(sPacket, sPacket.Length);

  AddMsg('체결내역 요청 전송');

end;



function  TfmMain.IsScalping(idx:integer):boolean;
var
  openTm    : double;
  GapSec    : integer;
  sBaseSec  : string;
  bRslt     : boolean;
begin

  openTm  := strtofloatdef(fmMain.gdPosMine.Cells[POS_OPEN_TICKCOUNT, idx] ,0);

  GapSec  := __CalcTimeGapSec(openTm) ;

  sBaseSec := fmMain.m_setting[idx].scalping.Items[fmMain.m_setting[idx].scalping.ItemIndex];

  bRslt  := (GapSec < strtointdef(sBaseSec,0));
  Result := bRslt;
end;


procedure TfmMain.SendData_OrdSvr(sData:string; len:integer);
begin

  try
    idTcpOrd.IOHandler.Write(sdata);
  except
    showmessage('통신오류, 데이터전송 실패');
    exit;
  end;

end;


procedure TfmMain.EnableSignalAlarmTimer();
begin
  tmrSignalAlarmCnt       := 0;
  tmrSignalAlarm.Interval := 1000;
  tmrSignalAlarm.Enabled  := true;
end;


procedure TfmMain.WndProc(var Message: TMessage);
var
  itemMD  : TItemMD;
  ItemPos : TItemPos;
  ItemTSStatus : TItemTSStatus;
  itemCntr : TItemCntr;
  idx    : integer;
begin
  inherited;

  try
    if Message.Msg = WM_GRID_REAL_MD then
    begin
      itemMD := TItemMD(Message.LParam);

      idx := itemMD.idxPrcGrid;
      fmPrcGrid.gdTick.Cells[TICK_ARTC, idx] := itemMD.artc;
      fmPrcGrid.gdTick.Cells[TICK_PRC,  idx] := itemMD.close;
      fmPrcGrid.gdTick.Cells[TICK_TM,   idx] := itemMD.time;

      idx := itemMD.idxPosGrid;
      if idx>-1 then
      begin
        fmMain.gdPosMine.Cells[POS_NOWPRC,  idx]  := itemMD.close;
        fmMain.gdPosMine.Cells[POS_PL_TICK, idx]  := __ts.Calc_PLTick(idx, itemMD.close);
      end;

      __signal.CheckSignal(itemMD.artc, itemMD.close);

      edtTicker.Text := itemMD.time;

      FreeAndNil(itemMD);
    end;

    if Message.Msg = WM_GRID_POSITION then
    begin
      ItemPos := TItemPos(Message.LParam);

      idx := ItemPos.idx;
      fmMain.gdPosMine.Cells[POS_MASTER,   idx] := ItemPos.masterId;
      fmMain.gdPosMine.Cells[POS_ARTC,     idx] := ItemPos.artc;
      fmMain.gdPosMine.Cells[POS_TS_SLSHIFT, idx]   := ItemPos.tsSLShift;
      fmMain.gdPosMine.Cells[POS_TS_CUT, idx]       := ItemPos.tsCutPrc;
      fmMain.gdPosMine.Cells[POS_SL_PRC, idx]       := ItemPos.slCutPrc;
      fmMain.gdPosMine.Cells[POS_SL_TICK, idx]      := ItemPos.slTick;

      FreeAndNil(ItemPos);
    end;

    if Message.Msg = WM_GRID_TS_STATUS then
    begin
      ItemTSStatus := TItemTSStatus(Message.LParam);

      idx := ItemTSStatus.idx;
      fmMain.gdPosMine.Cells[POS_TS_STATUS,idx] := ItemTSStatus.tsStatus;

      if ItemTSStatus.tsBestPrc > 0 then
        fmMain.gdPosMine.Cells[POS_TS_BEST, idx] := formatfloat('#0.#',ItemTSStatus.tsBestPrc);

      if ItemTSStatus.slPrc <> 0 then
      begin
        fmMain.gdPosMine.Cells[POS_SL_PRC, idx] := __FmtPrcD( __ts.m[idx].dotcnt, ItemTSStatus.slPrc);
        fmMain.gdPosMine.Cells[POS_SL_TICK, idx] := formatfloat('#0.#', ItemTSStatus.slTick);
      end;

      FreeAndNil(ItemTSStatus);
    end;

    if Message.Msg = WM_GRID_POSINSERT then
    begin
      ItemPos := TItemPos(Message.LParam);

      idx := ItemPos.idx;

      gdPosMine.Cells[POS_MASTER, idx]  := itemPos.masterId;
      gdPosMine.Cells[POS_ARTC, idx]    := itemPos.artc;
      gdPosMine.Cells[POS_STATUS,idx]   := POS_STATUS_NONE;

      FreeAndNil(ItemPos);
    end;

    if Message.Msg = WM_GRID_CNTR then
    begin
      itemCntr := TItemCntr(Message.LParam);

      idx := itemCntr.idx;

      if itemCntr.posStatus<>'' then
      begin

        fmMain.gdPosMine.Cells[POS_STATUS,idx] := itemCntr.posStatus;
        fmMain.gdPosMine.Cells[POS_SIDE,  idx] := itemCntr.side;
        fmMain.gdPosMine.Cells[POS_TM,    idx] := itemCntr.tm;
        fmMain.gdPosMine.Cells[POS_QTY,   idx] := itemCntr.qty;
        fmMain.gdPosMine.Cells[POS_AVG,   idx] := itemCntr.avg;
        if fmMain.gdPosMine.Cells[POS_OPEN_TICKCOUNT,   idx]='' then
          fmMain.gdPosMine.Cells[POS_OPEN_TICKCOUNT,   idx] := floattostr(GetTickCount());
      end
      else
      begin
        fmMain.gdPosMine.Cells[POS_QTY, idx]    := itemCntr.qty;
      end;
      FreeAndNil(itemCntr);
    end;

    if Message.Msg = WM_GRID_POSCLR then
    BEGIN
      idx := Message.WParam;

      gdPosMine.cells[POS_STATUS,idx]   := POS_STATUS_NONE;
      //gdPosMine.cells[POS_ARTC,idx]     := 'N/A' ;
      gdPosMine.cells[POS_SIDE   ,idx]  := '';
      gdPosMine.cells[POS_QTY   ,idx]   := '';
      gdPosMine.cells[POS_TM    ,idx]   := '';
      gdPosMine.cells[POS_AVG   ,idx]   := '';
      gdPosMine.cells[POS_TS_STATUS, idx] := '';
      gdPosMine.cells[POS_TS_SLSHIFT,idx] := '';
      gdPosMine.cells[POS_TS_CUT,    idx] := '';
      gdPosMine.cells[POS_TS_BEST,   idx] := '';
      gdPosMine.cells[POS_SL_TICK,   idx] := '';
      gdPosMine.cells[POS_SL_PRC,    idx] := '';
      gdPosMine.cells[POS_OPEN_TICKCOUNT,idx] := '';
    END;

  except

  end;
end;



constructor TEditStyleHookColor.Create(AControl: TWinControl);
begin
  inherited;
  //call the UpdateColors method to use the custom colors
  UpdateColors;
end;

//Here you set the colors of the style hook
procedure TEditStyleHookColor.UpdateColors;
var
  LStyle: TCustomStyleServices;
begin
  if (Control.Enabled) then
  begin

      if (pos(LowerCase(Control.Name), 'edmasterid') >0)  or
          (pos(LowerCase(Control.Name), 'edstk') >0)      or
          (pos(LowerCase(Control.Name), 'edside') >0)     or
          (pos(LowerCase(Control.Name), 'edcntrqty') >0)

      then
      begin
        if _bMarkChanged then
        begin
          Brush.Color := clYellow;//TWinControlH(Control).Color; //use the Control color
          FontColor   := clred;//TWinControlH(Control).Font.Color;//use the Control font color
        end
        else
        begin
          LStyle := StyleServices;
          Brush.Color := LStyle.GetStyleColor(scEdit);
          FontColor := LStyle.GetStyleFontColor(sfEditBoxTextNormal);
        end;
      end
      else if (pos(LowerCase(Control.Name), 'edclrtp') >0) then
      begin
        if _bMarkChanged then
        begin
          if control.Tag=1 then
          begin
            Brush.Color := clBlack;
            FontColor   := clYellow;
          end
          else
          begin
            Brush.Color := clYellow;
            FontColor   := clred;
          end;
        end
        else
        begin
          LStyle := StyleServices;
          Brush.Color := LStyle.GetStyleColor(scEdit);
          FontColor := LStyle.GetStyleFontColor(sfEditBoxTextNormal);
        end;
      end
      else
      begin
        LStyle := StyleServices;
        Brush.Color := LStyle.GetStyleColor(scEdit);
        FontColor := LStyle.GetStyleFontColor(sfEditBoxTextNormal);
      end;
  end
  else
  begin
    //if the control is disabled use the colors of the style
    LStyle := StyleServices;
    Brush.Color := LStyle.GetStyleColor(scEditDisabled);
    FontColor := LStyle.GetStyleFontColor(sfEditBoxTextDisabled);
  end;
end;



//Handle the messages of the control
procedure TEditStyleHookColor.WndProc(var Message: TMessage);
begin
  case Message.Msg of
    CN_CTLCOLORMSGBOX..CN_CTLCOLORSTATIC:
      begin
        //Get the colors
        UpdateColors;
        SetTextColor(Message.WParam, ColorToRGB(FontColor));
        SetBkColor(Message.WParam, ColorToRGB(Brush.Color));
        Message.Result := LRESULT(Brush.Handle);
        Handled := True;
      end;
    CM_ENABLEDCHANGED:
      begin
        //Get the colors
        UpdateColors;
        Handled := False;
      end
  else
    inherited WndProc(Message);
  end;
end;


initialization

  TCustomStyleEngine.RegisterStyleHook(TEdit, TEditStyleHookColor);

end.
