unit uPosDataProcess;

interface

uses
  System.Classes, uAlphaProtocol, windows , system.SysUtils
    ,ProtoGetU, ProtoSetU, winapi.messages, vcl.forms
    ,uQueueEx
  ;

type

  TProtoPos = class(TProtoGet)
  public
    function ParsingPacket(instr : string) : integer;

  end;

  // receive login packet and return to mt4
  TPosDataThrd = class(TThread)
  public
    constructor Create();
    destructor Destroy();
    function ThreadId():cardinal;
    procedure AddData(pNewItem : PTQItem);
    function GetData():PTQItem;
    procedure Lock();
    procedure UnLock();
  protected
    procedure Execute();override;
  protected
    m_ThreadId  : cardinal;
    m_protoGet  : TProtoPos;

    m_DataList   : TList;
    m_csData     : TRTLCriticalSection;


  end;



implementation

uses
   uBasketCommon, fmMainU, uCtrls , CommonUtils , uPostThread
  ;



constructor TPosDataThrd.Create;
begin
  m_protoGet  := TProtoPos.Create;
  InitializeCriticalSection(m_csData);
  m_DataList  := TList.create;

  inherited;
end;



destructor TPosDataThrd.Destroy;
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


function TPosDataThrd.ThreadId():cardinal;
begin
  Result := m_ThreadId;
end;


procedure TPosDataThrd.Lock();
begin
  EnterCriticalSection(m_csData);
end;


procedure TPosDataThrd.Unlock();
begin
  LeaveCriticalSection(m_csData);
end;


// add one data into the data list so that one of the workers can read it.
procedure TPosDataThrd.AddData(pNewItem : PTQItem);
var
  pItem : PTQItem;
begin

  New(pItem);
  CopyMemory(pItem, pNewItem, sizeof(TQItem));

  Dispose(pNewItem);

  Lock();

  try

    m_DataList.add(pItem);

  finally

    Unlock();

  end;


end;


function TPosDataThrd.GetData():PTQItem;
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


procedure TPosDataThrd.Execute();
var
  M : Msg;

  data : string;
  ret  : integer;

  i         : integer;
  iSymbol   : integer;
  iSide     : integer;

  dOpen   : double;
  //dPlPip  : double;

  //TODO itemPos : TItemPos;
  dwRslt  : DWORD;
begin

  m_ThreadId := GetCurrentThreadId();

  //TODO
//  while (not terminated) do //and (m_bcontinue) do
//  begin
//
//    Sleep(10);
//
//    while PeekMessage(M, 0, 0, 0, PM_REMOVE) do
//    begin
//      if M.Message = WM_QUIT then Break;
//      if M.message <> WM_POSITION then
//        continue;
//
//      TranslateMessage(M);
//      DispatchMessage(M);
//
//      __GetThreadMsgString(M, data);
//
//      ret := m_protoGet.ParsingPacket(data);
//      if ret=0 then
//      begin
//        fmMain.AddMsg('This is not a POSITION packet');
//        continue;
//      end;
//
//      iSymbol := -1; iSide :=-1; dOpen:=0;
//
//      itemPos := TitemPos.create;
//
//      for i := 0 to ret-1 do
//      begin
//
//        if m_protoGet.KV[i].Key = FDN_SYMBOL_IDX  then
//          iSymbol := strtoint(m_protoGet.KV[i].Value);
//
//        if m_protoGet.KV[i].Key = FDN_SIDE_IDX  then
//          iSide := strtoint(m_protoGet.KV[i].Value);
//
//        if m_protoGet.KV[i].Key = FDS_MT4_TICKET  then
//          itemPos.sTicket := m_protoGet.KV[i].Value;
//
//        if m_protoGet.KV[i].Key = FDD_OPEN_PRC  then
//        begin
//          dOpen := strtofloat(m_protoGet.KV[i].Value);
//          itemPos.sOpenPrc := __FmtPrcD(__gdDecimalCnt(iSymbol, iSide),dOpen );
//        end;
//
//        if m_protoGet.KV[i].Key = FDD_LOTS  then
//          itemPos.sLots := m_protoGet.KV[i].Value;
//
//        if m_protoGet.KV[i].Key = FDD_PROFIT  then
//          itemPos.sPL := m_protoGet.KV[i].Value;
//
//        if m_protoGet.KV[i].Key = FDS_CLR_TP  then
//          itemPos.sClrTp := m_protoGet.KV[i].Value;
//      end;
//
//      if( (iSymbol > 0) and (iSide>0) )then
//      begin
//        itemPos.iSymbol  := iSymbol;
//        itemPos.iSide    := iSide;
//        itemPos.sOpenPrc := __FmtPrcD(__gdDecimalCnt(iSymbol, iSide),dOpen );
//
//  //      dPlPip := strtofloatdef(itemPos.sPL,0);
//  //      if dPlPip<>0 then
//  //        dPlPip := dPlPip / __gdPipSize(iSymbol, iSide);
//  //
//  //      itemPos.sPlPip := __FmtPip(dPlPip);
//
//        SendMessageTimeOut(Application.MainForm.Handle,
//                                WM_GRID_POSITION,
//                                wParam(LongInt(sizeof(itemPos))),
//                                Lparam(LongInt(itemPos)),
//                                SMTO_ABORTIFHUNG,
//                                TIMEOUT_SENDMSG * 2 * 3,
//                                dwRslt
//                                );
//      end;
//
//    end;
//
//  end;

end;


(*
SYMBOL      : FDS_SYMBOL
SYMBOL_IDX  : FDN_SYMBOL_IDX
SIDE_IDX    : FDN_SIDE_IDX
TICKET      : FDS_MT4_TICKET
ORG_TICKET  : FDS_MT4_TICKET_ORG
OPEN_PRC    : FDD_OPEN_PRC
LOTS        : FDD_LOTS
SIDE        : FDS_ORD_SIDE
PL          : FDD_PROFIT
*)

function TProtoPos.ParsingPacket(instr : string) : integer;
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

    except
      // something wrong, left part (key) is not an integer value
      Result := -1; Exit;
    end;
  end;

  Result := Length(KV);

end;



end.
