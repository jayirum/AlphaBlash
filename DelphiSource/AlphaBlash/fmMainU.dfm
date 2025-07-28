object fmMain: TfmMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'AlphaBlash'
  ClientHeight = 408
  ClientWidth = 628
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object sPanel2: TsPanel
    Left = 0
    Top = 0
    Width = 628
    Height = 29
    Align = alTop
    TabOrder = 0
    ExplicitWidth = 626
    object Label1: TLabel
      Left = 12
      Top = 4
      Width = 158
      Height = 18
      Caption = 'You Gain, Alpha Enables'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -15
      Font.Name = 'Tahoma'
      Font.Style = [fsItalic]
      ParentFont = False
    end
    object slabel3: TLabel
      Left = 418
      Top = 5
      Width = 55
      Height = 16
      Caption = 'Language'
    end
    object cbLanguage: TComboBox
      Left = 479
      Top = 3
      Width = 142
      Height = 24
      Style = csDropDownList
      TabOrder = 0
      OnChange = cbLanguageChange
    end
  end
  object PageControl2: TPageControl
    Left = 0
    Top = 29
    Width = 628
    Height = 353
    ActivePage = tabUpdate
    Align = alClient
    TabOrder = 1
    ExplicitLeft = 36
    ExplicitTop = 49
    ExplicitWidth = 733
    ExplicitHeight = 368
    object tabUpdate: TTabSheet
      Caption = 'Manage MT4/5 Files'
      ExplicitWidth = 861
      ExplicitHeight = 590
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 620
        Height = 322
        Align = alClient
        TabOrder = 0
        ExplicitLeft = -304
        ExplicitTop = -112
        ExplicitWidth = 894
        ExplicitHeight = 618
        object lblBeSure: TLabel
          Left = 10
          Top = 13
          Width = 281
          Height = 16
          Caption = '>> Please make sure AlphaExperts are latest <<'
        end
        object lblDownload: TLabel
          Left = 10
          Top = 258
          Width = 127
          Height = 16
          Caption = 'Downloading Progress'
        end
        object SG1: TStringGrid
          Left = 10
          Top = 37
          Width = 600
          Height = 198
          ColCount = 2
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goRowSelect]
          TabOrder = 0
          OnDblClick = SG1DblClick
          ColWidths = (
            238
            215)
          RowHeights = (
            24
            24
            24
            24
            24)
        end
        object pbDownload: TProgressBar
          Left = 142
          Top = 260
          Width = 332
          Height = 17
          TabOrder = 1
          Visible = False
        end
        object btnClose: TButton
          Left = 520
          Top = 254
          Width = 90
          Height = 36
          Caption = 'Close'
          TabOrder = 2
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 382
    Width = 628
    Height = 26
    Align = alBottom
    TabOrder = 2
    ExplicitTop = 678
    ExplicitWidth = 902
    object cbMsg: TComboBox
      Left = 1
      Top = 1
      Width = 626
      Height = 24
      Align = alClient
      TabOrder = 0
      ExplicitLeft = 136
      ExplicitTop = 8
      ExplicitWidth = 624
    end
  end
  object tmrInit: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrInitTimer
    Left = 156
    Top = 237
  end
  object IdAntiFreeze1: TIdAntiFreeze
    Left = 220
    Top = 237
  end
  object IdHTTP1: TIdHTTP
    AllowCookies = True
    HandleRedirects = True
    ProxyParams.BasicAuthentication = False
    ProxyParams.ProxyPort = 0
    Request.ContentLength = -1
    Request.ContentRangeEnd = -1
    Request.ContentRangeStart = -1
    Request.ContentRangeInstanceLength = -1
    Request.Accept = 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8'
    Request.BasicAuthentication = False
    Request.UserAgent = 
      'Mozilla/5.0 (Macintosh; Intel Mac OS X x.y; rv:10.0) Gecko/20100' +
      '101 Firefox/10.0'
    Request.Ranges.Units = 'bytes'
    Request.Ranges = <>
    HTTPOptions = [hoForceEncodeParams]
    Left = 292
    Top = 237
  end
  object tmrCompMain: TTimer
    Enabled = False
    OnTimer = tmrCompMainTimer
    Left = 364
    Top = 254
  end
  object tmrUpdateMain: TTimer
    Enabled = False
    OnTimer = tmrUpdateMainTimer
    Left = 424
    Top = 248
  end
end
