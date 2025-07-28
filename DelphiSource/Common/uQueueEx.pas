unit uQueueEx;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs,
  IniFiles, TypInfo, WinSock, SyncObjs, TlHelp32
  ;

const
  MAX_Q = 5;

type

  PTQItem = ^TQItem;
  TQItem = record
    sKey  : string;  // ctx key
    sCode : string;
    data  : string;
    etc   : string;
  end;

  CQueueEx = class
  private
    m_cs      : TCriticalSection;
    m_list    : TLIST;
    m_sMsg    : string;
  public
    constructor Create();
    Destructor  Destroy();
    Procedure   Add(sKey:string; sCode:string; sData:string; etc:string='');
    function    Get():PTQItem;

    function    GetMsg():string;

  private
    procedure   Lock();
    procedure   Unlock();
  end;



const

  __BUF_SIZE = 1024;

  RET_FAIL  = -1;
  RET_EMPTY = 0;
  RET_GET   = 1;

var
  __QueueEx  : array [0..MAX_Q-1] of CQueueEx;

implementation

//const

//var



constructor CQueueEx.create();
begin
  m_cs    := TCriticalSection.Create;
  m_list  := TList.Create;
end;


destructor  CQueueEx.Destroy;
begin
    FreeAndNil(m_list);
    FreeAndNil(m_cs)
end;


procedure CQueueEx.Lock;
begin
  m_cs.Enter;
end;

procedure CQueueEx.Unlock;
begin
  m_cs.Leave;
end;

procedure CQueueEx.Add(sKey:string; sCode:string; sData:string; etc:string='');
var
  pItem : ^TQItem;

begin

  New(pItem);
  pItem.sKey := sKey;
  pItem.sCode := sCode;
  pItem.data  := sData;
  pItem.etc  := etc;


  Lock();
  try

    m_list.Add(pItem);

  finally
    Unlock();
  end;
end;


{
// 반드시 밖에서 Dispose 해줘야 한다.

  Return
    -1 : 에러
     0 : no data
     1 : get data

}
function CQueueEx.Get():PTQItem;
var
  pItem : PTQItem;
begin

  Lock();
  try
    try
      if m_list.Count =0 then
      begin
        Result := Nil;
        exit;
      end;

      Result := m_list[0];
      m_list.Delete(0);
    except
      m_sMsg := '[CQueueEx.Get()] exception 발생';
      showmessage(m_sMsg);
      Result := Nil;
    end;
  finally
    Unlock();
  end;
end;


function CQueueEx.GetMsg():string;
begin
  Result := m_sMsg;
end;

end.
