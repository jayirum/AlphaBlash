program AlphaBasket;

uses
  Vcl.Forms,
  fmMainU in 'fmMainU.pas' {fmMain},
  Vcl.Themes,
  Vcl.Styles,
  fmExeUpdateU in 'fmExeUpdateU.pas' {fmExeUpdate},
  CheckUpdateThread in 'CheckUpdateThread.pas',
  fmMT4UpdateU in 'fmMT4UpdateU.pas' {fmMT4Update},
  CommonUtils in '..\Common\CommonUtils.pas',
  CommonVal in '..\Common\CommonVal.pas',
  uPacketProcess in 'uPacketProcess.pas',
  uAlphaProtocol in '..\Common\uAlphaProtocol.pas',
  uBasketCommon in 'uBasketCommon.pas',
  uLoginProcess in 'uLoginProcess.pas',
  uMDProcess in 'uMDProcess.pas',
  uPosDataProcess in 'uPosDataProcess.pas',
  uCtrls in 'uCtrls.pas',
  MTLoggerU in '..\Common\MTLoggerU.pas',
  ProtoGetU in '..\Common\ProtoGetU.pas',
  ProtoSetU in '..\Common\ProtoSetU.pas',
  uPostThread in '..\Common\uPostThread.pas',
  uQueueEx in '..\Common\uQueueEx.pas',
  uConfig in 'uConfig.pas' {fmConfig},
  uLoadSymbols in 'uLoadSymbols.pas',
  uDataHandler in 'uDataHandler.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  TStyleManager.TrySetStyle('Cyan Dusk');
  Application.CreateForm(TfmMain, fmMain);
  Application.CreateForm(TfmConfig, fmConfig);
  Application.Run;
end.
