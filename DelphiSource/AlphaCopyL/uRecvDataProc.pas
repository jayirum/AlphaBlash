{
  Q 에서 데이터 읽어서 처리
}
unit uRecvDataProc;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs
  ,IdExceptionCore, IdCustomTCPServer, IdTCPServer, IdContext, IdComponent, IdIoHandler, IdGlobal
  ,ProtoGetU, uAlphaProtocol, uQueueEx
  ;


type

  TRecvDataProc = class(TThread)
  public
    constructor Create();
    destructor Destroy();

    procedure StopThread();
  protected
    procedure Execute();override;

  private
    m_bContinue : boolean;

  end;

var
  __recvProc : TRecvDataProc;

  procedure __CreateRecvDataProc();
  procedure __DestroyRecvDataProc();

implementation

uses
  fmMainU, uLocalCommon, uTcpSvr, MTLoggerU
  ;


procedure __CreateRecvDataProc();
begin
  __recvProc := TRecvDataProc.Create;

end;

procedure __DestroyRecvDataProc();
begin
  __recvProc.StopThread;
  FreeAndNil(__recvProc);
end;

procedure TRecvDataProc.StopThread();
begin
  m_bContinue := false;
end;

constructor TRecvDataProc.Create();
begin
  m_bContinue := True;
  inherited;
end;

destructor TRecvDataProc.Destroy();
begin
  StopThread();
end;

procedure TRecvDataProc.Execute;
var
  iTerminal : integer;
  sCode     : string;
  sData     : string;
  nDataLen  : integer;
begin

  while (m_bContinue=True) and (terminated=False) do
  begin

    Sleep(10);

    nDataLen := __tcpSvr.GetRecvData(iTerminal, sCode, sData);
    if nDataLen<=0 then
      continue;

    // 주문은 그냥 bypass
    if ScODE=CODE_MASTER_ORDER then
    BEGIN
      //LOGGING
      __log.log(INFO, format('[MASTER_ORD](%s)',[sData]));

      // publishing
      __tcpSvr.SendData(ALL_COPIERS, sCode, sData);

    END;

  end;

end;

end.
