
unit uTcpSvr;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs
  ,IdExceptionCore, IdCustomTCPServer, IdTCPServer, IdContext, IdComponent, IdIoHandler, IdGlobal
  ,ProtoGetU, uAlphaProtocol, uQueueEx
  ;


const
  Q_RECV = 0;
  Q_SEND = 1;

type

  ClientCtx = class(TObject)
  public
    iTerminal : integer;  // iTerminal
    mcTp      : string;
  end;

  TSendThrd = class(TThread)
  protected
    procedure Execute();override;
  end;

  TTcpSvr = class(TObject)
  public
    function StartSvr(sIp:string; nPort:integer):boolean;
    procedure StopSvr();

    function  GetRecvData(var iTerminal:integer;var sCode:string; var sRecvBuf:string):integer;
    procedure  SendData(iTerminal:integer; sCode,sSendBuf:string);

    function  GetMsg():string;

  private
    procedure IdTCPServerConnect(AContext: TIdContext);
    procedure IdTCPServerDisconnect(AContext: TIdContext);
    procedure IdTCPServerExecute(AContext: TIdContext);
    procedure IdTCPServerStatus(ASender: TObject; const AStatus: TIdStatus;
                                const AStatusText: string);
    procedure ClientDiscon(ASender: TObject);
    procedure ShowNumberOfClients(bDisconn:boolean);
    //procedure IdTCPServerException(AContext: TIdContext; AException: Exception);

    procedure LoginProc(var inRecvBuf:string; var AContext: TIdContext);
    function  Ctx_Get(iTerminal:integer; var ctx:TIdContext):boolean;
    procedure Send2All(sData:string; bIncludeMaster:boolean);

  private
    m_svr       : TIdTCPServer;
    m_SendThrd  : TSendThrd;
    m_bTcpContinue : boolean;
    m_sIp          : string;
    m_nPort        : integer;
    m_sMsg         : string;
    m_nClientCnt   : integer;
    m_queue        : array[0..1] of CQueueEx;
  end;

var
  __tcpSvr : TTcpSvr;

  function __CreateSvrSocket(sIp:string; nPort:integer):boolean;
  procedure __DestroySvrSocket();

implementation

uses
  fmMainU, uLocalCommon
  ;


function __CreateSvrSocket(sIp:string; nPort:integer):boolean;
begin
  __tcpSvr := TTcpSvr.Create;
  __tcpSvr.m_queue[Q_RECV] := CQueueEx.create;
  __tcpSvr.m_queue[Q_SEND] := CQueueEx.create;
  Result  := __tcpSvr.StartSvr(sIp, nPort);
end;

procedure __DestroySvrSocket();
begin
  __tcpSvr.StopSvr;
  FreeAndNil(__tcpSvr);
end;


function TTcpSvr.StartSvr(sIp:string; nPort:integer):boolean;
begin

  Result := true;

  try
    m_sIp   := sIp;
    m_nPort := nPort;

    m_nClientCnt  := 0;

    m_queue[Q_RECV] := CQueueEx.Create;
    m_queue[Q_SEND] := CQueueEx.Create;

    m_svr         := TIdTCPServer.Create();
    m_svr.Active  := False;

    m_svr.OnConnect       := IdTCPServerConnect;
    m_svr.OnDisconnect    := IdTCPServerDisconnect;
    m_svr.OnExecute       := IdTCPServerExecute;
    m_svr.OnStatus        := IdTCPServerStatus;

    m_svr.Bindings.Clear;
    m_svr.Bindings.Add.Ip   := sIp;
    m_svr.Bindings.Add.Port := m_nPort;

    m_bTcpContinue := True;

    m_svr.Active   := True;

    m_SendThrd := TSendThrd.Create;

  except
    m_sMsg := 'exceptin in creating server socket';
    Result := false;
  end;
end;


procedure TTcpSvr.StopSvr();
begin

  m_bTcpContinue := False;

  if Assigned(m_SendThrd) then
  begin
    m_SendThrd.Terminate;
    FreeAndNil(m_SendThrd);
  end;


  if Assigned(__tcpSvr) then
  begin

    if m_svr.Active then
      m_svr.Active := False;

    FreeAndNil(m_queue[Q_RECV]);
    FreeAndNil(m_queue[Q_SEND]);

    FreeAndNil(m_svr);
  end;
end;



// event handler of connect(accept)
procedure TTcpSvr.IdTCPServerConnect(AContext: TIdContext);
//var
//    ip          : string;
//    port        : Integer;
//    peerIP      : string;
//    peerPort    : Integer;
begin

    // ... getting IP address and Port of Client that connected
//    ip        := AContext.Binding.IP;
//    port      := AContext.Binding.Port;
//    peerIP    := AContext.Binding.PeerIP;
//    peerPort  := AContext.Binding.PeerPort;

    AContext.Connection.OnDisconnected := ClientDiscon;
    AContext.Connection.IOHandler.ReadTimeout := 100;

    // ... message log
//    AddMsg('Client Connected!' + 'Port=' + IntToStr(Port)
//                      + ' '   + '(PeerIP=' + PeerIP
//                      + ' - ' + 'PeerPort=' + IntToStr(PeerPort) + ')'
//           );

    // ... display the number of clients connected
    ShowNumberOfClients(false);

end;
// .............................................................................


procedure TTcpSvr.ClientDiscon(ASender: TObject);
begin
  //showmessage('ClientDiscon');
end;

// *****************************************************************************
//   EVENT : onDisconnect()
//           OCCURS ANY TIME A CLIENT IS DISCONNECTED
// *****************************************************************************
procedure TTcpSvr.IdTCPServerDisconnect(AContext: TIdContext);
var
  peerIP      : string;

begin

    // ... getting IP address and Port of Client that connected
    peerIP    := AContext.Binding.PeerIP;

    // ... message log
    //AddMsg(Format('Client Disconnected(Key:%s)(%s)', [ClientCtx(AContext.Data).sKey, peerIP]));

    // ... display the number of clients connected
    ShowNumberOfClients(true);
end;
// .............................................................................


procedure TTcpSvr.LoginProc(var inRecvBuf:string; var AContext: TIdContext);
var
  mcTp      : string;

  iTerminal : integer;
  mcTerminal: string;
  sendBuf   : string;
  clCtx     : ClientCtx;
begin

  if __FindClientTerminalIdx(inRecvBuf, iTerminal, mcTerminal)=False  then
  begin
    //fmMain.AddMsg(format('failed to find client idx(%s)',[inRecvBuf]),false, true);
    fmMain.AddMsg('failed to find client idx');
    __Set_LoginErrBuffer(sendBuf);
    AContext.Connection.IOHandler.Write(sendBuf);  // send error
    fmMain.AddMsg(format('[Return Login Err](%s)', [sendBuf]));
    exit;
  end;


  mcTp := __GetValue(inRecvBuf, FDS_MASTERCOPIER_TP);

  // M, C 가 안맞는 경우 Send return
  if mcTp<>mcTerminal then
  begin
    fmMain.AddMsg(format('Master Copier type unmatched(EA:%s, App:%s)',[mcTp, mcTerminal]));
    __Set_LoginErrBuffer(sendBuf);
    AContext.Connection.IOHandler.Write(sendBuf);  // send error
    fmMain.AddMsg(format('[Return Login Err](%s)', [sendBuf]));
    exit;
  end;


  // M, C 가 맞는 경우 Send return
  clCtx := ClientCtx.Create;
  clCtx.iTerminal := iTerminal;
  clCtx.mcTp      := mcTp;
  AContext.Data   := clCtx;

  __Set_LoginOkBuffer(iTerminal, sendBuf);
  AContext.Connection.IOHandler.Write(sendBuf);
  fmMain.AddMsg(format('LoginOk(%s)',[sendBuf]));

end;

// receive routine
procedure TTcpSvr.IdTCPServerExecute(AContext: TIdContext);
var
  sRecvBuf: string;

  code    : string;
  iTerminal : integer;
  sendBuf   : string;
begin

  while m_bTcpContinue do
  begin
    Sleep(10);

    try
      sRecvBuf := '';
      sRecvBuf := AContext.Connection.IOHandler.ReadLn;
      if length(sRecvBuf)>0 then
      begin

        code := __PacketCode(sRecvBuf);

        // Login 이면 IdContext 저장
        if code=CODE_LOGON then
        begin

          LoginProc( sRecvBuf, AContext );
          continue;

        end;

        iTerminal := strtointdef(__GetValue(sRecvBuf, FDN_TERMINAL_IDX),-1);
        if iTerminal<0 then
        begin
          fmMain.AddMsg(format('No Terminal Idx in the packet from EA(%s)',[sRecvBuf]));
          __Set_WrongIdxBuffer(sendBuf);
          AContext.Connection.IOHandler.Write(sendBuf);
          continue;
        end;

        __QueueEx[Q_RECV].Add(iTerminal, code, sRecvBuf);

      end;

    except
      //fmMain.AddMsg('exception during proocessing of received data', false , true);
      //exit;
    end;
  end;


end;
// .............................................................................


// *****************************************************************************
//   EVENT : onStatus()
//           ON STATUS CONNECTION
// *****************************************************************************
procedure TTcpSvr.IdTCPServerStatus(ASender: TObject; const AStatus: TIdStatus;
                                     const AStatusText: string);
begin
    // ... OnStatus is a TIdStatusEvent property that represents the event handler
    //     triggered when the current connection state is changed...

    // ... message log
    //AddMsg('Status:'+AStatusText);
end;

procedure TTcpSvr.ShowNumberOfClients(bDisconn:boolean);
begin

    try
        // ... get number of clients connected
        m_nClientCnt := m_svr.Contexts.LockList.Count;
    finally
        m_svr.Contexts.UnlockList;
    end;

    // ... client disconnected?
    if bDisconn then dec(m_nClientCnt);

    //AddMsg('Client Count:'+inttostr(nclients));
end;


function  TTcpSvr.GetRecvData(var iTerminal:integer;var sCode:string; var sRecvBuf:string):integer;
VAR
  pItem : PTQItem;
begin

  Result := 0;

  pItem := m_queue[Q_RECV].Get();
  if pItem<>NIL then
  BEGIN
    iTerminal := pItem.nKey;
    sCode     := pItem.sCode;
    sRecvBuf  := pItem.data;
    Result    := length(sRecvBuf);

    Dispose(pItem);
  END;


end;

procedure TTcpSvr.SendData(iTerminal:integer; sCode,sSendBuf:string);
begin
  m_queue[Q_SEND].Add(iTerminal, sCode, sSendBuf);
end;



function  TTcpSvr.GetMsg():string;
begin
  Result := m_sMsg;
end;



function  TTcpSvr.Ctx_Get(iTerminal:integer; var ctx:TIdContext):boolean;
var
  i   : integer;
  lstConn : TLIST;
begin
  Result := False;

  //cnt := IdTCPServer.Contexts.Count;
  lstConn := m_svr.Contexts.LockList;
  try
   for i := 0 to lstConn.Count-1 do
    begin
      if ClientCtx(TIdContext(lstConn[i]).Data).iTerminal = iTerminal then
      begin
        ctx := TIdContext(lstConn[i]);
        Result := true;
        exit;
      end;

    end;
  finally
    m_svr.Contexts.UnlockList;
  end;

end;


// 모든 user 에게 전송
procedure TTcpSvr.Send2All(sData:string; bIncludeMaster:boolean);
var
  i   : integer;
  lstConn : TLIST;
begin


  lstConn := m_svr.Contexts.LockList;
  try
   for i := 0 to lstConn.Count-1 do
    begin

      if bIncludeMaster=False then
      begin
        if ClientCtx(TIdContext(lstConn[i]).Data).mcTp='M' then
          continue;
      end;

      TIdContext(lstConn[i]).Connection.IOHandler.Write(sData);

    end;
  finally
    m_svr.Contexts.UnlockList;
  end;

end;


procedure TSendThrd.Execute;
var
  pItem     : PTQItem;
  iTerminal : integer;
  sCode     : string;
  data      : string;
  etc       : string;

  ctx     : TIdContext;
begin

  while (not Terminated) and (__tcpSvr.m_svr.Active) do
  begin
    Sleep(10);

    pItem := __tcpSvr.m_queue[Q_SEND].Get();

    if pItem <> NIL then
    begin

      iTerminal   := pItem.nKey;
      sCode       := pItem.sCode;
      data        := pItem.data;
      etc         := pItem.etc;
      Dispose(pItem);

      //fmMain.AddMsg(format('[Get SendQ](%s)', [data]));
      if (iTerminal=ALL_CLIENTS) OR (iTerminal=ALL_COPIERS) then
      BEGIN
        __tcpSvr.Send2All(data, (iTerminal=ALL_CLIENTS));
        continue;
      END;



      if __tcpSvr.Ctx_Get(iTerminal, ctx) then
      begin
        try
          ctx.Connection.IOHandler.Write(data);
          fmMain.AddMsg(format('[send](%s)', [data]));
        except
          fmMain.AddMsg(format('[send-except](%d)', [iTerminal]));
        end;
      end;

    end;

  end;

end;

end.
