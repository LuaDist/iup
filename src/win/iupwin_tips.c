/** \file
 * \brief Windows Driver TIPS management
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>

#include <windows.h>
#include <commctrl.h>

#include "iup.h" 
#include "iupcbs.h" 

#include "iup_object.h" 
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_attrib.h" 
#include "iup_str.h" 

#include "iupwin_drv.h"
#include "iupwin_handle.h"


#ifndef TTM_POPUP   /* Not defined for MingW and Cygwin */
#define TTM_POPUP  (WM_USER + 34)
#endif

static HWND winTipsCreate(HWND hParent)
{
  RECT rect = {1,1,1,1};
  HWND tips_hwnd = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, (LPSTR) NULL, TTS_ALWAYSTIP|TTS_NOPREFIX, 
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                                  hParent, (HMENU)NULL, iupwin_hinstance, NULL); 
  SendMessage(tips_hwnd, TTM_SETMAXTIPWIDTH, 0, (LPARAM)(INT)3000); 
  SendMessage(tips_hwnd, TTM_SETMARGIN, (WPARAM)0, (LPARAM)&rect);
  return tips_hwnd;
}

static void winTipsSendMessage(Ihandle* ih, HWND tips_hwnd, UINT msg)
{
  TOOLINFO ti;

  ZeroMemory(&ti, sizeof(TOOLINFO));
  ti.cbSize = sizeof(TOOLINFO); 
  ti.uFlags = TTF_SUBCLASS;
  ti.hinst = iupwin_hinstance; 
  ti.uId = 0; 
  ti.hwnd = ih->handle; 
  ti.lpszText = LPSTR_TEXTCALLBACK;
  ti.rect.right = 3000;
  ti.rect.bottom = 3000;

  SendMessage(tips_hwnd, msg, 0, (LPARAM)&ti);
}

int iupdrvBaseSetTipAttrib(Ihandle* ih, const char* value)
{
  HWND tips_hwnd = (HWND)iupAttribGet(ih, "_IUPWIN_TIPSWIN");
  if (!tips_hwnd)
  {
    tips_hwnd = winTipsCreate(ih->handle);

    iupwinHandleAdd(ih, tips_hwnd);
    iupAttribSetStr(ih, "_IUPWIN_TIPSWIN", (char*)tips_hwnd);
  }

  if (value)
    winTipsSendMessage(ih, tips_hwnd, TTM_ADDTOOL);
  else
    winTipsSendMessage(ih, tips_hwnd, TTM_DELTOOL);

  return 1;
}

void iupwinTipsDestroy(Ihandle* ih)
{
  HWND tips_hwnd = (HWND)iupAttribGet(ih, "_IUPWIN_TIPSWIN");
  if (tips_hwnd)
  {
    winTipsSendMessage(ih, tips_hwnd, TTM_DELTOOL);

    iupAttribSetStr(ih, "_IUPWIN_TIPSWIN", NULL);

    iupwinHandleRemove(tips_hwnd);
    DestroyWindow(tips_hwnd);
  }
}

int iupdrvBaseSetTipVisibleAttrib(Ihandle* ih, const char* value)
{
  HWND tips_hwnd = (HWND)iupAttribGet(ih, "_IUPWIN_TIPSWIN");
  if (!tips_hwnd)
    return 0;

  if (iupStrBoolean(value))
    SendMessage(tips_hwnd, TTM_POPUP, 0, 0);  /* Works in Visual Styles Only */
  else
    SendMessage(tips_hwnd, TTM_POP, 0, 0);

  return 0;
}

char* iupdrvBaseGetTipVisibleAttrib(Ihandle* ih)
{
  HWND tips_hwnd = (HWND)iupAttribGet(ih, "_IUPWIN_TIPSWIN");
  if (!tips_hwnd)
    return NULL;

  if (IsWindowVisible(tips_hwnd))
    return "Yes";
  else
    return "No";
}

void iupwinTipsUpdateInfo(Ihandle* ih, HWND tips_hwnd)
{
  COLORREF color, tip_color;
  char* value;

  {
    HFONT hfont;
    value = iupAttribGetStr(ih, "TIPFONT");
    if (value)
    {
      if (iupStrEqualNoCase(value, "SYSTEM"))
        hfont = NULL;
      else
        hfont = iupwinGetHFont(value);
    }
    else
      hfont = (HFONT)iupwinGetHFontAttrib(ih);

    if (hfont)
    {
      HFONT tip_hfont = (HFONT)SendMessage(tips_hwnd, WM_GETFONT, 0, 0);
      if (tip_hfont != hfont)
        SendMessage(tips_hwnd, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE,0));
    }
  }

  iupwinGetColorRef(ih, "TIPBGCOLOR", &color);
  tip_color = (COLORREF)SendMessage(tips_hwnd, TTM_GETTIPBKCOLOR, 0, 0);
  if (color != tip_color)
    SendMessage(tips_hwnd, TTM_SETTIPBKCOLOR, (WPARAM)color, 0);

  iupwinGetColorRef(ih, "TIPFGCOLOR", &color);
  tip_color = (COLORREF)SendMessage(tips_hwnd, TTM_GETTIPTEXTCOLOR, 0, 0);
  if (color != tip_color)
    SendMessage(tips_hwnd, TTM_SETTIPTEXTCOLOR, (WPARAM)color, 0);

  {
    int balloon = IupGetInt(ih, "TIPBALLOON");  /* must use IupGetInt to use inheritance */
    DWORD style = GetWindowLong(tips_hwnd, GWL_STYLE);
    int tip_balloon = style & TTS_BALLOON? 1: 0; 
    if (tip_balloon != balloon)
    {
      if (balloon)
        style |= TTS_BALLOON;
      else
        style &= ~TTS_BALLOON;
      SetWindowLong(tips_hwnd, GWL_STYLE, style);
    }

    if (balloon)
    {
      char* balloon_title = IupGetAttribute(ih, "TIPBALLOONTITLE"); /* must use IupGetAttribute to use inheritance */
      int balloon_icon = IupGetInt(ih, "TIPBALLOONTITLEICON");  /* must use IupGetInt to use inheritance */
      SendMessage(tips_hwnd, TTM_SETTITLEA, balloon_icon, (LPARAM)balloon_title);
    }
    else
      SendMessage(tips_hwnd, TTM_SETTITLEA, 0, 0);
  }

  {
    int delay = IupGetInt(ih, "TIPDELAY"); /* must use IupGetInt to use inheritance */
    int tip_delay = SendMessage(tips_hwnd, TTM_GETDELAYTIME, TTDT_AUTOPOP, 0);
    if (delay != tip_delay)
      SendMessage(tips_hwnd, TTM_SETDELAYTIME, TTDT_AUTOPOP, (LPARAM)MAKELONG(delay, 0));
  }

  {
    TOOLINFO ti;

    ZeroMemory(&ti, sizeof(TOOLINFO));
    ti.cbSize = sizeof(TOOLINFO); 
    ti.uId = 0; 
    ti.hwnd = ih->handle;

    value = iupAttribGet(ih, "TIPRECT");
    if (value)
    {
      int x1, x2, y1, y2;
      sscanf(value, "%d %d %d %d", &x1, &y1, &x2, &y2);
      ti.rect.left = x1; ti.rect.right = x2;
      ti.rect.top = y1; ti.rect.bottom = y2;
    }
    else
      GetClientRect(ih->handle, &ti.rect);

    SendMessage(tips_hwnd, TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
  }
}

void iupwinTipsGetDispInfo(LPARAM lp)
{
  NMTTDISPINFO* tips_info;
  Ihandle* ih;
  HWND tips_hwnd;
  IFnii cb;

  if (!lp) 
    return;

  tips_info = (NMTTDISPINFO*)lp;
  ih = iupwinHandleGet(tips_info->hdr.hwndFrom);  /* hwndFrom is the tooltip window */
  if (!ih) 
    return;

  tips_hwnd = (HWND)iupAttribGet(ih, "_IUPWIN_TIPSWIN");
  if (tips_hwnd != tips_info->hdr.hwndFrom) 
    return;

  tips_info->hinst = NULL;

  cb = (IFnii)IupGetCallback(ih, "TIPS_CB");
  if (cb)
  {
    int x, y;
    iupdrvGetCursorPos(&x, &y);
    iupdrvScreenToClient(ih, &x, &y);
    cb(ih, x, y);
  }

  tips_info->lpszText = IupGetAttribute(ih, "TIP");  /* must use IupGetAttribute to use inheritance */

  iupwinTipsUpdateInfo(ih, tips_hwnd);
}
