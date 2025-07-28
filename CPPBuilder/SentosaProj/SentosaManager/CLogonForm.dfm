object frmLogon: TfrmLogon
  Left = 0
  Top = 0
  Caption = 'Log On'
  ClientHeight = 297
  ClientWidth = 539
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 8
    Width = 88
    Height = 40
    Caption = 'Logon'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -33
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 172
    Top = 72
    Width = 54
    Height = 19
    Caption = 'User ID'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 159
    Top = 120
    Width = 67
    Height = 19
    Caption = 'Password'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object pnlMsg: TPanel
    Left = 0
    Top = 256
    Width = 539
    Height = 41
    Align = alBottom
    TabOrder = 0
  end
  object edtUserID: TEdit
    Left = 236
    Top = 72
    Width = 121
    Height = 21
    TabOrder = 1
    Text = 'test01'
  end
  object edtPwd: TEdit
    Left = 236
    Top = 122
    Width = 121
    Height = 21
    TabOrder = 2
  end
  object btnLogon: TButton
    Left = 165
    Top = 160
    Width = 97
    Height = 41
    Caption = 'Go'
    TabOrder = 3
    OnClick = btnLogonClick
  end
  object btnCancel: TButton
    Left = 285
    Top = 160
    Width = 97
    Height = 41
    Caption = 'Cancel'
    TabOrder = 4
    OnClick = btnCancelClick
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 500
    OnTimer = Timer1Timer
    Left = 448
    Top = 208
  end
  object tmrClose: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrCloseTimer
    Left = 256
    Top = 208
  end
end
