unit uPacketProcess;

interface

uses
  System.Classes, uAlphaProtocol
  , uLoginProcess, uMDProcess, uPosDataProcess, uSymbolSpecProcess
  , uRealPLOrder
  ;

type

  CPacketProcess = class(TThread)
  public
    constructor Create();
    destructor destroy();override;
  protected
    procedure Execute();override;

  private
     m_login  : TLoginThrd;
     m_md     : TMarketDataThrd;
     m_symbol : TSymbolSpecThrd;
     m_pos    : TPosDataThrd;

     m_bContinue : boolean;
  end;


var
  __process : CPacketProcess;

implementation

uses
  uQueueEx, uTwoWayCommon, fmMainU, ProtoGetU, uPostThread
  ;


constructor CPacketProcess.Create;
begin
  m_bContinue := true;

  m_login  := TLoginThrd.Create();
  m_md     := TMarketDataThrd.Create();
  m_symbol := TSymbolSpecThrd.Create();
  m_pos    := TPosDataThrd.Create();

  inherited;
end;

destructor  CPacketProcess.Destroy;
begin
  // postthreadmessage to workers
  m_bContinue := false;
  m_login.Terminate;
  m_md.Terminate;
  m_symbol.Terminate;
  m_pos.Terminate;

end;


procedure CPacketProcess.Execute();
var
  nCode : integer;
  sCode : string;
  data      : string;
  ansiData  : string;
  ret       : integer;
  pItem     : PTQItem;
begin

  while (not terminated) and (m_bcontinue) do
  begin
    Sleep(10);

    data  := '';
    pItem := __QueueEx[Q_RECV].Get();

    if pItem <> NIL then
    begin
      //nCode := pItem.nCode;
      sCode := pItem.sCode;
      data  := pItem.data;
      Dispose(pItem);

      // 패킷 코드에 따라 각 스레드에게 POST
      //sCode := __PacketCode(data);

      if sCode = CODE_LOGON  then
        __PostThreadMsg(pItem, m_login.ThreadId(), WM_LOGON, AnsiString(data), Length(data) )
      //__PostThreadMsg(pItem:PTQItem; thrdID:cardinal; wmMsg:cardinal; ansiData:ansistring; dataLen:integer);
      else if sCode = CODE_SYMBOL_SPEC  then
        __PostThreadMsg(pItem, m_symbol.ThreadId(), WM_SYMBOL, AnsiString(data), Length(data) )

      else if sCode = CODE_MARKET_DATA then
        __PostThreadMsg(pItem, m_md.ThreadId(), WM_MD, AnsiString(data), Length(data) )

      else if sCode = CODE_POSITION then
        __PostThreadMsg(pItem, m_pos.ThreadId(), WM_POSITION, AnsiString(data), Length(data) )
      ;
    end;


  end;

end;




end.
