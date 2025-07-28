unit uMain;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs,Vcl.Menus,
  uLocalCommon,
  uBasicForm,
  uPriceComparison, System.Win.TaskbarCore, Vcl.Taskbar, Vcl.ExtCtrls,
  Vcl.ToolWin, Vcl.ActnMan, Vcl.ActnCtrls, Vcl.ActnMenus, System.Actions,
  Vcl.ActnList
  ;

type
  TfrmMain = class(TForm)
    MainMenu1: TMainMenu;
    mnuFile: TMenuItem;
    mnuMD: TMenuItem;
    subComparison: TMenuItem;
    Shape1: TShape;
    procedure subComparisonClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  frmMain: TfrmMain;

implementation

{$R *.dfm}




// __Form_Create(oForm: TComponentClass; var Form; frmMode : EN_FORM_MODE);
procedure TfrmMain.FormCreate(Sender: TObject);
begin
  ActionMainMenuBar1.ActionManager.Actions := ActionList1;
end;

procedure TfrmMain.subComparisonClick(Sender: TObject);
begin
  __Form_Create(TfrmComparison, frmComparison, FORM_MDI);
end;

end.
