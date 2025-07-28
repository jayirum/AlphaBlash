unit uRecvThrd;

interface


uses
  System.Classes, vcl.dialogs, system.SysUtils
  ;


type

  TRecvThrd = class(TThread)
    procedure Execute();override;
  end;

var
  __rcvThrd   : TRecvThrd;

implementation

uses
  uMain, uPacketThrd, XAlphaPacket;


procedure TRecvThrd.Execute();
var
  s : string;
begin

  while not terminated and fmMain.idTcpOrd.connected and fmMain.m_bTerminate=false do
  begin

    try
      s := fmMain.idTcpOrd.IOHandler.ReadLn();
      if length(s)>0 then
      begin
        //fmMain.AddMsg(format('[%s]',[s]));
        __PacketQ.Add(s);
      end;

    except
      //on E: EIdSocketHandleError do
      begin

        showmessage('������ ���� ����.�ٽ� ������ �ּ���');
        exit;

      end;
    end;

  end;


end;

end.
