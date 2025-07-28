unit uMDProcess;

interface

uses
  System.Classes, uAlphaProtocol, windows, system.SysUtils
  ,ProtoGetU, ProtoSetU, winapi.messages, vcl.forms, vcl.dialogs
  ,uBasketCommon, uQueueEx
  ;

type

  TProtoMD = class(TProtoGet)
  public
    function ParsingPacket(instr : string) : integer;

  end;


  TMDThreadPool = class(TObject)
    private
      FThreadList  : TList;
      m_DataList   : TList;
      m_csData     : TRTLCriticalSection;
    public
      constructor Create;
      destructor Destroy; override;
      procedure CreateThreadPool(iCount:integer);
      procedure ResumeThreadPool();
      procedure AddData(pInItem : PTQItem);
      procedure Lock();
      procedure Unlock();
      function GetData():PTQItem;
  end;


  TWorkerThread = class(TThread)
  protected
    procedure Execute();override;
    function  OpenOrder(iUpdatedIdx:integer):boolean;
  protected
    m_ThreadId  : cardinal;
    m_protoGet  : TProtoMD;

  public
    constructor Create(bSuspend:Boolean);
    function ThreadId():cardinal;

  private

  end;


var
  __mdThreadPool :TMDThreadPool;

implementation

uses
  fmMainU, uCtrls, uPostThread, commonutils, uDatahandler
  ;


constructor TMDThreadPool.create;
begin
  InitializeCriticalSection(m_csData);
  FThreadList := TList.create;
  m_DataList  := TList.create;
  inherited;
end;

destructor TMDThreadPool.Destroy;
var
  i : integer;
begin
  for i := 0 to FThreadList.count-1 do
  begin
    TWorkerThread(FThreadList.items[i]).Terminate;
  end;

  DeleteCriticalSection(m_csData);

  inherited;
end;

procedure TMDThreadPool.CreateThreadPool(iCount:integer);
var
  i : integer;
begin

  for i := 0 to iCount-1 do
  begin
    FThreadList.Add(TWorkerThread.create(True));
  end;


end;


procedure TMDThreadPool.ResumeThreadPool();
var
  i : integer;
begin


  for i := 0 to FThreadList.Count-1 do
  begin
    TWorkerThread(FThreadList.items[i]).resume;
  end;

end;

procedure TMDThreadPool.Lock();
begin
  EnterCriticalSection(m_csData);
end;


procedure TMDThreadPool.Unlock();
begin
  LeaveCriticalSection(m_csData);
end;

function TMDThreadPool.GetData():PTQItem;
begin

  Result := Nil;

  Lock();

  Try

    if m_DataList.Count=0 then
      exit;

    Result := PTQItem(m_DataList[0]);
    m_DataList.Delete(0);


  Finally
    Unlock();
  End;

end;



// add one data into the data list so that one of the workers can read it.
procedure TMDThreadPool.AddData(pInItem : PTQItem);
var
  pItem : PTQItem;
begin

  New(pItem);
  //CopyMemory(pItem, pNewItem, sizeof(TQItem));
  pItem.sKey := pInItem.sKey;
  pItem.sCode := pInItem.sCode;
  pItem.data := pInItem.data;
  pItem.etc := pInItem.etc;

  Dispose(pInItem);

  Lock();

  try

    m_DataList.add(pItem);

  finally

    Unlock();

  end;


end;





constructor TWorkerThread.Create;
begin
  m_protoGet  := TProtoMD.Create;

  inherited;
end;

function TWorkerThread.ThreadId():cardinal;
begin
  Result := m_ThreadId;
end;


//BROKER_KEY
//BID     : FDS_BID
//ASK     : FDS_ASK
//SPREAD  : FDS_SPREAD
//procedure TWorkerThread.Execute();
//var
//
//  item : TQItem;
//  bExist : boolean;
//
//  sBrokerKey : string;
//  iSymbol   : integer;
//  sSymbol   : string;
//  sBid      : string;
//  sAsk      : string;
//  sSpread   : string;
//
//  ret  : integer;
//  i    : integer;
//
//begin
//
//  m_ThreadId := GetCurrentThreadId();
//
//  while (not terminated) do //and (m_bcontinue) do
//  begin
//    Sleep(10);
//
//    bExist := __mdThreadPool.GetData(item);
//    if not bExist then
//      continue;
//
//    ret := m_protoGet.ParsingPacket(item.data);
//    if ret=0 then
//    begin
//      fmMain.AddMsg (False,'This is not a MarketData packet');
//      continue;
//    end;
//
//    iSymbol := -1;
//
//    for i := 0 to ret-1 do
//    begin
//
//      if m_protoGet.KV[i].Key = FDS_KEY  then
//        sBrokerKey := m_protoGet.KV[i].Value;
//
//      if m_protoGet.KV[i].Key = FDN_SYMBOL_IDX  then
//        iSymbol := strtoint(m_protoGet.KV[i].Value);
//
//      if m_protoGet.KV[i].Key = FDS_SYMBOL  then
//        sSymbol := m_protoGet.KV[i].Value;
//
//      if m_protoGet.KV[i].Key = FDD_BID  then
//        sBid := m_protoGet.KV[i].Value;
//
//      if m_protoGet.KV[i].Key = FDD_ASK  then
//        sAsk := m_protoGet.KV[i].Value;
//
//      if m_protoGet.KV[i].Key = FDD_SPREAD  then
//        sSpread := m_protoGet.KV[i].Value;
//
//    end;
//
//    // All brokers haven't started sending market data yet.
//    if not __dataHandler.Is_ReadyTrade() then
//    begin
//      __dataHandler.Mark_BrokerStartMD(sBrokerKey);
//      continue;
//    end;
//
//
//    // Has opened positions for this symbol?
//    if __dataHandler.Is_AlreadOpened(iSymbol) then
//    begin
//      //TODO. 손익계산
//    end
//
//    // Best price
//    ELSE
//    BEGIN
//      __dataHandler.UpdateBestPrc(iSymbol, sSymbol, sBid, sAsk, sSpread, sBrokerKey, '');
//    END;
//
//  end;
//
//end;

 procedure TWorkerThread.Execute();
var

  pItem : PTQItem;
  bExist : boolean;

  sBrokerKey : string;
  iSymbol   : integer;
  sSymbol   : string;
  sBid      : string;
  sAsk      : string;
  sSpread   : string;

  ret         : integer;
  i           : integer;
  sMDData     : string;
  iUpdatedIdx : integer;
begin

  m_ThreadId := GetCurrentThreadId();

  while (not terminated) do //and (m_bcontinue) do
  begin

    Sleep(10);

    iUpdatedIdx := -1;

    pItem := __mdThreadPool.GetData();
    if pItem = NIL then
      continue;

    sMDData := pItem.data;
    Dispose(pItem);

    try
    ret := m_protoGet.ParsingPacket(sMDData);
    if ret=0 then
    begin
      fmMain.AddMsg(False,'This is not a MarketData packet');
      //Dispose(pItem);
      continue;
    end;
    except
      showmessage('try except-1');

    end;

    iSymbol := -1;

    for i := 0 to ret-1 do
    begin

      if m_protoGet.KV[i].Key = FDS_KEY  then
        sBrokerKey := m_protoGet.KV[i].Value;

      if m_protoGet.KV[i].Key = FDN_SYMBOL_IDX  then
        iSymbol := strtoint(m_protoGet.KV[i].Value);

      if m_protoGet.KV[i].Key = FDS_SYMBOL  then
        sSymbol := m_protoGet.KV[i].Value;

      if m_protoGet.KV[i].Key = FDD_BID  then
        sBid := m_protoGet.KV[i].Value;

      if m_protoGet.KV[i].Key = FDD_ASK  then
        sAsk := m_protoGet.KV[i].Value;

      if m_protoGet.KV[i].Key = FDD_SPREAD  then
        sSpread := m_protoGet.KV[i].Value;

    end;

    // All brokers haven't started sending market data yet.
    if not __dataHandler.Is_ReadyTrade() then
    begin
      __dataHandler.Mark_BrokerStartMD(sBrokerKey);
      continue;
    end;

    // Has opened positions for this symbol?
    if __dataHandler.Is_AlreadOpened(iSymbol) then
    begin
      //TODO. 손익계산
    end


    // Best price
    ELSE
    BEGIN

      iUpdatedIdx := __dataHandler.UpdateBestPrc(iSymbol, sSymbol, sBid, sAsk, sSpread, sBrokerKey, '');
//      if iUpdatedIdx>-1 then
//      begin
//        // Check whether open order or not
//      end;


    END;


  end; // while

end;

function TWorkerThread.OpenOrder(iUpdatedIdx:integer):boolean;
var
  nComm : integer;

begin

  Result := false;

  nComm := __dataHandler.m_arrData[iUpdatedIdx].Spec.nCommPt * 2;

  //if __dataHandler.m_arrData[iUpdatedIdx].Bestprc.gapBidAsk < nComm then
  //  exit;


end;

function TProtoMD.ParsingPacket(instr : string) : integer;
var
  intres, i1, pos1 : integer;
  StrArray  : TStringArray;
  s1        : string;

  sKey : string;
  sVal : string;
begin

  intres := SplitPacket(instr, StrArray);

  if intres < 0 then
  begin
    Result := -1;
    showmessage('TProtoMD.ParsingPacket-1:'+inttostr(intres));
    Exit;
  end;

  SetLength(KV, Length(StrArray));
  for i1 := 0 to Length(StrArray) - 1 do
  begin
    s1 := StrArray[i1];
    pos1 := Pos('=', s1);
    if pos1 <= 0 then
    begin
      // something wrong, no '=' sign in the field
      Result := -1;
      Exit;
    end;
    try
      sKey := Copy(s1, 1, pos1 - 1);
      Delete(s1, 1, pos1);
      sVal := s1;

      KV[i1].Key := sKey;
      KV[i1].Value := sVal;

    except
      // something wrong, left part (key) is not an integer value
      Result := -1;
      showmessage('TProtoMD.ParsingPacket-2');
      Exit;
    end;
  end;

  Result := Length(KV);

end;



end.
