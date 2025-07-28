unit uPrcList;

interface

uses
  system.classes, windows
  ;

type
  TPRCLIST = class
  private
    m_list : TStringList;
    m_cs   : TRTLCriticalSection;
  public
    constructor Create;
    destructor Destroy; override;
  public
    procedure InsertArtc(artc:string);
    function SavePrc(artc:string; prcFormatted:string):integer;
    function Idx(artc:string):integer;
    function Artc(idx:integer):string;
    function Prc(artc:string):string;
    function Count():integer;


  end;

var
  __prcList : TPRCLIST;

implementation

uses
  uCommonDef
  ;


constructor TPRCLIST.Create;
begin
  m_list := TStringList.Create;
  InitializeCriticalSection(m_cs);
end;


destructor TPRCLIST.Destroy;
begin
  DeleteCriticalSection(m_cs);
  m_list.Free;
end;


function TPRCLIST.Count():integer;
begin
  Result := m_list.Count;
end;

procedure TPRCLIST.InsertArtc(artc:string);
var
  i : integer;
BEGIN

  EnterCriticalSection(m_cs);
  m_list.Values[artc] := '0';
  m_list.sort;
  LeaveCriticalSection(m_cs);

END;


function TPRCLIST.SavePrc(artc:string; prcFormatted:string):integer;
var
  i : integer;
BEGIN

  EnterCriticalSection(m_cs);
  try
    try
      i := m_list.IndexOfName(artc);
      if i>-1 then
      begin
        m_list.ValueFromIndex[i] := prcFormatted;
      end;
      Result := i;
    except
      Result := -1;
    end;
  finally
    LeaveCriticalSection(m_cs);

  end;

END;


function TPRCLIST.Idx(artc:string):integer;
var
  i : integer;
begin
  EnterCriticalSection(m_cs);
  try
    try
      i := m_list.IndexOfName(artc);
      Result := i;
    except
      Result := -1;
    end;
  finally
    LeaveCriticalSection(m_cs);

  end;

end;


function TPRCLIST.Artc(idx:integer):string;
begin
  Result := '';
  if idx < 0 then
    exit;

  EnterCriticalSection(m_cs);
  Try
    Result := m_list.Names[idx];
  Finally
    LeaveCriticalSection(m_cs);
  End;


end;

function TPRCLIST.Prc(artc:string):string;
var
  s : string;
begin
  EnterCriticalSection(m_cs);
  try
    s := m_list.Values[artc];
    Result := s;
  finally
    LeaveCriticalSection(m_cs);
  end;

end;



end.
