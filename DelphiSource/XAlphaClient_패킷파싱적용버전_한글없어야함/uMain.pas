unit uMain;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs
  ,uMastDB, uPacketParse, XAlphaPacket, sSkinProvider, sSkinManager,
  Vcl.StdCtrls, sComboBox, Vcl.ExtCtrls, sPanel, CommonUtils
  ;

type
  TfmMain = class(TForm)
    pnlBody: TsPanel;
    pnlTop: TsPanel;
    pnlBottom: TsPanel;
    cbMsg: TsComboBox;
    sSkinManager1: TsSkinManager;
    sSkinProvider1: TsSkinProvider;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);

    procedure AddMsg(msg:string);
  private
    { Private declarations }
    procedure WndProc(var Message:TMessage);override;
  public
    { Public declarations }
  end;

var
  fmMain: TfmMain;

implementation

var
  _Parser : CPacketParse;

{$R *.dfm}

procedure TfmMain.FormCreate(Sender: TObject);
begin
  __Mast := TMastDB.Create(NIL);

  if __Mast.Initialize()=false then
  begin
    exit;
  end;

end;

procedure TfmMain.FormShow(Sender: TObject);
begin
  _Parser := CPacketParse.Create(fmMain.Handle);
end;

procedure TfmMain.WndProc(var Message: TMessage);
var
  paRecvPack : PAnsiChar;
  nRecvLen   : integer;

  asRecvPack : AnsiString;
begin
  inherited;

  if Message.Msg = WM_NEW_PACKET then
  BEGIN
    nRecvLen    := Integer(Message.WParam);
    paRecvPack  := PAnsiChar(Message.LParam);
    asRecvPack  := StrPas(paRecvPack);

    FreeMem(paRecvPack);
    AddMsg(asRecvPack);
  END;

end;


procedure TfmMain.AddMsg(msg:string);
begin
    cbMsg.Items.Add(format('[%s] %s', [__NowHMS(), msg]));
    cbMsg.ItemIndex := cbMsg.ItemIndex-1;
end;

end.
