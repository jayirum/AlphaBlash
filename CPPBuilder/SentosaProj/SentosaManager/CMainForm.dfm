object frmMain: TfrmMain
  Left = 0
  Top = 0
  Caption = 'Sentosa Manager'
  ClientHeight = 700
  ClientWidth = 1124
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsMDIForm
  Menu = MainMenu
  OldCreateOrder = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Shape1: TShape
    Left = 0
    Top = 57
    Width = 1124
    Height = 643
    Align = alClient
    ExplicitTop = 35
    ExplicitWidth = 1093
    ExplicitHeight = 480
  end
  object tbAtiveWindows: TToolBar
    Left = 0
    Top = 0
    Width = 1124
    Height = 33
    ButtonHeight = 27
    Caption = 'tbAtiveWindows'
    Color = clWhite
    ParentColor = False
    TabOrder = 0
    object btnDashBoard: TButton
      Left = 0
      Top = 0
      Width = 77
      Height = 27
      Caption = 'Dash Board'
      TabOrder = 0
      OnClick = btnDashBoardClick
    end
    object btnPriceCompare: TButton
      Left = 77
      Top = 0
      Width = 132
      Height = 27
      Caption = 'Price Comparison'
      TabOrder = 1
      OnClick = btnPriceCompareClick
    end
    object btnPosOrd: TButton
      Left = 209
      Top = 0
      Width = 104
      Height = 27
      Caption = 'Positions && Orders'
      TabOrder = 2
      OnClick = btnPosOrdClick
    end
    object btnLogon: TButton
      Left = 313
      Top = 0
      Width = 75
      Height = 27
      Caption = 'Logon'
      TabOrder = 3
      OnClick = btnLogonClick
    end
  end
  object cbMsg: TComboBox
    Left = 0
    Top = 33
    Width = 1124
    Height = 24
    Align = alTop
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
  end
  object Memo1: TMemo
    Left = 24
    Top = 448
    Width = 585
    Height = 193
    Lines.Strings = (
      'Memo1')
    ScrollBars = ssBoth
    TabOrder = 2
    Visible = False
  end
  object Panel1: TPanel
    Left = 584
    Top = 328
    Width = 185
    Height = 41
    Caption = 'Panel1'
    Color = 16777115
    ParentBackground = False
    TabOrder = 3
    Visible = False
  end
  object btnCloseApp: TButton
    Left = 960
    Top = 120
    Width = 75
    Height = 25
    Caption = 'CloseApp'
    TabOrder = 4
    Visible = False
    OnClick = btnCloseAppClick
  end
  object MainMenu: TMainMenu
    Left = 48
    Top = 208
    object mnuFile: TMenuItem
      Caption = 'File'
      object itemClose: TMenuItem
        Caption = 'Close'
        OnClick = itemCloseClick
      end
    end
    object mnuMD: TMenuItem
      Caption = 'MarketData'
      object itemPriceComare: TMenuItem
        Caption = 'PriceComparison'
      end
    end
  end
  object tmrLogonAuth: TTimer
    Enabled = False
    OnTimer = tmrLogonAuthTimer
    Left = 136
    Top = 208
  end
  object tmrDashBoard: TTimer
    Enabled = False
    OnTimer = tmrDashBoardTimer
    Left = 392
    Top = 248
  end
  object tmrMsgRed: TTimer
    Enabled = False
    Interval = 5000
    OnTimer = tmrMsgRedTimer
    Left = 368
    Top = 512
  end
  object tmrTerminate: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrTerminateTimer
    Left = 504
    Top = 528
  end
end
