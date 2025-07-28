unit uTcpTest;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs
  ,IdTCPServer, IdBaseComponent, IdComponent, IdCustomTCPServer, IdContext,  IdServerIOHandlerStack, IDIOHandler,
  Vcl.StdCtrls, IdServerIOHandler, IdServerIOHandlerSocket
  ;

type
  TfmTcpTest = class(TForm)
    IdTCPServer1: TIdTCPServer;
    Memo1: TMemo;
    IdServerIOHandlerStack1: TIdServerIOHandlerStack;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure IdTCPServer1Connect(AContext: TIdContext);
    procedure IdTCPServer1Disconnect(AContext: TIdContext);
    procedure IdTCPServer1Exception(AContext: TIdContext;
      AException: Exception);
    procedure IdTCPServer1Execute(AContext: TIdContext);
    procedure IdTCPServer1Status(ASender: TObject; const AStatus: TIdStatus;
      const AStatusText: string);
  private
    { Private declarations }
    m_buf : string;

    procedure AddMsg(msg:string);
  public
    { Public declarations }
  end;

var
  fmTcpTest: TfmTcpTest;

implementation

{$R *.dfm}

procedure TfmTcpTest.FormCreate(Sender: TObject);
begin

  ///////////////////////////////////////////////////
  ///  socket
  IdTCPServer1.Active          := False;

  IdTCPServer1.Bindings.Clear;
  // ... Bindings is a property of class: TIdSocketHandles;

  // ... add listening ports:

  // ... add a port for connections from guest clients.
  IdTCPServer1.Bindings.Add.Port := 20010;
  IdTCPServer1.Bindings.Add.IP   := '127.0.0.1';

  // ... etc..


  // ... ok, Active the Server!
  IdTCPServer1.Active   := True;

end;

procedure TfmTcpTest.FormShow(Sender: TObject);
begin
//
end;

procedure TfmTcpTest.IdTCPServer1Connect(AContext: TIdContext);
begin
  AddMsg(format('connect - %d', [idtcpserver1.Contexts.LockList.Count]));
end;

procedure TfmTcpTest.IdTCPServer1Disconnect(AContext: TIdContext);
begin
  AddMsg( format('disconnect - %d', [idtcpserver1.Contexts.LockList.Count]));

end;

procedure TfmTcpTest.IdTCPServer1Exception(AContext: TIdContext;
  AException: Exception);
begin
    AddMsg( 'exception happened');

end;

procedure TfmTcpTest.IdTCPServer1Execute(AContext: TIdContext);
var
    Port          : Integer;
    PeerPort      : Integer;
    PeerIP        : string;

    msgFromClient : string;
    msgToClient   : string;
  IO : TIdIOHandler;
  s1 : string;
  bytestoreceive: Integer;
begin
//                 IO := AContext.Connection.IOHandler;
//
//  try
//    bytestoreceive := IO.InputBuffer.Size;
//    if bytestoreceive > 0 then
//    begin
//      msgFromClient := IO.ReadString(bytestoreceive);
//      IO.InputBuffer.Clear;
//      AddMsg('CLIENT:'+ msgFromClient);
//    end;
//  except
//
//  end;


    // ... get message from client
    msgFromClient := AContext.Connection.IOHandler.ReadLn;


    // ... message log
    AddMsg('CLIENT:'+ msgFromClient);
//    // ...
//
//    // ... process message from Client
//
//    // ...
//
//    // ... send response to Client
//
//    //AContext.Connection.IOHandler.WriteLn('... message sent from server :)');
end;


procedure TfmTcpTest.IdTCPServer1Status(ASender: TObject;
  const AStatus: TIdStatus; const AStatusText: string);
begin
  AddMsg(AStatusText);
end;

procedure TfmTcpTest.AddMsg(msg:string);
begin
  TThread.Queue(nil, procedure
                       begin
                           memo1.Lines.insert(0, msg);
                       end
                 );
end;

end.
