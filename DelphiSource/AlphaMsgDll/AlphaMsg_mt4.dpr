library AlphaMsg_mt4;

{ Important note about DLL memory management: ShareMem must be the
  first unit in your library's USES clause AND your project's (select
  Project-View Source) USES clause if your DLL exports any procedures or
  functions that pass strings as parameters or function results. This
  applies to all strings passed to and from your DLL--even those that
  are nested in records and classes. ShareMem is the interface unit to
  the BORLNDMM.DLL shared memory manager, which must be deployed along
  with your DLL. To avoid using BORLNDMM.DLL, pass string information
  using PChar or ShortString parameters. }

uses
  System.SysUtils,
  Windows,
  Vcl.Dialogs,
  System.Classes,
  Messages,
  Forms,
  DLLFormU in 'DLLFormU.pas' {DLLForm};

//var DLLF : TDLLForm;

{$R *.res}

/// Our thread that will be used to provide a message Queue for our Window(s)
type TDLLThread = class(TThread)
protected
   procedure execute; override;
end;

// Global Vars
var
  dllsaveexit : pointer;  // save the pointer to the old dll exit routine we will
                          //
  dllthread : tdllthread; // pointer to our dll thread object

  frmeahandle : hwnd;     // Global reference for our main DLL form.
                          // This variable is used to indicate whether our main form has been created or not
                          // See initDLL and ddlthread.execute

  g_sSoundDir : string;

/// create a top most window
/// making sure MT4 is the owner of the window (so that we can make it stay on top of MT4)
procedure createtopwindow(classtype : TComponentClass; var ref);
var
  Instance: TComponent;
  l1 : longint;
begin
  Instance := TComponent(classtype.NewInstance);

  Application.handle := mt4apphandle;

  Instance.Create(application);
  TComponent(Ref) := Instance;

  application.handle := DLLapphandle;

  // set the windows properties so that it stays on top
  l1 := getwindowlong(tform(ref).handle,gwl_exstyle);
  l1 := l1 or ws_ex_topmost and not ws_ex_appwindow;
  setwindowlong(tform(ref).handle,gwl_exstyle,l1);
end;

// execute procedure for our new thread with message queue
procedure tdllthread.execute();
var waitresult : dword;
begin



     // we must create first window within our new thread else it wont receive any messages
     createtopwindow(TDLLForm, DLLForm);

     DLLForm.Show; // we must only show the window
                 // if we showmodal then we will never be able to processmessages
                 // therefore our window will not respond to user input and MT4 will soon crash
                 // If you only want to show modal windows then just hide this base window and
                 // create other modal windows from within the invisible window.

     frmeahandle := DLLForm.Handle; // copy the handle of our new window into a global var to indicate that this form actually exists
                                  // see dllinit

     DLLForm.Set_SoundFileDir(g_sSoundDir);

     // Keep the thread execution alive and enable message processing
     while not self.terminated do
     begin
           Application.ProcessMessages; //process all outstanding messages

           // enter idle state until either a std windows message arrives
           // or one of our events is set.
           // In this example only one event is used to wake the thread when the DLL wants to close
           // we could use many more events if required to signal other requirements eg open another window etc.
           // in reality since the user has no direct input method into the EA Code it is unlikely we really need these events

           waitresult := msgwaitformultipleobjects(2, wakeEvents, false, 10, qs_allinput);
           // if waitresult = 0 then our wakeevent[0] was set because we want to terminate the thread

           //we could use other events to signal other actions if required
           (*if (waitresult = 1)  then
           begin
                sysutils.beep;
           end;           *)

           //Sleep(10);

     end;



     /// kill our form and set its refence to nil
     DLLForm.Free;
     DLLForm := nil;

     // any other windows we have created should be removed here also


end;

// final clean up code executed when all attached EAs have called deinit
Procedure killDLLThread;
begin

     if dllthread <> nil then
     begin


          // if the modal dialog form is open then close it
          //if frmmodaldialog <> nil then postmessage(frmmodaldialog.handle,wm_close,0,0);


          // kill the DLL thread and wait for it to terminate
          dllthread.Terminate; // signal we want the thread to terminate
          setevent(wakeEvents[0]); // wake the thread up incase it was in an idle state
          dllthread.WaitFor; // wait for thread to terminate
          dllthread.Free; // kill the thread object
          dllthread := nil; // set the reference to nil because we use this to check if the thread is already created

          //frmmodaldialog := nil;      // set our global reference to any open Dialogs to nil
          frmeahandle :=0;            // set our global reference to our main DLL form to 0 - see var declaration for info

     end;

end;

// Final exit code for the DLL only called when the DLL is unloaded from memory
// MT4 does not forceably unload a DLL for example If you recompile the MQ4 file when the EA is running
// therefore This code will not execute until all EAs are removed from the chart or the MT4 app is Closed.
procedure LibExit;
var x : longint;
begin
      killDllThread;  // if the DLL is shutting down make sure all our forms and objects are destroyed
                      // incase the user did not call Deinit themselves


      // remove our events for thread wakeup
      for x := 0 to maxevents-1 do Closehandle(wakeEvents[x]);

      // kill our string list objects
      //ActiveEAs.free;
      //messagesIn.free;
      //messagesOut.free;
      //messagessent.free;

     DeleteCriticalSection(Threadlock); // delete our thread lock object

     ExitProc := dllsaveexit;  // restore exit procedure chain
end;



function IntegerFunction(value : integer) : integer; stdcall;
begin
  Result := value * 2;
end;

function CharFunction(instring : pointer) : integer; stdcall;
var b : Array [0..10000] of byte;
    p : pointer;
    s : string;
    index, len, i1 : integer;
begin

  p := instring;
  index := 0; len := 0;
  while (len < 9999) do
  begin

    CopyMemory(Addr(b[index]), p, 1);
    Inc(PByte(P), 1);
    if b[index] = 0 then Break;
    Inc(index);
    Inc(len);

  end;

  //ShowMessage(IntToStr(len));

  SetLength(s, len);
  for i1 := 1 to len do
  begin
    s[i1] := Chr(b[i1 - 1]);
  end;

  ShowMessage(s);

  //outstring := instring;

  Result := len;
end;

function WideCharFunction(title : pointer; mess : pointer) : integer; stdcall;
var b : Array [0..10000] of byte;
    p : pointer;
    s1, s2 : string;
    index, len, i1 : integer;
begin

  (*p := title;
  index := 0; len := 0;
  while (len < 9999) do
  begin

    CopyMemory(Addr(b[index]), p, 1);
    Inc(PByte(P), 1);
    if b[index] = 0 then Break;
    Inc(index);
    Inc(len);

  end;       *)

  s1 := WideCharToString(title);
  s2 := WideCharToString(mess);

  ShowMessage('Title : ' + s1 + #13#10 +
  'Message : ' + s2);

  //outstring := instring;

  Result := len;
end;

procedure ShowFunction; stdcall;
begin
  DLLForm.Show;
end;

procedure HideFunction; stdcall;
begin
  DLLForm.Hide;
end;


// sound : 0-no play, 1-info, 2-order, 3-error
function AlphaMsg_AddMessage(title : pointer; mess : pointer; sound : integer ) : integer; stdcall;
var s1, s2 : string;
begin
  SetLength(DLLForm.MessQ, Length(DLLForm.MessQ) + 1);
  s1 := WideCharToString(title);
  s2 := WideCharToString(mess);
  with DLLForm.MessQ[Length(DLLForm.MessQ) - 1] Do
  begin
    Title   := s1;
    Mess    := s2;
    Dt      := Now();
    DTStr   := DateTimeToStr(DT);
    nSound  := sound;
  end;

  if not DLLForm.Showing then
  begin
    DLLForm.Show;
  end
  else
  begin
    DLLForm.Panel1.Visible := TRUE;
    DLLForm.UpdateData(True);
    DLLForm.IncCurrIdx();
  end;
end;

procedure AlphaMsg_Initialize(pSoundDir : pointer); stdcall;
begin


 g_sSoundDir := WideCharToString(pSoundDir);

 entercriticalsection(ThreadLock); // lock out other threads - both other EAs and our DLL
                                       // because we are accessing the ActiveEAs object and we dont want anyone else to get a chance
                                       // to alter or use it till we are done here
 if dllthread = nil then
 begin

    // get the window handle of the MT4 Applications main window
    /// we will use this when creating windows so that they belong to the MT4 app rather than our DLL
    //mt4apphandle := getparent(chartwin);
    //while (getparent(mt4apphandle) <> 0) do mt4apphandle := getparent(mt4apphandle);

    //start our new DLL thread in a running state
    dllthread := tdllthread.Create(false);
    dllthread.Priority := tplower; // set the thread priority to lower (7) this is the same level as a Mt4 EA thread

    // we must wait here until our main form has been created by our DLLThread
    // Before we can send a message to our form
    while frmeahandle = 0 do
    begin
         sleep(5);
    end;

 end;

 leavecriticalsection(ThreadLock);


 //DLLF := TDLLForm.Create(nil);
 //
 dllsaveexit := Exitproc;
 ExitProc := @LibExit; // the function libexit will now be called should the DLL be unloaded before ,
                       // deinitdllform is called by all EAs that are using this DLL

 //DLLForm.FormShow(nil);

 DLLForm.AdjustPosition;

end;

// exported DLL function for our EAs to call
procedure AlphaMsg_DeInitialize; stdcall;
var x: longint;
begin
     entercriticalsection(threadlock); // lock out other threads   (EAs)

     if dllthread <> nil then
     begin
          // if we find this charts Hwnd then delete it from our ActiveEAs list
          /// and also inform the main window that we want to remove the tab for this EA
          (*x := getEaindex(charthwnd);
          if x <> -1 then
          begin
               /// delete references for this EA
               activeeas.Objects[x].free; // free our EAobj for this EA
               activeEAs.delete(x);  // delete this EAs entry

               // sendmessage to main window to remove the tab for this EA
               // we want to wait using SendMessage incase our user clicks on the tab control whilst
               // we are half way through removing the EA tab.
               sendmessage(frmea.handle,custommsgval,msg_closetab,x);

          end;          *)

          // If there are no EAs left then kill the DLL
          //if activeEas.count = 0 then
          KillDllThread;

          //result := 1;
     end
     else
     begin
      //result := -1;
     end;

     leavecriticalsection(threadlock); // allow other threads (EAs)
end;



exports

   IntegerFunction,
   CharFunction,
   WideCharFunction,
   AlphaMsg_Initialize,
   AlphaMsg_DeInitialize,
   ShowFunction,
   HideFunction,
   AlphaMsg_AddMessage;

begin
     /// some initialisation when the DLL is first loaded.
     //custommsgval := registerwindowmessage(pchar('MT4EADLLcustomMessage')); // create a custom windows message value for later use - to keep things thread safe

     DLLapphandle := application.handle; // save the application handle of this DLL


     InitializeCriticalSection(Threadlock);  /// create a lock obect so we can lock out other threads



     //activeEAs := tstringlist.create;   //used to monitor all the separate EAs that are using this DLL
     //messagesIn := tstringlist.create;  // Used to store incoming messages
     //messagesOut := tstringlist.create; // Used To store outgoing messages
     //messagessent := tstringlist.create; // Used To store outgoing messages that have already been sent

     // create some events so we can signal our thread directly
     // dont need to give each event a name eg 'wakedllthread' .. we could use NIL for all of them
     wakeEvents[0] := createevent(NIL,false,false,pchar('WakeDllThread'));
     wakeEvents[1] := createevent(NIL,false,false,pchar('Beep'));


     mt4apphandle := 0;
     DLLForm := nil;
     frmeahandle := 0;
     dllthread := nil;


     //frmmodaldialog := nil;  // set all object pointers to  nil ... probably overkill but !!
end.
