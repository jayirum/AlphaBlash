program XCopyMacro;



uses
  Vcl.Forms,
  uMain in 'uMain.pas' {TfmMain},
  CommonUtils in '..\Common\CommonUtils.pas',
  XAlphaPacket in '..\Common\XAlphaPacket.pas',
  uNotify in 'uNotify.pas' {fmNotify},
  Vcl.Themes,
  Vcl.Styles,
  uCommonDef in 'uCommonDef.pas',
  uPickOrdButton in 'uPickOrdButton.pas' {PickButton},
  uOrdThrd in 'uOrdThrd.pas',
  uPacketThrd in 'uPacketThrd.pas',
  uRecvThrd in 'uRecvThrd.pas',
  uTickThrd in 'uTickThrd.pas',
  uPrcList in 'uPrcList.pas',
  uPrcGrid in 'uPrcGrid.pas' {fmPrcGrid},
  uControlSize in 'uControlSize.pas',
  uTrailingStop in 'uTrailingStop.pas',
  uSettingTS in 'uSettingTS.pas' {FrmSettingTS},
  MTLoggerU in '..\Common\MTLoggerU.pas',
  uPos_Manually in 'uPos_Manually.pas' {FmPosManual},
  uSignal in 'uSignal.pas';

{$R *.res}


begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  TStyleManager.TrySetStyle('Cyan Dusk');
  Application.CreateForm(TfmMain, fmMain);

  Application.Run;
end.
