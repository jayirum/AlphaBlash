unit uFmPriceComparison;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs,
  uFmBasicForm
  ;

type
  TfmComparison = class(TfmBasic)

  protected
    procedure RecvDataProc(sRecvData:string); override;

  public
    { Public declarations }
  end;

var
  fmComparison: TfmComparison;

implementation

{$R *.dfm}


procedure TfmComparison.RecvDataProc(sRecvData:string);
begin
  //TODO. Packet parsing
  showmessage(sRecvData);
end;


end.
