object fmMain: TfmMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'XCopyMacro 1.0'
  ClientHeight = 923
  ClientWidth = 769
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlBottom: TsPanel
    Left = 0
    Top = 0
    Width = 769
    Height = 23
    Align = alTop
    TabOrder = 0
    object cbMsg: TsComboBox
      Left = 1
      Top = 1
      Width = 767
      Height = 21
      Align = alClient
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Color = clWhite
      Style = csDropDownList
      ItemIndex = -1
      TabOrder = 0
    end
  end
  object pnlTop: TPanel
    Left = 0
    Top = 23
    Width = 769
    Height = 29
    Align = alTop
    TabOrder = 1
    object lblLastLogonoffTime: TLabel
      Left = 217
      Top = 6
      Width = 91
      Height = 18
      AutoSize = False
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentColor = False
      ParentFont = False
    end
    object Label16: TLabel
      Left = 6
      Top = 7
      Width = 49
      Height = 16
      Caption = 'ID '#47532#49828#53944
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object cbMasterIDs: TComboBox
      Left = 61
      Top = 4
      Width = 89
      Height = 21
      Style = csDropDownList
      TabOrder = 2
      OnChange = cbMasterIDsChange
    end
    object pnlLoginTp: TPanel
      Left = 151
      Top = 4
      Width = 63
      Height = 22
      ParentCustomHint = False
      BevelKind = bkFlat
      BevelOuter = bvNone
      BiDiMode = bdLeftToRight
      Color = clWhite
      Ctl3D = True
      DoubleBuffered = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentBiDiMode = False
      ParentBackground = False
      ParentCtl3D = False
      ParentDoubleBuffered = False
      ParentFont = False
      ParentShowHint = False
      ShowHint = False
      TabOrder = 1
    end
    object btnConn: TButton
      Left = 533
      Top = 2
      Width = 84
      Height = 25
      Caption = #48708#48128#48264#54840' '#49849#51064
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnClick = btnConnClick
    end
    object edPwd: TEdit
      Left = 457
      Top = 4
      Width = 69
      Height = 21
      Alignment = taCenter
      PasswordChar = '*'
      TabOrder = 3
      Text = 'edPwd'
      OnKeyPress = edPwdKeyPress
    end
    object chkMute1: TCheckBox
      Left = 294
      Top = 5
      Width = 100
      Height = 17
      Caption = #52404#44208#49688#49888' Mute'
      TabOrder = 4
    end
  end
  object sbBgSetting: TScrollBox
    Left = 0
    Top = 76
    Width = 769
    Height = 298
    Align = alTop
    TabOrder = 2
    object Label32: TLabel
      Left = 1002
      Top = 218
      Width = 22
      Height = 16
      Caption = #49688#47049
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label35: TLabel
      Left = 1082
      Top = 218
      Width = 22
      Height = 16
      Caption = #48169#54693
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label30: TLabel
      Left = 1000
      Top = 244
      Width = 22
      Height = 16
      Caption = #49688#47049
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label37: TLabel
      Left = 1080
      Top = 244
      Width = 22
      Height = 16
      Caption = #48169#54693
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label36: TLabel
      Left = 998
      Top = 191
      Width = 22
      Height = 16
      Caption = #49688#47049
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label41: TLabel
      Left = 1082
      Top = 191
      Width = 22
      Height = 16
      Caption = #48169#54693
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label38: TLabel
      Left = 851
      Top = 173
      Width = 22
      Height = 16
      Caption = #49688#47049
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label29: TLabel
      Left = 836
      Top = 199
      Width = 37
      Height = 16
      Caption = #54001' '#44032#52824
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label39: TLabel
      Left = 855
      Top = 233
      Width = 22
      Height = 16
      Caption = #49688#47049
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label31: TLabel
      Left = 840
      Top = 257
      Width = 37
      Height = 16
      Caption = #54001' '#44032#52824
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label40: TLabel
      Left = 929
      Top = 239
      Width = 22
      Height = 16
      Caption = #49688#47049
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label33: TLabel
      Left = 915
      Top = 266
      Width = 37
      Height = 16
      Caption = #54001' '#44032#52824
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label28: TLabel
      Left = 915
      Top = 289
      Width = 37
      Height = 16
      Caption = #45800#53440' '#52488
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Visible = False
    end
    object Label9: TLabel
      Left = 453
      Top = 241
      Width = 33
      Height = 13
      Caption = #44592#51456#44032
    end
    object Label11: TLabel
      Left = 572
      Top = 241
      Width = 22
      Height = 14
      Caption = #54001#49688
    end
    object Label13: TLabel
      Left = 608
      Top = 157
      Width = 13
      Height = 13
      Caption = 'Up'
    end
    object Label14: TLabel
      Left = 608
      Top = 181
      Width = 13
      Height = 13
      Caption = 'Dn'
    end
    object GroupBox2: TGroupBox
      Left = 1
      Top = 2
      Width = 345
      Height = 139
      Caption = '<'#49444#51221'1>'
      Color = clBtnFace
      ParentBackground = False
      ParentColor = False
      TabOrder = 0
      object Label10: TLabel
        Left = 111
        Top = 49
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label12: TLabel
        Left = 111
        Top = 76
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label42: TLabel
        Left = 111
        Top = 101
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object lbsTSstatus1: TLabel
        Left = 247
        Top = 53
        Width = 33
        Height = 13
        Caption = #48120#49444#51221
      end
      object Label19: TLabel
        Left = 183
        Top = 103
        Width = 37
        Height = 16
        Caption = #45800#53440' '#52488
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object edbuyx1: TEdit
        Left = 67
        Top = 100
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 4
      end
      object edbuyy1: TEdit
        Left = 120
        Top = 100
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 5
      end
      object edsellx1: TEdit
        Left = 67
        Top = 48
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 6
      end
      object edselly1: TEdit
        Left = 120
        Top = 48
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 7
      end
      object edclrx1: TEdit
        Left = 67
        Top = 74
        Width = 44
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 8
      end
      object edclry1: TEdit
        Left = 120
        Top = 74
        Width = 44
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 9
      end
      object Button1: TButton
        Left = 3
        Top = 100
        Width = 62
        Height = 21
        Caption = #47588#49688#48260#53948
        TabOrder = 1
        OnClick = Button1Click
      end
      object Button2: TButton
        Left = 3
        Top = 47
        Width = 62
        Height = 21
        Caption = #47588#46020#48260#53948
        TabOrder = 2
        OnClick = Button2Click
      end
      object Button3: TButton
        Left = 3
        Top = 74
        Width = 62
        Height = 21
        Caption = #52397#49328#48260#53948
        TabOrder = 3
        OnClick = Button3Click
      end
      object cbStk1: TComboBox
        Left = 115
        Top = 18
        Width = 97
        Height = 21
        Style = csDropDownList
        TabOrder = 0
        OnChange = cbStk1Change
      end
      object cbMasterId1: TComboBox
        Left = 4
        Top = 18
        Width = 97
        Height = 21
        Style = csDropDownList
        TabOrder = 10
        OnChange = cbMasterId1Change
      end
      object chkRvs1: TCheckBox
        Left = 218
        Top = 20
        Width = 62
        Height = 17
        Caption = 'Reverse'
        TabOrder = 11
      end
      object edArtc1: TEdit
        Left = 374
        Top = 180
        Width = 17
        Height = 21
        TabOrder = 12
        Visible = False
      end
      object btnTS1: TButton
        Left = 183
        Top = 48
        Width = 62
        Height = 21
        Caption = 'TS '#49444#51221
        TabOrder = 13
        OnClick = btnTS1Click
      end
      object cbScalping1: TComboBox
        Left = 230
        Top = 101
        Width = 48
        Height = 21
        Style = csDropDownList
        TabOrder = 14
      end
      object chkAddPos1: TCheckBox
        Left = 183
        Top = 78
        Width = 97
        Height = 17
        Caption = #47932#53440#44592' '#54728#50857
        TabOrder = 15
      end
      object chkClrYN1: TCheckBox
        Left = 269
        Top = 78
        Width = 97
        Height = 17
        Caption = #52397#49328#54728#50857
        Checked = True
        State = cbChecked
        TabOrder = 16
      end
    end
    object GroupBox5: TGroupBox
      Left = 3
      Top = 148
      Width = 345
      Height = 139
      Caption = '<'#49444#51221'3>'
      Color = clBtnFace
      ParentBackground = False
      ParentColor = False
      TabOrder = 1
      object Label20: TLabel
        Left = 110
        Top = 49
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label22: TLabel
        Left = 111
        Top = 76
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label46: TLabel
        Left = 111
        Top = 99
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object lbsTSstatus3: TLabel
        Left = 247
        Top = 50
        Width = 33
        Height = 13
        Caption = #48120#49444#51221
      end
      object Label8: TLabel
        Left = 181
        Top = 101
        Width = 37
        Height = 16
        Caption = #45800#53440' '#52488
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object edbuyx3: TEdit
        Left = 66
        Top = 98
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 0
      end
      object edbuyy3: TEdit
        Left = 119
        Top = 98
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 1
      end
      object edsellx3: TEdit
        Left = 67
        Top = 48
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 2
      end
      object edselly3: TEdit
        Left = 119
        Top = 48
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 3
      end
      object edclrx3: TEdit
        Left = 67
        Top = 73
        Width = 44
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 4
      end
      object edclry3: TEdit
        Left = 119
        Top = 73
        Width = 44
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 5
      end
      object Button7: TButton
        Left = 4
        Top = 98
        Width = 62
        Height = 21
        Caption = #47588#49688#48260#53948
        TabOrder = 7
        OnClick = Button7Click
      end
      object Button8: TButton
        Left = 3
        Top = 49
        Width = 62
        Height = 21
        Caption = #47588#46020#48260#53948
        TabOrder = 8
        OnClick = Button8Click
      end
      object Button9: TButton
        Left = 3
        Top = 73
        Width = 62
        Height = 21
        Caption = #52397#49328#48260#53948
        TabOrder = 9
        OnClick = Button9Click
      end
      object cbStk3: TComboBox
        Left = 115
        Top = 16
        Width = 97
        Height = 21
        Style = csDropDownList
        TabOrder = 6
        OnChange = cbStk3Change
      end
      object cbMasterId3: TComboBox
        Left = 4
        Top = 16
        Width = 97
        Height = 21
        Style = csDropDownList
        TabOrder = 10
        OnChange = cbMasterId3Change
      end
      object chkRvs3: TCheckBox
        Left = 218
        Top = 17
        Width = 62
        Height = 17
        Caption = 'Reverse'
        TabOrder = 11
      end
      object edArtc3: TEdit
        Left = 381
        Top = 194
        Width = 17
        Height = 21
        TabOrder = 12
        Visible = False
      end
      object btnTS3: TButton
        Left = 181
        Top = 44
        Width = 62
        Height = 21
        Caption = 'TS '#49444#51221
        TabOrder = 13
        OnClick = btnTS3Click
      end
      object cbScalping3: TComboBox
        Left = 228
        Top = 98
        Width = 49
        Height = 21
        Style = csDropDownList
        TabOrder = 14
      end
      object chkAddPos3: TCheckBox
        Left = 181
        Top = 75
        Width = 97
        Height = 17
        Caption = #47932#53440#44592' '#54728#50857
        TabOrder = 15
      end
      object chkClrYN3: TCheckBox
        Left = 260
        Top = 75
        Width = 97
        Height = 17
        Caption = #52397#49328#54728#50857
        Checked = True
        State = cbChecked
        TabOrder = 16
      end
    end
    object GroupBox3: TGroupBox
      Left = 373
      Top = 2
      Width = 345
      Height = 139
      Caption = '<'#49444#51221'2>'
      Color = clBtnFace
      ParentBackground = False
      ParentColor = False
      TabOrder = 2
      object Label15: TLabel
        Left = 109
        Top = 48
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label17: TLabel
        Left = 109
        Top = 78
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label45: TLabel
        Left = 109
        Top = 102
        Width = 8
        Height = 16
        Caption = #8553
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object lbsTSstatus2: TLabel
        Left = 242
        Top = 53
        Width = 33
        Height = 13
        Caption = #48120#49444#51221
      end
      object Label21: TLabel
        Left = 173
        Top = 103
        Width = 37
        Height = 16
        Caption = #45800#53440' '#52488
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object edbuyx2: TEdit
        Left = 66
        Top = 100
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 0
      end
      object edbuyy2: TEdit
        Left = 117
        Top = 100
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 1
      end
      object edsellx2: TEdit
        Left = 66
        Top = 47
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 2
      end
      object edselly2: TEdit
        Left = 117
        Top = 47
        Width = 44
        Height = 21
        NumbersOnly = True
        TabOrder = 3
      end
      object edclrx2: TEdit
        Left = 66
        Top = 74
        Width = 44
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 8
      end
      object edclry2: TEdit
        Left = 117
        Top = 74
        Width = 44
        Height = 21
        Alignment = taCenter
        NumbersOnly = True
        TabOrder = 9
      end
      object Button4: TButton
        Left = 3
        Top = 101
        Width = 62
        Height = 21
        Caption = #47588#49688#48260#53948
        TabOrder = 5
        OnClick = Button4Click
      end
      object Button5: TButton
        Left = 3
        Top = 47
        Width = 62
        Height = 21
        Caption = #47588#46020#48260#53948
        TabOrder = 6
        OnClick = Button5Click
      end
      object Button6: TButton
        Left = 3
        Top = 74
        Width = 62
        Height = 21
        Caption = #52397#49328#48260#53948
        TabOrder = 7
        OnClick = Button6Click
      end
      object cbStk2: TComboBox
        Left = 106
        Top = 18
        Width = 97
        Height = 21
        Style = csDropDownList
        TabOrder = 4
        OnChange = cbStk2Change
      end
      object cbMasterId2: TComboBox
        Left = 3
        Top = 18
        Width = 97
        Height = 21
        Style = csDropDownList
        TabOrder = 10
        OnChange = cbMasterId2Change
      end
      object chkRvs2: TCheckBox
        Left = 209
        Top = 20
        Width = 62
        Height = 17
        Caption = 'Reverse'
        TabOrder = 11
      end
      object edArtc2: TEdit
        Left = 359
        Top = 139
        Width = 17
        Height = 21
        TabOrder = 12
        Visible = False
      end
      object btnTS2: TButton
        Left = 173
        Top = 48
        Width = 62
        Height = 21
        Caption = 'TS '#49444#51221
        TabOrder = 13
        OnClick = btnTS2Click
      end
      object cbScalping2: TComboBox
        Left = 217
        Top = 99
        Width = 49
        Height = 21
        Style = csDropDownList
        TabOrder = 14
      end
      object chkAddPos2: TCheckBox
        Left = 173
        Top = 77
        Width = 97
        Height = 17
        Caption = #47932#53440#44592' '#54728#50857
        TabOrder = 15
      end
      object chkClrYN2: TCheckBox
        Left = 254
        Top = 77
        Width = 97
        Height = 17
        Caption = #52397#49328#54728#50857
        Checked = True
        State = cbChecked
        TabOrder = 16
      end
    end
    object btnShowPos: TButton
      Left = 956
      Top = 163
      Width = 75
      Height = 18
      Caption = #54252#51648#49496' '#48372#51060#44592
      TabOrder = 3
      Visible = False
    end
    object btnShowCntr: TButton
      Left = 1032
      Top = 163
      Width = 75
      Height = 18
      Caption = #52404#44208' '#48372#51060#44592
      TabOrder = 4
      Visible = False
    end
    object cbPosSetQty1: TComboBox
      Left = 1027
      Top = 215
      Width = 39
      Height = 21
      TabOrder = 5
      Visible = False
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10')
    end
    object cbPosSetSide1: TComboBox
      Left = 1106
      Top = 215
      Width = 54
      Height = 21
      Style = csDropDownList
      TabOrder = 6
      Visible = False
      Items.Strings = (
        #47588#46020
        #47588#49688)
    end
    object cbPosSetQty2: TComboBox
      Left = 1027
      Top = 242
      Width = 39
      Height = 21
      ItemIndex = 2
      TabOrder = 7
      Text = '2'
      Visible = False
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10')
    end
    object cbPosSetSide2: TComboBox
      Left = 1106
      Top = 242
      Width = 54
      Height = 21
      Style = csDropDownList
      TabOrder = 8
      Visible = False
      Items.Strings = (
        #47588#46020
        #47588#49688)
    end
    object cbPosSetQty3: TComboBox
      Left = 1025
      Top = 188
      Width = 39
      Height = 21
      TabOrder = 9
      Visible = False
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10')
    end
    object cbPosSetSide3: TComboBox
      Left = 1106
      Top = 188
      Width = 54
      Height = 21
      Style = csDropDownList
      TabOrder = 10
      Visible = False
      Items.Strings = (
        #47588#46020
        #47588#49688)
    end
    object edqty1: TEdit
      Left = 876
      Top = 170
      Width = 49
      Height = 21
      NumbersOnly = True
      TabOrder = 11
      Text = '1'
      Visible = False
    end
    object edtickval1: TEdit
      Left = 876
      Top = 196
      Width = 49
      Height = 21
      NumbersOnly = True
      TabOrder = 12
      Text = '2000'
      Visible = False
    end
    object edtickval2: TEdit
      Left = 878
      Top = 256
      Width = 49
      Height = 21
      NumbersOnly = True
      TabOrder = 13
      Text = '2000'
      Visible = False
    end
    object edqty2: TEdit
      Left = 878
      Top = 229
      Width = 49
      Height = 21
      NumbersOnly = True
      TabOrder = 14
      Text = '1'
      Visible = False
    end
    object edqty3: TEdit
      Left = 953
      Top = 235
      Width = 49
      Height = 21
      NumbersOnly = True
      TabOrder = 15
      Text = '1'
      Visible = False
    end
    object edtickval3: TEdit
      Left = 953
      Top = 261
      Width = 49
      Height = 21
      NumbersOnly = True
      TabOrder = 16
      Text = '2000'
      Visible = False
    end
    object edMasterID_1: TEdit
      Left = 791
      Top = 146
      Width = 25
      Height = 21
      CharCase = ecUpperCase
      TabOrder = 17
      Text = 'EDIT1'
      Visible = False
    end
    object edMasterID_2: TEdit
      Left = 799
      Top = 154
      Width = 25
      Height = 21
      CharCase = ecUpperCase
      TabOrder = 18
      Text = 'EDIT1'
      Visible = False
    end
    object edMasterID_3: TEdit
      Left = 805
      Top = 172
      Width = 25
      Height = 21
      CharCase = ecUpperCase
      TabOrder = 19
      Text = 'EDIT1'
      Visible = False
    end
    object cbSignalStk1: TComboBox
      Left = 376
      Top = 155
      Width = 62
      Height = 21
      Style = csDropDownList
      TabOrder = 20
      OnChange = cbSignalStk1Change
    end
    object cbSignalUpDown1: TComboBox
      Left = 444
      Top = 155
      Width = 49
      Height = 21
      Style = csDropDownList
      TabOrder = 21
      OnChange = cbStk1Change
      Items.Strings = (
        'UP'
        'DN')
    end
    object edtSignalPrc1: TEdit
      Left = 498
      Top = 155
      Width = 77
      Height = 21
      Alignment = taCenter
      TabOrder = 22
      OnClick = edtSignalPrc1Click
    end
    object cbSignalUpDown2: TComboBox
      Left = 444
      Top = 178
      Width = 49
      Height = 21
      Style = csDropDownList
      TabOrder = 23
      OnChange = cbStk1Change
      Items.Strings = (
        'UP'
        'DN')
    end
    object edtSignalPrc2: TEdit
      Left = 498
      Top = 178
      Width = 77
      Height = 21
      Alignment = taCenter
      TabOrder = 24
      OnClick = edtSignalPrc2Click
    end
    object cbSignalStk2: TComboBox
      Left = 376
      Top = 178
      Width = 62
      Height = 21
      Style = csDropDownList
      TabOrder = 25
      OnChange = cbSignalStk2Change
    end
    object cbSignalUpDown3: TComboBox
      Left = 444
      Top = 201
      Width = 49
      Height = 21
      Style = csDropDownList
      TabOrder = 26
      OnChange = cbStk1Change
      Items.Strings = (
        'UP'
        'DN')
    end
    object edtSignalPrc3: TEdit
      Left = 498
      Top = 201
      Width = 77
      Height = 21
      Alignment = taCenter
      TabOrder = 27
      OnClick = edtSignalPrc3Click
    end
    object cbSignalStk3: TComboBox
      Left = 376
      Top = 201
      Width = 62
      Height = 21
      Style = csDropDownList
      TabOrder = 28
      OnChange = cbSignalStk3Change
    end
    object chPopup: TCheckBox
      Left = 686
      Top = 288
      Width = 76
      Height = 17
      Caption = 'PopUp '#50508#47548
      Checked = True
      State = cbChecked
      TabOrder = 29
      Visible = False
    end
    object edtCalcPrcBase: TEdit
      Left = 489
      Top = 237
      Width = 77
      Height = 21
      Alignment = taCenter
      TabOrder = 31
      OnClick = edtSignalPrc3Click
    end
    object cbCalcPrc: TComboBox
      Left = 376
      Top = 237
      Width = 62
      Height = 21
      Style = csDropDownList
      TabOrder = 30
      OnChange = cbSignalStk1Change
    end
    object edtCalcPrcTick: TEdit
      Left = 597
      Top = 237
      Width = 41
      Height = 21
      NumbersOnly = True
      TabOrder = 32
      OnClick = edtCalcPrcTickClick
      OnKeyPress = edtCalcPrcTickKeyPress
    end
    object edtCalcPrcH: TEdit
      Left = 631
      Top = 155
      Width = 77
      Height = 21
      Alignment = taCenter
      ReadOnly = True
      TabOrder = 34
      OnClick = edtSignalPrc3Click
    end
    object edtCalcPrcL: TEdit
      Left = 631
      Top = 178
      Width = 77
      Height = 21
      Alignment = taCenter
      ReadOnly = True
      TabOrder = 35
      OnClick = edtSignalPrc3Click
    end
    object btnCalcPrc: TButton
      Left = 645
      Top = 235
      Width = 75
      Height = 25
      Caption = #44228#49328
      TabOrder = 33
      OnClick = btnCalcPrcClick
    end
  end
  object sbBgCntr: TScrollBox
    Left = 0
    Top = 608
    Width = 769
    Height = 315
    VertScrollBar.Style = ssFlat
    Align = alBottom
    TabOrder = 3
    object lblCntrMaster: TLabel
      Left = 557
      Top = 4
      Width = 94
      Height = 13
      Caption = '[ Master '#52404#44208#45236#50669' ]'
    end
    object lblCntrMine: TLabel
      Left = 557
      Top = 156
      Width = 83
      Height = 13
      Caption = '[ '#45208#51032' '#52404#44208#45236#50669' ]'
    end
    object gdCntrMaster: TAdvStringGrid
      Left = 97
      Top = 3
      Width = 454
      Height = 142
      Cursor = crDefault
      ColCount = 13
      DrawingStyle = gdsClassic
      FixedCols = 0
      RowCount = 2
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
      ScrollBars = ssBoth
      TabOrder = 0
      HoverRowCells = [hcNormal, hcSelected]
      OnGetCellColor = gdCntrMasterGetCellColor
      OnGetAlignment = gdCntrMasterGetAlignment
      ActiveCellFont.Charset = DEFAULT_CHARSET
      ActiveCellFont.Color = clWindowText
      ActiveCellFont.Height = -11
      ActiveCellFont.Name = 'Tahoma'
      ActiveCellFont.Style = [fsBold]
      ColumnHeaders.Strings = (
        'No.'
        'ID'
        #52404#44208#49884#44033
        #54408#47785
        #48169#54693
        #52397#49328#50668#48512
        #44032#44201
        #49688#47049
        #49552#51061#54001
        #49552#51061
        #53440#51077
        'Lvg'
        #52404#44208#48264#54840)
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
      FixedColWidth = 32
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
        32
        64
        51
        73
        47
        34
        34
        58
        39
        4
        46
        44
        64)
      RowHeights = (
        22
        22)
    end
    object gdCntrMine: TAdvStringGrid
      Left = 97
      Top = 151
      Width = 454
      Height = 142
      Cursor = crDefault
      ColCount = 13
      DrawingStyle = gdsClassic
      FixedCols = 0
      RowCount = 2
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
      ScrollBars = ssBoth
      TabOrder = 1
      HoverRowCells = [hcNormal, hcSelected]
      OnGetCellColor = gdCntrMineGetCellColor
      OnGetAlignment = gdCntrMineGetAlignment
      ActiveCellFont.Charset = DEFAULT_CHARSET
      ActiveCellFont.Color = clWindowText
      ActiveCellFont.Height = -11
      ActiveCellFont.Name = 'Tahoma'
      ActiveCellFont.Style = [fsBold]
      ColumnHeaders.Strings = (
        'No.'
        'ID'
        #52404#44208#49884#44033
        #54408#47785
        #48169#54693
        #52397#49328#50668#48512
        #44032#44201
        #49688#47049
        #49552#51061#54001
        #49552#51061
        #53440#51077
        'Lvg'
        #52404#44208#48264#54840)
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
      FixedColWidth = 29
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
      SelectionResizer = True
      ShowDesignHelper = False
      SortSettings.DefaultFormat = ssAutomatic
      Version = '8.4.2.2'
      ColWidths = (
        29
        55
        13
        66
        48
        30
        55
        35
        49
        55
        51
        7
        8)
      RowHeights = (
        22
        22)
    end
    object Panel10: TPanel
      Left = 0
      Top = 0
      Width = 94
      Height = 311
      Align = alLeft
      TabOrder = 2
      object Label3: TLabel
        Left = 2
        Top = 47
        Width = 25
        Height = 13
        Caption = #53440' '#51077
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label7: TLabel
        Left = 12
        Top = 75
        Width = 14
        Height = 13
        Caption = 'I D'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label1: TLabel
        Left = 2
        Top = 105
        Width = 25
        Height = 13
        Caption = #51333' '#47785
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label2: TLabel
        Left = 2
        Top = 134
        Width = 25
        Height = 13
        Caption = #48169' '#54693
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label6: TLabel
        Left = 2
        Top = 164
        Width = 25
        Height = 13
        Caption = #44032' '#44201
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label5: TLabel
        Left = 2
        Top = 192
        Width = 25
        Height = 13
        Caption = #49884' '#44033
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label4: TLabel
        Left = 2
        Top = 222
        Width = 25
        Height = 13
        Caption = #49688' '#47049
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label34: TLabel
        Left = 9
        Top = 249
        Width = 17
        Height = 13
        Caption = 'No.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object edClrTp: TEdit
        Left = 27
        Top = 44
        Width = 62
        Height = 21
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        TabOrder = 0
      end
      object edMasterId: TEdit
        Left = 27
        Top = 72
        Width = 62
        Height = 21
        Alignment = taCenter
        ReadOnly = True
        TabOrder = 1
      end
      object edStk: TEdit
        Left = 27
        Top = 101
        Width = 62
        Height = 21
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        TabOrder = 2
      end
      object edSide: TEdit
        Left = 27
        Top = 130
        Width = 62
        Height = 21
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        TabOrder = 3
      end
      object edCntrPrc: TEdit
        Left = 27
        Top = 159
        Width = 62
        Height = 21
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        TabOrder = 4
      end
      object edCntrTm: TEdit
        Left = 27
        Top = 188
        Width = 62
        Height = 21
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        TabOrder = 5
      end
      object edCntrQty: TEdit
        Left = 27
        Top = 217
        Width = 62
        Height = 21
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        NumbersOnly = True
        ParentFont = False
        ReadOnly = True
        TabOrder = 6
      end
      object edCntrNo: TEdit
        Left = 27
        Top = 246
        Width = 62
        Height = 21
        Alignment = taCenter
        ReadOnly = True
        TabOrder = 7
      end
      object btnCntrHist: TButton
        Left = 5
        Top = 3
        Width = 83
        Height = 26
        Caption = #47560#49828#53552#52404#44208#51312#54924
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 8
        OnClick = btnCntrHistClick
      end
    end
  end
  object pnlSettingSummary: TPanel
    Left = 0
    Top = 52
    Width = 769
    Height = 24
    Align = alTop
    TabOrder = 4
    object chkMacroOrd1: TCheckBox
      Left = 13
      Top = 2
      Width = 126
      Height = 17
      Caption = #47588#53356#47196#51452#47928#49892#54665
      TabOrder = 0
      OnClick = chkMacroOrd1Click
    end
    object chkMacroOrd2: TCheckBox
      Left = 153
      Top = 2
      Width = 126
      Height = 17
      Caption = #47588#53356#47196#51452#47928'-2'
      TabOrder = 1
      OnClick = chkMacroOrd2Click
    end
    object chkMacroOrd3: TCheckBox
      Left = 294
      Top = 2
      Width = 126
      Height = 17
      Caption = #47588#53356#47196#51452#47928'-3'
      TabOrder = 2
      OnClick = chkMacroOrd3Click
    end
    object btnShowSetting: TButton
      Left = 461
      Top = 3
      Width = 75
      Height = 18
      Caption = #49444#51221' '#48372#51060#44592
      TabOrder = 3
      OnClick = btnShowSettingClick
    end
    object btnShowPrcGrid: TButton
      Left = 542
      Top = 3
      Width = 75
      Height = 18
      Caption = #49884#49464' '#48372#51060#44592
      TabOrder = 4
      OnClick = btnShowPrcGridClick
    end
    object edtTicker: TEdit
      Left = 623
      Top = 3
      Width = 53
      Height = 21
      Alignment = taCenter
      ReadOnly = True
      TabOrder = 5
    end
  end
  object sbBgPos: TPanel
    Left = 0
    Top = 374
    Width = 769
    Height = 234
    Align = alClient
    TabOrder = 5
    object gdPosMine: TAdvStringGrid
      Left = 4
      Top = 0
      Width = 765
      Height = 111
      Cursor = crDefault
      ColCount = 16
      DrawingStyle = gdsClassic
      FixedCols = 0
      RowCount = 4
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
      ParentFont = False
      ScrollBars = ssBoth
      TabOrder = 0
      HoverRowCells = [hcNormal, hcSelected]
      OnGetCellColor = gdPosMineGetCellColor
      OnGetAlignment = gdPosMineGetAlignment
      ActiveCellFont.Charset = DEFAULT_CHARSET
      ActiveCellFont.Color = clWindowText
      ActiveCellFont.Height = -11
      ActiveCellFont.Name = 'Tahoma'
      ActiveCellFont.Style = [fsBold]
      ColumnHeaders.Strings = (
        'Master'
        #51333#47785
        #49345#53468
        #48169#54693
        #49688#47049
        #49884#44033
        #54217#45800
        #54788#51116#44032
        'PLTick'
        'TS'#49345#53468
        'TSBest'
        'TSShift'
        'TSCut'
        'SLPrc'
        'SLCut'
        'TickCnt')
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
      FixedColWidth = 47
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
        47
        36
        38
        52
        33
        59
        58
        55
        42
        55
        46
        47
        46
        57
        47
        64)
      RowHeights = (
        22
        22
        22
        22)
    end
    object gdPosMaster: TAdvStringGrid
      Left = 3
      Top = 117
      Width = 399
      Height = 110
      Cursor = crDefault
      ColCount = 16
      DrawingStyle = gdsClassic
      FixedCols = 0
      RowCount = 4
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
      ParentFont = False
      ScrollBars = ssBoth
      TabOrder = 1
      HoverRowCells = [hcNormal, hcSelected]
      OnGetCellColor = gdPosMasterGetCellColor
      OnGetAlignment = gdPosMasterGetAlignment
      ActiveCellFont.Charset = DEFAULT_CHARSET
      ActiveCellFont.Color = clWindowText
      ActiveCellFont.Height = -11
      ActiveCellFont.Name = 'Tahoma'
      ActiveCellFont.Style = [fsBold]
      ColumnHeaders.Strings = (
        'Master'
        #51333#47785
        #49345#53468
        #48169#54693
        #49688#47049
        #49884#44033
        #54217#45800
        #54788#51116#44032
        'PLTick'
        'TS'#49345#53468
        'TSBest'
        'TSShift'
        'TSCut'
        'SLCut'
        'TickCnt')
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
        64
        64
        73
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
        22
        22)
    end
    object gdDiff: TAdvStringGrid
      Left = 407
      Top = 117
      Width = 140
      Height = 100
      Cursor = crDefault
      ColCount = 2
      DrawingStyle = gdsClassic
      FixedCols = 0
      RowCount = 4
      ScrollBars = ssBoth
      TabOrder = 2
      HoverRowCells = [hcNormal, hcSelected]
      OnGetAlignment = gdDiffGetAlignment
      ActiveCellFont.Charset = DEFAULT_CHARSET
      ActiveCellFont.Color = clWindowText
      ActiveCellFont.Height = -11
      ActiveCellFont.Name = 'Tahoma'
      ActiveCellFont.Style = [fsBold]
      ColumnHeaders.Strings = (
        #44032#44201
        #54001)
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
      ShowDesignHelper = False
      SortSettings.DefaultFormat = ssAutomatic
      Version = '8.4.2.2'
      ColWidths = (
        64
        64)
      RowHeights = (
        22
        22
        22
        22)
    end
    object btnPosSet1: TButton
      Left = 669
      Top = 117
      Width = 92
      Height = 28
      Caption = #54252#51648#49496' '#51077#47141
      TabOrder = 3
      OnClick = btnPosSet1Click
    end
    object edtDebug: TEdit
      Left = 553
      Top = 117
      Width = 121
      Height = 21
      TabOrder = 4
      Text = 'edtDebug'
    end
  end
  object idTcpOrd: TIdTCPClient
    OnDisconnected = idTcpOrdDisconnected
    OnConnected = idTcpOrdConnected
    ConnectTimeout = 0
    IPVersion = Id_IPv4
    Port = 0
    ReadTimeout = -1
    Left = 1104
    Top = 289
  end
  object tmrSetIDCombo: TThreadedTimer
    OnTimer = tmrSetIDComboTimer
    Left = 1056
    Top = 249
  end
  object tmrUnMarkChange: TThreadedTimer
    OnTimer = tmrUnMarkChangeTimer
    Left = 1104
    Top = 353
  end
  object tmrTryConn: TTimer
    Enabled = False
    OnTimer = tmrTryConnTimer
    Left = 1040
    Top = 305
  end
  object IdTcpTick: TIdTCPClient
    OnConnected = IdTcpTickConnected
    ConnectTimeout = 0
    IPVersion = Id_IPv4
    Port = 0
    ReadTimeout = -1
    Left = 1104
    Top = 241
  end
  object tmrSignalAlarm: TThreadedTimer
    OnTimer = tmrSignalAlarmTimer
    Left = 728
    Top = 140
  end
  object tmrTicker: TThreadedTimer
    Interval = 500
    OnTimer = tmrTickerTimer
    Left = 720
    Top = 196
  end
end
