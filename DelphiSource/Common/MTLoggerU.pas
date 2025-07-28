unit MTLoggerU;

interface

uses
  System.Classes, System.SysUtils, Windows, VCL.Graphics,
  StrUtils, SyncObjs, WinAPI.Messages;

//type TMessagingMethod = (tmDelphi, tmCPP);

const
  INFO  = 0;
  ERR   = 1;

  WM_LOGMSG_INFO = WM_USER + 7001;
  WM_LOGMSG_ERR = WM_USER + 7002;

type TMTLogger = class(TThread)
    private
      m_bInitialized, m_bFileOpened : boolean;
      m_cs                : TCriticalSection;
      m_FileHandle              : THandle;
      //MessagesQueue           : TStringList;
      //Bytes                   : TBytes;
      m_LastFileDateTime        : TDateTime;
      m_sExeFolder              : string;
      m_sExeName                : string;
      m_bContinue               : boolean;
      function MakeFileName : string;
    protected
      procedure Execute; override;
    public
      //MessagingMethod : TMessagingMethod;
      m_FileEncoding    : TEncoding;
      m_FileName        : string;

      function Initialize(sFolder, sExe:string):boolean;
      function OpenFile : boolean;
      procedure CloseFile;
      function log(tp:integer;mess : string) : integer;
      procedure SetStop();

end;

implementation

//uses fmMainU;

{ TMTLogger }


procedure TMTLogger.SetStop();
begin
  m_bContinue := false;
end;

function TMTLogger.MakeFileName() : string;
var
  s1,
  datestr : string;

begin
  Result := '';

  DateTimeToString(datestr,'yyyymmdd', Now());
  s1 := m_sExeFolder + '\' + m_sExeName + '_' + datestr + '.log';
  Result := s1;
end;

// Possible Return Values :
// 0 - Ok, message accepted
// -1 - class not ready to receive messages;
function TMTLogger.log(tp:integer; mess : string) : integer;
var B : TBytes;
    p : Pointer;
    sTM : string;
begin
  Result := -1;
  if not (m_bInitialized and m_bFileOpened) then
  begin
     Exit;
  end;

  DateTimeToString(sTm,'yyyymmdd_hh:mm:ss:zzz', Now());

  if tp = ERR then
    B := m_FileEncoding.GetBytes('[E]['+sTm+']'+mess+ #13#10)
  else
    B := m_FileEncoding.GetBytes('[I]['+sTm+']'+mess+ #13#10);

  p := GetMemory(Length(B));
  CopyMemory(p, Addr(B[0]), Length(B));
  PostThreadMessage(Self.ThreadID, WM_LOGMSG_INFO, Length(B), LPARAM(p));

  Result := 0;
end;

procedure TMTLogger.Execute;
var WaitRes : DWORD;
  EventCount: DWORD;
  EventArray: DWORD;
  M : Msg;
  B : TBytes;
begin

  while (not Terminated) and (m_bContinue = True) do
  begin

    Sleep(10);
    while PeekMessage(M, 0, 0, 0, PM_REMOVE) do
    begin
      if M.Message = WM_QUIT then Break;

      TranslateMessage(M);
      DispatchMessage(M);

      if (M.message = WM_LOGMSG_INFO) OR (M.message = WM_LOGMSG_ERR) then
      begin

        // change file if another day came
        if Trunc(Now()) <> Trunc(m_LastFileDateTime) then
        begin
          FlushFileBuffers(m_FileHandle);
          CloseFile;
          OpenFile;
        end;

        SetLength(B, M.wParam);
        CopyMemory(B, Pointer(M.LParam), M.wParam);
        FreeMemory(Pointer(M.LParam));

        if m_bFileOpened then
        begin
          FileSeek(m_FileHandle, 0, 2);
          FileWrite(m_FileHandle, Pchar(B)^, Length(B));
          FlushFileBuffers(m_FileHandle);
        end;

      end;
    end;

  end;

end;

function TMTLogger.Initialize(sFolder, sExe:string):boolean;
begin
  m_cs := TCriticalSection.Create;

  m_FileEncoding := TEncoding.UTF8;
  m_bInitialized := TRUE;

  m_sExeFolder := sFolder;
  m_sExeName   := sExe;

  OpenFile;

  m_bContinue := True;

  Start;

  m_bContinue := True;
  Result := True;

end;

function TMTLogger.OpenFile : boolean;
var filepath  : string;
begin

  filepath := MakeFileName;
  m_LastFileDateTime := Now();

  if not FileExists(filepath) then
  begin
    m_FileHandle := FileCreate(PWideChar(Filepath), fmOpenReadWrite);
    FileClose(m_FileHandle);
  end;

  m_FileHandle := System.SysUtils.FileOpen(PWideChar(Filepath), fmOpenReadWrite or fmShareDenyNone);

  Result := m_FileHandle <> INVALID_HANDLE_VALUE;
  m_bFileOpened := Result;

end;

procedure TMTLogger.CloseFile;
begin
  if not m_bFileOpened then Exit;
  try
    m_cs.Enter;
    FileClose(m_FileHandle);
    m_bFileOpened := FALSE;
  finally
    m_cs.Leave;
  end;
end;

end.
