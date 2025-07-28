unit ProtoSetU;

interface

uses
  System.Classes, System.SysUtils, Windows, VCL.Graphics,
  StrUtils, SyncObjs, WinAPI.Messages
  ,uAlphaProtocol
  ;

//const SOH_BYTE = #01;
//const STX_BYTE = #02;


type TProtoSet = class(TObject)

    private
      KV : TKeyValueArray;
      function FindIndexByKey(key : string) : integer;
    protected

    public
      constructor Create;
      procedure Start;
      procedure SetVal(nFd : string; val : string); overload;
      procedure SetVal(nFd : string; val : integer); overload;
      procedure SetVal(nFd : string; val : double); overload;

      function Complete(var outstr : string) : integer;
    private
      m_bSetSuccYN : boolean;

end;

implementation

{ TProtoSet }

// return values :
// 0 or bigger - index of found key in array
// -1 - not found
function TProtoSet.FindIndexByKey(key : string) : integer;
var
  i1: Integer;
begin
  Result := -1;
  for i1 := 0 to Length(KV) - 1 do
  begin
    if KV[i1].Key = key then
    begin
       Result := i1;
       Break;
    end;
  end;
end;

procedure TProtoSet.SetVal(nFd: string; val: string);
var index : integer;
begin
  index := FindIndexByKey(nFd);
  if index <> -1 then
  begin
    KV[index].Value := val;
    KV[index].Key := nFd;
  end
  else
  begin
    SetLength(KV, Length(KV) + 1);
    index := Length(KV) - 1;
    KV[index].Value := val;
    KV[index].Key := nFd;
  end;

  if nFd=FDS_SUCC_YN then
    m_bSetSuccYN := true;

end;

procedure TProtoSet.SetVal(nFd:string; val: integer);
var index : integer;
begin
  index := FindIndexByKey(nFd);
  if index <> -1 then
  begin
    KV[index].Value := IntToStr(val);
    KV[index].Key := nFd;
  end
  else
  begin
    SetLength(KV, Length(KV) + 1);
    index := Length(KV) - 1;
    KV[index].Value := IntToStr(val);
    KV[index].Key := nFd;
  end;
end;

procedure TProtoSet.SetVal(nFd: string; val: double);
var index : integer;
begin
  index := FindIndexByKey(nFd);
  if index <> -1 then
  begin
    KV[index].Value := FloatToStr(val);
    KV[index].Key := nFd;
  end
  else
  begin
    SetLength(KV, Length(KV) + 1);
    index := Length(KV) - 1;
    KV[index].Value := FloatToStr(val);
    KV[index].Key := nFd;
  end;
end;

procedure TProtoSet.Start;
//var index : integer;
begin
  SetLength(KV, 0);
  m_bSetSuccYN := false;
end;

{ TProtoSet }

function TProtoSet.Complete(var outstr: string): integer;
var
  i1: Integer;
  sizestr : string;

begin

  // FDS_SUCC_YN 을 Set 하지 않았으면 default 로 Y 설정한다.
	if (m_bSetSuccYN=false) then
		SetVal(FDS_SUCC_YN, 'Y');

  outstr := '';
  for i1 := 0 to Length(KV) - 1 do
  begin
    outstr := outstr + KV[i1].Key + '=' +
    KV[i1].Value + DEF_DELIMITER; //SOH_BYTE;
  end;

  sizestr := IntToStr(Length(outstr));
  while Length(sizestr) < 4 do
  sizestr := '0' + sizestr;

  outstr := DEF_STX(*STX_BYTE*) + FDS_PACK_LLEN+'=' + sizestr + DEF_DELIMITER(*SOH_BYTE*) + outstr;
  Result := length(outstr);
end;

constructor TProtoSet.Create;
begin
  Start;
  inherited;
end;

end.
