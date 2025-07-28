unit uFmBasicForm;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs,
  uLocalCommon,
  uQueueEx
  ;

type
  TfmBasic = class(TForm)
    procedure FormClose(Sender: TObject; var Action: TCloseAction);

  protected

    procedure WndProc(var message : TMessage);override;
    procedure RecvDataProc(sRecvData:string); virtual;abstract;
  public
    { Public declarations }
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;

    procedure Form_Show(frmMode : EN_FORM_MODE);

  end;

var
  fmBasic: TfmBasic;

  procedure __Form_Create(oForm: TComponentClass; var Form; frmMode : EN_FORM_MODE);

implementation

{$R *.dfm}


procedure __Form_Create(oForm: TComponentClass; var Form; frmMode : EN_FORM_MODE);
var
	i: Integer;
  bTF: Boolean;
begin
  if frmMode <> FORM_MODAL  then
  begin
		bTF := False;

    for i := 0 to Application.MainForm.MDIChildCount - 1 do
    begin
      if Application.MainForm.MDIChildren[i].ClassName = oForm.ClassName then
      begin
        bTF := True;
        Application.MainForm.MDIChildren[i].Show;
        Break;
      end;
    end;

	  if bTF then Exit;
  end;

  Application.CreateForm(oForm, Form);

  TfmBasic(Form).Form_Show(frmMode);
end;

constructor TfmBasic.Create(AOwner: TComponent);
var
	i: Integer;
begin
  inherited;
end;

destructor TfmBasic.Destroy;
begin

  inherited;
end;



procedure TfmBasic.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  Action := cafree;
end;

procedure TfmBasic.Form_Show(frmMode : EN_FORM_MODE);
begin
//	ShowMode := iShowMode;

  case frmMode of
    FORM_MODAL :
    begin
          Visible     := False;
          FormStyle   := fsNormal;
          Position    := poMainFormCenter;
          BorderIcons := [biSystemMenu];
          WindowState := wsNormal;
          ShowModal;
    end;
    FORM_MDI :
    begin //MDI
          FormStyle   := fsMDIChild;
          WindowState := wsNormal;
          Visible     := True;
    end;
    FORM_MDI_MAX :
    begin //MDI Max
          FormStyle   := fsMDIChild;
          WindowState := wsMaximized;
          Visible     := True;
    end;
  end;
end;



procedure TfmBasic.WndProc(var message : TMessage);
var
  sPacket : string;
  nLen    : integer;
  pItem   : PTQItem;
begin

  Assert(Message.Msg = WM_RECV_DATA, 'Message.Msg <> WM_RECV_DATA in BasicForm');

  pItem := PTQItem(Message.LParam);
  nLen  := Message.WParam;

  sPacket := pItem.data;
  Dispose(pItem);

  RecvDataProc(sPacket);

end;

end.
