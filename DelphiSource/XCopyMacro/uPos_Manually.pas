unit uPos_Manually;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls;

type
  TFmPosManual = class(TForm)
    Label2: TLabel;
    cbSide: TComboBox;
    cbQty: TComboBox;
    Label3: TLabel;
    Label4: TLabel;
    edtAvg: TEdit;
    cbIdx: TComboBox;
    Button1: TButton;
    rgSide: TRadioGroup;
    rgIdx: TRadioGroup;
    rgQty: TRadioGroup;
    Button2: TButton;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  FmPosManual: TFmPosManual;


  procedure __ShowPosInputBox();

implementation

{$R *.dfm}

uses
  uMain, uCommonDef, uTrailingStop
  ;


procedure __ShowPosInputBox();
begin
  FmPosManual := TFmPosManual.Create(application);
  FmPosManual.ShowModal();
end;


procedure TFmPosManual.Button1Click(Sender: TObject);
var
  idx   : integer;
  side  : string;
  qty   : integer;

begin

  if ( rgIdx.ItemIndex < 0) or
    (rgQty.ItemIndex < 0 ) then
  begin
    showmessage('값을 선택하세요');
    exit;
  end
  ;


  idx := rgIdx.ItemIndex+1;

  qty := rgQty.ItemIndex;

  if qty > 0  then
  begin
    if rgSide.itemindex<0 then
    begin
      showmessage('방향을 선택하세요');
      exit;
    end;

    if strtofloatdef(edtAvg.text,0)=0 then
    begin
      showmessage('평단을 입력하세요');
      exit;
    end;


  end;


  if rgSide.ItemIndex=0 then
    side := SIDE_BUY
  else
    side := SIDE_SELL
  ;


  if qty = 0 then
  begin
    fmMain.gdPosMine.cells[POS_STATUS,idx] := POS_STATUS_NONE;
    fmMain.gdPosMine.cells[POS_SIDE,  idx] := '';
    fmMain.gdPosMine.cells[POS_AVG,   idx] := '';
    fmMain.gdPosMine.cells[POS_QTY,   idx] := '';
    fmMain.gdPosMine.cells[POS_SL_PRC,   idx] := '';

    //fmMain.gdPosMaster.cells[POS_ARTC,  cbIdx.ItemIndex] := m_setting[idx].artcCd.Text;
    //fmMain.gdPosMaster.cells[POS_STATUS,cbIdx.ItemIndex] := POS_STATUS_NONE;
    fmMain.gdPosMaster.cells[POS_SIDE,  idx] := '';
    fmMain.gdPosMaster.cells[POS_AVG,   idx] := '0';
    fmMain.gdPosMaster.cells[POS_QTY,   idx] := '0';
    fmMain.gdPosMaster.cells[POS_TM,    idx] := '';

  end
  else
  begin
    fmMain.gdPosMine.cells[POS_STATUS,idx] := POS_STATUS_OPEN;
    fmMain.gdPosMine.cells[POS_SIDE,  idx] := __Side( side );
    fmMain.gdPosMine.cells[POS_AVG,   idx] := edtAvg.text;
    fmMain.gdPosMine.cells[POS_QTY,   idx] := inttostr(qty);
    fmMain.gdPosMine.cells[POS_OPEN_TICKCOUNT,   idx] := floattostr(GetTickCount());
  end;

  fmMain.gdPosMine.cells[POS_TM,    idx]      := '';
  fmMain.gdPosMine.cells[POS_TS_SLSHIFT, idx] := '';
  fmMain.gdPosMine.cells[POS_TS_CUT, idx]     := '';
  fmMain.gdPosMine.cells[POS_TS_BEST,  idx]   := '';
  fmMain.gdPosMine.cells[POS_SL_TICK,   idx]  := '';
  fmMain.gdPosMine.cells[POS_SL_PRC,   idx]   := '';


  __ts.Init_TS(idx);

  Close;
end;


procedure TFmPosManual.Button2Click(Sender: TObject);
var
  idx : integer;
begin

  for idx := 1 to MAX_STK do
  BEGIN
    fmMain.gdPosMine.cells[POS_STATUS,idx] := POS_STATUS_NONE;
    fmMain.gdPosMine.cells[POS_SIDE,  idx] := '';
    fmMain.gdPosMine.cells[POS_AVG,   idx] := '';
    fmMain.gdPosMine.cells[POS_QTY,   idx] := '';
    fmMain.gdPosMine.cells[POS_OPEN_TICKCOUNT,   idx] := '';
    fmMain.gdPosMine.cells[POS_SL_PRC,   idx] := '';

    fmMain.gdPosMaster.cells[POS_SIDE,  idx] := '';
    fmMain.gdPosMaster.cells[POS_AVG,   idx] := '0';
    fmMain.gdPosMaster.cells[POS_QTY,   idx] := '0';
    fmMain.gdPosMaster.cells[POS_TM,    idx] := '';
  END;
end;

procedure TFmPosManual.FormCreate(Sender: TObject);
begin
  width   := 358;
  height  := 385;
  left := fmMain.btnPosSet1.Left + fmMain.btnPosSet1.Width + 10;
  Top  := fmMain.btnPosSet1.Top;
end;

procedure TFmPosManual.FormShow(Sender: TObject);
begin

end;



//procedure TFmPosManual.Button1Click(Sender: TObject);
//var
//  idx : integer;
//begin
//
//  if (cbIdx.ItemIndex < 0) or (cbSide.ItemIndex<0) or (cbQty.ItemIndex<0) then
//  begin
//    showmessage('값을 선택하세요');
//    exit;
//  end;
//
//  idx := strtoint(cbIdx.Items[cbIdx.ItemIndex] );
//
//  //fmMain.gdPosMine.cells[POS_MASTER,  cbIdx.ItemIndex] := m_setting[idx].master.Text;
//  //fmMain.gdPosMine.cells[POS_ARTC,  cbIdx.ItemIndex] := m_setting[idx].artcCd.Text;
//
//  if strtointdef(cbQty.Items[cbQty.ItemIndex],0) = 0 then
//  begin
//    fmMain.gdPosMine.cells[POS_STATUS,idx] := POS_STATUS_NONE;
//    fmMain.gdPosMine.cells[POS_SIDE,  idx] := '';
//    fmMain.gdPosMine.cells[POS_AVG,   idx] := '';
//    fmMain.gdPosMine.cells[POS_QTY,   idx] := '';
//
//    //fmMain.gdPosMaster.cells[POS_ARTC,  cbIdx.ItemIndex] := m_setting[idx].artcCd.Text;
//    //fmMain.gdPosMaster.cells[POS_STATUS,cbIdx.ItemIndex] := POS_STATUS_NONE;
//    fmMain.gdPosMaster.cells[POS_SIDE,  idx] := '';
//    fmMain.gdPosMaster.cells[POS_AVG,   idx] := '0';
//    fmMain.gdPosMaster.cells[POS_QTY,   idx] := '0';
//    fmMain.gdPosMaster.cells[POS_TM,    idx] := '';
//
//  end
//  else
//  begin
//    fmMain.gdPosMine.cells[POS_STATUS,idx] := POS_STATUS_OPEN;
//    fmMain.gdPosMine.cells[POS_SIDE,  idx] := __Side( cbSide.Items[cbSide.ItemIndex] );
//    fmMain.gdPosMine.cells[POS_AVG,   idx] := edtAvg.text;
//    fmMain.gdPosMine.cells[POS_QTY,   idx] := cbQty.Items[cbQty.ItemIndex];
//  end;
//
//  fmMain.gdPosMine.cells[POS_TM,    idx] := '';
//  fmMain.gdPosMine.cells[POS_TS_LVL_1, idx] := '';
//  fmMain.gdPosMine.cells[POS_TS_LVL_2, idx] := '';
//  fmMain.gdPosMine.cells[POS_TS_LVL_3, idx] := '';
//  fmMain.gdPosMine.cells[POS_TS_BEST,  idx] := '';
//  fmMain.gdPosMine.cells[POS_SL_PRC,   idx] := '';
//
//
//  __ts.Init_TS(idx);
//
//  Close;
//end;
//
//

end.
