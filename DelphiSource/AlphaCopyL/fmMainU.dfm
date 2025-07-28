object fmMain: TfmMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'AlphaCopy Local'
  ClientHeight = 594
  ClientWidth = 804
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
    Width = 804
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
    object slabel3: TLabel
      Left = 306
      Top = 5
      Width = 55
      Height = 16
      Caption = 'Language'
      Visible = False
    end
    object cbLanguage: TComboBox
      Left = 367
      Top = 3
      Width = 142
      Height = 24
      Style = csDropDownList
      TabOrder = 0
      Visible = False
    end
    object btnClose: TButton
      Left = 701
      Top = 1
      Width = 90
      Height = 25
      Caption = 'Close'
      TabOrder = 1
      TabStop = False
      OnClick = btnCloseClick
    end
  end
  object pgMain: TPageControl
    Left = 0
    Top = 55
    Width = 804
    Height = 539
    ActivePage = tabMainCfg
    Align = alClient
    TabOrder = 1
    OnChange = pgMainChange
    object tabMainTerminal: TTabSheet
      Caption = 'MT4 Terminals'
      object pnlBg1: TPanel
        Left = 0
        Top = 0
        Width = 796
        Height = 508
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
          Left = 34
          Top = 194
          Width = 127
          Height = 16
          Caption = 'Downloading Progress'
          Visible = False
        end
        object lblMsgWait: TLabel
          Left = 28
          Top = 172
          Width = 316
          Height = 16
          Caption = 'Please wait until Alpha Version Status  are confirmed...'
        end
        object pbDownload: TProgressBar
          Left = 166
          Top = 196
          Width = 332
          Height = 17
          TabOrder = 0
          Visible = False
        end
        object gdTerminal: TAdvStringGrid
          Left = 28
          Top = 46
          Width = 661
          Height = 126
          Cursor = crDefault
          ColCount = 4
          DrawingStyle = gdsClassic
          FixedCols = 0
          RowCount = 2
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
          ScrollBars = ssBoth
          TabOrder = 1
          HoverRowCells = [hcNormal, hcSelected]
          OnGetAlignment = gdTerminalGetAlignment
          OnDblClickCell = gdTerminalDblClickCell
          ActiveCellFont.Charset = DEFAULT_CHARSET
          ActiveCellFont.Color = clWindowText
          ActiveCellFont.Height = -11
          ActiveCellFont.Name = 'Tahoma'
          ActiveCellFont.Style = [fsBold]
          ColumnHeaders.Strings = (
            'MT4 Terminal'
            'Alpha Version Status'
            'Alias'
            'M / C')
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
          FixedColWidth = 307
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
            307
            147
            124
            75)
          RowHeights = (
            22
            22)
        end
      end
    end
    object tabMainMC: TTabSheet
      Caption = 'Master/Copier'
      ImageIndex = 1
      object pnlBg2: TPanel
        Left = 0
        Top = 0
        Width = 796
        Height = 508
        Align = alClient
        TabOrder = 0
        object gdMT4List: TAdvStringGrid
          Left = 270
          Top = 38
          Width = 121
          Height = 187
          Cursor = crDefault
          ColCount = 1
          DrawingStyle = gdsClassic
          FixedCols = 0
          RowCount = 6
          ScrollBars = ssNone
          TabOrder = 0
          HoverRowCells = [hcNormal, hcSelected]
          OnGetAlignment = gdMT4ListGetAlignment
          OnClickCell = gdMT4ListClickCell
          ActiveCellFont.Charset = DEFAULT_CHARSET
          ActiveCellFont.Color = clWindowText
          ActiveCellFont.Height = -11
          ActiveCellFont.Name = 'Tahoma'
          ActiveCellFont.Style = [fsBold]
          ColumnHeaders.Strings = (
            'MT4 List')
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
          SortSettings.DefaultFormat = ssAutomatic
          Version = '8.4.2.2'
          ColWidths = (
            64)
          RowHeights = (
            22
            22
            22
            22
            22
            22)
        end
        object gdCopier: TAdvStringGrid
          Left = 502
          Top = 38
          Width = 121
          Height = 187
          Cursor = crDefault
          ColCount = 1
          DrawingStyle = gdsClassic
          FixedCols = 0
          RowCount = 6
          ScrollBars = ssNone
          TabOrder = 1
          HoverRowCells = [hcNormal, hcSelected]
          OnGetAlignment = gdCopierGetAlignment
          OnClickCell = gdCopierClickCell
          ActiveCellFont.Charset = DEFAULT_CHARSET
          ActiveCellFont.Color = clWindowText
          ActiveCellFont.Height = -11
          ActiveCellFont.Name = 'Tahoma'
          ActiveCellFont.Style = [fsBold]
          ColumnHeaders.Strings = (
            'Copiers')
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
          SortSettings.DefaultFormat = ssAutomatic
          Version = '8.4.2.2'
          ColWidths = (
            64)
          RowHeights = (
            22
            22
            22
            22
            22
            22)
        end
        object btnMaster2List: TButton
          Left = 183
          Top = 119
          Width = 75
          Height = 33
          Caption = '>>>'
          TabOrder = 2
          OnClick = btnMaster2ListClick
        end
        object btnList2Master: TButton
          Left = 183
          Top = 63
          Width = 75
          Height = 33
          BiDiMode = bdLeftToRight
          Caption = '<<<'
          ParentBiDiMode = False
          TabOrder = 3
          OnClick = btnList2MasterClick
        end
        object btnCopier2List: TButton
          Left = 413
          Top = 119
          Width = 75
          Height = 33
          BiDiMode = bdLeftToRight
          Caption = '<<<'
          ParentBiDiMode = False
          TabOrder = 4
          OnClick = btnCopier2ListClick
        end
        object btnList2Copier: TButton
          Left = 413
          Top = 63
          Width = 75
          Height = 33
          Caption = '>>>'
          TabOrder = 5
          OnClick = btnList2CopierClick
        end
        object gdMaster: TAdvStringGrid
          Left = 44
          Top = 38
          Width = 121
          Height = 187
          Cursor = crDefault
          ColCount = 1
          DrawingStyle = gdsClassic
          FixedCols = 0
          RowCount = 6
          ScrollBars = ssNone
          TabOrder = 6
          HoverRowCells = [hcNormal, hcSelected]
          OnGetAlignment = gdMasterGetAlignment
          OnClickCell = gdMasterClickCell
          ActiveCellFont.Charset = DEFAULT_CHARSET
          ActiveCellFont.Color = clWindowText
          ActiveCellFont.Height = -11
          ActiveCellFont.Name = 'Tahoma'
          ActiveCellFont.Style = [fsBold]
          ColumnHeaders.Strings = (
            'Master')
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
          SortSettings.DefaultFormat = ssAutomatic
          Version = '8.4.2.2'
          ColWidths = (
            64)
          RowHeights = (
            22
            22
            22
            22
            22
            22)
        end
        object btnSaveMS: TButton
          Left = 274
          Top = 264
          Width = 103
          Height = 41
          Caption = 'Save'
          TabOrder = 7
          OnClick = btnSaveMSClick
        end
        object btnRefreshMC: TButton
          Left = 648
          Top = 0
          Width = 75
          Height = 25
          Caption = 'Refresh'
          TabOrder = 8
          OnClick = btnRefreshMCClick
        end
      end
    end
    object tabMainCfg: TTabSheet
      Caption = 'Config'
      ImageIndex = 2
      object pnlBg3: TPanel
        Left = 0
        Top = 0
        Width = 796
        Height = 508
        Align = alClient
        TabOrder = 0
        object gdCfgMT4: TAdvStringGrid
          Left = 8
          Top = 11
          Width = 78
          Height = 171
          Cursor = crDefault
          ColCount = 1
          DrawingStyle = gdsClassic
          FixedCols = 0
          RowCount = 2
          ScrollBars = ssNone
          TabOrder = 0
          HoverRowCells = [hcNormal, hcSelected]
          OnGetAlignment = gdCfgMT4GetAlignment
          OnClickCell = gdCfgMT4ClickCell
          ActiveCellFont.Charset = DEFAULT_CHARSET
          ActiveCellFont.Color = clWindowText
          ActiveCellFont.Height = -11
          ActiveCellFont.Name = 'Tahoma'
          ActiveCellFont.Style = [fsBold]
          ColumnHeaders.Strings = (
            'MT4 List')
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
          SortSettings.DefaultFormat = ssAutomatic
          Version = '8.4.2.2'
          ColWidths = (
            64)
          RowHeights = (
            22
            22)
        end
        object pgCfg: TPageControl
          Left = 146
          Top = 11
          Width = 639
          Height = 486
          ActivePage = tabMaster
          TabOrder = 1
          OnChange = pgCfgChange
          object tabMaster: TTabSheet
            Caption = 'Master'
            object Label10: TLabel
              Left = 25
              Top = 60
              Width = 58
              Height = 16
              Caption = 'Number : '
            end
            object lblSymbolMCnt: TLabel
              Left = 84
              Top = 60
              Width = 37
              Height = 16
              AutoSize = False
              Caption = '0'
            end
            object Label9: TLabel
              Left = 143
              Top = 76
              Width = 216
              Height = 16
              Caption = '* Double Click to Remove from Grid *'
            end
            object edtSymbolAdd: TEdit
              Left = 16
              Top = 26
              Width = 121
              Height = 24
              Alignment = taCenter
              AutoSelect = False
              TabOrder = 0
              OnKeyPress = edtSymbolAddKeyPress
            end
            object btnSymbolAdd: TButton
              Left = 143
              Top = 26
              Width = 97
              Height = 25
              Caption = 'Add Symbol'
              TabOrder = 1
              OnClick = btnSymbolAddClick
            end
            object edtDebug: TEdit
              Left = 336
              Top = 26
              Width = 121
              Height = 24
              TabOrder = 2
            end
            object gdSymbolM: TAdvStringGrid
              Left = 16
              Top = 76
              Width = 121
              Height = 242
              Cursor = crDefault
              ColCount = 1
              DrawingStyle = gdsClassic
              FixedCols = 0
              RowCount = 2
              ScrollBars = ssVertical
              TabOrder = 3
              HoverRowCells = [hcNormal, hcSelected]
              OnGetAlignment = gdSymbolMGetAlignment
              OnDblClickCell = gdSymbolMDblClickCell
              ActiveCellFont.Charset = DEFAULT_CHARSET
              ActiveCellFont.Color = clWindowText
              ActiveCellFont.Height = -11
              ActiveCellFont.Name = 'Tahoma'
              ActiveCellFont.Style = [fsBold]
              ColumnHeaders.Strings = (
                'Master Symbols')
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
              FixedColWidth = 106
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
              SortSettings.DefaultFormat = ssAutomatic
              Version = '8.4.2.2'
              ColWidths = (
                106)
              RowHeights = (
                22
                22)
            end
            object btnSaveMasterSymbol: TButton
              Left = 24
              Top = 324
              Width = 97
              Height = 27
              Caption = 'Save'
              TabOrder = 4
              OnClick = btnSaveMasterSymbolClick
            end
            object GroupBox1: TGroupBox
              Left = 144
              Top = 131
              Width = 338
              Height = 185
              Caption = ' Notice !'
              TabOrder = 5
              object Label6: TLabel
                Left = 7
                Top = 30
                Width = 321
                Height = 16
                Caption = '# You MUST add the symbols which you want to publish'
              end
              object Label7: TLabel
                Left = 7
                Top = 83
                Width = 247
                Height = 32
                Caption = 
                  '# You MUST write the same symbol codes '#13#10'    with your MT4 exact' +
                  'ly! (case sensitive)'
              end
              object Label8: TLabel
                Left = 7
                Top = 158
                Width = 210
                Height = 16
                Caption = '# At most 20 symbols can be added!'
              end
            end
          end
          object tabCopierSymbols: TTabSheet
            Caption = 'Copier-Symbols'
            ImageIndex = 1
            object pnlCfgC: TPanel
              Left = 0
              Top = 0
              Width = 631
              Height = 455
              Align = alClient
              Color = 8454143
              ParentBackground = False
              TabOrder = 0
              object Label12: TLabel
                Left = 239
                Top = 26
                Width = 42
                Height = 16
                Caption = 'Copier-'
              end
              object lblCopierAlias: TLabel
                Left = 285
                Top = 26
                Width = 35
                Height = 16
                Caption = 'copier'
              end
              object Label2: TLabel
                Left = 34
                Top = 317
                Width = 100
                Height = 16
                AutoSize = False
                Caption = 'Dbl click to move'
              end
              object lblSymbolTotM: TLabel
                Left = 42
                Top = 349
                Width = 30
                Height = 16
                AutoSize = False
                Caption = '0'
                Visible = False
              end
              object Label4: TLabel
                Left = 181
                Top = 317
                Width = 39
                Height = 16
                AutoSize = False
                Caption = 'Total:'
              end
              object lblSymbolTotC: TLabel
                Left = 220
                Top = 317
                Width = 30
                Height = 16
                AutoSize = False
                Caption = '0'
              end
              object Label13: TLabel
                Left = 39
                Top = 24
                Width = 44
                Height = 16
                Caption = 'Master-'
              end
              object lblMasterAlias: TLabel
                Left = 89
                Top = 24
                Width = 40
                Height = 16
                Caption = 'master'
              end
              object gdSymbolC: TAdvStringGrid
                Left = 181
                Top = 48
                Width = 173
                Height = 265
                Cursor = crDefault
                ColCount = 3
                DrawingStyle = gdsClassic
                FixedCols = 0
                RowCount = 2
                Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
                ScrollBars = ssVertical
                TabOrder = 0
                HoverRowCells = [hcNormal, hcSelected]
                OnGetCellColor = gdSymbolCGetCellColor
                OnGetAlignment = gdSymbolCGetAlignment
                OnClickCell = gdSymbolCClickCell
                ActiveCellFont.Charset = DEFAULT_CHARSET
                ActiveCellFont.Color = clWindowText
                ActiveCellFont.Height = -11
                ActiveCellFont.Name = 'Tahoma'
                ActiveCellFont.Style = [fsBold]
                ColumnHeaders.Strings = (
                  'Master'
                  'Copier'
                  'Remove')
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
                SortSettings.DefaultFormat = ssAutomatic
                Version = '8.4.2.2'
                ColWidths = (
                  64
                  64
                  64)
                RowHeights = (
                  22
                  22)
              end
              object btnSaveSymbolC: TButton
                Left = 285
                Top = 361
                Width = 90
                Height = 36
                Caption = 'Save'
                TabOrder = 1
                OnClick = btnSaveSymbolCClick
              end
              object gdSymbolM2: TAdvStringGrid
                Left = 47
                Top = 46
                Width = 80
                Height = 265
                Cursor = crDefault
                ColCount = 1
                DrawingStyle = gdsClassic
                FixedCols = 0
                RowCount = 2
                ScrollBars = ssVertical
                TabOrder = 2
                HoverRowCells = [hcNormal, hcSelected]
                OnGetAlignment = gdSymbolMGetAlignment
                OnDblClickCell = gdSymbolM2DblClickCell
                ActiveCellFont.Charset = DEFAULT_CHARSET
                ActiveCellFont.Color = clWindowText
                ActiveCellFont.Height = -11
                ActiveCellFont.Name = 'Tahoma'
                ActiveCellFont.Style = [fsBold]
                ColumnHeaders.Strings = (
                  'Master')
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
                SortSettings.DefaultFormat = ssAutomatic
                Version = '8.4.2.2'
                ColWidths = (
                  64)
                RowHeights = (
                  22
                  22)
              end
              object btnClrCSymbols: TButton
                Left = 181
                Top = 361
                Width = 90
                Height = 36
                Caption = 'Clear'
                TabOrder = 3
                OnClick = btnClrCSymbolsClick
              end
              object chkUseMCode: TCheckBox
                Left = 416
                Top = 48
                Width = 127
                Height = 17
                Caption = 'Use Master Code'
                TabOrder = 4
                OnClick = chkUseMCodeClick
              end
            end
          end
          object tabCopierCfg: TTabSheet
            Caption = 'Copier-Options'
            ImageIndex = 2
            ExplicitLeft = 0
            ExplicitTop = 0
            ExplicitWidth = 0
            ExplicitHeight = 0
            object ScrollBox1: TScrollBox
              Left = 0
              Top = 0
              Width = 631
              Height = 455
              Align = alClient
              Color = 14090156
              ParentColor = False
              TabOrder = 0
              object GroupBox4: TGroupBox
                Left = 309
                Top = 11
                Width = 290
                Height = 181
                Caption = 'Order type to be copied'
                TabOrder = 0
                object GroupBox7: TGroupBox
                  Left = 15
                  Top = 25
                  Width = 226
                  Height = 45
                  Caption = 'Market Order'
                  TabOrder = 0
                  object chkMktOpen: TCheckBox
                    Left = 24
                    Top = 20
                    Width = 57
                    Height = 17
                    TabStop = False
                    Caption = 'Open'
                    Checked = True
                    State = cbChecked
                    TabOrder = 0
                    OnClick = chkMktOpenClick
                  end
                  object chkMktClose: TCheckBox
                    Left = 114
                    Top = 20
                    Width = 57
                    Height = 17
                    TabStop = False
                    Caption = 'Close'
                    TabOrder = 1
                  end
                end
                object GroupBox8: TGroupBox
                  Left = 15
                  Top = 77
                  Width = 226
                  Height = 90
                  Caption = 'Pending Order'
                  TabOrder = 1
                  object chkLimitOrd: TCheckBox
                    Left = 7
                    Top = 28
                    Width = 201
                    Height = 17
                    TabStop = False
                    Caption = 'LIMIT (Buy Limit / Sell Limit)'
                    TabOrder = 0
                  end
                  object chkStopOrd: TCheckBox
                    Left = 7
                    Top = 57
                    Width = 201
                    Height = 17
                    TabStop = False
                    Caption = 'STOP (Buy Stop / Sell Stop)'
                    TabOrder = 1
                  end
                end
              end
              object GroupBox3: TGroupBox
                Left = 10
                Top = 82
                Width = 283
                Height = 138
                Caption = 'SL / TP'
                TabOrder = 1
                object lblSLSameDist: TLabel
                  Left = 31
                  Top = 87
                  Width = 176
                  Height = 32
                  Caption = 'Use the Same distance (pips) '#13#10'from entry price like on Master'
                  OnClick = lblSLSameDistClick
                end
                object chkSL: TCheckBox
                  Left = 12
                  Top = 32
                  Width = 49
                  Height = 17
                  TabStop = False
                  Caption = 'S/L'
                  TabOrder = 0
                end
                object chkTP: TCheckBox
                  Left = 96
                  Top = 32
                  Width = 49
                  Height = 17
                  TabStop = False
                  Caption = 'T/P'
                  TabOrder = 1
                end
                object rdoSLSamePrc: TRadioButton
                  Left = 12
                  Top = 64
                  Width = 268
                  Height = 17
                  Caption = 'Use the exact same price with Master Price'
                  TabOrder = 2
                  OnClick = rdoSLSamePrcClick
                end
                object rdoSLSameDist: TRadioButton
                  Left = 12
                  Top = 87
                  Width = 17
                  Height = 18
                  TabOrder = 3
                  OnClick = rdoSLSameDistClick
                end
              end
              object GroupBox2: TGroupBox
                Left = 10
                Top = 11
                Width = 283
                Height = 65
                Caption = 'Copy Type'
                TabOrder = 2
                object rdoTrade: TRadioButton
                  Left = 19
                  Top = 27
                  Width = 60
                  Height = 17
                  Caption = 'Trade'
                  TabOrder = 0
                  OnClick = rdoTradeClick
                end
                object rdoSignal: TRadioButton
                  Left = 103
                  Top = 27
                  Width = 113
                  Height = 17
                  Caption = 'Signal'
                  TabOrder = 1
                end
              end
              object GroupBox5: TGroupBox
                Left = 10
                Top = 227
                Width = 283
                Height = 153
                Caption = 'Lots Size'
                TabOrder = 3
                object edtMultiplier: TEdit
                  Left = 96
                  Top = 24
                  Width = 73
                  Height = 24
                  TabStop = False
                  ParentShowHint = False
                  ReadOnly = True
                  ShowHint = True
                  TabOrder = 1
                end
                object rdoFixedLots: TRadioButton
                  Left = 12
                  Top = 53
                  Width = 79
                  Height = 17
                  Caption = 'Fixed Lots'
                  ParentShowHint = False
                  ShowHint = True
                  TabOrder = 2
                  OnClick = rdoFixedLotsClick
                end
                object edtFixedLots: TEdit
                  Left = 96
                  Top = 52
                  Width = 73
                  Height = 24
                  TabStop = False
                  ParentShowHint = False
                  ReadOnly = True
                  ShowHint = True
                  TabOrder = 3
                end
                object chkMaxOneOrd: TCheckBox
                  Left = 12
                  Top = 96
                  Width = 180
                  Height = 17
                  TabStop = False
                  Caption = 'Max lots size for     1 order'
                  ParentShowHint = False
                  ShowHint = True
                  TabOrder = 4
                  OnClick = chkMaxOneOrdClick
                end
                object edtMaxOneOrd: TEdit
                  Left = 198
                  Top = 92
                  Width = 73
                  Height = 24
                  TabStop = False
                  ParentShowHint = False
                  ReadOnly = True
                  ShowHint = True
                  TabOrder = 5
                end
                object chkMaxTotOrd: TCheckBox
                  Left = 12
                  Top = 122
                  Width = 181
                  Height = 17
                  TabStop = False
                  Caption = 'Max lots size for total order'
                  ParentShowHint = False
                  ShowHint = True
                  TabOrder = 6
                  OnClick = chkMaxTotOrdClick
                end
                object edtMaxTotOrd: TEdit
                  Left = 198
                  Top = 121
                  Width = 73
                  Height = 24
                  TabStop = False
                  ParentShowHint = False
                  ReadOnly = True
                  ShowHint = True
                  TabOrder = 7
                end
                object rdoMultiplier: TRadioButton
                  Left = 12
                  Top = 29
                  Width = 78
                  Height = 17
                  Caption = 'Multiplier'
                  ParentShowHint = False
                  ShowHint = True
                  TabOrder = 0
                  OnClick = rdoMultiplierClick
                end
              end
              object GroupBox6: TGroupBox
                Left = 309
                Top = 198
                Width = 290
                Height = 105
                Caption = 'Slippage of Open Order'
                TabOrder = 4
                object Label11: TLabel
                  Left = 100
                  Top = 70
                  Width = 33
                  Height = 16
                  Caption = 'Pip(s)'
                end
                object chkSlippage: TCheckBox
                  Left = 16
                  Top = 16
                  Width = 241
                  Height = 48
                  Caption = 'Max difference between '#13#10'Master'#39's price and current market price'
                  TabOrder = 0
                  OnClick = chkSlippageClick
                end
                object edtSlippage: TEdit
                  Left = 32
                  Top = 66
                  Width = 65
                  Height = 24
                  ReadOnly = True
                  TabOrder = 1
                end
              end
              object btnSaveCopierOptions: TButton
                Left = 309
                Top = 412
                Width = 90
                Height = 36
                Caption = 'Save'
                TabOrder = 5
                OnClick = btnSaveCopierOptionsClick
              end
              object btnLoadCOptions: TButton
                Left = 196
                Top = 412
                Width = 90
                Height = 36
                Caption = 'Refresh'
                TabOrder = 6
                OnClick = btnLoadCOptionsClick
              end
              object GroupBox9: TGroupBox
                Left = 309
                Top = 309
                Width = 292
                Height = 96
                Caption = 'Non Copy TimeOut'
                TabOrder = 7
                object Label3: TLabel
                  Left = 96
                  Top = 22
                  Width = 47
                  Height = 16
                  Caption = 'Timeout'
                end
                object Label5: TLabel
                  Left = 212
                  Top = 22
                  Width = 57
                  Height = 16
                  Caption = 'Minitue(s)'
                end
                object Label14: TLabel
                  Left = 15
                  Top = 57
                  Width = 241
                  Height = 32
                  Caption = 
                    'If a Master order was placed before '#13#10'"Timeout min" , that order' +
                    ' will not copied.'
                end
                object chkNonCopyTimeout: TCheckBox
                  Left = 16
                  Top = 23
                  Width = 54
                  Height = 17
                  Caption = 'Apply '
                  TabOrder = 0
                end
                object edtNonCopyMins: TEdit
                  Left = 156
                  Top = 16
                  Width = 50
                  Height = 24
                  ReadOnly = True
                  TabOrder = 1
                end
              end
            end
          end
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 29
    Width = 804
    Height = 26
    Align = alTop
    TabOrder = 2
    object cbMsg: TComboBox
      Left = 1
      Top = 1
      Width = 802
      Height = 24
      Align = alClient
      Style = csDropDownList
      TabOrder = 0
      TabStop = False
    end
  end
  object tmrInit: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tmrInitTimer
    Left = 852
    Top = 245
  end
  object IdAntiFreeze1: TIdAntiFreeze
    Left = 852
    Top = 293
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
    Left = 852
    Top = 333
  end
  object tmrCompMain: TTimer
    Enabled = False
    OnTimer = tmrCompMainTimer
    Left = 852
    Top = 182
  end
  object tmrUpdateMain: TTimer
    Enabled = False
    OnTimer = tmrUpdateMainTimer
    Left = 848
    Top = 216
  end
  object idSvr: TIdTCPServer
    Bindings = <>
    DefaultPort = 0
    Left = 8
    Top = 352
  end
end
