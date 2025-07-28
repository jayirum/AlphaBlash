unit uSettingTS;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls, Vcl.ExtCtrls
  ,uCommonDef, CommonUtils
  ;


type



//  TSettingCtrls = record
//    tsChecked   : ^TCheckBox;
//    tsLvl_1     : ^TComboBox;
//    tsLvl_2     : ^TComboBox;
//    tsLvl_3     : ^TComboBox;
//    tsOffset_2  : ^TComboBox;
//    tsOffset_3  : ^TComboBox;
//
//    slChecked   : ^TCheckBox;
//    slTickA     : ^TComboBox;
//  end;
  TSettingCtrls = record
    tsChecked     : ^TCheckBox;
    tsSkipClrTick : ^TComboBox;
    slShiftTick   : ^TEdit;
    tsPTTick      : ^TEdit;
    tsOffset      : ^TEdit;
    slTick        : ^TEdit;
  end;

  TFrmSettingTS = class(TForm)
    Panel1: TPanel;
    GroupBox1: TGroupBox;
    chTS1: TCheckBox;
    GroupBox2: TGroupBox;
    chTS2: TCheckBox;
    GroupBox3: TGroupBox;
    chTS3: TCheckBox;
    Label47: TLabel;
    Label14: TLabel;
    Label11: TLabel;
    Label17: TLabel;
    Button2: TButton;
    Label2: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    btnSave1: TButton;
    btnSave2: TButton;
    btnSave3: TButton;
    Label10: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label1: TLabel;
    cbSkipClr_1: TComboBox;
    Label3: TLabel;
    cbSkipClr_2: TComboBox;
    Label6: TLabel;
    cbSkipClr_3: TComboBox;
    cbTSLevel2_1_bak: TComboBox;
    edtTSLevel2_1: TEdit;
    cbTSLevel3_1_bak: TComboBox;
    edtTSLevel3_1: TEdit;
    edtSLTickA1: TEdit;
    cbSLTickA1_bak: TComboBox;
    cbOffSet3_1_bak: TComboBox;
    edtOffSet3_1: TEdit;
    cbTSLevel2_2_bak: TComboBox;
    cbTSLevel3_2_bak: TComboBox;
    cbSLTickA2_bak: TComboBox;
    cbOffSet3_2_bak: TComboBox;
    cbTSLevel2_3_bak: TComboBox;
    cbTSLevel3_3_bak: TComboBox;
    cbSLTickA3_bak: TComboBox;
    cbOffSet3_3_bak: TComboBox;
    edtTSLevel2_2: TEdit;
    edtTSLevel3_2: TEdit;
    edtSLTickA2: TEdit;
    edtOffSet3_2: TEdit;
    edtTSLevel2_3: TEdit;
    edtTSLevel3_3: TEdit;
    edtSLTickA3: TEdit;
    edtOffSet3_3: TEdit;
    procedure FormCreate(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure ReadConfig();
    procedure SaveConfig(i:integer);

    procedure Button2Click(Sender: TObject);
    procedure btnSave1Click(Sender: TObject);
    procedure btnSave2Click(Sender: TObject);
    procedure btnSave3Click(Sender: TObject);
    procedure edtTSLevel2_1lick(Sender: TObject);
    procedure edtTSLevel3_1Click(Sender: TObject);
    procedure edtSLTickA1Click(Sender: TObject);
    procedure edtTSLevel2_2Click(Sender: TObject);
    procedure edtTSLevel3_2Click(Sender: TObject);
    procedure edtSLTickA2Click(Sender: TObject);
    procedure edtOffSet3_2Click(Sender: TObject);
    procedure edtTSLevel2_3Click(Sender: TObject);
    procedure edtTSLevel3_3Click(Sender: TObject);
    procedure edtSLTickA3Click(Sender: TObject);
    procedure edtOffSet3_3Click(Sender: TObject);
    procedure edtOffSet3_1Click(Sender: TObject);


  private
    { Private declarations }
    function  Verify_Save(idx:integer):boolean;
    procedure SaveSetting(idx:integer);

  public
    { Public declarations }
    m_setting     : array [1..MAX_STK] of TSettingCtrls;


  end;

var
  fmTS: TFrmSettingTS;
  //__settingTs : ARRAY[1..MAX_STK] OF TTSSETTING;

  procedure __CreateTSsetting(x,y:integer; bShow:boolean);
  function __IsCreated():boolean;
  function __IsTSChecked(idx:integer):boolean;
  function __TSUse_Desc(idx:integer):string;
  function __TSLevel_Tick(idx:integer; tsLvl:TTS_STATUS):double;
  function __OffSet_Tick(idx:integer):double;

  function __SLTick(idx:integer):double;


implementation

uses
  uMain, uTrailingStop;

{$R *.dfm}




function __IsCreated():boolean;
begin
  Result := assigned(fmTS);
end;


function __TSUse_Desc(idx:integer):string;
BEGIN
  Result := 'TS 미설정';
  if fmTS.m_setting[idx].tsChecked.Checked then
    Result := 'TS 설정';
END;

//function __SLUse_Desc(idx:integer):string;
//begin
//  Result := 'SL 미설정';
//  if fmTS.m_setting[idx].slChecked.Checked then
//    Result := 'SL 설정';
//end;



function __OffSet_Tick(idx:integer):double;
BEGIN
  Result := 0;
  if not assigned(fmTS) then
    exit;

  Result := strtofloatdef(fmTS.m_setting[idx].tsOffset.Text,0);
END;

procedure __CreateTSsetting(x,y:integer; bShow:boolean);
begin
  if not assigned(fmTS) then
    fmTS := TFrmSettingTS.Create(application);

  if bShow=False then
    exit;

  fmTS.Left := x+fmTS.Width;
  fmTS.Top  := y;
  fmTS.Show();
end;


function __IsTSChecked(idx:integer):boolean;
BEGIN
  Result := false;
  if not assigned(fmTS) then
    exit;

  Result := fmTS.m_setting[idx].tschecked.Checked;
END;

//function __IsSLChecked(idx:integer):boolean;
//begin
//  Result := fmTS.m_setting[idx].slChecked.Checked;
//end;



function __TSLevel_Tick(idx:integer; tsLvl:TTS_STATUS):double;
var
  s : string;
begin

//  if tsLvl = TS_SKIPCLR then
//    s := fmTS.m_Setting[idx].tsSkipClrTick.Items[ fmTS.m_Setting[idx].tsSkipClrTick.ItemIndex ]

  if tsLvl = TS_SLSHIFT then
    s := fmTS.m_Setting[idx].slShiftTick.Text
    //s := fmTS.m_Setting[idx].slShiftTick.Items[ fmTS.m_Setting[idx].slShiftTick.ItemIndex ]

  else if tsLvl = TS_CUT then
    s := fmTS.m_Setting[idx].tsPTTick.Text;
    //s := fmTS.m_Setting[idx].tsPTTick.Items[ fmTS.m_Setting[idx].tsPTTick.ItemIndex ]
  ;

  Result := strtofloatdef(s,0);
end;


function __OffSet_TSTick(idx:integer):double;
var
  s : string;
begin
  Result := 0;
  if not assigned(fmTS) then
    exit;

  s := fmTS.m_Setting[idx].tsOffset.Text;

  Result := strtofloatdef(s,0);
end;



function __SLTick(idx:integer):double;
var
  s : string;
begin
  s := fmTS.m_Setting[idx].slTick.Text;
  Result := strtofloatdef(s,0);
end;



procedure TFrmSettingTS.SaveSetting(idx:integer);
begin

  SaveConfig(idx);

  if fmTS.m_setting[idx].tsChecked.Checked = True then
    __ts.Init_TS(idx);

  if idx=1 then
  begin
    fmMain.lbsTSstatus1.Caption := __TSUse_Desc(idx) ;
//    fmMain.lblOffSet1_1.Caption := '['+fmTS.m_setting[idx].tsOffset_2.Items[fmTS.m_setting[idx].tsOffset_2.ItemIndex]+']';
//    fmMain.lblOffSet2_1.Caption := '['+fmTS.m_setting[idx].tsOffset_3.Items[fmTS.m_setting[idx].tsOffset_3.ItemIndex]+']';
  end
  else if idx=2 then
  begin
    fmMain.lbsTSstatus2.Caption := __TSUse_Desc(idx) ;
//    fmMain.lblOffSet1_2.Caption := '['+fmTS.m_setting[idx].tsOffset_2.Items[fmTS.m_setting[idx].tsOffset_2.ItemIndex]+']';
//    fmMain.lblOffSet2_2.Caption := '['+fmTS.m_setting[idx].tsOffset_3.Items[fmTS.m_setting[idx].tsOffset_3.ItemIndex]+']';
  end
  else if idx=3 then
  begin
    fmMain.lbsTSstatus3.Caption := __TSUse_Desc(idx) ;
//    fmMain.lblOffSet1_3.Caption := '['+fmTS.m_setting[idx].tsOffset_2.Items[fmTS.m_setting[idx].tsOffset_2.ItemIndex]+']';
//    fmMain.lblOffSet2_3.Caption := '['+fmTS.m_setting[idx].tsOffset_3.Items[fmTS.m_setting[idx].tsOffset_3.ItemIndex]+']';
  end
  ;


end;

procedure TFrmSettingTS.btnSave1Click(Sender: TObject);
var
  i : integer;
begin

  i := 1;
  SaveSetting(i);
end;

procedure TFrmSettingTS.btnSave2Click(Sender: TObject);
var
  i : integer;
begin

  i := 2;
  SaveSetting(i);

end;

procedure TFrmSettingTS.btnSave3Click(Sender: TObject);
var
  i : integer;
begin

  i := 3;
  SaveSetting(i);

end;

procedure TFrmSettingTS.Button2Click(Sender: TObject);
begin
  hide;
end;

procedure TFrmSettingTS.edtOffSet3_1Click(Sender: TObject);
begin
edtOffSet3_1.Text := '';
end;

procedure TFrmSettingTS.edtOffSet3_2Click(Sender: TObject);
begin
edtOffSet3_2.Text:='';

end;

procedure TFrmSettingTS.edtOffSet3_3Click(Sender: TObject);
begin
edtOffSet3_3.Text:='';

end;

procedure TFrmSettingTS.edtSLTickA1Click(Sender: TObject);
begin
   edtSLTickA1.Text := '';
end;

procedure TFrmSettingTS.edtSLTickA2Click(Sender: TObject);
begin
edtSLTickA2.Text:='';

end;

procedure TFrmSettingTS.edtSLTickA3Click(Sender: TObject);
begin
edtSLTickA3.Text:='';

end;

procedure TFrmSettingTS.edtTSLevel2_1lick(Sender: TObject);
begin
  edtTSLevel2_1.Text := '';

end;

procedure TFrmSettingTS.edtTSLevel2_2Click(Sender: TObject);
begin
edtTSLevel2_2.Text := '';
end;

procedure TFrmSettingTS.edtTSLevel2_3Click(Sender: TObject);
begin
edtTSLevel2_3.Text:='';

end;

procedure TFrmSettingTS.edtTSLevel3_1Click(Sender: TObject);
begin
  edtTSLevel3_1.Text := '';
end;

procedure TFrmSettingTS.edtTSLevel3_2Click(Sender: TObject);
begin
edtTSLevel3_2.Text := '';

end;

procedure TFrmSettingTS.edtTSLevel3_3Click(Sender: TObject);
begin
edtTSLevel3_3.Text:='';

end;

//
//procedure TFrmSettingTS.Button1Click(Sender: TObject);
//var
//  i : integer;
//begin
//
//  SaveConfig();
//
//  for i :=1 to MAX_STK do
//  begin
//    if fmTS.m_setting[i].tsChecked.Checked = True then
//      __ts.Init_TS(i);
//
//  end;
//
//
//  //
//  fmMain.lbsTSstatus1.Caption := __TSUse_Desc(1) ;
//  fmMain.lbsTSstatus2.Caption := __TSUse_Desc(2) ;
//  fmMain.lbsTSstatus3.Caption := __TSUse_Desc(3) ;
//
//  fmMain.lblSLstatus1.Caption := __SLUse_Desc(1) ;
//  fmMain.lblSLstatus2.Caption := __SLUse_Desc(2) ;
//  fmMain.lblSLstatus3.Caption := __SLUse_Desc(3) ;
//
//
//  hide;
//end;



function TFrmSettingTS.Verify_Save(idx:integer):boolean;
begin
  Result := true;
  exit;


  if m_Setting[idx].tschecked.Checked = true then
  begin
    if ( fmMain.m_setting[idx].master.ItemIndex=-1) or
        (fmMain.m_setting[idx].artcNm.ItemIndex=-1)
    then
    begin
      fmMain.AddMsg('Copy ID 와 품목을 먼저 선택하세요', true);
      exit;
    end;

//    if (m_Setting[idx].tsstart.ItemIndex=-1) or
//        (m_Setting[idx].tsstep.ItemIndex=-1) or
//        (m_Setting[idx].tscutrate.ItemIndex=-1)
//    then
//    begin
//      fmMain.AddMsg('T/S 값들은 먼저 선택하세요',true);
//      exit;
//    end;
  end;

end;


procedure TFrmSettingTS.FormCreate(Sender: TObject);
Begin

  m_Setting[1].tschecked      := @chTs1;
  m_Setting[1].tsSkipClrTick  := @cbSkipClr_1;
  m_Setting[1].slShiftTick    := @edtTSLevel2_1;
  m_Setting[1].tsPTTick       := @edtTSLevel3_1;
  m_Setting[1].tsOffset       := @edtOffSet3_1;
  m_Setting[1].slTick         := @edtSLTickA1;

  m_Setting[2].tschecked      := @chTs2;
  m_Setting[2].tsSkipClrTick  := @cbSkipClr_2;
  m_Setting[2].slShiftTick    := @edtTSLevel2_2;
  m_Setting[2].tsPTTick       := @edtTSLevel3_2;
  m_Setting[2].tsOffset       := @edtOffSet3_2;
  m_Setting[2].slTick         := @edtSLTickA2;

  m_Setting[3].tschecked      := @chTs3;
  m_Setting[3].tsSkipClrTick  := @cbSkipClr_3;
  m_Setting[3].slShiftTick    := @edtTSLevel2_3;
  m_Setting[3].tsPTTick       := @edtTSLevel3_3;
  m_Setting[3].tsOffset       := @edtOffSet3_3;
  m_Setting[3].slTick         := @edtSLTickA3;

end;

procedure TFrmSettingTS.FormShow(Sender: TObject);
begin
  ReadConfig();
end;

procedure TFrmSettingTS.ReadConfig();
var
  s : string;
  i : integer;
  key : string;
  idx : integer;
  i2  : integer;
begin
  for i := 1 to MAX_STK do
  begin
    // TS적용
    key := format('TS%d_USE', [i]);
    s := __Get_CFGFile('TSSL', key, '', False, __CnfgName);
    m_Setting[i].tschecked.checked := (s='Y');

    //
    key := format('TS%d_SKIPCLR', [i]);
    s := __Get_CFGFile('TSSL', key, '', False, __CnfgName);

    idx := 0;
    for i2 := 0 to m_Setting[i].tsSkipClrTick.Items.Count -1 do
    begin
      if m_Setting[i].tsSkipClrTick.Items[i2] = s then
      begin
        idx := i2;
        break;
      end;
    end;
    m_Setting[i].tsSkipClrTick.itemindex := idx;


    //tsSkipClrTick
    key := format('TS%d_SLSHIFT', [i]);
    s := __Get_CFGFile('TSSL', key, '', False, __CnfgName);
    m_Setting[i].slShiftTick.Text := s;

    //
    key := format('TS%d_PT', [i]);
    s := __Get_CFGFile('TSSL', key, '', False, __CnfgName);
    m_Setting[i].tsPTTick.Text := s;

    //
    key := format('TS%d_OFFSET', [i]);
    s := __Get_CFGFile('TSSL', key, '', False, __CnfgName);
    m_Setting[i].tsOffset.Text := s;

    //
    key := format('SL%d_TICK', [i]);
    s := __Get_CFGFile('TSSL', key, '', False, __CnfgName);
    m_Setting[i].slTick.Text := s;

  end;
end;




procedure TFrmSettingTS.SaveConfig(i:integer);
var
  s : string;
  key : string;
begin

    Verify_Save(i);

    // TS적용
    s := 'N';
    if m_Setting[i].tsChecked.Checked=true then
      s := 'Y';

    key := format('TS%d_USE', [i]);
    __Set_CFGFile('TSSL', key, s, False, __CnfgName);

    /// skip clr
    s := m_Setting[i].tsSkipClrTick.Items[ m_Setting[i].tsSkipClrTick.ItemIndex ];
    key := format('TS%d_SKIPCLR', [i]);
    __Set_CFGFile('TSSL', key, s, False, __CnfgName);

    /// SL SHIFT
    s := m_Setting[i].slShiftTick.Text;
    key := format('TS%d_SLSHIFT', [i]);
    __Set_CFGFile('TSSL', key, s, False, __CnfgName);


    /// PROFIT TAKING
    s := m_Setting[i].tsPTTick.Text;
    key := format('TS%d_PT', [i]);
    __Set_CFGFile('TSSL', key, s, False, __CnfgName);

    /// Offset
    s := m_Setting[i].tsOffset.Text;
    key := format('TS%d_OFFSET', [i]);
    __Set_CFGFile('TSSL', key, s, False, __CnfgName);


    /// SL
    s := m_Setting[i].slTick.Text;
    key := format('SL%d_TICK', [i]);
    __Set_CFGFile('TSSL', key, s, False, __CnfgName);


    // TS 스레드에 통보
    if m_Setting[i].tschecked.Checked = True then
      __ts.Init_TS(i)
    else
      __ts.Clear_TS(i);

end;



end.
