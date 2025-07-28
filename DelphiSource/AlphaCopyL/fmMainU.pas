unit fmMainU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.ComCtrls, System.IOUtils, System.Types,
  WinApi.SHLObj, Vcl.Grids, ShellAPI, IdAntiFreezeBase, IdAntiFreeze,
  IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient, IdHTTP, IniFiles,
  IdIOHandler, IdIOHandlerStream, CheckUpdateThread, FileCtrl, sSkinManager,
  Vcl.StdCtrls, AdvUtil, AdvObj, BaseGrid, AdvGrid, sPanel,
  CommonUtils, MultiLanguageU, system.zip, CommonVal,
  uLocalCommon, MTLoggerU, IdCustomTCPServer, IdTCPServer
  ;

type TMT4Info = record
  idx       : integer;
  ExePath   : string; // C:\Program Files (x86)\OANDA - MetaTrader
  CopyPath  : string; // C:\Users\JAYKIM\AppData\Roaming\MetaQuotes\Terminal\BB16F565FAAA6B23A20C26C49416FF05\MQL4
  Icon      : TIcon;
  StateStr  : string;
  State     : integer;
  sTerminalTp : string;
  Alias      : string;
  MCTp       : string;  //M:master, C:Copier
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
    pgMain: TPageControl;
    tabMainTerminal: TTabSheet;
    pnlBg1: TPanel;
    lblBeSure: TLabel;
    lblDownload: TLabel;
    pbDownload: TProgressBar;
    Panel2: TPanel;
    cbMsg: TComboBox;
    btnClose: TButton;
    tabMainMC: TTabSheet;
    gdTerminal: TAdvStringGrid;
    pnlBg2: TPanel;
    tabMainCfg: TTabSheet;
    pnlBg3: TPanel;
    gdMT4List: TAdvStringGrid;
    gdCopier: TAdvStringGrid;
    btnMaster2List: TButton;
    btnList2Master: TButton;
    btnCopier2List: TButton;
    btnList2Copier: TButton;
    gdMaster: TAdvStringGrid;
    gdCfgMT4: TAdvStringGrid;
    btnSaveMS: TButton;
    lblMsgWait: TLabel;
    btnRefreshMC: TButton;
    pgCfg: TPageControl;
    tabMaster: TTabSheet;
    Label10: TLabel;
    lblSymbolMCnt: TLabel;
    Label9: TLabel;
    edtSymbolAdd: TEdit;
    btnSymbolAdd: TButton;
    edtDebug: TEdit;
    gdSymbolM: TAdvStringGrid;
    btnSaveMasterSymbol: TButton;
    GroupBox1: TGroupBox;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    tabCopierSymbols: TTabSheet;
    pnlCfgC: TPanel;
    Label12: TLabel;
    lblCopierAlias: TLabel;
    Label2: TLabel;
    lblSymbolTotM: TLabel;
    Label4: TLabel;
    lblSymbolTotC: TLabel;
    Label13: TLabel;
    lblMasterAlias: TLabel;
    gdSymbolC: TAdvStringGrid;
    btnSaveSymbolC: TButton;
    gdSymbolM2: TAdvStringGrid;
    tabCopierCfg: TTabSheet;
    ScrollBox1: TScrollBox;
    GroupBox4: TGroupBox;
    GroupBox3: TGroupBox;
    lblSLSameDist: TLabel;
    GroupBox2: TGroupBox;
    GroupBox5: TGroupBox;
    edtMultiplier: TEdit;
    rdoFixedLots: TRadioButton;
    edtFixedLots: TEdit;
    chkMaxOneOrd: TCheckBox;
    edtMaxOneOrd: TEdit;
    chkMaxTotOrd: TCheckBox;
    edtMaxTotOrd: TEdit;
    rdoMultiplier: TRadioButton;
    chkSL: TCheckBox;
    chkTP: TCheckBox;
    rdoSLSamePrc: TRadioButton;
    rdoSLSameDist: TRadioButton;
    GroupBox6: TGroupBox;
    chkSlippage: TCheckBox;
    edtSlippage: TEdit;
    Label11: TLabel;
    rdoTrade: TRadioButton;
    rdoSignal: TRadioButton;
    btnSaveCopierOptions: TButton;
    btnLoadCOptions: TButton;
    btnClrCSymbols: TButton;
    GroupBox7: TGroupBox;
    chkMktOpen: TCheckBox;
    chkMktClose: TCheckBox;
    GroupBox8: TGroupBox;
    chkLimitOrd: TCheckBox;
    chkStopOrd: TCheckBox;
    idSvr: TIdTCPServer;
    GroupBox9: TGroupBox;
    chkNonCopyTimeout: TCheckBox;
    Label3: TLabel;
    edtNonCopyMins: TEdit;
    Label5: TLabel;
    Label14: TLabel;
    chkUseMCode: TCheckBox;
    procedure FormCreate(Sender: TObject);
    procedure InitCtrls();
    procedure FormShow(Sender: TObject);
    procedure tmrInitTimer(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure btnCloseClick(Sender: TObject);

    function  Must_Update():boolean;
    procedure Update_MT4Files();
    procedure Update_MainApp();

    //procedure cbLanguageChange(Sender: TObject);

    procedure TranslateUI;
    //TODO procedure SetupLanguageComboBox;

    procedure UpdateLocalVersionInfo();
    procedure tmrCompMainTimer(Sender: TObject);
    procedure tmrUpdateMainTimer(Sender: TObject);
    procedure gdTerminalGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdTerminalDblClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure pgMainChange(Sender: TObject);
    procedure gdMasterGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdMT4ListGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdCopierGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure btnMaster2ListClick(Sender: TObject);
    procedure gdMasterClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure gdMT4ListClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure gdCopierClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure btnList2MasterClick(Sender: TObject);
    procedure btnCopier2ListClick(Sender: TObject);
    procedure btnList2CopierClick(Sender: TObject);
    procedure btnSaveMSClick(Sender: TObject);
    procedure gdCfgMT4ClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure gdSymbolMDblClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure gdSymbolMGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdCfgMT4GetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure btnSymbolAddClick(Sender: TObject);
    procedure btnSaveMasterSymbolClick(Sender: TObject);
    procedure edtSymbolAddKeyPress(Sender: TObject; var Key: Char);
    procedure gdSymbolM2DblClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure gdSymbolCDblClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure btnRefreshMCClick(Sender: TObject);
    procedure gdSymbolCGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure gdSymbolCGetCellColor(Sender: TObject; ARow, ACol: Integer;
      AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
    procedure gdSymbolCClickCell(Sender: TObject; ARow, ACol: Integer);
    procedure Button1Click(Sender: TObject);
    procedure btnSaveSymbolCClick(Sender: TObject);
    procedure pgCfgChange(Sender: TObject);
    procedure tabMasterShow(Sender: TObject);
    procedure tabCopierSymbolsShow(Sender: TObject);
    procedure tabCopierCfgShow(Sender: TObject);
    procedure tabMainCfgShow(Sender: TObject);
    procedure tabMainMCShow(Sender: TObject);
    procedure rdoTradeClick(Sender: TObject);
    procedure rdoSignalClick(Sender: TObject);
    procedure rdoSLSamePrcClick(Sender: TObject);
    procedure rdoSLSameDistClick(Sender: TObject);
    procedure lblSLSameDistClick(Sender: TObject);
    procedure rdoMultiplierClick(Sender: TObject);
    procedure rdoFixedLotsClick(Sender: TObject);
    procedure chkMktOpenClick(Sender: TObject);
    procedure chkMaxOneOrdClick(Sender: TObject);
    procedure chkMaxTotOrdClick(Sender: TObject);
    procedure chkSlippageClick(Sender: TObject);
    procedure lblSlippageClick(Sender: TObject);
    procedure btnClrCSymbolsClick(Sender: TObject);
    procedure btnLoadCOptionsClick(Sender: TObject);
    procedure btnSaveCopierOptionsClick(Sender: TObject);
    procedure chkUseMCodeClick(Sender: TObject);


  public
    procedure AddMsg(sMsg:string; bStress:boolean=false; bShow:boolean=false);
    procedure ShowUpdateForm(ARow:integer);


  private

    function  IS_READY():boolean;
    procedure SET_READY_DONE();   // 모든 준비가 완료되면 한번만 호출
    procedure Init_OtherClasses();

    procedure CollectMT4Info;
    procedure Read_EAConfig_MCTp();
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
    MT4Info         : Array of TMT4Info;
    FilesToDownload : Array of TFilesToDownload;

    VerMajor, VerMinor, VerBuild : cardinal;
    //IniFileRead : boolean;
    //MajorInt, MinorInt, BuildInt : integer;
    m_ExeFolder         : string;
    m_Mt4DownloadFolder : string;
    ExeVersionRetrieved : Boolean;
    //m_sAppCurrVer: string;
    //NewVersionStr: string;
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

    m_bReady        : boolean;
    //TODO ML : TMultiLanguage;

    { Public declarations }
  end;

var
  fmMain: TfmMain;
  __log : TMTLogger;

implementation

{$R *.dfm}

uses fmExeUpdateU, fmMT4UpdateU, uMasterCopierTab, uConfigTab, uEAConfigFile
    ,uQueueEx, uTcpSvr, uRecvDataProc
    ;





procedure TfmMain.SET_READY_DONE();
begin
  m_bReady := true;

  Init_OtherClasses();
end;


procedure TfmMain.Init_OtherClasses();
var
  sIp, sPort:string;
  sIni : string;
begin

  __CreateMCTabClass();
  __CreateCfgTabClass();

  __mc.Init;
  __cfg.Init;
  __cfg.Reload_LeftMT4List();

  pgMain.ActivePage := tabMainterminal;

  sIni := m_sAppNameWithoutExt+'.ini';

  sIp   := __Get_INIFile(sIni, SEC_NETWORK, 'LISTEN_IP');
  sPort := __Get_INIFile(sIni, SEC_NETWORK, 'LISTEN_PORT');

  if __CreateSvrSocket(sIp, strtoint(sPort))=True then
  begin
    AddMsg(Format('Create Sever Socket(%s)(%s)',[sIp, sPort]));
  end;

  __CreateRecvDataProc();


end;



procedure TfmMain.TranslateUI;
var
  i1: Integer;
begin
  //Caption := ML.GetTranslatedText('FORM_CAPTION');
  //tabTerminals.Caption   := 'Setting'; //ML.GetTranslatedText('UPDATE_SHEET_NAME');
  lblBeSure.Caption   := '>> Please make sure AlphaExperts are latest <<';  //ML.GetTranslatedText('BE_SURE_CAPTION');
  slabel3.Caption     := 'Language'; //ML.GetTranslatedText('LANGUAGE_WORD');
  lblDownload.Caption := 'Downloading Progress';  //ML.GetTranslatedText('DOWNLOAD_PROGRESS');
end;

function  TfmMain.Must_Update():boolean;
begin
  Result := m_bNeedUpdate;
end;


function  TfmMain.IS_READY():boolean;
BEGIN
  Result := m_bReady;
END;


procedure TfmMain.pgMainChange(Sender: TObject);
begin

  if ((Sender as TPageControl).ActivePage = tabMainTerminal) then
  begin
    //_ShowHide(TAB_TERMINAL, True);
  end
  else if ((Sender as TPageControl).ActivePage = tabMainMC) then
  begin
    __mc.Reload_MasterAndCopiers();
  end
  else if ((Sender as TPageControl).ActivePage = tabMainCfg) then
  begin
    __mc.Reload_MasterAndCopiers();
    pgCfg.ActivePage := tabMaster;
    __cfg.Init();
  end
end;



procedure TfmMain.rdoFixedLotsClick(Sender: TObject);
begin
  rdoMultiplier.checked := false;
  rdoFixedLots.checked  := true;

  edtMultiplier.Clear;
  edtMultiplier.ReadOnly  := not rdoMultiplier.checked;
  edtFixedLots.ReadOnly   := not rdoFixedLots.checked;
end;

procedure TfmMain.rdoMultiplierClick(Sender: TObject);
begin
  rdoMultiplier.checked := true;
  rdoFixedLots.checked  := false;

  edtMultiplier.ReadOnly  := not rdoMultiplier.checked;
  edtFixedLots.ReadOnly   := not rdoFixedLots.checked;
  edtFixedLots.Clear;

end;

procedure TfmMain.rdoSignalClick(Sender: TObject);
begin
  rdoTrade.checked := False;
  rdoSignal.checked := True;
end;

procedure TfmMain.rdoSLSameDistClick(Sender: TObject);
begin
  rdoSLSamePrc.Checked  := False;
  rdoSLSameDist.Checked := True;
end;

procedure TfmMain.rdoSLSamePrcClick(Sender: TObject);
begin
  rdoSLSamePrc.Checked  := True;
  rdoSLSameDist.Checked := False;
end;

procedure TfmMain.rdoTradeClick(Sender: TObject);
begin
  rdoTrade.Checked := True;
  rdoSignal.checked := False;
end;

procedure TfmMain.pgCfgChange(Sender: TObject);
begin

  if __cfg.Is_MT4List_selected=False then
  begin
    AddMsg('Please choose one MT4 from left grid', false, false);
    exit;
  end;


  if pgCfg.ActivePage = tabMaster then
  begin
    if not __cfg.IsMatchedMCType(True) then
    begin
      AddMsg('This MT4 is not Master', false, true);
      pgCfg.ActivePage := tabCopierSymbols;
      exit;
    end;

    __cfg.MastreTab_LoadSymbols();

  end
  else if pgCfg.ActivePage = tabCopierSymbols then
  begin

    if not __cfg.IsMatchedMCType(False) then
    begin
      AddMsg('This MT4 is not Copier', false, true);
      pgCfg.ActivePage := tabMaster;
      exit;
    end;

    __cfg.CSymTab_Show();
  end
  else if pgCfg.ActivePage = tabCopierCfg then
  begin

    if not __cfg.IsMatchedMCType(False) then
    begin
      AddMsg('This MT4 is not Copier', false, true);
      pgCfg.ActivePage := tabCopierSymbols;
      exit;
    end;

    __cfg.COptionTab_Show();

  end
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

procedure TfmMain.ShowUpdateForm(ARow:integer);
begin

  if MT4Info[ARow].State <> STATE_NEED_UPDATE then Exit;

  fmMT4Update.m_MT4Index := ARow;
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

  if Must_Update() then
  begin

    DeleteFile(fmMain.m_ExeFolder + '\'+__MAIN_ZIP);

    // download a zipfile for main files
    fmExeUpdate.ShowModal;

    // 사용자가 Update 하면 update.zip 파일이 생성된다.
    if FileExists(fmMain.m_ExeFolder + '\'+ __MAIN_ZIP ) then
    begin

      // AlphaBlashUp.exe 를 호출하면서 프로세스 종료.
      // ==> 이 application upgrade 를 위해
      ShellExecute(Handle, PWidechar('open'), PWideChar(__UP_EXE),
                    PWidechar(m_sAppName), nil, SW_HIDE);
      Application.Terminate;
    end;

  // Caption := m_sAppNameWithoutExt+' Ver. ' + IntToStr(VerMajor) + '.' +
  // IntToStr(VerMinor) + '.' + IntToStr(VerBuild) +
  // ' (Update Ver. ' + NewVersionStr + ' is available)';
  end;



  SET_READY_DONE();


end;

procedure TfmMain.tabCopierCfgShow(Sender: TObject);
begin
  showmessage('3');
end;

procedure TfmMain.tabCopierSymbolsShow(Sender: TObject);
begin
  //__cfg.CSymTab_Show();
end;

procedure TfmMain.tabMainCfgShow(Sender: TObject);
begin
  __mc.Init;
  __cfg.Init;
end;

procedure TfmMain.tabMainMCShow(Sender: TObject);
begin
  __mc.Init;
end;

procedure TfmMain.tabMasterShow(Sender: TObject);
begin
  //__cfg.MastreTab_LoadSymbols();
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

  Read_EAConfig_MCTp();
  //


  if Download_SvrVerIni()=False then
  BEGIN
    //AddMsg(ML.GetTranslatedText('ERR_DOWNLOAD_VERIONINFO'), True, True);
    AddMsg('Failed to download version info', True, true);
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
  gdTerminal.RowCount := 1 + Length(MT4Info);
  for i1 := 0 to High(MT4Info) do
  begin
    gdTerminal.Cells[GDTERMINAL_PATH, 1 + i1]   := MT4Info[i1].ExePath;
    gdTerminal.Cells[GDTERMINAL_STATUS, 1 + i1] := StateStrings[MT4Info[i1].State];
    gdTerminal.Cells[GDTERMINAL_ALIAS, 1 + i1]  := __GetAlias(MT4Info[i1].ExePath);
    gdTerminal.Cells[GDTERMINAL_MCTP, 1 + i1]   := __MCTpDesc(MT4Info[i1].MCTp);
  end;



  tmrCompMain.Interval  := 500;
  tmrCompMain.Enabled   := true;

end;



procedure TfmMain.gdMasterClickCell(Sender: TObject; ARow, ACol: Integer);
begin
  __mc.SelectRow(MC_MASTER, Arow);
end;

procedure TfmMain.gdMasterGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  HAlign := tacenter;
end;

procedure TfmMain.gdMT4ListClickCell(Sender: TObject; ARow, ACol: Integer);
begin
  __mc.SelectRow(MC_LIST, Arow);
end;

procedure TfmMain.gdMT4ListGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  HAlign := tacenter;
end;

procedure TfmMain.gdSymbolCClickCell(Sender: TObject; ARow, ACol: Integer);
begin
  if Arow=0 then
    exit;

  if ACol=2 then
  begin
    __cfg.CSymTab_RemoveSymbol(ARow);
  end;
end;

procedure TfmMain.gdSymbolCDblClickCell(Sender: TObject; ARow, ACol: Integer);
begin
  //__cfg.Copier_AddSymbol(Arow);
end;

procedure TfmMain.gdSymbolCGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  Halign := taCenter;
end;

procedure TfmMain.gdSymbolCGetCellColor(Sender: TObject; ARow, ACol: Integer;
  AState: TGridDrawState; ABrush: TBrush; AFont: TFont);
begin
  if Arow=0 then
    exit;

  if ACol=2 then
  begin
    AFont.Color := clRed;
    AFont.Style := [fsBold];
  end;
end;

procedure TfmMain.gdSymbolM2DblClickCell(Sender: TObject; ARow, ACol: Integer);
begin
  __cfg.CSymTab_AddSymbol(Arow);
end;

procedure TfmMain.gdSymbolMDblClickCell(Sender: TObject; ARow, ACol: Integer);
begin

  __cfg.MasterTab_RemoveSymbol(ARow);
end;

procedure TfmMain.gdSymbolMGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  HAlign := taCenter;
end;

procedure TfmMain.gdCfgMT4ClickCell(Sender: TObject; ARow, ACol: Integer);
begin
  __cfg.Select_MT4_fromLeft(ARow);
end;

procedure TfmMain.gdCfgMT4GetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  HAlign := taCenter;
end;

procedure TfmMain.gdCopierClickCell(Sender: TObject; ARow, ACol: Integer);
begin
  __mc.SelectRow(MC_COPIER, Arow);
end;

procedure TfmMain.gdCopierGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  HAlign := tacenter;
end;

procedure TfmMain.gdTerminalDblClickCell(Sender: TObject; ARow, ACol: Integer);
begin
  if Arow = 0 then
    exit;

    ShowUpdateForm(arow);
end;

procedure TfmMain.gdTerminalGetAlignment(Sender: TObject; ARow,
  ACol: Integer; var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  if ARow = 0 then
    HAlign := taCenter
  else
  begin
    if ACol<>0 then
      HAlign := taCenter;
  end;
end;

procedure TfmMain.btnCloseClick(Sender: TObject);
begin
  Close;
end;

procedure TfmMain.btnClrCSymbolsClick(Sender: TObject);
begin
  __cfg.CSymTab_ClrAll();
end;

procedure TfmMain.btnList2MasterClick(Sender: TObject);
begin
  __mc.Move(MC_LIST, MC_MASTER);
end;

procedure TfmMain.btnLoadCOptionsClick(Sender: TObject);
begin
  __cfg.COptionTab_LoadOptions();
end;

procedure TfmMain.btnList2CopierClick(Sender: TObject);
begin
  __mc.Move(MC_LIST, MC_COPIER);
end;

procedure TfmMain.btnMaster2ListClick(Sender: TObject);
begin
  __mc.Move(MC_MASTER, MC_LIST);
end;

procedure TfmMain.btnRefreshMCClick(Sender: TObject);
begin
  __mc.Init;
end;

procedure TfmMain.btnSaveMSClick(Sender: TObject);
begin

  __mc.Save();

end;

procedure TfmMain.btnSaveSymbolCClick(Sender: TObject);
begin
  __cfg.CSymTab_SaveSymbol();
end;

procedure TfmMain.btnSymbolAddClick(Sender: TObject);
begin
  __cfg.MasterTab_AddSymbol(edtSymbolAdd.Text);
end;

procedure TfmMain.Button1Click(Sender: TObject);
begin
  __cfg.CSymTab_LoadAllSymbols;
end;

procedure TfmMain.chkMaxOneOrdClick(Sender: TObject);
begin
  edtMaxOneOrd.ReadOnly := not chkMaxOneOrd.Checked;
end;

procedure TfmMain.chkMaxTotOrdClick(Sender: TObject);
begin
  edtMaxTotOrd.ReadOnly := not chkMaxTotOrd.Checked;
end;

procedure TfmMain.chkMktOpenClick(Sender: TObject);
begin
  if chkMktOpen.Checked = False then
    chkMktOpen.Checked := True;
end;


procedure TfmMain.chkSlippageClick(Sender: TObject);
begin
  if chkSlippage.Checked = False then
    edtSlippage.Clear;

  edtSlippage.ReadOnly := not chkSlippage.Checked;
end;


procedure TfmMain.chkUseMCodeClick(Sender: TObject);
begin
  __cfg.CSymTab_UseMasterCode(chkUseMCode.Checked);
end;

procedure TfmMain.lblSlippageClick(Sender: TObject);
begin
  chkSlippage.Checked := not chkSlippage.Checked;
end;



procedure TfmMain.btnSaveCopierOptionsClick(Sender: TObject);
begin
  __cfg.COptionTab_Save();
end;

procedure TfmMain.btnSaveMasterSymbolClick(Sender: TObject);
begin

  __cfg.MasterTab_SaveSymbol;

end;

procedure TfmMain.btnCopier2ListClick(Sender: TObject);
begin
  __mc.Move(MC_COPIER, MC_LIST);
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


{
  EA Config 파일 읽어서 MC_TP 가져온다.
}
procedure TfmMain.Read_EAConfig_MCTp();
var
  i   : integer;
  val : string;
begin
  for i := 0 to __TerminalCnt()-1 do
  begin
    val := __EACnfg_Get(MT4Info[i].CopyPath, SEC_MC_TP, 'MC_TP');
    MT4Info[i].MCTp := val;
  end;

end;


{
  MT4 사용자폴더 Looping 돌면서 정보 조회
}
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
            MT4Info[Length(MT4Info) - 1].idx := Length(MT4Info) - 1;
            MT4Info[Length(MT4Info) - 1].sTerminalTp := TERMINAL_TP_MT4;
            MT4Info[Length(MT4Info) - 1].CopyPath := UserPath;
            MT4Info[Length(MT4Info) - 1].Alias   := __GetAlias(MT4Path);
          end
          else
          begin
            UserPath := Dirs[i1] + '\MQL5';
            if DirectoryExists(UserPath) then
            begin
              MT4Info[Length(MT4Info) - 1].idx := Length(MT4Info) - 1;
              MT4Info[Length(MT4Info) - 1].sTerminalTp := TERMINAL_TP_MT5;
              MT4Info[Length(MT4Info) - 1].CopyPath := UserPath;
              MT4Info[Length(MT4Info) - 1].Alias   := __GetAlias(MT4Path);
            end
            else
            BEGIN
              //AddMsg(ML.GetTranslatedText('ERR_DOWNLOAD_VERIONINFO'), true, true);
              AddMsg('Failed to download version info', true, true);
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

  if assigned(__mc) then
    FreeAndNil(__mc);

  if assigned(__cfg) then
    FreeAndNil(__cfg);

  if assigned(__tcpSvr) then
    __tcpSvr.StopSvr();

  if assigned(__log) then
    __log.SetStop;

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
    //AddMsg(ML.GetTranslatedText('ERR_NO_SVR_VERSION_INFO'), true, true);
    AddMsg('The server does not have version info', true, true);
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
  //IniFileRead         := TRUE;
  IDHTTP1.ReadTimeout := 3000;

  try
    sVersionFileUrl := 'http://'+m_sUpdateUrl +'/'+ m_sVersionFile;
    sServerData     := IDHTTP1.Get(sVersionFileUrl);
  except
    //IniFileRead := FALSE;
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



procedure TfmMain.edtSymbolAddKeyPress(Sender: TObject; var Key: Char);
begin
  if key = #13 then
    btnSymbolAddClick(sender);
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

  m_bNeedUpdate := false;
  m_bSecondRun  := false;
  m_bReady      := false;


  __log := TMTLogger.create(True);
  __log.Initialize(GetCurrentDir(), ExtractFileName(Application.ExeName));

  if ParamCount > 0 then
  begin
    if ParamStr(1) = __PARAM_UPDATE then
      m_bSecondRun := True;
  end;


  InitCtrls();

end;

procedure TfmMain.InitCtrls();
var
  i1      : integer;
begin

  //__PageCtrlInit();



  gdMT4List.Width := 120;
  gdMaster.Width  := gdMT4List.Width;
  gdCopier.Width  := gdMT4List.Width;
  gdSymbolM.Width := gdMT4List.Width;
  gdCfgMT4.Width  := gdMT4List.Width;


  gdMT4List.ColWidths[0]  := gdMT4List.Width-5;
  gdMaster.ColWidths[0]   := gdMaster.Width-5;
  gdCopier.ColWidths[0]   := gdCopier.Width-5;
  gdSymbolM.ColWidths[0]  := gdSymbolM.Width-5;
  gdCfgMT4.ColWidths[0]  := gdSymbolM.Width-5;

  gdSymbolM2.Width := 80;
  gdSymbolM2.ColWidths[0] := gdSymbolM2.Width - 5;

  gdSymbolC.Width := 220;
  gdSymbolC.ColWidths[0] := 75;
  gdSymbolC.ColWidths[1] := 75;
  gdSymbolC.ColWidths[2] := 60;


//  __gdMC[MC_MASTER] := @gdMaster;
//  __gdMC[MC_LIST]   := @gdMT4List;
//  __gdMC[MC_COPIER] := @gdCopier;
//
//  __gdMC[MC_MASTER].RowCount  := MAX_MT4_COUNT;
//  __gdMC[MC_LIST].RowCount    := MAX_MT4_COUNT;
//  __gdMC[MC_COPIER].RowCount  := MAX_MT4_COUNT;

//  __CreateMCTabClass();
//  __CreateCfgTabClass();


end;


procedure TfmMain.lblSLSameDistClick(Sender: TObject);
begin
  rdoSLSameDistClick(sender);
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


  cbLanguage.ItemIndex := 0;

  // read local ini file
  if( ReadCnfgInfo()=false ) then
  begin
    //AddMsg(ML.GetTranslatedText('ERR_READ_CONFIG'), True, False);
    AddMsg('Fail to read config file',True, False);
    exit;
  end;


  tmrInit.Interval := 1000;
  tmrInit.Enabled  := TRUE;
end;


//TODO
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


procedure  TfmMain.AddMsg(sMsg:string; bStress:boolean=false; bShow:boolean=false);
var
  msg: string;
  tm : string;
begin
  tm := format('[%s]', [__NowHMS()]);

  if bStress then
    msg := tm + '!!!==> ' + sMsg
  else
    msg := tm + sMsg
  ;

  cbMsg.Items.Insert(0, msg);
  cbMsg.ItemIndex := 0;

  __log.log(INFO, msg);

  if bShow then
    showmessage(sMsg);

end;

end.
