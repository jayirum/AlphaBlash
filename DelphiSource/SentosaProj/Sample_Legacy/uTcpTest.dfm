object fmTcpTest: TfmTcpTest
  Left = 0
  Top = 0
  Caption = 'fmTcpTest'
  ClientHeight = 299
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 96
    Top = 48
    Width = 361
    Height = 153
    Lines.Strings = (
      'Memo1')
    TabOrder = 0
  end
  object IdTCPServer1: TIdTCPServer
    OnStatus = IdTCPServer1Status
    Bindings = <>
    DefaultPort = 0
    IOHandler = IdServerIOHandlerStack1
    OnConnect = IdTCPServer1Connect
    OnDisconnect = IdTCPServer1Disconnect
    OnException = IdTCPServer1Exception
    OnExecute = IdTCPServer1Execute
    Left = 488
    Top = 216
  end
  object IdServerIOHandlerStack1: TIdServerIOHandlerStack
    Left = 564
    Top = 240
  end
end
