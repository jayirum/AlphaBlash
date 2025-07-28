unit uTickThrd;

interface


uses
  System.Classes, vcl.dialogs, system.SysUtils, windows, VCL.FORMS
  ;


type

  TTickThrd = class(TThread)
    procedure Execute();override;
  end;

var
  __tickThrd   : TTickThrd;


implementation

uses
  uMain, uPacketThrd, XAlphaPacket, uPrcList, uCommonDef, uPrcGrid, uTrailingStop, uNotify;


procedure TTickThrd.Execute();
var
  s       : string;
  artc
  ,close
  ,time   : string;
  idx     : integer;

  itemMD : TItemMD;
  dwRslt : DWORD;
begin

  while (not terminated) and (fmMain.idTcpTick.connected) and (fmMain.m_bTerminate=false) do
  begin

    try
      s := fmMain.idTcpTick.IOHandler.ReadLn();
      if length(s)>0 then
      begin
        artc  := __Artc(TRIM(Copy(s, 3, 8)));
        close := __PrcFmt(artc, TRIM(Copy(s, 11, 15)));
        time  := TRIM(Copy(s, 27, 11));

        idx := __prcList.SavePrc(artc, close);
        if idx>-1 then
        begin

          itemMD := TItemMD.Create;
          itemMD.artc   := artc;
          itemMD.close  := close;
          itemMD.time   := time;
          itemMD.idxPrcGrid := idx;
          itemMD.idxPosGrid := -1;
//          fmPrcGrid.gdTick.Cells[TICK_ARTC, idx] := artc;
//          fmPrcGrid.gdTick.Cells[TICK_PRC, idx]  := close;
//          fmPrcGrid.gdTick.Cells[TICK_TM, idx]   := time;


          for idx := 1 to MAX_STK do
          BEGIN
            if fmMain.gdPosMine.Cells[POS_ARTC, idx] = artc then
            begin
                itemMD.idxPosGrid := idx;
//              fmMain.LockPosGrid();
//              fmMain.gdPosMine.Cells[POS_NOWPRC, idx]  := close;
//              fmMain.gdPosMine.Cells[POS_PL_TICK, idx] := __ts.Calc_PLTick(idx, close);
//              fmMain.UnLockPosGrid();
            end;
          END;

          SendMessageTimeOut(Application.MainForm.Handle,
                              WM_GRID_REAL_MD,
                              wParam(LongInt(sizeof(itemMD))),
                              Lparam(LongInt(itemMD)),
                              SMTO_ABORTIFHUNG,
                              TIMEOUT_SENDMSG,
                              dwRslt
                              );



          //TS점검
          //if __ts.TsCount()>0 then
            __ts.Update_Tick(artc,close);

        end;
      end;

    except
      //on E: EIdSocketHandleError do
      begin
        fmMain.AddMsg('시세서버 disconnect', B_SIREN);
        exit;

      end;
    end;

  end;


end;

end.
