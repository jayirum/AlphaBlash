unit uMDProcess;

interface

uses
  System.Classes, uAlphaProtocol, windows, system.SysUtils
  ,ProtoGetU, ProtoSetU, winapi.messages, vcl.forms
  ;

type

  TProtoMD = class(TProtoGet)
  public
    function ParsingPacket(instr : string) : integer;

  end;


  // receive login packet and return to mt4
  TMarketDataThrd = class(TThread)
  protected
    procedure Execute();override;
  protected
    m_ThreadId  : cardinal;
    m_protoGet  : TProtoMD;

  public
    constructor Create();
    function ThreadId():cardinal;

  private

  end;



implementation

uses
  uQueueEx, uTwoWayCommon, fmMainU, uCtrls, uPostThread, uRealPLOrder, commonutils
  ;



constructor TMarketDataThrd.Create;
begin
  m_protoGet  := TProtoMD.Create;

  inherited;
end;

function TMarketDataThrd.ThreadId():cardinal;
begin
  Result := m_ThreadId;
end;



//BID     : FDS_BID
//ASK     : FDS_ASK
//SPREAD  : FDS_SPREAD
procedure TMarketDataThrd.Execute();
var
  M : Msg;

  data : string;
  ret  : integer;
  i    : integer;
  iSymbol   : integer;
  iSide     : integer;
  sBid,
  sAsk,
  sSpread   : string;
  dPLPip    : double;
  sNowPrc   : string;
  dNewSpread   : double;
  dLeastSpread : double;
  dOpenPrc     : double;

  itemMD    : TItemMD;
  dwRslt    : DWORD;
begin

  m_ThreadId := GetCurrentThreadId();

  while (not terminated) do //and (m_bcontinue) do
  begin
    Sleep(10);

    while PeekMessage(M, 0, 0, 0, PM_REMOVE) do
    begin
      if M.Message = WM_QUIT then Break;
      if M.Message <> WM_MD then
        continue;

      TranslateMessage(M);
      DispatchMessage(M);

      __GetThreadMsgString(M, data);

      ret := m_protoGet.ParsingPacket(data);
      if ret=0 then
      begin
        fmMain.AddMsg('This is not a MarketData packet');
        continue;
      end;

      iSymbol := -1;
      iSide   := -1;

      for i := 0 to ret-1 do
      begin

        if m_protoGet.KV[i].Key = FDN_SYMBOL_IDX  then
          iSymbol := strtoint(m_protoGet.KV[i].Value);

        if m_protoGet.KV[i].Key = FDN_SIDE_IDX  then
          iSide := strtoint(m_protoGet.KV[i].Value);

        if m_protoGet.KV[i].Key = FDD_BID  then
          sBid := m_protoGet.KV[i].Value;

        if m_protoGet.KV[i].Key = FDD_ASK  then
          sAsk := m_protoGet.KV[i].Value;

        if m_protoGet.KV[i].Key = FDD_SPREAD  then
          sSpread := m_protoGet.KV[i].Value;

      end;

    
      // 매수포지션 ==> 매도청산 => BID 참조
      if iSide = IDX_BUY then
        sNowPrc := sBid
      else if iSide = IDX_SELL then
        sNowPrc := sAsk
      ;

      if iSide = 0 then
      begin
        fmMain.AddMsg(format('MD IDX 이상(iSymbol:%d)(iSide:%d)',[iSymbol,iSide]), false, true);
        continue;
      end;

      // 일중 최소 spread 기록
      dNewSpread := strtofloatdef(sSpread,0);
      if dNewSpread>0 then
      begin
        if iSide = IDX_BUY then
        BEGIN
          dLeastSpread := strtofloatdef(__ctrls[iSymbol].spreadMinB.Text,0);
          if (dLeastSpread = 0)  then
            __ctrls[iSymbol].spreadMinB.Text := floattostr(dNewSpread)
          else if dLeastSpread > dNewSpread  then
            __ctrls[iSymbol].spreadMinB.Text := floattostr(dNewSpread)
          ;

          dLeastSpread := strtofloatdef(__ctrls[iSymbol].spreadMaxB.Text,0);
          if (dLeastSpread = 0)  then
            __ctrls[iSymbol].spreadMaxB.Text := floattostr(dNewSpread)
          else if dLeastSpread < dNewSpread  then
            __ctrls[iSymbol].spreadMaxB.Text := floattostr(dNewSpread)
          ;
        END;

        if iSide = IDX_SELL then
        BEGIN
          dLeastSpread := strtofloatdef(__ctrls[iSymbol].spreadMinS.Text,0);
          if (dLeastSpread = 0)  then
            __ctrls[iSymbol].spreadMinS.Text := floattostr(dNewSpread)
          else if dLeastSpread > dNewSpread  then
            __ctrls[iSymbol].spreadMinS.Text := floattostr(dNewSpread)
          ;

          dLeastSpread := strtofloatdef(__ctrls[iSymbol].spreadMaxS.Text,0);
          if (dLeastSpread = 0)  then
            __ctrls[iSymbol].spreadMaxS.Text := floattostr(dNewSpread)
          else if dLeastSpread < dNewSpread  then
            __ctrls[iSymbol].spreadMaxS.Text := floattostr(dNewSpread)
          ;
        END;


      end;



      // 실시간 손익PIP 계산
      if __gdExistPosition(iSymbol, iSide) then
      begin
        dOpenPrc := __gdOpenPrc(iSymbol,iSide);
        if(dOpenPrc<=0) then
          continue;

        if iSide=IDX_SELL then
          dPLPip := dOpenPrc - __gdNowPrc(iSymbol,iSide)
        else
          dPLPip := __gdNowPrc(iSymbol,iSide) - dOpenPrc
        ;
        dPLPip := dPLPip / __gdPipSize(iSymbol,iSide);
      end
      else
        dPLPip := 0;



      itemMD := TItemMD.Create;
      itemMD.iSymbol  := iSymbol;
      itemMD.iSide    := iSide;
      itemMD.sClose   := sNowPrc;
      itemMD.sSpread  := __FmtPip( strtofloat(sSpread));
      itemMD.sPlPip   := __FmtPip(dPLPIp);

      SendMessageTimeOut(Application.MainForm.Handle,
                            WM_GRID_REAL_MD,
                            wParam(LongInt(sizeof(itemMD))),
                            Lparam(LongInt(itemMD)),
                            SMTO_ABORTIFHUNG,
                            TIMEOUT_SENDMSG,
                            dwRslt
                            );

      // RealPL thread 들에게 전파
      __DeployMD(iSymbol);
    end;

  end;

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
