unit uSignal;

interface

uses
  System.Classes, system.SysUtils, winapi.windows
  , uCommonDef, VCL.dialogs, VCL.Forms , Vcl.StdCtrls, Vcl.ExtCtrls

  ;

const
  MAX_SIGNAL = 3;

  IDX_UP = 0;
  IDX_DN = 1;

type

  TSetting = record
    stk     : ^TComboBox;
    updn    : ^TComboBox;
    basePrc : ^TEdit;
    prevPrc : double;

  end;

  TSignal = class
  private
    m : array[0..MAX_SIGNAL-1] of TSetting;

  public
    constructor Create;
    destructor Destroy; override;
  public
    procedure AddStk(stk:string);
    procedure CheckSignal(stk:string;prc:string);

  end;

var
  __signal : TSignal;

  procedure __CreateSignal();

implementation

uses
  uMain, uNotify
  ;


procedure __CreateSignal();
begin
  __signal := TSignal.create;
end;

constructor TSignal.Create;
begin
  m[0].stk := @fmMain.cbSignalStk1;
  m[0].updn := @fmMain.cbSignalUpDown1;
  m[0].basePrc := @fmMain.edtSignalPrc1;

  m[1].stk := @fmMain.cbSignalStk2;
  m[1].updn := @fmMain.cbSignalUpDown2;
  m[1].basePrc := @fmMain.edtSignalPrc2;

  m[2].stk := @fmMain.cbSignalStk3;
  m[2].updn := @fmMain.cbSignalUpDown3;
  m[2].basePrc := @fmMain.edtSignalPrc3;
end;


destructor TSignal.Destroy;
begin
  //
end;


procedure TSignal.AddStk(stk:string);
var
  i : integer;
begin

  for i := 0 to MAX_SIGNAL-1 do
  begin

    m[i].stk.Items.Add(__Artc(stk));

  end;




end;

procedure TSignal.CheckSignal(stk:string;prc:string);
var
  idx       : integer;
  dBasePrc
  ,dNowPrc  : double;
begin

  for idx := 0 to MAX_SIGNAL-1 do
  BEGIN
    if m[idx].stk.ItemIndex <= 0 then
      continue;

    if m[idx].stk.Items[m[idx].stk.ItemIndex] <> stk then
      continue;

    dBasePrc := strtofloatdef(m[idx].basePrc.Text,0);
    if  dBasePrc <= 0 then
      continue;

    dNowPrc := strtofloatdef(prc,0);

    if (m[idx].updn.ItemIndex = IDX_UP) THEN
    BEGIN

      if (dBasePrc <= dNowPrc)        and
         (m[idx].prevPrc > 0 )        and
         (m[idx].prevPrc <= dNowPrc)  then
      begin
        __SignalAlarm(m[idx].stk.Text, IDX_UP, m[idx].basePrc.Text);
        m[idx].stk.ItemIndex := 0;

        fmMain.EnableSignalAlarmTimer();

        continue;
      end
      ELSE
      begin
        m[idx].prevPrc := dNowPrc;
      end;
    END;

    if (m[idx].updn.ItemIndex = IDX_DN) THEN
    BEGIN
      if (dBasePrc >= dNowPrc)        and
         (m[idx].prevPrc > 0 )        and
         (m[idx].prevPrc >= dNowPrc)  then
      begin
        __SignalAlarm(m[idx].stk.Text, IDX_DN, m[idx].basePrc.Text);
        m[idx].stk.ItemIndex := 0;

        fmMain.EnableSignalAlarmTimer();

        continue;
      end
      ELSE
      begin
        m[idx].prevPrc := dNowPrc;
      end;
    END;



  END;


end;



end.
