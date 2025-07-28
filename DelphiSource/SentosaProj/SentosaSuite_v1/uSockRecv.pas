unit uSockRecv;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes,
  IdComponent, IdTCPConnection, IdTCPClient,
  uQueueEx
  ;

type
  TSockRecv = class(TThread)
  public
    constructor Create();
    function ThreadId():cardinal;

    function Initialize(sSvrIP:string; nPort:integer):boolean;
    function Connect():boolean;
    function GetMsg():string;
  protected
    procedure Execute();override;

    procedure IdTCPClientConnected(Sender: TObject);
    procedure IdTCPClientDisconnected(Sender: TObject);


  private
    m_idTCPClient   : TIdTCPClient;
    m_sSvrIP        : string;
    m_nPort         : integer;
    m_sMsg          : string;

  end;

implementation

//uses
//  uQueueEx
//  ;

const
  THREAD_SUSPENDED = true;


constructor TSockRecv.Create;
begin
  inherited Create(THREAD_SUSPENDED);
  FreeOnTerminate := true;      // ndicates whether the thread should free itself when it stops executing
end;

function TSockRecv.ThreadId():cardinal;
begin
  Result := ThreadID;   // TThread
end;

function TSockRecv.Initialize(sSvrIP:string; nPort:integer):boolean;
begin
  Result := true;
  try
  m_sSvrIP := sSvrIP;
  m_nPort  := nPort;

  m_idTCPClient := TIdTCPClient.Create();

  // ... set properties
  m_idTCPClient.Host := m_sSvrIP;
  m_idTCPClient.Port := m_nPort;

  // ... callback functions
  m_idTCPClient.OnConnected     := IdTCPClientConnected;
  m_idTCPClient.OnDisconnected  := IdTCPClientDisconnected;

  Except
    Result := false;
  end;

end;


function TSockRecv.Connect():boolean;
begin
  Result := true;
  try
    m_IdTCPClient.Connect;
  except
    on E: Exception do
    begin
        m_sMsg := 'CONNECTION ERROR! ' + E.Message;
        Result := false;
    end;
  end;




end;

procedure TSockRecv.Execute();
begin
  Result := '';
  nReadLen :=0;
  try
    sRecvData := m_sock.IOHandler.ReadLn();

    nReadLen =  StrLen(sRecvData);
  except
    //TODO. LOGGING
    exit;
  end;

  Result := sRecvData;
  Inherited;
end;

procedure TSockRecv.IdTCPClientConnected(Sender: TObject);
begin
  Resume();
end;

procedure TSockRecv.IdTCPClientDisconnected(Sender: TObject);
begin

  Suspend();

end;


function TSockRecv.GetMsg():string;
begin
  Result := m_sMsg;
end;



end.
