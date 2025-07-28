unit fmExeUpdateU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, IdComponent,
  IdTCPConnection, IdTCPClient, IdHTTP, IdBaseComponent, IdAntiFreezeBase,
  IdAntiFreeze, Vcl.ComCtrls, VCLUnZip, CommonVal, sLabel, Vcl.ExtCtrls;

type
  TfmExeUpdate = class(TForm)
    lbl1: TLabel;
    lbl2: TLabel;
    btnUpdate: TButton;
    btnNotNow: TButton;
    IdHTTP1: TIdHTTP;
    pb1: TProgressBar;
    btnCancel: TButton;
    lbl3: TLabel;
    VCLUnZip1: TVCLUnZip;
    lblMsg: TsLabel;
    Timer1: TTimer;
    procedure FormShow(Sender: TObject);
    procedure btnUpdateClick(Sender: TObject);
    procedure IdHTTP1Work(ASender: TObject; AWorkMode: TWorkMode;
      AWorkCount: Int64);
    procedure btnCancelClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
  private
    { Private declarations }
  public
    ExeSize : Int64;
    TerminatedByUser : boolean;
    { Public declarations }
  end;

var
  fmExeUpdate: TfmExeUpdate;

implementation

{$R *.dfm}

uses fmMainU;

procedure TfmExeUpdate.btnCancelClick(Sender: TObject);
begin
  TerminatedByUser := TRUE;
  IdHTTP1.Disconnect;
  Close;
end;

procedure TfmExeUpdate.btnUpdateClick(Sender : TObject);
var
  exeerror: Boolean;
  M : TMemoryStream;
  S : TStringList;
  sAppName, sOldVerApp : string;

  slSvrIniContents  : TStringList;
  sServerData       : string;
  sVersionFileUrl   : string;

  sZipFile : string;
begin
  TerminatedByUser := FALSE;
  exeerror := FALSE;
  try
    IDHttp1.Head('http://'+fmMain.m_sUpdateUrl + '/'+__MAIN_ZIP);

  except
    exeerror := TRUE;
  end;

  if exeerror then
  begin
    if not TerminatedByUser then
      //ShowMessage(fmMain.ML.GetTranslatedText('ERR_DOWNLOAD_MAINAPP'))
      ShowMessage('Some troubles with retrieving AlphaCopyL.exe !')
    else
      //ShowMessage(fmMain.ML.GetTranslatedText('CANCEL_BY_USER'));
      ShowMessage('Update Terminated by User');
    Close;
    Exit;
  end;

    DeleteFile(fmMain.m_ExeFolder + '\'+fmMain.m_sAppNameWithoutExt+__MAIN_ZIP);

    exesize := IDHttp1.Response.ContentLength;

    btnUpdate.Visible := FALSE;
    btnNotNow.Visible := FALSE;
    btnCancel.Visible := TRUE;
    pb1.Visible := TRUE;
    pb1.Position := 0;
    lbl3.Visible := TRUE;
    lbl3.Caption := '0%';
    M := TMemoryStream.Create;
  try
    //IDHTTP1.Get( 'http://'+fmMain.m_sUpdateUrl + '/'+fmMain.m_sAppName, M);

    sZipFile :=  'http://'+fmMain.m_sUpdateUrl + '/'+__MAIN_ZIP;
    IDHTTP1.Get(sZipFile, M);


  except
    exeerror := TRUE;
  end;


  if (exeerror) or (TerminatedByUser) then
  begin
    if not TerminatedByUser then
      ShowMessage('Some troubles with retrieving AlphaCopyL.exe !')
    else
      //ShowMessage(fmMain.ML.GetTranslatedText('CANCEL_BY_USER'));
      ShowMessage('Update Terminated by User');
    M.Free;
    Close;
    Exit;
  end;

  M.Seek(0, 0);
  M.SaveToFile(fmMain.m_ExeFolder + '\'+__MAIN_ZIP);
  M.Free;

  //S := TStringList.Create;
  //S.Add('TIMEOUT /T 2 /NOBREAK');
  //S.Add('kill /f '+sAppName);
  //S.Add('ping 127.0.0.1 -n 2 > nul ');
  //TODO S.Add('DEL '+sOldVerApp);
  //S.Add('REN '+ sAppName + ' '+ sOldVerApp);
  //S.Add(sOldVerApp);
  //S.SaveToFile(fmMain.m_ExeFolder + '\'+__BAT_RERUN);
  //S.Free;

  Close;
end;

procedure TfmExeUpdate.FormShow(Sender: TObject);
begin
  //lbl1.Caption := fmMain.ML.GetTranslatedText('MAINEXE_CURR_VER') + fmMain.m_sAppCurrVer;
  //lbl2.Caption := fmMain.ML.GetTranslatedText('MAINEXE_NEW_VER') + fmMain.NewVersionStr
  //TODO lblMsg.caption := fmMain.ML.GetTranslatedText('MSG_RERUN_AFTER_UPDATE');

  timer1.interval := 500;
  timer1.enabled  := true;
end;

procedure TfmExeUpdate.IdHTTP1Work(ASender: TObject; AWorkMode: TWorkMode;
  AWorkCount: Int64);
begin
  pb1.Position := Trunc(AworkCount / ExeSize * 100.0);
  lbl3.Caption := IntToStr(pb1.Position) + '%';
end;

procedure TfmExeUpdate.Timer1Timer(Sender: TObject);
begin
  timer1.enabled := false;
  btnUpdateClick(sender);
end;

end.
