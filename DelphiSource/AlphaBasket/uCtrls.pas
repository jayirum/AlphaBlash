unit uCtrls;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.ComCtrls, System.IOUtils, System.Types,
  WinApi.SHLObj, Vcl.Grids, ShellAPI, IdAntiFreezeBase, IdAntiFreeze,
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdHTTP, IniFiles,
  IdIOHandler, IdIOHandlerStream, CheckUpdateThread, FileCtrl, sSkinManager,
  Vcl.StdCtrls, sComboBoxes, sSkinProvider, sPageControl, sLabel, sPanel, sComboBox
  ,CommonUtils, sButton, acProgressBar, acImage, system.zip, CommonVal
  ,AdvUtil,AdvObj, BaseGrid, AdvGrid
  ,IdExceptionCore,
  IdCustomTCPServer, IdTCPServer, IdContext
  ,uQueue
  ,uBasketCommon
  ;


type
  TCtrls = record
    symbol        : ^TEdit;
    key_buy       : ^TEdit;
    key_sell      : ^TEdit;
    broker_buy    : ^TEdit;
    broker_sell   : ^TEdit;
    baseSpread    : ^TEdit;
    tsLvl_1       : ^TEdit;
    tsLvl_2       : ^TEdit;
    tsOffset_1    : ^TEdit;
    tsOffset_2    : ^TEdit;
    openLots      : ^TEdit;
    gdPos         : ^TStringGrid;

    spreadMinB,
    spreadMaxB,
    spreadMinS,
    spreadMaxS    : ^TEdit;

    netPL         : ^TEdit;
  end;

var

  __ctrls : array [1..MAX_SYMBOL] of TCtrls;

  procedure __LinkSettingCtrls();

  function __gdDecimalCnt   (iSymbol, iSide:integer):integer;
  function __gdPipSize      (iSymbol, iSide:integer):double;
  function __gdOpenPrc      (iSymbol, iSide:integer):double;
  function __gdNowPrc       (iSymbol, iSide:integer):double;
  function __gdExistPosition(iSymbol, iSide:integer):boolean;
  function __gdCurrPLPip    (iSymbol, iSide:integer):double;
  function __gdIsSending    (iSymbol, iSide:integer):boolean;
  //function __gdMarkBeingCut (iSymbol, iSide:integer; bMark:boolean):boolean;
  function  __gdPLStatus    (iSymbol, iSide:integer):string;
  //TODO function  __gdPLStatusTp  (iSymbol, iSide:integer):TPL_STATUS;
  function  __gdTS_BestPip  (iSymbol, iSide:integer):double;
  //function  __gdTS_BestPrc  (iSymbol, iSide:integer):double;
  function  __gdLots        (iSymbol, iSide:Integer):double;
  function  __gdTicket      (iSymbol, iSide:Integer):string;
  function  __gdBroker      (iSymbol, iSide:Integer):string;
  function  __gdSpread      (iSymbol, iSide:Integer):double;


  function  __BaseSpreadPip (iSymbol:integer):double;
  function  __TS_LvlPip     (iSymbol:integer; iLvl:integer):double;
  function  __TS_OffSetPip  (iSymbol:integer; iLvl:integer):double;
  function  __OpenLots      (iSymbol:integer):double;
  function  __Symbol        (isymbol:integer):string;
  function  __AwayPrc       (iSymbol, iSide:integer; dBasePrc, dAwayPip:double; bProfit:boolean=True):double;
  function  __Broker        (iSymbol, iSide:integer):string;
  function  __Key           (iSymbol, iSide:integer):string;

  //TODO function  __PLStatusDesc  (status:TPL_STATUS):string;
  function  __Calc_LvlPrc (iSymbol, iSide:integer; lvl:integer):double;

  function  __CtrCnt():integer;

implementation

uses
  fmMainu
  ;

var
  _CtrlCnt : integer;

function  __CtrCnt():integer;
begin
  Result := _CtrlCnt;
end;

procedure __LinkSettingCtrls();
var
  i : integer;
begin

  _CtrlCnt := 0;

  i := 1;

  __ctrls[i].key_buy        := @fmMain.edtKeyBuy_A;
  __ctrls[i].key_sell       := @fmMain.edtKeySell_A;
  __ctrls[i].symbol         := @fmMain.edtSymbol_A;
  __ctrls[i].broker_buy     := @fmMain.edtBrokerBuy_A;
  __ctrls[i].broker_sell    := @fmMain.edtBrokerSell_A;
  __ctrls[i].basespread     := @fmMain.edtBaseSpread_A;
  __ctrls[i].tsLvl_1        := @fmMain.edtLvl_1_A;
  __ctrls[i].tsLvl_2        := @fmMain.edtLvl_2_A;
  __ctrls[i].tsOffset_1     := @fmMain.edtOffset_1_A;
  __ctrls[i].tsOffset_2     := @fmMain.edtOffset_2_A;
  __ctrls[i].openLots       := @fmMain.edtOpenLots;
  //TODO __ctrls[i].gdPos          := @fmMain.gdPos_A;

  __ctrls[i].gdPos.ColWidths[POS_PL] := 0;

  __ctrls[i].spreadMinB     := @fmMain.edtSpreadMinB;
  __ctrls[i].spreadMaxB     := @fmMain.edtSpreadMaxB;
  __ctrls[i].spreadMinS     := @fmMain.edtSpreadMinS;
  __ctrls[i].spreadMaxS     := @fmMain.edtSpreadMaxS;

  __ctrls[i].netPL          := @fmMain.edtNetPL;

  _CtrlCnt := i;

end;



function __gdDecimalCnt(iSymbol, iSide:integer):integer;
begin
  Result := 0; //TODO StrToInt(__ctrls[iSymbol].gdPos.cells[POS_DECIMAL, iSide]);
end;


function __gdPipSize(iSymbol, iSide:integer):double;
begin
  Result := 0; //TODO StrToFloat(__ctrls[iSymbol].gdPos.cells[POS_PIPSIZE, iSide]);
end;



function __gdOpenPrc(iSymbol, iSide:integer):double;
begin
  Result := StrToFloat(__ctrls[iSymbol].gdPos.cells[POS_OPENPRC, iSide]);
end;


function __gdNowPrc(iSymbol, iSide:integer):double;
begin
  Result := StrToFloatDef(__ctrls[iSymbol].gdPos.cells[POS_NOWPRC, iSide],0);
end;


function __gdExistPosition(iSymbol, iSide:integer):boolean;
begin

  Result := False;

  if strtofloatdef(__ctrls[iSymbol].gdPos.Cells[POS_OPENPRC, iSide],0) > 0 then
    Result := True;

end;

function __gdCurrPLPip(iSymbol, iSide:integer):double;
begin

  Result := strtofloatdef(__ctrls[iSymbol].gdPos.Cells[POS_PL_PIP, iSide],0) ;

end;


function __BaseSpreadPip(iSymbol:integer):double;
begin
  Result := strtofloatdef(__ctrls[iSymbol].baseSpread.Text, 5);
end;

function  __Symbol(isymbol:integer):string;
begin
  Result := __ctrls[iSymbol].symbol.Text;
end;

function __gdIsSending(iSymbol, iSide:integer):boolean;
begin
  Result := TRUE; //TODO (__ctrls[iSymbol].gdPos.Cells[POS_SENDING, iSide] = 'Y');
end;


//function __gdMarkBeingCut(iSymbol, iSide:integer; bMark:boolean):boolean;
//begin
//  if bMark then
//    __ctrls[iSymbol].gdPos.Cells[POS_BEING_CUT, iSide] := 'Y'
//  else
//    __ctrls[iSymbol].gdPos.Cells[POS_BEING_CUT, iSide] := 'N'
//  ;
//
//  Result := true;
//end;


function  __gdPLStatus(iSymbol, iSide:integer):string;
begin
  Result := ''; //TODO __ctrls[iSymbol].gdPos.Cells[POS_PL_STATUS, iSide];
end;

function  __gdLots(iSymbol, iSide:integer):double;
begin
  Result := 0; //TODO strtofloatdef(__ctrls[iSymbol].gdPos.Cells[POS_LOTS, iSide],0);
end;

function  __gdTicket      (iSymbol, iSide:Integer):string;
begin
  Result := __ctrls[iSymbol].gdPos.Cells[POS_TICKET, iSide];
end;


function  __gdBroker      (iSymbol, iSide:Integer):string;
begin
  Result := __ctrls[iSymbol].gdPos.Cells[POS_BROKER, iSide];
end;


function  __gdSpread      (iSymbol, iSide:Integer):double;
begin
  Result := 0; //TODO strtofloatdef(__ctrls[iSymbol].gdPos.Cells[POS_SPREAD, iSide],0);
end;


//TODO
//function  __gdPLStatusTp(iSymbol, iSide:integer):TPL_STATUS;
//var
//  sDesc : string;
//begin
//  sDesc := __ctrls[iSymbol].gdPos.Cells[POS_PL_STATUS, iSide];
//
//  Result := PL_NONE;
//       if sDesc = 'P_1'  then  Result := PL_P1
//  else if sDesc = 'TS_1' then  Result := TS_1
//  else if sDesc = 'TS_2' then  Result := TS_2
//end;
//
//
//TODO
//function __PLStatusDesc(status:TPL_STATUS):string;
//begin
//  Result := 'N';
//       if status = PL_P1    then  Result := 'P_1'
//  else if status = TS_1     then  Result := 'TS_1'
//  else if status = TS_2     then  Result := 'TS_2'
//
//end;

function  __TS_LvlPip(iSymbol:integer; iLvl:integer):double;
var
  sLvl:string;
begin

  if iLvl=1 then
    sLvl := __ctrls[iSymbol].tsLvl_1.Text
  else
    sLvl := __ctrls[iSymbol].tsLvl_2.Text
  ;

  Result := strtofloat(sLvl);

end;

function  __TS_OffSetPip(iSymbol:integer; iLvl:integer):double;
var
  sOffSet:string;
begin

  if iLvl=1 then
    sOffSet := __ctrls[iSymbol].tsOffset_1.Text
  else
    sOffSet := __ctrls[iSymbol].tsOffset_1.Text
  ;

  Result := strtofloat(sOffSet);

end;


//function  __gdTS_BestPrc(iSymbol:integer; iSide:integer):double;
//begin
//
//  Result := strtofloatdef(__ctrls[iSymbol].gdPos.Cells[POS_TS_BEST, iSide],0);
//end;

function  __gdTS_BestPip(iSymbol:integer; iSide:integer):double;
begin

  Result := 0; //TODO strtofloatdef(__ctrls[iSymbol].gdPos.Cells[POS_TS_BEST, iSide],0);
end;

function  __OpenLots(iSymbol:integer):double;
begin

  Result := strtofloatdef(__ctrls[iSymbol].openLots.Text, 0);
end;

function  __Broker(iSymbol, iSide:integer):string;
begin
  if iSide=IDX_BUY then
    Result := __ctrls[iSymbol].broker_buy.Text
  else
    Result := __ctrls[iSymbol].broker_sell.Text
  ;
end;

function  __Key(iSymbol, iSide:integer):string;
begin
  if iSide=IDX_BUY then
    Result := __ctrls[iSymbol].key_buy.Text
  else
    Result := __ctrls[iSymbol].key_sell.Text
  ;
end;


{
  TS_LVL 에 따른 가격 구하기
}
function  __Calc_LvlPrc(iSymbol,iSide:integer; lvl:integer):double;
begin

  Result := __AwayPrc(iSymbol, iSide, __gdOpenPrc(iSymbol, iSide), __TS_LvlPip(iSymbol, lvl), True);

end;

function  __AwayPrc(iSymbol, iSide:integer; dBasePrc, dAwayPip:double; bProfit:boolean=True):double;
begin

  Result := 0;

  if bProfit then
  begin
    if iSide=IDX_BUY then
    BEGIN
      Result := dBasePrc
                +
                ( dAwayPip * __gdPipSize(iSymbol, iSide));
    END;

    if iSide=IDX_SELL then
    BEGIN
      Result := dBasePrc
                -
                ( dAwayPip * __gdPipSize(iSymbol, iSide));
    END;
  end
  else
  begin
    if iSide=IDX_BUY then
    BEGIN
      Result := dBasePrc
                -
                ( dAwayPip * __gdPipSize(iSymbol, iSide));
    END;

    if iSide=IDX_SELL then
    BEGIN
      Result := dBasePrc
                +
                ( dAwayPip * __gdPipSize(iSymbol, iSide));
    END;
  end


end;


end.
