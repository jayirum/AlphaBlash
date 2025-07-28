object fmMain: TfmMain
  Left = 0
  Top = 0
  Caption = 'SentosaSuite'
  ClientHeight = 480
  ClientWidth = 826
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsMDIForm
  Menu = MainMenu1
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Shape1: TShape
    Left = 0
    Top = 0
    Width = 826
    Height = 459
    Align = alClient
    ExplicitLeft = 328
    ExplicitTop = 240
    ExplicitWidth = 65
    ExplicitHeight = 65
  end
  object cbMsg: TComboBox
    Left = 0
    Top = 459
    Width = 826
    Height = 21
    Align = alBottom
    TabOrder = 0
    ExplicitLeft = 672
    ExplicitTop = 440
    ExplicitWidth = 145
  end
  object MainMenu1: TMainMenu
    Left = 208
    Top = 24
    object mnuFile: TMenuItem
      Caption = 'File'
    end
    object mnuMD: TMenuItem
      Caption = 'Market Data'
      object subComparison: TMenuItem
        Caption = 'Price Comparison'
        OnClick = subComparisonClick
      end
    end
  end
end
