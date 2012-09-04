/** \file
 * \brief IupDialog class
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_dlglist.h"
#include "iup_attrib.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_drvfont.h"
#include "iup_focus.h"
#include "iup_str.h"
#define _IUPDLG_PRIVATE
#include "iup_dialog.h"
#include "iup_image.h"

#include "iupwin_drv.h"
#include "iupwin_handle.h"
#include "iupwin_brush.h"
#include "iupwin_info.h"


#define IUPWIN_TRAY_NOTIFICATION 102

static int WM_HELPMSG;

static int winDialogSetBgColorAttrib(Ihandle* ih, const char* value);
static int winDialogSetTrayAttrib(Ihandle *ih, const char *value);

/****************************************************************
                     Utilities
****************************************************************/

int iupdrvDialogIsVisible(Ihandle* ih)
{
  return iupdrvIsVisible(ih);
}

void iupdrvDialogGetSize(Ihandle* ih, InativeHandle* handle, int *w, int *h)
{
  RECT rect;
  if (!handle)
    handle = ih->handle;
  GetWindowRect(handle, &rect);
  if (w) *w = rect.right-rect.left;
  if (h) *h = rect.bottom-rect.top;
}

void iupdrvDialogSetVisible(Ihandle* ih, int visible)
{
  ShowWindow(ih->handle, visible? ih->data->cmd_show: SW_HIDE);
}

void iupdrvDialogGetPosition(Ihandle *ih, InativeHandle* handle, int *x, int *y)
{
  RECT rect;
  if (!handle)
    handle = ih->handle;
  GetWindowRect(handle, &rect);
  if (x) *x = rect.left;
  if (y) *y = rect.top;
}

void iupdrvDialogSetPosition(Ihandle *ih, int x, int y)
{
  /* Only moves the window and places it at the top of the Z order. */
  SetWindowPos(ih->handle, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
}

static void winDialogGetWindowDecor(Ihandle* ih, int *border, int *caption)
{
  WINDOWINFO wi;
  wi.cbSize = sizeof(WINDOWINFO);
  GetWindowInfo(ih->handle, &wi);

  *border = wi.cxWindowBorders;

  if (wi.rcClient.bottom == wi.rcClient.top)
    *caption = wi.rcClient.bottom - wi.cyWindowBorders; 
  else
  {
    /* caption = window height - top border - client height */
    *caption = (wi.rcWindow.bottom-wi.rcWindow.top) - 2*wi.cyWindowBorders - (wi.rcClient.bottom-wi.rcClient.top); 
  }
}

void iupdrvDialogGetDecoration(Ihandle* ih, int *border, int *caption, int *menu)
{
  if (ih->data->menu)
    *menu = iupdrvMenuGetMenuBarSize(ih->data->menu);
  else
    *menu = 0;

  if (ih->handle)
  {
    winDialogGetWindowDecor(ih, border, caption);

    if (*menu)
      *caption -= *menu;
  }
  else
  {
    int has_titlebar = iupAttribGetBoolean(ih, "MAXBOX")  ||
                       iupAttribGetBoolean(ih, "MINBOX")  ||
                       iupAttribGetBoolean(ih, "MENUBOX") || 
                       IupGetAttribute(ih, "TITLE");  /* must use IupGetAttribute to check from the native implementation */

    *caption = 0;
    if (has_titlebar)
    {
      if (iupAttribGetBoolean(ih, "TOOLBOX") && iupAttribGet(ih, "PARENTDIALOG"))
        *caption = GetSystemMetrics(SM_CYSMCAPTION); /* tool window */
      else
        *caption = GetSystemMetrics(SM_CYCAPTION);   /* normal window */
    }

    *border = 0;
    if (iupAttribGetBoolean(ih, "RESIZE"))
    {
      /* has_border */
      *border = GetSystemMetrics(SM_CXFRAME);     /* Thickness of the sizing border around the perimeter of a window  */
    }                                             /* that can be resized, in pixels.                                  */
    else if (has_titlebar)
    {
      /* has_border */
      *border = GetSystemMetrics(SM_CXFIXEDFRAME);  /* Thickness of the frame around the perimeter of a window        */
    }                                               /* that has a caption but is not sizable, in pixels.              */
    else if (iupAttribGetBoolean(ih, "BORDER"))
    {
      /* has_border */
      *border = GetSystemMetrics(SM_CXBORDER);
    }
  }
}

int iupdrvDialogSetPlacement(Ihandle* ih)
{
  char* placement;

  ih->data->cmd_show = SW_SHOWNORMAL;
  ih->data->show_state = IUP_SHOW;

  if (iupAttribGetBoolean(ih, "FULLSCREEN"))
    return 1;
  
  placement = iupAttribGet(ih, "PLACEMENT");
  if (!placement)
  {
    if (IsIconic(ih->handle) || IsZoomed(ih->handle))
      ih->data->show_state = IUP_RESTORE;
    return 0;
  }

  if (iupStrEqualNoCase(placement, "MAXIMIZED"))
  {
    ih->data->cmd_show = SW_SHOWMAXIMIZED;
    ih->data->show_state = IUP_MAXIMIZE;
  }
  else if (iupStrEqualNoCase(placement, "MINIMIZED"))
  {
    ih->data->cmd_show = SW_SHOWMINIMIZED;
    ih->data->show_state = IUP_MINIMIZE;
  }
  else if (iupStrEqualNoCase(placement, "FULL"))
  {
    int width, height, x, y;
    int caption, border, menu;
    iupdrvDialogGetDecoration(ih, &border, &caption, &menu);

    /* the dialog will cover the task bar */
    iupdrvGetFullSize(&width, &height);

    /* position the decoration and menu outside the screen */
    x = -(border);
    y = -(border+caption+menu);

    width += 2*border;
    height += 2*border + caption + menu;

    /* set the new size and position */
    /* WM_SIZE will update the layout */
    SetWindowPos(ih->handle, HWND_TOP, x, y, width, height, 0);

    if (IsIconic(ih->handle) || IsZoomed(ih->handle))
      ih->data->show_state = IUP_RESTORE;
  }

  iupAttribSetStr(ih, "PLACEMENT", NULL); /* reset to NORMAL */

  return 1;
}

static void winDialogMDIRefreshMenu(Ihandle* ih)
{
  /* We manually update the menu when a MDI child is added or removed. */
  Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
  PostMessage(client->handle, WM_MDIREFRESHMENU, 0, 0);
}

static int winDialogMDICloseChildren(Ihandle* ih)
{
  Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
  if (iupObjectCheck(client))
  {
    HWND hWndChild = (HWND)SendMessage(client->handle, WM_MDIGETACTIVE, 0, 0);

    /* As long as the MDI client has a child, close it */
    while (hWndChild)
    {
      Ihandle* child = iupwinHandleGet(hWndChild); 
      if (iupObjectCheck(child) && iupAttribGetBoolean(child, "MDICHILD"))
      {
        Icallback cb = IupGetCallback(child, "CLOSE_CB");
        if (cb)
        {
          int ret = cb(child);
          if (ret == IUP_IGNORE)
            return 0;
          if (ret == IUP_CLOSE)
            IupExitLoop();
        }

        IupDestroy(child);
      }

      hWndChild = (HWND)SendMessage(client->handle, WM_MDIGETACTIVE, 0, 0);
    }
  }
  return 1;
}


/****************************************************************************
                            WindowProc
****************************************************************************/


static Ihandle* winMinMaxHandle = NULL;

static int winDialogCheckMinMaxInfo(Ihandle* ih, MINMAXINFO* minmax)
{
  int min_w = 1, min_h = 1;          /* MINSIZE default value */
  int max_w = 65535, max_h = 65535;  /* MAXSIZE default value */

  iupStrToIntInt(iupAttribGet(ih, "MINSIZE"), &min_w, &min_h, 'x');
  iupStrToIntInt(iupAttribGet(ih, "MAXSIZE"), &max_w, &max_h, 'x');

  minmax->ptMinTrackSize.x = min_w;
  minmax->ptMinTrackSize.y = min_h;
  minmax->ptMaxTrackSize.x = max_w;
  minmax->ptMaxTrackSize.y = max_h;

  if (winMinMaxHandle == ih)
    winMinMaxHandle = NULL;

  return 1;
}

static void winDialogResize(Ihandle* ih, int width, int height)
{
  IFnii cb;

  iupdrvDialogGetSize(ih, NULL, &(ih->currentwidth), &(ih->currentheight));

  cb = (IFnii)IupGetCallback(ih, "RESIZE_CB");
  if (!cb || cb(ih, width, height)!=IUP_IGNORE)  /* width and height here are for the client area */
  {
    ih->data->ignore_resize = 1;
    IupRefresh(ih);
    ih->data->ignore_resize = 0;
  }
}

static int winDialogBaseProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{
  if (iupwinBaseContainerProc(ih, msg, wp, lp, result))
    return 1;

  iupwinMenuDialogProc(ih, msg, wp, lp);

  switch (msg)
  {
  case WM_GETMINMAXINFO:
    {
      if (winDialogCheckMinMaxInfo(ih, (MINMAXINFO*)lp))
      {
        *result = 0;
        return 1;
      }
      break;
    }
  case WM_MOVE:
    {
      IFnii cb = (IFnii)IupGetCallback(ih, "MOVE_CB");
      int x, y;
      /* ignore LPARAM because they are the clientpos */
      iupdrvDialogGetPosition(ih, NULL, &x, &y);
      if (cb) cb(ih, x, y);
      break;
    }
  case WM_SIZE:
    {
      if (ih->data->ignore_resize)
        break;

      switch(wp)
      {
      case SIZE_MINIMIZED:
        {
          if (ih->data->show_state != IUP_MINIMIZE)
          {
            IFni show_cb = (IFni)IupGetCallback(ih, "SHOW_CB");
            ih->data->show_state = IUP_MINIMIZE;
            if (show_cb && show_cb(ih, IUP_MINIMIZE) == IUP_CLOSE)
              IupExitLoop();
          }
          break;
        }
      case SIZE_MAXIMIZED:
        {
          if (ih->data->show_state != IUP_MAXIMIZE)
          {
            IFni show_cb = (IFni)IupGetCallback(ih, "SHOW_CB");
            ih->data->show_state = IUP_MAXIMIZE;
            if (show_cb && show_cb(ih, IUP_MAXIMIZE) == IUP_CLOSE)
              IupExitLoop();
          }

          winDialogResize(ih, LOWORD(lp), HIWORD(lp));

          if (iupAttribGetBoolean(ih, "MDICHILD"))
          {
            /* WORKAROUND: when a child MDI dialog is maximized, 
               its title is displayed inside the MDI client area.
               So we force a MDI client size update */
            RECT rect;
            Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
            GetClientRect(client->handle, &rect);
            PostMessage(client->handle, WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(rect.right-rect.left, rect.bottom-rect.top));
          }
          break;
        }
      case SIZE_RESTORED:
        {
          if (ih->data->show_state == IUP_MAXIMIZE || ih->data->show_state == IUP_MINIMIZE)
          {
            IFni show_cb = (IFni)IupGetCallback(ih, "SHOW_CB");
            ih->data->show_state = IUP_RESTORE;
            if (show_cb && show_cb(ih, IUP_RESTORE) == IUP_CLOSE)
              IupExitLoop();
          }

          winDialogResize(ih, LOWORD(lp), HIWORD(lp));
          break;
        }
      }

      if (iupAttribGetBoolean(ih, "MDIFRAME"))
      {
        /* We are going to manually position the MDI client, 
           so abort MDI frame processing. */
        *result = 0;
        return 1;
      }
      else
        break;
    }
  case WM_USER+IUPWIN_TRAY_NOTIFICATION:
    {
      int dclick  = 0;
      int button  = 0;
      int pressed = 0;

      switch (lp)
      {
      case WM_LBUTTONDOWN:
        pressed = 1;
        button  = 1;
        break;
      case WM_MBUTTONDOWN:
        pressed = 1;
        button  = 2;
        break;
      case WM_RBUTTONDOWN:
        pressed = 1;
        button  = 3;
        break;
      case WM_LBUTTONDBLCLK:
        dclick = 1;
        button = 1;
        break;
      case WM_MBUTTONDBLCLK:
        dclick = 1;
        button = 2;
        break;
      case WM_RBUTTONDBLCLK:
        dclick = 1;
        button = 3;
        break;
      case WM_LBUTTONUP:
        button = 1;
        break;
      case WM_MBUTTONUP:
        button = 2;
        break;
      case WM_RBUTTONUP:
        button = 3;
        break;
      }

      if (button != 0)
      {
        IFniii cb = (IFniii)IupGetCallback(ih, "TRAYCLICK_CB");
        if (cb && cb(ih, button, pressed, dclick) == IUP_CLOSE)
          IupExitLoop();
      }

      break;
    }
  case WM_CLOSE:
    {
      Icallback cb = IupGetCallback(ih, "CLOSE_CB");
      if (cb)
      {
        int ret = cb(ih);
        if (ret == IUP_IGNORE)
        {
          *result = 0;
          return 1;
        }
        if (ret == IUP_CLOSE)
          IupExitLoop();
      }

      /* child mdi is automatically destroyed */
      if (iupAttribGetBoolean(ih, "MDICHILD"))
        IupDestroy(ih);
      else
      {
        if (!winDialogMDICloseChildren(ih))
        {
          *result = 0;
          return 1;
        }

        IupHide(ih); /* IUP default processing */
      }

      *result = 0;
      return 1;
    }
  case WM_COPYDATA:  /* usually from SetGlobal("SINGLEINSTANCE") */
    {
      COPYDATASTRUCT* cds = (COPYDATASTRUCT*)lp;
      IFnsi cb = (IFnsi)IupGetCallback(ih, "COPYDATA_CB");
      if (cb) cb(ih, cds->lpData, cds->cbData);
      break; 
    }
  case WM_SETCURSOR: 
    { 
      if (ih->handle == (HWND)wp && LOWORD(lp)==HTCLIENT)
      {
        HCURSOR hCur = (HCURSOR)iupAttribGet(ih, "_IUPWIN_HCURSOR");
        if (hCur)
        {
          SetCursor(hCur); 
          *result = 1;
          return 1;
        }
        else if (iupAttribGet(ih, "CURSOR"))
        {
          SetCursor(NULL); 
          *result = 1;
          return 1;
        }
      }
      break; 
    } 
  case WM_ERASEBKGND:
    {
      HBITMAP hBitmap = (HBITMAP)iupAttribGet(ih, "_IUPWIN_BACKGROUND_BITMAP");
      if (hBitmap) 
      {
        RECT rect;
        HDC hdc = (HDC)wp;

        HBRUSH hBrush = CreatePatternBrush(hBitmap);
        GetClientRect(ih->handle, &rect); 
        FillRect(hdc, &rect, hBrush); 
        DeleteObject(hBrush);

        /* return non zero value */
        *result = 1;
        return 1; 
      }
      else
      {
        unsigned char r, g, b;
        char* color = iupAttribGet(ih, "_IUPWIN_BACKGROUND_COLOR");
        if (iupStrToRGB(color, &r, &g, &b))
        {
          RECT rect;
          HDC hdc = (HDC)wp;

          SetDCBrushColor(hdc, RGB(r,g,b));
          GetClientRect(ih->handle, &rect); 
          FillRect(hdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH)); 

          /* return non zero value */
          *result = 1;
          return 1;
        }
      }
      break;
    }
  case WM_DESTROY:
    {
      /* Since WM_CLOSE changed the Windows default processing                            */
      /* WM_DESTROY is NOT received by IupDialogs                                         */
      /* Except when they are children of other IupDialogs and the parent is destroyed.   */
      /* So we have to destroy the child dialog.                                          */
      /* The application is responsable for destroying the children before this happen.   */
      IupDestroy(ih);
      break;
    }
  }

  if (msg == (UINT)WM_HELPMSG)
  {
    Ihandle* child = NULL;
    DWORD* struct_ptr = (DWORD*)lp;
    if (*struct_ptr == sizeof(CHOOSECOLOR))
    {
      CHOOSECOLOR* choosecolor = (CHOOSECOLOR*)lp;
      child = (Ihandle*)choosecolor->lCustData;
    }
    if (*struct_ptr == sizeof(CHOOSEFONT))
    {
      CHOOSEFONT* choosefont = (CHOOSEFONT*)lp;
      child = (Ihandle*)choosefont->lCustData;
    }

    if (child)
    {
      Icallback cb = IupGetCallback(child, "HELP_CB");
      if (cb && cb(child) == IUP_CLOSE)
        EndDialog((HWND)iupAttribGet(child, "HWND"), IDCANCEL);
    }
  }

  return 0;
}

static LRESULT CALLBACK winDialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{   
  LRESULT result;
  Ihandle *ih = iupwinHandleGet(hwnd); 
  if (!ih)
  {
    /* the first time WM_GETMINMAXINFO is called, Ihandle is not associated yet */
    if (msg == WM_GETMINMAXINFO && winMinMaxHandle)
    {
      if (winDialogCheckMinMaxInfo(winMinMaxHandle, (MINMAXINFO*)lp))
        return 0;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
  }

  if (winDialogBaseProc(ih, msg, wp, lp, &result))
    return result;

  return DefWindowProc(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK winDialogMDIChildProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{   
  LRESULT result;
  Ihandle *ih = iupwinHandleGet(hwnd); 
  if (!ih)
  {
    /* the first time WM_GETMINMAXINFO is called, Ihandle is not associated yet */
    if (msg == WM_GETMINMAXINFO && winMinMaxHandle)
    {
      if (winDialogCheckMinMaxInfo(winMinMaxHandle, (MINMAXINFO*)lp))
        return 0;
    }

    return DefMDIChildProc(hwnd, msg, wp, lp);
  }

  if (msg == WM_MDIACTIVATE)
  {
    HWND hNewActive = (HWND)lp;
    if (hNewActive == ih->handle)
    {
      Icallback cb = (Icallback)IupGetCallback(ih, "MDIACTIVATE_CB");
      if (cb) cb(ih);
    }
  }

  if (winDialogBaseProc(ih, msg, wp, lp, &result))
    return result;

  return DefMDIChildProc(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK winDialogMDIFrameProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{   
  LRESULT result;
  HWND hWndClient = NULL;
  Ihandle *ih = iupwinHandleGet(hwnd); 
  if (!ih)
  {
    /* the first time WM_GETMINMAXINFO is called, Ihandle is not associated yet */
    if (msg == WM_GETMINMAXINFO && winMinMaxHandle)
    {
      if (winDialogCheckMinMaxInfo(winMinMaxHandle, (MINMAXINFO*)lp))
        return 0;
    }

    return DefFrameProc(hwnd, hWndClient, msg, wp, lp);
  }

  {
    Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
    if (client) hWndClient = client->handle;
  }

  if (winDialogBaseProc(ih, msg, wp, lp, &result))
    return result;

  if (msg == WM_MENUCOMMAND)
  {
    int menuId = GetMenuItemID((HMENU)lp, (int)wp);
    if (menuId >= IUP_MDI_FIRSTCHILD && hWndClient)
    {
      /* we manually activate the MDI child when its menu item is selected. */
      HWND hChild = GetDlgItem(hWndClient, menuId);
      if (hChild)
        SendMessage(hWndClient, WM_MDIACTIVATE, (WPARAM)hChild, 0);
    }
    else if (menuId >= SC_SIZE && menuId <= SC_CONTEXTHELP)
    {
      /* we manually forward the message to the MDI child */
      HWND hChild = (HWND)SendMessage(hWndClient, WM_MDIGETACTIVE, 0, 0);
      if (hChild)
        SendMessage(hChild, WM_SYSCOMMAND, (WPARAM)menuId, 0);
    }
  }

  return DefFrameProc(hwnd, hWndClient, msg, wp, lp);
}

static void winDialogRegisterClass(int mdi)
{
  char* name;
  WNDCLASS wndclass;
  WNDPROC winproc;
  ZeroMemory(&wndclass, sizeof(WNDCLASS));
  
  if (mdi == 2)
  {
    name = "IupDialogMDIChild";
    winproc = (WNDPROC)winDialogMDIChildProc;
  }
  else if (mdi == 1)
  {
    name = "IupDialogMDIFrame";
    winproc = (WNDPROC)winDialogMDIFrameProc;
  }
  else
  {
    if (mdi == -1)
      name = "IupDialogControl";
    else
      name = "IupDialog";
    winproc = (WNDPROC)winDialogProc;
  }

  wndclass.hInstance      = iupwin_hinstance;
  wndclass.lpszClassName  = name;
  wndclass.lpfnWndProc    = (WNDPROC)winproc;
  wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);

  /* To use a standard system color, must increase the background-color value by one */
  if (mdi == 1)
    wndclass.hbrBackground  = (HBRUSH)(COLOR_APPWORKSPACE+1);  
  else
    wndclass.hbrBackground  = (HBRUSH)(COLOR_BTNFACE+1);

  if (mdi == 0)
    wndclass.style |= CS_SAVEBITS;

  if (mdi == -1)
    wndclass.style |=  CS_HREDRAW | CS_VREDRAW;
    
  RegisterClass(&wndclass);
}


/****************************************************************
                     dialog class functions
****************************************************************/

static int winDialogMapMethod(Ihandle* ih)
{
  InativeHandle* native_parent;
  DWORD dwStyle = WS_CLIPSIBLINGS, 
        dwExStyle = 0;
  int has_titlebar = 0,
      has_border = 0;
  char* classname = "IupDialog";

  char* title = iupAttribGet(ih, "TITLE"); 
  if (title)
    has_titlebar = 1;

  if (iupAttribGetBoolean(ih, "RESIZE"))
  {
    dwStyle |= WS_THICKFRAME;
    has_border = 1;
  }
  else
    iupAttribSetStr(ih, "MAXBOX", "NO");
  if (iupAttribGetBoolean(ih, "MENUBOX"))
  {
    dwStyle |= WS_SYSMENU;
    has_titlebar = 1;
  }
  if (iupAttribGetBoolean(ih, "MAXBOX"))
  {
    dwStyle |= WS_MAXIMIZEBOX;
    has_titlebar = 1;
  }
  if (iupAttribGetBoolean(ih, "MINBOX"))
  {
    dwStyle |= WS_MINIMIZEBOX;
    has_titlebar = 1;
  }
  if (iupAttribGetBoolean(ih, "BORDER") || has_titlebar)
    has_border = 1;

  if (iupAttribGetBoolean(ih, "MDICHILD"))
  {
    Ihandle *client;

    /* must have a parent dialog (the mdi frame) */
    Ihandle* parent = IupGetAttributeHandle(ih, "PARENTDIALOG");
    if (!parent || !parent->handle)
      return IUP_ERROR;

    /* set when the mdi client is mapped */
    client = (Ihandle*)iupAttribGet(parent, "MDICLIENT_HANDLE");
    if (!client)
      return IUP_ERROR;

    /* store the mdi client handle in each mdi child also */
    iupAttribSetStr(ih, "MDICLIENT_HANDLE", (char*)client);

    classname = "IupDialogMDIChild";

    /* The actual parent is the mdi client */
    native_parent = client->handle;

    dwStyle |= WS_CHILD;
    if (has_titlebar)
      dwStyle |= WS_CAPTION;
    else if (has_border)
      dwStyle |= WS_BORDER;

    if (!IupGetName(ih))
      iupAttribSetHandleName(ih);
  }
  else
  {
    native_parent = iupDialogGetNativeParent(ih);

    if (native_parent)
    {
      dwStyle |= WS_POPUP;

      if (has_titlebar)
        dwStyle |= WS_CAPTION;
      else if (has_border)
        dwStyle |= WS_BORDER;
    }
    else
    {
      if (has_titlebar)
      {
        dwStyle |= WS_OVERLAPPED;
      }
      else 
      {
        if (has_border)
          dwStyle |= WS_POPUP | WS_BORDER;
        else
          dwStyle |= WS_POPUP;

        dwExStyle |= WS_EX_NOACTIVATE; /* this will hide it from the taskbar */ 
      }
    }

    if (iupAttribGetBoolean(ih, "MDIFRAME"))
    {
      COLORREF color = GetSysColor(COLOR_BTNFACE);
      iupAttribSetStrf(ih, "_IUPWIN_BACKGROUND_COLOR", "%d %d %d", (int)GetRValue(color), 
                                                                   (int)GetGValue(color), 
                                                                   (int)GetBValue(color));
      classname = "IupDialogMDIFrame";
    }
  }

  if (iupAttribGetBoolean(ih, "TOOLBOX") && native_parent)
    dwExStyle |= WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE;

  if (iupAttribGetBoolean(ih, "DIALOGFRAME") && native_parent)
    dwExStyle |= WS_EX_DLGMODALFRAME;  /* this will hide the MENUBOX but not the close button */

  iupwinGetNativeParentStyle(ih, &dwExStyle, &dwStyle);

  if (iupAttribGetBoolean(ih, "HELPBUTTON"))
    dwExStyle |= WS_EX_CONTEXTHELP;

  if (iupAttribGetBoolean(ih, "CONTROL") && native_parent) 
  {
    /* TODO: this were used by LuaCom to create embeded controls, 
       don't know if it is still working */
    dwStyle = WS_CHILD | WS_TABSTOP | WS_CLIPCHILDREN;
    classname = "IupDialogControl";
  }

  /* CreateWindowEx will send WM_GETMINMAXINFO before Ihandle is associated with HWND */
  if (iupAttribGet(ih, "MINSIZE") || iupAttribGet(ih, "MAXSIZE"))
    winMinMaxHandle = ih;

  /* size will be updated in IupRefresh -> winDialogLayoutUpdate */
  /* position will be updated in iupDialogShowXY              */

  if (iupAttribGetBoolean(ih, "MDICHILD"))
    ih->handle = CreateMDIWindow(classname, 
                                title,              /* title */
                                dwStyle,            /* style */
                                0,                  /* x-position */
                                0,                  /* y-position */
                                100,                /* horizontal size - set this to avoid size calculation problems */
                                100,                /* vertical size */
                                native_parent,      /* owner window */
                                iupwin_hinstance,   /* instance of app. */
                                0);                 /* no creation parameters */
  else
    ih->handle = CreateWindowEx(dwExStyle,          /* extended styles */
                              classname,          /* class */
                              title,              /* title */
                              dwStyle,            /* style */
                              0,                  /* x-position */
                              0,                  /* y-position */
                              100,                /* horizontal size - set this to avoid size calculation problems */
                              100,                /* vertical size */
                              native_parent,      /* owner window */
                              (HMENU)0,           /* Menu or child-window identifier */
                              iupwin_hinstance,   /* instance of app. */
                              NULL);              /* no creation parameters */
  if (!ih->handle)
    return IUP_ERROR;

  /* associate HWND with Ihandle*, all Win32 controls must call this. */
  iupwinHandleAdd(ih, ih->handle);

  if (iupStrEqual(classname, "IupDialogMDIChild")) /* hides the mdi child */
    ShowWindow(ih->handle, SW_HIDE);

  /* configure for DROP of files */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSetStr(ih, "DROPFILESTARGET", "YES");

  /* Reset attributes handled during creation that */
  /* also can be changed later, and can be consulted from the native system. */
  iupAttribSetStr(ih, "TITLE", NULL); 

  /* Ignore VISIBLE before mapping */
  iupAttribSetStr(ih, "VISIBLE", NULL);

  /* Set the default CmdShow for ShowWindow */
  ih->data->cmd_show = SW_SHOWNORMAL;

  if (iupAttribGetBoolean(ih, "MDICHILD"))
    winDialogMDIRefreshMenu(ih);

  return IUP_NOERROR;
}

static void winDialogUnMapMethod(Ihandle* ih)
{
  if (ih->data->menu) 
  {
    ih->data->menu->handle = NULL; /* the dialog will destroy the native menu */
    IupDestroy(ih->data->menu);  
  }

  if (iupAttribGet(ih, "_IUPDLG_HASTRAY"))
    winDialogSetTrayAttrib(ih, NULL);

  iupwinTipsDestroy(ih);
  iupwinDestroyDragDrop(ih);

  /* remove the association before destroying */
  iupwinHandleRemove(ih->handle);

  /* Destroys the window, so we can destroy the class */
  if (iupAttribGetBoolean(ih, "MDICHILD")) 
  {
    /* for MDICHILDs must send WM_MDIDESTROY, instead of calling DestroyWindow */
    Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
    SendMessage(client->handle, WM_MDIDESTROY, (WPARAM)ih->handle, 0);

    winDialogMDIRefreshMenu(ih);
  }
  else
    DestroyWindow(ih->handle); /* this will destroy the Windows children also. */
                               /* but IupDestroy already destroyed the IUP children */
                               /* so it is safe to call DestroyWindow */
}

static void winDialogLayoutUpdateMethod(Ihandle *ih)
{
  if (ih->data->ignore_resize)
    return;

  ih->data->ignore_resize = 1;

  /* for dialogs the position is not updated here */
  SetWindowPos(ih->handle, 0, 0, 0, ih->currentwidth, ih->currentheight,
               SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING);

  ih->data->ignore_resize = 0;
}

static void winDialogReleaseMethod(Iclass* ic)
{
  (void)ic;
  if (iupwinClassExist("IupDialog"))
  {
    UnregisterClass("IupDialog", iupwin_hinstance);
    UnregisterClass("IupDialogControl", iupwin_hinstance);
    UnregisterClass("IupDialogMDIChild", iupwin_hinstance);
    UnregisterClass("IupDialogMDIFrame", iupwin_hinstance);
  }
}



/****************************************************************************
                                   Attributes
****************************************************************************/


static char* winDialogGetClientOffsetAttrib(Ihandle *ih)
{
  (void)ih;
  return "0x0";
}

static char* winDialogGetClientSizeAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  RECT rect;
  GetClientRect(ih->handle, &rect);
  sprintf(str, "%dx%d", (int)(rect.right-rect.left), (int)(rect.bottom-rect.top));
  return str;
}

static int winDialogSetBgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (iupStrToRGB(value, &r, &g, &b))
  {
    iupAttribStoreStr(ih, "_IUPWIN_BACKGROUND_COLOR", value);
    iupAttribSetStr(ih, "_IUPWIN_BACKGROUND_BITMAP", NULL);
    RedrawWindow(ih->handle, NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN); /* post WM_ERASEBKGND and WM_PAINT */
    return 1;
  }
  return 0;
}

static int winDialogSetBackgroundAttrib(Ihandle* ih, const char* value)
{
  if (winDialogSetBgColorAttrib(ih, value))
    return 1;
  else
  {
    HBITMAP hBitmap = iupImageGetImage(value, ih, 0);
    if (hBitmap)
    {
      iupAttribSetStr(ih, "_IUPWIN_BACKGROUND_COLOR", NULL);
      iupAttribSetStr(ih, "_IUPWIN_BACKGROUND_BITMAP", (char*)hBitmap);
      RedrawWindow(ih->handle, NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN); /* post WM_ERASEBKGND and WM_PAINT */
      return 1;
    }
  }

  return 0;
}

static HWND iupwin_mdifirst = NULL;
static HWND iupwin_mdinext = NULL;

static char* winDialogGetMdiActiveAttrib(Ihandle *ih)
{
  Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
  if (client)
  {
    HWND hchild = (HWND)SendMessage(client->handle, WM_MDIGETACTIVE, 0, 0);
    Ihandle* child = iupwinHandleGet(hchild); 
    if (child)
    {
      iupwin_mdinext = NULL;
      iupwin_mdifirst = hchild;
      return IupGetName(child);
    }
  }

  iupwin_mdifirst = NULL;
  iupwin_mdinext = NULL;
  return NULL;
}

static char* winDialogGetMdiNextAttrib(Ihandle *ih)
{
  Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
  if (client)
  {
    Ihandle* child;
    HWND hchild = iupwin_mdinext? iupwin_mdinext: iupwin_mdifirst;

    /* Skip the icon title windows */
    while (hchild && GetWindow (hchild, GW_OWNER))
      hchild = GetWindow(hchild, GW_HWNDNEXT);

    if (!hchild || hchild == iupwin_mdifirst)
    {
      iupwin_mdinext = NULL;
      return NULL;
    }

    child = iupwinHandleGet(hchild); 
    if (child)
    {
      iupwin_mdinext = hchild;
      return IupGetName(child);
    }
  }

  iupwin_mdinext = NULL;
  return NULL;
}

typedef BOOL (WINAPI*winSetLayeredWindowAttributes)(
  HWND hwnd,
  COLORREF crKey,
  BYTE bAlpha,
  DWORD dwFlags);

static int winDialogSetOpacityAttrib(Ihandle *ih, const char *value)
{
  DWORD dwExStyle = GetWindowLong(ih->handle, GWL_EXSTYLE);
  if (!value)
  {
    if (dwExStyle & WS_EX_LAYERED)
    {
      dwExStyle &= ~WS_EX_LAYERED;   /* remove the style */
      SetWindowLong(ih->handle, GWL_EXSTYLE, dwExStyle);
      RedrawWindow(ih->handle, NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN); /* invalidate everything and all children */
    }
    return 0;
  }

  if (!(dwExStyle & WS_EX_LAYERED))
  {
    dwExStyle |= WS_EX_LAYERED;   /* add the style */
    SetWindowLong(ih->handle, GWL_EXSTYLE, dwExStyle);
  }

  {
    static winSetLayeredWindowAttributes mySetLayeredWindowAttributes = NULL;

    int opacity;
    if (!iupStrToInt(value, &opacity))
      return 0;

    if (!mySetLayeredWindowAttributes)
    {
      HMODULE hinstDll = LoadLibrary("user32.dll");
      if (hinstDll)
        mySetLayeredWindowAttributes = (winSetLayeredWindowAttributes)GetProcAddress(hinstDll, "SetLayeredWindowAttributes");
    }

    if (mySetLayeredWindowAttributes)
      mySetLayeredWindowAttributes(ih->handle, 0, (BYTE)opacity, LWA_ALPHA);
  }

  RedrawWindow(ih->handle, NULL, NULL, RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN); /* invalidate everything and all children */
  return 1;
}

static int winDialogSetMdiArrangeAttrib(Ihandle *ih, const char *value)
{
  Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
  if (client)
  {
    UINT msg = 0;
    WPARAM wp = 0;

    if (iupStrEqualNoCase(value, "TILEHORIZONTAL"))
    {
      msg = WM_MDITILE;
      wp = MDITILE_HORIZONTAL;
    }
    else if (iupStrEqualNoCase(value, "TILEVERTICAL"))
    {
      msg = WM_MDITILE;
      wp = MDITILE_VERTICAL;
    }
    else if (iupStrEqualNoCase(value, "CASCADE"))
      msg = WM_MDICASCADE;
    else if (iupStrEqualNoCase(value, "ICON")) 
      msg = WM_MDIICONARRANGE;

    if (msg)
      SendMessage(client->handle, msg, wp, 0);
  }
  return 0;
}

static int winDialogSetMdiActivateAttrib(Ihandle *ih, const char *value)
{
  Ihandle* client = (Ihandle*)iupAttribGet(ih, "MDICLIENT_HANDLE");
  if (client)
  {
    Ihandle* child = IupGetHandle(value);
    if (child)
      SendMessage(client->handle, WM_MDIACTIVATE, (WPARAM)child->handle, 0);
    else
    {
      HWND hchild = (HWND)SendMessage(client->handle, WM_MDIGETACTIVE, 0, 0);
      if (iupStrEqualNoCase(value, "NEXT"))
        SendMessage(client->handle, WM_MDINEXT, (WPARAM)hchild, TRUE);
      else if (iupStrEqualNoCase(value, "PREVIOUS"))
        SendMessage(client->handle, WM_MDINEXT, (WPARAM)hchild, FALSE);
    }
  }
  return 0;
}

static int winDialogSetMdiCloseAllAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  winDialogMDICloseChildren(ih);
  return 0;
}

static void winDialogTrayMessage(HWND hWnd, DWORD dwMessage, HICON hIcon, const char* value)
{
  NOTIFYICONDATA tnd;
  memset(&tnd, 0, sizeof(NOTIFYICONDATA));

  tnd.cbSize  = sizeof(NOTIFYICONDATA);
  tnd.hWnd    = hWnd;
  tnd.uID     = 1000;

  if (dwMessage == NIM_ADD)
  {
    tnd.uFlags = NIF_MESSAGE;
    tnd.uCallbackMessage = WM_USER+IUPWIN_TRAY_NOTIFICATION;
  }
  else if (dwMessage == NIM_MODIFY)
  {
    if (hIcon)  
    {
      tnd.uFlags |= NIF_ICON;
      tnd.hIcon = hIcon;
    }

    if (value) 
    {
      tnd.uFlags |= NIF_TIP;
      iupStrCopyN((char*)tnd.szTip, sizeof(tnd.szTip), value);
    }
  }

  Shell_NotifyIcon(dwMessage, &tnd);
}

static void winDialogTrayBalloonMessage(Ihandle *ih, const char* value)
{
  NOTIFYICONDATA tnd;
  memset(&tnd, 0, sizeof(NOTIFYICONDATA));

  tnd.cbSize  = sizeof(NOTIFYICONDATA);
  tnd.hWnd    = ih->handle;
  tnd.uID     = 1000;
  tnd.uFlags |= NIF_INFO;

  /* set to NULL to remove the tooltip */
  if (value) 
  {
    char* balloon_title;

    iupStrCopyN((char*)tnd.szInfo, sizeof(tnd.szInfo), value);

    tnd.uTimeout = IupGetInt(ih, "TRAYTIPBALLOONDELAY"); /* must use IupGetInt to use inheritance */

    balloon_title = IupGetAttribute(ih, "TRAYTIPBALLOONTITLE");
    if (balloon_title)
      iupStrCopyN((char*)tnd.szInfoTitle, sizeof(tnd.szInfoTitle), balloon_title);

    tnd.dwInfoFlags = IupGetInt(ih, "TRAYTIPBALLOONTITLEICON");
  }

  Shell_NotifyIcon(NIM_MODIFY, &tnd);
}

static int winDialogCheckTray(Ihandle *ih)
{
  if (iupAttribGet(ih, "_IUPDLG_HASTRAY"))
    return 1;

  if (iupAttribGetBoolean(ih, "TRAY"))
  {
    winDialogTrayMessage(ih->handle, NIM_ADD, NULL, NULL);
    iupAttribSetStr(ih, "_IUPDLG_HASTRAY", "YES");
    return 1;
  }

  return 0;
}

static int winDialogSetTrayAttrib(Ihandle *ih, const char *value)
{
  int tray = iupStrBoolean(value);
  if (iupAttribGet(ih, "_IUPDLG_HASTRAY"))
  {
    if (!tray)
    {
      winDialogTrayMessage(ih->handle, NIM_DELETE, NULL, NULL);
      iupAttribSetStr(ih, "_IUPDLG_HASTRAY", NULL);
    }
  }
  else
  {
    if (tray)
    {
      winDialogTrayMessage(ih->handle, NIM_ADD, NULL, NULL);
      iupAttribSetStr(ih, "_IUPDLG_HASTRAY", "YES");
    }
  }
  return 1;
}

static int winDialogSetTrayTipAttrib(Ihandle *ih, const char *value)
{
  if (winDialogCheckTray(ih))
  {
    int balloon = IupGetInt(ih, "TRAYTIPBALLOON");  /* must use IupGetInt to use inheritance */
    if (balloon)
      winDialogTrayBalloonMessage(ih, value);
    else
      winDialogTrayMessage(ih->handle, NIM_MODIFY, NULL, value);
  }
  return 1;
}

static int winDialogSetTrayImageAttrib(Ihandle *ih, const char *value)
{
  if (winDialogCheckTray(ih))
  {
    HICON hIcon = (HICON)iupImageGetIcon(value);
    if (hIcon)
      winDialogTrayMessage(ih->handle, NIM_MODIFY, hIcon, NULL);
  }
  return 1;
}

static int winDialogSetBringFrontAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    SetForegroundWindow(ih->handle);
  return 0;
}

static char* winDialogGetActiveWindowAttrib(Ihandle* ih)
{
  WINDOWINFO wininfo;
  wininfo.cbSize = sizeof(WINDOWINFO);
  GetWindowInfo(ih->handle, &wininfo);
  if (wininfo.dwWindowStatus & WS_ACTIVECAPTION)
    return "Yes";
  else
    return "No";
}    

static int winDialogSetTopMostAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    SetWindowPos(ih->handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
  else
    SetWindowPos(ih->handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
  return 1;
}

static int winDialogSetIconAttrib(Ihandle* ih, const char *value)
{
  if (!value)
    SendMessage(ih->handle, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)NULL);
  else
  {
    HICON icon = iupImageGetIcon(value);
    if (icon)
      SendMessage(ih->handle, WM_SETICON, (WPARAM) ICON_BIG, (LPARAM)icon);    
  }

  if (IsIconic(ih->handle))
    RedrawWindow(ih->handle, NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW); /* redraw the icon */
  else
    RedrawWindow(ih->handle, NULL, NULL, RDW_FRAME|RDW_UPDATENOW); /* only the frame needs to be redrawn */

  return 1;
}

static int winDialogSetFullScreenAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
  {
    if (!iupAttribGet(ih, "_IUPWIN_FS_STYLE"))
    {
      int width, height;
      LONG off_style, new_style;

      BOOL visible = ShowWindow (ih->handle, SW_HIDE);

      /* remove the decorations */
      off_style = WS_BORDER | WS_THICKFRAME | WS_CAPTION | 
                  WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;
      new_style = GetWindowLong(ih->handle, GWL_STYLE);
      iupAttribSetStr(ih, "_IUPWIN_FS_STYLE", (char*)new_style);
      new_style &= (~off_style);
      SetWindowLong(ih->handle, GWL_STYLE, new_style);

      /* save the previous decoration attributes */
      iupAttribStoreStr(ih, "_IUPWIN_FS_MAXBOX", iupAttribGet(ih, "MAXBOX"));
      iupAttribStoreStr(ih, "_IUPWIN_FS_MINBOX", iupAttribGet(ih, "MINBOX"));
      iupAttribStoreStr(ih, "_IUPWIN_FS_MENUBOX",iupAttribGet(ih, "MENUBOX"));
      iupAttribStoreStr(ih, "_IUPWIN_FS_RESIZE", iupAttribGet(ih, "RESIZE"));
      iupAttribStoreStr(ih, "_IUPWIN_FS_BORDER", iupAttribGet(ih, "BORDER"));
      iupAttribStoreStr(ih, "_IUPWIN_FS_TITLE",  IupGetAttribute(ih, "TITLE"));  /* must use IupGetAttribute to check from the native implementation */

      /* save the previous position and size */
      iupAttribStoreStr(ih, "_IUPWIN_FS_X", IupGetAttribute(ih, "X"));  /* must use IupGetAttribute to check from the native implementation */
      iupAttribStoreStr(ih, "_IUPWIN_FS_Y", IupGetAttribute(ih, "Y"));
      iupAttribStoreStr(ih, "_IUPWIN_FS_SIZE", IupGetAttribute(ih, "RASTERSIZE"));

      /* remove the decorations attributes */
      iupAttribSetStr(ih, "MAXBOX", "NO");
      iupAttribSetStr(ih, "MINBOX", "NO");
      iupAttribSetStr(ih, "MENUBOX", "NO");
      IupSetAttribute(ih, "TITLE", NULL);
      iupAttribSetStr(ih, "RESIZE", "NO");
      iupAttribSetStr(ih, "BORDER", "NO");

      /* full screen size */
      iupdrvGetFullSize(&width, &height);

      SetWindowPos(ih->handle, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED);

      /* layout will be updated in WM_SIZE */
      if (visible)
        ShowWindow(ih->handle, SW_SHOW);
    }
  }
  else
  {
    LONG style = (LONG)iupAttribGet(ih, "_IUPWIN_FS_STYLE");
    if (style)
    {
      BOOL visible = ShowWindow(ih->handle, SW_HIDE);

      /* restore the decorations attributes */
      iupAttribStoreStr(ih, "MAXBOX", iupAttribGet(ih, "_IUPWIN_FS_MAXBOX"));
      iupAttribStoreStr(ih, "MINBOX", iupAttribGet(ih, "_IUPWIN_FS_MINBOX"));
      iupAttribStoreStr(ih, "MENUBOX",iupAttribGet(ih, "_IUPWIN_FS_MENUBOX"));
      IupSetAttribute(ih, "TITLE",  iupAttribGet(ih, "_IUPWIN_FS_TITLE"));  /* TITLE is not stored in the HashTable */
      iupAttribStoreStr(ih, "RESIZE", iupAttribGet(ih, "_IUPWIN_FS_RESIZE"));
      iupAttribStoreStr(ih, "BORDER", iupAttribGet(ih, "_IUPWIN_FS_BORDER"));

      /* restore the decorations */
      SetWindowLong(ih->handle, GWL_STYLE, style);

      /* restore position and size */
      SetWindowPos(ih->handle, HWND_TOP, 
                   iupAttribGetInt(ih, "_IUPWIN_FS_X"), 
                   iupAttribGetInt(ih, "_IUPWIN_FS_Y"), 
                   IupGetInt(ih, "_IUPWIN_FS_SIZE"), 
                   IupGetInt2(ih, "_IUPWIN_FS_SIZE"), 0);

      /* layout will be updated in WM_SIZE */
      if (visible)
        ShowWindow(ih->handle, SW_SHOW);

      /* remove auxiliar attributes */
      iupAttribSetStr(ih, "_IUPWIN_FS_MAXBOX", NULL);
      iupAttribSetStr(ih, "_IUPWIN_FS_MINBOX", NULL);
      iupAttribSetStr(ih, "_IUPWIN_FS_MENUBOX",NULL);
      iupAttribSetStr(ih, "_IUPWIN_FS_TITLE",  NULL);
      iupAttribSetStr(ih, "_IUPWIN_FS_RESIZE", NULL);
      iupAttribSetStr(ih, "_IUPWIN_FS_BORDER", NULL);

      iupAttribSetStr(ih, "_IUPWIN_FS_X", NULL);
      iupAttribSetStr(ih, "_IUPWIN_FS_Y", NULL);
      iupAttribSetStr(ih, "_IUPWIN_FS_SIZE", NULL);

      iupAttribSetStr(ih, "_IUPWIN_FS_STYLE", NULL);
    }
  }
  return 1;
}

void iupdrvDialogInitClass(Iclass* ic)
{
  if (!iupwinClassExist("IupDialog"))
  {
    winDialogRegisterClass(0);
    winDialogRegisterClass(1);
    winDialogRegisterClass(2);
    winDialogRegisterClass(-1);

    WM_HELPMSG = RegisterWindowMessage(HELPMSGSTRING);
  }

  /* Driver Dependent Class functions */
  ic->Map = winDialogMapMethod;
  ic->UnMap = winDialogUnMapMethod;
  ic->LayoutUpdate = winDialogLayoutUpdateMethod;
  ic->Release = winDialogReleaseMethod;

  /* Callback Windows Only*/
  iupClassRegisterCallback(ic, "MDIACTIVATE_CB", "");

  /* Callback Windows and GTK Only */
  iupClassRegisterCallback(ic, "TRAYCLICK_CB", "iii");

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, winDialogSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "TITLE", iupdrvBaseGetTitleAttrib, iupdrvBaseSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* Base Container */
  iupClassRegisterAttribute(ic, "CLIENTSIZE", winDialogGetClientSizeAttrib, iupDialogSetClientSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);  /* dialog is the only not read-only */
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", winDialogGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* IupDialog only */
  iupClassRegisterAttribute(ic, "BACKGROUND", NULL, winDialogSetBackgroundAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ICON", NULL, winDialogSetIconAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FULLSCREEN", NULL, winDialogSetFullScreenAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SAVEUNDER", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MINSIZE", NULL, iupBaseSetMinSizeAttrib, IUPAF_SAMEASSYSTEM, "1x1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MAXSIZE", NULL, iupBaseSetMaxSizeAttrib, IUPAF_SAMEASSYSTEM, "65535x65535", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* IupDialog Windows Only */
  iupClassRegisterAttribute(ic, "HWND", iupBaseGetWidAttrib, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDIARRANGE", NULL, winDialogSetMdiArrangeAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDIACTIVATE", NULL, winDialogSetMdiActivateAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDICLOSEALL", NULL, winDialogSetMdiCloseAllAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDIACTIVE", winDialogGetMdiActiveAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDINEXT", winDialogGetMdiNextAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OPACITY", NULL, winDialogSetOpacityAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LAYERALPHA", NULL, winDialogSetOpacityAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BRINGFRONT", NULL, winDialogSetBringFrontAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COMPOSITED", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED);

  iupClassRegisterAttribute(ic, "CONTROL", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HELPBUTTON", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOOLBOX", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDIFRAME", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDICLIENT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDIMENU", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MDICHILD", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* IupDialog Windows and GTK Only */
  iupClassRegisterAttribute(ic, "ACTIVEWINDOW", winDialogGetActiveWindowAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOPMOST", NULL, winDialogSetTopMostAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOPMOST", NULL, winDialogSetTopMostAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TRAY", NULL, winDialogSetTrayAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TRAYIMAGE", NULL, winDialogSetTrayImageAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TRAYTIP", NULL, winDialogSetTrayTipAttrib, NULL, NULL, IUPAF_NO_INHERIT);

  /* IupDialog Windows Only */
  iupClassRegisterAttribute(ic, "TRAYTIPDELAY", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TRAYTIPBALLOON", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TRAYTIPBALLOONTITLE", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TRAYTIPBALLOONTITLEICON", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);

  /* Not Supported */
  iupClassRegisterAttribute(ic, "DIALOGHINT", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}
