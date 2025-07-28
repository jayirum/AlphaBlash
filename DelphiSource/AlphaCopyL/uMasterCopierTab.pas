unit uMasterCopierTab;

interface

uses
  System.Classes, Windows, Messages, SysUtils, Variants, Graphics, Controls, Dialogs
  ,uLocalCommon,AdvUtil, AdvObj, AdvGrid
  ;

const
  MC_MASTER = 1;
  MC_LIST   = 2;
  MC_COPIER = 3;


type
  CMasterCopierTab = class(TObject)
  public
    procedure   Init();

    procedure   Reload_MasterAndCopiers();
    procedure   Move(gdFromIdx, gdToIdx : integer);
    function    Save():boolean;
    procedure   SelectRow(gdIdx, row:integer);

  private
    function   EA_Save_MCTp(sMCTp:string; sAlias:string):boolean;
    function   ExistAlias(mcIdx:integer; sAlias:string; var oRow:integer):boolean;

  private
    m_gd          : array[1..3] of ^TAdvStringGrid;
    m_SelectedRow : array[1..3] of integer;
    m_gdDataCnt   : array[1..3] of integer;
  end;

var
  __mc : CMasterCopierTab;

  procedure __CreateMCTabClass;
  function  __MasterAlias():string;

implementation


uses
  fmMainU, CommonVal, CommonUtils, uEAConfigFile, uTcpSvr
  ;


procedure __CreateMCTabClass;
BEGIN
  __mc := CMasterCopierTab.Create;

END;

// Master grid 에서 alias 를 가져온다.
function  __MasterAlias():string;
begin
  Result := __mc.m_gd[MC_MASTER].Cells[0, 1];
end;


procedure CMasterCopierTab.Init();
begin
  __mc.m_gd[MC_MASTER] := @fmMain.gdMaster;
  __mc.m_gd[MC_LIST]   := @fmMain.gdMT4List;
  __mc.m_gd[MC_COPIER] := @fmMain.gdCopier;

end;

procedure CMasterCopierTab.SelectRow(gdIdx, row:integer);
begin
  m_SelectedRow[gdIdx] := row;
end;


procedure CMasterCopierTab.Reload_MasterAndCopiers();
var
  i
  ,iTerminal
  ,iGrid    : integer;
  val       : string;
begin


  for i := 1 to length(m_SelectedRow) do
  begin
    m_gd[i].ClearNormalCells;
    //m_gd[i].RowCount  := MAX_MT4_COUNT;
    m_SelectedRow[i]  := 0;
    m_gdDataCnt[i]    := 0;
  end;


  for iTerminal := 0 to __TerminalCnt()-1 do
  begin
    // Read from EA config file
    val := __EACnfg_Get(fmMain.MT4Info[iTerminal].CopyPath, SEC_MC_TP, 'MC_TP');

    if val='M' then
    begin
      Inc(m_gdDataCnt[MC_MASTER]);
      iGrid := m_gdDataCnt[MC_MASTER];

      m_gd[MC_MASTER].cells[0, iGrid] := fmMain.MT4Info[iTerminal].Alias;

    end
    else if val='C' Then
    begin
      Inc(m_gdDataCnt[MC_COPIER]);
      iGrid := m_gdDataCnt[MC_COPIER];

      m_gd[MC_COPIER].cells[0, iGrid] := fmMain.MT4Info[iTerminal].Alias;
    end
    else
    begin
      Inc(m_gdDataCnt[MC_LIST]);
      iGrid := m_gdDataCnt[MC_LIST];

      m_gd[MC_LIST].cells[0, iGrid] := fmMain.MT4Info[iTerminal].Alias;
    end
    ;

  end;


  for i := 1 to length(m_SelectedRow) do
  begin
    m_gd[i].Row  := MAX_MT4_COUNT;
  end;


end;




function CMasterCopierTab.ExistAlias(mcIdx:integer; sAlias:string; vAR oRow:integer):boolean;
var
  i : integer;
begin

  Result := False;

  for i := 1 to m_gdDataCnt[mcIdx] do
  begin
    if sAlias = m_gd[mcIdx].Cells[0, i] then
    begin
      Result := True;
      oRow    := i;
      exit;
    end;

  end;

end;

{
  각 EA 폴더에 있는 AlphaCopyL.ini 에 저장한다.
}
function CMasterCopierTab.Save():boolean;
var
  i,
  oRow,
  iTerminal,
  cnt       : integer;
  sAlias    : string;
  mcTp      : string;
  sendBuf   : string;
  sCode     : string;
begin

  if Not __Confirm('Confirm to Save?') then
    exit;

  // check Master Grid
  cnt := 0;
  for i := 1 to m_gdDataCnt[MC_MASTER] do
  BEGIN
    if m_gd[MC_MASTER].Cells[0, i] <> '' then
      cnt := cnt + 1;

  END;

//  if cnt=0 then
//  begin
//    fmMain.AddMsg('No Master!', false, true);
//    exit;
//  end;

  if cnt>1 then
  begin
    fmMain.AddMsg('Only one Master is allowed', false, true);
    exit;
  end;


  cnt := Length(fmMain.MT4Info);
  for iTerminal := 0 to cnt-1 do
  BEGIN

    sAlias := fmMain.MT4Info[iTerminal].Alias;

    if ExistAlias(MC_MASTER, sAlias, oRow)=True  then
    begin
      IF NOT EA_Save_MCTp('M', sAlias) THEN
        EXIT;

      fmMain.MT4Info[iTerminal].MCTp := 'M';
      mcTp := MCTP_MASTER;

    end;

    if ExistAlias(MC_COPIER, sAlias, oRow)=True  then
    begin
      IF NOT EA_Save_MCTp('C', sAlias) THEN
        EXIT;

      fmMain.MT4Info[iTerminal].MCTp := 'C';
      mcTp := MCTP_COPIER;
    end;

    if ExistAlias(MC_LIST, sAlias, oRow)=True  then
    begin

      IF NOT EA_Save_MCTp('', sAlias) THEN
        EXIT;
      fmMain.MT4Info[iTerminal].MCTp := '';
    end;
  END;


  // Send data to EAs
  if mcTp<>'' then
  begin
    __Set_CfgSymbolBuffer(iTerminal, mcTp, sendBuf, sCode);

    if mcTp=MCTP_COPIER then
      __tcpSvr.SendData(ALL_COPIERS, sCode, sendBuf)
    else
      __tcpSvr.SendData(ALL_COPIERS, sCode, sendBuf)
    ;

  end;

  Result := True;

end;

function CMasterCopierTab.EA_Save_MCTp(sMCTp:string; sAlias:string):boolean;
var
  iTerminal : integer;
  sendBuf   : string;
  sCode     : string;
begin
  Result := False;

  iTerminal := __GetTerminalIdx(sAlias);
  if iTerminal<0 then exit;

  __EACnfg_Save(sAlias, fmMain.MT4Info[iTerminal].CopyPath, SEC_MC_TP, 'MC_TP', sMCTp);

  // Send data to EAs
  __Set_ResetMCTpBuffer(iTerminal, sMCTp, sendBuf, sCode);
  __tcpSvr.SendData(ALL_COPIERS, sCode, sendBuf);

  Result := True;
end;

{
  리스트 끼리 이동
}
procedure CMasterCopierTab.Move(gdFromIdx, gdToIdx : integer);
var
  sAlias  : string;
  arr     : array[1..MAX_MT4_COUNT] of string;
  idx     : integer;
  cnt     : integer;
begin

  if m_SelectedRow[gdFromIdx] <= 0 then
  begin
    fmMain.AddMsg('Select item to be moved first!');
    exit;
  end;

  sAlias := m_gd[gdFromIdx].Cells[0, m_SelectedRow[gdFromIdx]];
  if sAlias.Length=0 then
    exit;

  /////////////////////////////////////////
  // remove FROM grid
  m_gd[gdFromIdx].RemoveRows(m_SelectedRow[gdFromIdx], 1);
  m_gd[gdFromIdx].AddRow;

  m_SelectedRow[gdFromIdx] := 0;
  m_gdDataCnt[gdFromIdx]   := m_gdDataCnt[gdFromIdx]-1;
  m_gd[gdFromIdx].Row      := MAX_MT4_COUNT;
  //
  /////////////////////////////////////////



  /////////////////////////////////////////
  // TO grid re sorting
  cnt := m_gdDataCnt[gdToIdx] + 1;

  m_gd[gdToIdx].Cells[0, cnt] := sAlias;
  m_gdDataCnt[gdToIdx]        := cnt;

  m_SelectedRow[gdToIdx]  := 0;
  m_gd[gdToIdx].Row       := MAX_MT4_COUNT;
  //
  /////////////////////////////////////////

end;



end.
