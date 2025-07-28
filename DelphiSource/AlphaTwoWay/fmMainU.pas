unit fmMainU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.ComCtrls, System.IOUtils, System.Types,
  WinApi.SHLObj, Vcl.Grids, ShellAPI, IdAntiFreezeBase, IdAntiFreeze,
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdHTTP, IniFiles,
  IdIOHandler, IdIOHandlerStream, CheckUpdateThread, FileCtrl, sSkinManager,
  Vcl.StdCtrls, sComboBoxes, sSkinProvider, sPageControl, sLabel, sPanel, sComboBox
  ,CommonUtils, sButton, acProgressBar, acImage, system.zip, CommonVal
  ,AdvUtil,AdvObj, BaseGrid, AdvGrid
  ,IdExceptionCore, IdCustomTCPServer, IdTCPServer, IdContext
  ,uTwoWayCommon,MTLoggerU, uQueueEx, IdGlobal
  ;

type TMT4Info = record
  ExePath   : string; // C:\Program Files (x86)\OANDA - MetaTrader
  CopyPath  : string; // C:\Users\JAYKIM\AppData\Roaming\MetaQuotes\Terminal\BB16F565FAAA6B23A20C26C49416FF05\MQL4
  Icon      : TIcon;
  StateStr  : string;
  State     : integer;
  sTerminalTp : string;
end;

const
  STATE_GETTING_FILES = 1;
  STATE_NETWORK_ERROR = 2;
  STATE_COMPARING_FILES = 3;
  STATE_LATEST = 4;
  STATE_NEED_UPDATE = 5;

  StateStrings : Array[1..5] of string =
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


  ClientCtx = class(TObject)
  public
    sKey   : string;
  end;

  TSendThrd = class(TThread)
  protected
    procedure Execute();override;
  end;

  TfmMain = class(TForm)
    sPanel2: TsPanel;
    Label1: TLabel;
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
    Label6: TLabel;
    TabSheet2: TTabSheet;
    tmrInitUpdate: TTimer;
    IdAntiFreeze1: TIdAntiFreeze;
    IdHTTP1: TIdHTTP;
    tmrCompMain: TTimer;
    tmrUpdateMain: TTimer;
    btnSvrStart: TButton;
    Panel3: TPanel;
    Label5: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label4: TLabel;
    Label16: TLabel;
    edtBrokerBuy_A: TEdit;
    edtBrokerSell_A: TEdit;
    edtSymbol_A: TEdit;
    btnCnfgSave_A: TButton;
    gdPos_A: TAdvStringGrid;
    edtBaseSpread_A: TEdit;
    edtLvl_1_A: TEdit;
    edtLvl_2_A: TEdit;
    edtOffset_1_A: TEdit;
    edtOffset_2_A: TEdit;
    Label2: TLabel;
    Label3: TLabel;
    Button2: TButton;
    Label7: TLabel;
    edtOpenLots: TEdit;
    Button3: TButton;
    edtKeyBuy_A: TEdit;
    Label13: TLabel;
    Label14: TLabel;
    edtKeySell_A: TEdit;
    edtSpreadMinB: TEdit;
    edtSpreadMinS: TEdit;
    edtSpreadMaxB: TEdit;
    edtSpreadMaxS: TEdit;
    btnCloseAll: TButton;
    edtNetPL: TEdit;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);

    procedure tmrInitUpdateTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure SG1DblClick(Sender: TObject);
    procedure btnCloseClick(Sender: TObject);

    function  Must_Update():boolean;
    procedure Update_MT4Files();
    procedure Update_MainApp();

    procedure UpdateLocalVersionInfo();
    procedure tmrCompMainTimer(Sender: TObject);
    procedure tmrUpdateMainTimer(Sender: TObject);
    procedure btnSvrStartClick(Sender: TObject);
    procedure btnCnfgSave_AClick(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure IdTCPServerException(AContext: TIdContext; AException: Exception);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure Button3Click(Sender: TObject);
    procedure btnCloseAllClick(Sender: TObject);

  /////////////////////////////////////////////////////////////////////////////
  ///  2Way 관련

  private
    m_bReadyToTrade : Boolean;


  /////////////////////////////////////////////////////////////////////////////
  ///  Broker ARRAY 관련
  private
      procedure InitForStart();
      function  Ctx_Get(sKey:string; var ctx:TIdContext):boolean;
      //function  Ctx_Count():integer;

  /////////////////////////////////////////////////////////////////////////////
  ///  IdTCPServer 관련
  private
    IdTcpServer : TIdTCPServer;
    m_SendThrd  : TSendThrd;
    m_bTcpContinue : boolean;
  public
    procedure StartSvr();
    procedure StopSvr();
    procedure IdTCPServerConnect(AContext: TIdContext);
    procedure IdTCPServerDisconnect(AContext: TIdContext);
    procedure IdTCPServerExecute(AContext: TIdContext);
    procedure IdTCPServerStatus(ASender: TObject; const AStatus: TIdStatus;
                                const AStatusText: string);
    procedure ClientDiscon(ASender: TObject);
    procedure ShowNumberOfClients(bDisconn:boolean);
    procedure ResetGrid(iSymbol:integer; iSide:integer);
  public
    procedure AddMsg(sMsg:string; bStress:boolean=false; bShow:boolean=false);

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

    // Grid 갱신을 위해.
    procedure WndProc(var Message: TMessage);override;

    { Private declarations }
  private
    m_log : TMTLogger;
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
    //TODO UpdateThrd : TCheckUpdateThread;

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

    { Public declarations }
  end;

var
  fmMain: TfmMain;


  //////////////////////////////////////////////
  ///  Broker / MT4 Array 관련


implementation

{$R *.dfm}

uses fmExeUpdateU, fmMT4UpdateU, uPacketProcess, uCtrls, uRealPLOrder
      ,ProtoGetU, uAlphaProtocol
    ;


//function  TfmMain.Ctx_Count():integer;
//begin
//
//  try
//    Result := IdTCPServer.Contexts.LockList.Count
//  finally
//    IdTCPServer.Contexts.UnlockList;
//  end;
//end;

function  TfmMain.Ctx_Get(sKey:string; var ctx:TIdContext):boolean;
var
  i   : integer;
  lstConn : TLIST;
  //bConn : boolean;
begin
  Result := False;

  //cnt := IdTCPServer.Contexts.Count;
  lstConn := IdTCPServer.Contexts.LockList;
  try
   for i := 0 to lstConn.Count-1 do
    begin
      if ClientCtx(TIdContext(lstConn[i]).Data).sKey = sKey then
      begin
        ctx := TIdContext(lstConn[i]);
        //bConn := ctx.Connection.Connected;
        Result := true;
        exit;
      end;

    end;
  finally
    IdTCPServer.Contexts.UnlockList;
  end;

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
  //TODO
//  UpdateThrd := TCheckUpdateThread.Create(TRUE);
//  UpdateThrd.IDHTTP := IdHTTP1;
//  UpdateThrd.Start;
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


procedure TfmMain.tmrInitUpdateTimer(Sender: TObject);
var
  i1 : integer;
begin
  tmrInitUpdate.Enabled := FALSE;

  //
  if m_bSecondRun then
  begin
    Del_Old_Temp();
  end;


  //
  CollectMT4Info();

  //TODO
//  if Download_SvrVerIni()=False then
//  BEGIN
//    exit;
//  END;
//
//  // compare server version with local version
//  if CompareWholeVersion()=false then
//  BEGIN
//    //Application.Terminate();
//    exit;
//  END;

//  DeleteFile(m_ExeFolder + '\'+__TEMP_SVR_INI);


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

procedure TfmMain.btnSvrStartClick(Sender: TObject);
begin
  StopSvr;
  StartSvr;
end;

procedure TfmMain.btnCnfgSave_AClick(Sender: TObject);
VAR
  iSymbol : integer;
  //iSide   : integer;
  itemCnfg : TItemCnfg;
  dwRslt   : DWORD;
begin
  if (edtSymbol_A.text='') or
     (edtBrokerBuy_A.text='') or
     (edtBrokerSell_A.text='') or
     (edtBaseSpread_A.text='') or
     (edtLvl_1_A.text='') or
     (edtLvl_2_A.text='') or
     (edtOffSet_1_A.text='') or
     (edtOffSet_2_A.text='') then

  begin
    ShowMessage('Fill all the values');
    exit;
  end;

  iSymbol := 1;

  itemCnfg := TItemCnfg.Create;
  itemCnfg.iSymbol    := iSymbol;
  itemCnfg.keyBuy     := __ctrls[iSymbol].key_buy.Text;
  itemCnfg.keySell    := __ctrls[iSymbol].key_sell.Text;
  itemCnfg.brokerBuy  := __ctrls[iSymbol].broker_buy.Text;
  itemCnfg.brokerSell := __ctrls[iSymbol].broker_sell.Text;

  SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_SAVECNFG,
                              wParam(LongInt(sizeof(itemCnfg))),
                              Lparam(LongInt(itemCnfg)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );

  __CreatRealPLOrder(iSymbol);
end;

procedure TfmMain.ResetGrid(iSymbol:integer; iSide:integer);
VAR
  itemReset : TItemReset;
  dwRslt    : DWORD;
begin

  itemReset := TItemReset.Create;
  itemReset.iSymbol   := iSymbol;
  itemReset.iSide     := iSide;

  SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_RESET,
                              wParam(LongInt(sizeof(itemReset))),
                              Lparam(LongInt(itemReset)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );


//  __gdLock(iSymbol);
//  __ctrls[iSymbol].gdPos.Cells[POS_BROKER,  iSide] := __Broker(iSymbol, iSide);
//  __ctrls[iSymbol].gdPos.Cells[POS_TICKET,  iSide] := '';
//  __ctrls[iSymbol].gdPos.Cells[POS_OPENPRC, iSide] := '';
//  __ctrls[iSymbol].gdPos.Cells[POS_LOTS,    iSide] := '';
//  __ctrls[iSymbol].gdPos.Cells[POS_PL,      iSide] := '';
//  __ctrls[iSymbol].gdPos.Cells[POS_CLR_TP,  iSide] := __ClrTpDesc(CLRTP_NONE);
//  __gdUnLock(iSymbol);

end;

procedure TfmMain.btnCloseAllClick(Sender: TObject);
var
  i   : integer;
  lstConn : TLIST;
begin

  lstConn := IdTCPServer.Contexts.LockList;
  try
   for i := 0 to lstConn.Count-1 do
    begin
      TIdContext(lstConn[i]).Connection.Disconnect;
    end;
  finally
    IdTCPServer.Contexts.UnlockList;
  end;

end;

procedure TfmMain.Button2Click(Sender: TObject);
begin
  m_bReadyToTrade := True;
end;

procedure TfmMain.Button3Click(Sender: TObject);
begin
  ResetGrid(1, IDX_BUY);
  __ctrls[1].spreadMinB.text := '';
  __ctrls[1].spreadMaxB.text := '';
  __ctrls[1].spreadMinS.text := '';
  __ctrls[1].spreadMaxS.text := '';

end;

//TODO
//procedure TfmMain.cbLanguageChange(Sender: TObject);
//begin
//
//  ML.SetActiveLangIndex(cbLanguage.ItemIndex);
//
//  TranslateUI;
//
//end;


procedure TfmMain.CollectMT4Info;
var
  Dirs    : TStringDynArray;
  i1      : integer;
  L       : TStringList;
  MT4Path : string;
  UserPath: string;

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
              //TODO AddMsg(ML.GetTranslatedText('ERR_DOWNLOAD_VERIONINFO'), true, true);
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


  if __QueueEx[Q_RECV]<>nil then
    FreeAndNil(__QueueEx[Q_RECV]);

  if __QueueEx[Q_SEND]<>nil then
    FreeAndNil(__QueueEx[Q_SEND]);

  if assigned(m_log) then
    m_log.SetStop;

end;


procedure TfmMain.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
begin

  StopSvr();

  if __process<>NIL then
    FreeAndNil(__process);
end;

function TfmMain.CompareWholeVersion():boolean;
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
    //TODO AddMsg(ML.GetTranslatedText('ERR_NO_SVR_VERSION_INFO'), true, true);
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
  fn                : string;

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
begin

  m_log := TMTLogger.create(True);
  m_log.Initialize(GetCurrentDir(), ExtractFileName(Application.ExeName));

  m_bReadyToTrade := False;

  __QueueEx[Q_RECV] := nil;
  __QueueEx[Q_SEND] := nil;
  IdTcpServer     := NIL;
  m_SendThrd      := NIL;


  m_bSecondRun := False;
  if ParamCount > 0 then
  begin
    if ParamStr(1) = __PARAM_UPDATE then
      m_bSecondRun := True;
  end;


end;

procedure TfmMain.InitForStart();
begin

  __LinkSettingCtrls();

  __QueueEx[Q_RECV] := CQueueEx.Create;
  __QueueEx[Q_SEND] := CQueueEx.Create;
  __process       := CPacketProcess.create;

end;

procedure TfmMain.FormShow(Sender: TObject);
begin

  m_ExeFolder   := GetCurrentDir;

  InitForStart();

  // Update 되었으면 신규 파일 처리하고 종료
  //if( Run_UpdatedMain()=true ) then
  //  exit;

 //TODO
//  ML := TMultiLanguage.Create;
//  ires := ML.Initialize;
//  if ires<0 then
//  begin
//    AddMsg('No language Files', True, False);
//    Exit;
//  end;
//
//   if ires = 0 then
//  begin
//    SetupLanguageComboBox;
//    TranslateUI;
//  end;


  SG1.Width    := 600;
  SG1.ColCount := 2;
  SG1.ColWidths[0] := 350;
  SG1.ColWidths[1] := 250;

  SG1.RowCount := 1;
  SG1.Cells[0, 0] := 'MT4/5 Terminal at PC';
  SG1.Cells[1, 0] := 'AlphaExperts Version Status';

  //TODO
//  cbLanguage.ItemIndex := 0;
//
//  // read local ini file
//  if( ReadCnfgInfo()=false ) then
//  begin
//    AddMsg(ML.GetTranslatedText('ERR_READ_CONFIG'), True, False);
//    exit;
//  end;


  tmrInitUpdate.Interval := 1000;
  tmrInitUpdate.Enabled  := TRUE;
end;

//
//procedure TfmMain.SetupLanguageComboBox;
//var
//  i1: Integer;
//begin
//  cbLanguage.Clear;
//
//  for i1 := 0 to ML.LangNumber - 1 do
//  begin
//    cbLanguage.Items.Add(ML.GetLanguageName(i1));
//  end;
//end;


procedure  TfmMain.AddMsg(sMsg:string; bStress:boolean; bShow:boolean);
var
  msg:string;
begin
  if bStress then
    msg := format('[%s] !!!==> %s', [__NowHMS(), sMsg])
  else
    msg := format('[%s] %s', [__NowHMS(), sMsg]);

  TThread.Queue(nil, procedure
                     begin
                        cbMsg.Items.Insert(0, msg);
                        cbMsg.ItemIndex := 0;
                     end
               );

  if bShow then
    showmessage(msg);

  m_log.log(INFO, msg);
end;

procedure TfmMain.StartSvr();
begin


  IdTCPServer         := TIdTCPServer.Create(self);
  IdTCPServer.Active  := False;

  IdTCPServer.OnConnect       := IdTCPServerConnect;
  IdTCPServer.OnDisconnect    := IdTCPServerDisconnect;
  IdTCPServer.OnExecute       := IdTCPServerExecute;
  IdTCPServer.OnStatus        := IdTCPServerStatus;

  IdTCPServer.Bindings.Clear;
  IdTCPServer.Bindings.Add.Ip   := '127.0.0.1';
  IdTCPServer.Bindings.Add.Port := 20200;

  IdTCPServer.Active   := True;

  m_SendThrd := TSendThrd.Create;

  m_bTcpContinue := True;

  AddMsg('Server Start');

end;


procedure TfmMain.StopSvr();
begin

  if Assigned(m_SendThrd) then
  begin
    m_SendThrd.Terminate;
    FreeAndNil(m_SendThrd);
  end;


  if Assigned(IdTCPServer) then
  begin

    if IdTCPServer.Active then
      IdTCPServer.Active := False;

    FreeAndNil(IdTCPServer);
  end;
end;

procedure TfmMain.IdTCPServerConnect(AContext: TIdContext);
var
    ip          : string;
    port        : Integer;
    peerIP      : string;
    peerPort    : Integer;
begin
    // ... OnConnect is a TIdServerThreadEvent property that represents the event
    //     handler signalled when a new client connection is connected to the server.

    // ... Use OnConnect to perform actions for the client after it is connected
    //     and prior to execution in the OnExecute event handler.

    // ... see indy doc:
    //     http://www.indyproject.org/sockets/docs/index.en.aspx


    //TODO
//    if m_bReadyToTrade = False then
//    begin
//      AddMsg('Must be ready to trade first!', false, true);
//      //TODO domething ctx.Connection.IOHandler.Write( data);
//      AContext.Connection.Disconnect;
//      exit;
//    end;


    // ... getting IP address and Port of Client that connected
    ip        := AContext.Binding.IP;
    port      := AContext.Binding.Port;
    peerIP    := AContext.Binding.PeerIP;
    peerPort  := AContext.Binding.PeerPort;

    AContext.Connection.OnDisconnected := ClientDiscon;
    AContext.Connection.IOHandler.ReadTimeout := 100;



    // ... message log
    AddMsg('Client Connected!' + 'Port=' + IntToStr(Port)
                      + ' '   + '(PeerIP=' + PeerIP
                      + ' - ' + 'PeerPort=' + IntToStr(PeerPort) + ')'
           );

    // ... display the number of clients connected
    ShowNumberOfClients(false);

end;
// .............................................................................


procedure TfmMain.ClientDiscon(ASender: TObject);
begin
  //showmessage('ClientDiscon');
end;

// *****************************************************************************
//   EVENT : onDisconnect()
//           OCCURS ANY TIME A CLIENT IS DISCONNECTED
// *****************************************************************************
procedure TfmMain.IdTCPServerDisconnect(AContext: TIdContext);
var
  peerIP      : string;

begin

    // ... getting IP address and Port of Client that connected
    peerIP    := AContext.Binding.PeerIP;

    // ... message log
    AddMsg(Format('Client Disconnected(Key:%s)(%s)', [ClientCtx(AContext.Data).sKey, peerIP]));

    // ... display the number of clients connected
    ShowNumberOfClients(true);
end;
// .............................................................................


// *****************************************************************************
//   EVENT : onExecute()
//           ON EXECUTE THREAD CLIENT
// *****************************************************************************
procedure TfmMain.IdTCPServerException(AContext: TIdContext;
  AException: Exception);
begin
  showmessage(' IdTCPServerException');
end;

procedure TfmMain.IdTCPServerExecute(AContext: TIdContext);
var
  msg     : string;
  key     : string;
  clCtx   : ClientCtx;
  code    : string;
begin

  while m_bTcpContinue do
  begin
    Sleep(10);

    try
      msg := '';
      msg := AContext.Connection.IOHandler.ReadLn;
      if length(msg)>0 then
      begin

        code := __PacketCode(msg);
        key  := '';

        // Login 이면 IdContext 저장
        if code=CODE_LOGON then
        begin
          key := __GetValue(msg, FDS_KEY);
          if key<>'' then
          begin
            clCtx         := ClientCtx.Create;
            clCtx.sKey    := key;
            AContext.Data := clCtx;
          end;
        end;

        __QueueEx[Q_RECV].Add(code, code, msg);

        // ... message log
        if code<>CODE_MARKET_DATA then
          AddMsg('[RECV]'+ msg);
      end;

    except
//      IdTCPServer.Contexts.LockList;
//      IdTcpServer.Contexts.Remove(Pointer(AContext));
//      IdTCPServer.Contexts.UnlockList;

      exit;
    end;
  end;


end;
// .............................................................................


// *****************************************************************************
//   EVENT : onStatus()
//           ON STATUS CONNECTION
// *****************************************************************************
procedure TfmMain.IdTCPServerStatus(ASender: TObject; const AStatus: TIdStatus;
                                     const AStatusText: string);
begin
    // ... OnStatus is a TIdStatusEvent property that represents the event handler
    //     triggered when the current connection state is changed...

    // ... message log
    AddMsg('Status:'+AStatusText);
end;

procedure TfmMain.ShowNumberOfClients(bDisconn:boolean);
var
    nClients : integer;
begin

    try
        // ... get number of clients connected
        nClients := IdTCPServer.Contexts.LockList.Count;
    finally
        IdTCPServer.Contexts.UnlockList;
    end;

    // ... client disconnected?
    if bDisconn then dec(nClients);

    AddMsg('Client Count:'+inttostr(nclients));
end;

procedure TfmMain.WndProc(var Message: TMessage);
var
  itemCnfg      : TItemCnfg;
  itemReset     : TItemReset;
  itemPos       : TItemPos;
  itemTsStatus  : TItemTsStatus;
  itemMD        : TItemMD;
  itemSpec      : TitemSpec;
  itemBestPrc   : TItemBestPrc;
  itemSending   : TItemSending;
  iSymbol,iSide : integer;

  dOpenB, dOpenS: double;
begin
  inherited;

  try
    if Message.Msg = WM_GRID_SAVECNFG then
    begin
      itemCnfg := TitemCnfg(Message.LParam);

      iSymbol := itemCnfg.iSymbol;

      __ctrls[iSymbol].gdPos.Cells[POS_KEY, IDX_BUY]      := itemCnfg.keyBuy;
      __ctrls[iSymbol].gdPos.Cells[POS_KEY, IDX_SELL]     := itemCnfg.keySell;
      __ctrls[iSymbol].gdPos.Cells[POS_BROKER, IDX_BUY]   := itemCnfg.brokerBuy;
      __ctrls[iSymbol].gdPos.Cells[POS_BROKER, IDX_SELL]  := itemCnfg.brokerSell;
      __ctrls[iSymbol].gdPos.Cells[POS_TS_LVL_1, IDX_SELL]  := itemCnfg.brokerSell;

      FreeAndNil(itemCnfg);
    end;

    if Message.Msg = WM_GRID_RESET then
    begin
      itemReset := TItemReset(Message.LParam);
      iSymbol := itemReset.iSymbol;
      iSide   := itemReset.iSide;

      __ctrls[iSymbol].gdPos.Cells[POS_BROKER,  iSide] := __Broker(iSymbol, iSide);
      __ctrls[iSymbol].gdPos.Cells[POS_TICKET,  iSide] := '';
      __ctrls[iSymbol].gdPos.Cells[POS_OPENPRC, iSide] := '';
      __ctrls[iSymbol].gdPos.Cells[POS_LOTS,    iSide] := '';
      __ctrls[iSymbol].gdPos.Cells[POS_PL,      iSide] := '';
      __ctrls[iSymbol].gdPos.Cells[POS_CLR_TP,  iSide] := __ClrTpDesc(CLRTP_NONE);
      __ctrls[iSymbol].gdPos.Cells[POS_SENDING, iSide] := 'N';

      FreeAndNil(itemReset);
    end;

    if Message.Msg = WM_GRID_POSITION then
    begin
      itemPos := TItemPos(Message.LParam);

      iSymbol := itemPos.iSymbol;
      iSide   := itemPos.iSide;

      if itemPos.sClrTp = CLRTP_CLOSE then
      begin
        __ctrls[iSymbol].gdPos.Cells[POS_KEY,  iSide]    := __Key(iSymbol, iSide);
        __ctrls[iSymbol].gdPos.Cells[POS_BROKER,  iSide] := __Broker(iSymbol, iSide);
        __ctrls[iSymbol].gdPos.Cells[POS_TICKET,  iSide] := '';
        __ctrls[iSymbol].gdPos.Cells[POS_OPENPRC, iSide] := '';
        __ctrls[iSymbol].gdPos.Cells[POS_LOTS,    iSide] := '';
        __ctrls[iSymbol].gdPos.Cells[POS_PL,      iSide] := '';
        __ctrls[iSymbol].gdPos.Cells[POS_PL_PIP,  iSide] := '';
        __ctrls[iSymbol].gdPos.Cells[POS_CLR_TP,  iSide] := __ClrTpDesc(CLRTP_NONE);
        __ctrls[iSymbol].gdPos.Cells[POS_TS_BEST, iSide] := '';
        __ctrls[iSymbol].gdPos.Cells[POS_PL_STATUS, iSide] := '';
        __ctrls[iSymbol].gdPos.Cells[POS_TS_CUTPRC, iSide] := '';
        __ctrls[iSymbol].gdPos.Cells[POS_SENDING,   iSide] := 'N';
      end
      ELSE
      BEGIN
        __ctrls[iSymbol].gdPos.Cells[POS_LOTS,  iSide]    := itemPos.sLots;
        __ctrls[iSymbol].gdPos.Cells[POS_PL,      iSide]  := itemPos.sPL;
        __ctrls[iSymbol].gdPos.Cells[POS_CLR_TP,  iSide]  := __ClrTpDesc(itemPos.sClrTp);
        __ctrls[iSymbol].gdPos.Cells[POS_OPENPRC, iSide]  := itemPos.sOpenPrc;
        __ctrls[iSymbol].gdPos.Cells[POS_TICKET,  iSide]  := itemPos.sTicket;

        // Open 인 경우는 둘다
        __ctrls[iSymbol].gdPos.Cells[POS_SENDING,   IDX_SELL] := 'N';
        __ctrls[iSymbol].gdPos.Cells[POS_SENDING,   IDX_BUY] := 'N';


        // 내 순 손익
        dOPenB := strtofloatdef(__ctrls[iSymbol].gdPos.Cells[POS_OPENPRC, IDX_BUY], 0);
        dOpenS := strtofloatdef(__ctrls[iSymbol].gdPos.Cells[POS_OPENPRC, IDX_SELL], 0);
        if (dOpenB>0) and (dOpenS>0) then
        begin

          __ctrls[iSymbol].netPL.text := formatfloat('#0.#0', (dOpenS-dOPenB) /__gdPipSize(iSymbol, IDX_BUY));
        end;

      END;



      FreeAndNil(itemPos);
    end;

    if Message.Msg = WM_GRID_TSSTATUS then
    begin
      itemTSStatus := TitemTSStatus(Message.LParam);
      iSymbol := itemTSStatus.iSymbol;
      iSide   := itemTSStatus.iSide;

      __ctrls[iSymbol].gdPos.Cells[POS_PL_STATUS, iSide] := itemTSStatus.sStatus;
      //__ctrls[iSymbol].gdPos.Cells[POS_TS_BEST,   iSide] := itemTSStatus.sTsBest;

      FreeAndNil(itemTSStatus);
    end;

    if Message.Msg = WM_GRID_REAL_MD then
    begin
      itemMD := TitemMD(Message.LParam);
      iSymbol := itemMD.iSymbol;
      iSide   := itemMD.iSide;

      __ctrls[iSymbol].gdPos.Cells[POS_NOWPRC, iSide] := itemMD.sClose;
      __ctrls[iSymbol].gdPos.Cells[POS_SPREAD, iSide] := itemMD.sSpread;
      __ctrls[iSymbol].gdPos.Cells[POS_PL_PIP, iSide] := itemMD.sPlPip;

      FreeAndNil(itemMD);
    end;

    if Message.Msg = WM_GRID_SPEC then
    begin
      itemSpec := TitemSpec(Message.LParam);
      iSymbol := itemSpec.iSymbol;
      iSide   := itemSpec.iSide;

      __ctrls[iSymbol].gdPos.Cells[POS_DECIMAL, iSide] := itemSpec.sDecimal;
      __ctrls[iSymbol].gdPos.Cells[POS_PIPSIZE, iSide] := itemSpec.sPipSize;

      FreeAndNil(itemSpec);
    end;

    if Message.Msg = WM_GRID_BEST_PRC then
    begin
      itemBestPrc := TitemBestPrc(Message.LParam);
      iSymbol := itemBestPrc.iSymbol;
      iSide   := itemBestPrc.iSide;

      __ctrls[iSymbol].gdPos.Cells[POS_TS_BEST, iSide]  := itemBestPrc.sBestPip;
      __ctrls[iSymbol].gdPos.Cells[POS_TS_CUTPRC, iSide] := itemBestPrc.sCutPip;

      FreeAndNil(itemBestPrc);
    end;

    if Message.Msg = WM_GRID_SENDING then
    begin
      itemSending := TitemSending(Message.LParam);
      iSymbol := itemSending.iSymbol;
      iSide   := itemSending.iSide;

      __ctrls[iSymbol].gdPos.Cells[POS_SENDING, iSide]  := 'Y';

      if itemSending.bBoth then
      begin
        if iSide=IDX_BUY then
          __ctrls[iSymbol].gdPos.Cells[POS_SENDING, IDX_SELL]  := 'Y'
        else
          __ctrls[iSymbol].gdPos.Cells[POS_SENDING, IDX_BUY]  := 'Y'
        ;
      end;


      FreeAndNil(itemSending);
    end;


  except

  end;

end;


procedure TSendThrd.Execute;
var
  pItem  : PTQItem;
  nCode  : integer;
  data    : string;

  key     : string;
  ctx     : TIdContext;

  B : TIdBytes;
  dataLen : integer;
begin
  while (not Terminated) and (fmMain.IdTCPServer.Active) do
  begin
    Sleep(10);

    pItem := __QueueEx[Q_SEND].Get();

    if pItem <> NIL then
    begin

      //nCode  := pItem.nCode;
      key    := pItem.sCode;
      data   := pItem.data;
      Dispose(pItem);

      dataLen := length(data);
      SetLength(B, dataLen);
      CopyMemory(Addr(B[0]), Addr(data[1]), dataLen);

      fmMain.AddMsg(format('[Get SendQ](%s)', [data]));
      if fmMain.Ctx_Get(key, ctx) then
      begin
        try
          ctx.Connection.IOHandler.Write(data);
          fmMain.AddMsg(format('[send](%s)', [data]));
        except
          fmMain.AddMsg(format('[send-except](%s)', [key]));
        end;
      end;

    end;

  end;

end;

end.
