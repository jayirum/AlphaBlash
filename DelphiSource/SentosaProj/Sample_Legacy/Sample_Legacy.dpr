program Sample_Legacy;

uses
  Vcl.Forms,
  fmMainU in 'fmMainU.pas' {fmMain},
  Vcl.Themes,
  Vcl.Styles,
  fmExeUpdateU in 'fmExeUpdateU.pas' {fmExeUpdate},
  CheckUpdateThread in 'CheckUpdateThread.pas',
  fmMT4UpdateU in 'fmMT4UpdateU.pas' {fmMT4Update},
  uPacketProcess in 'uPacketProcess.pas',
  uLocalCommon in 'uLocalCommon.pas',
  uLoginProcess in 'uLoginProcess.pas',
  uMDProcess in 'uMDProcess.pas',
  uPosDataProcess in 'uPosDataProcess.pas',
  uCtrls in 'uCtrls.pas',
  uSymbolSpecProcess in 'uSymbolSpecProcess.pas',
  CommonUtils in '..\..\Common\CommonUtils.pas',
  MTLoggerU in '..\..\Common\MTLoggerU.pas',
  ProtoGetU in '..\..\Common\ProtoGetU.pas',
  ProtoSetU in '..\..\Common\ProtoSetU.pas',
  uAlphaProtocol in '..\..\Common\uAlphaProtocol.pas',
  uPostThread in '..\..\Common\uPostThread.pas',
  uQueueEx in '..\..\Common\uQueueEx.pas',
  uTcpClient in '..\..\Common\uTcpClient.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  TStyleManager.TrySetStyle('Cyan Dusk');
  Application.CreateForm(TfmMain, fmMain);
  Application.Run;
end.
