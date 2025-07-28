unit uMastDB;

interface

uses
  System.SysUtils, System.Classes
  ,IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient
  ,CommonUtils, Vcl.Dialogs, IdThreadComponent
  ,XAlphaPacket
  ;

type



  TMastDB = class(TDataModule)
    IdTCPClient1: TIdTCPClient;
    IdThreadComponent1: TIdThreadComponent;
    procedure DataModuleCreate(Sender: TObject);

    function  Initialize():boolean;
    procedure IdTCPClient1Connected(Sender: TObject);
    procedure IdTCPClient1Disconnected(Sender: TObject);
    procedure DataModuleDestroy(Sender: TObject);
    procedure IdThreadComponent1Run(Sender: TIdThreadComponent);
    Procedure  SendData(sData:string; len:integer);

    function  IsConnected():boolean;
    function ConnectToSv():boolean;
    
  private
    { Private declarations }
    m_bConnected : boolean;
  public
    { Public declarations }
    m_CnfgName  : string;
  end;

var
  __Mast     : TMastDB;
  __PacketQ  : CPacketQueue;


implementation

{%CLASSGROUP 'Vcl.Controls.TControl'}

{$R *.dfm}

procedure TMastDB.DataModuleCreate(Sender: TObject);
var
  sSvrIp
  ,sSvrPort : string;
begin

  m_bConnected := False;
   
end;



function  TMastDB.IsConnected():boolean;
begin
  Result := m_bConnected;
end;

function TMastDB.ConnectToSv():boolean;
var
  sSvrIp
  ,sSvrPort : string;
begin

  m_bConnected := False;
  
  Result := True;
  
  m_CnfgName := __Get_CFGFileName();

  sSvrIp    := __Get_CFGFile('SERVER', 'IP', '', False, m_CnfgName);
  sSvrPort  := __Get_CFGFile('SERVER', 'PORT', '', false, m_CnfgName);

  if (sSvrIp='') or (sSvrPort='') then
  begin
    showmessage('SERVER 정보 없음');
    Result := False;
    exit;
  end;

  
  IdTCPClient1.Host  := sSvrIp;
  IdTCPClient1.Port  := strtoint(sSvrPort);
  IdTCPClient1.Connect;

end;

    
function  TMastDB.Initialize():boolean;

begin

  __PacketQ := CPacketQueue.Create();

  if ConnectToSv=False then
    exit;
  
end;


procedure TMastDB.DataModuleDestroy(Sender: TObject);
begin
    __PacketQ.Destroy();

      if idThreadComponent1.active then begin
       idThreadComponent1.active := False;
    end;


end;

procedure TMastDB.IdTCPClient1Connected(Sender: TObject);
var
  s : string;
begin

  IdThreadComponent1.Active  := True;

  m_bConnected := True;
end;

procedure TMastDB.IdTCPClient1Disconnected(Sender: TObject);
begin
  showmessage('disconn');
  m_bConnected := False;
  IdThreadComponent1.Active := false;
end;

procedure TMastDB.IdThreadComponent1Run(Sender: TIdThreadComponent);
var
    s : string;
begin

  try
    // ... read message from server
    s := IdTCPClient1.IOHandler.ReadLn();
  except
      
      on e: Exception do
      begin
         m_bConnected := false;
         IdThreadComponent1.Active := false;
         exit;
      end;
  end;

  __PacketQ.Add(s);

end;


procedure TMastDB.SendData(sData:string; len:integer);
begin
  IdTCPClient1.IOHandler.Write(sdata);
end;

end.
