unit uLoginProcess;

interface

uses
  System.Classes, uAlphaProtocol, windows, system.SysUtils
  ,ProtoGetU, ProtoSetU, winapi.messages
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

  public
    constructor Create();
    function ThreadId():cardinal;

  end;



implementation

uses
  uQueueEx, uTwoWayCommon, fmMainU, uCtrls, uPostThread
  ;



constructor TLoginThrd.Create;
begin
  m_protoGet  := TProtoLogin.Create;
  m_protoSet  := TProtoSet.create;
  inherited;
end;


function TLoginThrd.ThreadId():cardinal;
begin
  Result := m_ThreadId;
end;



procedure TLoginThrd.Execute();
var
  M : Msg;

  data : string;
  ret  : integer;
  i1,i2: integer;
  outstr : string;
  sKey   : string;

  bMatched : boolean;
begin

  m_ThreadId := GetCurrentThreadId();

  while (not terminated) do //and (m_bcontinue) do
  begin
    Sleep(10);

    while PeekMessage(M, 0, 0, 0, PM_REMOVE) do
    begin
      if M.Message = WM_QUIT then Break;
      if M.message <> WM_LOGON then
        continue;

      TranslateMessage(M);
      DispatchMessage(M);

      data := '';
      __GetThreadMsgString(M, data);

      ret := m_protoGet.ParsingPacket(data);
      if ret=0 then
      begin
        fmMain.AddMsg('This is not a Login packet');
        continue;
      end;

      m_protoSet.Start;
      for i1 := 0 to ret-1 do
      begin

        if m_protoGet.KV[i1].Key = FDS_KEY then
        BEGIN

          sKey := m_protoGet.KV[i1].Value;

          m_protoSet.SetVal(FDS_CODE, CODE_LOGON );

          bMatched := False;

          for i2 := 1 to __CtrCnt() do
          BEGIN
            if sKey = __ctrls[i2].key_buy.text then
            begin
              m_protoSet.SetVal(FDN_SYMBOL_IDX, i2);
              m_protoSet.SetVal(FDS_SYMBOL,     __Symbol(i2));
              m_protoSet.SetVal(FDN_SIDE_IDX,   IDX_BUY);

              bMatched := True;
            end

            else if sKey = __ctrls[i2].key_sell.text then
            begin
              m_protoSet.SetVal(FDN_SYMBOL_IDX, i2);
              m_protoSet.SetVal(FDN_SIDE_IDX,   IDX_SELL);
              m_protoSet.SetVal(FDS_SYMBOL,     __Symbol(i2));
              bMatched := True;
            end;
          END;

          if bMatched = True then
            m_protoSet.SetVal(FDN_ERR_CODE, E_OK)
          else
            m_protoSet.SetVal(FDN_ERR_CODE, E_NO_KEY)
          ;

          m_protoSet.Complete(outstr);

          __QueueEx[Q_SEND].Add(CODE_LOGON, sKey, outstr);

        END;

      end;

    end;






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

