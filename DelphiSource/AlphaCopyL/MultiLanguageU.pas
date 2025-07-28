unit MultiLanguageU;

interface

uses Windows, Messages, System.SysUtils, System.Variants, System.Classes, Graphics,
  Controls, Forms, Dialogs;

const LANG_NUMBER = 2;

type TOneLanguage = record
  FileName : string;
  LangName : string;
  RawFile : TStringList;
end;

type
  TMultiLanguage = class
  private
    Languages : Array[0.. LANG_NUMBER - 1] of TOneLanguage;


  public
    LangNumber : integer;
    ActiveLangIndex : integer;
    function Initialize : integer;
    function GetTranslatedText(key: string): string;
    function GetLanguageName(index : integer) : string;
    procedure SetActiveLangIndex(index: integer);
  end;

implementation

uses fmMainU;

{ TMultiLanguage }

procedure TMultiLanguage.SetActiveLangIndex(index : integer);
begin

  ActiveLangIndex := index;

end;

function TMultiLanguage.GetLanguageName(index : integer) : string;
begin
  Result := Languages[index].LangName;
end;

// return text by key name for active language
// if key not found - return "-----" to see you have some untranslated things
// for this language
function TMultiLanguage.GetTranslatedText(key : string) : string;
var
  i1: Integer;
  s1 : string;
begin
  for i1 := 0 to Languages[ActiveLangIndex].RawFile.Count - 1 do
  begin
    if Copy(Languages[ActiveLangIndex].RawFile[i1], 1, Length(key) + 1) = key + '=' then
    begin
      s1 := Languages[ActiveLangIndex].RawFile[i1];
      Delete(s1, 1, Length(key) + 1);
      Result := Trim(s1);
      Exit;
    end;
  end;
  Result := '-----';
end;

// Result : -1 is some error, 0  - OK
function TMultiLanguage.Initialize : integer;
var
  i1: Integer;
begin
  Result := -1;

  Languages[0].FileName := fmMain.m_ExeFolder + '\LangFiles\Eng.txt';
  Languages[0].LangName := 'English';
  Languages[1].FileName := fmMain.m_ExeFolder + '\LangFiles\Kor.txt';
  Languages[1].LangName := 'Korean';

  for i1 := 0 to LANG_NUMBER - 1 do
  begin
    if not FileExists(Languages[i1].FileName) then Exit;

    Languages[i1].RawFile := TStringList.Create;
    try
      //Languages[i1].RawFile.LoadFromFile(Languages[i1].FileName);
      Languages[i1].RawFile.LoadFromFile(Languages[i1].FileName, TEncoding.Unicode);
    except
       Exit;
    end;
  end;

  LangNumber := LANG_NUMBER;
  ActiveLangIndex := 0;

  Result := 0;
end;

end.
