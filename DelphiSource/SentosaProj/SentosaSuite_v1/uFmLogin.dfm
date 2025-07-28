object fmLogin: TfmLogin
  Left = 0
  Top = 0
  Caption = 'Login'
  ClientHeight = 231
  ClientWidth = 505
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 152
    Top = 48
    Width = 57
    Height = 21
    Caption = 'User ID'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 0
    Width = 98
    Height = 40
    Caption = 'Logon '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -33
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 168
    Top = 88
    Width = 31
    Height = 21
    Caption = 'Pwd'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object edtUserID: TEdit
    Left = 223
    Top = 51
    Width = 121
    Height = 21
    TabOrder = 0
  end
  object edtPwd: TEdit
    Left = 223
    Top = 91
    Width = 121
    Height = 21
    TabOrder = 1
  end
  object btnLogon: TButton
    Left = 139
    Top = 128
    Width = 113
    Height = 57
    Caption = 'Go'
    Default = True
    TabOrder = 2
    OnClick = btnLogonClick
  end
  object btnCancel: TButton
    Left = 283
    Top = 128
    Width = 113
    Height = 57
    Caption = 'Close'
    TabOrder = 3
    TabStop = False
    OnClick = btnCancelClick
  end
  object pnlMsg: TPanel
    Left = 0
    Top = 200
    Width = 505
    Height = 31
    Align = alBottom
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
  end
end
