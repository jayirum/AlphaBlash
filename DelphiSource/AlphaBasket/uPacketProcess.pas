unit uPacketProcess;

interface

uses
  System.Classes, System.SysUtils, System.Variants, Windows
  , uAlphaProtocol, uLoginProcess, uMDProcess, uPosDataProcess
  , uBasketCommon
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
     //m_md     : TMDThreadPool;
     m_pos    : TPosDataThrd;

     m_bContinue : boolean;
  end;


var
  __process : CPacketProcess;

implementation

uses
  uQueueEx, fmMainU, ProtoGetU, uPostThread
  ;


constructor CPacketProcess.Create;
begin
  m_bContinue := true;

  m_login  := TLoginThrd.Create();
  m_pos    := TPosDataThrd.Create();

  __mdThreadPool := TMDThreadPool.Create();
  __mdThreadPool.CreateThreadPool(1); //TODO (MAX_THREAD);
  Sleep(1000);
  __mdThreadPool.ResumeThreadPool();


  inherited;
end;

destructor  CPacketProcess.Destroy;
begin
  // postthreadmessage to workers
  m_bContinue := false;
  m_login.Terminate;
  m_pos.Terminate;
  FreeAndNil(__mdThreadPool);

end;


procedure CPacketProcess.Execute();
var
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
      sCode := pItem.sCode;
      data  := pItem.data;

      // 패킷 코드에 따라 각 스레드에게 POST

      if sCode = CODE_LOGON  then
        m_login.AddData(pItem)

      else if sCode = CODE_MARKET_DATA then
        __mdThreadPool.AddData(pItem)
        
      else if sCode = CODE_POSITION then
        m_pos.AddData (pItem)
      ;


      // Dispose(pItem);
      // 여기서 Dispose 하면 안되고, 각 수신처에서 해야 한다.

    end;


  end;

end;




end.
