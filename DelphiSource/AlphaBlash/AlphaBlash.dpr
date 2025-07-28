program AlphaBlash;

uses
  Vcl.Forms,
  fmMainU in 'fmMainU.pas' {fmMain},
  Vcl.Themes,
  Vcl.Styles,
  fmExeUpdateU in 'fmExeUpdateU.pas' {fmExeUpdate},
  CheckUpdateThread in 'CheckUpdateThread.pas',
  fmMT4UpdateU in 'fmMT4UpdateU.pas' {fmMT4Update},
  CommonUtils in '..\Common\CommonUtils.pas',
  MultiLanguageU in 'MultiLanguageU.pas',
  CommonVal in '..\Common\CommonVal.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  TStyleManager.TrySetStyle('Onyx Blue');
  Application.CreateForm(TfmMain, fmMain);
  Application.CreateForm(TfmExeUpdate, fmExeUpdate);
  Application.CreateForm(TfmMT4Update, fmMT4Update);
  Application.Run;
end.
