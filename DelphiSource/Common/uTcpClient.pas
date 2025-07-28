unit uTcpClient;

interface

uses
  System.Classes, uAlphaProtocol, windows, system.SysUtils
  ,ProtoGetU, ProtoSetU, winapi.messages, vcl.forms, IdTCPClient
  ;

type

  // receive login packet and return to mt4
  TTcpClient = class
  public
    function  Initialize(sSvrIP:string; sSvrPort:string):boolean;
    function  ConnectSvr():boolean;
    procedure Disconnect();
    function  IsConnected():boolean;

    constructor Create();
    destructor Destroy();override;

    function GetMsg():string;
    function ReadData(var sRecvData:string):integer;
    function SendData(sData:string):boolean;
  private
    procedure OnConnected(Sender: TObject);
    procedure OnDisconnected(Sender: TObject);
  private
    m_sock      : TIdTCPClient;
    m_sSvrIp    : string;
    m_nSvrPort  : integer;

    m_cs        : TRTLCriticalSection;
    m_sMsg      : string;
  end;


//var
//  __realPLOrd : array [1..MAX_SYMBOL] of TRealPLOrder;
//
//  procedure __CreatRealPLOrder(iSymbol:integer);
//  procedure __DeployMD(iSymbol:integer);

implementation

//uses
//  uQueueEx, fmMainU, uCtrls, CommonUtils, uPostThread
//  ;




constructor TTcpClient.Create;
begin
  m_sock  := NIL;
  InitializeCriticalSection(m_cs);

end;

destructor TTcpClient.Destroy();
begin
    Disconnect();
    FreeAndNil(m_sock);
    DeleteCriticalSection(m_cs);
  inherited;
end;

function TTcpClient.Initialize(sSvrIP:string; sSvrPort:string):boolean;
begin
  Result := true;

  try

    m_sSvrIp    := sSvrIP;
    m_nSvrPort  := strtoint(sSvrPort);

    m_sock := TIdTCPClient.Create();

    m_sock.Host := m_sSvrIp;
    m_sock.Port := m_nSvrPort;

    // ... callback functions
    m_sock.OnConnected     := OnConnected;
    m_sock.OnDisconnected  := OnDisconnected;



  except
    Result := false;
  end;

end;


function  TTcpClient.ConnectSvr():boolean;
begin
  Result := true;
  try
    m_sock.Connect;
  except
    on E: Exception do
    begin
        m_sMsg := 'CONNECTION ERROR! ' + E.Message;
        Result := false;
    end;
  end;
end;



function TTcpClient.ReadData(var sRecvData:string):integer; // must be run in a thread
begin

  Result := -1;

  try
    sRecvData := m_sock.IOHandler.ReadLn();

    Result :=  Length(sRecvData);
  except
    on E: Exception do
    begin
        m_sMsg := 'Recv ERROR! ' + E.Message;
        Result := 0;
    end;
  end;

end;

function TTcpClient.SendData(sData: string):boolean;
begin

  Result := true;
  try
    m_sock.IOHandler.Write(sData);
  except
    Result := false;
  end;

end;


procedure TTcpClient.Disconnect();
begin
  m_sock.Disconnect;
end;

procedure TTcpClient.OnConnected(Sender: TObject);
begin
    // ... messages log
    m_sMsg := 'Connected!';
end;

procedure TTcpClient.OnDisConnected(Sender: TObject);
begin
    // ... messages log
    m_sMsg := 'DisConnected!!!';
end;

function TTcpClient.GetMsg():string;
begin
  Result := m_sMsg;
end;

function TTcpClient.IsConnected():boolean;
begin
  Result := m_sock.Connected;
end;

end.

