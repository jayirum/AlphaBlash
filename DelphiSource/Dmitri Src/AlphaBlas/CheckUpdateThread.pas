unit CheckUpdateThread;

interface

uses
  System.Classes, System.SysUtils, Forms, Windows, VCL.Graphics,
  StrUtils, SyncObjs, IdComponent, IdTCPConnection, IdTCPClient, IdHTTP, IniFiles,
  IdIOHandler, IdIOHandlerStream, dialogs;



type
  TCheckUpdateThread = class(TThread)
  private
    procedure CompareFilesInfo;
    procedure NetWorkErrorInfo;
    function CompareTwoFiles(fn1, fn2 : string): boolean;
    function GetFileSize(const aFilename: String): Int64;
    procedure UpdateState;

    { Private declarations }
  protected
    procedure Execute; override;
  public

    IDHTTP : TIDHTTP;
    UpdateIndex : integer;
    UpdateStateStr : string;
    UpdateStateInt : integer;
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

procedure TCheckUpdateThread.NetWorkErrorInfo;
var
  i1: Integer;
begin
  for i1 := 1 to fmMain.SG1.RowCount - 1 do
  begin
    fmMain.MT4Info[i1 - 1].State := STATE_NETWORK_ERROR;
    fmMain.MT4Info[i1 - 1].StateStr := StateStrings[STATE_NETWORK_ERROR];
    fmMain.SG1.Cells[1, i1] := fmMain.MT4Info[i1 - 1].StateStr;
  end;
end;

procedure TCheckUpdateThread.UpdateState;
begin
  //
  fmMain.MT4Info[UpdateIndex].State := UpdateStateInt;
  fmMain.MT4Info[UpdateIndex].StateStr := StateStrings[UpdateStateInt];
  fmMain.SG1.Cells[1, UpdateIndex + 1] := fmMain.MT4Info[UpdateIndex].StateStr;
end;

procedure TCheckUpdateThread.CompareFilesInfo;
var
  i1: Integer;
begin
  for i1 := 1 to fmMain.SG1.RowCount - 1 do
  begin
    fmMain.MT4Info[i1 - 1].State := STATE_COMPARING_FILES;
    fmMain.MT4Info[i1 - 1].StateStr := StateStrings[STATE_COMPARING_FILES];
    fmMain.SG1.Cells[1, i1] := fmMain.MT4Info[i1 - 1].StateStr;
  end;
end;

procedure TCheckUpdateThread.Execute;
var
  i1, i2, state : Integer;
  M : TMemoryStream;
  error : boolean;
  fn1, fn2, statestr : string;
  resb, latest : Boolean;
begin
  { Place thread code here }

  while (TRUE) do
  begin

    Sleep(1);

    CreateDir(fmMain.ExeFolder + '\Temp');
    error := FALSE;

    for i1 := 0 to Length(fmMain.FilesToDownload) - 1 do
    begin
      M := TMemoryStream.Create;
      try
      IDHTTP.Get(UPDATE_HOST + '/' + fmMain.FilesToDownload[i1].FileName, M);
      except
        Synchronize(NetworkErrorInfo);
        error := TRUE;
        Break;
      end;
      M.Seek(0, 0);
      M.SaveToFile(fmMain.ExeFolder + '\Temp\' + fmMain.FilesToDownload[i1].FileName);
    end;

    if not error then
    Synchronize(CompareFilesInfo);

    if error then
    Break;

    try

    for i1 := 0 to Length(fmMain.MT4Info) - 1 do
    begin
      latest := TRUE;
      for i2 := 0 to Length(fmMain.FilesToDownload) - 1 do
      begin
        fn1 := fmMain.ExeFolder + '\Temp\' + fmMain.FilesToDownload[i2].FileName;
        fn2 := fmMain.MT4Info[i1].CopyPath + '\' + fmMain.FilesToDownload[i2].Folder + '\' + fmMain.FilesToDownload[i2].FileName;
        resb := CompareTwoFiles(fn1, fn2);
        if not resb then
        begin
          latest := FALSE;
          Break;
        end;
      end;

      if not latest then state := STATE_NEED_UPDATE else state := STATE_LATEST;
      
      //if not latest then statestr := 'Update' else statestr := 'Latest';

      UpdateIndex := i1; UpdateStateInt := state;
      Synchronize(UpdateState);
    end;

    except
      ShowMessage(IntToStr(i1) + ';' + IntToStr(i2) + #13#10 + fn1 + #13#10 + fn2);
    end;

    Break;
  end;

end;



procedure TCheckUpdateThread.Initialize;
begin
    //
end;

end.
