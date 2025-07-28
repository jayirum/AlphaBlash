program XCopy;

uses
  Vcl.Forms,
  uMain in 'uMain.pas' {TfmMain},
  CommonUtils in '..\Common\CommonUtils.pas',
  XAlphaPacket in '..\Common\XAlphaPacket.pas',
  uNotify in 'uNotify.pas' {fmNotify},
  Vcl.Themes,
  Vcl.Styles;

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TfmMain, fmMain);
  Application.Run;
end.
