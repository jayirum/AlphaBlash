
unit uEAConfigFile;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs,
  IniFiles, Forms, TypInfo, WinSock, DBClient, ShellApi, TlHelp32 , CommonVal, uLocalCommon
  ;




  procedure __EACnfg_Save(sAlias, sTerminalPath, sSec, sIdent, sValue: String );
  function  __EACnfg_Get(sTerminalPath, sSec, sIdent: String; sDefault: String=''): String;

  procedure _Create(sAlias, sTerminalPath:string);
//  procedure _SaveData(sFile, sSec, sIdent, sValue: String );
  procedure _SaveInitialConfig(sAlias, sTerminalPath:string);

implementation





(* File ======================================================================*)
procedure __EACnfg_Save(sAlias, sTerminalPath, sSec, sIdent, sValue: String);
var
	INI: TIniFile;
  sFile : string;
begin

  sFile := sTerminalPath+'\Files\AlphaCopyL.ini';
  if FileExists(sFile) = False then
  begin
    _Create(sAlias, sTerminalPath);
    _SaveInitialConfig(sAlias,sTerminalPath);
  end;


	INI := TIniFile.Create(sFile);

  try
    INI.WriteString(sSec, sIdent, sValue);
  finally
    INI.Free;
  end;
end;

function __EACnfg_Get(sTerminalPath, sSec, sIdent: String; sDefault: String=''): String;
var
	INI: TIniFile;
  sFile : string;
begin
  sFile := sTerminalPath+'\Files\AlphaCopyL.ini';
	INI := TIniFile.Create(sFile);

  try
    Result := INI.ReadString(sSec, sIdent, sDefault);
  finally
    INI.Free;
  end;
end;


procedure _Create(sAlias, sTerminalPath:string);
var
	INI: TIniFile;
  sFile : string;
begin

  sFile := sTerminalPath+'\Files\AlphaCopyL.ini';
  INI := TIniFile.Create(sFile);

  try
    INI.WriteString('CREATED', 'BY', 'DEFAULT');
    INI.WriteString('CREATED', 'ALIAS', sAlias);
  finally
    INI.Free;
  end;
end;


procedure _SaveInitialConfig(sAlias, sTerminalPath:string);
begin

  __EACnfg_Save(sAlias, sTerminalPath, SEC_LANG, 'MAIN', 'EN');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SERVER_INFO, 'IP', '127.0.0.1');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SERVER_INFO, 'PORT', '32131');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SERVER_INFO, 'SENDTIMEOUT', '10');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SERVER_INFO, 'RECVTIMEOUT', '10000');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_MT4ORD_RETRY, 'RETRY_CNT', '10');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_MT4ORD_RETRY, 'RETRY_SLEEP', '100');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_TIMOUT, 'SLEEP_MS', '10');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_MC_TP, 'MC_TP', '');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_M, 'CNT', '5');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_M, '1', 'EURUSD');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_M, '2', 'EURGBP');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_M, '3', 'GBPUSD');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_M, '4', 'EURJPY');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_M, '5', 'USDJPY');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_C, 'CNT', '5');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_C, '1', 'EURUSD=EURUSD');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_C, '2', 'EURGBP=EURGBP');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_C, '3', 'GBPUSD=GBPUSD');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_C, '4', 'EURJPY=EURJPY');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_SYMBOL_C, '5', 'USDJPY=USDJPY');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'COPY_TP', 'T');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'ORD_MARKET_YN', 'Y');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'ORD_CLOSE_YN', 'Y');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'ORD_PENDING_YN', 'N');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'ORD_LIMIT_YN', 'N');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'ORD_STOP_YN', 'N');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'SL_YN', 'N');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'TP_YN', 'N');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'SLTP_TYPE', '2'); // 1-Same Price with Master,  2-Same Pip with master

  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'COPY_VOL_TP', 'N');  // 1-Multiplier, 2-fixed, 3-ratio
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'VOL_MULTIPLIER_VAL', 'N');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'VOL_FIXED_VAL', 'N');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'MAXLOT_ONEORDER_YN', 'N');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'MAXLOT_ONEORDER_VAL', '10');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'MAXLOT_TOTORDER_YN', 'N');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'MAXLOT_TOTORDER_VAL', '10');

  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'MAX_SLPG_YN',  'Y');
  __EACnfg_Save(sAlias, sTerminalPath, SEC_CNFG_C, 'MAX_SLPG_VAL', '10');

  end;

end.
