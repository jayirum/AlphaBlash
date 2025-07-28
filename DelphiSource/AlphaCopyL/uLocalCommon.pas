unit uLocalCommon;

interface

uses
  System.Classes, Windows, Messages, SysUtils, Variants, Graphics, Controls, Dialogs
  ,uAlphaProtocol
  ;


const



  GDTERMINAL_PATH = 0;
  GDTERMINAL_STATUS = 1;
  GDTERMINAL_ALIAS  = 2;
  GDTERMINAL_MCTP   = 3;

  MAX_MT4_COUNT     = 5;

  MAX_MASTER_SYMBOL = 10;

  ALL_CLIENTS = 99999;
  ALL_COPIERS = 99998;
  ALL_MASTER  = 99997;


  //////////////////////////////////////////////////////////////////////////////
  ///  INI FILE
  INI_FILE = 'AlphaCopyL.ini';
  SEC_NETWORK = 'NETWORK_INFO';


  //////////////////////////////////////////////////////////////////////////////
  ///  EA Config File
  SEC_LANG          = 'LANGUAGE';
  SEC_SERVER_INFO   = 'ALPHACOPYL_INFO';
  SEC_MT4ORD_RETRY  = 'MT4ORDER_RETRY';
  SEC_TIMOUT        = 'TIMEOUT';
  SEC_MC_TP         = 'MC_TP';
  SEC_SYMBOL_M      = 'SYMBOL_M';
  SEC_SYMBOL_C      = 'SYMBOL_C';
  SEC_CNFG_C        = 'CNFG_C';

  SLTP_SAMEPRC  = 1;  // Master 와 같은 가격
  SLTP_SAMEDIST = 2;  // Master 의 Open-SL 간격과 같은 간격

  VOLTP_MULTI   = 1;  // Multiplier
  VOLTP_FIXED   = 2;

  COPY_TP             ='COPY_TP';
  ORD_MARKET_YN       ='ORD_MARKET_YN';
  ORD_OPEN_YN         ='ORD_OPEN_YN';
  ORD_CLOSE_YN        ='ORD_CLOSE_YN';
  ORD_PENDING_YN      ='ORD_PENDING_YN';
  ORD_LIMIT_YN        ='ORD_LIMIT_YN';
  ORD_STOP_YN         ='ORD_STOP_YN';
  SL_YN               ='SL_YN';
  TP_YN               ='TP_YN';
  SLTP_TYPE           ='SLTP_TYPE';
  COPY_VOL_TP         ='COPY_VOL_TP';
  VOL_MULTIPLIER_VAL  ='VOL_MULTIPLIER_VAL';
  VOL_FIXED_VAL       ='VOL_FIXED_VAL';
  MAXLOT_ONEORDER_YN  ='MAXLOT_ONEORDER_YN';
  MAXLOT_ONEORDER_VAL ='MAXLOT_ONEORDER_VAL';
  MAXLOT_TOTORDER_YN  ='MAXLOT_TOTORDER_YN';
  MAXLOT_TOTORDER_VAL ='MAXLOT_TOTORDER_VAL';
  MAX_SLPG_YN         ='MAX_SLPG_YN';
  MAX_SLPG_VAL        ='MAX_SLPG_VAL';
  NONCOPY_TIMEOUT_YN  ='NONCOPY_TIMEOUT_YN';
  NONCOPY_TIMEOUT_MIN ='NONCOPY_TIMEOUT_MIN';


  function __GetAlias(sPath:string):string;
  function __TerminalCnt():integer;
  function __GetTerminalIdx(sAlias:string):integer;
  function __MCTpDesc(mcTp:string):string;
  function __MCTpFromDesc(desc:string):string;

  function  __FindClientTerminalIdx(sData:string; var iTeminalIdx:Integer; var mcTp:string):boolean;
  function  __Set_LoginErrBuffer(var outStr:string):integer;
  function  __Set_LoginOkBuffer(iTerminal:integer;var outStr:string):integer;
  function  __Set_CfgSymbolBuffer(iTerminal:integer; mcTp:string;var outStr:string; var outCode:string):integer;
  function  __Set_CfgGeneralBuffer(iTerminal:integer; mcTp:string;var outStr:string; var outCode:string):integer;
  function  __Set_ResetMCTpBuffer(iTerminal:integer; mcTp:string;var outStr:string; var outCode:string):integer;
  function  __Set_WrongIdxBuffer(var outStr:string):integer;

  //var


implementation

uses
  fmMainU, ProtoGetU, ProtoSetU
  ;


function __MCTpDesc(mcTp:string):string;
begin
  Result := 'None';

  if UpperCase(mcTp) = 'M' then
    Result := 'Master'
  else if UpperCase(mcTp) = 'C' then
    Result := 'Copier'
  ;

end;

function __MCTpFromDesc(desc:string):string;
begin
  Result := '';

  if UpperCase(desc) = 'MASTER' then
    Result := 'M'
  else if UpperCase(desc) = 'COPIER' then
    Result := 'C'
  ;
end;


function __GetTerminalIdx(sAlias:string):integer;
var
  i : integer;
begin

  Result := -1;
  for i := 0 to __TerminalCnt()-1 do
  begin
    if fmMain.MT4Info[i].Alias = sAlias then
    begin
      Result := i;
      break;
    end;
  end;

  if Result<0 then
     fmMain.AddMsg('Select a MT4 from left list first', false, true);


end;

function __TerminalCnt():integer;
begin
  Result := Length(fmMain.MT4Info);
end;

{
// packet 형태
   - code, broker, mc_tp
}
function __FindClientTerminalIdx(sData:string; var iTeminalIdx:Integer; var mcTp:string):boolean;
var
  sTerminalNm : string;
  i           : integer;
begin
  Result := False;
  sTerminalNm := __GetValue(sData, FDS_TERMINAL_NAME);
  if sTerminalNm='' then
    exit;

  iTeminalIdx := -1;
  for i := 0 to __TerminalCnt()-1 do
  begin
    if pos(fmMain.MT4Info[i].Alias,  sTerminalNm)>0  then
    begin
      iTeminalIdx := i;
      mcTp        := fmMain.MT4Info[i].MCTp;
      Result      := True;
      break;
    end
    else if  pos(sTerminalNm, fmMain.MT4Info[i].Alias)>0  then
    begin
      iTeminalIdx := i;
      mcTp        := fmMain.MT4Info[i].MCTp;
      Result      := True;
      break;
    end;
  end;

end;

{
  client 에게 전송할 메세지 구성
 (CODE=CODE_LOGON;FDS_ERR_YN=Y;FDS_ERR_MSG=EA and AlphaCopyL are unmatched;FDN_ERR_CODE=E_EA_APP_UNMATCHED)

}
function  __Set_LoginErrBuffer(var outStr:string):integer;
var
  protoSet  : TProtoSet;
begin

  protoSet := TProtoSet.create;
  protoSet.Start;
  protoSet.SetVal(FDS_CODE,     CODE_LOGON);
  protoSet.SetVal(FDS_ERR_YN,   'Y' );
  protoSet.SetVal(FDS_ERR_MSG,  'EA and AlphaCopyL are unmatched');
  protoSet.SetVal(FDN_ERR_CODE, E_EA_APP_UNMATCHED);
  Result := protoSet.Complete(outStr);
  FreeAndNil(protoSet);
end;


function  __Set_WrongIdxBuffer(var outStr:string):integer;
var
  protoSet  : TProtoSet;
begin

  protoSet := TProtoSet.create;
  protoSet.Start;
  protoSet.SetVal(FDS_CODE,     CODE_LOGON);
  protoSet.SetVal(FDS_ERR_YN,   'Y' );
  protoSet.SetVal(FDS_ERR_MSG,  'No terminal index in the packet');
  protoSet.SetVal(FDN_ERR_CODE, E_NO_BROKER_IDX);
  Result := protoSet.Complete(outStr);
  FreeAndNil(protoSet);
end;


function  __Set_LoginOkBuffer(iTerminal:integer;var outStr:string):integer;
var
  protoSet  : TProtoSet;
begin

  protoSet := TProtoSet.create;
  protoSet.Start;
  protoSet.SetVal(FDS_CODE,     CODE_LOGON);
  protoSet.SetVal(FDS_ERR_YN,   'N' );
  protoSet.SetVal(FDS_ERR_MSG,  '');
  protoSet.SetVal(FDN_ERR_CODE, E_OK);
  protoSet.SetVal(FDN_TERMINAL_IDX, iTerminal);
  Result := protoSet.Complete(outStr);
  FreeAndNil(protoSet);
end;


// (CODE=CODE_CONFIG_SYMBOL;FDN_TERMINAL_IDX=1;FDS_MASTERCOPIER_TP=M;)
function  __Set_CfgSymbolBuffer(iTerminal:integer; mcTp:string;
                                var outStr:string; var outCode:string):integer;
var
  protoSet  : TProtoSet;
begin

  outCode := CODE_CONFIG_SYMBOL;

  protoSet := TProtoSet.create;
  protoSet.Start;
  protoSet.SetVal(FDS_CODE,     CODE_CONFIG_SYMBOL);
  protoSet.SetVal(FDS_ERR_YN,   'N' );
  protoSet.SetVal(FDS_ERR_MSG,  '');
  protoSet.SetVal(FDN_ERR_CODE, E_OK);

  protoSet.SetVal(FDN_TERMINAL_IDX, iTerminal);
  protoSet.SetVal(FDS_MASTERCOPIER_TP, mcTp);
  Result := protoSet.Complete(outStr);
  FreeAndNil(protoSet);
end;


// (CODE=CODE_CONFIG_GENERAL;FDN_TERMINAL_IDX=1;FDS_MASTERCOPIER_TP=M;)
function  __Set_CfgGeneralBuffer(iTerminal:integer; mcTp:string;
                                var outStr:string; var outCode:string):integer;
var
  protoSet  : TProtoSet;
begin

  outCode := CODE_CONFIG_GENERAL;

  protoSet := TProtoSet.create;
  protoSet.Start;
  protoSet.SetVal(FDS_CODE,     CODE_CONFIG_GENERAL);
  protoSet.SetVal(FDS_ERR_YN,   'N' );
  protoSet.SetVal(FDS_ERR_MSG,  '');
  protoSet.SetVal(FDN_ERR_CODE, E_OK);

  protoSet.SetVal(FDN_TERMINAL_IDX, iTerminal);
  protoSet.SetVal(FDS_MASTERCOPIER_TP, mcTp);
  Result := protoSet.Complete(outStr);
  FreeAndNil(protoSet);
end;

function  __Set_ResetMCTpBuffer(iTerminal:integer; mcTp:string;
                                var outStr:string; var outCode:string):integer;
var
  protoSet  : TProtoSet;
begin

  outCode := CODE_RESET_MCTP;

  protoSet := TProtoSet.create;
  protoSet.Start;
  protoSet.SetVal(FDS_CODE,     CODE_RESET_MCTP);
  protoSet.SetVal(FDS_ERR_YN,   'N' );
  protoSet.SetVal(FDS_ERR_MSG,  '');
  protoSet.SetVal(FDN_ERR_CODE, E_OK);

  protoSet.SetVal(FDN_TERMINAL_IDX, iTerminal);
  protoSet.SetVal(FDS_MASTERCOPIER_TP, mcTp);
  Result := protoSet.Complete(outStr);
  FreeAndNil(protoSet);
end;


(*
C:\Program Files (x86)\IronFX MetaTrader 4
와 같은 형태에서 IronFX 를 꺼낸다.
*)
function __GetAlias(sPath:string):string;
var
  arr1,
  arr2 : TStringList;

  i    : integer;
  sNum : string;
begin
  sNum := '';
  if Pos('5', sPath) > 0 then
    sNum := '_MT5';

  arr1 := TStringList.Create;
  arr1.StrictDelimiter := True;
  arr1.Delimiter := '\';
  arr1.DelimitedText := sPath;

  i := arr1.count-1;

  arr2 := TstringList.Create;
  arr2.StrictDelimiter := True;
  arr2.Delimiter := ' ';
  arr2.DelimitedText := arr1[i];

  Result := arr2[0]+sNum;

  FreeAndNil(arr1);
  FreeAndNil(arr2);
end;

end.
