
unit uPostThread;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs,
  IniFiles, TypInfo, WinSock, SyncObjs, TlHelp32
  ,uQueueEx
  ;

const
  BUF_LEN     = 256;
  BIGBUF_LEN  = 512;

  WM_LOGON    = WM_USER + 681;
  WM_MD       = WM_USER + 682;
  WM_SYMBOL   = WM_USER + 683;
  WM_POSITION = WM_USER + 684;

type

  TPostBuf = packed record
    buf : array [0..BUF_LEN-1] of ansichar;
  end;

  TPostBufBig = packed record
    buf : array [0..BIGBUF_LEN-1] of ansichar;
  end;




  procedure __PostThreadMsg(pItem:PTQItem; thrdID:cardinal; wmMsg:cardinal; ansiData:ansistring; dataLen:integer);
  function  __GetThreadMsgString(M:Msg; var sData:string):integer;

implementation

//const

//var

procedure __PostThreadMsg(pItem:PTQItem; thrdID:cardinal; wmMsg:cardinal; ansiData:ansistring; dataLen:integer);
var
  B : TBytes;
  P : Pointer;

begin

  SetLength(B, dataLen);
  CopyMemory(Addr(B[0]), Addr(ansiData[1]), dataLen);

  //p := GetMemory(Length(B));
  p := GetMemory(dataLen);
  CopyMemory(p, Addr(B[0]), Length(B));

  PostThreadMessage(thrdID, wmMsg, dataLen, LPARAM(p));

  Dispose(pItem);

end;

//
function __GetThreadMsgString(M:Msg; var sData:string):integer;
var
  B     : TBytes;
  Buff  : TPostBuf;
  sAnsi : ansistring;
begin
  SetLength(B, M.wParam);
  CopyMemory(B, Pointer(M.LParam), M.wParam);

  ZeroMemory(Addr(Buff.buf[0]), sizeof(Buff));
  CopyMemory(Addr(Buff.buf[0]), B, Length(B));

  sData  := '';
  sData  := string(Buff.buf);

  FreeMemory(Pointer(M.LParam));

  Result := M.wParam;

end;


end.
