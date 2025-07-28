unit uNotify;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, sLabel, Vcl.Buttons,
  sBitBtn, Vcl.ExtCtrls, sPanel, MMSystem;

type
  TNotify = class(TForm)
    Timer1: TTimer;
    Panel1: TPanel;
    lblTitle: TLabel;
    Panel2: TPanel;
    lblBody: TLabel;
    btnClose: TButton;


    procedure btnCloseClick(Sender: TObject);
    procedure btnCloseKeyPress(Sender: TObject; var Key: Char);
    procedure FormShow(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
    PROCEDURE AdjustPosition();
  private
    m_nSoundCnt : integer;

  public
    { Public declarations }
//    m_tp : integer;
//    m_id : string;
    m_sWav : string;
    m_bSiren : boolean;
    m_title, m_msg : string;

  end;

const
  TP_LOGON  = 1;
  TP_LOGOFF = 2;
  TP_SELL   = 3;
  TP_BUY    = 4;
  TP_SIREN  = 8;
  TP_NONE   = 9;

  TIMEOUT_SEC = 5;


var
  fmNotify: TNotify;


  procedure __ShowLogin(tp:integer; id:string; bShow:boolean);
  procedure __ShowCntr(tp:integer; clrTp:string; id:string; bShow:boolean);
  procedure __Siren(msg:string);
  procedure __SignalAlarm(stk:string; updn:integer;basePrc:string) ;

  //procedure __ShowAlarm(tp:integer; id:string);
  //procedure __ShowMsg(stitle:string; sMsg:string);
  //procedure __ShowWarning(sTitle:string; sMsg:string);

implementation

uses
  uCommonDef, uSignal, uMain;

{$R *.dfm}


procedure __ShowLogin(tp:integer; id:string; bShow:boolean);
var
  clrDesc : string;
  wav     : string;
begin

  wav := ExtractFilePath(ParamStr(0)) + '\LogOnOff.wav';
  SndPlaySound (Pchar(wav), Snd_Async);


end;


procedure __ShowCntr(tp:integer; clrTp:string; id:string; bShow:boolean);
var
  clrDesc : string;
  wav     : string;
begin

  if fmMain.chkMute1.Checked=True then
    exit;

  if tp=TP_SELL then
    wav := ExtractFilePath(ParamStr(0)) + '\SELL.wav'
  else if tp=TP_BUY then
    wav := ExtractFilePath(ParamStr(0)) + '\BUY.wav';

  SndPlaySound (Pchar(wav), Snd_Async);

end;


procedure __Siren(msg:string);
var
  wav : string;
begin

  wav := ExtractFilePath(ParamStr(0)) + '\Siren.wav';
  SndPlaySound (Pchar(wav), Snd_Async);

end;



procedure __SignalAlarm(stk:string; updn:integer;basePrc:string) ;
var
  wav : string;
  msg : string;
  sUpDn: string;
begin

  wav := ExtractFilePath(ParamStr(0)) + '\Signal.wav';
  SndPlaySound (Pchar(wav), Snd_Async);

  if stk='' then
    exit;

  if updn=IDX_UP then sUpDn := 'UP'
  else                sUpDn := 'DOWN'
  ;

  msg := format('[%s][%s] %s 가격을 통과함', [stk, sUpDn, basePrc]);
  fmMain.AddMsg(msg);


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

procedure TNotify.FormCreate(Sender: TObject);
begin
  width   := 272;
  height  := 244;
  m_swav := ExtractFilePath(ParamStr(0));
  AdjustPosition();
end;


procedure TNotify.Timer1Timer(Sender: TObject);
begin

  SndPlaySound (Pchar(m_swav), Snd_Async);

//  if Not m_bSiren then
//  begin
//    if m_nSoundCnt=2 then
//    begin
      timer1.Enabled := false;
      fmNotify.close;
//    end;
//  end;
//
//  m_nSoundCnt := m_nSoundCnt + 1;

end;



PROCEDURE TNotify.AdjustPosition();
begin
  Left := Screen.Monitors[0].WorkareaRect.Width - Width;
  Top :=  Screen.Monitors[0].WorkareaRect.Height - Height;

end;

procedure TNotify.FormShow(Sender: TObject);
begin


//
//  if m_tp=TP_NONE then
//  BEGIN
//    fmNotify.lblTitle.Caption := m_title;
//    fmNotify.lblBody.Caption  := m_msg;
//    exit;
//  END;
//
//
//  if m_tp=TP_LOGON then
//  BEGIN
//    fmNotify.lblTitle.Caption := '로그온';
//    fmNotify.lblBody.Caption  := format('[%s] 로그인 했습니다. 확인바랍니다.',[m_id]);
//    m_swav := m_swav + '\LogOnOff.wav';
//  END;
//  if m_tp=TP_LOGOFF then
//  BEGIN
//    fmNotify.lblTitle.Caption := '로그아웃';
//    fmNotify.lblBody.Caption  := format('[%s] 로그아웃 했습니다. 확인바랍니다.',[m_id]);
//    m_swav := m_swav + '\LogOnOff.wav';
//  END;
//  if m_tp=TP_SELL then
//  BEGIN
//    fmNotify.lblTitle.Caption := '체결발생';
//    fmNotify.lblBody.Caption  := format('[%s] 매도체결 했습니다. 확인바랍니다.',[m_id]);
//    m_swav := m_swav + '\SELL.wav';
//  END;
//  if m_tp=TP_BUY then
//  BEGIN
//    fmNotify.lblTitle.Caption := '체결발생';
//    fmNotify.lblBody.Caption  := format('[%s] 매수체결 했습니다. 확인바랍니다.',[m_id]);
//    m_swav := m_swav + '\buy.wav';
//  END;
//  if m_tp=TP_SIREN then
//  BEGIN
//    fmNotify.lblTitle.Caption := '!!!경고!!!';
//    fmNotify.lblBody.Caption  := format('%s',[m_id]);
//    m_swav := m_swav + '\siren.wav';
//  END;
//
//  m_nSoundCnt := 0;
//  SndPlaySound (Pchar(m_swav), Snd_Async);
//
//  timer1.Interval := 5000;
//  timer1.Enabled := true;

  //AdjustPosition();

end;

end.
