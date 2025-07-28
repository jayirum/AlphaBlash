unit uPacketParse;

interface

uses
    XAlphaPacket, classes, windows, sysutils, uMastDB
    ;

type
  CPacketParse = class(TThread)
  public
    constructor Create(fmMainHandle:HWND);
    destructor Destory();
  protected
      procedure Execute(); override;
  private
      m_mainHandle : HWND;
      m_bContinue  : Boolean;
  end;

implementation




constructor CPacketParse.Create(fmMainHandle: HWND);
begin
    m_mainHandle := fmMainHandle;
    m_bContinue  := True;
    inherited Create(False);
end;

destructor  CPacketParse.Destory;
begin
  m_bContinue := False;
  inherited Destroy();
end;


procedure CPacketParse.Execute;
var
  asRcvPacket : AnsiString;
  nRcvLen   : integer;
  nRslt     : integer;
  sMsg      : string;

  paSendBuf : PAnsiChar;
begin
  if Terminated then Exit;

  while m_bContinue  do
  begin

    asRcvPacket := '';
    nRslt := __Mast.m_PacketQ.GetOnePacket(asRcvPacket, nRcvLen);
    if __Mast.m_PacketQ.IsFailed(nRslt)=false then
    begin
      if nRcvLen>0 then
      begin
        GetMem(paSendBuf, nRcvLen+1);
        CopyMemory(paSendBuf, PAnsiChar(asRcvPacket), nRcvLen);
        SendMessage( m_mainHandle, WM_NEW_PACKET, WPARAM(nRcvLen+1), LPARAM(paSendBuf));
      end;
    end
    else
    begin
      //TODO
    end;


  end;

end;

end.
