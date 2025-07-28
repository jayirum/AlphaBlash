program SentosaSuite_v1;

uses
  Vcl.Forms,
  uLocalCommon in 'uLocalCommon.pas',
  uQueueEx in '..\..\Common\uQueueEx.pas',
  uFmBasicForm in 'uFmBasicForm.pas' {fmBasic},
  uFmMain in 'uFmMain.pas' {fmMain},
  uFmPriceComparison in 'uFmPriceComparison.pas' {fmComparison},
  CommonUtils in '..\..\Common\CommonUtils.pas',
  MTLoggerU in '..\..\Common\MTLoggerU.pas',
  ProtoGetU in '..\..\Common\ProtoGetU.pas',
  ProtoSetU in '..\..\Common\ProtoSetU.pas',
  uTcpClient in '..\..\Common\uTcpClient.pas',
  uFmLogin in 'uFmLogin.pas' {fmLogin};

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TfmMain, fmMain);
  Application.Run;
end.
