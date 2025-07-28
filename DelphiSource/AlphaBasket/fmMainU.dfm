object fmMain: TfmMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'AlphaBasket'
  ClientHeight = 684
  ClientWidth = 904
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
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object Label6: TLabel
    Left = 36
    Top = 206
    Width = 42
    Height = 16
    Caption = 'Symbol'
  end
  object sPanel2: TsPanel
    Left = 0
    Top = 0
    Width = 904
    Height = 29
    Align = alTop
    TabOrder = 0
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
    object btnCloseAll: TButton
      Left = 600
      Top = 3
      Width = 99
      Height = 25
      Caption = 'Close All Clients'
      TabOrder = 0
      OnClick = btnCloseAllClick
    end
  end
  object PageControl2: TPageControl
    Left = 0
    Top = 55
    Width = 904
    Height = 629
    ActivePage = TabSheet2
    Align = alClient
    TabOrder = 1
    object tabUpdate: TTabSheet
      Caption = 'Manage MT4/5 Files'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 896
        Height = 598
        Align = alClient
        TabOrder = 0
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
    object TabSheet2: TTabSheet
      Caption = 'Main Page'
      ImageIndex = 2
      object Panel3: TPanel
        Left = 0
        Top = 0
        Width = 896
        Height = 585
        Align = alTop
        TabOrder = 0
        object Label5: TLabel
          Left = 63
          Top = 447
          Width = 54
          Height = 16
          Caption = 'Symbol A'
        end
        object Label8: TLabel
          Left = 300
          Top = 447
          Width = 65
          Height = 16
          Caption = 'Base PL Pip'
        end
        object Label9: TLabel
          Left = 379
          Top = 447
          Width = 60
          Height = 16
          Caption = 'TS Level 1'
        end
        object Label10: TLabel
          Left = 462
          Top = 446
          Width = 60
          Height = 16
          Caption = 'TS Level 2'
        end
        object Label11: TLabel
          Left = 374
          Top = 503
          Width = 67
          Height = 16
          Caption = 'TS OffSet 1'
        end
        object Label12: TLabel
          Left = 458
          Top = 503
          Width = 67
          Height = 16
          Caption = 'TS OffSet 2'
        end
        object Label4: TLabel
          Left = 139
          Top = 495
          Width = 63
          Height = 16
          Caption = 'BUY Broker'
        end
        object Label16: TLabel
          Left = 221
          Top = 495
          Width = 68
          Height = 16
          Caption = 'SELL Broker'
        end
        object Label2: TLabel
          Left = 108
          Top = 3
          Width = 69
          Height = 16
          Caption = 'Market Data'
        end
        object Label3: TLabel
          Left = 108
          Top = 161
          Width = 44
          Height = 16
          Caption = 'Position'
        end
        object Label7: TLabel
          Left = 563
          Top = 446
          Width = 57
          Height = 16
          Caption = 'Open Lots'
        end
        object Label13: TLabel
          Left = 145
          Top = 447
          Width = 46
          Height = 16
          Caption = 'BUY Key'
        end
        object Label14: TLabel
          Left = 230
          Top = 447
          Width = 51
          Height = 16
          Caption = 'SELL Key'
        end
        object edtBrokerBuy_A: TEdit
          Left = 136
          Top = 517
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 1
          Text = 'OANDA'
        end
        object edtBrokerSell_A: TEdit
          Left = 220
          Top = 517
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 2
          Text = 'IRONFX'
        end
        object edtSymbol_A: TEdit
          Left = 51
          Top = 469
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 0
          Text = 'EURUSD'
        end
        object gdPos: TAdvStringGrid
          Left = 108
          Top = 183
          Width = 771
          Height = 256
          Cursor = crDefault
          ColCount = 11
          DrawingStyle = gdsClassic
          RowCount = 14
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 9
          HoverRowCells = [hcNormal, hcSelected]
          ActiveCellFont.Charset = DEFAULT_CHARSET
          ActiveCellFont.Color = clWindowText
          ActiveCellFont.Height = -11
          ActiveCellFont.Name = 'Tahoma'
          ActiveCellFont.Style = [fsBold]
          ColumnHeaders.Strings = (
            'Symbol'
            'Side'
            'Vol'
            'Price'
            'Curr'
            'PL'
            'PL Points'
            'Net PL'
            'Broker'
            'Time'
            'Ticket')
          ControlLook.FixedGradientHoverFrom = clGray
          ControlLook.FixedGradientHoverTo = clWhite
          ControlLook.FixedGradientDownFrom = clGray
          ControlLook.FixedGradientDownTo = clSilver
          ControlLook.DropDownHeader.Font.Charset = DEFAULT_CHARSET
          ControlLook.DropDownHeader.Font.Color = clWindowText
          ControlLook.DropDownHeader.Font.Height = -11
          ControlLook.DropDownHeader.Font.Name = 'Tahoma'
          ControlLook.DropDownHeader.Font.Style = []
          ControlLook.DropDownHeader.Visible = True
          ControlLook.DropDownHeader.Buttons = <>
          ControlLook.DropDownFooter.Font.Charset = DEFAULT_CHARSET
          ControlLook.DropDownFooter.Font.Color = clWindowText
          ControlLook.DropDownFooter.Font.Height = -11
          ControlLook.DropDownFooter.Font.Name = 'Tahoma'
          ControlLook.DropDownFooter.Font.Style = []
          ControlLook.DropDownFooter.Visible = True
          ControlLook.DropDownFooter.Buttons = <>
          DefaultAlignment = taCenter
          Filter = <>
          FilterDropDown.Font.Charset = DEFAULT_CHARSET
          FilterDropDown.Font.Color = clWindowText
          FilterDropDown.Font.Height = -11
          FilterDropDown.Font.Name = 'Tahoma'
          FilterDropDown.Font.Style = []
          FilterDropDown.TextChecked = 'Checked'
          FilterDropDown.TextUnChecked = 'Unchecked'
          FilterDropDownClear = '(All)'
          FilterEdit.TypeNames.Strings = (
            'Starts with'
            'Ends with'
            'Contains'
            'Not contains'
            'Equal'
            'Not equal'
            'Larger than'
            'Smaller than'
            'Clear')
          FixedRowHeight = 22
          FixedFont.Charset = DEFAULT_CHARSET
          FixedFont.Color = clWindowText
          FixedFont.Height = -11
          FixedFont.Name = 'Tahoma'
          FixedFont.Style = [fsBold]
          FloatFormat = '%.2f'
          HoverButtons.Buttons = <>
          HoverButtons.Position = hbLeftFromColumnLeft
          HTMLSettings.ImageFolder = 'images'
          HTMLSettings.ImageBaseName = 'img'
          PrintSettings.DateFormat = 'dd/mm/yyyy'
          PrintSettings.Font.Charset = DEFAULT_CHARSET
          PrintSettings.Font.Color = clWindowText
          PrintSettings.Font.Height = -11
          PrintSettings.Font.Name = 'Tahoma'
          PrintSettings.Font.Style = []
          PrintSettings.FixedFont.Charset = DEFAULT_CHARSET
          PrintSettings.FixedFont.Color = clWindowText
          PrintSettings.FixedFont.Height = -11
          PrintSettings.FixedFont.Name = 'Tahoma'
          PrintSettings.FixedFont.Style = []
          PrintSettings.HeaderFont.Charset = DEFAULT_CHARSET
          PrintSettings.HeaderFont.Color = clWindowText
          PrintSettings.HeaderFont.Height = -11
          PrintSettings.HeaderFont.Name = 'Tahoma'
          PrintSettings.HeaderFont.Style = []
          PrintSettings.FooterFont.Charset = DEFAULT_CHARSET
          PrintSettings.FooterFont.Color = clWindowText
          PrintSettings.FooterFont.Height = -11
          PrintSettings.FooterFont.Name = 'Tahoma'
          PrintSettings.FooterFont.Style = []
          PrintSettings.PageNumSep = '/'
          SearchFooter.FindNextCaption = 'Find &next'
          SearchFooter.FindPrevCaption = 'Find &previous'
          SearchFooter.Font.Charset = DEFAULT_CHARSET
          SearchFooter.Font.Color = clWindowText
          SearchFooter.Font.Height = -11
          SearchFooter.Font.Name = 'Tahoma'
          SearchFooter.Font.Style = []
          SearchFooter.HighLightCaption = 'Highlight'
          SearchFooter.HintClose = 'Close'
          SearchFooter.HintFindNext = 'Find next occurrence'
          SearchFooter.HintFindPrev = 'Find previous occurrence'
          SearchFooter.HintHighlight = 'Highlight occurrences'
          SearchFooter.MatchCaseCaption = 'Match case'
          SearchFooter.ResultFormat = '(%d of %d)'
          ShowDesignHelper = False
          SortSettings.DefaultFormat = ssAutomatic
          Version = '8.4.2.2'
          ColWidths = (
            64
            64
            64
            49
            57
            65
            65
            65
            111
            64
            64)
          RowHeights = (
            22
            22
            22
            22
            22
            22
            22
            22
            22
            22
            22
            22
            22
            22)
        end
        object edtBaseSpread_A: TEdit
          Left = 308
          Top = 469
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 3
          Text = '3'
        end
        object edtLvl_1_A: TEdit
          Left = 379
          Top = 469
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 4
          Text = '4'
        end
        object edtLvl_2_A: TEdit
          Left = 464
          Top = 469
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 6
          Text = '5'
        end
        object edtOffset_1_A: TEdit
          Left = 379
          Top = 517
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 5
          Text = '1'
        end
        object edtOffset_2_A: TEdit
          Left = 464
          Top = 517
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 7
          Text = '2'
        end
        object btnStartTrade: TButton
          Left = -1
          Top = 119
          Width = 103
          Height = 28
          Caption = 'Start Trade'
          TabOrder = 10
          OnClick = btnStartTradeClick
        end
        object edtOpenLots: TEdit
          Left = 567
          Top = 469
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 8
          Text = '0.01'
        end
        object edtKeyBuy_A: TEdit
          Left = 135
          Top = 469
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 11
          Text = 'KEY1'
        end
        object edtKeySell_A: TEdit
          Left = 220
          Top = 469
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 12
          Text = 'KEY2'
        end
        object edtSpreadMinB: TEdit
          Left = 136
          Top = 547
          Width = 36
          Height = 24
          Alignment = taCenter
          TabOrder = 13
          Text = '0'
        end
        object edtSpreadMinS: TEdit
          Left = 220
          Top = 547
          Width = 36
          Height = 24
          Alignment = taCenter
          TabOrder = 14
          Text = '0'
        end
        object edtSpreadMaxB: TEdit
          Left = 173
          Top = 547
          Width = 36
          Height = 24
          Alignment = taCenter
          TabOrder = 15
          Text = '0'
        end
        object edtSpreadMaxS: TEdit
          Left = 258
          Top = 547
          Width = 36
          Height = 24
          Alignment = taCenter
          TabOrder = 16
          Text = '0'
        end
        object edtNetPL: TEdit
          Left = 344
          Top = 547
          Width = 65
          Height = 24
          Alignment = taCenter
          TabOrder = 17
          Text = '0'
        end
        object Button1: TButton
          Left = -1
          Top = 0
          Width = 75
          Height = 25
          Caption = 'Config'
          TabOrder = 18
        end
        object Button6: TButton
          Left = 8
          Top = 163
          Width = 75
          Height = 33
          Caption = 'Stop Trade'
          TabOrder = 19
        end
        object btnSvrStart: TButton
          Left = 0
          Top = 59
          Width = 83
          Height = 38
          Caption = 'Server Start'
          TabOrder = 20
          OnClick = btnSvrStartClick
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 29
    Width = 904
    Height = 26
    Align = alTop
    TabOrder = 2
    object cbMsg: TComboBox
      Left = 1
      Top = 1
      Width = 902
      Height = 24
      Align = alClient
      Style = csDropDownList
      TabOrder = 0
    end
  end
  object btnReset: TButton
    Left = 849
    Top = 642
    Width = 47
    Height = 25
    Caption = 'Reset'
    TabOrder = 3
    OnClick = btnResetClick
  end
  object tmrInitUpdate: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrInitUpdateTimer
    Left = 468
    Top = 5
  end
  object IdAntiFreeze1: TIdAntiFreeze
    Left = 300
    Top = 65533
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
    Left = 372
    Top = 65533
  end
  object tmrCompMain: TTimer
    Enabled = False
    OnTimer = tmrCompMainTimer
    Left = 204
    Top = 65534
  end
  object tmrUpdateMain: TTimer
    Enabled = False
    OnTimer = tmrUpdateMainTimer
    Left = 536
    Top = 16
  end
end
