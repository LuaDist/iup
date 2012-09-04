/** \file
 * \brief Menu Resources
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_dialog.h"
#include "iup_str.h"
#include "iup_image.h"
#include "iup_dlglist.h"
#include "iup_focus.h"
#include "iup_menu.h"

#include "iupwin_drv.h"
#include "iupwin_handle.h"
#include "iupwin_brush.h"


Ihandle* iupwinMenuGetHandle(HMENU hMenu)
{
  MENUINFO menuinfo;
  menuinfo.cbSize = sizeof(MENUINFO);
  menuinfo.fMask = MIM_MENUDATA;
  if (GetMenuInfo(hMenu, &menuinfo))
    return (Ihandle*)menuinfo.dwMenuData;
  else
    return NULL;
}

Ihandle* iupwinMenuGetItemHandle(HMENU hMenu, int menuId)
{
  MENUITEMINFO menuiteminfo;
  menuiteminfo.cbSize = sizeof(MENUITEMINFO);
  menuiteminfo.fMask = MIIM_DATA;

  if (GetMenuItemInfo(hMenu, menuId, FALSE, &menuiteminfo))
    return (Ihandle*)menuiteminfo.dwItemData;
  else
    return NULL;
}

int iupdrvMenuGetMenuBarSize(Ihandle* ih)
{
  (void)ih;
  return GetSystemMetrics(SM_CYMENU);
}

static void winMenuUpdateBar(Ihandle* ih)
{
  if (iupMenuIsMenuBar(ih) && ih->parent->handle)
    DrawMenuBar(ih->parent->handle);
  else if (ih->parent)
  {
    ih = ih->parent; /* check also for children of a menu bar */
    if (iupMenuIsMenuBar(ih) && ih->parent->handle)
      DrawMenuBar(ih->parent->handle);
  }
}

static void winMenuGetLastPos(Ihandle* ih, int *last_pos, int *pos)
{
  Ihandle* child;
  *last_pos=0;
  *pos=0;
  for (child=ih->parent->firstchild; child; child=child->brother)
  {
    if (child == ih)
      *pos = *last_pos;
    (*last_pos)++;
  }
}

static void winItemCheckToggle(Ihandle* ih)
{
  if (iupAttribGetBoolean(ih->parent, "RADIO"))
  {
    int last_pos, pos;
    winMenuGetLastPos(ih, &last_pos, &pos);
    CheckMenuRadioItem((HMENU)ih->handle, 0, last_pos, pos, MF_BYPOSITION);

    winMenuUpdateBar(ih);
  }
  else if (iupAttribGetBoolean(ih, "AUTOTOGGLE"))
  {
    if (GetMenuState((HMENU)ih->handle, (UINT)ih->serial, MF_BYCOMMAND) & MF_CHECKED)
      CheckMenuItem((HMENU)ih->handle, (UINT)ih->serial, MF_UNCHECKED|MF_BYCOMMAND);
    else
      CheckMenuItem((HMENU)ih->handle, (UINT)ih->serial, MF_CHECKED|MF_BYCOMMAND);

    winMenuUpdateBar(ih);
  }
}

void iupwinMenuDialogProc(Ihandle* ih_dialog, UINT msg, WPARAM wp, LPARAM lp)
{
  /* called only from winDialogBaseProc */

  switch (msg)
  {
  case WM_INITMENUPOPUP:
    {
      HMENU hMenu = (HMENU)wp;
      Ihandle *ih = iupwinMenuGetHandle(hMenu);
      if (ih)
      {
        Icallback cb = (Icallback)IupGetCallback(ih, "OPEN_CB");
        if (!cb && ih->parent) cb = (Icallback)IupGetCallback(ih->parent, "OPEN_CB");  /* check also in the Submenu */
        if (cb) cb(ih);
      }
      break;
    }
  case WM_UNINITMENUPOPUP:
    {
      HMENU hMenu = (HMENU)wp;
      Ihandle *ih = iupwinMenuGetHandle(hMenu);
      if (ih)
      {
        Icallback cb = (Icallback)IupGetCallback(ih, "MENUCLOSE_CB");
        if (!cb && ih->parent) cb = (Icallback)IupGetCallback(ih->parent, "MENUCLOSE_CB");  /* check also in the Submenu */
        if (cb) cb(ih);
      }
      break;
    }
  case WM_MENUSELECT:
    {
      HMENU hMenu = (HMENU)lp;
      Ihandle *ih;

      if (!lp)
        break;

      if ((HIWORD(wp) & MF_POPUP) || (HIWORD(wp) & MF_SYSMENU)) /* drop-down ih or submenu. */
      {
        UINT menuindex = LOWORD(wp);
        HMENU hsubmenu = GetSubMenu(hMenu, menuindex);
        ih = iupwinMenuGetHandle(hsubmenu);  /* returns the handle of a IupMenu */
        if (ih) ih = ih->parent;  /* gets the submenu */
      }
      else /* ih item */
      {
        UINT menuID = LOWORD(wp);
        ih = iupwinMenuGetItemHandle(hMenu, menuID);
      }

      if (ih)
      {
        Icallback cb = IupGetCallback(ih, "HIGHLIGHT_CB");
        if (cb) cb(ih);
      }
      break;
    }
  case WM_MENUCOMMAND:
    {
      int menuId = GetMenuItemID((HMENU)lp, (int)wp);
      Icallback cb;
      Ihandle* ih;
        
      if (menuId >= IUP_MDI_FIRSTCHILD)
        break;
        
      ih  = iupwinMenuGetItemHandle((HMENU)lp, menuId);
      if (!ih)
        break;

      winItemCheckToggle(ih);

      cb = IupGetCallback(ih, "ACTION");
      if (cb && cb(ih) == IUP_CLOSE)
        IupExitLoop();

      break;
    }
  case WM_ENTERMENULOOP:
    {
      /* Simulate WM_KILLFOCUS when the menu interaction is started */
      Ihandle* lastfocus = (Ihandle*)iupAttribGet(ih_dialog, "_IUPWIN_LASTFOCUS");
      if (lastfocus) iupCallKillFocusCb(lastfocus);
      break;
    }
  case WM_EXITMENULOOP:
    {
      /* Simulate WM_GETFOCUS when the menu interaction is stopped */
      Ihandle* lastfocus = (Ihandle*)iupAttribGet(ih_dialog, "_IUPWIN_LASTFOCUS");
      if (lastfocus) iupCallGetFocusCb(lastfocus);
      break;
    }
  }
}

int iupdrvMenuPopup(Ihandle* ih, int x, int y)
{
  HWND hWndActive = GetActiveWindow();
  int tray_menu = 0;
  int menuId;

  if (!hWndActive)
  {
    /* search for a valid handle */
    Ihandle* dlg = iupDlgListFirst();
    do 
    {
      if (dlg->handle)
      {
        hWndActive = dlg->handle;  /* found a valid handle */

        /* if not a "TRAY" dialog, keep searching, because TRAY is a special case */
        if (iupAttribGetBoolean(dlg, "TRAY")) 
          break;
      }
      dlg = iupDlgListNext();
    } while (dlg);
  }

  /* Necessary to avoid tray dialogs to lock popup menus (they get sticky after the 1st time) */
  if (hWndActive)
  {
    Ihandle* dlg = iupwinHandleGet(hWndActive);
    if (dlg && iupAttribGetBoolean(dlg, "TRAY"))
    {
      /* To display a context menu for a notification icon, 
         the current window must be the foreground window. */
      SetForegroundWindow(hWndActive);
      tray_menu = 1;
    }
  }

  /* stop processing here. messages will not go to the message loop */
  menuId = TrackPopupMenu((HMENU)ih->handle, TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, x, y, 0, hWndActive, NULL);

  if (tray_menu)
  {
    /* You must force a task switch to the application that 
       called TrackPopupMenu at some time in the near future. 
       This is done by posting a benign message to the window. */
    PostMessage(hWndActive, WM_NULL, 0, 0);
  }

  if (menuId)
  {
    Icallback cb;
    Ihandle* ih_item = iupwinMenuGetItemHandle((HMENU)ih->handle, menuId);
    if (!ih_item) return IUP_NOERROR;

    winItemCheckToggle(ih_item);

    cb = IupGetCallback(ih_item, "ACTION");
    if (cb && cb(ih_item) == IUP_CLOSE)
      IupExitLoop();
  }

  return IUP_NOERROR;
}


/*******************************************************************************************/


static int winMenuSetBgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  /* must use IupGetAttribute to use inheritance */
  if (iupStrToRGB(value, &r, &g, &b))
  {
    MENUINFO menuinfo;
    menuinfo.cbSize = sizeof(MENUINFO);
    menuinfo.fMask = MIM_BACKGROUND;
    menuinfo.hbrBack = iupwinBrushGet(RGB(r,g,b)); 
    SetMenuInfo((HMENU)ih->handle, &menuinfo);
    winMenuUpdateBar(ih);
  }
  return 1;
}

static int winMenuAddParentSubmenu(Ihandle* ih)
{
  int pos;
  MENUITEMINFO menuiteminfo;

  pos = IupGetChildPos(ih->parent, ih);
  ih->serial = iupMenuGetChildId(ih);

  menuiteminfo.cbSize = sizeof(MENUITEMINFO); 
  menuiteminfo.fMask = MIIM_ID|MIIM_DATA|MIIM_SUBMENU|MIIM_STRING; 
  menuiteminfo.dwTypeData = ""; /* must set or it will be not possible to update */
  menuiteminfo.cch = 0;
  menuiteminfo.wID = (UINT)ih->serial;
  menuiteminfo.dwItemData = (ULONG_PTR)ih; 
  menuiteminfo.hSubMenu = (HMENU)ih->firstchild->handle;

  if (!InsertMenuItem((HMENU)ih->parent->handle, pos, TRUE, &menuiteminfo))
    return IUP_ERROR;

  ih->handle = ih->parent->handle; /* gets the HMENU of the parent */

  /* must update attributes since submenu is actually created after it is mapped */
  iupAttribUpdate(ih); 
  iupAttribUpdateFromParent(ih);

  winMenuUpdateBar(ih);

  return IUP_NOERROR;
}

static void winMenuChildUnMapMethod(Ihandle* ih)
{
  RemoveMenu((HMENU)ih->handle, (UINT)ih->serial, MF_BYCOMMAND);
}

static void winMenuUnMapMethod(Ihandle* ih)
{
  if (iupMenuIsMenuBar(ih))
  {
    SetMenu(ih->parent->handle, NULL);
    ih->parent = NULL;
  }

  if (!iupMenuIsMenuBar(ih) && ih->parent)
  {
    /* parent is a submenu, it is destroyed here */
    RemoveMenu((HMENU)ih->parent->handle, (UINT)ih->parent->serial, MF_BYCOMMAND);
  }

  DestroyMenu((HMENU)ih->handle);   /* DestroyMenu is recursive */
}

static int winMenuMapMethod(Ihandle* ih)
{
  MENUINFO menuinfo;

  if (iupMenuIsMenuBar(ih))
  {
    /* top level menu used for MENU attribute in IupDialog (a menu bar) */

    ih->handle = (InativeHandle*)CreateMenu();
    if (!ih->handle)
      return IUP_ERROR;

    SetMenu(ih->parent->handle, (HMENU)ih->handle);
  }
  else
  {
    if (ih->parent)
    {
      /* parent is a submenu, it is created here */

      ih->handle = (InativeHandle*)CreatePopupMenu();
      if (!ih->handle)
        return IUP_ERROR;

      if (winMenuAddParentSubmenu(ih->parent) == IUP_ERROR)
      {
        DestroyMenu((HMENU)ih->handle);
        return IUP_ERROR;
      }
    }
    else  
    {
      /* top level menu used for IupPopup */

      ih->handle = (InativeHandle*)CreatePopupMenu();
      if (!ih->handle)
        return IUP_ERROR;

      iupAttribSetStr(ih, "_IUPWIN_POPUP_MENU", "1");
    }
  }

  menuinfo.cbSize = sizeof(MENUINFO);
  menuinfo.fMask = MIM_MENUDATA;
  menuinfo.dwMenuData = (ULONG_PTR)ih;

  if (!iupAttribGetInherit(ih, "_IUPWIN_POPUP_MENU"))   /* check in the top level menu using inheritance */
  {
    menuinfo.fMask |= MIM_STYLE;
    menuinfo.dwStyle = MNS_NOTIFYBYPOS;
  }

  SetMenuInfo((HMENU)ih->handle, &menuinfo);

  winMenuUpdateBar(ih);

  return IUP_NOERROR;
}

void iupdrvMenuInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = winMenuMapMethod;
  ic->UnMap = winMenuUnMapMethod;

  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, winMenuSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "MENUBGCOLOR", IUPAF_DEFAULT);
}


/*******************************************************************************************/


static int winItemSetImageAttrib(Ihandle* ih, const char* value)
{
  HBITMAP hBitmapUnchecked, hBitmapChecked;
  char* impress;

  if (ih->handle == (InativeHandle*)-1) /* check if submenu is actually created */
    return 1;

  hBitmapUnchecked = iupImageGetImage(value, ih, 0);

  impress = iupAttribGet(ih, "IMPRESS");
  if (impress)
    hBitmapChecked = iupImageGetImage(impress, ih, 0);
  else
    hBitmapChecked = hBitmapUnchecked;

  SetMenuItemBitmaps((HMENU)ih->handle, (UINT)ih->serial, MF_BYCOMMAND, hBitmapUnchecked, hBitmapChecked);

  winMenuUpdateBar(ih);

  return 1;
}

static int winItemSetImpressAttrib(Ihandle* ih, const char* value)
{
  HBITMAP hBitmapUnchecked, hBitmapChecked;

  char *image = iupAttribGet(ih, "IMPRESS");
  hBitmapUnchecked = iupImageGetImage(image, ih, 0);

  if (value)
    hBitmapChecked = iupImageGetImage(value, ih, 0);
  else
    hBitmapChecked = hBitmapUnchecked;

  SetMenuItemBitmaps((HMENU)ih->handle, (UINT)ih->serial, MF_BYCOMMAND, hBitmapUnchecked, hBitmapChecked);

  winMenuUpdateBar(ih);

  return 1;
}

static int winItemSetTitleAttrib(Ihandle* ih, const char* value)
{
  char *str;
  MENUITEMINFO menuiteminfo;

  if (ih->handle == (InativeHandle*)-1) /* check if submenu is actually created */
    return 1;

  if (!value)
  {
    str = "     ";
    value = str;
  }
  else
    str = iupMenuProcessTitle(ih, value);

  menuiteminfo.cbSize = sizeof(MENUITEMINFO); 
  menuiteminfo.fMask = MIIM_TYPE;
  menuiteminfo.fType = MFT_STRING;
  menuiteminfo.dwTypeData = str;
  menuiteminfo.cch = strlen(str);

  SetMenuItemInfo((HMENU)ih->handle, (UINT)ih->serial, FALSE, &menuiteminfo);

  if (str != value) free(str);

  winMenuUpdateBar(ih);

  return 1;
}

static int winItemSetTitleImageAttrib(Ihandle* ih, const char* value)
{
  HBITMAP hBitmap;
  MENUITEMINFO menuiteminfo;

  if (ih->handle == (InativeHandle*)-1) /* check if submenu is actually created */
    return 1;

  hBitmap = iupImageGetImage(value, ih, 0);

  menuiteminfo.cbSize = sizeof(MENUITEMINFO); 
  menuiteminfo.fMask = MIIM_BITMAP; 
  menuiteminfo.hbmpItem = hBitmap;

  SetMenuItemInfo((HMENU)ih->handle, (UINT)ih->serial, FALSE, &menuiteminfo);

  winMenuUpdateBar(ih);

  return 1;
}

static int winItemSetActiveAttrib(Ihandle* ih, const char* value)
{
  if (ih->handle == (InativeHandle*)-1) /* check if submenu is actually created */
    return 1;

  if (iupStrBoolean(value))
    EnableMenuItem((HMENU)ih->handle, (UINT)ih->serial, MF_ENABLED|MF_BYCOMMAND);
  else
    EnableMenuItem((HMENU)ih->handle, (UINT)ih->serial, MF_GRAYED|MF_BYCOMMAND);

  winMenuUpdateBar(ih);

  return 0;
}

static char* winItemGetActiveAttrib(Ihandle* ih)
{
  if (ih->handle == (InativeHandle*)-1) /* check if submenu is actually created */
    return NULL;

  if (GetMenuState((HMENU)ih->handle, (UINT)ih->serial, MF_BYCOMMAND) & MF_GRAYED)
    return "NO";
  else
    return "YES";
}

static int winItemSetValueAttrib(Ihandle* ih, const char* value)
{
  if (iupAttribGetBoolean(ih->parent, "RADIO"))
  {
    int last_pos, pos;
    winMenuGetLastPos(ih, &last_pos, &pos);
    CheckMenuRadioItem((HMENU)ih->handle, 0, last_pos, pos, MF_BYPOSITION);
  }
  else
  {
    if (iupStrBoolean(value))
      CheckMenuItem((HMENU)ih->handle, (UINT)ih->serial, MF_CHECKED|MF_BYCOMMAND);
    else
      CheckMenuItem((HMENU)ih->handle, (UINT)ih->serial, MF_UNCHECKED|MF_BYCOMMAND);
  }

  winMenuUpdateBar(ih);

  return 0;
}

static char* winItemGetValueAttrib(Ihandle* ih)
{
  if (GetMenuState((HMENU)ih->handle, (UINT)ih->serial, MF_BYCOMMAND) & MF_CHECKED)
    return "ON";
  else
    return "OFF";
}

static int winItemMapMethod(Ihandle* ih)
{
  int pos;
  MENUITEMINFO menuiteminfo;

  if (!ih->parent || !IsMenu((HMENU)ih->parent->handle))
    return IUP_ERROR;

  pos = IupGetChildPos(ih->parent, ih);
  ih->serial = iupMenuGetChildId(ih);

  menuiteminfo.cbSize = sizeof(MENUITEMINFO); 
  menuiteminfo.fMask = MIIM_ID|MIIM_DATA|MIIM_STRING; 
  menuiteminfo.dwTypeData = ""; /* must set or it will be not possible to update */
  menuiteminfo.cch = 0;
  menuiteminfo.wID = (UINT)ih->serial;
  menuiteminfo.dwItemData = (ULONG_PTR)ih; 

  if (!InsertMenuItem((HMENU)ih->parent->handle, pos, TRUE, &menuiteminfo))
    return IUP_ERROR;

  ih->handle = ih->parent->handle; /* gets the HMENU of the parent */
  winMenuUpdateBar(ih);

  return IUP_NOERROR;
}

void iupdrvItemInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = winItemMapMethod;
  ic->UnMap = winMenuChildUnMapMethod;

  /* Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", winItemGetActiveAttrib, winItemSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "MENUBGCOLOR", IUPAF_DEFAULT);  /* used by IupImage */

  /* IupItem only */
  iupClassRegisterAttribute(ic, "VALUE", winItemGetValueAttrib, winItemSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, winItemSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEIMAGE", NULL, winItemSetTitleImageAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, winItemSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMPRESS", NULL, winItemSetImpressAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* Used by iupdrvImageCreateImage */
  /* necessary because it uses an old HBITMAP solution */
  iupClassRegisterAttribute(ic, "FLAT_ALPHA", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
}


/*******************************************************************************************/


static int winSubmenuMapMethod(Ihandle* ih)
{
  if (!ih->parent || !IsMenu((HMENU)ih->parent->handle))
    return IUP_ERROR;

  return iupBaseTypeVoidMapMethod(ih);
}

void iupdrvSubmenuInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = winSubmenuMapMethod;

  /* IupSubmenu only */
  iupClassRegisterAttribute(ic, "ACTIVE", winItemGetActiveAttrib, winItemSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, winItemSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, winItemSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "MENUBGCOLOR", IUPAF_DEFAULT);  /* used by IupImage */

  /* Used by iupdrvImageCreateImage */
  /* necessary because it uses an old HBITMAP solution */
  iupClassRegisterAttribute(ic, "FLAT_ALPHA", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
}


/*******************************************************************************************/


static int winSeparatorMapMethod(Ihandle* ih)
{
  int pos;
  MENUITEMINFO menuiteminfo;

  if (!ih->parent || !IsMenu((HMENU)ih->parent->handle))
    return IUP_ERROR;

  pos = IupGetChildPos(ih->parent, ih);
  ih->serial = iupMenuGetChildId(ih);

  menuiteminfo.cbSize = sizeof(MENUITEMINFO); 
  menuiteminfo.fMask = MIIM_FTYPE|MIIM_ID|MIIM_DATA; 
  menuiteminfo.fType = MFT_SEPARATOR; 
  menuiteminfo.wID = (UINT)ih->serial;
  menuiteminfo.dwItemData = (ULONG_PTR)ih; 

  if (!InsertMenuItem((HMENU)ih->parent->handle, pos, TRUE, &menuiteminfo))
    return IUP_ERROR;

  ih->handle = ih->parent->handle; /* gets the HMENU of the parent */
  winMenuUpdateBar(ih);

  return IUP_NOERROR;
}

void iupdrvSeparatorInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = winSeparatorMapMethod;
  ic->UnMap = winMenuChildUnMapMethod;
}
