(*

*)
unit uDataHandler;

interface

Uses
  System.sysutils, windows, Messages, classes
  , uBasketCommon
  ;


type


  CDataHandler = class(TObject)
  private
    m_nSymbolCnt  : integer;
    m_nBrokerCnt  : integer;
    m_bTradeStart : boolean;
  public
    //m_arrSymbols : array[0..MAX_SYMBOL-1] of TSymbolSpec;  // 최초 로드하는 심볼 저장. 저장된 index 를 EA들과 공유

    procedure Add_Symbol(iSymbol     :integer;
                          symbol      :string;
                          dPipSize    :double;
                          nDecimalCnt :integer;
                          nActiveSpreadPt:integer;
                          nTSPtOpen   :integer;
                          nTSPtClose  :integer;
                          nCommPt     :integer;
                          nTargetPt   :integer
                          );
    function Get_SymbolCnt():integer;

  public
    m_arrData : array[0..MAX_SYMBOL-1] of TData;
    m_arrCs   : array[0..MAX_SYMBOL-1] of TRTLCriticalSection;
    m_lstBroker : TList;

    procedure Set_RealBrokerCnt(cnt:integer);
    procedure Add_BrokerWhenLogin(sBrokerKey, sBrokerName:string);
    procedure Mark_BrokerStartMD(sBrokerKey:string);
    function Is_ReadyTrade():boolean;

    //
    function Is_UnderActiveSpread(iSymbol:integer; sSymbol, sSpread:string):boolean;
    function GetNextSymbol(iPrevSymbol:integer):string;

  public
    constructor Create;
    destructor Destroy;

    function  Is_AlreadOpened(iSymbol:integer) : boolean;
    function UpdateBestPrc(iSymbol:integer;
                          sSymbol, sNewBid, sNewAsk, sSpreadPt, sNewBrokerKey, sNewBrokerName:string):integer;

    procedure PrintNewBestPrc(bBid, bAsk:boolean; sNewBid, sNewAsk:string; iSymbol:integer);
    function Is_AlreadySet_BestPrc(iSymbol:integer):boolean;
    function CheckFireOpen(iSymbol:integer; sNewBid, sNewAsk, sNewBrokerKey:string):boolean;
  end;


var
  __dataHandler : CDataHandler;

implementation

uses
  fmMainU, CommonUtils;

constructor CDataHandler.Create;
var
  i : integer;
begin

  m_nSymbolCnt  := 0;
  m_nBrokerCnt  := 0;
  m_bTradeStart := false;

  for i := 0 to MAX_SYMBOL-1 do
  begin

    m_arrData[i].bOpen := false;
    InitializeCriticalSection(m_arrCs[i]);


  end;

  m_lstBroker := TList.Create;

  inherited;
end;


destructor CDataHandler.Destroy;
var
  i : integer;
begin
  for i := 0 to MAX_SYMBOL-1 do
  begin

    m_arrData[i].bOpen := false;
    DeleteCriticalSection(m_arrCs[i]);
  end;

  FreeAndNil(m_lstBroker);

  inherited;
end;




procedure CDataHandler.Set_RealBrokerCnt(cnt:integer);
begin
  m_nBrokerCnt := cnt;
end;


// Trade can be started after all brokers start sending market data
function CDataHandler.Is_ReadyTrade():boolean;
begin
  Result := false;
  if m_lstBroker.Count < m_nBrokerCnt then
    exit;

  Result := m_bTradeStart;
end;


procedure CDataHandler.Add_BrokerWhenLogin(sBrokerKey, sBrokerName:string);
var
  i       : integer;
  pBroker : PTBroker;
begin

  // If this broker is already in the list, just exit
  for i := 0 to m_lstBroker.Count-1 do
  begin


    if TBroker(m_lstBroker[i]^).BrokerKey = sBrokerKey then
    begin
      exit;
    end;
  end;

  New(pBroker);
  pBroker.BrokerKey   := sBrokerKey;
  pBroker.BrokerName  := sBrokerName;
  pBroker.bStartMD    := False;
  m_lstBroker.Add(pBroker);

end;

// When brokers start sending MD, mark it.
procedure CDataHandler.Mark_BrokerStartMD(sBrokerKey:string);
var
  i       : integer;
  pBroker : PTBroker;
  bFound  : boolean;
  nMarkedCnt : integer;
begin

  if m_lstBroker.Count < m_nBrokerCnt then
    exit;


  bFound  := False;

  // If this broker is already in the list, just exit
  nMarkedCnt := 0;
  for i := 0 to m_lstBroker.Count-1 do
  begin

    if TBroker(m_lstBroker[i]^).BrokerKey = sBrokerKey then
    begin

      TBroker(m_lstBroker[i]^).bStartMD := True;
      bFound  := True;
      Inc(nMarkedCnt);
    end
    else
    begin
      if TBroker(m_lstBroker[i]^).bStartMD = True then
        Inc(nMarkedCnt);
    end;
  end;

  if bFound then
    fmMain.AddMsg(True, Format('[MARK_STARTMD] (BrokerKey:%s)', [sBrokerKey]) , true)
  else
  begin
    fmMain.AddMsg(False, Format('[MARK_STARTMD] Failed to find Broker (BrokerKey:%s)', [sBrokerKey]) );
    exit;
  end;

  if nMarkedCnt = m_lstBroker.Count then
  begin
    m_bTradeStart := true;
    fmMain.AddMsg(True, 'All Brokers have sent 1st Market Data. Start Trading.');
  end;



end;


function CDataHandler.Get_SymbolCnt():integer;
begin
  Result := m_nSymbolCnt;
end;

procedure CDataHandler.Add_Symbol(iSymbol     :integer;
                                  symbol      :string;
                                  dPipSize    :double;
                                  nDecimalCnt :integer;
                                  nActiveSpreadPt:integer;
                                  nTSPtOpen   :integer;
                                  nTSPtClose  :integer;
                                  nCommPt     :integer;
                                  nTargetPt   :integer
                                  );
begin
  m_arrData[iSymbol].symbol := symbol;
  m_arrData[iSymbol].bOpen  := false;

  m_arrData[iSymbol].Spec.nDecimalCnt  := nDecimalCnt;
  m_arrData[iSymbol].Spec.dPipSize     := dPipSize;
  m_arrData[iSymbol].Spec.nActiveSpreadPt := nActiveSpreadPt;
  m_arrData[iSymbol].Spec.nTSPtOpen    := nTSPtOpen;
  m_arrData[iSymbol].Spec.nTSPtClose   := nTSPtClose;
  m_arrData[iSymbol].Spec.nCommPt      := nCommPt;
  m_arrData[iSymbol].Spec.nTargetPt    := nTargetPt;

  Inc(m_nSymbolCnt);
end;



// 첫 시작은 -1 을 넣어야 한다.
function CDataHandler.GetNextSymbol(iPrevSymbol:integer):string;
begin
  Result := '';

  if iPrevSymbol=m_nSymbolCnt-1 then
    exit;

  Result := m_arrData[iPrevSymbol].symbol;

end;


// Has opened positions for this symbol?
function  CDataHandler.Is_AlreadOpened(iSymbol:integer) : boolean;
begin
  Result := m_arrData[iSymbol].bOpen;
end;



// if fire order, return true;
function CDataHandler.CheckFireOpen(iSymbol:integer; sNewBid, sNewAsk, sNewBrokerKey:string):boolean;
var
  dGap : double;
  nNewGap : integer;
begin

  Result := False;

  // before TS starts
  // before touch open condition (greater than cost)
  if m_arrData[iSymbol].Bestprc.nGapMax_Open = 0 then
    exit;


  dGap := 0;
  nNewGap := 0;

  // new bid or new ask goes loss direction with same brokers
  if m_arrData[iSymbol].bestPrc.bidBrokerKey = sNewBrokerKey then
  begin
    // bid goes badly
    if CompareStr(m_arrData[iSymbol].bestPrc.bidPrc, sNewBid) > 0 then
      dGap := strtofloat(sNewBid) - strtofloat(m_arrData[iSymbol].bestPrc.askPrc);
  end;

  if m_arrData[iSymbol].bestPrc.askBrokerKey = sNewBrokerKey then
  begin
    // bid goes badly
    if CompareStr(m_arrData[iSymbol].bestPrc.askPrc, sNewAsk) < 0 then
      dGap := strtofloat(m_arrData[iSymbol].bestPrc.bidPrc) - strtofloat(sNewAsk);
  END;


  if dGap <> 0 then
    nNewGap := strtoint(formatfloat('#0', dGap / m_arrData[iSymbol].Spec.dPipSize) );

  //TODO. 새로운 GAP 이 MAX 에서 TS 만큼 떨어졌으면 FIRE!!!
  if (m_arrData[iSymbol].Bestprc.nGapMax_Open - nNewGap) >= m_arrData[iSymbol].Spec.nTSPtOpen then
  begin
    fmMain.AddMsg(true, format('[Fire Open-%s](Bid-%s-%s)(Ask-%s-%s)(Gap:%d)',
                              [
                              m_arrData[iSymbol].symbol
                              ,m_arrData[iSymbol].Bestprc.bidPrc
                              ,m_arrData[iSymbol].Bestprc.bidBrokerKey
                              ,m_arrData[iSymbol].Bestprc.askPrc
                              ,m_arrData[iSymbol].Bestprc.askBrokerKey
                              ,m_arrData[iSymbol].Bestprc.nGapBidAsk
                              ]));
    m_arrData[iSymbol].Bestprc.nGapMax_Open := 0;

    Result := true;
    exit;
  end;
end;

function CDataHandler.UpdateBestPrc(iSymbol:integer;
                sSymbol, sNewBid, sNewAsk, sSpreadPt, sNewBrokerKey, sNewBrokerName:string):integer;
var
  bBid, bAsk : boolean;
  nComp   : integer;
  nNewGap    : integer;
  dGap    : double;
begin

  Result := -1;

  bBid := false;
  bAsk := false;

  EnterCriticalSection(m_arrCs[iSymbol]);

  try

  // Don't update after positions have already opened.
  if Is_AlreadOpened(iSymbol) then
    exit;

  // The spread of the symbols must be in the range
  if not Is_UnderActiveSpread(iSymbol, sSymbol, sSpreadPt) then
  begin
    exit;
  end;

  // Check Fire Open Order
  if CheckFireOpen(iSymbol, sNewBid, sNewAsk, sNewBrokerKey) then
    exit; //TODO. EXIT???


  ////////////////////////////////////////////////////////////////
  ///  check BID
  if m_arrData[iSymbol].bestPrc.bidPrc = '' then
    bBid := True
  else
  begin
    // get highest bid
    if CompareStr(m_arrData[iSymbol].bestPrc.bidPrc, sNewBid) < 0 then
      bBid := True;
  end;


  ////////////////////////////////////////////////////////////////
  ///  check ASK
  if m_arrData[iSymbol].bestPrc.askPrc = '' then
    bAsk := True
  else
  begin
    // get lowest ask
    if CompareStr(m_arrData[iSymbol].bestPrc.askPrc, sNewAsk) > 0 then
      bAsk := True;
  end;

  // One broker can not have best prices of bid and ask
  if bBid and bAsk then
  begin
    //todo. logging
    exit;
  end;


  if bBid then
  begin
    m_arrData[iSymbol].bestPrc.bidPrc         := sNewBid;
    m_arrData[iSymbol].bestPrc.bidBrokerKey   := sNewBrokerKey;
  end;

  if bAsk then
  begin
    m_arrData[iSymbol].bestPrc.AskPrc         := sNewAsk;
    m_arrData[iSymbol].bestPrc.AskBrokerKey   := sNewBrokerKey;
  end;



  // put the gap (bid-ask)
  if bBid or bAsk then
  begin

    // return the symbol idx which is updated
    Result := iSymbol;

    if Is_AlreadySet_BestPrc(iSymbol) then
    begin
      dGap := strtofloat(m_arrData[iSymbol].bestPrc.bidPrc) - strtofloat(m_arrData[iSymbol].bestPrc.askPrc);

      nNewGap := 0;
      if dGap <> 0 then
        nNewGap := strtoint(formatfloat('#0', dGap / m_arrData[iSymbol].Spec.dPipSize) );

      m_arrData[iSymbol].Bestprc.nGapBidAsk := nNewGap;

      PrintNewBestPrc(bBid, bAsk, sNewBid, sNewAsk, iSymbol);

      if nNewGap < 0 then
      begin
        exit;
      end;

      //
      // check if the gap is now being Trailing Stop or needs to be TS
      //

      // Set the 1st value on GapMax_Open
      if m_arrData[iSymbol].Bestprc.nGapMax_Open = 0 then
      begin

        // gap goes over the cost ==> set GapMax
        if nNewGap >= m_arrData[iSymbol].Spec.nCommPt*2 then
          m_arrData[iSymbol].Bestprc.nGapMax_Open := nNewGap;

      end
      else if m_arrData[iSymbol].Bestprc.nGapMax_Open > 0 then // Aleady TS has started.
      begin
        // if new gap is bigger than the current GapMax, update the increasing maxgap
        if m_arrData[iSymbol].Bestprc.nGapMax_Open <= nNewGap then
          m_arrData[iSymbol].Bestprc.nGapMax_Open := nNewGap
      end;
//
//        // if new gap is smaller than the current GapMax, check TS
//        else
//        begin
//          // if the new gap falled TSPt from GapMax ==> Open Order
//          if (m_arrData[iSymbol].Bestprc.nGapMax_Open - nNewGap) >= m_arrData[iSymbol].Spec.nTSPtOpen then
//          begin
//            fmMain.AddMsg(true, format('[Fire Open-%s](Bid-%s-%s)(Ask-%s-%s)(Gap:%d)',
//                                      [
//                                      m_arrData[iSymbol].symbol
//                                      ,m_arrData[iSymbol].Bestprc.bidPrc
//                                      ,m_arrData[iSymbol].Bestprc.bidBrokerKey
//                                      ,m_arrData[iSymbol].Bestprc.askPrc
//                                      ,m_arrData[iSymbol].Bestprc.askBrokerKey
//                                      ,m_arrData[iSymbol].Bestprc.nGapBidAsk
//                                      ]));
//            m_arrData[iSymbol].Bestprc.nGapMax_Open := 0;
//          end;
//
//        end;
//
//      end;

    end;


  end;


  finally
    LeaveCriticalSection(m_arrCs[iSymbol]);

  end;
end;


function CDataHandler.Is_AlreadySet_BestPrc(iSymbol:integer):boolean;
begin
  Result := False;

  if (m_arrData[iSymbol].bestPrc.AskPrc <> '') and
      (m_arrData[iSymbol].bestPrc.AskPrc <> '0') and
      (m_arrData[iSymbol].bestPrc.bidPrc <> '') and
      (m_arrData[iSymbol].bestPrc.bidPrc <> '0') then
      Result := True;
end;

procedure CDataHandler.PrintNewBestPrc(bBid, bAsk:boolean; sNewBid, sNewAsk:string; iSymbol:integer);
begin
  if bBid then
  begin
    fmMain.AddMsg(True, Format('[BEST_BID] (%s)(%s) (prev:%s) < (new:%s). Gap(%d pt)',
                        [ m_arrData[iSymbol].symbol,
                          m_arrData[iSymbol].Bestprc.bidBrokerKey,
                          m_arrData[iSymbol].bestPrc.bidPrc,
                          sNewBid,
                          m_arrData[iSymbol].bestPrc.nGapBidAsk
                        ] ));
  end;
  if bAsk then
  begin
    fmMain.AddMsg(True, Format('[BEST_ASK] (%s)(%s) (prev:%s) > (new:%s). Gap(%d pt)',
                      [ m_arrData[iSymbol].symbol,
                        m_arrData[iSymbol].bestPrc.askBrokerKey,
                        m_arrData[iSymbol].bestPrc.AskPrc,
                        sNewAsk,
                        m_arrData[iSymbol].bestPrc.nGapBidAsk
                      ] ));
  end;
end;

function CDataHandler.Is_UnderActiveSpread(iSymbol:integer; sSymbol, sSpread:string):boolean;
var
  dSpread    : double;
  sIntSpread : string;
  nSpread : integer;
begin

  Result  := True;
  dSpread := strtofloat(sSpread);
  if dSpread =0 then
    nSpread := 0
  else
  begin
    sIntSpread := FormatFloat('#', strtofloat(sSpread) );
    nSpread := strtoint(sIntSpread);
  end;

  if m_arrData[iSymbol].Spec.nActiveSpreadPt < nSpread then
  begin

    fmMain.AddMsg(False, Format('[MAX_SPREAD] (%s) spread(%s) is over (%d)',
                           [sSymbol, sSpread, m_arrData[iSymbol].Spec.nActiveSpreadPt]));

    Result := False;
  end;

end;

end.
