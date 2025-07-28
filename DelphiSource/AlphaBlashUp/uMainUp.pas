unit uMainUp;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.ComCtrls, System.IOUtils, System.Types,
  WinApi.SHLObj, Vcl.Grids, ShellAPI, IdAntiFreezeBase, IdAntiFreeze,

  FileCtrl, sSkinManager,
  Vcl.StdCtrls, sComboBoxes, sSkinProvider, sPageControl, sLabel, sPanel, sComboBox
  ,CommonUtils, sButton, acProgressBar, acImage, system.zip, CommonVal
  ;


type
  TfmMainUp = class(TForm)
    sPanel2: TsPanel;
    sPanel1: TsPanel;
    sSkinManager1: TsSkinManager;
    sSkinProvider1: TsSkinProvider;
    Timer1: TTimer;
    Timer2: TTimer;
    Label1: TLabel;
    Label2: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure Timer1Timer(Sender: TObject);
    procedure Timer2Timer(Sender: TObject);

  public
    function Run_UpdatedMain():boolean;


  private

    { Private declarations }
    m_sMainExe : string;
  public
    m_ExeFolder : string;

    { Public declarations }
  end;

var
  fmMainUp: TfmMainUp;

implementation

{$R *.dfm}



procedure TfmMainUp.FormClose(Sender: TObject; var Action: TCloseAction);
begin
//    if UT.Started then
//    UT.Suspend;
//  UT.Terminate;
//  Sleep(500);
//  UT.Free;
end;



function TfmMainUp.Run_UpdatedMain():boolean;
var
  sExeName  : string;
  zip       : TZipFile;
  zipfileName : string;
begin

  //TODO __KillProcess(__MAIN_EXE);


  zipfileName := m_ExeFolder + '\'+ __MAIN_ZIP;
  zip := TZipFile.Create;
  try

    zip.Open(zipfileName, zmRead);
    zip.ExtractAll();
    zip.Close;
    zip.Free;

    timer2.Interval := 1000;
    timer2.Enabled := true;
  except
    showmessage('Except in unzipping');
  end;

  Result := true;
end;

procedure TfmMainUp.Timer1Timer(Sender: TObject);
begin

  timer1.Enabled := False;
  if( Run_UpdatedMain()=true ) then
    exit;

end;

procedure TfmMainUp.Timer2Timer(Sender: TObject);
var
 sExeName : string;

begin

  timer2.Enabled := false;
  sExeName := m_ExeFolder + '\'+m_sMainExe;
  ShellExecute(Handle, PWidechar('open'), PWideChar(sExeName), PChar(__PARAM_UPDATE), nil, SW_SHOWNORMAL);
  Application.Terminate;

end;

procedure TfmMainUp.FormCreate(Sender: TObject);
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

  // this line must ahead of the ires := ML.Initialize;
  m_ExeFolder   := GetCurrentDir;
  m_sMainExe    := ParamStr(1);

  timer1.Interval := 1000;
  timer1.Enabled := true;


end;

procedure TfmMainUp.FormShow(Sender: TObject);
var
  i1:integer;
begin


end;


end.
