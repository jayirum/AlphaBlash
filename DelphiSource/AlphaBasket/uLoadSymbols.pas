unit uLoadSymbols;

interface

uses
  System.Classes, uAlphaProtocol, windows, system.SysUtils
  ,uBasketCommon, IniFiles, commonutils, uDatahandler, fmMainU
  ;



  procedure LoadSymbol;


implementation


procedure LoadSymbol;

VAR
  symbolIni : TIniFile;
  fileName  : string;
  i : integer;
  sIdx    : string;
  sLine   : string;

  arrStr  : TStringList;
  sCount  : string;

begin
  fileName := 'symbols.ini';

  sCount :=  __Get_CFGValue('SYMBOLS', 'COUNT', fileName, GetCurrentDir() );
  fmMain.AddMsg(true, Format('AddSymbol Count:%s', [sCount] ));

  for i := 0 to strtoint(sCount)-1 do
  begin
    sIdx := inttostr(i);
    sLine :=  __Get_CFGValue('SYMBOLS', sIdx, fileName, GetCurrentDir() );

    arrStr := TStringList.Create;

    __Split('/', sLine, arrStr);

    __dataHandler.Add_Symbol( i,
                              arrStr[0],
                              strtofloat(arrStr[1]),
                              strtoint(arrStr[2]),
                              strtoint(arrStr[3]),
                              strtoint(arrStr[4]),
                              strtoint(arrStr[5]),
                              strtoint(arrStr[6]),
                              strtoint(arrStr[7])
                              );
    fmMain.AddMsg(true, Format( 'AddSymbol[%d](%s)(Pipsize:%.5f)(DecimalCnt:%d)(MaxSpread:%d)'+
                                '(TSPtOpen:%d)(TSPtClose:%d)(CommPt:%d)(TargetPt:%d)',
                                 [
                                 i,
                                 arrStr[0],
                                 strtofloat(arrStr[1]),
                                 strtoint(arrStr[2]),
                                 strtoint(arrStr[3]),
                                  strtoint(arrStr[4]),
                                  strtoint(arrStr[5]),
                                  strtoint(arrStr[6]),
                                  strtoint(arrStr[7])

                                 ] ));

    arrStr.Free;

  end;
end;

end.
