unit uCommonUtils;

interface

uses
  Messages, Graphics, Classes, SysUtils, windows, TlHelp32;

//const

//var
  function __KillProcess(const ProcName: String): Boolean;
  function __ExtractAppName(sFullPath:string):string;

implementation


//   s1 := 'C:\FX-WORLD\FXWorld.exe';
function __ExtractAppName(sFullPath:string):string;
var
  sFull : string;
  i1, i2 : integer;
  len1, len2 : integer;
begin
  sFull := sFullPath;
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

function __KillProcess(const ProcName: String): Boolean;
var
  Process32: TProcessEntry32;
  SHandle:   THandle;
  Next:      Boolean;
  hProcess: THandle;
  i: Integer;
 
begin
  Result:=True;
 
  Process32.dwSize       :=SizeOf(TProcessEntry32);
  Process32.th32ProcessID:=0;
  SHandle                :=CreateToolHelp32Snapshot(TH32CS_SNAPPROCESS, 0);
 
  // 종료하고자 하는 프로세스가 실행중인지 확인하는 의미와 함께...
  if Process32First(SHandle, Process32) then begin
    repeat
      Next:=Process32Next(SHandle, Process32);
      if AnsiCompareText(Process32.szExeFile, Trim(ProcName))=0 then break;
    until not Next;
  end;
  CloseHandle(SHandle);
 
  // 프로세스가 실행중이라면 Open & Terminate
  if Process32.th32ProcessID<>0 then begin
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

initialization

finalization

end.
