#if 0

compile with -lX11

---- CDEF

void sendFakeKey(char*,int,int);
const int _ShiftMask, _LockMask, _ControlMask, _Mod1Mask, _Mod2Mask, _Mod3Mask, _Mod4Mask;

---- LDEF

local ffi=require'ffi'
local string=require'string'

local masks={
   shift=C._ShiftMask,
   lock=C._LockMask,
   control=C._ControlMask,
   mod1=C._Mod1Mask,
   mod2=C._Mod2Mask,
   mod3=C._Mod3Mask,
   mod4=C._Mod4Mask,
}

local function sendFakeKey(keyname,mask,mode)
  assert(type(keyname)=='string','keyname has to be string')
  if not mask then mask=0 end
  assert(type(mask)=='number','mask has to be an integer ORed from the masks table')
  if not mode or mode==2 then
    C.sendFakeKey(ffi.cast('char*',keyname),1,mask)
    C.sendFakeKey(ffi.cast('char*',keyname),0,mask)
  elseif mode==1 or mode==0 then
    C.sendFakeKey(ffi.cast('char*',keyname),mode,mask)
  else
    error'mode has to be 0,1 or 2'
  end
end
return {sendFakeKey=sendFakeKey,masks=masks}

---- MDEF

{:sendFakeKey,:masks}=L

replacer={
   [' ']:  'space'
   ["\n"]: 'Return'
}

sendFakeString=(str)->
   for i=1,str\len!
      ch=str\sub i,i
      mask=0
      if ch\upper! == ch
         mask=bit.bor(mask,masks.shift)
      if replacer[ch]
         ch=replacer[ch]
      sendFakeKey ch, mask, 2

{:sendFakeKey, :masks, :sendFakeString}

---- END

#endif

#include <X11/Xlib.h>
#include <X11/keysym.h>

const int _ShiftMask = ShiftMask;
const int _LockMask = LockMask;
const int _ControlMask = ControlMask;
const int _Mod1Mask = Mod1Mask;
const int _Mod2Mask = Mod2Mask;
const int _Mod3Mask = Mod3Mask;
const int _Mod4Mask = Mod4Mask;


// The key code to be sent.
// A full list of available codes can be found in /usr/include/X11/keysymdef.h

// Function to create a keyboard event
XKeyEvent createKeyEvent(Display *display, Window win,
                           Window winRoot, int press,
                           int keycode, int modifiers)
{
   XKeyEvent event;

   event.display     = display;
   event.window      = win;
   event.root        = winRoot;
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = True;
   event.keycode     = XKeysymToKeycode(display, keycode);
   event.state       = modifiers;

   if(press)
      event.type = KeyPress;
   else
      event.type = KeyRelease;

   return event;
}

void sendFakeKey(char* keystring,int mode,int mask)
{
// Obtain the X11 display.
   Display *display = XOpenDisplay(0);
   if(display == NULL)
      return;

// Get the root window for the current display.
   Window winRoot = XDefaultRootWindow(display);

// Find the window which has the current keyboard focus.
   Window winFocus;
   int    revert;
   XGetInputFocus(display, &winFocus, &revert);

   KeySym KEYCODE = XStringToKeysym(keystring);

// Send a fake key press event to the window.
   XKeyEvent event = createKeyEvent(display, winFocus, winRoot, mode, KEYCODE, mask);
   XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

// Done.
   XCloseDisplay(display);
}