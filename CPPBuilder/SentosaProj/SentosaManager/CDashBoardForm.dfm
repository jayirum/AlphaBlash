object frmDashBoard: TfrmDashBoard
  Left = 0
  Top = 0
  Caption = 'Dash Board'
  ClientHeight = 632
  ClientWidth = 1103
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lblEaInfo: TLabel
    Left = 200
    Top = 48
    Width = 128
    Height = 19
    Caption = '[ Ea Information ]'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentColor = False
    ParentFont = False
  end
  object Shape1: TShape
    Left = 0
    Top = 0
    Width = 866
    Height = 632
    Align = alClient
    ExplicitLeft = 416
    ExplicitTop = 384
    ExplicitWidth = 65
    ExplicitHeight = 65
  end
  object Label1: TLabel
    Left = 8
    Top = 26
    Width = 115
    Height = 18
    Caption = 'Ea Information '
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
  end
  object pnlAppDetails: TPanel
    Left = 866
    Top = 0
    Width = 237
    Height = 632
    Align = alRight
    Color = clWhite
    ParentBackground = False
    TabOrder = 0
    object Label2: TLabel
      Left = 29
      Top = 96
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Balance'
    end
    object Label3: TLabel
      Left = 36
      Top = 133
      Width = 30
      Height = 13
      Alignment = taRightJustify
      Caption = 'Equity'
    end
    object Label4: TLabel
      Left = 9
      Top = 169
      Width = 57
      Height = 13
      Alignment = taRightJustify
      Caption = 'Free Margin'
    end
    object Label5: TLabel
      Left = 33
      Top = 26
      Width = 33
      Height = 13
      Caption = 'APP ID'
    end
    object Label6: TLabel
      Left = 35
      Top = 60
      Width = 31
      Height = 13
      Caption = 'Broker'
    end
    object Label7: TLabel
      Left = 19
      Top = 585
      Width = 54
      Height = 13
      Caption = 'Logon Time'
      Visible = False
    end
    object Label8: TLabel
      Left = 40
      Top = 205
      Width = 26
      Height = 13
      Alignment = taRightJustify
      Caption = 'Profit'
    end
    object edtAppId: TEdit
      Left = 72
      Top = 23
      Width = 143
      Height = 21
      ReadOnly = True
      TabOrder = 0
    end
    object edtBroker: TEdit
      Left = 72
      Top = 58
      Width = 143
      Height = 21
      ReadOnly = True
      TabOrder = 1
    end
    object edtAccNo: TEdit
      Left = 18
      Top = 405
      Width = 207
      Height = 21
      ReadOnly = True
      TabOrder = 2
      Visible = False
    end
    object edtIP: TEdit
      Left = 18
      Top = 505
      Width = 207
      Height = 21
      ReadOnly = True
      TabOrder = 3
      Visible = False
    end
    object edtMacAddr: TEdit
      Left = 19
      Top = 555
      Width = 207
      Height = 21
      ReadOnly = True
      TabOrder = 4
      Visible = False
    end
    object edtLogonTime: TEdit
      Left = 19
      Top = 598
      Width = 207
      Height = 21
      ReadOnly = True
      TabOrder = 5
      Visible = False
    end
    object btnLogOut: TButton
      Left = 16
      Top = 321
      Width = 75
      Height = 25
      Caption = 'EA LogOut'
      TabOrder = 6
      OnClick = btnLogOutClick
    end
    object edtLiveDemo: TEdit
      Left = 18
      Top = 455
      Width = 207
      Height = 21
      ReadOnly = True
      TabOrder = 7
      Visible = False
    end
    object edtBalance: TEdit
      Left = 72
      Top = 94
      Width = 143
      Height = 21
      ReadOnly = True
      TabOrder = 8
    end
    object edtEquity: TEdit
      Left = 72
      Top = 130
      Width = 143
      Height = 21
      ReadOnly = True
      TabOrder = 9
    end
    object edtFreeMgn: TEdit
      Left = 72
      Top = 166
      Width = 143
      Height = 21
      ReadOnly = True
      TabOrder = 10
    end
    object edtProfit: TEdit
      Left = 72
      Top = 202
      Width = 143
      Height = 21
      ReadOnly = True
      TabOrder = 11
    end
    object btnOrderDetail: TButton
      Left = 16
      Top = 290
      Width = 75
      Height = 25
      Caption = 'Order Details'
      TabOrder = 12
    end
  end
  object gdEAInfo: TAdvStringGrid
    Left = 8
    Top = 50
    Width = 400
    Height = 250
    Cursor = crDefault
    DrawingStyle = gdsClassic
    ScrollBars = ssBoth
    TabOrder = 1
    OnSelectCell = gdEAInfoSelectCell
    HoverRowCells = [hcNormal, hcSelected]
    ActiveCellFont.Charset = DEFAULT_CHARSET
    ActiveCellFont.Color = clWindowText
    ActiveCellFont.Height = -11
    ActiveCellFont.Name = 'Tahoma'
    ActiveCellFont.Style = [fsBold]
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
    ScrollWidth = 16
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
  end
end
