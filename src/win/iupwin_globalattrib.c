/** \file
 * \brief Windows Driver iupdrvSetGlobal
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_key.h"
#include "iup_class.h"

#include "iupwin_drv.h"


static int win_monitor_index = 0;
static HANDLE win_singleintance = NULL;
static HWND win_findwindow = NULL;
static HHOOK win_OldGetMessageHook = NULL;


int iupdrvCheckMainScreen(int *w, int *h)
{
  (void)w;
  (void)h;
  return 0;
}

static int winGlobalSetMutex(const char* name)
{
  if (win_singleintance)
    ReleaseMutex(win_singleintance);

  /* try to create a mutex (will fail if already one of that name) */
  win_singleintance = CreateMutex(NULL, FALSE, name);

  /* Return TRUE if existing semaphore opened */
  if (win_singleintance != NULL && GetLastError()==ERROR_ALREADY_EXISTS)
  {
    CloseHandle(win_singleintance);
    return 1;
  }

  /* wasn’t found, new one created therefore return FALSE */
  return (win_singleintance == NULL);
}

static BOOL CALLBACK winGlobalEnumWindowProc(HWND hWnd, LPARAM lParam)
{
  char* name = (char*)lParam;
  char str[256];
  int len = GetWindowText(hWnd, str, 256);
  if (len)
  {
    if (iupStrEqualPartial(str, name))
    {
      win_findwindow = hWnd;
      return FALSE;
    }
  }

  return TRUE;  /* continue searching */
}

static HWND winGlobalFindWindow(const char* name)
{
  win_findwindow = NULL;
  EnumWindows(winGlobalEnumWindowProc, (LPARAM)name);
  return win_findwindow;
}

static void winGlobalFindInstance(const char* name)
{
  HWND hWnd = winGlobalFindWindow(name);
  if (hWnd)
  {
    LPTSTR cmdLine = GetCommandLine();

    SetForegroundWindow(hWnd);

    /* Command line is not empty. Send it to the first instance. */ 
    if (strlen(cmdLine) != 0) 
    {
      COPYDATASTRUCT cds;
      cds.dwData = (ULONG_PTR)"IUP_DATA";
      cds.cbData = strlen(cmdLine)+1;
      cds.lpData = cmdLine;
      SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
    }
  }
}

static BOOL CALLBACK winGlobalMonitorInfoEnum(HMONITOR handle, HDC handle_dc, LPRECT rect, LPARAM data)
{
  RECT* monitors_rect = (RECT*)data;
  monitors_rect[win_monitor_index] = *rect;
  win_monitor_index++;
  (void)handle_dc;
  (void)handle;
  return TRUE;
}

static LRESULT CALLBACK winHookGetMessageProc(int hcode, WPARAM gm_wp, LPARAM gm_lp)
{
  MSG* gm_msg = (MSG*)gm_lp;
  UINT msg = gm_msg->message;
  WPARAM wp = gm_msg->wParam;
  LPARAM lp = gm_msg->lParam;
  POINT pt = gm_msg->pt;
  static int last_button = 0;
  static int last_pressed = 0;
  char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;

  if (hcode!=HC_ACTION)
    return CallNextHookEx(win_OldGetMessageHook, hcode, gm_wp, gm_lp);

  switch (msg)
  {
  case WM_MOUSEWHEEL:
    {
      IFfiis cb = (IFfiis)IupGetFunction("GLOBALWHEEL_CB");
      if (cb)
      {
        short delta = (short)HIWORD(wp);

        iupwinButtonKeySetStatus(LOWORD(wp), status, 0);
        
        cb((float)delta/120.0f, LOWORD(lp), HIWORD(lp), status);
      }
      break;
    }
  case WM_NCXBUTTONDBLCLK:
  case WM_NCLBUTTONDBLCLK:
  case WM_NCMBUTTONDBLCLK:
  case WM_NCRBUTTONDBLCLK:
  case WM_NCXBUTTONDOWN:
  case WM_NCLBUTTONDOWN:
  case WM_NCMBUTTONDOWN:
  case WM_NCRBUTTONDOWN:
  case WM_XBUTTONDBLCLK:
  case WM_LBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_XBUTTONDOWN:
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
    {
      int doubleclick = 0, button = 0;

      IFiiiis cb = (IFiiiis) IupGetFunction("GLOBALBUTTON_CB");
      if (!cb)
        break;

      if (msg==WM_XBUTTONDBLCLK || msg==WM_NCXBUTTONDBLCLK ||
          msg==WM_LBUTTONDBLCLK || msg==WM_NCLBUTTONDBLCLK ||
          msg==WM_MBUTTONDBLCLK || msg==WM_NCMBUTTONDBLCLK ||
          msg==WM_RBUTTONDBLCLK || msg==WM_NCRBUTTONDBLCLK)
        doubleclick = 1;

      if (msg>=WM_MOUSEFIRST && msg<=WM_MOUSELAST)
        iupwinButtonKeySetStatus(LOWORD(wp), status, doubleclick);
      else if (doubleclick)
        iupKEY_SETDOUBLE(status);

      if (msg==WM_LBUTTONDOWN || msg==WM_LBUTTONDBLCLK || msg==WM_NCLBUTTONDOWN || msg==WM_NCLBUTTONDBLCLK)
      {
        button = IUP_BUTTON1;
        iupKEY_SETBUTTON1(status);  
      }
      else if (msg==WM_MBUTTONDOWN || msg==WM_MBUTTONDBLCLK || msg==WM_NCMBUTTONDOWN || msg==WM_NCMBUTTONDBLCLK)
      {
        button = IUP_BUTTON2;
        iupKEY_SETBUTTON2(status);  
      }
      else if (msg==WM_RBUTTONDOWN || msg==WM_RBUTTONDBLCLK || msg==WM_NCRBUTTONDOWN || msg==WM_NCRBUTTONDBLCLK)
      {
        button = IUP_BUTTON3;
        iupKEY_SETBUTTON3(status);  
      }
      else if (msg==WM_XBUTTONDOWN || msg==WM_XBUTTONDBLCLK || msg==WM_NCXBUTTONDOWN || msg==WM_NCXBUTTONDBLCLK)
      {
        if (HIWORD(wp) == XBUTTON1)
        {
          button = IUP_BUTTON4;
          iupKEY_SETBUTTON4(status);  
        }
        else
        {
          button = IUP_BUTTON5;
          iupKEY_SETBUTTON5(status);  
        }
      }

      if (last_button == button && last_pressed == 1)
        break;  /* do nothing because last state was already pressed */

      cb(button, 1, pt.x, pt.y, status);

      last_button = button;
      last_pressed = 1;
      break;
    }
  case WM_NCXBUTTONUP:
  case WM_NCLBUTTONUP:
  case WM_NCMBUTTONUP:
  case WM_NCRBUTTONUP:
  case WM_XBUTTONUP:
  case WM_LBUTTONUP:
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
    {
      int button=0;
      IFiiiis cb = (IFiiiis) IupGetFunction("GLOBALBUTTON_CB");
      if (!cb)
        break;

      if (msg>=WM_MOUSEFIRST && msg<=WM_MOUSELAST)
        iupwinButtonKeySetStatus(LOWORD(wp), status, 0);

      /* also updates the button status, since wp could not have the flag */
      if (msg==WM_LBUTTONUP || msg==WM_NCLBUTTONUP)
      {
        button = IUP_BUTTON1;
        iupKEY_SETBUTTON1(status);  
      }
      else if (msg==WM_MBUTTONUP || msg==WM_NCMBUTTONUP)
      {
        button = IUP_BUTTON2;
        iupKEY_SETBUTTON2(status);
      }
      else if (msg==WM_RBUTTONUP || msg==WM_NCRBUTTONUP)
      {
        button = IUP_BUTTON3;
        iupKEY_SETBUTTON3(status);
      }
      else if (msg==WM_XBUTTONUP || msg==WM_NCXBUTTONUP)
      {
        if (HIWORD(wp) == XBUTTON1)
        {
          button = IUP_BUTTON4;
          iupKEY_SETBUTTON4(status);
        }
        else
        {
          button = IUP_BUTTON5;
          iupKEY_SETBUTTON5(status);
        }
      }

      if (last_button == button && last_pressed == 0)
        break;  /* do nothing because last state was already released */

      cb(button, 0, pt.x, pt.y, status);

      last_button = button;
      last_pressed = 0;
      break;
    }
  case WM_NCMOUSEMOVE:
  case WM_MOUSEMOVE:
    {
      IFiis cb = (IFiis)IupGetFunction("GLOBALMOTION_CB");
      if (cb)
      {
        if (msg>=WM_MOUSEFIRST && msg<=WM_MOUSELAST)
          iupwinButtonKeySetStatus(LOWORD(wp), status, 0);

        cb(pt.x, pt.y, status);
      }
      break;
    }
  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
  case WM_KEYUP:
    {
      IFii cb = (IFii)IupGetFunction("GLOBALKEYPRESS_CB");
      if (cb)
      {
        int pressed = (msg==WM_KEYDOWN || msg==WM_SYSKEYDOWN)? 1: 0;
        int code = iupwinKeyDecode((int)wp);
        if (code != 0)
          cb(code, pressed);
      }
      break;
    }
  default:
    break;
  }

  return CallNextHookEx(win_OldGetMessageHook, hcode, gm_wp, gm_lp);
}

int iupdrvSetGlobal(const char *name, const char *value)
{
  if (iupStrEqual(name, "INPUTCALLBACKS"))
  {
    if (iupStrBoolean(value))
    {
      if (!win_OldGetMessageHook)
        win_OldGetMessageHook = SetWindowsHookEx(WH_GETMESSAGE, 
                                                 (HOOKPROC)winHookGetMessageProc,
                                                 NULL, GetCurrentThreadId());
    }
    else 
    {
      if (win_OldGetMessageHook)
      {
        UnhookWindowsHookEx(win_OldGetMessageHook);
        win_OldGetMessageHook = NULL;
      }
    }
    return 1;
  }
  if (iupStrEqual(name, "DLL_HINSTANCE"))
  {
    iupwin_dll_hinstance = (HINSTANCE)value;
    return 0;
  }
  if (iupStrEqual(name, "SINGLEINSTANCE"))
  {
    if (winGlobalSetMutex(value))
    {
      winGlobalFindInstance(value);
      return 0;  /* don't save the attribute, mutex already exist */
    }
    else
      return 1; /* save the attribute, this is the first instance */
  }
  return 1;
}

char *iupdrvGetGlobal(const char *name)
{
  if (iupStrEqual(name, "VIRTUALSCREEN"))
  {
    char *str = iupStrGetMemory(50);
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN); 
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN); 
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    sprintf(str, "%d %d %d %d", x, y, w, h);
    return str;
  }
  if (iupStrEqual(name, "MONITORSINFO"))
  {
    int i;
    int monitors_count = GetSystemMetrics(SM_CMONITORS);
    RECT* monitors_rect = malloc(monitors_count*sizeof(RECT));
    char *str = iupStrGetMemory(monitors_count*50);
    char* pstr = str;

    win_monitor_index = 0;
    EnumDisplayMonitors(NULL, NULL, winGlobalMonitorInfoEnum, (LPARAM)monitors_rect);

    for (i=0; i < monitors_count; i++)
      pstr += sprintf(pstr, "%d %d %d %d\n", (int)monitors_rect[i].left, (int)monitors_rect[i].top, (int)(monitors_rect[i].right-monitors_rect[i].left), (int)(monitors_rect[i].bottom-monitors_rect[i].top));

    free(monitors_rect);
    return str;
  }
  if (iupStrEqual(name, "TRUECOLORCANVAS"))
  {
    if (iupdrvGetScreenDepth() > 8)
      return "YES";
    return "NO";
  }
  if (iupStrEqual(name, "DLL_HINSTANCE"))
  {
    return (char*)iupwin_dll_hinstance;
  }
  if (iupStrEqual(name, "SYSTEMCODEPAGE"))
  {
    CPINFOEX info;
    char* str = iupStrGetMemory(20);
    GetCPInfoEx(CP_ACP, 0, &info);
    sprintf(str, "%d", info.CodePage);
    return str;
  }
  if (iupStrEqual(name, "LASTERROR"))
  {
    DWORD error = GetLastError();
    if (error)
    {
      LPVOID lpMsgBuf = NULL;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|
                    FORMAT_MESSAGE_FROM_SYSTEM|
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, error, 0, 
                    (LPTSTR)&lpMsgBuf, 0, NULL);
      if (lpMsgBuf)
      {
        char* str = iupStrGetMemoryCopy((const char*)lpMsgBuf);
        LocalFree(lpMsgBuf);
        return str;
      }
      else
        return "Unknown Error";
    }
  }
  return NULL;
}
