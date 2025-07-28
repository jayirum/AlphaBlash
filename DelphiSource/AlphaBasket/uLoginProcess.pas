unit uLoginProcess;

interface

uses
  System.Classes, uAlphaProtocol, windows, system.SysUtils, winapi.messages
  ,ProtoGetU, ProtoSetU, uDataHandler, uQueueEx
  ;

type

  TProtoLogin = class(TProtoGet)
  public
    function ParsingPacket(instr : string) : integer;

  end;

  // receive login packet and return to mt4
  TLoginThrd = class(TThread)
  protected
    procedure Execute();override;
  protected
    m_ThreadId  : cardinal;
    m_protoGet  : TProtoLogin;
    m_protoSet  : TProtoSet;

    m_DataList   : TList;
    m_csData     : TRTLCriticalSection;

  public
    constructor Create();
    destructor Destroy();
    function ThreadId():cardinal;
    procedure AddData(pInItem : PTQItem);
    function GetData():PTQItem;
    procedure Lock();
    procedure UnLock();


  end;



implementation

uses
  uBasketCommon, fmMainU, uCtrls, uPostThread
  ;



constructor TLoginThrd.Create;
begin
  InitializeCriticalSection(m_csData);
  m_DataList  := TList.create;

  m_protoGet  := TProtoLogin.Create;
  m_protoSet  := TProtoSet.create;
  inherited;
end;


destructor TLoginThrd.Destroy;
var
  i : integer;
begin
  for i := 0 to m_DataList.count-1 do
  begin
    Dispose(m_DataList[i]);
  end;

  m_DataList.Destroy();
  DeleteCriticalSection(m_csData);

  inherited;
end;

function TLoginThrd.ThreadId():cardinal;
begin
  Result := m_ThreadId;
end;



procedure TLoginThrd.Lock();
begin
  EnterCriticalSection(m_csData);
end;


procedure TLoginThrd.Unlock();
begin
  LeaveCriticalSection(m_csData);
end;

function TLoginThrd.GetData():PTQItem;
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
procedure TLoginThrd.AddData(pInItem : PTQItem);
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

procedure TLoginThrd.Execute();
var
  M : Msg;

  data : string;
  ret  : integer;
  i1,i2: integer;
  outstr : string;
  sBrokerKey   : string;
  sBrokerName  : string;

  sMsg : string;
  //bMatched : boolean;
  pItem : PTQItem;
begin

  m_ThreadId := GetCurrentThreadId();

  while (not terminated) do //and (m_bcontinue) do
  begin
    Sleep(10);
    pItem := GetData();
    if pItem = NIL then
      continue;

    ret := m_protoGet.ParsingPacket(pItem.data);
    if ret=0 then
    begin
      fmMain.AddMsg(False, 'This is not a Login packet');
      Dispose(pItem);
      continue;
    end;


    sBrokerKey  := '';
    sBrokerName := '';

    for i1 := 0 to ret-1 do
    begin

      if m_protoGet.KV[i1].Key = FDS_KEY then
        sBrokerKey := m_protoGet.KV[i1].Value;

      if m_protoGet.KV[i1].Key = FDS_BROKER then
        sBrokerName := m_protoGet.KV[i1].Value;
    end;

    if (sBrokerKey='') or (sBrokerName='') then
    begin
      fmMain.AddMsg(False, 'Wrong Login Packet');
      Dispose(pItem);
      continue;
    end;


    sMsg := Format('[LOGIN] (BrokerKey:%s) (Broker:%s)', [sBrokerKey, sBrokerName] );
    fmMain.AddMsg(True, sMsg);
    __dataHandler.Add_BrokerWhenLogin(sBrokerKey, sBrokerName);

    // compose return packet
    m_protoSet.Start;
    m_protoSet.SetVal(FDS_CODE, CODE_LOGON );

    m_protoSet.SetVal(FDN_ARRAY_SIZE, __dataHandler.Get_SymbolCnt());

      outstr := '';
    for i2 := 0 to __dataHandler.Get_SymbolCnt()-1 do
    begin
      outstr := outstr + FDS_SYMBOL+'='+__dataHandler.m_arrData[i2].symbol + DEF_DELI_COLUMN;
      outstr := outstr + FDN_SYMBOL_IDX+'='+inttostr(i2) + DEF_DELI_ARRAY;
    end;

    m_protoSet.SetVal(FDS_ARRAY_DATA, outstr);
    m_protoSet.SetVal(FDN_ERR_CODE, E_OK);

    m_protoSet.Complete(outstr);

    __QueueEx[Q_SEND].Add(sBrokerKey, CODE_LOGON, outstr);

    Dispose(pItem);

  end;

end;


function TProtoLogin.ParsingPacket(instr : string) : integer;
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
    Result := -1; Exit;
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
      Result := Length(KV);

    except
      // something wrong, left part (key) is not an integer value
      Result := -1; Exit;
    end;
  end;

  Result := Length(KV);

end;


end.

