unit fmMT4UpdateU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, fmMainU, TlHelp32, PsAPI,
  Vcl.ExtCtrls, System.IOUtils, IdBaseComponent, IdIntercept,
  IdInterceptThrottler, sLabel, sButton, sPanel,ShellAPI
  ,CommonUtils, sCheckBox;

type
  TfmMT4Update = class(TForm)
    Timer1: TTimer;
    IdInterceptThrottler1: TIdInterceptThrottler;
    sPanel1: TsPanel;
    btnUpdate: TsButton;
    btnNotNow: TsButton;
    sLabel1: TsLabel;
    lblExeStatus: TsLabel;
    lblWarning: TsLabel;
    chkRunMT4: TsCheckBox;
    procedure FormShow(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure btnUpdateClick(Sender: TObject);
    procedure btnNotNowClick(Sender: TObject);
  private
    function ProcessExists(sTerminalExeName: string): Boolean;
    function TerminateProcessByID(ProcessID: Cardinal): Boolean;
    procedure ReRunTerminal();
    function  GetID_Of_RunningTerminal():DWORD;

    { Private declarations }
  public
    m_MT4Index : integer;
    m_ProcessFileName  : string;  // Array of string;
    m_ProcessID        : dword; //      Array of DWORD;
    //m_KillID      : DWORD;
    //m_bReRunMT4   : boolean;
    { Public declarations }
  end;

var
  fmMT4Update: TfmMT4Update;

implementation

{$R *.dfm}

const DEF_NON_RUNNING = 'NOT Running';
const DEF_RUNNING     = 'Running';


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

function TfmMT4Update.ProcessExists(sTerminalExeName: string): Boolean;
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

  Result := False;

  m_ProcessFileName  := '';
  m_ProcessID        := 0;

  FSnapshotHandle := CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  FProcessEntry32.dwSize := SizeOf(FProcessEntry32);
  ContinueLoop := Process32First(FSnapshotHandle, FProcessEntry32);

  while Integer(ContinueLoop) <> 0 do
  begin
    if UpperCase(ExtractFileName(FProcessEntry32.szExeFile)) = UpperCase(ExtractFileName(sTerminalExeName)) then
    begin
      myPID := FProcessEntry32.th32ProcessID;
      myHandle := OpenProcess(PROCESS_QUERY_INFORMATION or PROCESS_VM_READ, False, myPID);
      if myHandle <> 0 then
      try
        SetLength(fullPath, MAX_PATH);
        if GetModuleFileNameEx(myHandle, 0, PChar(fullPath), MAX_PATH) > 0 then
        begin
          SetLength(fullPath, StrLen(PChar(fullPath)));
          if UpperCase(ExtractFileName(fullPath)) = UpperCase(sTerminalExeName) then
          begin
            Result := True;
            //SetLength(m_ProcessFileName, Length(m_ProcessFileName) + 1);
            //SetLength(m_ProcessID, Length(m_ProcessID) + 1);

            m_ProcessFileName  := fullPath;
            m_ProcessID        := myPID;
            exit;
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


procedure TfmMT4Update.ReRunTerminal();
var
  sTerminal : string;
begin

  if fmMain.MT4Info[m_MT4Index].sTerminalTp=TERMINAL_TP_MT4 then
    sTerminal := fmMain.MT4Info[m_MT4Index].ExePath + '\terminal.exe'
  else
    sTerminal := fmMain.MT4Info[m_MT4Index].ExePath + '\terminal64.exe';

  ShellExecute(Handle, PWidechar('open'), PWideChar(sTerminal),nil, nil, SW_SHOW);

end;



function TfmMT4Update.GetID_Of_RunningTerminal():DWORD;
var
  i1      : Integer;
  bMT4Found,
  bMT5Found : boolean;
begin

  Result := 0;

  lblExeStatus.Caption    := DEF_NON_RUNNING;
  lblExeStatus.Font.Color := clGreen;

  bMT4Found := false;
  bMT5Found := false;

  if fmMain.MT4Info[m_MT4Index].sTerminalTp = TERMINAL_TP_MT4 THEN
  BEGIN
    if ProcessExists('terminal.exe') then
    begin
      if Pos(fmMain.MT4Info[m_MT4Index].ExePath, m_ProcessFileName) > 0 then
        bMT4Found := true;
    end;
  END;

  if fmMain.MT4Info[m_MT4Index].sTerminalTp = TERMINAL_TP_MT5 THEN
  BEGIN
    if ProcessExists('terminal64.exe') then
    begin
      if Pos(fmMain.MT4Info[m_MT4Index].ExePath, m_ProcessFileName) > 0 then
        bMT5Found := true;
    end;
  END;


  if bMT4Found or bMT5Found  then
  begin
    lblExeStatus.Caption := DEF_RUNNING;
    lblExeStatus.Font.Color := clRed;
    Result := m_ProcessID; //[i1];

    lblWarning.Caption    := 'When you click [Update], the running MT4 will be closed'; //fmMain.ML.GetTranslatedText('WARN_CLOSE_MT4');
    lblWarning.Font.Color := clred;
    lblWarning.Visible    := true;

  end;

end;

procedure TfmMT4Update.Timer1Timer(Sender: TObject);
var
  i1      : Integer;
  bMT4Found,
  bMT5Found : boolean;
begin

  Timer1.Enabled := false;

  if not IsWindowVisible(Handle) then Exit;

  // re-run terminal
  if chkRunMT4.Checked then
  begin
    ReRunTerminal();
    Close;
    exit;
  end;

  GetID_Of_RunningTerminal();

end;

procedure TfmMT4Update.btnNotNowClick(Sender: TObject);
begin
  Close;
end;

procedure TfmMT4Update.btnUpdateClick(Sender: TObject);
var i1          : integer;
    fn1, fn2    : string;
    error       : boolean;
    bFound      : boolean;
    folderTp,
    terminalTp  : string;
    bTerminalRunning : string;
    KillID    : DWORD;
begin


  KillID := GetID_Of_RunningTerminal();
  if KillID>0 then
  begin
    TerminateProcessByID(KillID);
    Sleep(1000);
  end;

  try

    // check for termial.exe still ran
    if KillID > 0 then
    begin
      KillID := GetID_Of_RunningTerminal();
      if KillID>0 then
      begin
        ShowMessage('Process was not killed, please wait some time and try again or quit it manually !'); //fmMain.ML.GetTranslatedText('ERR_NONE_KILLED_TERMINAL'));
        exit;
      end;
    end;

    // if here - it means we can start update procedure

    error := FALSE;
    for i1 := 0 to Length(fmMain.FilesToDownload) - 1 do
    begin
      fn1 := fmMain.m_Mt4DownloadFolder + '\' + fmMain.FilesToDownload[i1].FileName;
      fn2 := fmMain.MT4Info[m_MT4Index].CopyPath + '\' + fmMain.FilesToDownload[i1].Folder + '\' +
      fmMain.FilesToDownload[i1].FileName;

      folderTp    := __Get_TerminalTpOfFolder(fmMain.MT4Info[m_MT4Index].CopyPath);
      terminalTp  := __Get_TerminalTpOfFile(fmMain.FilesToDownload[i1].FileName);

      if terminalTp<>TERMINAL_TP_ALL then
      begin
        if folderTp<>terminalTp then
          continue;
      end;

      ForceDirectories(fmMain.MT4Info[m_MT4Index].CopyPath + '\' + fmMain.FilesToDownload[i1].Folder);
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
      ShowMessage('Failed to copy Alpha Files. Please try again !');  //fmMain.ML.GetTranslatedText('ERR_COPY_MT4FILES'));
      Exit;
    end
    else
    begin
      fmMain.MT4Info[m_MT4Index].State    := STATE_LATEST;
      fmMain.MT4Info[m_MT4Index].StateStr := StateStrings[STATE_LATEST];
      fmMain.gdTerminal.Cells[1, m_MT4Index +1]  := fmMain.MT4Info[m_MT4Index].StateStr;

      ShowMessage('Updated successfully !');  //fmMain.ML.GetTranslatedText('SUCC_COPY_MT4FILES'));

      // re-reun mt4 terminal in timer
      if chkRunMT4.Checked then
      begin
        Timer1.Interval := 3000;
        Timer1.Enabled  := TRUE;
        Exit;
      end;
    end;

  except
    ShowMessage('Failed to copy Alpha Files. Please try again !');  //fmMain.ML.GetTranslatedText('ERR_COPY_MT4FILES'));
    Exit;

  end; // end of try

end;

procedure TfmMT4Update.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  Timer1.Enabled := FALSE;
end;

procedure TfmMT4Update.FormShow(Sender: TObject);
var
  i1: Integer;
begin

  chkRunMT4.Checked := False;

  Caption := 'Update MT4 (' + fmMain.MT4Info[m_MT4Index].ExePath + ')';

  chkRunMT4.Caption :=  'Run MT4 after Update'; //fmMain.ML.GetTranslatedText('WANT_RUN_MT4');

  Timer1.Enabled := TRUE;


end;

end.
