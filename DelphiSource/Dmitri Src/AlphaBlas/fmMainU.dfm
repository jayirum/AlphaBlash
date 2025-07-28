object fmMain: TfmMain
  Left = 0
  Top = 0
  Caption = 'AlphaBlash'
  ClientHeight = 299
  ClientWidth = 684
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 684
    Height = 299
    ActivePage = tsUpdate
    Align = alClient
    TabOrder = 0
    ExplicitWidth = 635
    object tsUpdate: TTabSheet
      Caption = 'EA Update and Apply'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 627
      ExplicitHeight = 0
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 676
        Height = 271
        Align = alClient
        TabOrder = 0
        ExplicitWidth = 627
        object SG1: TStringGrid
          Left = 16
          Top = 16
          Width = 606
          Height = 225
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goRowSelect]
          TabOrder = 0
          OnDblClick = SG1DblClick
          ColWidths = (
            64
            64
            64
            64
            64)
          RowHeights = (
            24
            24
            24
            24
            24)
        end
      end
    end
    object tsLogin: TTabSheet
      Caption = 'Login'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 627
      ExplicitHeight = 0
    end
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
    Left = 532
    Top = 216
  end
  object IdAntiFreeze1: TIdAntiFreeze
    Left = 460
    Top = 216
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer1Timer
    Left = 396
    Top = 216
  end
end
