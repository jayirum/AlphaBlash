unit EnDecrypt;

interface

uses
	Windows, Messages, Variants, Classes, Graphics, Controls, Dialogs, sysutils,
  Math ;
//
  function ValueToHex(const S: String): String;
  function HexToValue(const S: String) : String;
  function __Encrypt(const S: String):String; //; Key1, Key2, Key3: WORD): String;
  function __Decrypt(const S: String):String; //; Key1, Key2, Key3: WORD): String;



const
  HexaChar: array [0..15] of Char=('0','1','2','3','4','5','6','7','8','9',
                                   'A','B','C','D','E','F');

  KEY_1 = 3;
  KEY_2 = 7;
  KEY_3 = 5;

implementation

// Byte로 구성된 데이터를 Hexadecimal 문자열로 변환
// _____________________________________________________________________________
function ValueToHex(const S: String): String;
var i: Integer;
begin
  SetLength(Result, Length(S)*2); // 문자열 크기를 설정
  for i:=0 to Length(S)-1 do begin
    Result[(i*2)+1]:=HexaChar[Integer(S[i+1]) shr 4];
    Result[(i*2)+2]:=HexaChar[Integer(S[i+1]) and $0f];
  end;
end;

// Hexadecimal로 구성된 문자열을 Byte 데이터로 변환
// _____________________________________________________________________________
function HexToValue(const S: String) : String;
var i: Integer;
begin

  SetLength(Result, Length(S) div 2);
  for i:=0 to (Length(S) div 2)-1 do begin
    Result[i+1] := Char(StrToInt('$'+Copy(S,(i*2)+1, 2)));
  end;
end;


// 암호걸기
// _____________________________________________________________________________
//function Encrypt(const S: String; Key1, Key2, Key3: WORD): String;
function __Encrypt(const S: String): String;
var
  i: Byte;
  FirstResult: String;
  Key1, Key2, Key3: WORD;
begin
  Key1 := KEY_1;
  Key2 := KEY_2;
  Key3 := KEY_3;

  try
    SetLength(FirstResult, Length(S));
    for i:=1 to Length(S) do begin
      FirstResult[i]:=Char(Byte(S[i]) xor (Key1 shr 8));
      Key1          :=(Byte(FirstResult[i])+Key1)*Key2+Key3;
    end;
    Result:=ValueToHex(FirstResult);
  except
    Result := '';
  end;
end;


//var
//  i: Byte;
//  FirstResult: String;
//begin
//  SetLength(FirstResult, Length(S));
//  for i:=1 to Length(S) do begin
//    FirstResult[i]:=Char(Byte(S[i]) xor (Key1 shr 8));
//    Key1          :=(Byte(FirstResult[i])+Key1)*Key2+Key3;
//  end;
//  Result:=ValueToHex(FirstResult);
//end;

// 암호풀기
// _____________________________________________________________________________
function __Decrypt(const S: String): String;
var
  i: Byte;
  FirstResult: String;
  Key1, Key2, Key3: WORD;
begin

  Key1 := KEY_1;
  Key2 := KEY_2;
  Key3 := KEY_3;

  try
    FirstResult:=HexToValue(S);
    SetLength(Result, Length(FirstResult));
    for i:=1 to Length(FirstResult) do begin
      Result[i]:=Char(Byte(FirstResult[i]) xor (Key1 shr 8));
      Key1     :=(Byte(FirstResult[i])+Key1)*Key2+Key3;
    end;
  except
    result := '';

  end;
end;

//function Decrypt(const S: String; Key1, Key2, Key3: WORD): String;
//var
//  i: Byte;
//  FirstResult: String;
//begin
//  FirstResult:=HexToValue(S);
//  SetLength(Result, Length(FirstResult));
//  for i:=1 to Length(FirstResult) do begin
//    Result[i]:=Char(Byte(FirstResult[i]) xor (Key1 shr 8));
//    Key1     :=(Byte(FirstResult[i])+Key1)*Key2+Key3;
//  end;
//end;

end.
