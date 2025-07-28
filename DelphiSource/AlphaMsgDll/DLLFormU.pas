unit DLLFormU;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.ExtCtrls, Vcl.StdCtrls, Vcl.Buttons,
  MMSystem;//, acPNG;

type TMessQueue = record
  Title   : string;
  Mess    : string;
  DT      : TDateTime;
  DTStr   : string;
  nSound  : integer;
end;

type TMessQ = Array of TMessQueue;

type
  TDLLForm = class(TForm)
    Panel1: TPanel;
    lblMessage: TLabel;
    lblCurrNum: TLabel;
    btnLeft: TButton;
    btnRight: TButton;
    Panel2: TPanel;
    lblTitle: TLabel;
    SpeedButton1: TSpeedButton;
    lblTime: TLabel;
    SpeedButton2: TSpeedButton;
    procedure FormShow(Sender: TObject);
    procedure btnLeftClick(Sender: TObject);
    procedure btnRightClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure SpeedButton1Click(Sender: TObject);
  private

    { Private declarations }
    m_sSoundDir   : string;
    m_sAlarmFile  : string;
    m_sSirenFile  : string;
    m_bPlaySound  : boolean;

    procedure PlaySound(nSoundTp:integer);
  public
    MessQ : TMessQ;
    CurrNum : integer;
    procedure IncCurrIdx();
    procedure UpdateData(bPlayedByOuter:boolean);
    procedure AdjustPosition;
    procedure Set_SoundFileDir(sDir:string);

    { Public declarations }
  end;

const
     maxevents = 2;

     msg_opennewtab = 1;           // used in messaeg.wparam to signify different actions for our custom windows message : custommsgval
     msg_processmsglist = 2;
     msg_closetab = 3;

     SOUND_TP_NONE = 0;
     SOUND_TP_INFO = 1;
     SOUND_TP_ORD  = 2;
     SOUND_TP_ERR  = 3;

var
  DLLForm: TDLLForm;

  MT4apphandle,DLLApphandle : hwnd; // stores the Application handles for both MT4 and this DLL.
                                    // These are used when creating windows so we can ensure MT4 owns them
                                    // In this way our windows can be made to stay on top of MT4

  wakeEvents : array[0 .. maxevents - 1] of dword;  // used to signal our new thread

  ThreadLock : TRTLCriticalSection;  // Used to force other threads to wait until there are no other threads using the either this section of code.
                                    // or the objects that are thread sensitive

implementation

{$R *.dfm}


procedure TDLLForm.IncCurrIdx();
begin
  Inc(CurrNum);
end;

procedure TDLLForm.UpdateData(bPlayedByOuter:boolean);
begin

  lblTitle.Caption    := MessQ[CurrNum].Title;
  lblMessage.Caption  := MessQ[CurrNum].Mess;
  lblTime.caption     := '['+MessQ[CurrNum].DTStr+']';
  lblCurrNum.Caption  := IntToStr(CurrNum + 1) + '/' + IntToStr(Length(MessQ));

  if (CurrNum > 0) then
  begin
    btnLeft.Enabled     := True;
    btnLeft.Font.Color  := clBlue;
    btnLeft.Font.style  := [fsbold];
  end
  else
  begin
    btnLeft.Enabled     := false;
    btnLeft.Font.Color  := clBlack;
    btnLeft.Font.style  := [];
  end;

  if(CurrNum < Length(MessQ) - 1) then
  begin
    btnRight.Enabled     := True;
    btnRight.Font.Color  := clBlue;
    btnRight.Font.style  := [fsbold];
  end
  else
  begin
    btnRight.Enabled     := false;
    btnRight.Font.Color  := clBlack;
    btnRight.Font.style  := [];
  end;

  if (bPlayedByOuter) and (MessQ[CurrNum].nSound > 0 ) then
  begin
    PlaySound(MessQ[CurrNum].nSound);
  end;

end;

procedure TDLLForm.btnLeftClick(Sender: TObject);
begin
  Dec(CurrNum);
  UpdateData(False);
end;

procedure TDLLForm.btnRightClick(Sender: TObject);
begin
  Inc(CurrNum);
  UpdateData(False);
end;

procedure TDLLForm.Set_SoundFileDir(sDir:string);
begin
  m_sSoundDir   := sDir;
  m_sAlarmFile  := sDir + '\' + 'Alpha_Alarm.wav';
  m_sSirenFile  := sDir + '\' + 'Alpha_Siren.wav';
end;

procedure TDLLForm.PlaySound(nSoundTp:integer);
begin
  if nSoundTp = SOUND_TP_ERR then
    SndPlaySound (Pchar(m_sSirenFile), Snd_Async)
  else
    SndPlaySound (Pchar(m_sAlarmFile), Snd_Async);

end;


procedure TDLLForm.FormCreate(Sender: TObject);
begin
  SetLength(MessQ, 0);
end;

procedure TDLLForm.AdjustPosition;
begin
  //Left := Screen.WorkAreaRect.Width - Width;
  //Top := Screen.WorkAreaRect.Height - Height;

  // then

  //Label1.Caption := Screen.Monitors[0].WorkareaRect

  Left := Screen.Monitors[0].WorkareaRect.Width - Width;
  Top :=  Screen.Monitors[0].WorkareaRect.Height - Height;

end;

procedure TDLLForm.FormShow(Sender: TObject);
begin

  CurrNum := 0;

  if Length(MessQ) = 0 then
  begin
    //Caption := 'No Messages';
    Panel1.Visible := FALSE;
    Exit;
  end;

  Panel1.Visible := TRUE;
  UpdateData(True);
  AdjustPosition;
end;

procedure TDLLForm.SpeedButton1Click(Sender: TObject);
begin
  close;
end;

end.
