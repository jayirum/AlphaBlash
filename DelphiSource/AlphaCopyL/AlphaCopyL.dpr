program AlphaCopyL;

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
  CommonVal in '..\Common\CommonVal.pas',
  uLocalCommon in 'uLocalCommon.pas',
  uMasterCopierTab in 'uMasterCopierTab.pas',
  uEAConfigFile in 'uEAConfigFile.pas',
  uConfigTab in 'uConfigTab.pas',
  MTLoggerU in '..\Common\MTLoggerU.pas',
  ProtoGetU in '..\Common\ProtoGetU.pas',
  ProtoSetU in '..\Common\ProtoSetU.pas',
  uAlphaProtocol in '..\Common\uAlphaProtocol.pas',
  uQueueEx in '..\Common\uQueueEx.pas',
  uTcpSvr in 'uTcpSvr.pas',
  uRecvDataProc in 'uRecvDataProc.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  TStyleManager.TrySetStyle('Cyan Dusk');
  Application.CreateForm(TfmMain, fmMain);
  Application.Run;
end.
