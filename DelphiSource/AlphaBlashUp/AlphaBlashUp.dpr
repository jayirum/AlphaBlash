program AlphaBlashUp;

uses
  Vcl.Forms,
  Vcl.Themes,
  Vcl.Styles,
  CommonUtils in '..\Common\CommonUtils.pas',
  uMainUp in 'uMainUp.pas' {fmMainUp},
  CommonVal in '..\Common\CommonVal.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TfmMainUp, fmMainUp);
  TStyleManager.TrySetStyle('Onyx Blue');
  Application.Run;
end.
