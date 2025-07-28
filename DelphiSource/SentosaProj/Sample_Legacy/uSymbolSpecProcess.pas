unit uSymbolSpecProcess;

interface

uses
  System.Classes, uAlphaProtocol, windows , system.SysUtils
    ,ProtoGetU, ProtoSetU, winapi.messages, vcl.forms

  ;

type


  TProtoSpec = class(TProtoGet)
  public
    function ParsingPacket(instr : string) : integer;

  end;

  // receive login packet and return to mt4
  TSymbolSpecThrd = class(TThread)
  public
    constructor Create();
    procedure Execute();override;
  protected
    m_ThreadId  : cardinal;
    m_protoGet  : TProtoSpec;

  public
    function ThreadId():cardinal;

  end;



implementation

uses
  uQueueEx, uTwoWayCommon, fmMainU, uCtrls, uPostThread
  ;



constructor TSymbolSpecThrd.Create;
begin
  m_protoGet  := TProtoSpec.Create;

  inherited;
end;

function TSymbolSpecThrd.ThreadId():cardinal;
begin
  Result := m_ThreadId;
end;

procedure TSymbolSpecThrd.Execute();
var
  M : Msg;

  data : string;
  ret  : integer;
  i    : integer;
  iSymbol   : integer;
  iSide     : integer;
  nDecimal  : integer;
  dPipSize  : double;

  itemSpec  : TItemSpec;
  dwRslt    : DWORD;
begin

  m_ThreadId := GetCurrentThreadId();

  while (not terminated) do //and (m_bcontinue) do
  begin

    Sleep(10);

    while PeekMessage(M, 0, 0, 0, PM_REMOVE) do
    begin
      if M.Message = WM_QUIT then Break;
      if M.message <> WM_SYMBOL then
        continue;


      TranslateMessage(M);
      DispatchMessage(M);

      __GetThreadMsgString(M, data);

      ret := m_protoGet.ParsingPacket(data);
      if ret=0 then
      begin
        fmMain.AddMsg('This is not a symbol spec packet');
        continue;
      end;

      iSymbol   := -1; iSide := -1; nDecimal := 0; dPipSize := 0;
      for i := 0 to ret-1 do
      begin

        if m_protoGet.KV[i].Key = FDN_SYMBOL_IDX  then
          iSymbol := strtoint(m_protoGet.KV[i].Value);

        if m_protoGet.KV[i].Key = FDN_SIDE_IDX  then
          iSide := strtoint(m_protoGet.KV[i].Value);

        if m_protoGet.KV[i].Key = FDN_DECIMAL  then
          nDecimal := strtoint(m_protoGet.KV[i].Value);

        if m_protoGet.KV[i].Key = FDD_PIP_SIZE  then
          dPipSize := strtofloat(m_protoGet.KV[i].Value);

      end;

      if (iSymbol < 1) OR (iSide<IDX_BUY) then
      BEGIN
        fmMain.AddMsg(format('SPEC IDX ÀÌ»ף(iSymbol:%d)(iSide:%d)',[iSymbol,iSide]), false, true);
        continue;
      END;

      itemSpec := titemSpec.Create;
      itemSpec.iSymbol  := iSymbol;
      itemSpec.iSide    := iSide;
      itemSpec.sDecimal := inttostr(nDecimal);
      itemSpec.sPipSize := formatfloat('#.#####', dPipSize);

      SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_SPEC,
                              wParam(LongInt(sizeof(itemSpec))),
                              Lparam(LongInt(itemSpec)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );
    end;

  end;

end;


(*
SYMBOL  : FDS_SYMBOL
DECIMAL : FDN_DECIMAL
PIPSIZE : FDD_PIP_SIZE

*)

function TProtoSpec.ParsingPacket(instr : string) : integer;
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
