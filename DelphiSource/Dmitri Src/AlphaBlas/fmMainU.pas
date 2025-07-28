unit fmMainU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.ComCtrls, System.IOUtils, System.Types,
  WinApi.SHLObj, Vcl.Grids, ShellAPI, IdAntiFreezeBase, IdAntiFreeze,
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdHTTP, IniFiles,
  IdIOHandler, IdIOHandlerStream, CheckUpdateThread, FileCtrl;

type TMT4Info = record
  ExePath : string;
  CopyPath : string;
  Icon : TIcon;
  StateStr : string;
  State : integer;
end;

const STATE_GETTING_FILES = 1;
const STATE_NETWORK_ERROR = 2;
const STATE_COMPARING_FILES = 3;
const STATE_LATEST = 4;
const STATE_NEED_UPDATE = 5;

const StateStrings : Array[1..5] of string =
('In progress (getting files) ...',
'Network error ...',
'In progress (comparing files) ...',
'Latest',
'Update available (double-click to update)'
);

type TFilesToDownload = record
  Folder : string;
  FileName : string;
  Downloaded : boolean;
  Error : boolean;
end;

const UPDATE_HOST = 'http://project2020.fun';


type
  TfmMain = class(TForm)
    PageControl1: TPageControl;
    tsUpdate: TTabSheet;
    tsLogin: TTabSheet;
    Panel1: TPanel;
    SG1: TStringGrid;
    IdHTTP1: TIdHTTP;
    IdAntiFreeze1: TIdAntiFreeze;
    Timer1: TTimer;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure SG1DblClick(Sender: TObject);
  private
    procedure CollectMT4Info;
    function GetSpecialFolderPath(CSIDLFolder: Integer): string;
    function GetExeIcon(FileName: string): TIcon;
    procedure DelFilesFromDir(Directory, FileMask: string; DelSubDirs: Boolean);
    { Private declarations }
  public
    MT4Info : Array of TMT4Info;
    FilesToDownload : Array of TFilesToDownload;
    VerMajor, VerMinor, VerBuild : cardinal;
    IniFileRead : boolean;
    MajorInt, MinorInt, BuildInt : integer;
    ExeFolder: string;
    ExeVersionRetrieved: Boolean;
    CurrentVersionStr: string;
    NewVersionStr: string;
    NewExeAvailable: Boolean;
    UT : TCheckUpdateThread;
    { Public declarations }
  end;

var
  fmMain: TfmMain;

implementation

{$R *.dfm}

uses fmExeUpdateU, fmMT4UpdateU;

function TfmMain.GetExeIcon(FileName : string) : TIcon;
var IconIndex: word;
  Buffer: array[0..2048] of char;
  IconHandle: HIcon;
  Bitmap : TBitmap;
begin
      StrCopy(@Buffer, PChar(FileName));
      IconIndex := 0;
      IconHandle := ExtractAssociatedIcon(HInstance, Buffer, IconIndex);
      if IconHandle <> 0 then
        Icon.Handle := IconHandle;
      Bitmap := TBitmap.Create;
      try
        Bitmap.Width := Icon.Width;
        Bitmap.Height := Icon.Height;
        Bitmap.Canvas.Draw(0, 0, Icon);
        //    SpeedButton1.Glyph.Assign(Bitmap);
      finally
        Bitmap.Free;
      end;
end;

function TfmMain.GetSpecialFolderPath(CSIDLFolder: Integer): string;
var
   FilePath: array [0..MAX_PATH] of char;
begin
  SHGetFolderPath(0, CSIDLFolder, 0, 0, FilePath);
  Result := FilePath;
end;

procedure TfmMain.SG1DblClick(Sender: TObject);
var selectedrow, index : integer;
begin
  selectedrow := SG1.Row;
  index := selectedrow - 1;
  if index < 0 then Exit;

  if MT4Info[index].State <> STATE_NEED_UPDATE then Exit;

  fmMT4Update.MT4Index := index;
  fmMT4Update.ShowModal;

end;

procedure TfmMain.DelFilesFromDir(Directory, FileMask: string; DelSubDirs: Boolean);
var
  SourceLst: string;
  FOS: TSHFileOpStruct;
begin
  FillChar(FOS, SizeOf(FOS), 0);
  FOS.Wnd := Application.MainForm.Handle;
  FOS.wFunc := FO_DELETE;
  SourceLst := Directory + '\' + FileMask + #0;
  FOS.pFrom := PChar(SourceLst);
  if not DelSubDirs then
    FOS.fFlags := FOS.fFlags OR FOF_FILESONLY;
  // Remove the next line if you want a confirmation dialog box
  FOS.fFlags := FOS.fFlags OR FOF_NOCONFIRMATION;
  // Uncomment the next line for a "silent operation" (no progress box)
  // FOS.fFlags := FOS.fFlags OR FOF_SILENT;
  SHFileOperation(FOS);
end;

procedure TfmMain.Timer1Timer(Sender: TObject);
begin
  Timer1.Enabled := FALSE;

  if NewExeAvailable then
  begin
    DeleteFile(fmMain.ExeFolder + '\update.bat');
    fmExeUpdate.ShowModal;
    if FileExists(fmMain.ExeFolder + '\update.bat') then
    begin
      ShellExecute(Handle, PWidechar('open'), PWideChar(fmMain.ExeFolder + '\update.bat'),
      nil, nil, SW_HIDE);
      Application.Terminate;
    end;

   Caption := 'AlphaBlash Ver. ' + IntToStr(VerMajor) + '.' +
   IntToStr(VerMinor) + '.' + IntToStr(VerBuild) +
   ' (Update Ver. ' + NewVersionStr + ' is available)';

  end;

  // before to run thread
  // we need to clear Temp folder

  if DirectoryExists(fmMain.ExeFolder + '\Temp') then
  begin
    //TDirectory.Delete(fmMain.ExeFolder + '\Temp');
    DelFilesFromDir(fmMain.ExeFolder + '\Temp', '*.*', TRUE);
    RemoveDirectory(PWideChar(fmMain.ExeFolder + '\Temp'));
  end;



  UT := TCheckUpdateThread.Create(TRUE);
  UT.IDHTTP := IdHTTP1;
  UT.Start;

end;

procedure TfmMain.CollectMT4Info;
var  Dirs : TStringDynArray;
    i1 : integer;
    L : TStringList;
    MT4Path : string;
  s: string;
begin
  SetLength(MT4Info, 0);
  Dirs := TDirectory.GetDirectories(GetSpecialFolderPath(CSIDL_APPDATA) + '\MetaQuotes\Terminal\');

  for i1 := 0 to High(Dirs) do
  begin
    if FileExists(Dirs[i1] + '\origin.txt') then
    begin
      L := TStringList.Create;
      L.LoadFromFile(Dirs[i1] + '\origin.txt');
      if L.Count > 0 then
      begin
        MT4Path := L.Strings[0];
        if (DirectoryExists(MT4Path)) and ((Pos('webinstall', LowerCase(MT4Path)) <= 0))  then
        begin
          SetLength(MT4Info, Length(MT4Info) + 1);
          MT4Info[Length(MT4Info) - 1].ExePath := MT4Path;
          MT4Info[Length(MT4Info) - 1].CopyPath := Dirs[i1] + '\MQL4';
          MT4Info[Length(MT4Info) - 1].Icon := TIcon.Create;
          if FileExists(MT4Info[Length(MT4Info) - 1].ExePath + '\terminal.ico') then
          MT4Info[Length(MT4Info) - 1].Icon.LoadFromFile(MT4Info[Length(MT4Info) - 1].ExePath + '\terminal.ico');
          MT4Info[Length(MT4Info) - 1].State := STATE_GETTING_FILES;
          MT4Info[Length(MT4Info) - 1].StateStr := 'In progress (getting files) ...';
        end;
      end;
      L.Free;
    end;
  end;

  (*s := '';
  for i1 := 0 to High(Dirs) do
  begin
    s := s + Dirs[i1] + #13#10;
  end;   *)

  //ShowMessage(s);

  SG1.ColCount := 2;
  SG1.ColWidths[0] := 300;
  SG1.ColWidths[1] := 300;

  SG1.RowCount := 1 + Length(MT4Info);
  SG1.Cells[0, 0] := 'MT4 Terminal at PC';
  SG1.Cells[1, 0] := 'Alpha Status';

  for i1 := 0 to High(MT4Info) do
  begin
    SG1.Cells[0, 1 + i1] := MT4Info[i1].ExePath;
    SG1.Cells[1, 1 + i1] := StateStrings[MT4Info[i1].State];
  end;



end;

procedure TfmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
//    if UT.Started then
//    UT.Suspend;
//  UT.Terminate;
//  Sleep(500);
//  UT.Free;
end;

procedure TfmMain.FormCreate(Sender: TObject);
var S : TStringList;
    T : TIniFile;
    s1, status, fn : string;
  i1: Integer;
  IniSections : Array of string;
  filesnumber : integer;
  i2: Integer;
begin
  CollectMT4Info;

  GetProductVersion(Application.ExeName, VerMajor, VerMinor, VerBuild);
  Caption := 'AlphaBlash Ver. ' + IntToStr(VerMajor) + '.' +
  IntToStr(VerMinor) + '.' + IntToStr(VerBuild);

  CurrentVersionStr := IntToStr(VerMajor) + '.' +
  IntToStr(VerMinor) + '.' + IntToStr(VerBuild);

  S := TStringList.Create;
  IniFileRead := TRUE;
  NewExeAvailable := FALSE;
  IDHTTP1.ReadTimeout := 3000;
  try
  s1 := IDHTTP1.Get(UPDATE_HOST + '/version.ini');
  except
    IniFileRead := FALSE;
  end;

  ExeFolder := GetCurrentDir;
  DeleteFile(fmMain.ExeFolder + '\update.bat');

  //status := IDHTTP1.ResponseText;

  if not IniFileRead then Exit;
  S.Text := s1;
  DeleteFile(ExeFolder + '\temp.ini');
  S.SaveToFile(ExeFolder + '\temp.ini');
  S.Free;
  T := TIniFile.Create(ExeFolder + '\temp.ini');
  MajorInt := T.ReadInteger('EXEVERSION', 'Major', -1);
  MinorInt := T.ReadInteger('EXEVERSION', 'Minor', -1);
  BuildInt := T.ReadInteger('EXEVERSION', 'Build', -1);


  // reading other files  to download

  SetLength(FilesToDownload, 0);
  SetLength(IniSections, 3);
  IniSections[0] := 'EXPERTS';
  IniSections[1] := 'FILES';
  IniSections[2] := 'LIBRARIES';

  for i1 := 0 to 2 do
  begin
    filesnumber := T.ReadInteger(IniSections[i1], 'FilesNumber', 0);
    for i2 := 1 to filesnumber do
    begin
      fn := T.ReadString(IniSections[i1], 'FileName' + IntToStr(i2), '');
      if fn <> '' then
      begin
         SetLength(FilesToDownload, Length(FilesToDownload) + 1);
         FilesToDownload[Length(FilesToDownload) - 1].FileName := fn;
         FilesToDownload[Length(FilesToDownload) - 1].Folder := IniSections[i1];
         FilesToDownload[Length(FilesToDownload) - 1].Downloaded := FALSE;
         FilesToDownload[Length(FilesToDownload) - 1].Error := FALSE;
      end;
    end;
  end;

  T.Free;


  DeleteFile(ExeFolder + '\temp.ini');
  DeleteFile(ExeFolder + '\AlphaBlash__.exe');
  DeleteFile(ExeFolder + '\update.bat');

  ExeVersionRetrieved := (MajorInt <> -1) and (MinorInt <> -1) and (BuildInt <> -1);

  if ExeVersionRetrieved then
  begin

    if (MajorInt > VerMajor) or
    ((MajorInt = VerMajor) and (MinorInt > VerMinor)) or
    ((MajorInt = VerMajor) and (MinorInt = VerMinor) and (BuildInt > VerBuild))
    then
    begin
        NewVersionStr := IntToStr(MajorInt) + '.' +
        IntToStr(MinorInt) + '.' + IntToStr(BuildInt);
        //ShowMess`age('Current version is : ' +  CurrentVersionStr + ' , update (' +
        //NewVersionStr + ') is available');
        NewExeAvailable := TRUE;
    end;

  end;

end;

procedure TfmMain.FormShow(Sender: TObject);
begin
  Timer1.Enabled := TRUE;
end;

end.
