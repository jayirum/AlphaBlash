object FrmSettingTS: TFrmSettingTS
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'FrmSettingTS'
  ClientHeight = 406
  ClientWidth = 581
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 581
    Height = 406
    Align = alClient
    TabOrder = 0
    object Label1: TLabel
      Left = 316
      Top = 199
      Width = 61
      Height = 13
      Alignment = taRightJustify
      Caption = #44048#49884' '#49884#51089' '#54001
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label3: TLabel
      Left = 316
      Top = 216
      Width = 61
      Height = 13
      Alignment = taRightJustify
      Caption = #44048#49884' '#49884#51089' '#54001
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label6: TLabel
      Left = 316
      Top = 239
      Width = 61
      Height = 13
      Alignment = taRightJustify
      Caption = #44048#49884' '#49884#51089' '#54001
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object GroupBox1: TGroupBox
      Left = 6
      Top = -4
      Width = 289
      Height = 178
      Caption = #49444#51221'1'
      Color = 13434879
      ParentBackground = False
      ParentColor = False
      TabOrder = 0
      object Label47: TLabel
        Left = 57
        Top = 52
        Width = 54
        Height = 13
        Alignment = taRightJustify
        Caption = 'S/L Shift '#54001
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label14: TLabel
        Left = 61
        Top = 84
        Width = 50
        Height = 13
        Alignment = taRightJustify
        Caption = 'T/S Cut '#54001
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label5: TLabel
        Left = 184
        Top = 83
        Width = 35
        Height = 13
        Alignment = taRightJustify
        Caption = 'OffSet '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label10: TLabel
        Left = 86
        Top = 114
        Width = 25
        Height = 13
        Caption = 'SL '#54001
      end
      object chTS1: TCheckBox
        Left = 13
        Top = 25
        Width = 55
        Height = 17
        Caption = 'TS'#51201#50857
        TabOrder = 0
      end
      object btnSave1: TButton
        Left = 192
        Top = 150
        Width = 75
        Height = 25
        Caption = #51200' '#51109
        TabOrder = 1
        OnClick = btnSave1Click
      end
      object edtTSLevel2_1: TEdit
        Left = 117
        Top = 49
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 2
        Text = '10'
        OnClick = edtTSLevel2_1lick
      end
      object edtTSLevel3_1: TEdit
        Left = 117
        Top = 80
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 3
        Text = '10'
        OnClick = edtTSLevel3_1Click
      end
      object edtSLTickA1: TEdit
        Left = 117
        Top = 112
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 4
        Text = '20'
        OnClick = edtSLTickA1Click
      end
      object edtOffSet3_1: TEdit
        Left = 221
        Top = 80
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 5
        Text = '5'
        OnClick = edtOffSet3_1Click
      end
    end
    object GroupBox2: TGroupBox
      Left = 289
      Top = 2
      Width = 289
      Height = 178
      Caption = #49444#51221'2'
      Color = 8454143
      ParentBackground = False
      ParentColor = False
      TabOrder = 1
      object Label2: TLabel
        Left = 65
        Top = 49
        Width = 54
        Height = 13
        Alignment = taRightJustify
        Caption = 'S/L Shift '#54001
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label4: TLabel
        Left = 69
        Top = 79
        Width = 50
        Height = 13
        Alignment = taRightJustify
        Caption = 'T/S Cut '#54001
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label7: TLabel
        Left = 193
        Top = 78
        Width = 35
        Height = 13
        Alignment = taRightJustify
        Caption = 'OffSet '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label12: TLabel
        Left = 94
        Top = 112
        Width = 25
        Height = 13
        Caption = 'SL '#54001
      end
      object chTS2: TCheckBox
        Left = 12
        Top = 25
        Width = 55
        Height = 17
        Caption = 'TS'#51201#50857
        TabOrder = 0
      end
      object btnSave2: TButton
        Left = 210
        Top = 150
        Width = 75
        Height = 25
        Caption = #51200' '#51109
        TabOrder = 1
        OnClick = btnSave2Click
      end
      object edtTSLevel2_2: TEdit
        Left = 125
        Top = 43
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 2
        Text = '10'
        OnClick = edtTSLevel2_2Click
      end
      object edtTSLevel3_2: TEdit
        Left = 125
        Top = 74
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 3
        Text = '10'
        OnClick = edtTSLevel3_2Click
      end
      object edtSLTickA2: TEdit
        Left = 125
        Top = 106
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 4
        Text = '20'
        OnClick = edtSLTickA2Click
      end
      object edtOffSet3_2: TEdit
        Left = 229
        Top = 74
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 5
        Text = '5'
        OnClick = edtOffSet3_2Click
      end
    end
    object GroupBox3: TGroupBox
      Left = 0
      Top = 180
      Width = 289
      Height = 178
      Caption = #49444#51221'3'
      Color = 12058623
      ParentBackground = False
      ParentColor = False
      TabOrder = 2
      object Label11: TLabel
        Left = 57
        Top = 58
        Width = 54
        Height = 13
        Alignment = taRightJustify
        Caption = 'S/L Shift '#54001
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label17: TLabel
        Left = 61
        Top = 84
        Width = 50
        Height = 13
        Alignment = taRightJustify
        Caption = 'T/S Cut '#54001
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label8: TLabel
        Left = 190
        Top = 84
        Width = 35
        Height = 13
        Alignment = taRightJustify
        Caption = 'OffSet '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label13: TLabel
        Left = 83
        Top = 108
        Width = 25
        Height = 13
        Caption = 'SL '#54001
      end
      object chTS3: TCheckBox
        Left = 13
        Top = 28
        Width = 55
        Height = 17
        Caption = 'TS'#51201#50857
        TabOrder = 0
      end
      object btnSave3: TButton
        Left = 214
        Top = 150
        Width = 75
        Height = 25
        Caption = #51200' '#51109
        TabOrder = 1
        OnClick = btnSave3Click
      end
      object edtTSLevel2_3: TEdit
        Left = 123
        Top = 48
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 2
        Text = '10'
        OnClick = edtTSLevel2_3Click
      end
      object edtTSLevel3_3: TEdit
        Left = 123
        Top = 79
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 3
        Text = '10'
        OnClick = edtTSLevel3_3Click
      end
      object edtSLTickA3: TEdit
        Left = 123
        Top = 111
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 4
        Text = '20'
        OnClick = edtSLTickA3Click
      end
      object edtOffSet3_3: TEdit
        Left = 231
        Top = 79
        Width = 46
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 5
        Text = '5'
        OnClick = edtOffSet3_3Click
      end
    end
    object Button2: TButton
      Left = 293
      Top = 366
      Width = 104
      Height = 33
      Caption = #45803#44592
      TabOrder = 3
      OnClick = Button2Click
    end
    object cbSkipClr_1: TComboBox
      Left = 383
      Top = 193
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 4
      Visible = False
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '18'
        '20'
        '22'
        '24'
        '26'
        '28'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50')
    end
    object cbSkipClr_2: TComboBox
      Left = 383
      Top = 214
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 5
      Visible = False
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '18'
        '20'
        '22'
        '24'
        '26'
        '28'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50')
    end
    object cbSkipClr_3: TComboBox
      Left = 383
      Top = 235
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 6
      Visible = False
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '18'
        '20'
        '22'
        '24'
        '26'
        '28'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50')
    end
    object cbTSLevel2_1_bak: TComboBox
      Left = 373
      Top = 285
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 7
      Visible = False
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50')
    end
    object cbTSLevel3_1_bak: TComboBox
      Left = 425
      Top = 285
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 8
      Visible = False
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50'
        '52'
        '54'
        '56'
        '58'
        '60'
        '62'
        '64'
        '66'
        '68'
        '70'
        '72'
        '74'
        '76'
        '78'
        '80'
        '82'
        '84'
        '86'
        '88'
        '90'
        '92'
        '94'
        '96'
        '98'
        '100')
    end
    object cbSLTickA1_bak: TComboBox
      Left = 477
      Top = 285
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 9
      Visible = False
      Items.Strings = (
        '5'
        '6'
        '8'
        '10'
        '12'
        '14'
        '16'
        '18'
        '20'
        '22'
        '24'
        '26'
        '28'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '45'
        '50'
        '55'
        '60'
        '65'
        '70'
        '75'
        '80'
        '85'
        '90'
        '95'
        '100'
        '105'
        '110'
        '115'
        '120'
        '125'
        '130'
        '135'
        '140'
        '145'
        '150'
        '155'
        '160'
        '165'
        '170'
        '175'
        '180'
        '185'
        '190'
        '195'
        '200')
    end
    object cbOffSet3_1_bak: TComboBox
      Left = 529
      Top = 285
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 10
      Visible = False
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50'
        '52'
        '52'
        '56'
        '58'
        '60'
        '62'
        '64'
        '66'
        '68'
        '70')
    end
    object cbTSLevel2_2_bak: TComboBox
      Left = 373
      Top = 312
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 11
      Visible = False
      Items.Strings = (
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50')
    end
    object cbTSLevel3_2_bak: TComboBox
      Left = 425
      Top = 312
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 12
      Visible = False
      Items.Strings = (
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50'
        '52'
        '54'
        '56'
        '58'
        '60'
        '62'
        '64'
        '66'
        '68'
        '70'
        '72'
        '74'
        '76'
        '78'
        '80'
        '82'
        '84'
        '86'
        '88'
        '90'
        '92'
        '94'
        '96'
        '98'
        '100')
    end
    object cbSLTickA2_bak: TComboBox
      Left = 477
      Top = 312
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 13
      Visible = False
      Items.Strings = (
        '5'
        '6'
        '8'
        '10'
        '12'
        '14'
        '16'
        '18'
        '20'
        '22'
        '24'
        '26'
        '28'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '45'
        '50'
        '55'
        '60'
        '65'
        '70'
        '75'
        '80'
        '85'
        '90'
        '95'
        '100'
        '105'
        '110'
        '115'
        '120'
        '125'
        '130'
        '135'
        '140'
        '145'
        '150'
        '155'
        '160'
        '165'
        '170'
        '175'
        '180'
        '185'
        '190'
        '195'
        '200')
    end
    object cbOffSet3_2_bak: TComboBox
      Left = 529
      Top = 312
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 14
      Visible = False
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50'
        '52'
        '52'
        '56'
        '58'
        '60'
        '62'
        '64'
        '66'
        '68'
        '70')
    end
    object cbTSLevel2_3_bak: TComboBox
      Left = 373
      Top = 339
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 15
      Visible = False
      Items.Strings = (
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50')
    end
    object cbTSLevel3_3_bak: TComboBox
      Left = 425
      Top = 339
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 16
      Visible = False
      Items.Strings = (
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50'
        '52'
        '54'
        '56'
        '58'
        '60'
        '62'
        '64'
        '66'
        '68'
        '70'
        '72'
        '74'
        '76'
        '78'
        '80'
        '82'
        '84'
        '86'
        '88'
        '90'
        '92'
        '94'
        '96'
        '98'
        '100')
    end
    object cbSLTickA3_bak: TComboBox
      Left = 477
      Top = 339
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 17
      Visible = False
      Items.Strings = (
        '5'
        '6'
        '8'
        '10'
        '12'
        '14'
        '16'
        '18'
        '20'
        '22'
        '24'
        '26'
        '28'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '45'
        '50'
        '55'
        '60'
        '65'
        '70'
        '75'
        '80'
        '85'
        '90'
        '95'
        '100'
        '105'
        '110'
        '115'
        '120'
        '125'
        '130'
        '135'
        '140'
        '145'
        '150'
        '155'
        '160'
        '165'
        '170'
        '175'
        '180'
        '185'
        '190'
        '195'
        '200')
    end
    object cbOffSet3_3_bak: TComboBox
      Left = 529
      Top = 339
      Width = 46
      Height = 21
      Style = csDropDownList
      TabOrder = 18
      Visible = False
      Items.Strings = (
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19'
        '20'
        '21'
        '22'
        '23'
        '24'
        '25'
        '26'
        '27'
        '28'
        '29'
        '30'
        '32'
        '34'
        '36'
        '38'
        '40'
        '42'
        '44'
        '46'
        '48'
        '50'
        '52'
        '52'
        '56'
        '58'
        '60'
        '62'
        '64'
        '66'
        '68'
        '70')
    end
  end
end
