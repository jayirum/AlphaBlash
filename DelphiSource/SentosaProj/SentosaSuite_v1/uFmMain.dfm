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
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Shape1: TShape
    Left = 0
    Top = 35
    Width = 826
    Height = 424
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
  end
  object pnlTop: TPanel
    Left = 0
    Top = 0
    Width = 826
    Height = 35
    Align = alTop
    TabOrder = 1
    object btnLogAuth: TButton
      Left = 8
      Top = 4
      Width = 129
      Height = 25
      Caption = 'LogonAuthenticate'
      TabOrder = 0
      OnClick = btnLogAuthClick
    end
    object Button1: TButton
      Left = 160
      Top = 4
      Width = 97
      Height = 25
      Caption = 'Logon'
      TabOrder = 1
      OnClick = Button1Click
    end
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
  object tmrForLogauth: TTimer
    Enabled = False
    OnTimer = tmrForLogauthTimer
    Left = 96
    Top = 64
  end
  object tmrLogon: TTimer
    Enabled = False
    OnTimer = tmrLogonTimer
    Left = 88
    Top = 128
  end
end
