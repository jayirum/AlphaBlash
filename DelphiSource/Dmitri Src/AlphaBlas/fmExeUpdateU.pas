unit fmExeUpdateU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, IdComponent,
  IdTCPConnection, IdTCPClient, IdHTTP, IdBaseComponent, IdAntiFreezeBase,
  IdAntiFreeze, Vcl.ComCtrls;

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
    procedure FormShow(Sender: TObject);
    procedure btnUpdateClick(Sender: TObject);
    procedure IdHTTP1Work(ASender: TObject; AWorkMode: TWorkMode;
      AWorkCount: Int64);
    procedure btnCancelClick(Sender: TObject);
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
end;

procedure TfmExeUpdate.btnUpdateClick(Sender : TObject);
var
  exeerror: Boolean;
  M : TMemoryStream;
  S : TStringList;
begin
  TerminatedByUser := FALSE;
  exeerror := FALSE;
  try
  IDHttp1.Head(UPDATE_HOST + '/AlphaBlash.exe');
  except
    exeerror := TRUE;
  end;

  if exeerror then
  begin
    if not TerminatedByUser then
    ShowMessage('Some troubles with retrieving AlphaBlash.exe !')
    else ShowMessage('Update terminated by user !');
    Close;
    Exit;
  end;

  DeleteFile(fmMain.ExeFolder + '\AlphaBlash__.exe');

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
  IDHTTP1.Get(UPDATE_HOST + '/AlphaBlash.exe', M);
  except
    exeerror := TRUE;
  end;


  if (exeerror) or (TerminatedByUser) then
  begin
    if not TerminatedByUser then
    ShowMessage('Some troubles with retrieving AlphaBlash.exe !')
    else ShowMessage('Update terminated by user !');
    M.Free;
    Close;
    Exit;
  end;

  M.Seek(0, 0);
  M.SaveToFile(fmMain.ExeFolder + '\AlphaBlash__.exe');

  M.Free;

  S := TStringList.Create;
  S.Add('TIMEOUT /T 2 /NOBREAK');
  S.Add('DEL AlphaBlash.exe');
  S.Add('COPY AlphaBlash__.exe AlphaBlash.exe /Y');
  S.Add('DEL AlphaBlash__.exe');
  S.Add('AlphaBlash.exe');
  S.SaveToFile(fmMain.ExeFolder + '\update.bat');
  S.Free;

  Close;
end;

procedure TfmExeUpdate.FormShow(Sender: TObject);
begin
lbl1.Caption := 'Current Version is : ' + fmMain.CurrentVersionStr;
lbl2.Caption := 'New Version available : ' + fmMain.NewVersionStr
end;

procedure TfmExeUpdate.IdHTTP1Work(ASender: TObject; AWorkMode: TWorkMode;
  AWorkCount: Int64);
begin
  pb1.Position := Trunc(AworkCount / ExeSize * 100.0);
  lbl3.Caption := IntToStr(pb1.Position) + '%';
end;

end.
