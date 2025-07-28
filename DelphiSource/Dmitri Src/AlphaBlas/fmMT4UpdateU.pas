unit fmMT4UpdateU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, fmMainU, TlHelp32, PsAPI,
  Vcl.ExtCtrls, System.IOUtils;

type
  TfmMT4Update = class(TForm)
    btnUpdate: TButton;
    btnNotNow: TButton;
    Label1: TLabel;
    lblExeStatus: TLabel;
    cbKillExe: TCheckBox;
    Timer1: TTimer;
    procedure FormShow(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure btnUpdateClick(Sender: TObject);
  private
    function ProcessExists(anExeFileName: string): Boolean;
    function TerminateProcessByID(ProcessID: Cardinal): Boolean;
    { Private declarations }
  public
    MT4Index : integer;
    ProcessFileNames : Array of string;
    ProcessIDs : Array of DWORD;
    KillID : DWORD;
    { Public declarations }
  end;

var
  fmMT4Update: TfmMT4Update;

implementation

{$R *.dfm}

function TfmMT4Update.TerminateProcessByID(ProcessID: Cardinal): Boolean;
var
  hProcess : THandle;
begin
  Result := False;
  hProcess := OpenProcess(PROCESS_TERMINATE,False,ProcessID);
  if hProcess > 0 then
  try
    Result := Win32Check(TerminateProcess(hProcess,0));
  finally
    CloseHandle(hProcess);
  end;
end;

function TfmMT4Update.ProcessExists(anExeFileName: string): Boolean;
var
  ContinueLoop: BOOL;
  FSnapshotHandle: THandle;
  FProcessEntry32: TProcessEntry32;
  fullPath: string;
  myHandle: THandle;
  myPID: DWORD;
begin
  // wsyma 2016-04-20 Erkennung, ob ein Prozess in einem bestimmten Pfad schon gestartet wurde.
  // Detection wether a process in a certain path is allready started.
  // http://stackoverflow.com/questions/876224/how-to-check-if-a-process-is-running-using-delphi
  // http://swissdelphicenter.ch/en/showcode.php?id=2010
  SetLength(ProcessFileNames, 0);
  SetLength(ProcessIDs, 0);
  FSnapshotHandle := CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  FProcessEntry32.dwSize := SizeOf(FProcessEntry32);
  ContinueLoop := Process32First(FSnapshotHandle, FProcessEntry32);
  Result := False;
  while Integer(ContinueLoop) <> 0 do
  begin
    if UpperCase(ExtractFileName(FProcessEntry32.szExeFile)) = UpperCase(ExtractFileName(anExeFileName)) then
    begin
      myPID := FProcessEntry32.th32ProcessID;
      myHandle := OpenProcess(PROCESS_QUERY_INFORMATION or PROCESS_VM_READ, False, myPID);
      if myHandle <> 0 then
      try
        SetLength(fullPath, MAX_PATH);
        if GetModuleFileNameEx(myHandle, 0, PChar(fullPath), MAX_PATH) > 0 then
        begin
          SetLength(fullPath, StrLen(PChar(fullPath)));
          if UpperCase(ExtractFileName(fullPath)) = UpperCase(anExeFileName) then
          begin
            Result := True;
            SetLength(ProcessFileNames, Length(ProcessFileNames) + 1);
            SetLength(ProcessIDs, Length(ProcessIDs) + 1);

            ProcessFileNames[Length(ProcessFileNames) - 1] := fullPath;
            ProcessIDs[Length(ProcessIDs) - 1] := myPID;
          end
          else
          begin
            fullPath := '';
          end;
        end;

      finally
        CloseHandle(myHandle);
      end;
      //if Result then
      //  Break;
    end;
    ContinueLoop := Process32Next(FSnapshotHandle, FProcessEntry32);
  end;
  CloseHandle(FSnapshotHandle);
end;

procedure TfmMT4Update.Timer1Timer(Sender: TObject);
var i1: Integer;
begin

if not IsWindowVisible(Handle) then Exit;


lblExeStatus.Caption := 'Idle';
lblExeStatus.Font.Color := clGreen;
cbKillExe.Visible := FALSE;

if ProcessExists('terminal.exe') then
begin
  for i1 := 0 to Length(ProcessFileNames) - 1 do
  begin
    if Pos(LowerCase(fmMain.MT4Info[MT4Index].ExePath), LowerCase(ProcessFileNames[i1])) > 0 then
    begin
      lblExeStatus.Caption := 'Ran';
      lblExeStatus.Font.Color := clRed;
      cbKillExe.Visible := TRUE;
      KillID := ProcessIDs[i1];
      Break;
    end;
  end;
end;

btnUpdate.Enabled := (not (cbKillExe.Visible)) or (cbKillExe.Checked);
end;

procedure TfmMT4Update.btnUpdateClick(Sender: TObject);
var i1 : integer;
    fn1, fn2 : string;
    error : boolean;
begin

Timer1.Enabled := FALSE;

if cbKillExe.Checked then
begin
  TerminateProcessByID(KillID);
  Sleep(500);
end;

try

// check for termial.exe still ran

if cbKillExe.Checked then
begin
  lblExeStatus.Caption := 'Idle';
  lblExeStatus.Font.Color := clGreen;
  cbKillExe.Visible := FALSE;

  if ProcessExists('terminal.exe') then
  begin
    for i1 := 0 to Length(ProcessFileNames) - 1 do
    begin
      if Pos(LowerCase(fmMain.MT4Info[MT4Index].ExePath), LowerCase(ProcessFileNames[i1])) > 0 then
      begin
        lblExeStatus.Caption := 'Ran';
        lblExeStatus.Font.Color := clRed;
        cbKillExe.Visible := TRUE;
        KillID := ProcessIDs[i1];
        Break;
      end;
    end;
  end;

  // if still ran - show message and exit
  if lblExeStatus.Caption = 'Ran' then
  begin
    ShowMessage('Process was not killed, please wait some time and try again or quit it manually !');
    Timer1.Enabled := TRUE;
    Exit;
  end;
end;

// if here - it means we can start update procedure

error := FALSE;
for i1 := 0 to Length(fmMain.FilesToDownload) - 1 do
begin
  fn1 := fmMain.ExeFolder + '\Temp\' + fmMain.FilesToDownload[i1].FileName;
  fn2 := fmMain.MT4Info[MT4Index].CopyPath + '\' + fmMain.FilesToDownload[i1].Folder + '\' +
  fmMain.FilesToDownload[i1].FileName;
  ForceDirectories(fmMain.MT4Info[MT4Index].CopyPath + '\' + fmMain.FilesToDownload[i1].Folder);
  DeleteFile(fn2);
  TFile.Copy(fn1, fn2);

  if not FileExists(fn2) then
  begin
    error := TRUE;
  end;
end;

// now check is it was succesfully or not
if error then
begin
  ShowMessage('Something went wrong, please try again !');
  Timer1.Enabled := TRUE;
  Exit;
end
else
begin
  fmMain.MT4Info[MT4Index].State := STATE_LATEST;
  fmMain.MT4Info[MT4Index].StateStr := StateStrings[STATE_LATEST];
  fmMain.SG1.Cells[1, MT4Index +1] := fmMain.MT4Info[MT4Index].StateStr;
  ShowMessage('Updated successfuly !');
  Timer1.Enabled := FALSE;
  Close;
  Exit;
end;

Timer1.Enabled := TRUE;
except
  ShowMessage('Something went wrong, please try again !');
  Timer1.Enabled := TRUE;
  Exit;
end;

end;

procedure TfmMT4Update.FormClose(Sender: TObject; var Action: TCloseAction);
begin
Timer1.Enabled := FALSE;
end;

procedure TfmMT4Update.FormShow(Sender: TObject);
var
  i1: Integer;
begin

Caption := 'Update MT4 (' + fmMain.MT4Info[MT4Index].ExePath + ')';

Timer1.Enabled := TRUE;


end;

end.
