program XAlphaClient;

uses
  Vcl.Forms,
  uMain in 'uMain.pas' {TfmMain},
  CommonUtils in '..\Common\CommonUtils.pas',
  XAlphaPacket in '..\Common\XAlphaPacket.pas',
  uMastDB in 'uMastDB.pas' {TMastDB: TDataModule};

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TfmMain, fmMain);
  Application.Run;
end.
