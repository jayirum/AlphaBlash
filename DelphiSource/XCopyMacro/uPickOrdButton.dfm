object PickButton: TPickButton
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Form1'
  ClientHeight = 156
  ClientWidth = 379
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 56
    Top = 24
    Width = 276
    Height = 18
    Alignment = taCenter
    Caption = #50896#54616#45716' '#51452#47928#48260#53948#51012' '#47560#50864#49828#47196' '#50724#47480#51901' '#53364#47533#54616#49464#50836
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object edX: TEdit
    Left = 100
    Top = 55
    Width = 65
    Height = 21
    Alignment = taCenter
    MaxLength = 3
    NumbersOnly = True
    TabOrder = 0
    OnKeyPress = edXKeyPress
  end
  object edY: TEdit
    Left = 190
    Top = 55
    Width = 65
    Height = 21
    Alignment = taCenter
    MaxLength = 3
    NumbersOnly = True
    TabOrder = 1
    OnKeyPress = edYKeyPress
  end
  object Button1: TButton
    Left = 136
    Top = 117
    Width = 75
    Height = 34
    Caption = 'Close'
    TabOrder = 2
    OnClick = Button1Click
  end
  object pnlXY: TPanel
    Left = 88
    Top = 84
    Width = 185
    Height = 24
    TabOrder = 3
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 379
    Height = 25
    Align = alTop
    TabOrder = 4
  end
  object tmrXY: TTimer
    Enabled = False
    OnTimer = tmrXYTimer
    Left = 272
    Top = 88
  end
end
