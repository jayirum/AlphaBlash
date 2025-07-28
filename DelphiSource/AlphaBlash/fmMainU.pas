unit fmMainU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.ComCtrls, System.IOUtils, System.Types,
  WinApi.SHLObj, Vcl.Grids, ShellAPI, IdAntiFreezeBase, IdAntiFreeze,
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdHTTP, IniFiles,
  IdIOHandler, IdIOHandlerStream, CheckUpdateThread, FileCtrl, sSkinManager,
  Vcl.StdCtrls, sComboBoxes, sSkinProvider, sPageControl, sLabel, sPanel, sComboBox
  ,CommonUtils, sButton, acProgressBar, acImage, MultiLanguageU, system.zip, CommonVal
  ;

type TMT4Info = record
  ExePath   : string; // C:\Program Files (x86)\OANDA - MetaTrader
  CopyPath  : string; // C:\Users\JAYKIM\AppData\Roaming\MetaQuotes\Terminal\BB16F565FAAA6B23A20C26C49416FF05\MQL4
  Icon      : TIcon;
  StateStr  : string;
  State     : integer;
  sTerminalTp : string;
end;

const STATE_GETTING_FILES = 1;
const STATE_NETWORK_ERROR = 2;
const STATE_COMPARING_FILES = 3;
const STATE_LATEST = 4;
const STATE_NEED_UPDATE = 5;

const StateStrings : Array[1..5] of string =
('',
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

//const UPDATE_HOST = 'http://project2020.fun';
//const UPDATE_HOST = 'http://update.bullash.com';


type
  TfmMain = class(TForm)
    sPanel2: TsPanel;
    Label1: TLabel;
    slabel3: TLabel;
    cbLanguage: TComboBox;
    tmrInit: TTimer;
    IdAntiFreeze1: TIdAntiFreeze;
    IdHTTP1: TIdHTTP;
    tmrCompMain: TTimer;
    tmrUpdateMain: TTimer;
    PageControl2: TPageControl;
    tabUpdate: TTabSheet;
    Panel1: TPanel;
    lblBeSure: TLabel;
    lblDownload: TLabel;
    SG1: TStringGrid;
    pbDownload: TProgressBar;
    btnClose: TButton;
    Panel2: TPanel;
    cbMsg: TComboBox;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure tmrInitTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure SG1DblClick(Sender: TObject);
    procedure btnCloseClick(Sender: TObject);

    function  Must_Update():boolean;
    procedure Update_MT4Files();
    procedure Update_MainApp();

    procedure cbLanguageChange(Sender: TObject);

    procedure TranslateUI;
    procedure SetupLanguageComboBox;

    procedure UpdateLocalVersionInfo();
    procedure tmrCompMainTimer(Sender: TObject);
    procedure tmrUpdateMainTimer(Sender: TObject);


  public
    procedure AddMsg(sMsg:string; bStress:boolean; bShow:boolean);

  private
    procedure CollectMT4Info;
    function GetSpecialFolderPath(CSIDLFolder: Integer): string;
    function GetExeIcon(FileName: string): TIcon;
    procedure DelFilesFromDir(Directory, FileMask: string; DelSubDirs: Boolean);
    function  ReadCnfgInfo():boolean;

    function  CompareWholeVersion( ):boolean;
    //procedure CompareAppVersion();
    function  Download_SvrVerIni():boolean;

    procedure Del_Old_Temp();

    { Private declarations }
  public
    MT4Info : Array of TMT4Info;
    FilesToDownload : Array of TFilesToDownload;
    VerMajor, VerMinor, VerBuild : cardinal;
    IniFileRead : boolean;
    MajorInt, MinorInt, BuildInt : integer;
    m_ExeFolder : string;
    m_Mt4DownloadFolder: string;
    ExeVersionRetrieved: Boolean;
    m_sAppCurrVer: string;
    NewVersionStr: string;
    //m_bNewExeAvailable: Boolean;
    UpdateThrd : TCheckUpdateThread;

    m_sConfigFile         : string;
    m_sAppName            : string;
    m_sAppNameWithoutExt  : string;
    // server info
    m_sUpdateUrl    : string;
    m_sVersionFile  : string;
    m_sWholeVersion : string;
    m_sSvrWholeVersion : string;
    m_bNeedUpdate   : boolean;
    m_bSecondRun      : boolean;    // AlphaBlashUp.exe 에서 update 후 실행하는 경우
    m_SvrVerIni     : TIniFile;

    ML : TMultiLanguage;

    { Public declarations }
  end;

var
  fmMain: TfmMain;

implementation

{$R *.dfm}

uses fmExeUpdateU, fmMT4UpdateU;


procedure TfmMain.TranslateUI;
var
  i1: Integer;
begin
  //Caption := ML.GetTranslatedText('FORM_CAPTION');
  tabUpdate.Caption   := ML.GetTranslatedText('UPDATE_SHEET_NAME');
  lblBeSure.Caption   := ML.GetTranslatedText('BE_SURE_CAPTION');
  slabel3.Caption     := ML.GetTranslatedText('LANGUAGE_WORD');
  lblDownload.Caption := ML.GetTranslatedText('DOWNLOAD_PROGRESS');
end;

function  TfmMain.Must_Update():boolean;
begin
  Result := m_bNeedUpdate;
end;

function  TfmMain.ReadCnfgInfo():boolean;
begin
  Result := false;

  m_sConfigFile := __Get_CFGFileName();


  m_sUpdateUrl  := __Get_CFGFile('UPDATE_INFO', 'UPDATE_URL', '', True, m_sConfigFile);
  if m_sUpdateUrl='' then
    exit;

  m_sVersionFile := __Get_CFGFile('UPDATE_INFO', 'VER_FILE', '', True, m_sConfigFile);


  //m_sWholeVersion := __Get_CFGFile('VERSION', 'VERSION', '', True, m_sConfigFile);

  m_sAppName            := __ExtractAppName(ParamStr(0));
  m_sAppNameWithoutExt  := __ExtractAppNameWithoutExt(ParamStr(0));
  Result := true;

end;

procedure TfmMain.UpdateLocalVersionInfo();
begin

  __Set_CFGFile('VERSION', 'VERSION', m_sSvrWholeVersion, True, m_sConfigFile);

end;


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

  fmMT4Update.m_MT4Index := index;
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


procedure TfmMain.Update_MT4Files();
begin

  UpdateThrd := TCheckUpdateThread.Create(TRUE);
  UpdateThrd.IDHTTP := IdHTTP1;
  UpdateThrd.Start;
end;


// CheckUpdateThread 에서 MT4 Files Update 완료 후 호출한다.
procedure TfmMain.Update_MainApp();
begin

  tmrUpdateMain.Interval := 500;
  tmrUpdateMain.Enabled  := true;

end;


procedure TfmMain.tmrUpdateMainTimer(Sender: TObject);
begin
  tmrUpdateMain.Enabled := False;

  if not Must_Update() then
    exit;

  DeleteFile(fmMain.m_ExeFolder + '\'+__MAIN_ZIP);

  // download a zipfile for main files
  fmExeUpdate.ShowModal;

  // 사용자가 Update 하면 update.zip 파일이 생성된다.
  if FileExists(fmMain.m_ExeFolder + '\'+ __MAIN_ZIP ) then
  begin
    ShellExecute(Handle, PWidechar('open'), PWideChar('AlphaBlashUp.exe'),
                  nil, nil, SW_HIDE);
    Application.Terminate;
  end;

 Caption := m_sAppNameWithoutExt+' Ver. ' + IntToStr(VerMajor) + '.' +
 IntToStr(VerMinor) + '.' + IntToStr(VerBuild) +
 ' (Update Ver. ' + NewVersionStr + ' is available)';

end;

procedure TfmMain.tmrCompMainTimer(Sender: TObject);
begin

  tmrCompMain.Enabled   := false;

  Update_MT4Files()


end;


procedure TfmMain.Del_Old_Temp();
VAR
  sTmp, sUp : string;
begin
  // temp.ini
  DeleteFile(m_ExeFolder + '\'+__TEMP_SVR_INI);

  // zip
  DeleteFile(fmMain.m_ExeFolder + '\'+__MAIN_ZIP);

  // AlphaBlashUp.exe, tmpAlphaBlashUp.exe
  if FileExists(m_ExeFolder+'\tmp'+__UP_EXE) then
  begin

    sTmp  := ExtractFilePath(ParamStr(0)) + 'tmp' + __UP_EXE;
    sUp   := ExtractFilePath(ParamStr(0)) + __UP_EXE;

    DeleteFile(sUp);

    RenameFile( sTmp, sUp);

  end;

  
end;


procedure TfmMain.tmrInitTimer(Sender: TObject);
var
  i1 : integer;
begin
  tmrInit.Enabled := FALSE;

  //
  if m_bSecondRun then
  begin
    Del_Old_Temp();
  end;


  //
  CollectMT4Info();

  //
  if Download_SvrVerIni()=False then
  BEGIN
    AddMsg(ML.GetTranslatedText('ERR_DOWNLOAD_VERIONINFO'), True, True);
    exit;
  END;

  // compare server version with local version
  if CompareWholeVersion()=false then
  BEGIN
    //Application.Terminate();
    exit;
  END;

  DeleteFile(m_ExeFolder + '\'+__TEMP_SVR_INI);


  // The version of this application itself.
  //별도관리하지 않는다. 버전 통합관리(CompareWholeVersion). CompareAppVersion();


  // draw grid with MT4 info
  SG1.RowCount := 1 + Length(MT4Info);
  for i1 := 0 to High(MT4Info) do
  begin
    SG1.Cells[0, 1 + i1] := MT4Info[i1].ExePath;
    SG1.Cells[1, 1 + i1] := StateStrings[MT4Info[i1].State];
  end;


  tmrCompMain.Interval  := 500;
  tmrCompMain.Enabled   := true;

end;


procedure TfmMain.btnCloseClick(Sender: TObject);
begin
  Close;
end;

procedure TfmMain.cbLanguageChange(Sender: TObject);
begin

  ML.SetActiveLangIndex(cbLanguage.ItemIndex);

  TranslateUI;

end;


procedure TfmMain.CollectMT4Info;
var
  Dirs    : TStringDynArray;
  i1      : integer;
  L       : TStringList;
  MT4Path : string;
  s       : string;
  UserPath: string;
  sTerminalTp : string;
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

          UserPath := Dirs[i1] + '\MQL4';
          if DirectoryExists(UserPath) then
          begin
            MT4Info[Length(MT4Info) - 1].sTerminalTp := TERMINAL_TP_MT4;
            MT4Info[Length(MT4Info) - 1].CopyPath := UserPath
          end
          else
          begin
            UserPath := Dirs[i1] + '\MQL5';
            if DirectoryExists(UserPath) then
            begin
              MT4Info[Length(MT4Info) - 1].sTerminalTp := TERMINAL_TP_MT5;
              MT4Info[Length(MT4Info) - 1].CopyPath := UserPath;
            end
            else
            BEGIN
              AddMsg(ML.GetTranslatedText('ERR_DOWNLOAD_VERIONINFO'), true, true);
              exit;
            END;
          end;

          MT4Info[Length(MT4Info) - 1].Icon := TIcon.Create;
          if FileExists(MT4Info[Length(MT4Info) - 1].ExePath + '\terminal.ico') then
          MT4Info[Length(MT4Info) - 1].Icon.LoadFromFile(MT4Info[Length(MT4Info) - 1].ExePath + '\terminal.ico');

          MT4Info[Length(MT4Info) - 1].State    := STATE_GETTING_FILES;
          MT4Info[Length(MT4Info) - 1].StateStr := StateStrings[STATE_GETTING_FILES];

        end;
      end;
      L.Free;
    end;
  end;

end;

procedure TfmMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
//    if UpdateThrd.Started then
//    UpdateThrd.Suspend;
//  UpdateThrd.Terminate;
//  Sleep(500);
//  UpdateThrd.Free;
end;


function TfmMain.CompareWholeVersion():boolean;
var
  sParam : string;
begin
  Result        := True;
  m_bNeedUpdate := True;

  // local
  m_sWholeVersion := __Get_CFGFile('VERSION', 'VERSION', '', True, m_sConfigFile);

  // server
  m_sSvrWholeVersion := m_SvrVerIni.ReadString('VERSION', 'VERSION', '');

  // AlphaBlashUp.exe 가 update 후 실행하는 경우 UPDATE 라는 파라미터가 전달된다.
  if m_bSecondRun then
  begin
    m_bNeedUpdate := False;
    // local ini 의 version 을 update 한다.
    UpdateLocalVersionInfo();
    exit;
  end;


  if m_sSvrWholeVersion ='' then
  BEGIN
    AddMsg(ML.GetTranslatedText('ERR_NO_SVR_VERSION_INFO'), true, true);
    Result := false;
    exit;
  END;

  // DownLoad 파일들이 없으면 최초 1회는 무조건 받는다.
  if not DirectoryExists(fmMain.m_Mt4DownloadFolder) then
  begin
    CreateDir(fmMain.m_Mt4DownloadFolder);
    m_bNeedUpdate   := True;
    m_sWholeVersion := '0';
  end
  else
  begin
    if strtofloat(m_sSvrWholeVersion) <= strtofloatdef(m_sWholeVersion,0) then
    begin
      m_bNeedUpdate := false;
    end
    else
    begin

      // Downloads 폴더 내 파일들은 모두 지우고 폴더를 새로 생성한다.
      TDirectory.Delete(fmMain.m_ExeFolder + '\'+ __DOWNLOADS_FOLDER, True);
      //DelFilesFromDir(fmMain.m_Mt4DownloadFolder, '*.*', TRUE);
      //RemoveDirectory(PWideChar(fmMain.m_Mt4DownloadFolder));
      CreateDir(fmMain.m_Mt4DownloadFolder);
    end;
  end;

end;

function  TfmMain.Download_SvrVerIni():boolean;
var
  slSvrIniContents  : TStringList;
  sServerData       : string;
  sVersionFileUrl   : string;

  sTempIni : string;
  IniSections       : Array of string;
  i1, i2            : integer;
  filesnumber       : integer;
  status, fn        : string;
begin
  slSvrIniContents    := TStringList.Create;
  IniFileRead         := TRUE;
  IDHTTP1.ReadTimeout := 3000;

  try
    sVersionFileUrl := 'http://'+m_sUpdateUrl +'/'+ m_sVersionFile;
    sServerData     := IDHTTP1.Get(sVersionFileUrl);
  except
    IniFileRead := FALSE;
    Result := False;
    exit;
  end;



  m_ExeFolder   := GetCurrentDir;
  m_Mt4DownloadFolder  := m_ExeFolder + '\'+__DOWNLOADS_FOLDER;

  //status := IDHTTP1.ResponseText;

  slSvrIniContents.Text := sServerData; // version.ini

  sTempIni := m_ExeFolder + '\'+__TEMP_SVR_INI;
  DeleteFile(sTempIni);
  slSvrIniContents.SaveToFile(sTempIni);
  slSvrIniContents.Free;

  m_SvrVerIni := TIniFile.Create(sTempIni);

  SetLength(FilesToDownload, 0);
  SetLength(IniSections, 3);
  IniSections[0] := 'EXPERTS';
  IniSections[1] := 'FILES';
  IniSections[2] := 'LIBRARIES';

  for i1 := 0 to 2 do
  begin
    filesnumber := m_SvrVerIni.ReadInteger(IniSections[i1], 'FilesNumber', 0);
    for i2 := 1 to filesnumber do
    begin
      fn := m_SvrVerIni.ReadString(IniSections[i1], 'FileName' + IntToStr(i2), '');
      if fn <> '' then
      begin
         SetLength(FilesToDownload, Length(FilesToDownload) + 1);
         FilesToDownload[Length(FilesToDownload) - 1].FileName    := fn;
         FilesToDownload[Length(FilesToDownload) - 1].Folder      := IniSections[i1];
         FilesToDownload[Length(FilesToDownload) - 1].Downloaded  := FALSE;
         FilesToDownload[Length(FilesToDownload) - 1].Error       := FALSE;
      end;
    end;
  end;

  //DeleteFile(sTempIni);

  Result := True;
end;



procedure TfmMain.FormCreate(Sender: TObject);
var
  status, fn  : string;
  i1: Integer;
  IniSections       : Array of string;
  filesnumber       : integer;
  i2: Integer;
  sVersionFileUrl   : string;
  bUpdate           : boolean;

  var ires : integer;
begin

  m_bSecondRun := False;
  if ParamCount > 0 then
  begin
    if ParamStr(1) = __PARAM_UPDATE then
      m_bSecondRun := True;
  end;



end;


procedure TfmMain.FormShow(Sender: TObject);
var
  i1:integer;
  ires : integer;
begin

  m_ExeFolder   := GetCurrentDir;

  // Update 되었으면 신규 파일 처리하고 종료
  //if( Run_UpdatedMain()=true ) then
  //  exit;


  ML := TMultiLanguage.Create;
  ires := ML.Initialize;
  if ires<0 then
  begin
    AddMsg('No language Files', True, False);
    Exit;
  end;

   if ires = 0 then
  begin
    SetupLanguageComboBox;
    TranslateUI;
  end;


  SG1.Width    := 600;
  SG1.ColCount := 2;
  SG1.ColWidths[0] := 350;
  SG1.ColWidths[1] := 250;

  SG1.RowCount := 1;
  SG1.Cells[0, 0] := 'MT4/5 Terminal at PC';
  SG1.Cells[1, 0] := 'AlphaExperts Version Status';

  cbLanguage.ItemIndex := 0;

  // read local ini file
  if( ReadCnfgInfo()=false ) then
  begin
    AddMsg(ML.GetTranslatedText('ERR_READ_CONFIG'), True, False);
    exit;
  end;


  tmrInit.Interval := 1000;
  tmrInit.Enabled  := TRUE;
end;


procedure TfmMain.SetupLanguageComboBox;
var
  i1: Integer;
begin
  cbLanguage.Clear;

  for i1 := 0 to ML.LangNumber - 1 do
  begin
    cbLanguage.Items.Add(ML.GetLanguageName(i1));
  end;
end;


procedure  TfmMain.AddMsg(sMsg:string; bStress:boolean; bShow:boolean);
var
  msg:string;
begin
  if bStress then
    msg := format('[%s] !!!==> %s', [__NowHMS(), sMsg])
  else
    msg := format('[%s] %s', [__NowHMS(), sMsg]);

  cbMsg.Items.Insert(0, msg);
  cbMsg.ItemIndex := 0;

  if bShow then
    showmessage(msg);

end;

end.
