object fmExeUpdate: TfmExeUpdate
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'AlphaBlash update available ...'
  ClientHeight = 122
  ClientWidth = 389
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lbl1: TLabel
    Left = 24
    Top = 16
    Width = 16
    Height = 13
    Caption = 'lbl1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object lbl2: TLabel
    Left = 24
    Top = 35
    Width = 16
    Height = 13
    Caption = 'lbl1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGreen
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object lbl3: TLabel
    Left = 24
    Top = 75
    Width = 16
    Height = 13
    Caption = 'lbl1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGreen
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object lblMsg: TsLabel
    Left = 56
    Top = 24
    Width = 15
    Height = 18
    AutoSize = False
    Caption = '---'
    ParentFont = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
  end
  object btnUpdate: TButton
    Left = 280
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Update'
    TabOrder = 0
    Visible = False
    OnClick = btnUpdateClick
  end
  object btnNotNow: TButton
    Left = 280
    Top = 39
    Width = 75
    Height = 25
    Caption = 'Not now'
    ModalResult = 2
    TabOrder = 1
    Visible = False
    OnClick = btnCancelClick
  end
  object pb1: TProgressBar
    Left = 56
    Top = 72
    Width = 201
    Height = 17
    Step = 1
    TabOrder = 2
    Visible = False
  end
  object btnCancel: TButton
    Left = 280
    Top = 70
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
    Visible = False
    OnClick = btnCancelClick
  end
  object IdHTTP1: TIdHTTP
    OnWork = IdHTTP1Work
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
    Left = 240
    Top = 12
  end
  object VCLUnZip1: TVCLUnZip
    Left = 192
    Top = 96
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 136
    Top = 32
  end
end
