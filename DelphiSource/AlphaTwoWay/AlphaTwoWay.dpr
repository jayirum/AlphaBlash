program AlphaTwoWay;

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
  uTwoWayCommon in 'uTwoWayCommon.pas',
  uLoginProcess in 'uLoginProcess.pas',
  uMDProcess in 'uMDProcess.pas',
  uPosDataProcess in 'uPosDataProcess.pas',
  uCtrls in 'uCtrls.pas',
  MTLoggerU in '..\Common\MTLoggerU.pas',
  ProtoGetU in '..\Common\ProtoGetU.pas',
  ProtoSetU in '..\Common\ProtoSetU.pas',
  uSymbolSpecProcess in 'uSymbolSpecProcess.pas',
  uRealPLOrder in 'uRealPLOrder.pas',
  uPostThread in '..\Common\uPostThread.pas',
  uQueueEx in '..\Common\uQueueEx.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  TStyleManager.TrySetStyle('Cyan Dusk');
  Application.CreateForm(TfmMain, fmMain);
  Application.Run;
end.
