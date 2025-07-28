(*==============================================================================
	StartDate : 2013-09-03 - Charles C
==============================================================================*)
unit StdUtils;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs,
  IniFiles, Forms, TypInfo, WinSock, DBClient, ShellApi,
  //IPWorks
  ipwipdaemon, ipwcore, ipwipport,
  //Client
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdStack, IdException,
  //License
  ActiveX, DB, MemDS, DBAccess, Uni, UniProvider, SQLServerUniProvider,
  //Download
  WinInet, ExtActns, UrlMon,
  // FastStrings
  FastStringFuncs, FastStrings;

(* Date ======================================================================*)
function FirstDate(dtDate: TDateTime): TDateTime;
function LastDate(dtDate: TDateTime): TDateTime;
function Inc_Month(sDate: String; NumberOfMonths: Integer=1): String;
function Inc_MonthMM(sDate: String; NumberOfMonths: Integer=1): String;
function NowDateTime(bDelimiter: Boolean=False): String;
function NowDateMSecTime(bDelimiter: Boolean=False): String;
function NowDate(bDelimiter: Boolean=False): String;
function NowTime(bDelimiter: Boolean=False): String;
function NowMSecTime(bDelimiter: Boolean=False): String;
function YYMM(dtDate: TDateTime): String;
function Inc_YYMM(dtDate: TDateTime; NumberOfMonths: Integer=1): String;

(* String ====================================================================*)
function  StrReplace(sSor, sOldStr, sNewStr: String; bAll: Boolean=True): String;
function  Get_Delimiter(const sSor: String; iCnt: Word; sDelimiter: String = '|'): String;
procedure Get_Parsing(const sSor: String; var vList: TStringList; sDelimiter: String = '|');
procedure Get_H5Parsing(const sSor: String; var vList: TStringList; sDefaultStartName:String; sDelimiter: String = #9);
function  Get_FirstParsing(const sSor: String; sDelimiter: String = #9): String;
procedure TextToStringList(const sSor: String; var vList: TStringList; sDelimiter: Char = #9);
function  AddStr(s: String; AddStr: String=#9): String;
function  RecordToStr(const Rec; iSize: Integer): String;
procedure StrToRecord(sStr: String; var Rec; iSize: Integer);
function  HeadUpdateToStr(const Rec; sData: String): String;
procedure StrToHeadRecord(sStr: String; var recHeader);
procedure StrToThinHeadRecord(sStr: String; var recHeader);
procedure StrToNLHeadRecord(sStr: String; var recHeader);
procedure StrToArr(sStr: String; var arr: array of Char);
procedure IntToArr(Value: Integer; var arr: array of Char);
procedure DblToArr(Value: Double; var arr: array of Char);
function  ArrToStr(arr: array of Char): String;
function  ArrToInt(arr: array of Char): Integer;
function  ArrToDbl(arr: array of Char): Double;
function  NumToStr(Value: LongInt; iLength: Word=3): String;
function  ABSStr(sFloat: String): Double;
function  FormatDotCnt(iCnt: Integer): String;
function  Get_DateDisp(sDate: String): String;
function  TextToDate(sDate: String): TDateTime;
function  Get_TimeDisp(sTM: String): String;
function  TextToFloat(sText: String): Double;

(* Encrypt ===================================================================*)
function GT_Encrypt(const S: String; Key: Word=902): String;
function GT_Decrypt(const S: String; Key: Word=902): String;

(* MessageBox ================================================================*)
procedure MsgInfo(aMsg: String; sTitle: String='확인');
procedure MsgError(aMsg: String; sTitle: String='오류');
procedure MsgWarning(aMsg: String; sTitle: String='경고');
function MsgYesNo(aMsg: String; sTitle: String='확인'): Boolean;
function MsgOkCancel(aMsg: String; sTitle: String='확인'): Boolean;
function MsgYesNoCancel(aMsg: String; sTitle: String='확인'): Integer;

(* IPWorks ===================================================================*)
procedure DataToClient(ServerSocket: TipwIPDaemon; iConnectID: Integer; SendData: String);
procedure DataToServer(ClientSocket: TipwIPPort; SendData: String);

(* File ======================================================================*)
procedure Set_INIFile(aINIFile, sSec, sIdent, sValue: String);
function  Get_INIFile(aINIFile, sSec, sIdent: String; sDefault: String=''): String;
procedure Set_CFGFile(sSec, sIdent, sValue: String; Encrypt: Boolean=False; sFileName: String='');
function  Get_CFGFile(sSec, sIdent: String; sDefault: String=''; Encrypt: Boolean=False; sFileName: String=''): String;
function  Get_CFGFileName():string;

procedure Set_Log(aLog: String; aType: String='D'; sPreFix: String='');
procedure Set_Err(aLog: String; sFile: String='');

(* DataSet ===================================================================*)
function StrToCode(s: String): String;
function CodeToStr(s: String): String;
function DataSetToStr(DataSet: TDataSet): String;
function StrToDataSet(s: String; DataSet: TClientDataSet; bFirst: Boolean=True): Boolean;
function InsertSQL(DataSet: TDataSet; sTable: String): String;
function UpdateSQL(DataSet: TDataSet; sTable, sKeyField: String; sWhere: String=''): String;
function DeleteSQL(DataSet: TDataSet; sTable, sKeyField: String; sWhere: String=''): String;

function ResultToStr(sResult: String; sType: String='RESULT'): String;
function StrToResult(sResult: String; sType: String='RESULT'): String;

function Uni_Open(DataSet: TUniQuery; sSQL: String; iFetchRows: Integer=50): String;
function Client_Open(DataSet: TClientDataSet; sSQL: String): Boolean;

(* IP ========================================================================*)
function GetLocalIP : String;
function GetMacAddress: String;

(* Resource ==================================================================*)
function Get_ResInfo(InfoName: String): String;

(* DownLoad ==================================================================*)
function InfoDownloadToFile(URL, FileName: String): Boolean;

(* Etc =======================================================================*)
procedure ComponetEnabled(Component: TComponent; Value: Variant; sProp: String='Enabled');
procedure ChangeProp(AGroup:TWinControl; name:string; value:variant);
procedure Delay(msecs: integer);

function Get_Lic: Boolean;
function Get_LicDate: String;

const
	__iHeaderSize     = 42;                // HeaderSize
	__iThinHeaderSize = 15;                // Thin HeaderSize (REAL, LOG)
	__iNLHeaderSize   = __iHeaderSize + 1; // 통보 HeaderSize

	__EOL = #10;                           // Socekt End Of Line

	__iFetchCount = 20;                    // Client Fetch Count

var
	__DebugMode: Boolean=False;
  __IdSocket: TIdTCPClient;

  __HTS_LOGINID: String;

  LIC_DATE: String='';                                                          // Lic Date
  LIC_KEY: String='';                                                           // Lic Key
  LIC_SIGN: Boolean=True;                                                       // Lic Sign

  GT_NOTICE_SHOW,                                                               // 통보여부
  GT_NOTICE_AUTOCLOSE,                                                          // 자동닫기
  GT_NOTICE_WAV: Boolean;                                                       // 소리알림 여부
  GT_NOTICE_CLOSETIME: Integer;                                                 // 자동닫기시간
  GT_NOTICE_WAVFILE: String;                                                    // 소리파일

  HTS_ID,
  HTS_PWD,
  HTS_ACNT_TP,
  HTS_MACADDRESS: String;

  __UPD_CORP_NM: String;                                                        // 업데이트파일/LOGIN 하단 업체명
  __UPD_CORP_MSG: String;                                                       // 업데이트파일/LOGIN 하단 메세지
  __HTS_VER: String;                                                            // HTS Version
  FHTS_VER : String;
  FHTS_ID : String;
  FHTS_PASSWORD : String;

implementation

const
	C1 = 13092;
	C2 = 75222;
	HexaChar : array [0..15] of Char = ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                      'A', 'B', 'C', 'D', 'E', 'F');

var
	_GT_Sign: Boolean=True;
  CritSect: TRTLCriticalSection;

  _ErrorCnt: Integer=0;

(* Date ======================================================================*)
function FirstDate(dtDate: TDateTime): TDateTime;
var
	iYear, iMonth, iDay: Word;
begin
	DecodeDate(dtDate, iYear, iMonth, iDay);

  Result := EncodeDate(iYear, iMonth, 1);
end;

function LastDate(dtDate: TDateTime): TDateTime;
var
	iYear, iMonth, iDay: Word;
begin
	DecodeDate(dtDate, iYear, iMonth, iDay);

	Inc(iMonth);
  if iMonth > 12 then
  begin
    Inc(iYear);
    iMonth := 1;
  end;

  Result := EncodeDate(iYear, iMonth, 1) - 1;
end;

function Inc_Month(sDate: String; NumberOfMonths: Integer=1): String;
var
  dtDate: TDateTime;
begin
	Result := sDate;

  dtDate := EncodeDate(StrToInt(Copy(sDate,1,4)),
                       StrToInt(Copy(sDate,5,2)),
                       StrToInt(Copy(sDate,7,2))
                       );

  Result := FormatDateTime('YYYYMMDD', IncMonth(dtDate, NumberOfMonths));
end;

function Inc_MonthMM(sDate: String; NumberOfMonths: Integer=1): String;
var
  dtDate: TDateTime;
begin
	Result := sDate;

  dtDate := EncodeDate(StrToInt(Copy(sDate,1,4)),
                       StrToInt(Copy(sDate,5,2)),
                       StrToInt(Copy(sDate,7,2))
                       );

  Result := FormatDateTime('MM', IncMonth(dtDate, NumberOfMonths));
end;

function NowDateTime(bDelimiter: Boolean=False): String;
begin
	Result := '';

  if bDelimiter then
  	Result := FormatDateTime('YYYY-MM-DD HH:NN:SS', Now)
  else
	  Result := FormatDateTime('YYYYMMDDHHNNSS', Now);
end;

function NowDateMSecTime(bDelimiter: Boolean=False): String;
begin
	Result := '';

  if bDelimiter then
  	Result := FormatDateTime('YYYY-MM-DD HH:NN:SS:ZZZ', Now)
  else
	  Result := FormatDateTime('YYYYMMDDHHNNSSZZZ', Now);

{  DecodeDate(Now, iYear, iMonth, iDay);
  DecodeTime(Now, iHour, iMin, iSec, iMSec);

  Result := Format('%d%0.2d%0.2d%0.2d%0.2d%0.2d%0.3d', [iYear, iMonth, iDay, iHour, iMin, iSec, iMSec]);
  Result := StrReplace(Result, ' ', '0');}
end;

function NowDate(bDelimiter: Boolean=False): String;
begin
	Result := '';

  if bDelimiter then
  	Result := FormatDateTime('YYYY-MM-DD', Now)
  else
	  Result := FormatDateTime('YYYYMMDD', Now);
end;

function NowTime(bDelimiter: Boolean=False): String;
begin
	Result := '';

  if bDelimiter then
    Result := FormatDateTime('HH:NN:SS', Now)
  else
	  Result := FormatDateTime('HHNNSS', Now);
end;

function NowMSecTime(bDelimiter: Boolean=False): String;
begin
	Result := '';

	if bDelimiter then
  	Result := FormatDateTime('HH:NN:SS:ZZZ', Now)
  else
	  Result := FormatDateTime('HHNNSSZZZ', Now);
end;

function YYMM(dtDate: TDateTime): String;
begin
  Result := FormatDateTime('YYMM', dtDate);
end;

function Inc_YYMM(dtDate: TDateTime; NumberOfMonths: Integer=1): String;
begin
  Result := FormatDateTime('YYMM', IncMonth(dtDate, NumberOfMonths));
end;

(* String ====================================================================*)
function StrReplace(sSor, sOldStr, sNewStr: String; bAll: Boolean=True): String;
begin
	if bAll then
		Result := FastAnsiReplace(sSor, sOldStr, sNewStr, [rfReplaceAll, rfIgnoreCase])
  else
   	Result := FastAnsiReplace(sSor, sOldStr, sNewStr, [rfIgnoreCase]);
end;

function Get_Delimiter(const sSor: String; iCnt: Word; sDelimiter: String = '|'): String;
var
	i, iPos: Integer;
	sValue: String;
begin
	sValue := '';
	iPos := 0;

	for i := 1 to Length(sSor) do
	begin
		if sSor[i] = sDelimiter[1] then
		begin
			inc(iPos);

			if iPos = iCnt then
			begin
				Result := Trim(sValue);
				Exit;
			end;

			sValue := '';
		end
		else sValue := sValue + sSor[i];
	end;

	inc(iPos);
	if iPos = iCnt then Result := Trim(sValue)
	else Result := '';
end;

procedure Get_Parsing(const sSor: String; var vList: TStringList; sDelimiter: String = '|');
var
	i: Integer;
  sValue: String;
begin
	vList.Clear;
	sValue := '';

	for i := 1 to Length(sSor) do
	begin
  	if sSor[i] = sDelimiter then
    begin
    	vList.Add(sValue);
      sValue := '';
    end
    else sValue := sValue + sSor[i];
  end;

  vList.Add(sValue);
end;

procedure Get_H5Parsing(const sSor: String; var vList: TStringList; sDefaultStartName:String; sDelimiter: String = #9);
var
	i, iPos: Integer;
  sName, sValue: String;
begin
	if vList = Nil then Exit;

	vList.Clear;
	sValue := '';

  iPos := 0;
  if sDefaultStartName <> '' then
  begin
	  sName  := sDefaultStartName;
		iPos := 1;
  end;

	for i := 1 to Length(sSor) do
	begin
  	if (sSor[i] = sDelimiter) or (sSor[i] = #13) or (sSor[i] = #10) then
    begin
      if iPos = 0 then
      begin
      	sName := sValue;
  	    sValue := '';
        Inc(iPos);
      end
      else
      begin
      	vList.Values[sName] := sValue;

        sName := '';
  	    sValue := '';
        iPos := 0;
      end;
    end
    else
    begin
      if (sSor[i] >= #32) and (sSor[i] <> ' ') then sValue := sValue + sSor[i];
    end;
  end;
end;

function Get_FirstParsing(const sSor: String; sDelimiter: String = #9): String;
var
	i: Integer;
  sValue: String;
begin
	Result := '';
  sValue := '';

	for i := 1 to Length(sSor) do
	begin
  	if sSor[i] = sDelimiter then Break
    else if (sSor[i] >= #32) and (sSor[i] <> ' ') then sValue := sValue + sSor[i];
  end;

  Result := sValue;
end;

procedure TextToStringList(const sSor: String; var vList: TStringList; sDelimiter: Char = #9);
begin
	if vList = Nil then Exit;

  with vList do
  begin
  	Clear;

		Delimiter := sDelimiter;
  	StrictDelimiter := True;
  	DelimitedText := sSor;
  end;
end;

function AddStr(s: String; AddStr: String=#9): String;
begin
	Result := s;
  Result := Format('%s'+AddStr, [s]);
end;

function RecordToStr(const Rec; iSize: Integer): String;
begin
  Result := '';
  if Not _GT_Sign then Exit;

  SetLength(Result, iSize);
  Move(Rec, Result[1], iSize);
end;

procedure StrToRecord(sStr: String; var Rec; iSize: Integer);
begin
  if Not _GT_Sign then Exit;

  Move(sStr[1], Rec, iSize);
end;

function HeadUpdateToStr(const Rec; sData: String): String;
var
  sHead: String;
begin
	Result := '';

	sHead := RecordToStr(Rec, __iHeaderSize);

  Result := sHead + Copy(sData, __iHeaderSize+1, Length(sData)-__iHeaderSize);
end;

procedure StrToHeadRecord(sStr: String; var recHeader);
begin
	StrToRecord(sStr, recHeader, __iHeaderSize);
end;

procedure StrToThinHeadRecord(sStr: String; var recHeader);
begin
	StrToRecord(sStr, recHeader, __iThinHeaderSize);
end;

procedure StrToNLHeadRecord(sStr: String; var recHeader);
begin
	StrToRecord(sStr, recHeader, __iNLHeaderSize);
end;

procedure StrToArr(sStr: String; var arr: array of Char);
var
  i: Integer;
begin
	if Not _GT_Sign then Exit;

  for i := 0 to SizeOf(arr) - 1 do
  begin
    if i < Length(sStr) then
      arr[i] := sStr[i+1]
    else
      arr[i] := ' ';
  end;
end;

procedure IntToArr(Value: Integer; var arr: array of Char);
var
  s: String;
  i, iPos: Integer;
begin
	if Not _GT_Sign then Exit;

  iPos := 0;
  s := IntToStr(Value);

  for i := 0 to SizeOf(arr)-1 do
  begin
    if i >= (SizeOf(arr)-Length(s)) then
    begin
      Inc(iPos);
      arr[i] := s[iPos];
    end
    else
      arr[i] := ' ';
  end;
end;

procedure DblToArr(Value: Double; var arr: array of Char);
var
  s: String;
  i, iPos: Integer;
begin
	if Not _GT_Sign then Exit;

  iPos := 0;
  s := FloatToStr(Value);

  for i := 0 to SizeOf(arr)-1 do
  begin
    if i >= (SizeOf(arr)-Length(s)) then
    begin
      Inc(iPos);
      arr[i] := s[iPos];
    end
    else
      arr[i] := ' ';
  end;
end;

function ArrToStr(arr: array of Char): String;
var
  i: Integer;
begin
  Result := '';
	if Not _GT_Sign then Exit;

  for i := 0 to SizeOf(arr) - 1 do
  begin
    Result := Result + arr[i];
  end;

  Result := Trim(Result);
end;

function ArrToInt(arr: array of Char): Integer;
var
  s: String;
  i: Integer;
begin
	if Not _GT_Sign then Exit;
  s := '';

  for i := 0 to SizeOf(arr)-1 do
  begin
    if arr[i] in ['0'..'9', '+', '-', '.' ] then
      s := s + arr[i];
  end;

  Result := StrToIntDef(s, 0);
end;

function ArrToDbl(arr: array of Char): Double;
var
  s: String;
  i: Integer;
begin
	if Not _GT_Sign then Exit;
  s := '';

  for i := 0 to SizeOf(arr)-1 do
  begin
    if arr[i] in ['0'..'9', '+', '-', '.' ] then
      s := s + arr[i];
  end;

  Result := StrToFloatDef(s, 0);
end;

function NumToStr(Value: LongInt; iLength: Word=3): String;
var
  sTmp, sResult: String;
  i, iLen: Word;
begin
  sResult := '';
  sTmp := IntToStr(Value);
  iLen := Length(sTmp);

  for i := 1 to iLength do
  begin
	  if (iLen >= i) then sResult := sTmp[iLen-i+1] + sResult
  	else sResult := '0' + sResult;
  end;

  Result := sResult;
end;

function ABSStr(sFloat: String): Double;
begin
	Result := ABS(StrToFloatDef(sFloat, 0));
end;

function FormatDotCnt(iCnt: Integer): String;
var
	i: Integer;
  sDot: String;
begin
  Result := '###0';
  sDot   := '';

	for i := 1 to iCnt do
    sDot := sDot + '0';

  if iCnt > 0 then Result := '###0.' + sDot;
end;

function Get_DateDisp(sDate: String): String;
begin
  Result := Copy(sDate, 1, 4) + '-' + Copy(sDate, 5, 2) + '-' + Copy(sDate, 7, 2);
end;

function TextToDate(sDate: String): TDateTime;
begin
  Result := StrToDateTime(Get_DateDisp(sDate));
end;

function Get_TimeDisp(sTM: String): String;
begin
  Result := Copy(sTM, 1, 2) + ':' + Copy(sTM, 3, 2) + ':' + Copy(sTM, 5, 2);
end;

function  TextToFloat(sText: String): Double;
var
	i: Integer;
  sValue: String;
begin
  sValue := '';

  Result := StrToIntDef(sText, 0);

  if (Result = 0) and (Length(sText) > 0) then
  begin
    for i := 1 to Length(sText) do
		begin
      if sText[i] in ['0'..'9', '.', '-'] then sValue := sValue + sText[i];
    end;

    Result := StrToFloatDef(sValue, 0);
  end;
end;

(* Encrypt ===================================================================*)
// Byte -> Hexadecimal
function ValueToHex(const S: String): String;
var
	i: Integer;
begin
	SetLength(Result, Length(S)*2); // 문자열 크기를 설정

	for i := 0 to Length(S)-1 do
  begin
    Result[(i*2)+1] := HexaChar[Integer(S[i+1]) shr 4];
    Result[(i*2)+2] := HexaChar[Integer(S[i+1]) and $0f];
  end;
end;

// Hexadecimal -> Byte
function HexToValue(const S: String): String;
var
	i: Integer;
begin
  SetLength(Result, Length(S) div 2);

  for i := 0 to (Length(S) div 2) - 1 do
  begin
	  Result[i+1] := Char(StrToInt('$'+Copy(S,(i*2)+1, 2)));
  end;
end;

function GT_Encrypt(const S: String; Key: Word=902): String;
var
  i: byte;
  FirstResult: String;
begin
  SetLength(FirstResult, Length(S)); // 문자열의 크기를 설정

  for i := 1 to Length(S) do
  begin
    FirstResult[i] := Char(byte(S[i]) xor (Key shr 8));
    Key := (byte(FirstResult[i]) + Key) * C1 + C2;
  end;

	Result := ValueToHex(FirstResult);
end;

function GT_Decrypt(const S: String; Key: Word=902): String;
var
	i: byte;
	FirstResult: String;
begin
	if Length(S) = 0 then Exit;
  
  FirstResult := HexToValue(S);
  SetLength( Result, Length(FirstResult) );

  for i := 1 to Length(FirstResult) do
  begin
    Result[i] := Char(byte(FirstResult[i]) xor (Key shr 8));
    Key := (byte(FirstResult[i]) + Key) * C1 + C2;
  end;
end;

(* MessageBox ================================================================*)
procedure MsgInfo(aMsg: String; sTitle: String='확인');
begin
  aMsg := StrReplace(aMsg, '\n', #13#10);
  Application.MessageBox(PChar(aMsg), PChar(sTitle), MB_OK+MB_ICONINFORMATION);
end;

procedure MsgError(aMsg: String; sTitle: String='오류');
begin
  aMsg := StrReplace(aMsg, '\n', #13#10);
  Application.MessageBox(PChar(aMsg), PChar(sTitle), MB_OK+MB_ICONERROR);
end;

procedure MsgWarning(aMsg: String; sTitle: String='경고');
begin
  aMsg := StrReplace(aMsg, '\n', #13#10);
  Application.MessageBox(PChar(aMsg), PChar(sTitle), MB_OK+MB_ICONWARNING);
end;

function MsgYesNo(aMsg: String; sTitle: String='확인'): Boolean;
begin
  aMsg := StrReplace(aMsg, '\n', #13#10);
  Result := Application.MessageBox(PChar(aMsg), PChar(sTitle), MB_YESNO+MB_ICONQUESTION+MB_DEFBUTTON2)=IDYES;
end;

function MsgOkCancel(aMsg: String; sTitle: String='확인'): Boolean;
begin
  aMsg := StrReplace(aMsg, '\n', #13#10);
  Result := Application.MessageBox(PChar(aMsg), PChar(sTitle), MB_OKCANCEL+MB_ICONQUESTION+MB_DEFBUTTON2)=IDOK;
end;

function MsgYesNoCancel(aMsg: String; sTitle: String='확인'): Integer;
begin
  aMsg := StrReplace(aMsg, '\n', #13#10);
  Result := Application.MessageBox(PChar(aMsg), PChar(sTitle), MB_YESNOCANCEL+MB_ICONQUESTION+MB_DEFBUTTON3);
  case Result of
    IDYES : Result := 1;
    IDNO  : Result := 2;
    IDCANCEL : Result := 3;
  end;
end;

(* IPWorks ===================================================================*)
procedure DataToClient(ServerSocket: TipwIPDaemon; iConnectID: Integer; SendData: String);
begin
  with ServerSocket do
  try
    Tag := 0;
    DataToSend[iConnectID] := SendData;
  except
  	on E: EIPWorks do
    begin
      if e.Code = 10035 then
      begin
        while Tag = 0 do DoEvents;
        DataToSend[iConnectID] := SendData;
      end;

      if e.Code = 301 then
	      Set_Log(Format('DataToClient Error <%d> = %d', [iConnectID, e.Code]), 'E');
    end;
  end;
end;

procedure DataToServer(ClientSocket: TipwIPPort; SendData: String);
begin
  with ClientSocket do
  try
    Tag := 0;
    DataToSend := SendData;
  except
  	on E: EIPWorks do
    begin
      Set_Log(Format('DataToServer Error = %d', [e.Code]), 'E');
      
      if e.Code = 10035 then
      begin
        while Tag = 0 do DoEvents;
        DataToSend := SendData;
      end;
    end;
  end;
end;

(* File ======================================================================*)
procedure Set_INIFile(aINIFile, sSec, sIdent, sValue: String);
var
	INI: TIniFile;
begin
	if Not DirectoryExists(aINIFile) then ForceDirectories(ExtractFilePath(aINIFile));

	INI := TIniFile.Create(aINIFile);

  try
    INI.WriteString(sSec, sIdent, sValue);
  finally
    INI.Free;
  end;
end;

function Get_INIFile(aINIFile, sSec, sIdent: String; sDefault: String=''): String;
var
	INI: TIniFile;
begin
	INI := TIniFile.Create(aINIFile);

  try
    Result := INI.ReadString(sSec, sIdent, sDefault);
  finally
    INI.Free;
  end;
end;

procedure Set_CFGFile(sSec, sIdent, sValue: String; Encrypt: Boolean=False; sFileName: String='');
var
	INI: TIniFile;
  sFile: String;
begin
	if sFileName = '' then sFile := ChangeFileExt(ParamStr(0), '.INI')
  else sFile := sFileName;

  INI := TIniFile.Create(sFile);

  if Encrypt then
  begin
  	sSec   := GT_Encrypt(sSec);
  	sIdent := GT_Encrypt(sIdent);
  	sValue := GT_Encrypt(sValue);
  end;

  try
    INI.WriteString(sSec, sIdent, sValue);
  finally
    INI.Free;
  end;
end;


function  Get_CFGFileName():string;
begin
  Result := ChangeFileExt(ParamStr(0), '.INI');
end;


function  Get_CFGFile(sSec, sIdent: String; sDefault: String=''; Encrypt: Boolean=False; sFileName: String=''): String;
var
	INI: TIniFile;
  sFile: String;
begin
	if sFileName = '' then sFile := ChangeFileExt(ParamStr(0), '.INI')
  else sFile := sFileName;

  INI := TIniFile.Create(sFile);

  try
	  if Not Encrypt then
  	  Result := INI.ReadString(sSec, sIdent, sDefault)
    else
  	  Result := GT_Decrypt(INI.ReadString(GT_Encrypt(sSec), GT_Encrypt(sIdent), GT_Encrypt(sDefault)));
  finally
    INI.Free;
  end;
end;

procedure Set_Log(aLog: String; aType: String='D'; sPreFix: String='');
var
	tLog: TextFile;
  sDir, sFileName, sFile: String;
begin
	if (__DebugMode = False) and (UpperCase(aType) = 'D') then Exit;

  sDir := ExtractFilePath(ParamStr(0)) + 'LOG';
  sFileName := ExtractFileName(ParamStr(0));

  if Not DirectoryExists(sDir) then ForceDirectories(sDir);

	sFile := Format('%s\%s_%s%s.LOG',  [sDir,
                                      Copy(sFileName, 1, Length(sFileName)-4),
                                      sPreFix,
                                      NowDate]);

{$I-}
  AssignFile(tLog, sFile);

  EnterCriticalSection(CritSect);

  try
    if FileExists(sFile) then Append(tLog)
    else Rewrite(tLog);

    Writeln(tLog, NowMSecTime(True), Format('[%s] %s', [aType, aLog]));
  finally
    CloseFile(tLog);
    LeaveCriticalSection(CritSect);
  end;
{$I+}
end;

procedure Set_Err(aLog: String; sFile: String='');
var
	tLog: TextFile;
begin
{$I-}
  AssignFile(tLog, sFile);

  EnterCriticalSection(CritSect);

  try
    if FileExists(sFile) then Append(tLog)
    else Rewrite(tLog);

    Writeln(tLog, NowMSecTime(True), aLog);
  finally
    CloseFile(tLog);
    LeaveCriticalSection(CritSect);
  end;
{$I+}
end;

(* DataSet ===================================================================*)
function FieldsToStr(DataSet: TDataSet): String;
var
  i: Integer;
begin
  Result := '<FIELDS>;';

  with DataSet do
  begin
    for i := 0 to Fields.Count - 1 do
    begin
      // 필드명,타입,크기,디스플레이명
      Result := Result + Format('%s,%s,%d,%s,%s;',
        [
          Fields[i].FieldName,
          FieldTypeNames[Fields[i].DataType],
          Fields[i].Size,
          BoolToStr(Fields[i].Required),
          Fields[i].DisplayName
        ]);
    end;
  end;

  Result := Result + '</FIELDS>;';
end;

function StrToCode(s: String): String;
var
  i: Integer;
begin
  Result := '';

  for i := 1 to Length(s) do
  begin
    if s[i] = '\' then Result := Result + '\\'
    else
    if s[i] = ';' then Result := Result + '\s'
    else
    if s[i] = #13 then Result := Result + '\r'
    else
    if s[i] = #10 then Result := Result + '\n'
    else
    if s[i] = ',' then Result := Result + '\c'
    else Result := Result + s[i];
  end;
end;

function CodeToStr(s: String): String;
var
  i: Integer;
  b: Boolean;
begin
  Result := '';
  b := False;
  for i := 1 to Length(s) do
  begin
    if (b = False) and (s[i] = '\') then
    begin
      b := True;
      Continue;
    end;

    if b then
    begin
      if s[i] = 's' then Result := Result + ';'
      else
      if s[i] = 'r' then Result := Result + #13
      else
      if s[i] = 'n' then Result := Result + #10
      else
      if s[i] = 'c' then Result := Result + ','
      else
      if s[i] = '\' then Result := Result + '\'
      else Result := Result + s[i];

      b := False;
    end
    else
      Result := Result + s[i];
  end;
end;

function DataToStr(DataSet: TDataSet): String;
var
  i: Integer;
begin
  Result := '<DATA>;';

  with DataSet do
  begin
    DisableControls;

    First;
    while not EOF do
    begin
      for i := 0 to Fields.Count - 1 do
      begin
        Result := Result +  StrToCode(Fields[i].AsString);
        if i = (Fields.Count -1) then Result := Result + ';'
        else Result := Result + ',';
      end;
      Next;
    end;

    EnableControls;
  end;
  Result := Result + '</DATA>;';
end;

function DataSetToStr(DataSet: TDataSet): String;
begin
	if Not _GT_Sign then Exit;
  Result := FieldsToStr(DataSet) + DataToStr(DataSet);
end;

function StrToDataSet(s: String; DataSet: TClientDataSet; bFirst: Boolean=True): Boolean;
var
  lst, v: TStringList;
  i, j: Integer;
  bField, bData: Boolean;
begin
	if Not _GT_Sign then Exit;

  bField := False;
  bData  := False;

  if bFirst then
  begin
    DataSet.Close;
    DataSet.FieldDefs.Clear;
  end;

  lst := TStringList.Create;
  lst.StrictDelimiter := True;
  lst.Delimiter := ';';

  v   := TStringList.Create;
  v.StrictDelimiter := True;
  v.Delimiter := ',';

  try
    DataSet.DisableControls;

    lst.DelimitedText := s;

    for i := 0 to lst.Count - 1 do
    begin
      if lst[i] = '<FIELDS>' then
      begin
        bField := True;
        Continue;
      end;
      if lst[i] = '</FIELDS>' then
      begin
        bField := False;

        if bFirst then DataSet.CreateDataSet;
        DataSet.Open;

        Continue;
      end;

      if lst[i] = '<DATA>' then
      begin
        bData := True;
        Continue;
      end;
      if lst[i] = '</DATA>' then
      begin
        bData := False;

        if not DataSet.Active then Exit;

        Continue;
      end;

      if (bField) and (bFirst) then
      begin
        v.DelimitedText := lst[i];

        DataSet.FieldDefs.Add(
          v[0],
          TFieldType(GetEnumValue(TypeInfo(TFieldType),'ft'+v[1])),
          StrToIntDef(v[2], 0),
          StrToBool(v[3])
        );
      end;

      if bData then
      begin
        DataSet.Append;

        v.DelimitedText := lst[i];

        for j := 0 to DataSet.Fields.Count - 1 do
        begin
          DataSet.Fields[j].AsString := CodeToStr( v[j] );
        end;
        DataSet.Post;
      end;
    end;
  finally
    FreeAndNil(lst);
    FreeAndNil(v);
    DataSet.EnableControls;
  end;
end;

function InsertSQL(DataSet: TDataSet; sTable: String): String;
var
	i: Integer;
  s, v: String;
begin
  Result := '';
	if Not _GT_Sign then Exit;

  s := '';
  v := '';

  //INSERT INTO %s (%s) VALUES (%s)
  with DataSet do
  begin
    for i := 0 to Fields.Count - 1 do
    begin
      s := s + Fields[i].FieldName + ',';

    	if Fields[i].DataType in [ftInteger, ftFloat, ftCurrency] then
	      v := v + Fields[i].AsString + ','
      else
      	v := v + QuotedStr(Fields[i].AsString) + ',';
    end;

    s := Copy(s, 1, Length(s)-1);
    v := Copy(v, 1, Length(v)-1);

    Result := Format('INSERT INTO %s (%s) VALUES (%s)', [sTable, s, v]);
  end;
end;

function UpdateSQL(DataSet: TDataSet; sTable, sKeyField: String; sWhere: String=''): String;
var
	i: Integer;
  s: String;
begin
  Result := '';
	if Not _GT_Sign then Exit;

  s := '';
  //UPDATE sTable SET (Field=sValue,...) WHERE KeyField = sValue
  with DataSet do
  begin
    for i := 0 to Fields.Count - 1 do
    begin
    	if Fields[i].DataType in [ftInteger, ftFloat, ftCurrency] then
	      s := s + Fields[i].FieldName + '=' + Fields[i].AsString + ','
      else
      	s := s + Fields[i].FieldName + '=' + QuotedStr(Fields[i].AsString) + ',';
    end;

    s := Copy(s, 1, Length(s)-1);
    
    if sWhere= '' then sWhere := Format('%s=%s', [sKeyField, QuotedStr(FieldByName(sKeyField).AsString)]);

    Result := Format('UPDATE %s SET %s WHERE %s', [sTable, s, sWhere]);
  end;
end;

function DeleteSQL(DataSet: TDataSet; sTable, sKeyField: String; sWhere: String=''): String;
begin
	Result := '';
	if Not _GT_Sign then Exit;

  //DELETE FROM TABLE WHERE sKeyField = sValue
  with DataSet do
  begin
    if sWhere = '' then sWhere := Format('%s=%s', [sKeyField, QuotedStr(FieldByName(sKeyField).AsString)]);
    
    Result := Format('DELETE FROM %s WHERE %s', [sTable, sWhere]);
  end;
end;

//==============================================================================
{
	try
    UniSQL.Execute;
    ResultToStr(UniSQL.RowsAffected);
  except
    on E:Exception do
      ResultToStr(E.Message, 'ERROR')
  end;
}
function ResultToStr(sResult: String; sType: String='RESULT'): String;
begin
  Result := Format('<%s>;%s;</%s>;', [UpperCase(sType), CodeToStr(sResult), UpperCase(sType)]);
end;

function RangeToStr(sResult, sType: String): String;
var
	sBegin, sEnd: String;
  iIdx, iCnt: Integer;
begin
  sBegin := Format('<%s>;',  [sType]);
  sEnd   := Format('</%s>;', [sType]);

  if (POS(sBegin, sResult) <> 0) and (POS(sEnd, sResult) <> 0) then
  begin
    iIdx := POS(sBegin, sResult) + Length(sBegin);
    iCnt := POS(sEnd, sResult) - (POS(sBegin, sResult) + Length(sEnd));

    Result := StrToCode(Copy(sResult, iIdx, iCnt));
  end;
end;

function StrToResult(sResult: String; sType: String='RESULT'): String;
begin
  Result := '';

  Result := RangeToStr(sResult, sType);
end;

function Uni_Open(DataSet: TUniQuery; sSQL: String; iFetchRows: Integer=50): String;
begin
  Result := '';

  with DataSet do
  try
    Close;
    FetchRows := iFetchRows;

    SQL.Text := sSQL;

    if UpperCase(Copy(sSQL, 1, 1)) = 'S' then Open
    else ExecSQL;
  except
    on E: Exception do
    begin
      Result := E.Message;
    	Set_Log(Format('Uni_Open Error = (%s) / [%s]', [Result, sSQL]), 'E');
    end;
  end;
end;

function Client_Open(DataSet: TClientDataSet; sSQL: String): Boolean;
var
	sValue, sError: String;
begin
	Result := True;
  sValue := '';

  with __IdSocket do
  begin
    try
      if Not Connected then Connect;
    except
      on E: EIdSocketError  do
      begin
        msgInfo(Format('네트워크 상태가 불안합니다. [ECode:%d]\n\n재접속하여 체크하시길 바랍니다.', [E.LastError]));

        Application.Terminate;
      end;
    end;

    try
      Write(StrToCode(sSQL) + __EOL);
      sValue := ReadLn(__EOL, 10000);
      //IOHandler.Write(StrToCode(sSQL) + __EOL);
      //sValue := IOHandler.ReadLn(__EOL, 10000);

      if sValue = '' then
      begin
        MsgError('네트워크 상태가 불안합니다. 재접속하여 체크하시길 바랍니다. (Socket)');

        Application.Terminate;
      end;

    except
      on E: EIdSocketError  do
      begin
        MsgInfo(Format('네트워크 상태가 불안합니다.\n\n재접속하여 체크하시길 바랍니다. [ECode:%d]', [E.LastError]));

        Application.Terminate;
      end;     
      on E: Exception do
      begin
        Result := False;
        MsgError(Format('네트워크 상태가 불안합니다. 재접속하여 체크하시길 바랍니다.\n\n[ %s ]', [E.Message]));

        Application.Terminate;
      end;
    end;
  end;

  sError := StrToResult(sValue, 'ERROR');
  if sError <> '' then
  begin
  	MsgError(sError);
    Exit;
  end;

  StrToDataSet(sValue, DataSet, True);
end;

(* IP ========================================================================*)
function GetLocalIP : String;
var
	WSAData: TWSAData;
 	HostName, IPAddress: String;
 	HostEnt: PHostEnt;
begin
 	WSAStartup(2, WSAData);
 	SetLength(HostName, 255);
 	GetHostname(PChar(HostName), 255);
 	SetLength(HostName, StrLen(PChar(HostName)));
 	HostEnt := GetHostByName(PChar(HostName));

 	with HostEnt^ do
		IPAddress := Format('%d.%d.%d.%d', [Byte(h_addr^[0]), Byte(h_addr^[1]),
   								Byte(h_addr^[2]), Byte(h_addr^[3])]);
 	WSACleanup;
 	Result := IPAddress;
end;

function GetMacAddress: String;
var
  UuidCreateFunc : function (var guid: TGUID):HResult;stdcall;
  handle: THandle;
  g:TGUID;
  WinVer: _OSVersionInfoA;
  i: integer;
begin
	Result := '';

  WinVer.dwOSVersionInfoSize := sizeof(WinVer);
  getversionex(WinVer);

  handle := LoadLibrary('RPCRT4.DLL');

  if WinVer.dwMajorVersion >= 5 then {Windows 2000 }
  	@UuidCreateFunc := GetProcAddress(Handle, 'UuidCreateSequential')
  else
	  @UuidCreateFunc := GetProcAddress(Handle, 'UuidCreate') ;

  UuidCreateFunc(g);
  
  for i:=2 to 7 do
	  Result := Result + IntToHex(g.d4[i],2);
end;


//InfoStr: array[1..InfoNum] of string = ('CompanyName', 'FileDescription', 'FileVersion', 'InternalName', 'LegalCopyright', 'LegalTradeMarks', 'OriginalFileName', 'ProductName', 'ProductVersion', 'Comments', 'GTS');
(* Resource ==================================================================*)
function Get_ResInfo(InfoName: String): String;
var
  S: string;
  n, Len: DWORD;
  Buf: PChar;
  Value: PChar;
begin
	Result := '';

 	s := Application.ExeName;
  n := GetFileVersionInfoSize(PChar(s), n);

  if n > 0 then
  begin
    Buf := AllocMem(n);
    GetFileVersionInfo(PChar(s), 0, n, Buf);

    if VerQueryValue(Buf, PChar('StringFileInfo\041203b5\' + InfoName), Pointer(Value), Len) then
    	Result := Value;

    FreeMem(Buf, n);
  end
end;

(* DownLoad ==================================================================*)
function InfoDownloadToFile(URL, FileName: String): Boolean;
var
	hr: HRESULT;
  sDir: String;
begin
	DeleteUrlCacheEntry(PChar(URL));

  sDir := ExtractFileDir(FileName);

  if Not DirectoryExists(sDir) then ForceDirectories(sDir);

	hr := UrlDownloadToFile(nil, PChar(URL), PChar(FileName), 0, nil);

	Result := hr = S_OK;
end;

(* Etc =======================================================================*)
procedure ComponetEnabled(Component: TComponent; Value: Variant; sProp: String='Enabled');
begin
	if GetPropInfo(Component, sProp) <> Nil then
  	 SetPropValue(Component, sProp, Value);
end;

procedure ChangeProp(AGroup:TWinControl; name:string; value:variant);
var
  n:integer;
begin
  for n:=0 to AGroup.ControlCount-1 do
  begin
    if GetPropInfo(AGroup.Controls[n],name)<>Nil then
       SetPropValue(AGroup.Controls[n], name, value);
  end;
end;

procedure Delay(msecs: integer);
var
   nT: Longint;
begin
   nT := GetTickCount;
   repeat
     Application.ProcessMessages;
   until ((GetTickCount - nT) >= Longint(msecs));
end;

(* License ===================================================================*)
function Get_Lic: Boolean;
begin
  Result := LIC_SIGN;
end;

function Get_LicDate: String;
begin
  Result := LIC_DATE;
end;

function GTLic_Connection: Boolean;
var
  sLicFile, sDefDate, sLicKey, sLicDate, sIP: String;
  dDate: Double;
begin
  sDefDate := '20300903';
  LIC_DATE := sDefDate;

  sLicFile := 'C:\Lic_Sign.dat';

  if Not FileExists(sLicFile) then
  begin
    Result := NowDate < sDefDate;
    Exit;
  end;

  sLicDate := Get_INIFile(sLicFile, 'LIC_SIGN', 'DATE', sDefDate);
  sLicKey  := Get_INIFile(sLicFile, 'LIC_SIGN', 'KEY',  '');

  dDate := StrToFloatDef(sLicDate, 0);
  dDate := (dDate + 902) * 9;

  if sLicKey = FloatToStr(dDate) then
  begin
    LIC_DATE := sLicDate;
    Result := NowDate < sLicDate;
  end
  else
  begin
    LIC_DATE := sDefDate;
    Result := NowDate < sDefDate;
  end;

  // DataBase Lic --------------------------------------------------------------
  if Not Result then
  begin
    sIP := GetLocalIP;
  end;
end;

initialization
	if UpperCase(Get_ResInfo('ProductName')) <> 'TRADEPLUS' then _GT_Sign := GTLic_Connection;
  InitializeCriticalSection(CritSect);

finalization
  DeleteCriticalSection(CritSect);

end.
