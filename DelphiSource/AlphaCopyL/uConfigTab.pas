unit uConfigTab;

interface

uses
  System.Classes, Windows, Messages, SysUtils, Variants, Graphics, Controls, Dialogs
  ,uLocalCommon,AdvUtil, AdvObj, AdvGrid, Vcl.ExtCtrls, Vcl.StdCtrls, vcl.comctrls
  ;

type

  TLeftMT4Grid = record
    gdTerminal    : ^TAdvStringGrid;
    selectedRow   : integer;
    selectedAlias : string;
  end;

  TMasterTab = record
    //pnlMaster     : ^TPanel;
    //tab           : ^TTabSheet;
    gdSymbol      : ^TAdvStringGrid;
    SymbolCnt     : integer;
    lblSymbolCnt  : ^TLabel;
    edtSymbolAdd  : ^TEdit;
  end;

  TCopierSymbolTab = record
    //pnlCopier  : ^TPanel;
    //tab           : ^TTabSheet;
    gdSymbolM  : ^TAdvStringGrid;
    gdSymbolC  : ^TAdvStringGrid;
    lblMasterAlias : ^TLabel;
    lblCopierAlias : ^TLabel;
    lblSymbolTotM  : ^TLabel;
    lblSymbolTotC  : ^TLabel;
    SymbolCntM     : Integer;
    SymbolCntC     : Integer;
    useMCode       : ^TCheckBox;
  end;

  TCopierOptionTab = record
    //tab : ^TTabSheet;
    trade,
    signal      : ^TRadioButton;

    sl,
    tp          : ^TCheckBox;
    sl_sameprc,
    sl_samedist : ^TRadioButton;

    multiple,
    fixed         : ^TRadioButton;
    multiple_val,
    fixed_val     : ^TEdit;
    maxone,
    maxtot        : ^TCheckBox;
    maxone_val,
    maxtot_val    : ^TEdit;

    mktopen,
    mktclose      : ^TCheckBox;

    pending_limit,
    pending_stop  : ^TCheckBox;

    slippage      : ^TCheckBox;
    slippage_val  : ^TEdit;

    noncopytimeout : ^TCheckBox;
    noncopytimeout_min : ^TEdit;

  end;

  CConfigTab = class(TObject)

    public  procedure   Init();
    public  procedure   Select_MT4_fromLeft(row:integer);


    private procedure   MasterTab_Show();
    public  procedure   MasterTab_AddSymbol(symbol:string);  // editbox 입력한 symbol grid 에 add
    public  procedure   MasterTab_RemoveSymbol(row:integer); // grid double click 으로 remove
    public  procedure   MasterTab_SaveSymbol() ;             // master EA config 에 저장
    public  procedure   MastreTab_LoadSymbols();

    public  Procedure   CSymTab_Show();
    public  procedure   CSymTab_AddSymbol(masterRow:integer);    // master grid 더블클릭으로 copier grid 에 add
    public  procedure   CSymTab_RemoveSymbol(copierRow:integer); // copier grid 의 remove 클릭으로 remove
    public  procedure   CSymTab_ClrAll();
    public  procedure   CSymTab_SaveSymbol();                    // copier EA config 에 저장
    public  procedure   CSymTab_LoadAllSymbols();                // master grid 와 copier grid 에 symbol 들 load
    public  Procedure   CSymTab_UseMasterCode(bUse:boolean);

    public  function    IsMatchedMCType(bMasterTab:boolean):boolean;


    public  procedure   COptionTab_Show();
    public  procedure   COptionTab_LoadOptions();
    public  procedure   COptionTab_Save();


    public  procedure   Reload_LeftMT4List();   // Main 이 MT4Info 에서 읽은 mt4 리스트를 left grid 에 표시
    public  function    Is_MT4List_selected():boolean;
  private

    m_leftTerminal  : TLeftMT4Grid;
    m_tabM        : TMasterTab;
    m_tabCSym     : TCopierSymbolTab;
    m_tabCOpt     : TCopierOptionTab;

    m_pgCfg    : ^TPageControl;
  end;

var
  __cfg : CConfigTab;

  procedure __CreateCfgTabClass();

implementation


uses
  fmMainU, uEAConfigFile, CommonVal, uMasterCopierTab, CommonUtils
  ,uTcpSvr
  ;


procedure __CreateCfgTabClass();
begin
  __cfg := CConfigTab.Create;

  //__CreateSymbolsEditClass();
end;



procedure CConfigTab.Init();
var
  i : integer;
begin

  m_leftTerminal.gdTerminal     := @fmMain.gdCfgMT4;
  m_leftTerminal.selectedRow    := 0;
  m_leftTerminal.selectedAlias  := '';

  m_pgCfg             := @fmMain.pgCfg;

  //m_tabM.pnlMaster    := @fmMain.pnlCfgM;
  m_tabM.gdSymbol     := @fmMain.gdSymbolM;
  m_tabM.lblSymbolCnt := @fmMain.lblSymbolMCnt;
  m_tabM.SymbolCnt    := 0;
  m_tabM.edtSymbolAdd := @fmMain.edtSymbolAdd;

  //m_tabCSym.pnlCopier  := @fmMain.pnlCfgC;
  m_tabCSym.gdSymbolM  := @fmMain.gdSymbolM2;
  m_tabCSym.gdSymbolC  := @fmMain.gdSymbolC;
  m_tabCSym.lblMasterAlias := @fmMain.lblMasterAlias;
  m_tabCSym.lblCopierAlias := @fmMain.lblCopierAlias;
  m_tabCSym.lblSymbolTotM  := @fmMain.lblSymbolTotM;
  m_tabCSym.lblSymbolTotC  := @fmMain.lblSymbolTotC;
  m_tabCSym.useMCode       := @fmMain.chkUseMCode;
  m_tabCSym.SymbolCntM     := 0;
  m_tabCSym.SymbolCntC     := 0;


  // copier options tab
  m_tabCOpt.trade       := @fmMain.rdoTrade;
  m_tabCOpt.signal      := @fmMain.rdoSignal;

  m_tabCOpt.sl          := @fmMain.chkSL;
  m_tabCOpt.tp          := @fmMain.chkTP;
  m_tabCOpt.sl_sameprc  := @fmMain.rdoSLSamePrc;
  m_tabCOpt.sl_samedist := @fmMain.rdoSLSameDist;

  m_tabCOpt.multiple    := @fmMain.rdoMultiplier;
  m_tabCOpt.fixed       := @fmMain.rdoFixedLots;
  m_tabCOpt.multiple_val:= @fmMain.edtMultiplier;
  m_tabCOpt.fixed_val   := @fmMain.edtFixedLots;
  m_tabCOpt.maxone      := @fmMain.chkMaxOneOrd;
  m_tabCOpt.maxtot      := @fmMain.chkMaxTotOrd;
  m_tabCOpt.maxone_val  := @fmMain.edtMaxOneOrd;
  m_tabCOpt.maxtot_val  := @fmMain.edtMaxTotOrd;

  m_tabCOpt.mktopen     := @fmMain.chkmktOpen;
  m_tabCOpt.mktclose    := @fmMain.chkMktClose;

  m_tabCOpt.pending_limit := @fmMain.chkLimitOrd;
  m_tabCOpt.pending_stop  := @fmMain.chkStopOrd;

  m_tabCOpt.slippage      := @fmMain.chkSlippage;
  m_tabCOpt.slippage_val  := @fmMain.edtSlippage;

  m_tabCOpt.noncopytimeout      := @fmMain.chkNonCopyTimeout;
  m_tabCOpt.noncopytimeout_min  := @fmMain.edtNonCopyMins;


  //m_tabCOpt.slippage.Caption := 'Max difference between'+#13#10+'Master price and current market price';

  m_tabCOpt.multiple_val.ShowHint := true;
  m_tabCOpt.multiple_val.Hint     := '0.01 ~ 100 (unit 0.01)#13#10Multipler value = 2, Master Lots = 0.1 -> 0.2';

  m_tabCOpt.fixed_val.ShowHint := true;
  m_tabCOpt.fixed_val.Hint     := '0.01 ~ 100 (unit 0.01)';

  m_tabCOpt.maxone_val.ShowHint := true;
  m_tabCOpt.maxone_val.Hint     := '0.01 ~ 100 (unit 0.01)';

  m_tabCOpt.maxtot_val.ShowHint := true;
  m_tabCOpt.maxtot_val.Hint     := '0.01 ~ 100 (unit 0.01)';

  m_tabCOpt.mktopen.ShowHint := true;
  m_tabCOpt.mktopen.Hint     := 'Copying Open order is imperative';

  m_tabCOpt.slippage_val.ShowHint := true;
  m_tabCOpt.slippage_val.Hint     := '2 ~ 100 (unit 0.1)';


  end;


// left grid 에서 선택된 MT4 가 있는가?
function  CConfigTab.Is_MT4List_selected():boolean;
BEGIN
  Result := ( (m_leftTerminal.selectedRow >-1 ) and (m_leftTerminal.selectedAlias<>'') )
END;


// Main 이 MT4Info 에서 읽은 mt4 리스트를 left grid 에 표시
procedure CConfigTab.Reload_LeftMT4List();
var
  i : integer;
begin
  m_leftTerminal.gdTerminal.RowCount := __TerminalCnt()+2;

  for i := 0 to __TerminalCnt()-1 do
  begin
    m_leftTerminal.gdTerminal.Cells[0, i+1] := fmMain.MT4Info[i].Alias;
    m_leftTerminal.gdTerminal.Cells[1, i+1] := __MCTpDesc(fmMain.MT4Info[i].MCTp);
  end;

  m_leftTerminal.gdTerminal.Row := m_leftTerminal.gdTerminal.RowCount-1;

end;


{
  선택된 Tab과 선택된Mt4 의 MC type 이 일치하는가
}

function CConfigTab.IsMatchedMCType(bMasterTab:boolean):boolean;
var
  iTerminal : integer;
  mcTp      : string;
begin

  iTerminal := __GetTerminalIdx( m_leftTerminal.selectedAlias );
  if iTerminal < 0 then
    exit;

  mcTp := fmMain.MT4Info[iTerminal].MCTp;

  Result := True;

  if (bMasterTab=True) and (mcTp<>'M') then
    Result := False;

  if (bMasterTab=False) and (mcTp<>'C') then
    Result := False;



end;


{
  선택된 MT4 가 master 인지 copier 인지에 따라 config 를 달리 보여준다.
}
procedure CConfigTab.Select_MT4_fromLeft(row:integer);
var
  idx : integer;
begin
  if row < 1 then
    exit;

  // Master 가 아직 설정되지 않았으면 이것 먼저 설정하게 강제로 이동한다.
  if __MasterAlias()='' then
  begin
    fmMain.AddMsg('Please set Master first');
    fmMain.pgMain.ActivePage := fmMain.tabMainMC;
    exit;
  end;


  m_leftTerminal.selectedAlias := m_leftTerminal.gdTerminal.Cells[0, row];

  m_leftTerminal.selectedRow := __GetTerminalIdx(m_leftTerminal.gdTerminal.Cells[0, row]);
  if m_leftTerminal.selectedRow < 0 then
    exit;

  MastreTab_LoadSymbols();

  if fmMain.MT4Info[m_leftTerminal.selectedRow].MCTp = 'M' then
  begin
    MasterTab_Show();
  end
  else if fmMain.MT4Info[m_leftTerminal.selectedRow].MCTp = 'C' then
  begin

    if m_pgCfg.ActivePage = fmMain.tabMaster then
      CSymTab_Show()
    else if m_pgCfg.ActivePage = fmMain.tabCopierSymbols then
      CSymTab_Show()
    else if m_pgCfg.ActivePage = fmMain.tabCopierCfg then
      COptionTab_Show()
    ;

  end
  ;

end;


//procedure CConfigTab.Master_ClearSymbols();
//begin
//  m_gdSymbolM.ClearNormalCells;
//  m_symbolM_cnt := 0;
//  MasterTab_ShowSymbolCnt();
//end;


{
  // copier EA config 에 저장
}
procedure CConfigTab.CSymTab_SaveSymbol;
var
  key   : string;
  val   : string;
  i     : integer;
  path  : string;
  iTerminal : integer;
  sendBuf, sCode : string;
begin

  if not __Confirm('Confirm to Save?') then
    exit;

  // COUNT
  val       := inttostr( m_tabCSym.SymbolCntC );
  iTerminal := __GetTerminalIdx(m_leftTerminal.selectedAlias);
  if iTerminal < 0 then    exit;


  path      := fmMain.MT4Info[iTerminal].CopyPath;

  __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_SYMBOL_C, 'CNT', val);

  for i := 1 to m_tabCSym.SymbolCntC do
  begin

    if m_tabCSym.gdSymbolC.Cells[1, i]='' then
    begin
      fmMain.AddMsg('Need to Add Copier Code', false, true);
      exit;
    end;

    key := 'SYMBOL'+inttostr(i);
    val := format('%s=%s', [
                            m_tabCSym.gdSymbolC.Cells[0, i],
                            m_tabCSym.gdSymbolC.Cells[1, i]
                            ]
                            );

    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_SYMBOL_C, key, val);
  end;

  // Send data to EA
  __Set_CfgSymbolBuffer(iTerminal, MCTP_COPIER, sendBuf, sCode);
  __tcpSvr.SendData(iTerminal, sCode, sendBuf);


end;


{
  EA Config 저장
}

// master EA config 에 저장
procedure CConfigTab.MasterTab_SaveSymbol() ;
var
  key   : string;
  val   : string;
  i     : integer;
  path  : string;
  iTerminal : integer;
  sendBuf   : string;
  sCode     : string;
begin

  if not __Confirm('Confirm to Save?') then
    exit;

  // COUNT
  val       := inttostr( m_tabM.SymbolCnt);
  iTerminal := __GetTerminalIdx(m_leftTerminal.selectedAlias);
  if iTerminal < 0 then exit;

  path      := fmMain.MT4Info[iTerminal].CopyPath;

  __EACnfg_Save(m_leftTerminal.selectedAlias,  path, SEC_SYMBOL_M, 'CNT', val);

  for i := 1 to m_tabM.SymbolCnt do
  begin

    key := 'SYMBOL'+inttostr(i);
    val := m_tabM.gdSymbol.Cells[0, i];

    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_SYMBOL_M, key, val);
  end;

    // Send data to EA
  __Set_CfgSymbolBuffer(iTerminal, MCTP_MASTER, sendBuf, sCode);
  __tcpSvr.SendData(iTerminal, sCode, sendBuf);

end;



{
  Master BackGround 의 Grid 에 올린다.

[SYMBOL_M]
CNT=5
1=EURUSD
2=EURGBP
3=GBPUSD
4=EURJPY
5=USDJPY
}

procedure CConfigTab.MastreTab_LoadSymbols();
var
  symbolCnt : integer;
  val : string;
  key : string;
  idx : integer;
  path : string;
  masterAlias : string;
  iTerminal   : integer;
begin

  masterAlias := __MasterAlias();
  iTerminal   := __GetTerminalIdx(masterAlias);
  if iTerminal < 0 then exit;

  path        := fmMain.MT4Info[iTerminal].CopyPath;
  val         := __EACnfg_Get(path, SEC_SYMBOL_M, 'CNT');
  symbolCnt   := strtoint(val);

  m_tabM.gdSymbol.ClearNormalCells;
  m_tabM.gdSymbol.RowCount  := symbolCnt+1;


  m_tabM.SymbolCnt := 0;
  for idx := 1 to symbolCnt do
  begin
    key := 'SYMBOL'+inttostr(idx);
    val := __EACnfg_Get(path, SEC_SYMBOL_M, key);

    if Trim(val) = '' then
      break;

    //if idx > 2 then
    //  m_gdSymbolM.AddRow;

    Inc (m_tabM.SymbolCnt);

    m_tabM.gdSymbol.Cells[0, m_tabM.SymbolCnt] := val;

  end;

  m_tabM.lblSymbolCnt.Caption := inttostr(m_tabM.SymbolCnt);

end;



{
  Master 의 symbol code 를 그대로 이용할 것인가?
}
Procedure CConfigTab.CSymTab_UseMasterCode(bUse:boolean);
var
  i : integer;
begin

  for i := 1 to m_tabCSym.SymbolCntC do
  begin
    if m_tabCSym.useMCode.Checked then
      m_tabCSym.gdSymbolC.cells[1, i] := m_tabCSym.gdSymbolC.cells[0, i]
    else
      m_tabCSym.gdSymbolC.cells[1, i] := ''
    ;
  end;



end;

{

master grid 와 copier grid 를 각 EA Config 에서 loading 한다.

[SYMBOL_M]
CNT=5
SYMBOL1=EURUSD
SYMBOL2=EURGBP

[SYMBOL_C]
CNT=5
SYMBOL1=EURUSD=EURUSD
SYMBOL2=EURGBP=EURGBP
SYMBOL3=GBPUSD=GBPUSD
SYMBOL4=EURJPY=EURJPY
SYMBOL5=USDJPY=USDJPY
}
procedure CConfigTab.CSymTab_LoadAllSymbols();
var
  symbolCnt : integer;
  val : string;
  key : string;
  idx : integer;
  iRow : integer;
  path : string;
  iTerminal : integer;
  sMasterAlias : string;
  sM, sC       : string;
  iPos         : integer;
begin

  ///////////////////////////////////////////
  ///  Master Grid

  sMasterAlias  := __MasterAlias();
  iTerminal     := __GetTerminalIdx( sMasterAlias );
  if iTerminal < 0 then exit;

  path          := fmMain.MT4Info[ iTerminal ].CopyPath;
  val           := __EACnfg_Get(path, SEC_SYMBOL_M, 'CNT');
  symbolCnt     := strtoint(val);

  m_tabCSym.gdSymbolM.ClearNormalCells;
  m_tabCSym.gdSymbolM.RowCount  := m_tabM.gdSymbol.RowCount;

  for idx := 1 to m_tabCSym.gdSymbolM.RowCount do
  begin

    key := 'SYMBOL'+inttostr(idx);
    val := __EACnfg_Get(path, SEC_SYMBOL_M, key);

    if Trim(val) = '' then
      break;

    m_tabCSym.gdSymbolM.Cells[0, idx] := val;

  end;


  ///////////////////////////////////////////
  ///  Copier Grid

  iTerminal     := __GetTerminalIdx( m_leftTerminal.selectedAlias );
  if iTerminal < 0 then exit;

  path          := fmMain.MT4Info[ iTerminal ].CopyPath;
  val           := __EACnfg_Get(path, SEC_SYMBOL_C, 'CNT');
  symbolCnt     := strtoint(val);

  m_tabCSym.gdSymbolC.ClearNormalCells;
  m_tabCSym.gdSymbolC.RowCount  := 2;
  m_tabCSym.SymbolCntC          := 0;
  m_tabCSym.lblSymbolTotC.Caption := '0';

  for idx := 1 to symbolCnt do
  begin

    key := 'SYMBOL'+inttostr(idx);
    val := __EACnfg_Get(path, SEC_SYMBOL_C, key);

    if Trim(val) = '' then
      break;

    iPos := Pos('=', val);
    if iPos < 1 then
      continue;

    sM := Copy(val, 1, iPos-1);
    sC := Copy(val, iPos+1, Length(val) - iPos);

    iRow := __PrepareAddNewRow(m_tabCSym.gdSymbolC^);

    m_tabCSym.gdSymbolC.Cells[0, iRow] := sM;
    m_tabCSym.gdSymbolC.Cells[1, iRow] := sC;
    m_tabCSym.gdSymbolC.Cells[2, iRow] := 'Remove';

    Inc(m_tabCSym.SymbolCntC);
    m_tabCSym.lblSymbolTotC.Caption := inttostr(m_tabCSym.SymbolCntC);
  end;



end;

procedure CConfigTab.MasterTab_Show();
begin

  m_pgCfg.ActivePage := fmMain.tabMaster;

end;


procedure CConfigTab.CSymTab_Show();
var
  i : integer;
  iTerminal : integer;
begin

  m_pgCfg.ActivePage := fmMain.tabCopierSymbols;

  m_tabCSym.lblCopierAlias.caption := m_leftTerminal.selectedAlias;
  m_tabCSym.lblMasterAlias.Caption := __MasterAlias();
  m_tabCSym.lblSymbolTotM.caption := '';
  m_tabCSym.lblSymbolTotC.caption := '';
  m_tabCSym.SymbolCntM            := 0;
  m_tabCSym.SymbolCntC            := 0;


  m_tabCSym.gdSymbolM.ClearNormalCells;
  m_tabCSym.gdSymbolM.RowCount := m_tabM.gdSymbol.RowCount;


  for i := 1 to m_tabCSym.gdSymbolM.RowCount do
  begin

    m_tabCSym.gdSymbolM.Cells[0,i] := m_tabM.gdSymbol.Cells[0,i];
    m_tabCSym.gdSymbolM.Cells[3,i] := 'Remove';

  end;

    // Master ID 가 선택된 상태이면 Copier symbol grid 의 내용을 채우지 않는다.
  iTerminal := __GetTerminalIdx( m_leftTerminal.selectedAlias );
  if iTerminal < 0 then exit;

  if fmMain.MT4Info[iTerminal].MCTp = 'M' then
    exit;

  CSymTab_LoadAllSymbols();

end;


{
  // master grid 더블클릭으로 copier grid 에 add
}
procedure CConfigTab.CSymTab_AddSymbol(masterRow:integer);
var
  symbol : string;
  idx    : integer;
begin

  // master
  symbol := m_tabM.gdSymbol.Cells[0, masterRow];
  if symbol='' then
    exit;

  // copier
  idx := __PrepareAddNewRow(m_tabCSym.gdSymbolC^);
  m_tabCSym.gdSymbolC.Cells[0, idx] := symbol;
  if m_tabCSym.useMCode.Checked=true then
    m_tabCSym.gdSymbolC.Cells[1, idx] := symbol;

  m_tabCSym.gdSymbolC.Cells[2, idx] := 'Remove';

  Inc(m_tabCSym.SymbolCntC);
  m_tabCSym.lblSymbolTotC.Caption := inttostr(m_tabCSym.SymbolCntC);

end;


{
  // copier grid 의 remove 클릭으로 remove
}
procedure CConfigTab.CSymTab_RemoveSymbol(copierRow:integer);
begin
  if not __Confirm('Confirm to Remove') then
    exit;

  m_tabCSym.gdSymbolC.RemoveRows(copierRow, 1);

  Dec(m_tabCSym.SymbolCntC);
  m_tabCSym.lblSymbolTotC.Caption := inttostr(m_tabCSym.SymbolCntC);
end;

procedure CConfigTab.CSymTab_ClrAll();
begin
  m_tabCSym.gdSymbolC.ClearNormalCells;
  m_tabCSym.gdSymbolC.RowCount    := 2;
  m_tabCSym.SymbolCntC            := 0;
  m_tabCSym.lblSymbolTotC.Caption := '0';
end;

{
// grid double click 으로 remove
}
procedure CConfigTab.MasterTab_RemoveSymbol(row:integer);
begin
  if row < 1 then
    exit;

  m_tabM.gdSymbol.RemoveRows(row, 1);
  Dec(m_tabM.SymbolCnt);
  m_tabM.lblSymbolCnt.Caption := inttostr(m_tabM.SymbolCnt);
end;



// editbox 입력한 symbol grid 에 add
procedure CConfigTab.MasterTab_AddSymbol(symbol:string);
begin

  if symbol='' then
  begin
    fmMain.AddMsg('Add symbol on the edit box', false, true);
    exit;
  end;

  if m_tabM.gdSymbol.RowCount = MAX_MASTER_SYMBOL+1 then
  begin
    fmMain.AddMsg('No more symbols', false, true);
    exit;
  end;

  m_tabM.gdSymbol.AddRow;
  m_tabM.gdSymbol.Cells[0, m_tabM.gdSymbol.RowCount-1] := symbol;
  Inc(m_tabM.SymbolCnt);
  m_tabM.lblSymbolCnt.Caption := inttostr(m_tabM.SymbolCnt);
  m_tabM.edtSymbolAdd.Clear;
end;


procedure CConfigTab.COptionTab_Show();
begin
  m_pgCfg.ActivePage := fmMain.tabCopierCfg;

  COptionTab_LoadOptions();
end;



// 해당 EA config 에 저장
procedure CConfigTab.COptionTab_Save();
var
  key   : string;
  val   : string;
  i     : integer;
  path  : string;
  iTerminal : integer;
  sCode, sendBuf : string;
begin

  if not __Confirm('Confirm to Save?') then
    exit;

  iTerminal := __GetTerminalIdx(m_leftTerminal.selectedAlias);
  if iTerminal < 0 then exit;


  path := fmMain.MT4Info[iTerminal].CopyPath;

  /////////////////////////////////////////////////
  ///  check the validity of data


  // sl / tp
  if (m_tabCOpt.sl.Checked=true) and (m_tabCOpt.tp.Checked=true) then
  begin
    if (m_tabCOpt.sl_sameprc.Checked=false) and (m_tabCOpt.sl_samedist.Checked=false) then
    begin
      fmMain.AddMsg('Please select same price or same distance option', false, true);
    end;
  end;


  // multiplier
  if m_tabCOpt.multiple.Checked=true then
  begin
    if strtofloatdef(m_tabCOpt.multiple_val.Text,0) = 0 then
    begin
      fmMain.AddMsg('Please put Multiplier number', false, true);
      exit;
    end;
  end;

  // fixed
  if m_tabCOpt.fixed.Checked=true then
  begin
    if strtofloatdef(m_tabCOpt.fixed_val.Text,0) = 0 then
    begin
      fmMain.AddMsg('Please put Fixed Lots number', false, true);
      exit;
    end;
  end;

  // max onelot
  if m_tabCOpt.maxone.Checked=true then
  begin
    if strtofloatdef(m_tabCOpt.maxone_val.Text,0) = 0 then
    begin
      fmMain.AddMsg('Please put Max lot size', false, true);
      exit;
    end;
  end;

  // max total lot
  if m_tabCOpt.maxtot.Checked=true then
  begin
    if strtofloatdef(m_tabCOpt.maxtot_val.Text,0) = 0 then
    begin
      fmMain.AddMsg('Please put Max Total lot size', false, true);
      exit;
    end;
  end;

  // slippage
  if m_tabCOpt.slippage.Checked=true then
  begin
    if strtofloatdef(m_tabCOpt.slippage_val.Text,0) = 0 then
    begin
      fmMain.AddMsg('Please put Slippage pips', false, true);
      exit;
    end;
  end;

  // non copy timeout
  if m_tabCOpt.noncopytimeout.Checked=true then
  begin
    if strtofloatdef(m_tabCOpt.noncopytimeout_min.Text,0) = 0 then
    begin
      fmMain.AddMsg('Please put timeout miniutes', false, true);
      exit;
    end;
  end;

  /////////////////////////////////////////////////
  ///  save data into the EA config file

  // copy option
  val := 'T';
  if m_tabCOpt.signal.Checked=True then val := 'S';
  __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, COPY_TP, val);


  // SL / TP
  val := 'N';
  if m_tabCOpt.sl.Checked=True then val := 'Y';
  __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, SL_YN, val);

  val := 'N';
  if m_tabCOpt.tp.Checked=True then val := 'Y';
  __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, TP_YN, val);

  if (m_tabCOpt.sl.Checked=false) and (m_tabCOpt.tp.Checked=false) then
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, SLTP_TYPE, '');
  end
  else
  begin
    if m_tabCOpt.sl_sameprc.Checked = True then
      val := inttostr(SLTP_SAMEPRC)
    else if m_tabCOpt.sl_samedist.Checked = True then
      val := inttostr(SLTP_SAMEDIST);

    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, SLTP_TYPE, val);
  end;


  // LOT SIZE
  if m_tabCOpt.multiple.Checked=True then
  begin

    val := inttostr(VOLTP_MULTI);
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, COPY_VOL_TP, val);

    val := m_tabCOpt.multiple_val.Text;
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, VOL_MULTIPLIER_VAL, val);

  end
  else
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, VOL_MULTIPLIER_VAL, '');
  end
  ;

  if m_tabCOpt.fixed.Checked=True then
  begin

    val := inttostr(VOLTP_FIXED);
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, COPY_VOL_TP, val);

    val := m_tabCOpt.fixed_val.Text;
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, VOL_FIXED_VAL, val);

  end
  else
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, VOL_FIXED_VAL, '');
  end;

  if m_tabCOpt.maxone.Checked=True then
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAXLOT_ONEORDER_YN, 'Y');

    val := m_tabCOpt.maxone_val.Text;
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAXLOT_ONEORDER_VAL, val);
  end
  else
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAXLOT_ONEORDER_YN, 'N');
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAXLOT_ONEORDER_VAL, '');
  end
  ;

  if m_tabCOpt.maxtot.Checked=True then
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAXLOT_TOTORDER_YN, 'Y');

    val := m_tabCOpt.maxtot_val.Text;
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAXLOT_TOTORDER_VAL, val);
  end
  else
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAXLOT_TOTORDER_VAL, 'Y');
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAXLOT_TOTORDER_VAL, '');
  end
  ;


  // market type
  if m_tabCOpt.mktopen.Checked=True then
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, ORD_OPEN_YN, 'Y')
  else
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, ORD_OPEN_YN, 'N')
  ;

  if m_tabCOpt.mktclose.Checked=True then
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, ORD_CLOSE_YN, 'Y')
  else
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, ORD_CLOSE_YN, 'N')
  ;


  // pending order
  if m_tabCOpt.pending_limit.Checked=True then
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, ORD_LIMIT_YN, 'Y')
  else
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, ORD_LIMIT_YN, 'N')
  ;

  if m_tabCOpt.pending_stop.Checked=True then
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, ORD_STOP_YN, 'Y')
  else
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, ORD_STOP_YN, 'N')
  ;

  // Slippage
  if m_tabCOpt.slippage.Checked=True then
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAX_SLPG_YN, 'Y');

    val := m_tabCOpt.slippage_val.Text;
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAX_SLPG_VAL, val);
  end
  else
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAX_SLPG_YN, 'N');
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, MAX_SLPG_VAL, '');
  end;


  // non copy timeout
  if m_tabCOpt.noncopytimeout.Checked=True then
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, NONCOPY_TIMEOUT_YN, 'Y');

    val := m_tabCOpt.noncopytimeout_min.Text;
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, NONCOPY_TIMEOUT_MIN, val);
  end
  else
  begin
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, NONCOPY_TIMEOUT_YN, 'N');
    __EACnfg_Save(m_leftTerminal.selectedAlias, path, SEC_CNFG_C, NONCOPY_TIMEOUT_MIN, '');
  end;


  ///////////////////////////////////////////////////////////////////////
  // Send data to EA
  __Set_CfgGeneralBuffer(iTerminal, MCTP_COPIER, sendBuf, sCode);
  __tcpSvr.SendData(iTerminal, sCode, sendBuf);
  ///////////////////////////////////////////////////////////////////////

end;


// 해당 MT4 EA config 파일에서 읽어온다.
procedure CConfigTab.COptionTab_LoadOptions();
var
  val : string;
  key : string;
  path : string;
  iTerminal : integer;
begin

  iTerminal     := __GetTerminalIdx( m_leftTerminal.selectedAlias );
  if iTerminal < 0 then exit;

  path := fmMain.MT4Info[ iTerminal ].CopyPath;


  // Copy Type
  val := __EACnfg_Get(path, SEC_CNFG_C, COPY_TP);
  if val='T' then m_tabCOpt.trade.Checked := true
  else            m_tabCOpt.signal.Checked := true
  ;


  // SL / TP
  val := __EACnfg_Get(path, SEC_CNFG_C, SL_YN);
  if val='Y' then m_tabCOpt.sl.Checked := True
  else            m_tabCOpt.sl.Checked := False
  ;

  val := __EACnfg_Get(path, SEC_CNFG_C, TP_YN);
  if val='Y' then m_tabCOpt.tp.Checked := True
  else            m_tabCOpt.tp.Checked := False
  ;

  if (m_tabCOpt.tp.Checked=false) and (m_tabCOpt.sl.Checked=false) then
  begin
    m_tabCOpt.sl_sameprc.Checked := false;
    m_tabCOpt.sl_samedist.Checked := false;
  end
  else
  begin
    // exact distance 가 default
    val := __EACnfg_Get(path, SEC_CNFG_C, SLTP_TYPE);
    if strtointdef(val, SLTP_SAMEPRC) = SLTP_SAMEPRC then
    BEGIN
      m_tabCOpt.sl_sameprc.Checked := true;
      m_tabCOpt.sl_samedist.Checked := false;
    END
    else
    begin
      m_tabCOpt.sl_sameprc.Checked := false;
      m_tabCOpt.sl_samedist.Checked := True;
    end
    ;
  end;


  // Volume
  val := __EACnfg_Get(path, SEC_CNFG_C, COPY_VOL_TP);
  if strtointdef(val, VOLTP_MULTI)=VOLTP_MULTI then
  begin
    m_tabCOpt.multiple.Checked := True;
    val := __EACnfg_Get(path, SEC_CNFG_C, VOL_MULTIPLIER_VAL);
    m_tabCOpt.multiple_val.Text := floattostr(strtofloatdef(val,0));

    m_tabCOpt.fixed_val.Text := '';
  end
  else
  begin
    m_tabCOpt.fixed.Checked := True;
    val := __EACnfg_Get(path, SEC_CNFG_C, VOL_FIXED_VAL);
    m_tabCOpt.fixed_val.Text := floattostr(strtofloatdef(val,0));

    m_tabCOpt.multiple_val.Text := '';;
  end
  ;


  val := __EACnfg_Get(path, SEC_CNFG_C, MAXLOT_ONEORDER_YN);
  if val='Y' then
  begin
    m_tabCOpt.maxone.Checked := True;

    val := __EACnfg_Get(path, SEC_CNFG_C, MAXLOT_ONEORDER_VAL);
    m_tabCOpt.maxone_val.Text := floattostr(strtofloatdef(val,0));;
  end
  else
  begin
    m_tabCOpt.maxone.Checked  := False;
    m_tabCOpt.maxone_val.Text := '';
  end
  ;

  val := __EACnfg_Get(path, SEC_CNFG_C, MAXLOT_TOTORDER_YN);
  if val='Y' then
  begin
    m_tabCOpt.maxtot.Checked := True;

    val := __EACnfg_Get(path, SEC_CNFG_C, MAXLOT_TOTORDER_VAL);
    m_tabCOpt.maxtot_val.Text := floattostr(strtofloatdef(val,0));;
  end
  else
  begin
    m_tabCOpt.maxtot.Checked  := False;
    m_tabCOpt.maxtot_val.Text := '';
  end
  ;


  // Order type - Market Order
  val := __EACnfg_Get(path, SEC_CNFG_C, ORD_OPEN_YN);
  if val='Y' then m_tabCOpt.mktopen.Checked := True
  else            m_tabCOpt.mktopen.Checked := False
  ;

  val := __EACnfg_Get(path, SEC_CNFG_C, ORD_CLOSE_YN);
  if val='Y' then m_tabCOpt.mktclose.Checked := True
  else            m_tabCOpt.mktclose.Checked := False
  ;


  // Order type - Pending Order
  val := __EACnfg_Get(path, SEC_CNFG_C, ORD_LIMIT_YN);
  if val='Y' then m_tabCOpt.pending_limit.Checked := True
  else            m_tabCOpt.pending_limit.Checked := False
  ;

  val := __EACnfg_Get(path, SEC_CNFG_C, ORD_STOP_YN);
  if val='Y' then m_tabCOpt.pending_stop.Checked := True
  else            m_tabCOpt.pending_stop.Checked := False
  ;


  // slippage
  val := __EACnfg_Get(path, SEC_CNFG_C, MAX_SLPG_YN);
  if val='Y' then
  begin
    m_tabCOpt.slippage.Checked := True;

    val := __EACnfg_Get(path, SEC_CNFG_C, MAX_SLPG_VAL);
    m_tabCOpt.slippage_val.Text := val;

  end
  else
  begin
    m_tabCOpt.slippage.Checked  := False;
    m_tabCOpt.slippage_val.Text := '';
  end
  ;


  // non copy timeout
  val := __EACnfg_Get(path, SEC_CNFG_C, NONCOPY_TIMEOUT_YN);
  if val='Y' then
  begin
    m_tabCOpt.noncopytimeout.Checked := True;

    val := __EACnfg_Get(path, SEC_CNFG_C, NONCOPY_TIMEOUT_MIN);
    m_tabCOpt.noncopytimeout_min.Text := inttostr(strtointdef(val, 3));
  end
  else
  begin
    m_tabCOpt.noncopytimeout.Checked  := False;
    m_tabCOpt.noncopytimeout_min.Text := '';
  end
  ;


  fmMain.ScrollBox1.Refresh;


end;

end.
