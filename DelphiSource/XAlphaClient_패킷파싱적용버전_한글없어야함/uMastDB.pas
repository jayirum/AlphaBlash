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
  private
    { Private declarations }
  public
    { Public declarations }
    m_CnfgName  : string;
    m_PacketQ   : CPacketQueue;
  end;

var
  __Mast      : TMastDB;


implementation

{%CLASSGROUP 'Vcl.Controls.TControl'}

{$R *.dfm}

procedure TMastDB.DataModuleCreate(Sender: TObject);
var
  sSvrIp
  ,sSvrPort : string;
begin

  m_PacketQ := CPacketQueue.Create();
   
end;

function  TMastDB.Initialize():boolean;
var
  sSvrIp
  ,sSvrPort : string;
begin

  m_CnfgName := __Get_CFGFileName();

  sSvrIp    := __Get_CFGFile('SERVER', 'IP', '', False, m_CnfgName);
  sSvrPort  := __Get_CFGFile('SERVER', 'PORT', '', false, m_CnfgName);

  if (sSvrIp='') or (sSvrPort='') then
  begin
    showmessage('SERVER 정보 없음');
    exit;
  end;


  IdTCPClient1.Host  := sSvrIp;
  IdTCPClient1.Port  := strtoint(sSvrPort);
  IdTCPClient1.Connect;

end;


procedure TMastDB.DataModuleDestroy(Sender: TObject);
begin
      if idThreadComponent1.active then begin
       idThreadComponent1.active := False;
    end;

    m_PacketQ.Destroy();
end;

procedure TMastDB.IdTCPClient1Connected(Sender: TObject);
var
  s : string;
begin
  showmessage('connect ok');
  IdThreadComponent1.Active  := True;


end;

procedure TMastDB.IdTCPClient1Disconnected(Sender: TObject);
begin
  showmessage('disconn');
end;

procedure TMastDB.IdThreadComponent1Run(Sender: TIdThreadComponent);
var
    s : string;
begin
    
    // ... read message from server
    s := IdTCPClient1.IOHandler.ReadLn();

    m_PacketQ.Add(s);
end;

end.
