object fmMT4Update: TfmMT4Update
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Update MT4'
  ClientHeight = 244
  ClientWidth = 424
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object sPanel1: TsPanel
    Left = 0
    Top = 0
    Width = 424
    Height = 244
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alClient
    BevelOuter = bvNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object sLabel1: TsLabel
      Left = 88
      Top = 56
      Width = 149
      Height = 16
      Alignment = taCenter
      Caption = 'MT4/5 Terminal  Status : '
    end
    object lblExeStatus: TsLabel
      Left = 243
      Top = 56
      Width = 69
      Height = 16
      Caption = 'lblExeStatus'
    end
    object lblWarning: TsLabel
      Left = 0
      Top = 97
      Width = 424
      Height = 16
      Alignment = taCenter
      AutoSize = False
      Caption = 'warning'
      Visible = False
    end
    object btnUpdate: TsButton
      Left = 126
      Top = 159
      Width = 75
      Height = 25
      Caption = 'Update'
      TabOrder = 0
      OnClick = btnUpdateClick
    end
    object btnNotNow: TsButton
      Left = 224
      Top = 159
      Width = 75
      Height = 25
      Caption = 'Close'
      TabOrder = 1
      OnClick = btnNotNowClick
    end
    object chkRunMT4: TsCheckBox
      Left = 104
      Top = 128
      Width = 14
      Height = 14
      TabOrder = 2
    end
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer1Timer
    Left = 256
    Top = 199
  end
  object IdInterceptThrottler1: TIdInterceptThrottler
    BitsPerSec = 0
    RecvBitsPerSec = 0
    SendBitsPerSec = 0
    Left = 160
    Top = 199
  end
end
