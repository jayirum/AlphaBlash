unit CheckUpdateThread;

interface

uses
  System.Classes, System.SysUtils, Forms, Windows, VCL.Graphics,
  StrUtils, SyncObjs, IdComponent, IdTCPConnection, IdTCPClient, IdHTTP, IniFiles,
  IdIOHandler, IdIOHandlerStream, dialogs
  ,CommonUtils, CommonVal;



type
  TCheckUpdateThread = class(TThread)
  private
    procedure UpdateStatus_CompareFiles();
    procedure UpdateStatus_NetworkErr();
    procedure UpdateStatus_OnGrid();

    function CompareTwoFiles(fn1, fn2 : string): boolean;
    function GetFileSize(const aFilename: String): Int64;

    procedure ShowProgressBar;
    procedure HideProgressBar;
    procedure UpdateDownloadState;


    { Private declarations }
  protected
    function  DownloadFiles():boolean;
    procedure Execute; override;
  public

    m_UpdateIndex : integer;
    m_sUpdateState : string;
    m_nUpdateState : integer;

    IDHTTP : TIDHTTP;
    m_nCurrDownload, m_nTotalDownload : integer;

    procedure Initialize;
    //function StartRecord : integer;
    //function StopRecord : integer;

  end;

implementation

uses fmMainU;


(*constructor TCheckUpdateThread.Create(Suspended : boolean);
begin
  H264InitializedOnce := FALSE;
  H264ReadyToRecord := FALSE;
  RecordingNow := FALSE;
  H264Bitmap := TBitmap.Create;
  inherited Create(Suspended);
end;           *)

function TCheckUpdateThread.GetFileSize(const aFilename: String): Int64;
var
  info: TWin32FileAttributeData;
begin
  result := -1;

  if NOT GetFileAttributesEx(PWideChar(aFileName), GetFileExInfoStandard, @info) then
    EXIT;

  result := Int64(info.nFileSizeLow) or Int64(info.nFileSizeHigh shl 32);
end;

function TCheckUpdateThread.CompareTwoFiles(fn1, fn2 : string) : boolean;
var S1, S2 : TFileStream;
    Buffer1, Buffer2 : Array of byte;
    fs1, fs2, read1, read2 : integer;
    equal : boolean;
  i1: Integer;
begin
  Result := TRUE;
  if not FileExists(fn1) then Exit(FALSE);
  if not FileExists(fn2) then Exit(FALSE);

  fs1 := GetFileSize(fn1); fs2 := GetFileSize(fn2);
  if fs1 <> fs2 then Exit(FALSE);

  S1 := TFileStream.Create(fn1, fmOpenRead);
  S2 := TFileStream.Create(fn2, fmOpenRead);

  S1.Seek(0, 0);
  S2.Seek(0, 0);

  SetLength(Buffer1, fs1);
  SetLength(Buffer2, fs2);

  read1 := S1.Read(Buffer1[0], fs1);
  read2 := S2.Read(Buffer2[0], fs2);

  equal := TRUE;

  for i1 := 0 to fs1 - 1 do
  begin
    if Buffer1[i1] <> Buffer2[i1] then
    begin
      equal := FALSE;
      Break;
    end;
  end;

  Result := equal;

  S1.Free;
  S2.Free;


end;


procedure TCheckUpdateThread.UpdateStatus_CompareFiles();
var
  i1 : integer;
begin

  m_UpdateIndex   := -1;
  m_nUpdateState  := STATE_COMPARING_FILES;
  UpdateStatus_OnGrid();
end;

procedure TCheckUpdateThread.UpdateStatus_NetworkErr();
begin
  m_UpdateIndex   := -1;
  m_nUpdateState  := STATE_NETWORK_ERROR;
  UpdateStatus_OnGrid();
end;

procedure TCheckUpdateThread.UpdateStatus_OnGrid();
var
  i1 : integer;
begin
  //
  if m_UpdateIndex>-1 then
  begin                             
    fmMain.MT4Info[m_UpdateIndex].State     := m_nUpdateState;
    fmMain.MT4Info[m_UpdateIndex].StateStr  := StateStrings[m_nUpdateState];
    fmMain.SG1.Cells[1, m_UpdateIndex + 1]  := fmMain.MT4Info[m_UpdateIndex].StateStr;
  end
  else
  begin
    for i1 := 1 to fmMain.SG1.RowCount - 1 do
    begin
      fmMain.MT4Info[i1 - 1].State    := m_nUpdateState;
      fmMain.MT4Info[i1 - 1].StateStr := StateStrings[m_nUpdateState];
      fmMain.SG1.Cells[1, i1]         := fmMain.MT4Info[i1 - 1].StateStr;
    end;
  end;
end;


procedure TCheckUpdateThread.ShowProgressBar;
begin
  fmMain.pbDownload.Visible := TRUE;
  fmMain.lblDownload.Visible := TRUE;
end;

procedure TCheckUpdateThread.HideProgressBar;
begin
  fmMain.pbDownload.Visible := FALSE;
  fmMain.lblDownload.Visible := FALSE;
end;

procedure TCheckUpdateThread.UpdateDownloadState;
var perc : extended;
    percint : integer;
begin
  perc := m_nCurrDownload / m_nTotalDownload * 100.0;
  percint := Round(perc);
  fmMain.lblDownload.Caption := 'Downloading (' + IntToStr(percint) + '%)';
  fmMain.pbDownload.Position := percint;
  Application.ProcessMessages;
end;



function TCheckUpdateThread.DownloadFiles:boolean;
var
  i1 : integer;
  M  : TMemoryStream;
  fileName : string;
begin
  m_nCurrDownload   := 0;
  m_nTotalDownload  := 0;
  
  for i1 := 0 to Length(fmMain.FilesToDownload) - 1 do
  begin
    m_nCurrDownload := i1 + 1;
    m_nTotalDownload := Length(fmMain.FilesToDownload);
    Synchronize(UpdateDownloadState);

    M := TMemoryStream.Create;
    try
      fileName := 'http://'+fmMain.m_sUpdateUrl + '/' + __SVRDIR_MT4FILES + '/' + fmMain.FilesToDownload[i1].FileName;
      IDHTTP.Get(filename, M);
    except

      //TODO fmMain.AddMsg(fmMain.ML.GetTranslatedText('ERR_DOWNLOAD_FILES'), true, true);
      Synchronize(UpdateStatus_NetworkErr);

      //showmessage(fmMain.ML.GetTranslatedText('ERR_DOWNLOAD_MAINAPP')+fmMain.FilesToDownload[i1].FileName);
      Result := FALSE;
      exit;
    end;
    M.Seek(0, 0);
    M.SaveToFile(fmMain.m_Mt4DownloadFolder + '\' + fmMain.FilesToDownload[i1].FileName);
  end;

  Result := True;

end;

procedure TCheckUpdateThread.Execute;
var
  i1, i2, state : Integer;
  error         : boolean;
  fn1, fn2,
  statestr      : string;
  bEqual         : boolean;
  fileName      : string;
  folderTp,
  terminalTp    : string;
begin
  { Place thread code here }

  error := FALSE;


  if Not fmMain.m_bSecondRun then
  begin
    if fmMain.Must_Update() then
    begin
      //TODO fmMain.AddMsg(fmMain.ML.GetTranslatedText('MSG_START_DOWNLOAD'), false, false);
      Synchronize(ShowProgressBar);
      DownloadFiles();
      Synchronize(HideProgressBar);
      //TODO fmMain.AddMsg(fmMain.ML.GetTranslatedText('MSG_COMPLETE_DOWNLOAD'), false, false);
      fmMain.Update_MainApp();

      exit;
    end;
  end;

  //

  Synchronize(UpdateStatus_CompareFiles);
  //TODO fmMain.AddMsg(fmMain.ML.GetTranslatedText('MSG_START_DOWNLOAD'), false, false);


  try

    for i1 := 0 to Length(fmMain.MT4Info) - 1 do
    begin
      bEqual := True;

      for i2 := 0 to Length(fmMain.FilesToDownload) - 1 do
      begin
        fn1 := fmMain.m_Mt4DownloadFolder + '\' + fmMain.FilesToDownload[i2].FileName;
        fn2 := fmMain.MT4Info[i1].CopyPath + '\' + fmMain.FilesToDownload[i2].Folder
                + '\' + fmMain.FilesToDownload[i2].FileName;

        folderTp    := __Get_TerminalTpOfFolder(fmMain.MT4Info[i1].CopyPath);
        terminalTp  := __Get_TerminalTpOfFile(fmMain.FilesToDownload[i2].FileName);

        bEqual := True;
        if terminalTp = TERMINAL_TP_ALL then
        begin
          bEqual := CompareTwoFiles(fn1, fn2);
        end
        else
        begin
          if folderTp=terminalTp then
            bEqual := CompareTwoFiles(fn1, fn2);
        end;


        if not bEqual then
        begin
          Break;
        end;
      end;

      if bEqual then state := STATE_LATEST
      else           state := STATE_NEED_UPDATE;


      //if not latest then statestr := 'Update' else statestr := 'Latest';

      m_UpdateIndex   := i1;
      m_nUpdateState  := state;
      Synchronize(UpdateStatus_OnGrid);
        
    end; // for i1 := 0 to Length(fmMain.MT4Info) - 1 do

  except
    ShowMessage(IntToStr(i1) + ';' + IntToStr(i2) + #13#10 + fn1 + #13#10 + fn2);
  end; // try

  //Synchronize(HideProgressBar);

  //TODO fmMain.AddMsg(fmMain.ML.GetTranslatedText('MSG_COMPLETE_DOWNLOAD'), false, false);
  fmMain.Update_MainApp();

end;



procedure TCheckUpdateThread.Initialize;
begin
    //
end;

end.
