(*
  packet structure

  STX
  134=DATA길이
  SOH
  DATA
*)


unit ProtoGetExU;

interface

uses
  System.Classes, System.SysUtils, Windows, VCL.Graphics,
  StrUtils, SyncObjs, WinAPI.Messages
  ,uAlphaProtocol
  ;

type TStringArray = Array of string;

const
  SOH_BYTE  = #01;
  STX_BYTE  = #02;

type
    TProtoGetEx = class(TObject)
    private
    protected
      KV    : TKeyValueArray;

      // recordse(Rs) 는 TList 의 Tlist 이다.
      // Inner TList 는 각 row들의 TRsValue 를 다시 list 로 가지고 있다.
      Rs    : TList;
    public
      constructor Create;
      function SplitPacket(instr : string; var oArray : TStringArray) : integer;

      // Parsoing Packet is a virtual function
      // "Abstract" directive modifies the effect of the virtual directive.
      // It means that the current class MUST NOT code the method - it is there only
      // as a placeholder to remind and ensure that derived classes implement it.
      function ParsingPacket(instr : string) : integer; virtual; abstract;

      function PacketCode(sPacket:string):string;
      //function GetValue(sPacket:string; sKey:string):string;

      // Rs
      function BuildRecordSet(instr : string) : integer;
      function RsCount():integer;
      function GetRsValue(idx:integer;sColName:string):string;
      procedure RsClose();
    protected
      function AddRsRow(idx:integer; instr : string; cDeli:char) : integer;

      function SplitIntoArray(instr : string; cDeli:char; var outstr : TStringArray) : integer;
    end;


implementation



function TProtoGetEx.RsCount():integer;
begin
  Result := 0;

  if not assigned(Rs) then
    exit;

  Result := Rs.Count;
end;

function TProtoGetEx.GetRsValue(idx:integer;sColName:string):string;
VAR
  listCols  : TList;
  i         : integer;
  kv        : PTRsValue;
begin

  Result := '';

  if idx >= RsCount() then
    exit;

  {
    [0,USERID,JAY][0,USER_NM,KIM][0,ACC,1234]
    [1,USERID,JAY][1,USER_NM,KIM][1,ACC,1234]
  }
  listCols := Rs.Items[idx];

  for i := 0 to listCols.Count-1 do
  begin
    kv := listCols.Items[i];
    if kv.Key=sColName then
    begin
      Result := kv.Value;
      exit;
    end;
  end;

end;


procedure TProtoGetEx.RsClose();
var
  iRow, iCol: integer;
  listCols  : TList;
begin

  for iRow := 0 to RsCount()-1 do
  begin
    listCols := Rs.Items[iRow];

    for iCol := 0 to listCols.Count-1 do
    begin
      Dispose(listCols.Items[iCol]);
    end;

    FreeAndNil(listCols);
  end;

  FreeAndNil(Rs);

end;

(*
 [USER_ID=JAY]0x5[USER_NM=KIM]0x5[ACC=1234]0x5 0x6
 [USER_ID=KEN]0x5[USER_NM=LEE]0x5[ACC=456]0x5 0x6
 [USER_ID=YOU]0x5[USER_NM=CHO]0x5[ACC=789]0x5 0x6 0x01
*)
function TProtoGetEx.BuildRecordSet(instr : string) : integer;
var
  s1      : string;
  arrRows : TStringArray;
  nRowCnt : integer;
  nColCnt : integer;
  i1,i2   : integer;
begin
  Result := 0;

  if assigned(Rs) then
    FreeAndNil(Rs);

  Rs      := TList.Create;
  SetLength(arrRows,0);

  s1 := instr;

  nRowCnt := SplitIntoArray(s1, DEF_DELI_ARRAY, arrRows);
  if nRowCnt=0 then
  begin
    FreeAndNil(Rs);
    FreeAndNil(arrRows);
    exit;
  end;


  for i1 := 0 to nRowCnt-1 do
  begin

    AddRsRow(i1, arrRows[i1], DEF_DELI_COLUMN);

  end;

  arrRows := nil;


  Result := RsCount();

end;



function TProtoGetEx.AddRsRow(idx:integer; instr : string; cDeli:char) : integer;
var
  findDeli  : integer;
  findEqual : integer;
  sData     : string;
  RsVal     : PTRsValue;
  nCnt      : integer;
  listCols  : TList;
begin
  nCnt := 0;

  while TRUE do
  begin
    findDeli := Pos(cDeli, instr);
    if findDeli <= 0 then Break;

    sData := Copy(instr, 1, findDeli - 1);
    Delete(instr, 1, findDeli);

    findEqual := pos('=', sData);
    if findEqual <=0 then break;

    New(RsVal);
    RsVal.idx    := idx;
    RsVal.Key    := copy(sData, 1, findEqual-1);
    RsVal.Value  := Copy(sData, findEqual+1, length(sData) - findEqual);

    listCols := TList.Create;
    listCols.Add(RsVal);
    nCnt := nCnt + 1;
  end;

  Rs.Add(listCols);

  Result := nCnt;
end;


function TProtoGetEx.PacketCode(sPacket:string):string;
var
  sCodeField  : string;
  find        : integer;
begin
  Result := '';

  sCodeField :=  FDS_CODE + '=';

  find := pos(sCodeField, sPacket);
  if find <=0 then
    exit;


  Result := copy(sPacket, find+4, PACKET_CODE_SIZE );
end;



{ TProtoGetEx }

constructor TProtoGetEx.Create;
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

function TProtoGetEx.SplitPacket(instr: string; var oArray: TStringArray): integer;
var
  datasize, pos1 : Integer;
  s1 : string;
begin
  Result := 0;
  SetLength(oArray, 0);

  if Length(instr) < HEADER_SIZE then
  begin
    Result := -4; Exit;
  end;

  if instr[1] <> STX_BYTE then
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


  if instr[10] <> SOH_BYTE then
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


  Result := SplitIntoArray(s1, SOH_BYTE, oArray);

//  while TRUE do
//  begin
//    pos1 := Pos(SOH_BYTE, s1);
//    if pos1 <= 0 then Break;
//    SetLength(outstr, Length(outstr) + 1);
//    outstr[Length(outstr) - 1] := Copy(s1, 1, pos1 - 1);
//    Delete(s1, 1, pos1);
//  end;
//
//  Result := Length(outstr);
end;



function TProtoGetEx.SplitIntoArray(instr : string; cDeli:char; var outstr : TStringArray) : integer;
var
  pos1 : integer;
begin
  Result := 0;
  while TRUE do
  begin
    pos1 := Pos(cDeli, instr);
    if pos1 <= 0 then Break;

    SetLength(outstr, Length(outstr) + 1);
    outstr[Length(outstr) - 1] := Copy(instr, 1, pos1 - 1);
    Delete(instr, 1, pos1);
  end;

  Result := Length(outstr);
end;
//
//function TProtoGetEx.GetValue(sPacket:string; sKey:string):string;
//var
//  sCodeField  : string;
//  keyIdx      : integer;
//  SOHIdx      : integer;
//  equalIdx    : integer;
//  sTemp       : string;
//  //unitLen     : integer;
//begin
//  Result := '';
//
//  sCodeField :=  sKey + '=';
//
//  keyIdx := pos(sCodeField, sPacket);
//  if keyIdx <=0 then
//    exit;
//
//  sTemp := Copy(sPacket, keyIdx, length(sPacket)-(keyIdx-1));
//  SOHIdx := Pos(SOH_BYTE, sTemp);
//  if SOHIdx<=0 then
//    exit;
//
//  equalIdx := Pos('=', sTemp);
//  if equalIdx<=0 then
//    exit;
//
//  Result := Copy(sTemp, equalIdx+1, SOHIdx-equalIdx-1);
//end;


end.
