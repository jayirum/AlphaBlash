unit uPickOrdButton;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, uCommonDef, Vcl.ExtCtrls;

type
  TPickButton = class(TForm)
    Label1: TLabel;
    edX: TEdit;
    edY: TEdit;
    Button1: TButton;
    tmrXY: TTimer;
    pnlXY: TPanel;
    Panel1: TPanel;
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure tmrXYTimer(Sender: TObject);
    procedure edYKeyPress(Sender: TObject; var Key: Char);
    procedure edXKeyPress(Sender: TObject; var Key: Char);
  private
      function InstallMouseHook: Boolean;
    { Private declarations }

  public
      procedure Pass(var x:integer; var y:integer);
  public
    { Public declarations }
     MouseHookHandle : hHook;
  end;

var
  fmPickButton: TPickButton;

  procedure __PickButton();



implementation

{$R *.dfm}

uses
  commonutils;


procedure __PickButton();
begin
  fmPickButton := TPickButton.Create(application);
  fmPickButton.ShowModal();

end;



function LowLevelMouseProc(nCode: Integer; wParam: wParam; lParam: lParam): LRESULT; stdcall;
var
 pt : TPoint;
begin

  Result := CallNextHookEx(fmPickButton.MouseHookHandle, nCode, wParam, lParam);

  if wParam = WM_RButtonDOWN then
  begin
    fmPickButton.panel1.Caption := format('[%s]mouse right event',[__NowHMS()]);
    GetCursorPos(pt);
    fmPickButton.edX.Text := IntToStr(pt.X);
    fmPickButton.edY.Text := IntToStr(pt.Y);

    __hookX := pt.X;
    __hookY := pt.Y;

    fmPickButton.Button1Click(application);
  end;

  if wParam = WM_LButtonDOWN then
    fmPickButton.panel1.Caption := format('[%s]mouse Left event',[__NowHMS()]);


end;

procedure TPickButton.Pass(var x: Integer; var y: Integer);
begin

end;
procedure TPickButton.tmrXYTimer(Sender: TObject);
var
  pt : TPoint;
begin
  GetCursorPos(pt);
  pnlXY.Caption := 'X='+IntToStr(pt.X)+ ', Y='+IntToStr(pt.Y);
end;

procedure TPickButton.Button1Click(Sender: TObject);
begin
  __hookX := strtointdef(fmPickButton.edX.Text,0);
  __hookY := strtointdef(fmPickButton.edY.Text,0);

  close;
end;

procedure TPickButton.edXKeyPress(Sender: TObject; var Key: Char);
begin
  if key = #13 THEN
    edY.SetFocus;

end;

procedure TPickButton.edYKeyPress(Sender: TObject; var Key: Char);
begin
  if key = #13 THEN
    Button1Click(SENDER);
end;

procedure TPickButton.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  tmrXY.Enabled := false;
  UnhookWindowsHookEx(fmPickButton.MouseHookHandle);
end;

procedure TPickButton.FormCreate(Sender: TObject);
begin
  //InstallMouseHook;
  tmrXY.Interval := 100;
  tmrXY.Enabled  := true;

end;

procedure TPickButton.FormShow(Sender: TObject);
begin
//
  InstallMouseHook;

  SetWindowPos(fmPickButton.handle, HWND_TOPMOST, fmPickButton.Left, fmPickButton.Top, fmPickButton.Width, fmPickButton.Height,0);
end;

function TPickButton.InstallMouseHook : Boolean;
begin
  Result := False;
  if fmPickButton.MouseHookHandle = 0 then
  begin
    fmPickButton.MouseHookHandle := SetWindowsHookEx(WH_MOUSE_LL, @LowLevelMouseProc, hInstance, 0);
    Result := fmPickButton.MouseHookHandle <> 0;
    if Result = FALSE then
    begin
      ShowMessage('Mouse Hook not installed, mouse tracking functionality disabled !');
    end;
  end;
end;

end.
