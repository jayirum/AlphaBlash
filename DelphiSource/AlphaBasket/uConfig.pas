unit uConfig;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.WinXCtrls, Vcl.Mask;

type
  TfmConfig = class(TForm)
    edSymbol1: TEdit;
    Edit2: TEdit;
    Edit3: TEdit;
    Edit4: TEdit;
    Edit5: TEdit;
    Edit6: TEdit;
    Edit7: TEdit;
    swTrade1: TToggleSwitch;
    swTrade2: TToggleSwitch;
    swTrade3: TToggleSwitch;
    swTrade4: TToggleSwitch;
    swTrade5: TToggleSwitch;
    swTrade6: TToggleSwitch;
    swTrade7: TToggleSwitch;
    Label1: TLabel;
    Edit8: TEdit;
    Label2: TLabel;
    Edit9: TEdit;
    Edit10: TEdit;
    Edit11: TEdit;
    Edit12: TEdit;
    Edit13: TEdit;
    Edit14: TEdit;
    Label3: TLabel;
    Edit15: TEdit;
    Edit16: TEdit;
    Edit17: TEdit;
    Edit18: TEdit;
    Edit19: TEdit;
    Edit20: TEdit;
    Edit21: TEdit;
    Label4: TLabel;
    Edit22: TEdit;
    Edit23: TEdit;
    Edit24: TEdit;
    Edit25: TEdit;
    Edit26: TEdit;
    Edit27: TEdit;
    Edit28: TEdit;
    MaskEdit1: TMaskEdit;
    Label5: TLabel;
    MaskEdit2: TMaskEdit;
    MaskEdit3: TMaskEdit;
    MaskEdit4: TMaskEdit;
    MaskEdit5: TMaskEdit;
    MaskEdit6: TMaskEdit;
    MaskEdit7: TMaskEdit;
    Label6: TLabel;
    Label7: TLabel;
    ToggleSwitch8: TToggleSwitch;
    ToggleSwitch9: TToggleSwitch;
    ToggleSwitch10: TToggleSwitch;
    ToggleSwitch11: TToggleSwitch;
    ToggleSwitch12: TToggleSwitch;
    ToggleSwitch13: TToggleSwitch;
    ToggleSwitch14: TToggleSwitch;
    Label8: TLabel;
    Edit29: TEdit;
    Label9: TLabel;
    MaskEdit8: TMaskEdit;
    Label10: TLabel;
    MaskEdit9: TMaskEdit;
    Button1: TButton;
    Label11: TLabel;
    Edit1: TEdit;
    Edit30: TEdit;
    Edit31: TEdit;
    Edit32: TEdit;
    Edit33: TEdit;
    Edit34: TEdit;
    Edit35: TEdit;
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  fmConfig: TfmConfig;

implementation

{$R *.dfm}

procedure TfmConfig.Button1Click(Sender: TObject);
begin
  close;
end;

end.
