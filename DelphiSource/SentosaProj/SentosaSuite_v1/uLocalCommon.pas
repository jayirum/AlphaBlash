unit uLocalCommon;

interface


Uses
  System.sysutils, windows, Messages, Vcl.ComCtrls, vcl.stdctrls, vcl.forms
  ;

type
  EN_FORM_MODE = (FORM_MODAL=0, FORM_MDI, FORM_MDI_MAX);

const
  Q_SEND_RELAY  = 0;
  Q_SEND_DATA   = 1;

  APPTP_EA      = 0;
  APPTP_SUITES  = 1;

  SOCKTP_RELAY_R  = 'R';
  SOCKTP_RELAY_S  = 'S';
  SOCKTP_DATA     = 'D';


  TIMEOUT_SENDMSG = 1000; // 1sec



  WM_RECV_DATA = WM_USER + 6761;


var
  _UserID        : string;
  _Pwd           : string;
  _RelaySvrIP    : string;
  _RelaySvrPort  : string;
  _DataSvrIP     : string;
  _DataSvrPort   : string;
  _AppID         : string;
  _bAuthSuccess   : boolean = false;

  /////////////////////////////////////////////////////////////
  ///
  procedure __AddMsg(sMsg:string; bStress:boolean=false);

  procedure __InitailzeCommon();
  procedure __DeInitailzeCommon();

  function  __Is_AuthDone():boolean;

implementation
uses
  CommonUtils, System.Classes, MTLoggerU, uFmMain
  ;

var
  _MsgCombo : ^TComboBox;
  _log      : TMTLogger;


function  __Is_AuthDone():boolean;
begin
  Result := _bAuthSuccess;
end;

procedure __InitailzeCommon();
begin
  _log := TMTLogger.create(True);
  _log.Initialize(GetCurrentDir(), ExtractFileName(Application.ExeName));

  _MsgCombo := @fmMain.cbMsg;
end;

procedure __DeInitailzeCommon();
begin
  FreeAndNil(_log);
end;

procedure __AddMsg(sMsg:string; bStress:boolean=false);
var
  msg:string;
begin
  if bStress then
    msg := format('[%s] !!!==> %s', [__NowHMS(), sMsg])
  else
    msg := format('[%s] %s', [__NowHMS(), sMsg]);

  TThread.Queue(nil, procedure
                     begin
                        _MsgCombo.Items.Insert(0, msg);
                        _MsgCombo.ItemIndex := 0;
                     end
               );

  //if bShow then
  //  showmessage(msg);

  _log.log(INFO, msg);

end;

end.
