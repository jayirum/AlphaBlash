unit uPrcGrid;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, AdvUtil, Vcl.Grids, AdvObj, BaseGrid,
  AdvGrid;

type
  TfmPrcGrid = class(TForm)
    gdTick: TAdvStringGrid;
    procedure FormCreate(Sender: TObject);
    procedure gdTickGetAlignment(Sender: TObject; ARow, ACol: Integer;
      var HAlign: TAlignment; var VAlign: TVAlignment);
    procedure FormShow(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
  private
    { Private declarations }
    m_left,m_top : integer;
  public
    { Public declarations }
  end;

var
  fmPrcGrid: TfmPrcGrid;


  procedure __CreatePrcGrid(mainRight:integer; top:integer);
  procedure __ShowPrcGrid(mainRight:integer; top:integer);
  procedure __HidePrcGrid(mainRight:integer; top:integer);

implementation

{$R *.dfm}

uses
  uCommonDef
  ;


procedure __CreatePrcGrid(mainRight:integer; top:integer);
begin
  fmPrcGrid := TfmPrcGrid.Create(application);

//  fmPrcGrid.m_left := mainRight;
//  fmPrcGrid.top    := top;


//  fmPrcGrid.Height    := 265;
//  fmPrcGrid.Width     := 217;
  fmPrcGrid.Left      := mainRight;
  fmPrcGrid.top       := top;
  //fmPrcGrid.Show();

end;

procedure __ShowPrcGrid(mainRight:integer; top:integer);
begin
  if not assigned(fmPrcGrid) then
    __CreatePrcGrid(mainRight, top);

  fmPrcGrid.Show();
end;

procedure __HidePrcGrid(mainRight:integer; top:integer);
begin
  if not assigned(fmPrcGrid) then
    __CreatePrcGrid(mainRight, top);

  fmPrcGrid.Hide();
end;


procedure TfmPrcGrid.FormCreate(Sender: TObject);
begin

  Height    := 265;
  Width     := 217
end;

procedure TfmPrcGrid.FormDestroy(Sender: TObject);
begin
//
end;

procedure TfmPrcGrid.FormShow(Sender: TObject);
begin
     // tick grid
  gdTick.ColWidths[TICK_ARTC] := 45;
  gdTick.Colwidths[TICK_PRC] := 65;
  gdTick.ColWidths[TICK_TM] := 60;

end;

procedure TfmPrcGrid.gdTickGetAlignment(Sender: TObject; ARow, ACol: Integer;
  var HAlign: TAlignment; var VAlign: TVAlignment);
begin
  if Arow = 0 then
    HAlign := taCenter
  else
  begin
         if ACol=TICK_ARTC then  HAlign := taCenter
    else if ACol=TICK_PRC then  HAlign := taRightJustify
    else if ACol=TICK_TM then  HAlign := taCenter;
  end;
end;


end.
