object fmMT4Update: TfmMT4Update
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Update MT4'
  ClientHeight = 91
  ClientWidth = 371
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
  object Label1: TLabel
    Left = 16
    Top = 13
    Width = 142
    Height = 13
    Caption = 'Terminal.exe process status :'
  end
  object lblExeStatus: TLabel
    Left = 168
    Top = 13
    Width = 59
    Height = 13
    Caption = 'lblExeStatus'
  end
  object btnUpdate: TButton
    Left = 280
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Update'
    TabOrder = 0
    OnClick = btnUpdateClick
  end
  object btnNotNow: TButton
    Left = 280
    Top = 39
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 1
  end
  object cbKillExe: TCheckBox
    Left = 16
    Top = 40
    Width = 177
    Height = 17
    Caption = 'Kill terminal.exe before update'
    TabOrder = 2
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 200
    OnTimer = Timer1Timer
    Left = 224
    Top = 48
  end
end
