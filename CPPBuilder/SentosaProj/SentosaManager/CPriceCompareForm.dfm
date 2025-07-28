object frmPriceCompare: TfrmPriceCompare
  Left = 0
  Top = 0
  Caption = 'frmPriceCompare'
  ClientHeight = 654
  ClientWidth = 1024
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Shape1: TShape
    Left = 0
    Top = 433
    Width = 1024
    Height = 221
    Align = alClient
    ExplicitLeft = 592
    ExplicitTop = 480
    ExplicitWidth = 65
    ExplicitHeight = 65
  end
  object Label2: TLabel
    Left = 928
    Top = 588
    Width = 84
    Height = 19
    Caption = 'Market Data'
    Color = 4259584
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clOlive
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentColor = False
    ParentFont = False
  end
  object cbSymbols: TComboBox
    Left = 26
    Top = 457
    Width = 155
    Height = 21
    Style = csDropDownList
    TabOrder = 0
    OnChange = cbSymbolsChange
  end
  object gdMD: TStringGrid
    Left = 25
    Top = 484
    Width = 825
    Height = 162
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
    TabOrder = 1
    OnSelectCell = gdMDSelectCell
  end
  object edtSymbol: TEdit
    Left = 1022
    Top = 586
    Width = 121
    Height = 21
    ReadOnly = True
    TabOrder = 2
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1024
    Height = 433
    Align = alTop
    Caption = 'Panel1'
    TabOrder = 3
    object Chart1: TChart
      Left = 1
      Top = 1
      Width = 1022
      Height = 403
      Title.Text.Strings = (
        'TChart')
      Align = alTop
      TabOrder = 0
      DefaultCanvas = 'TGDIPlusCanvas'
      ColorPaletteIndex = 13
    end
    object chkUseMktTime: TCheckBox
      Left = 20
      Top = 410
      Width = 97
      Height = 17
      Caption = 'MT5 Time'
      TabOrder = 1
      OnClick = chkUseMktTimeClick
    end
    object chkUseAsk: TCheckBox
      Left = 92
      Top = 410
      Width = 97
      Height = 17
      Caption = 'Use Ask'
      TabOrder = 2
    end
    object btnPrev: TButton
      Left = 392
      Top = 406
      Width = 75
      Height = 25
      Caption = '<<'
      Enabled = False
      TabOrder = 3
      OnClick = btnPrevClick
    end
    object btnNext: TButton
      Left = 486
      Top = 406
      Width = 75
      Height = 25
      Caption = '>>'
      Enabled = False
      TabOrder = 4
      OnClick = btnNextClick
    end
  end
  object btnSubs: TButton
    Left = 209
    Top = 455
    Width = 96
    Height = 25
    Caption = 'Re-Subscribe'
    TabOrder = 4
    OnClick = btnSubsClick
  end
end
