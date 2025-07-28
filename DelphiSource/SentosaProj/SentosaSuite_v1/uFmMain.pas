unit uFmMain;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs,Vcl.Menus,
  uLocalCommon,
  uFmBasicForm,
  uFmPriceComparison,
  CommonUtils,

  uQueueEx,
  uTcpClient,
  uFmLogin,
  Vcl.ExtCtrls, Vcl.StdCtrls
  ;

type

  TThread_Send = class(TThread)
  public
    constructor Create(bSuspend:Boolean);
    procedure Init(sSockTp:string);
  protected
    procedure Execute();override;
  private
    m_sockTp :string;
  end;

  TThread_Recv = class(TThread)
  public
    constructor Create(bSuspend:Boolean);
    procedure Init(sSockTp:string);
  protected
    procedure Execute();override;
  private
    m_sockTp : string;
  end;

  TThread_Check = class(TThread)
  protected
    procedure Execute();override;
  end;


  TSockets = class(TObject)
    sock      : TTcpClient;
    ThrdSend  : TThread_Send;
    ThrdRecv  : TThread_Recv;
    sockTp    : string;
  public
    constructor Create(sSockTp:string);
    destructor Destroy();override;

    function IsReady():boolean;
  end;


  //////////////////////////////////////////////////////////////////////////////
  ///
  TfmMain = class(TForm)
    MainMenu1: TMainMenu;
    mnuFile: TMenuItem;
    mnuMD: TMenuItem;
    subComparison: TMenuItem;
    Shape1: TShape;
    cbMsg: TComboBox;
    pnlTop: TPanel;
    btnLogAuth: TButton;
    tmrForLogauth: TTimer;
    tmrLogon: TTimer;
    Button1: TButton;
    procedure subComparisonClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure btnLogAuthClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure tmrForLogauthTimer(Sender: TObject);
    procedure tmrLogonTimer(Sender: TObject);
    procedure Button1Click(Sender: TObject);



  public
    { Public declarations }

  private
    procedure   Initialize();
    procedure   DeInitialize();
    procedure   Logon(sSockTp:string);
    PROCEDURE   SendLogon(sSockTp:string);


  private
    m_CheckThrd : TThread_Check;
  public
    m_sockRelayR : TSockets;
    m_sockRelayS : TSockets;
    m_sockData  : TSockets;
  end;

var
  fmMain: TfmMain;

implementation

uses
  ProtoSetU, ProtoGetU, uAlphaProtocol
  ;
{$R *.dfm}





procedure TfmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  __DeInitailzeCommon();
  DeInitialize();
end;

procedure TfmMain.FormCreate(Sender: TObject);
begin
  __QueueEx[Q_SEND_RELAY] := nil;
  __QueueEx[Q_SEND_DATA] := nil;

  m_sockRelayR  := nil;
  m_sockRelayS  := nil;
  m_sockData    := nil;
end;


procedure TfmMain.FormShow(Sender: TObject);
begin

  __InitailzeCommon();

  tmrForLogauth.Interval := 1000;
  tmrForLogauth.Enabled := true;
end;


procedure TfmMain.btnLogAuthClick(Sender: TObject);
begin

  fmLogin := TFmLogin.Create(self);
  fmLogin.ShowModal;
  //btnAuth.Enabled := false;

  if __Is_AuthDone()=true then
  BEGIN
    tmrLogon.Interval := 500;
    tmrLogon.Enabled  := true;
  END;


end;



procedure TfmMain.tmrLogonTimer(Sender: TObject);
begin
  tmrLogon.Enabled := false;

  DeInitialize();
  Initialize();

  LogOn(SOCKTP_RELAY_R);
  LogOn(SOCKTP_RELAY_S);
  //TODO
end;

PROCEDURE TfmMain.Logon(sSockTp: string);
begin

  if sSockTp = SOCKTP_RELAY_R then
  begin
    m_sockRelayR.sock.Initialize(_RelaySvrIP, _RelaySvrPort);

    if(m_sockRelayR.sock.ConnectSvr()=false) then
    begin
      __AddMsg('[R]Failed to connect Relay Svr:'+m_sockRelayR.sock.GetMsg, true);
      exit;
    end;
    m_sockRelayR.ThrdRecv.Resume;
    SendLogon(sSockTp);
  end

  else if sSockTp = SOCKTP_RELAY_S then
  begin
    m_sockRelayS.sock.Initialize(_RelaySvrIP, _RelaySvrPort);
    if(m_sockRelayS.sock.ConnectSvr()=false) then
    begin
      __AddMsg('[S]Failed to connect Relay Svr:'+m_sockRelayS.sock.GetMsg, true);
      exit;
    end;
    m_sockRelayS.ThrdRecv.Resume;
    SendLogon(sSockTp);
  end

  else if sSockTp = SOCKTP_DATA then
  begin
    m_sockData.sock.Initialize(_DataSvrIP, _DataSvrPort);
    //TODO
//    m_sockRelayS.sock.Initialize(_RelaySvrIP, _RelaySvrPort);
//    if(m_sockRelayS.sock.ConnectSvr()=false) then
//    begin
//      __AddMsg('[S]Failed to connect Relay Svr:'+m_sockRelayS.sock.GetMsg, true);
//      exit;
//    end;
//    SendLogon(sSockTp);
  end
  ;




  //todo. Data Server

end;

procedure   TfmMain.SendLogon(sSockTp:string);
var
  protoSet : TProtoSet;
  sSendBuf : string;
begin

  protoSet := TProtoSet.Create;
  protoSet.start();
  protoSet.SetVal(FDS_CODE, CODE_LOGON_AUTH);
  protoSet.SetVal(FDN_APP_TP, APPTP_SUITES);
  protoSet.SetVal(FDS_BROKER, 'BROKER');
  protoSet.SetVal(FDS_ACCNO_MY, 'ACCNO');
  protoSet.SetVal(FDS_USER_ID, _UserID);
  protoSet.SetVal(FDS_USER_PASSWORD, _Pwd);
  protoSet.SetVal(FDS_MAC_ADDR, 'MAC ADDR');
  protoSet.SetVal(FDS_LIVEDEMO, 'N');
  protoSet.SetVal(FDN_APP_TP, APPTP_SUITES);
  protoSet.SetVal(FDS_CLIENT_SOCKET_TP, sSockTp);
  protoSet.SetVal(FDS_KEY, _AppID);
  protoSet.Complete(sSendBuf);

  if sSockTp=SOCKTP_RELAY_R then      m_sockRelayR.sock.SendData(sSendBuf)
  else if sSockTp=SOCKTP_RELAY_S then m_sockRelayS.sock.SendData(sSendBuf)
  else if sSockTp=SOCKTP_DATA then    m_sockData.sock.SendData(sSendBuf)
  ;

  FreeAndNil(protoSet);
end;

procedure   TfmMain.Initialize();
begin

  DeInitialize();

  __QueueEx[Q_SEND_RELAY] := CQueueEx.Create;
  __QueueEx[Q_SEND_DATA] := CQueueEx.Create;

  m_sockRelayR := TSockets.Create(SOCKTP_RELAY_R);
  m_sockRelayS := TSockets.Create(SOCKTP_RELAY_S);
  m_sockData   := TSockets.Create(SOCKTP_DATA);

end;

procedure TfmMain.Button1Click(Sender: TObject);
begin
  tmrLogon.Interval := 500;
  tmrLogon.Enabled  := true;
end;

procedure TfmMain.DeInitialize();
begin

  if m_sockRelayR<>nil then FreeAndNil(m_sockRelayR);
  if m_sockRelayS<>nil then FreeAndNil(m_sockRelayS);
  if m_sockData<>nil then FreeAndNil(m_sockData);

  if __QueueEx[Q_SEND_RELAY]<>nil then FreeAndNil(__QueueEx[Q_SEND_RELAY]);
  if __QueueEx[Q_SEND_DATA]<>nil then FreeAndNil(__QueueEx[Q_SEND_DATA]);

end;


procedure TfmMain.subComparisonClick(Sender: TObject);
begin
  //__Form_Create(TfmComparison, fmComparison, FORM_MDI);
end;


procedure TfmMain.tmrForLogauthTimer(Sender: TObject);
begin
  tmrForLogauth.Enabled := false;

  btnLogAuthClick(self);

end;




////////////////////////////////////////////////////////////////////////////////
//
//
///
////////////////////////////////////////////////////////////////////////////////

constructor TSockets.Create(sSockTp:string);
begin
  sockTp := sSockTp;
  sock      := TTcpClient.Create;

  if sSockTp=SOCKTP_DATA then
    sock.Initialize(_DataSvrIP, _DataSvrPort)
  else
    sock.Initialize(_RelaySvrIP, _RelaySvrPort)
  ;
  ThrdSend  := TThread_Send.Create(true); ThrdSend.Init(sockTp);
  ThrdRecv  := TThread_Recv.Create(true); ThrdRecv.Init(sockTp);

end;

destructor TSockets.Destroy();
begin
  FreeAndNil(ThrdSend);
  FreeAndNil(ThrdRecv);
  FreeAndNil(sock);

  inherited;
end;


function TSockets.IsReady():boolean;
begin

  Result := true;
  if sock=NIL then
    Result := false;
end;


////////////////////////////////////////////////////////////////////////////////
//
//
///
////////////////////////////////////////////////////////////////////////////////

constructor TThread_Recv.Create(bSuspend:Boolean);
begin
  m_sockTp := '';
  inherited;
end;


procedure TThread_Recv.init(sSockTp:string);
begin
  m_sockTp := sSockTp;
  //Resume;
end;

procedure TThread_Recv.Execute;
var
  nRecvLen  : integer;
  sRecvData : string;
  i         : integer;
  childHandle : HWND;
  dRslt       : DWORD;
  pItem       : PTQItem;
  sPacketCode : string;
  sErrCode    : string;
  sErrMsg     : string;
begin

  while(not Terminated) do // and (fmMain.m_sockRelaySend.IsConnected) do
  begin

    Sleep(5);
    if m_sockTp='' then
      continue;

    nRecvLen  := 0;
    sRecvData := '';

    if m_sockTp=SOCKTP_RELAY_R then
    BEGIN
      if (fmMain.m_sockRelayR =NIL) or (fmMain.m_sockRelayR.IsReady()=false) then
        continue;
    END
    else if m_sockTp=SOCKTP_RELAY_S then
    BEGIN
      if (fmMain.m_sockRelayS=NIL) or (fmMain.m_sockRelayS.IsReady()=false) then
        continue;
    END

    //TODO
//    else if m_sockTp=SOCKTP_DATA then
//    BEGIN
//      if (fmMain.m_sockData=NIL) or (fmMain.m_sockData.IsReady()=false) then
//        continue;
//    END
    ;

    if m_sockTp=SOCKTP_RELAY_R then
      nRecvLen := fmMain.m_sockRelayR.sock.ReadData(sRecvData)

    else if m_sockTp=SOCKTP_RELAY_S then
      nRecvLen := fmMain.m_sockRelayS.sock.ReadData(sRecvData)

    else if m_sockTp=SOCKTP_DATA then
      nRecvLen := fmMain.m_sockData.sock.ReadData(sRecvData)
    ;

    if( nRecvLen=0 ) then
      continue;


    sPacketCode := __PacketCode(sRecvData);
    if sPacketCode = CODE_LOGON then
    BEGIN
      if __IsSuccess(sPacketCode, sErrCode, sErrMsg) = false then
      begin
        __AddMsg(Format('[%s]Logon err(%s)(%s)',[m_sockTp,sErrCode,sErrMsg]),true);
        continue;
      end;

    END;


    // Send socket 의 경우 LogOn 만 수신
    if m_sockTp=SOCKTP_RELAY_S THEN
    BEGIN
      //TODO
    END
    else if m_sockTp=SOCKTP_RELAY_R then
    begin

      for i := 0 to Application.MainForm.MDIChildCount - 1 do
      begin
        childHandle := NULL;

        childHandle := Application.MainForm.MDIChildren[i].Handle;

        New(pItem);
        pItem.data := sRecvData;
        SendMessageTimeout(childHandle,
                         WM_RECV_DATA,
                         wParam(LongInt(nRecvLen)),
                         lParam(LongInt(pItem)),
                         SMTO_NORMAL,
                         TIMEOUT_SENDMSG,
                         dRslt);

      end;
    end
    else if m_sockTp=SOCKTP_DATA then
    begin
      //TODO
    end;

  end;
end;


constructor TThread_Send.Create(bSuspend:Boolean);
begin
  m_sockTp := '';
  inherited;
end;

procedure TThread_Send.Init(sSockTp:string);
begin
  m_sockTp := sSockTp;
  Resume;
end;

procedure TThread_Send.Execute;
var
  pItem  : PTQItem;
  nCode  : integer;

  key     : string;
  data    : string;

  bRslt   : boolean;
begin
  while (not Terminated) do // and (fmMain.m_sockRelaySend.IsConnected) do
  begin
    Sleep(5);

    if m_sockTp='' then
      continue;

    if m_sockTp=SOCKTP_RELAY_R then
    BEGIN
      if (fmMain.m_sockRelayR =NIL) or (fmMain.m_sockRelayR.IsReady()=false) then
        continue;
    END
    else if m_sockTp=SOCKTP_RELAY_S then
    BEGIN
      if (fmMain.m_sockRelayS=NIL) or (fmMain.m_sockRelayS.IsReady()=false) then
        continue;
    END

    //TODO
//    else if m_sockTp=SOCKTP_DATA then
//    BEGIN
//      if (fmMain.m_sockData=NIL) or (fmMain.m_sockData.IsReady()=false) then
//        continue;
//    END
    ;


    if m_sockTp=SOCKTP_RELAY_S then
      pItem := __QueueEx[Q_SEND_RELAY].Get()
    else
      pItem := __QueueEx[Q_SEND_DATA].Get()
    ;

    if pItem <> NIL then
    begin

      //nCode  := pItem.nCode;
      key    := pItem.sCode;
      data   := pItem.data;
      Dispose(pItem);

      if m_sockTp=SOCKTP_RELAY_S then
      begin
        __AddMsg(format('[Get SendQ-Relay](%s)', [data]));
        if fmMain.m_sockRelayS.sock.SendData(data)=false then
        begin
          __AddMsg(format('[Failed to Send Relay](%s)', [fmMain.m_sockRelayS.sock.GetMsg()]));
        end;
      end
      else
      begin
        __AddMsg(format('[Get SendQ-Data](%s)', [data]));
        if fmMain.m_sockData.sock.SendData(data)=false then
        begin
          __AddMsg(format('[Failed to Send-Data](%s)', [fmMain.m_sockData.sock.GetMsg()]));
        end;

      end
      ;

    end;

  end;
end;



procedure TThread_Check.Execute;
var
  pItem  : PTQItem;
  nCode  : integer;

  key     : string;
  data    : string;
begin
  while (not Terminated) do // and (fmMain.m_sockRelaySend.IsConnected) do
  begin

    if fmMain.m_sockRelayR.sock.IsConnected=false then
    begin
      __AddMsg('connection for receiving is closed',true);
      //TODO fmMain.btnStart.Enabled := true;
    end;

    if fmMain.m_sockRelayS.sock.IsConnected=false then
    begin
      __AddMsg('connection for sending is closed',true);
      //fmMain.btnStart.Enabled := true;
    end;

    //TODO
//    if fmMain.m_sockData.sock.IsConnected=false then
//    begin
//      __AddMsg('connection for sending is closed',true);
//      //fmMain.btnStart.Enabled := true;
//    end;

  end;
end;

end.
