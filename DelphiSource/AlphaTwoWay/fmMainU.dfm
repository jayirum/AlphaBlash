object fmMain: TfmMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Alpha2Way'
  ClientHeight = 582
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
    object btnSvrStart: TButton
      Left = 725
      Top = 3
      Width = 107
      Height = 25
      Caption = 'Server Start'
      TabOrder = 0
      OnClick = btnSvrStartClick
    end
    object btnCloseAll: TButton
      Left = 600
      Top = 3
      Width = 99
      Height = 25
      Caption = 'Close All Clients'
      TabOrder = 1
      OnClick = btnCloseAllClick
    end
  end
  object PageControl2: TPageControl
    Left = 0
    Top = 55
    Width = 904
    Height = 527
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
        Height = 496
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
      Caption = 'Postions'
      ImageIndex = 2
      object Panel3: TPanel
        Left = 0
        Top = 0
        Width = 896
        Height = 393
        Align = alTop
        TabOrder = 0
        object Label5: TLabel
          Left = 47
          Top = 15
          Width = 54
          Height = 16
          Caption = 'Symbol A'
        end
        object Label8: TLabel
          Left = 284
          Top = 15
          Width = 65
          Height = 16
          Caption = 'Base PL Pip'
        end
        object Label9: TLabel
          Left = 363
          Top = 15
          Width = 60
          Height = 16
          Caption = 'TS Level 1'
        end
        object Label10: TLabel
          Left = 446
          Top = 14
          Width = 60
          Height = 16
          Caption = 'TS Level 2'
        end
        object Label11: TLabel
          Left = 358
          Top = 71
          Width = 67
          Height = 16
          Caption = 'TS OffSet 1'
        end
        object Label12: TLabel
          Left = 442
          Top = 71
          Width = 67
          Height = 16
          Caption = 'TS OffSet 2'
        end
        object Label4: TLabel
          Left = 123
          Top = 63
          Width = 63
          Height = 16
          Caption = 'BUY Broker'
        end
        object Label16: TLabel
          Left = 205
          Top = 63
          Width = 68
          Height = 16
          Caption = 'SELL Broker'
        end
        object Label2: TLabel
          Left = 4
          Top = 165
          Width = 22
          Height = 16
          Caption = 'BUY'
        end
        object Label3: TLabel
          Left = -1
          Top = 187
          Width = 27
          Height = 16
          Caption = 'SELL'
        end
        object Label7: TLabel
          Left = 547
          Top = 14
          Width = 57
          Height = 16
          Caption = 'Open Lots'
        end
        object Label13: TLabel
          Left = 129
          Top = 15
          Width = 46
          Height = 16
          Caption = 'BUY Key'
        end
        object Label14: TLabel
          Left = 214
          Top = 15
          Width = 51
          Height = 16
          Caption = 'SELL Key'
        end
        object edtBrokerBuy_A: TEdit
          Left = 120
          Top = 85
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 1
          Text = 'OANDA'
        end
        object edtBrokerSell_A: TEdit
          Left = 204
          Top = 85
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 2
          Text = 'IRONFX'
        end
        object edtSymbol_A: TEdit
          Left = 35
          Top = 37
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 0
          Text = 'EURUSD'
        end
        object btnCnfgSave_A: TButton
          Left = 644
          Top = 35
          Width = 91
          Height = 28
          Caption = 'Save'
          TabOrder = 9
          OnClick = btnCnfgSave_AClick
        end
        object gdPos_A: TAdvStringGrid
          Left = 28
          Top = 142
          Width = 800
          Height = 88
          Cursor = crDefault
          ColCount = 19
          DrawingStyle = gdsClassic
          FixedCols = 0
          RowCount = 3
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 10
          HoverRowCells = [hcNormal, hcSelected]
          ActiveCellFont.Charset = DEFAULT_CHARSET
          ActiveCellFont.Color = clWindowText
          ActiveCellFont.Height = -11
          ActiveCellFont.Name = 'Tahoma'
          ActiveCellFont.Style = [fsBold]
          ColumnHeaders.Strings = (
            'Key'
            'Broker'
            'Ticket'
            'Status'
            'Lots'
            'OpenPrc'
            'NowPrc'
            'Spread'
            'PL'
            'PLPip'
            'TS Status'
            'TS Best'
            'TS CutPrc'
            'Sending'
            'TS Lvl_1'
            'TS Lvl_2'
            'Decimal'
            'Pipsize'
            'PtSize')
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
            49
            2
            64
            64
            64
            64
            64
            64
            64
            64
            64
            64)
          RowHeights = (
            22
            22
            22)
        end
        object edtBaseSpread_A: TEdit
          Left = 292
          Top = 37
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 3
          Text = '3'
        end
        object edtLvl_1_A: TEdit
          Left = 363
          Top = 37
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 4
          Text = '4'
        end
        object edtLvl_2_A: TEdit
          Left = 448
          Top = 37
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 6
          Text = '5'
        end
        object edtOffset_1_A: TEdit
          Left = 363
          Top = 85
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 5
          Text = '1'
        end
        object edtOffset_2_A: TEdit
          Left = 448
          Top = 85
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 7
          Text = '2'
        end
        object Button2: TButton
          Left = 725
          Top = 345
          Width = 103
          Height = 28
          Caption = 'Ready to Trade'
          TabOrder = 11
          OnClick = Button2Click
        end
        object edtOpenLots: TEdit
          Left = 551
          Top = 37
          Width = 57
          Height = 24
          Alignment = taCenter
          TabOrder = 8
          Text = '0.01'
        end
        object edtKeyBuy_A: TEdit
          Left = 119
          Top = 37
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 12
          Text = 'KEY1'
        end
        object edtKeySell_A: TEdit
          Left = 204
          Top = 37
          Width = 74
          Height = 24
          Alignment = taCenter
          CharCase = ecUpperCase
          TabOrder = 13
          Text = 'KEY2'
        end
        object edtSpreadMinB: TEdit
          Left = 120
          Top = 115
          Width = 36
          Height = 24
          Alignment = taCenter
          TabOrder = 14
          Text = '0'
        end
        object edtSpreadMinS: TEdit
          Left = 204
          Top = 115
          Width = 36
          Height = 24
          Alignment = taCenter
          TabOrder = 15
          Text = '0'
        end
        object edtSpreadMaxB: TEdit
          Left = 157
          Top = 115
          Width = 36
          Height = 24
          Alignment = taCenter
          TabOrder = 16
          Text = '0'
        end
        object edtSpreadMaxS: TEdit
          Left = 242
          Top = 115
          Width = 36
          Height = 24
          Alignment = taCenter
          TabOrder = 17
          Text = '0'
        end
        object edtNetPL: TEdit
          Left = 328
          Top = 115
          Width = 65
          Height = 24
          Alignment = taCenter
          TabOrder = 18
          Text = '0'
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
  object Button3: TButton
    Left = 836
    Top = 219
    Width = 47
    Height = 25
    Caption = 'Reset'
    TabOrder = 3
    OnClick = Button3Click
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
