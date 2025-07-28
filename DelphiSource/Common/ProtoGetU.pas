(*
  packet structure

  STX
  134=DATA±Ê¿Ã
  SOH
  DATA
*)


unit ProtoGetU;

interface

uses
  System.Classes, System.SysUtils, Windows, VCL.Graphics,
  StrUtils, SyncObjs, WinAPI.Messages
  ,uAlphaProtocol
  ;

type TStringArray = Array of string;

type
    TProtoGet = class(TObject)
    private
    protected
      KV : TKeyValueArray;
    public
      constructor Create;
      function SplitPacket(instr : string; var outstr : TStringArray) : integer;
      // Parsoing Packet is a virtual function
      // "Abstract" directive modifies the effect of the virtual directive.
      // It means that the current class MUST NOT code the method - it is there only
      // as a placeholder to remind and ensure that derived classes implement it.
      function ParsingPacket(instr : string) : integer; virtual; abstract;

    end;




    function __PacketCode(sPacket:string):string;
    function __IsSuccess(sPacket:string; var sErrCode:string; var sErrMsg:string):boolean;
    function __GetValue(sPacket:string; sKey:string):string;

implementation


{ utils }
function __PacketCode(sPacket:string):string;
var
  sCodeField  : string;
  find        : integer;
begin
  Result := '';

  if length(sPacket) < PACKET_CODE_SIZE then
    exit;

  sCodeField :=  FDS_CODE + '=';

  find := pos(sCodeField, sPacket);
  if find <=0 then
    exit;



  Result := copy(sPacket, find+4, PACKET_CODE_SIZE );
end;

function __GetValue(sPacket:string; sKey:string):string;
var
  sCodeField  : string;
  keyIdx      : integer;
  SOHIdx      : integer;
  equalIdx    : integer;
  sTemp       : string;
  //unitLen     : integer;
begin
  Result := '';

  sCodeField :=  sKey + '=';

  keyIdx := pos(sCodeField, sPacket);
  if keyIdx <=0 then
    exit;

  sTemp := Copy(sPacket, keyIdx, length(sPacket)-(keyIdx-1));
  SOHIdx := Pos(DEF_DELIMITER, sTemp);
  if SOHIdx<=0 then
    exit;

  equalIdx := Pos('=', sTemp);
  if equalIdx<=0 then
    exit;

  Result := Copy(sTemp, equalIdx+1, SOHIdx-equalIdx-1);
end;


function __IsSuccess(sPacket:string; var sErrCode:string; var sErrMsg:string):boolean;
var
  sSuccYN : string;
begin
  Result := true;

  sSuccYN := __GetValue(sPacket, FDS_SUCC_YN);
  if sSuccYN='Y' then
    exit;

  Result := false;
  sErrCode  := __GetValue(sPacket, FDS_CODE);
  sErrMsg   := __GetValue(sPacket, FDS_MSG);

end;

{ TProtoGet }

constructor TProtoGet.Create;
begin
  inherited;
end;

// results :
// >= 0 - Ok, parsed succesfully, value means parsed fields number
// -1 - no STX symbol
// -2 - no 134= marker in the header
// -3 - wrong data size in the header
// -4 - too short string
// -5 - data size in the header is not corresponding with a real packet size
// -6 - no 0x01 symbol as header end

function TProtoGet.SplitPacket(instr: string; var outstr: TStringArray): integer;
var
  datasize, pos1 : Integer;
  s1 : string;
begin
  Result := 0;
  SetLength(outstr, 0);

  if Length(instr) < HEADER_SIZE then
  begin
    Result := -4; Exit;
  end;

  if instr[1] <> DEF_STX then
  begin
    Result := -1; Exit;
  end;

  if Copy(instr, 2, 4) <> FDS_PACK_LLEN+'=' then
  begin
    Result := -2; Exit;
  end;

  try
  datasize := StrToInt(Copy(instr, 6, 4));
  except
    Result := -3; Exit;
  end;


  if instr[10] <> DEF_DELIMITER then
  begin
    Result := -6; Exit;
  end;

  if datasize <> Length(instr) - HEADER_SIZE then
  begin
    s1 := Copy(instr, 1, datasize+HEADER_SIZE);
    instr := s1;
    //Result := -5;
    //Exit;
  end;

  s1 := instr;
  Delete(s1, 1, 10);

  while TRUE do
  begin
    pos1 := Pos(DEF_DELIMITER, s1);
    if pos1 <= 0 then Break;
    SetLength(outstr, Length(outstr) + 1);
    outstr[Length(outstr) - 1] := Copy(s1, 1, pos1 - 1);
    Delete(s1, 1, pos1);
  end;

  Result := Length(outstr);
end;



end.
