
unit XAlphaPacket;

interface

uses
	Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Dialogs,
  IniFiles, TypInfo, WinSock, SyncObjs, TlHelp32
  ;


type



	TCNTR = Packed Record
		//STX     : array[0..0] of char;
		//Len     : array[0..3] of char;
		Code    : array[0..1] of char;
		oldDataYn  : array[0..0] of char;
		masterId  : array[0..19] of char;
		cntrNo  : array[0..4] of char;
		stkCd   : array[0..9] of char;
		bsTp    : array[0..0] of char;
		cntrQty : array[0..2] of char;
		cntrPrc : array[0..9] of char;
		clrPl   : array[0..9] of char;
		cmsn    : array[0..4] of char;
		clrTp   : array[0..0] of char;
		bf_nclrQty  : array[0..2] of char;
		af_nclrQty  : array[0..2] of char;
		bf_avgPrc   : array[0..9] of char;
		af_avgPrc   : array[0..9] of char;
		bf_amt      : array[0..9] of char;
		af_amt      : array[0..9] of char;
		ordTp       : array[0..1] of char;
		tradeTm     : array[0..11] of char;
		lvg         : array[0..1] of char;
		//ETX         : array[0..0] of char;
  end;


	TLOGON = Packed Record
		//STX     : array[0..0] of char;
		//Len     : array[0..3] of char;
		Code    : array[0..1] of char;
		oldDataYn  : array[0..0] of char;
		masterId  : array[0..19] of char;
		Tm      : array[0..11] of char;
		loginTp : array[0..0] of char;	// I/O     
		masterNm  : array[0..19] of char;
		//ETX     : array[0..0] of char;
  end;


	TCL_PWD = packed Record
    Code : array[0..1] of char;
		Pwd : array[0..19] of char;
  end;

	TCL_CNTR_HIST = packed Record
    Code      : array[0..1] of char;
		MasterId  : array[0..19] of char;
  end;

	TRET_MSG = Packed Record
		Code    : array[0..1] of char;
		RetCode : array[0..1] of char
  end;


  TTICK = packed record
		Code  : array [0..1]  of char;
    stk	  : array [0..7] of char;
    close : array [0..14] of char;
    side	: array [0..0] of char;
    time	: array [0..10] of char;
//    open  : array [0..14] of char;
//    high  : array [0..14] of char;
//    low	  : array [0..14] of char;
//    gap	  : array [0..14] of char;
//    vol	  : array [0..9] of char;
//    amt	  : array [0..10] of char;
//    ydiffSign : array[0..1] of char;
//    chgrate   : array[0..5] of char;
//    execvol   : array[0..14] of char;
	end;

  CPacketQueue = class
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

    //function    IsLogonPacket(var sPacket:string):boolean;
    function    GetMsg():string;

    function    GetCode(var sPacket:string):string;

  private
    procedure   Lock();
    procedure   Unlock();
  end;



const

  __BUF_SIZE = 1024;

  RET_FAIL  = -1;
  RET_EMPTY = 0;
  RET_GET   = 1;



  CODE_PWD		  = '01';
  CODE_LOGONOFF = '02';
  CODE_CNTR	  	= '03';
  CODE_MSG		  = '04';
  CODE_CNTR_HIST= '05';


  RETCODE_PWD_OK	  =	'00';
  RETCODE_PWD_WRONG	= '01';
  RETCODE_CNTR_NODATA = '02';


var
    __PacketQ  : CPacketQueue;

implementation

//const

//var


function CPacketQueue.IsFailedToGet(ret:integer): boolean;
begin
  Result := False;
  if ret=RET_FAIL then
    Result := True;
end;

function CPacketQueue.IsGet(ret:integer): boolean;
begin 
  Result := False;
  if ret=RET_GET then
    Result := True;
end;

    
constructor CPacketQueue.create();
begin
  m_cs    := TCriticalSection.Create;
  m_list  := TStringList.Create;
end;


destructor  CPacketQueue.Destroy;
begin
    FreeAndNil(m_list);
    FreeAndNil(m_cs)
end;


procedure CPacketQueue.Lock;
begin
  m_cs.Enter;
end;

procedure CPacketQueue.Unlock;
begin
  m_cs.Leave;
end;

procedure CPacketQueue.Add(s: string);

begin

  Lock();
  try

    m_list.Add(s);

  finally
    Unlock();
  end;
end;


function CPacketQueue.GetOnePacket(var refRsltPacket:string ):integer;
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


function CPacketQueue.GetMsg():string;
begin
  Result := m_sMsg;
end;

function CPacketQueue.GetCode(var sPacket:string):string;
var
  sCode : string;
begin

  sCode := Copy(sPacket, 1, 2);

  Result := sCode;
end;

end.
