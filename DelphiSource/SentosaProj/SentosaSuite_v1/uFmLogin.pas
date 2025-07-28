unit uFmLogin;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, uFmBasicForm,
  uTcpClient, Vcl.StdCtrls, Vcl.ExtCtrls,
  CommonUtils, ProtoSetU, ProtoGetU, uAlphaProtocol

  ;

type

  TThread_Recv = class(TThread)
  protected
    procedure Execute();override;
  end;

  TProtoLogin = class(TProtoGet)
  public
    function ParsingPacket(instr : string) : integer;

  end;

  TFmLogin = class(TForm)
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    edtUserID: TEdit;
    edtPwd: TEdit;
    btnLogon: TButton;
    btnCancel: TButton;
    pnlMsg: TPanel;
    procedure btnCancelClick(Sender: TObject);
    procedure btnLogonClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormShow(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    function  LogonAuth() : boolean;
    procedure Deinitialize();
  public
    { Public declarations }
  private
    m_RecvThrd  : TThread_Recv;
  public
    m_sockAuth  : TTcpClient;
    m_get       : TProtoLogin;

  end;

var
  fmLogin: TFmLogin;

implementation

uses
  uLocalCommon
  ;

{$R *.dfm}



procedure TFmLogin.btnCancelClick(Sender: TObject);
begin

  Close;
end;

procedure TFmLogin.btnLogonClick(Sender: TObject);
var
  sIniFileName : string;
  sAuthSvrIP, sAuthSvrPort : string;
begin
  inherited;

  _bAuthSuccess := false;

  Deinitialize();


  if (Trim(edtUserID.Text)='') or (Trim(edtPwd.Text)='') then
  begin
    pnlMsg.Caption := 'Write User ID and Password';
    exit;
  end;

  _UserID := Trim(edtUserID.Text);
  _Pwd    := Trim(edtPwd.Text);

  sIniFileName  := __Get_CFGFileName();
  sAuthSvrIP    := __Get_IniFile(sIniFileName, 'SERVER_INFO', 'LOGON_AUTH_IP');
  sAuthSvrPort  := __Get_IniFile(sIniFileName, 'SERVER_INFO', 'LOGON_AUTH_PORT');

  if (length(sIniFileName)=0) or (length(sAuthSvrIP)=0) or (length(sAuthSvrPort)=0)  then
  begin
    pnlMsg.Caption := 'Wrong Auth Server IP/Port. Check Ini file';
    exit;
  end;

  m_sockAuth := TTcpClient.Create;
  m_sockAuth.Initialize(sAuthSvrIP, sAuthSvrPort);

  if(m_sockAuth.ConnectSvr()=false) then
  begin
    pnlMsg.Caption    := 'Failed to connect Aut Svr:'+m_sockAuth.GetMsg;
    exit;
  end;

  m_RecvThrd := TThread_Recv.Create;
  m_get      := TProtoLogin.Create;

  LogonAuth();


end;

function TFmLogin.LogonAuth() : boolean;
var
  protoSet : TProtoSet;
  sSendBuf : string;
begin

  protoSet := TProtoSet.create;
  protoSet.start();
  protoSet.SetVal(FDS_CODE, CODE_LOGON_AUTH);
  protoSet.SetVal(FDN_APP_TP, APPTP_SUITES);
  protoSet.SetVal(FDS_BROKER, 'BROKER');
  protoSet.SetVal(FDS_ACCNO_MY, 'ACCNO');
  protoSet.SetVal(FDS_USER_ID, edtUserID.Text);
  protoSet.SetVal(FDS_USER_PASSWORD, edtPwd.Text);
  protoSet.SetVal(FDS_MAC_ADDR, 'MAC ADDR');
  protoSet.SetVal(FDS_LIVEDEMO, 'N');
  protoSet.SetVal(FDN_APP_TP, APPTP_SUITES);
  protoSet.Complete(sSendBuf);

  m_sockAuth.SendData(sSendBuf);
  FreeAndNil(protoSet);


end;



procedure TThread_Recv.Execute;
var
  sRecvBuf : string;
  nRes     : integer;
  nCnt     : integer;
  i        : integer;
  sRelayIP : string;
  sRelayPort : string;
  sSuccYN    : string;
  sMsg       : string;
begin
  nRes := fmLogin.m_sockAuth.ReadData(sRecvBuf);
  if nRes<=0 then
  begin
    fmLogin.pnlMsg.Caption := 'Failed to recv:'+fmLogin.m_sockAuth.GetMsg;
    exit;
  end;

  nCnt := fmLogin.m_get.ParsingPacket(sRecvBuf);
  if nCnt=0 then
  begin
    fmLogin.pnlMsg.caption := 'wrong packet:'+sRecvBuf;
    exit;
  end;


  for i := 0 to nCnt-1 do
  begin

    if fmLogin.m_get.KV[i].Key = FDS_SUCC_YN  then
      sSuccYN := fmLogin.m_get.KV[i].Value;

    if fmLogin.m_get.KV[i].Key = FDS_MSG  then
      sMsg := fmLogin.m_get.KV[i].Value;

    if fmLogin.m_get.KV[i].Key = FDS_KEY  then
      _AppID := fmLogin.m_get.KV[i].Value;


    if fmLogin.m_get.KV[i].Key = FDS_RELAY_IP  then
      _RelaySvrIP := fmLogin.m_get.KV[i].Value;

    if fmLogin.m_get.KV[i].Key = FDS_RELAY_PORT  then
      _RelaySvrPort := fmLogin.m_get.KV[i].Value;

    if fmLogin.m_get.KV[i].Key = FDS_DATASVR_IP  then
      _DataSvrIP := fmLogin.m_get.KV[i].Value;

    if fmLogin.m_get.KV[i].Key = FDS_DATASVR_PORT  then
      _DataSvrPort := fmLogin.m_get.KV[i].Value;

  end;


  if sSuccYN<>'Y' then
  begin
    fmLogin.pnlMsg.Caption := 'Receive logon auth error:'+sMsg;
    exit;
  end;

  if  (Length(_RelaySvrIP)=0)   or
      (Length(_RelaySvrPort)=0) or
      (Length(_DataSvrIP)=0)    or
      (Length(_DataSvrPort)=0)  then
  begin
    fmLogin.pnlMsg.caption := 'RelayIP/Port or DataIP/Port in the packet:'+sRecvBuf;
    exit;
  end;


  __AddMsg(Format('[LogonAuth OK] RelySvr(%s)(%s), DataSvr(%s)(%s)', [_RelaySvrIP, _RelaySvrPort, _DataSvrIP, _DataSvrPort])
          ,true);

  _bAuthSuccess := true;

  fmLogin.Close;

end;



function TProtoLogin.ParsingPacket(instr : string) : integer;
var
  intres, i1, pos1 : integer;
  StrArray  : TStringArray;
  s1        : string;

  sKey : string;
  sVal : string;
begin

  intres := SplitPacket(instr, StrArray);

  if intres < 0 then
  begin
    Result := -1; Exit;
  end;

  SetLength(KV, Length(StrArray));
  for i1 := 0 to Length(StrArray) - 1 do
  begin
    s1 := StrArray[i1];
    pos1 := Pos('=', s1);
    if pos1 <= 0 then
    begin
      // something wrong, no '=' sign in the field
      Result := -1;
      Exit;
    end;
    try
      sKey := Copy(s1, 1, pos1 - 1);
      Delete(s1, 1, pos1);
      sVal := s1;

      KV[i1].Key := sKey;
      KV[i1].Value := sVal;

    except
      // something wrong, left part (key) is not an integer value
      Result := -1; Exit;
    end;
  end;

  Result := Length(KV);

end;


procedure TFmLogin.Deinitialize();
begin
  pnlMsg.Caption := '';

  FreeAndNil(m_sockAuth);
  FreeAndNil(m_RecvThrd);
  FreeAndNil(m_get);

end;

procedure TFmLogin.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  inherited;

  Deinitialize();
  Action := cafree;

end;



procedure TFmLogin.FormCreate(Sender: TObject);
begin
  m_sockAuth := nil;
  m_RecvThrd := nil;
  m_get      := nil;


end;

procedure TFmLogin.FormShow(Sender: TObject);
begin
  inherited;
  //
end;

end.
