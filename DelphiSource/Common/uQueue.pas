
unit uQueue;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs,
  IniFiles, TypInfo, WinSock, SyncObjs, TlHelp32
  ;

const
  MAX_Q = 5;

type



  CQueue = class
  private
    m_cs      : TCriticalSection;
    m_list    : TStringList;
    m_sMsg    : string;
  public
    constructor Create();
    Destructor  Destroy();
    Procedure   Add(s:string);
    function    GetOnePacket(var refRsltPacket:string):integer;
    function    IsFailedToGet(ret:integer): boolean;
    function    IsGet(ret:integer): boolean;

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
    __Queue  : array [0..MAX_Q-1] of CQueue;

implementation

//const

//var


function CQueue.IsFailedToGet(ret:integer): boolean;
begin
  Result := (ret=RET_FAIL);
end;

function CQueue.IsGet(ret:integer): boolean;
begin 
  Result := (ret=RET_GET);
end;


constructor CQueue.create();
begin
  m_cs    := TCriticalSection.Create;
  m_list  := TStringList.Create;
end;


destructor  CQueue.Destroy;
begin
    FreeAndNil(m_list);
    FreeAndNil(m_cs)
end;


procedure CQueue.Lock;
begin
  m_cs.Enter;
end;

procedure CQueue.Unlock;
begin
  m_cs.Leave;
end;

procedure CQueue.Add(s: string);

begin

  Lock();
  try

    m_list.Add(s);

  finally
    Unlock();
  end;
end;


function CQueue.GetOnePacket(var refRsltPacket:string ):integer;
begin

  Lock();
  try
    try
      if m_list.Count =0 then
      begin
        Result := RET_EMPTY;
        exit;
      end;

      Result := RET_GET;

      refRsltPacket := m_list[0];
      m_list.Delete(0);
    except
      m_sMsg := 'GetOnePacket exception ¹ß»ý';
      Result := RET_FAIL;
    end;
  finally
    Unlock();
  end;
end;


function CQueue.GetMsg():string;
begin
  Result := m_sMsg;
end;

end.
