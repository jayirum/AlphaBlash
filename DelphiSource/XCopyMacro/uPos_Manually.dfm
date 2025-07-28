object FmPosManual: TFmPosManual
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'FmPosManual'
  ClientHeight = 381
  ClientWidth = 342
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 328
    Top = 157
    Width = 25
    Height = 13
    Caption = #48169' '#54693
    Visible = False
  end
  object Label3: TLabel
    Left = 328
    Top = 200
    Width = 25
    Height = 13
    Caption = #49688' '#47049
    Visible = False
  end
  object Label4: TLabel
    Left = 174
    Top = 127
    Width = 25
    Height = 13
    Caption = #54217' '#45800
  end
  object cbSide: TComboBox
    Left = 407
    Top = 192
    Width = 89
    Height = 21
    Style = csDropDownList
    TabOrder = 0
    Visible = False
    Items.Strings = (
      #47588#49688
      #47588#46020)
  end
  object cbQty: TComboBox
    Left = 407
    Top = 232
    Width = 89
    Height = 21
    Style = csDropDownList
    TabOrder = 1
    Visible = False
    Items.Strings = (
      '0'
      '1'
      '2'
      '3'
      '4'
      '5')
  end
  object edtAvg: TEdit
    Left = 205
    Top = 119
    Width = 89
    Height = 21
    TabOrder = 2
  end
  object cbIdx: TComboBox
    Left = 407
    Top = 152
    Width = 89
    Height = 21
    Style = csDropDownList
    TabOrder = 3
    Visible = False
    Items.Strings = (
      '1'
      '2'
      '3')
  end
  object Button1: TButton
    Left = 189
    Top = 157
    Width = 105
    Height = 45
    Caption = #51201'  '#50857
    Default = True
    TabOrder = 4
    OnClick = Button1Click
  end
  object rgSide: TRadioGroup
    Left = 0
    Top = 105
    Width = 166
    Height = 97
    Caption = #54252#51648#49496' '#48169#54693
    Items.Strings = (
      #47588#49688
      #47588#46020)
    TabOrder = 5
  end
  object rgIdx: TRadioGroup
    Left = 0
    Top = 2
    Width = 166
    Height = 97
    Caption = #54252#51648#49496' '#46972#51064' ('#44536#47532#46300' '#49345' '#50948#52824')'
    Items.Strings = (
      #52395#48264#51704#46972#51064
      #46160#48264#51704#46972#51064
      #49464#48264#51704#46972#51064)
    TabOrder = 6
  end
  object rgQty: TRadioGroup
    Left = 172
    Top = 2
    Width = 166
    Height = 97
    Caption = #54252#51648#49496' '#49688#47049
    Items.Strings = (
      '0'
      '1')
    TabOrder = 7
  end
  object Button2: TButton
    Left = 205
    Top = 208
    Width = 75
    Height = 25
    Caption = 'Clear All'
    TabOrder = 8
    OnClick = Button2Click
  end
end
