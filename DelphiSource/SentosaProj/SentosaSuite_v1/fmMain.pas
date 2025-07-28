unit fmMain;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs,Vcl.Menus,
  uLocalCommon,
  fmBasicForm,
  //fmPriceComparison, 
  Vcl.ExtCtrls, Vcl.StdCtrls
  ;

type
  TfmMain = class(TForm)
    MainMenu1: TMainMenu;
    mnuFile: TMenuItem;
    mnuMD: TMenuItem;
    subComparison: TMenuItem;
    Shape1: TShape;
    cbMsg: TComboBox;
    procedure subComparisonClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);

    procedure  AddMsg(sMsg:string; bStress:boolean; bShow:boolean);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  fmMain: TfmMain;

implementation

{$R *.dfm}




// __Form_Create(oForm: TComponentClass; var Form; frmMode : EN_FORM_MODE);
procedure TfmMain.FormCreate(Sender: TObject);
begin
 //
end;

procedure TfmMain.subComparisonClick(Sender: TObject);
begin
  //__Form_Create(TfmComparison, fmComparison, FORM_MDI);
end;



procedure  TfmMain.AddMsg(sMsg:string; bStress:boolean; bShow:boolean);
var
  msg:string;
begin
  if bStress then
    msg := format('[%s] !!!==> %s', [__NowHMS(), sMsg])
  else
    msg := format('[%s] %s', [__NowHMS(), sMsg]);

  TThread.Queue(nil, procedure
                     begin
                        cbMsg.Items.Insert(0, msg);
                        cbMsg.ItemIndex := 0;
                     end
               );

  if bShow then
    showmessage(msg);

  m_log.log(INFO, msg);
end;

end.
