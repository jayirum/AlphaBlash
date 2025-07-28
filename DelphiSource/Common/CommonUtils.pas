
unit CommonUtils;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs,
  IniFiles, Forms, TypInfo, WinSock, DBClient, ShellApi, TlHelp32,
  AdvUtil, AdvObj, BaseGrid, AdvGrid
  ;


(* Encrypt ===================================================================*)
function ValueToHex(const S: String): String;
function HexToValue(const S: String) : String;
function __Encrypt(const S: String):String; //; Key1, Key2, Key3: WORD): String;
function __Decrypt(const S: String):String; //; Key1, Key2, Key3: WORD): String;



(* Ini ======================================================================*)
procedure __Set_INIFile(sIniFileNm, sSec, sKey, sValue: String);
function  __Get_INIFile(sIniFileNm, sSec, sKey: String; sDefault: String='';bFullName:boolean=true): String;

function  __Get_CFGFileName( sExtension:string='Ini'):string;

procedure __Set_CFGFile(sSec, sKey, sValue: String; Encrypt: Boolean=False; sFileName: String='';sExtension:string='Ini');
function  __Get_CFGFile(sSec, sKey: String; sDefault: String=''; Encrypt: Boolean=False; sFileName: String='';sExtension:string='Ini'): String;
function  __Get_CFGValue(sSec, sKey: String; sFileName: String;sDir:string; sDefault: String=''): String;

(* Process ==================================================================*)
function __KillProcess(const ProcName: String): Boolean;


(* Time ===================================================================*)
function __NowHMS():string;

(* Format ===================================================================*)
function __FmtPrcD(nDecimal:integer; dPrc:double):string;
function __FmtPrc(nDecimal:integer; sPrc:string):string;
function __FmtPip(dPip:double):string;

(* Etc ==================================================================*)
function __ExtractAppName(const sFullAppName:string):string;
function __ExtractAppNameWithoutExt(const sFullAppName:string):string;


function __Get_TerminalTpOfFolder(sPath:string):string;
function __Get_TerminalTpOfFile(sFileName:string):string;

function __Confirm(sMsg:string):boolean;

function __PrepareAddNewRow(var advGrid:TAdvStringGrid):integer;

function __Split(cDeli:char; sOrgin:string; var oArray:TstringList):integer;

(* WideToChar ==================================================================*)
function __CharFunction(instring : pointer; inLen:integer) : string; stdcall;

const

	__EOL = #10;                           // Socekt End Of Line


  TERMINAL_TP_ALL = 'ALL';
  TERMINAL_TP_MT4 = 'MT4';
  TERMINAL_TP_MT5 = 'MT5';

var
	__DebugMode: Boolean=False;


implementation

const
	HexaChar: array [0..15] of Char=('0','1','2','3','4','5','6','7','8','9',
                                   'A','B','C','D','E','F');

  KEY_1 = 3;
  KEY_2 = 7;
  KEY_3 = 5;


var
	//_GT_Sign: Boolean=True;
  CritSect: TRTLCriticalSection;

  _ErrorCnt: Integer=0;


(* Encrypt ===================================================================*)

// _____________________________________________________________________________
function ValueToHex(const S: String): String;
var i: Integer;
begin
  SetLength(Result, Length(S)*2); // 문자열 크기를 설정
  for i:=0 to Length(S)-1 do begin
    Result[(i*2)+1]:=HexaChar[Integer(S[i+1]) shr 4];
    Result[(i*2)+2]:=HexaChar[Integer(S[i+1]) and $0f];
  end;
end;

// Hexadecimal로 구성된 문자열을 Byte 데이터로 변환
// _____________________________________________________________________________
function HexToValue(const S: String) : String;
var i: Integer;
begin

  SetLength(Result, Length(S) div 2);
  for i:=0 to (Length(S) div 2)-1 do begin
    Result[i+1] := Char(StrToInt('$'+Copy(S,(i*2)+1, 2)));
  end;
end;

function __Encrypt(const S: String): String;
var
  i: Byte;
  FirstResult: String;
  Key1, Key2, Key3: WORD;
begin
  Key1 := KEY_1;
  Key2 := KEY_2;
  Key3 := KEY_3;

  try
    SetLength(FirstResult, Length(S));
    for i:=1 to Length(S) do begin
      FirstResult[i]:=Char(Byte(S[i]) xor (Key1 shr 8));
      Key1          :=(Byte(FirstResult[i])+Key1)*Key2+Key3;
    end;
    Result:=ValueToHex(FirstResult);
  except
    Result := '';
  end;
end;

function __Decrypt(const S: String): String;
var
  i: Byte;
  FirstResult: String;
  Key1, Key2, Key3: WORD;
begin

  Key1 := KEY_1;
  Key2 := KEY_2;
  Key3 := KEY_3;

  try
    FirstResult:=HexToValue(S);
    SetLength(Result, Length(FirstResult));
    for i:=1 to Length(FirstResult) do begin
      Result[i]:=Char(Byte(FirstResult[i]) xor (Key1 shr 8));
      Key1     :=(Byte(FirstResult[i])+Key1)*Key2+Key3;
    end;
  except
    result := '';

  end;
end;




(* File ======================================================================*)
procedure __Set_INIFile(sIniFileNm, sSec, sKey, sValue: String);
var
	INI: TIniFile;
begin
	if Not DirectoryExists(sIniFileNm) then ForceDirectories(ExtractFilePath(sIniFileNm));

	INI := TIniFile.Create(sIniFileNm);

  try
    INI.WriteString(sSec, sKey, sValue);
  finally
    INI.Free;
  end;
end;

function __Get_INIFile(sIniFileNm, sSec, sKey: String; sDefault: String='';bFullName:boolean=true): String;
var
	INI: TIniFile;
  sFullFile : string;
begin

  if bFullName then
    sFullFile := sIniFileNm
  else
    sFullFile := GetCurrentDir + '\\'+sIniFileNm;

	INI := TIniFile.Create(sFullFile);

  try
    Result := INI.ReadString(sSec, sKey, sDefault);
  finally
    INI.Free;
  end;
end;

procedure __Set_CFGFile(sSec, sKey, sValue: String;
                        Encrypt: Boolean=False;
                        sFileName: String='';
                        sExtension:string='Ini' );
var
	INI: TIniFile;
  sFile: String;
begin
	if sFileName = '' then sFile := ChangeFileExt(ParamStr(0), '.'+sExtension)
  else sFile := sFileName;

  INI := TIniFile.Create(sFile);
  sSec   := sSec;
  sKey := sKey;

  if Encrypt then
  begin
  	sValue := __Encrypt(sValue);
  end;

  try
    INI.WriteString(sSec, sKey, sValue);
  finally
    INI.Free;
  end;
end;


function  __Get_CFGFileName(sExtension:string='Ini'):string;
begin
  Result := ChangeFileExt(ParamStr(0), '.'+sExtension);
end;


function  __Get_CFGValue(sSec, sKey: String;
                        sFileName: String;
                        sDir:string;
                        sDefault: String=''
                        ): String;
var
  sFullName : string;
begin
  sFullName := sDir +'\\'+ sFileName;
  Result := __Get_CFGFile(sSec, sKey, sDefault,false, sFullName);
end;


function  __Get_CFGFile(sSec, sKey: String;
                        sDefault: String='';
                        Encrypt: Boolean=False;
                        sFileName: String='';
                        sExtension:string='Ini'): String;
var
	INI: TIniFile;
  sFile: String;
begin
	if sFileName = '' then
    sFile := ChangeFileExt(ParamStr(0), '.'+sExtension)
  else
    sFile := sFileName;

  INI := TIniFile.Create(sFile);

  try
	  if Not Encrypt then
  	  Result := INI.ReadString(sSec, sKey, sDefault)
    else
  	  Result := __Decrypt(INI.ReadString(sSec, sKey, sDefault));
  finally
    INI.Free;
  end;
end;

function __KillProcess(const ProcName: String): Boolean;
var
  Process32: TProcessEntry32;
  SHandle:   THandle;
  Next:      Boolean;
  hProcess: THandle;
  //i: Integer;

begin
  Result:=True;

  Process32.dwSize       :=SizeOf(TProcessEntry32);
  Process32.th32ProcessID:=0;
  SHandle                :=CreateToolHelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  // 종료하고자 하는 프로세스가 실행중인지 확인하는 의미와 함께...
  if Process32First(SHandle, Process32) then
  begin
    repeat
      Next:=Process32Next(SHandle, Process32);
      if AnsiCompareText(Process32.szExeFile, Trim(ProcName))=0 then break;
    until not Next;
  end;
  CloseHandle(SHandle);

  // 프로세스가 실행중이라면 Open & Terminate
  if Process32.th32ProcessID<>0 then
  begin
    hProcess:=OpenProcess(PROCESS_TERMINATE, True, Process32.th32ProcessID);
    if hProcess<>0 then begin
      if not TerminateProcess(hProcess, 0) then Result:=False;
    end
    // 프로세스 열기 실패
    else Result:=False;

    CloseHandle(hProcess);
  end // if Process32.th32ProcessID<>0
  else Result:=False;
end;



// d:\work\app\app.exe ==> app.exe
function __ExtractAppName(const sFullAppName:string):string;
var
  sFull : string;
  i1, i2 : integer;
  len1, len2 : integer;
begin
  sFull := sFullAppName;
  len1 := length(sFull);

  for i1 := 0 to len1 - 1 do
  begin
    len1 := length(sFull);
    i2 := pos('\', sFull);
    if i2=0 then
      break;

    len2 := len1 - i2;
    sFull := copy(sFull, i2+1, len2);

  end;
  Result := sFull;
end;



function __ExtractAppNameWithoutExt(const sFullAppName:string):string;
var
  sAppName : string;
begin

  Result := '';
  sAppName := __ExtractAppName(sFullAppName);
  if( Length(sAppName)>0)  then
  begin
    Result := Copy(sAppName, 1, pos('.', sAppName)-1);
  end;



end;


function __Get_TerminalTpOfFolder(sPath:string):string;
begin
  Result := TERMINAL_TP_MT4;
  if Pos('MQL5', Uppercase(sPath))>0 then
    Result := TERMINAL_TP_MT5;
end;

function __Get_TerminalTpOfFile(sFileName:string):string;
BEGIN
  Result := TERMINAL_TP_ALL;

  // alphablashmaster.ex5, alphablash.ex4
  if Pos('EX5', Uppercase(sFileName))>0 then
    Result := TERMINAL_TP_MT5
  else if Pos('EX4', Uppercase(sFileName))>0 then
    Result := TERMINAL_TP_MT4;

  // alphaoms_mt5.dll, alphaoms_mt4.dll
  if Pos('MT5', Uppercase(sFileName))>0 then
    Result := TERMINAL_TP_MT5
  else if Pos('MT4', Uppercase(sFileName))>0 then
    Result := TERMINAL_TP_MT4;


END;


function __NowHMS():string;
begin
  Result := FormatDateTime('HH:NN:SS', now);
end;



function __CharFunction(instring : pointer; inLen:integer) : string; stdcall;
var b : Array [0..10000] of byte;
    p : pointer;
    s : string;
    index, len, i1 : integer;
    ansi : ansistring;
begin

  p := instring;
  index := 0; len := 0;
  while (len < inLen) do
  begin

    CopyMemory(Addr(b[index]), p, 1);
    Inc(PByte(P), 1);
    //if b[index] = 0 then Break;
    Inc(index);
    Inc(len);

  end;


  SetLength(s, len);
  for i1 := 1 to len do
  begin
    //s[i1] := Chr(b[i1 - 1]);
    ansi[i1] := ansichar(Chr(b[i1 - 1]));
  end;

  s := string(ansi);
  //ShowMessage(s);

  //outstring := instring;

  Result := s;
end;


function __FmtPrc(nDecimal:integer; sPrc:string):string;
begin
  Result := __FmtPrcD(nDecimal, strtofloatdef(sPrc,0));
end;

function __FmtPrcD(nDecimal:integer; dPrc:double):string;
begin
       if nDecimal = 1 then  Result := formatfloat('#0.#', dPrc)
  else if nDecimal = 2 then  Result := formatfloat('#0.0#', dPrc)
  else if nDecimal = 3 then  Result := formatfloat('#0.00#', dPrc)
  else if nDecimal = 4 then  Result := formatfloat('#0.000#', dPrc)
  else if nDecimal = 5 then  Result := formatfloat('#0.0000#', dPrc)
  else                       Result := formatfloat('#', dPrc);

end;

function __FmtPip(dPip:double):string;
begin
  Result := FormatFloat('#0.0#', dPip);
end;


function __Confirm(sMsg:string):boolean;
begin
  Result := True;
  if MessageDlg(sMsg, mtConfirmation, [mbYes, mbNo],0) <> mrYes then
    Result := False;
end;


function __PrepareAddNewRow(var advGrid:TAdvStringGrid):integer;
var
  idxTarget : integer;
begin

  if advGrid.RowCount=2 then
  begin
    if advGrid.cells[0, 1]='' then
    begin
      idxTarget := 1;
    end
    else
    begin
      advGrid.AddRow;
      idxTarget := 2;
    end;
  end
  else
  begin
    advGrid.AddRow;
    idxTarget := advGrid.RowCount-1;
  end;

  Result := idxTarget;

end;


(*
  ex)
procedure Test
var
  sArray : TstringList;
  i      : integer;
  cnt    : integer;
  s      : string;
  arrRslt : array[0..2] of string;
begin

  sArray := TstringList.create;

  s := 'EURUSD/5/0.00001';

  cnt := __Split('/', s, sArray);

  arrRslt[0] := sArray[0];
  arrRslt[1] := sArray[1];
  arrRslt[2] := sArray[2];

  sArray.Free;
*)

function __Split(cDeli:char; sOrgin:string; var oArray:TstringList):integer;
begin
  //if not assigned(oArray) then
    //oArray :=


  oArray.Clear;
  oArray.Delimiter       := cDeli;
  oArray.StrictDelimiter := True; // Requires D2006 or newer.
  oArray.DelimitedText   := sOrgin;

  Result := oArray.Count;
end;


initialization
  InitializeCriticalSection(CritSect);

finalization
  DeleteCriticalSection(CritSect);

end.
