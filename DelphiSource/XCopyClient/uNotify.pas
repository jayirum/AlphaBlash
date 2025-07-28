unit uNotify;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, sLabel, Vcl.Buttons,
  sBitBtn, Vcl.ExtCtrls, sPanel, MMSystem;

type
  TNotify = class(TForm)
    sPanel1: TsPanel;
    sPanel2: TsPanel;
    btnClose: TsBitBtn;
    lblTitle: TsLabel;
    lblBody: TsLabel;
    Timer1: TTimer;


    procedure btnCloseClick(Sender: TObject);
    procedure btnCloseKeyPress(Sender: TObject; var Key: Char);
    procedure FormShow(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
  private
    { Private declarations }
    PROCEDURE AdjustPosition();
  private
    m_nSoundCnt : integer;

  public
    { Public declarations }
    m_tp : integer;
    m_id : string;
    m_sWav : string;

    m_title, m_msg : string;

  end;

const
  TP_LOGON  = 1;
  TP_LOGOFF = 2;
  TP_SELL   = 3;
  TP_BUY    = 4;
  TP_SIREN  = 8;
  TP_NONE   = 9;

var
  fmNotify: TNotify;


  procedure __ShowAlarm(tp:integer; id:string);
  procedure __ShowMsg(stitle:string; sMsg:string);
  procedure __ShowWarning(sTitle:string; sMsg:string);

implementation

{$R *.dfm}


procedure __ShowAlarm(tp:integer; id:string);
begin
  fmNotify := TNotify.Create(application);
  //fmNotify.Tile := '알림';
  fmNotify.m_tp := tp;
  fmNotify.m_id := id;
  //fmNotify.Position := poscreencenter;
  fmNotify.Show;

end;

procedure __ShowMsg(stitle:string; sMsg:string);
begin
   fmNotify := TNotify.Create(application);
  //fmNotify.Tile := '알림';
  fmNotify.m_tp := TP_NONE;
  fmNotify.m_title := sTitle;
  fmNotify.m_msg   := sMsg;
  //fmNotify.Position := poscreencenter;
  fmNotify.Show;
end;



procedure __ShowWarning(sTitle:string; sMsg:string);
begin
   fmNotify := TNotify.Create(application);
  fmNotify.m_tp := TP_SIREN;
  fmNotify.m_title := sTitle;
  fmNotify.m_msg   := sMsg;
  fmNotify.Show;
end;



procedure TNotify.btnCloseClick(Sender: TObject);
begin
  timer1.Enabled := false;
  Close;
end;

procedure TNotify.btnCloseKeyPress(Sender: TObject; var Key: Char);
begin
  if key = #13 then Close;
end;

procedure TNotify.FormShow(Sender: TObject);
begin

  m_swav := '';
  m_swav := ExtractFilePath(ParamStr(0));

  if m_tp=TP_NONE then
  BEGIN
    fmNotify.lblTitle.Caption := m_title;
    fmNotify.lblBody.Caption  := m_msg;

    exit;
  END;


  if m_tp=TP_LOGON then
  BEGIN
    fmNotify.lblTitle.Caption := '로그온';
    fmNotify.lblBody.Caption  := format('[%s] 로그인 했습니다. 확인바랍니다.',[m_id]);
    m_swav := m_swav + '\LogOnOff.wav';
  END;
  if m_tp=TP_LOGOFF then
  BEGIN
    fmNotify.lblTitle.Caption := '로그아웃';
    fmNotify.lblBody.Caption  := format('[%s] 로그아웃 했습니다. 확인바랍니다.',[m_id]);
    m_swav := m_swav + '\LogOnOff.wav';
  END;
  if m_tp=TP_SELL then
  BEGIN
    fmNotify.lblTitle.Caption := '체결발생';
    fmNotify.lblBody.Caption  := format('[%s] 매도체결 했습니다. 확인바랍니다.',[m_id]);
    m_swav := m_swav + '\SELL.wav';
  END;
  if m_tp=TP_BUY then
  BEGIN
    fmNotify.lblTitle.Caption := '체결발생';
    fmNotify.lblBody.Caption  := format('[%s] 매수체결 했습니다. 확인바랍니다.',[m_id]);
    m_swav := m_swav + '\buy.wav';
  END;
  if m_tp=TP_SIREN then
  BEGIN
    fmNotify.lblTitle.Caption := '!!!경고!!!';
    fmNotify.lblBody.Caption  := format('%s',[m_id]);
    m_swav := m_swav + '\siren.wav';
  END;


  SndPlaySound (Pchar(m_swav), Snd_Async);

  timer1.Interval := 5000;
  timer1.Enabled := true;

  AdjustPosition();

end;

procedure TNotify.Timer1Timer(Sender: TObject);
begin
  //if m_nSoundCnt=2 then
  //  fmNotify.Hide;

  if m_nSoundCnt = 5 then
  begin
    timer1.Enabled := false;
    fmNotify.close;
    exit;
  end;

  SndPlaySound (Pchar(m_swav), Snd_Async);

  m_nSoundCnt := m_nSoundCnt + 1;
end;



PROCEDURE TNotify.AdjustPosition();
begin
  Left := Screen.Monitors[0].WorkareaRect.Width - Width;
  Top :=  Screen.Monitors[0].WorkareaRect.Height - Height;

end;


end.
