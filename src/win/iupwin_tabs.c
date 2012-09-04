/** \file
* \brief Tabs Control
*
* See Copyright Notice in "iup.h"
*/

#include <windows.h>
#include <commctrl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_tabs.h"
#include "iup_image.h"
#include "iup_array.h"

#include "iupwin_drv.h"
#include "iupwin_handle.h"
#include "iupwin_draw.h"
#include "iupwin_info.h"


int iupdrvTabsExtraDecor(Ihandle* ih)
{
  (void)ih;
  return 0;
}

int iupdrvTabsGetLineCountAttrib(Ihandle* ih)
{
  return (int)SendMessage(ih->handle, TCM_GETROWCOUNT, 0, 0);
}

static HWND winTabsGetPageWindow(Ihandle* ih, int pos)
{
  TCITEM tie;
  tie.mask = TCIF_PARAM;
  SendMessage(ih->handle, TCM_GETITEM, pos, (LPARAM)&tie);
  return (HWND)tie.lParam;
}

void iupdrvTabsSetCurrentTab(Ihandle* ih, int pos)
{
  int prev_pos = SendMessage(ih->handle, TCM_GETCURSEL, 0, 0);
  HWND tab_page = winTabsGetPageWindow(ih, prev_pos);
  ShowWindow(tab_page, SW_HIDE);

  SendMessage(ih->handle, TCM_SETCURSEL, pos, 0);

  tab_page = winTabsGetPageWindow(ih, pos);
  ShowWindow(tab_page, SW_SHOW);
}

int iupdrvTabsGetCurrentTab(Ihandle* ih)
{
  return (int)SendMessage(ih->handle, TCM_GETCURSEL, 0, 0);
}

static int winTabsGetImageIndex(Ihandle* ih, const char* name)
{
  HIMAGELIST image_list;
  int count, i, bpp, ret;
  Iarray* bmp_array;
  HBITMAP *bmp_array_data, hMask=NULL;
  HBITMAP bmp = iupImageGetImage(name, ih, 0);
  if (!bmp)
    return -1;

  /* the array is used to avoi adding the same bitmap twice */
  bmp_array = (Iarray*)iupAttribGet(ih, "_IUPWIN_BMPARRAY");
  if (!bmp_array)
  {
    /* create the array if does not exist */
    bmp_array = iupArrayCreate(50, sizeof(HBITMAP));
    iupAttribSetStr(ih, "_IUPWIN_BMPARRAY", (char*)bmp_array);
  }

  bmp_array_data = iupArrayGetData(bmp_array);

  image_list = (HIMAGELIST)SendMessage(ih->handle, TCM_GETIMAGELIST, 0, 0);
  if (!image_list)
  {
    int width, height;
    UINT flags = ILC_COLOR32|ILC_MASK;

    /* must use this info, since image can be a driver image loaded from resources */
    iupdrvImageGetInfo(bmp, &width, &height, &bpp);

    /* create the image list if does not exist */
    image_list = ImageList_Create(width, height, flags, 0, 50);
    SendMessage(ih->handle, TCM_SETIMAGELIST, 0, (LPARAM)image_list);
  }
  else
    iupdrvImageGetInfo(bmp, NULL, NULL, &bpp);

  /* check if that bitmap is already added to the list,
     but we can not compare with the actual bitmap at the list since it is a copy */
  count = ImageList_GetImageCount(image_list);
  for (i=0; i<count; i++)
  {
    if (bmp_array_data[i] == bmp)
      return i;
  }

  if (bpp == 8)
  {
    Ihandle* image = IupGetHandle(name);
    if (image)
    {
      iupAttribSetStr(image, "_IUPIMG_NO_INVERT", "1");
      hMask = iupdrvImageCreateMask(image);
      iupAttribSetStr(image, "_IUPIMG_NO_INVERT", NULL);
    }
  }

  bmp_array_data = iupArrayInc(bmp_array);
  bmp_array_data[i] = bmp;
  ret = ImageList_Add(image_list, bmp, hMask);  /* the bmp is duplicated at the list */
  DeleteObject(hMask);
  return ret;
}

static int winTabsGetPageWindowPos(Ihandle* ih, HWND tab_page)
{
  TCITEM tie;
  int pos, num_tabs;

  num_tabs = (int)SendMessage(ih->handle, TCM_GETITEMCOUNT, 0, 0);
  tie.mask = TCIF_PARAM;

  for (pos=0; pos<num_tabs; pos++)
  {
    SendMessage(ih->handle, TCM_GETITEM, pos, (LPARAM)&tie);
    if (tab_page == (HWND)tie.lParam)
      return pos;
  }

  return -1;
}

static void winTabsPlacePageWindows(Ihandle* ih, int w, int h)
{
  TCITEM tie;
  int pos, num_tabs;
  RECT rect; 

  num_tabs = (int)SendMessage(ih->handle, TCM_GETITEMCOUNT, 0, 0);
  tie.mask = TCIF_PARAM;

  /* Calculate the display rectangle, assuming the 
     tab control is the size of the client area. */
  SetRect(&rect, 0, 0, w, h); 
  SendMessage(ih->handle, TCM_ADJUSTRECT, FALSE, (LPARAM)&rect);

  for (pos=0; pos<num_tabs; pos++)
  {
    SendMessage(ih->handle, TCM_GETITEM, pos, (LPARAM)&tie);

    SetWindowPos((HWND)tie.lParam, NULL, 
                   rect.left, rect.top,  
                   rect.right - rect.left, rect.bottom - rect.top, 
                   SWP_NOACTIVATE|SWP_NOZORDER);
  }
}

static int winTabsUsingXPStyles(Ihandle* ih)
{
  return iupwin_comctl32ver6 && ih->data->type == ITABS_TOP;
}

static void winTabsDrawPageBackground(Ihandle* ih, HDC hDC, RECT* rect)
{
  unsigned char r=0, g=0, b=0;
  char* color = iupAttribGetInheritNativeParent(ih, "BGCOLOR");
  if (!color) color = iupAttribGetInheritNativeParent(ih, "BACKGROUND");
  if (!color) color = iupAttribGet(ih, "BACKGROUND");
  if (!color) color = IupGetGlobal("DLGBGCOLOR");
  iupStrToRGB(color, &r, &g, &b);
  SetDCBrushColor(hDC, RGB(r,g,b));
  FillRect(hDC, rect, (HBRUSH)GetStockObject(DC_BRUSH));
}

static LRESULT CALLBACK winTabsPageWinProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{   
  switch (msg)
  {
  case WM_ERASEBKGND:
    {
      RECT rect;
      HDC hDC = (HDC)wp;
      Ihandle* ih = iupwinHandleGet(hWnd);
      GetClientRect(ih->handle, &rect); 
      winTabsDrawPageBackground(ih, hDC, &rect);

      /* return non zero value */
      return 1;
    }
  case WM_COMMAND:
  case WM_CTLCOLORSCROLLBAR:
  case WM_CTLCOLORBTN:
  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORLISTBOX:
  case WM_CTLCOLORSTATIC:
  case WM_DRAWITEM:
  case WM_HSCROLL:
  case WM_NOTIFY:
  case WM_VSCROLL:
    /* Forward the container messages to its parent. */
    return SendMessage(GetParent(hWnd), msg, wp, lp);
  }

  return DefWindowProc(hWnd, msg, wp, lp);
}

static HWND winTabCreatePageWindow(Ihandle* ih) 
{ 
  HWND hWnd;
  DWORD dwStyle = WS_CHILD|WS_CLIPSIBLINGS, 
      dwExStyle = 0;

  iupwinGetNativeParentStyle(ih, &dwExStyle, &dwStyle);

  hWnd = CreateWindowEx(dwExStyle, "IupTabsPage", NULL, dwStyle, 
                        0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 
                        ih->handle, NULL, iupwin_hinstance, NULL); 

  iupwinHandleAdd(ih, hWnd);

  return hWnd;
} 

/* ------------------------------------------------------------------------- */
/* winTabs - Sets and Gets accessors                                         */
/* ------------------------------------------------------------------------- */

static int winTabsSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');

  if (ih->handle)
  {
    SendMessage(ih->handle, TCM_SETPADDING, 0, MAKELPARAM(ih->data->horiz_padding, ih->data->vert_padding));
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int winTabsSetMultilineAttrib(Ihandle* ih, const char* value)
{
  if (ih->handle) /* allow to set only before mapping */
    return 0;

  if (iupStrBoolean(value))
    ih->data->is_multiline = 1;
  else
  {
    if (ih->data->type == ITABS_BOTTOM || ih->data->type == ITABS_TOP)
      ih->data->is_multiline = 0;
    else
      ih->data->is_multiline = 1;   /* always true if left/right */
  }

  return 0;
}

static char* winTabsGetMultilineAttrib(Ihandle* ih)
{
  if (ih->data->is_multiline)
    return "YES";
  else
    return "NO";
}

static int winTabsSetTabTypeAttrib(Ihandle* ih, const char* value)
{
  if (ih->handle) /* allow to set only before mapping */
    return 0;

  if(iupStrEqualNoCase(value, "BOTTOM"))
  {
    ih->data->type = ITABS_BOTTOM;
    ih->data->orientation = ITABS_HORIZONTAL;  /* TABTYPE controls TABORIENTATION in Windows */
  }
  else if(iupStrEqualNoCase(value, "LEFT"))
  {
    ih->data->type = ITABS_LEFT;
    ih->data->orientation = ITABS_VERTICAL;  /* TABTYPE controls TABORIENTATION in Windows */
    ih->data->is_multiline = 1; /* VERTICAL works only with MULTILINE */
  }
  else if(iupStrEqualNoCase(value, "RIGHT"))
  {
    ih->data->type = ITABS_RIGHT;
    ih->data->orientation = ITABS_VERTICAL;  /* TABTYPE controls TABORIENTATION in Windows */
    ih->data->is_multiline = 1; /* VERTICAL works only with MULTILINE */
  }
  else /* "TOP" */
  {
    ih->data->type = ITABS_TOP;
    ih->data->orientation = ITABS_HORIZONTAL;  /* TABTYPE controls TABORIENTATION in Windows */
  }

  return 0;
}

static int winTabsSetTabTitleAttrib(Ihandle* ih, int pos, const char* value)
{
  if (value)
  {
    TCITEM tie;

    tie.mask = TCIF_TEXT;
    tie.pszText = (char*)value;
    tie.cchTextMax = strlen(value);

    SendMessage(ih->handle, TCM_SETITEM, pos, (LPARAM)&tie);
  }
  return 1;
}

static int winTabsSetTabImageAttrib(Ihandle* ih, int pos, const char* value)
{
  if (value)
  {
    TCITEM tie;

    tie.mask = TCIF_IMAGE;
    tie.iImage = winTabsGetImageIndex(ih, value);

    SendMessage(ih->handle, TCM_SETITEM, pos, (LPARAM)&tie);
  }
  return 1;
}

static char* winTabsGetBgColorAttrib(Ihandle* ih)
{
  /* the most important use of this is to provide
     the correct background for images */
  if (iupwin_comctl32ver6)
  {
    COLORREF cr;
    if (iupwinDrawGetThemeTabsBgColor(ih->handle, &cr))
    {
      char* str = iupStrGetMemory(20);
      sprintf(str, "%d %d %d", (int)GetRValue(cr), (int)GetGValue(cr), (int)GetBValue(cr));
      return str;
    }
  }

  return IupGetGlobal("DLGBGCOLOR");
}

static int winTabsSetBgColorAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  iupdrvPostRedraw(ih);
  return 1;
}


/* ------------------------------------------------------------------------- */
/* winTabs - Calls the user callback to change of tab                        */
/* ------------------------------------------------------------------------- */

static int winTabsCtlColor(Ihandle* ih, HDC hdc, LRESULT *result)
{
  /* works only when NOT winTabsUsingXPStyles */
  unsigned char r, g, b;
  char* color = iupBaseNativeParentGetBgColorAttrib(ih);
  if (iupStrToRGB(color, &r, &g, &b))
  {
    SetDCBrushColor(hdc, RGB(r,g,b));
    *result = (LRESULT)GetStockObject(DC_BRUSH);
    return 1;
  }
  return 0;
}

static int winTabsWmNotify(Ihandle* ih, NMHDR* msg_info, int *result)
{
  (void)result;

  if (msg_info->code == TCN_SELCHANGING)
  {
    IFnnn cb = (IFnnn)IupGetCallback(ih, "TABCHANGE_CB");
    int prev_pos = SendMessage(ih->handle, TCM_GETCURSEL, 0, 0);
    iupAttribSetInt(ih, "_IUPTABS_PREV_CHILD_POS", prev_pos);

    /* save the previous handle if callback exists */
    if (cb)
    {
      Ihandle* prev_child = IupGetChild(ih, prev_pos);
      iupAttribSetStr(ih, "_IUPTABS_PREV_CHILD", (char*)prev_child);
    }
  }

  if (msg_info->code == TCN_SELCHANGE)
  {
    IFnnn cb = (IFnnn)IupGetCallback(ih, "TABCHANGE_CB");
    int pos = SendMessage(ih->handle, TCM_GETCURSEL, 0, 0);
    int prev_pos = iupAttribGetInt(ih, "_IUPTABS_PREV_CHILD_POS");
    HWND tab_page = winTabsGetPageWindow(ih, pos);
    ShowWindow(tab_page, SW_SHOW);
    tab_page = winTabsGetPageWindow(ih, prev_pos);
    ShowWindow(tab_page, SW_HIDE);

    if (cb)
    {
      Ihandle* child = IupGetChild(ih, pos);
      Ihandle* prev_child = (Ihandle*)iupAttribGet(ih, "_IUPTABS_PREV_CHILD");
      iupAttribSetStr(ih, "_IUPTABS_PREV_CHILD", NULL);

      /* avoid duplicate calls when a Tab is inside another Tab. */
      if (prev_child)
        cb(ih, child, prev_child);
    }
    else
    {
      IFnii cb2 = (IFnii)IupGetCallback(ih, "TABCHANGEPOS_CB");
      if (cb2)
        cb2(ih, pos, prev_pos);
    }
  }

  return 0; /* result not used */
}

static int winTabsProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{
  switch(msg)
  {
  case WM_SIZE:
    {
      WNDPROC oldProc = (WNDPROC)IupGetCallback(ih, "_IUPWIN_OLDPROC_CB");
      CallWindowProc(oldProc, ih->handle, msg, wp, lp);

      winTabsPlacePageWindows(ih, LOWORD(lp), HIWORD(lp));

      *result = 0;
      return 1;
    }
  }

  return iupwinBaseContainerProc(ih, msg, wp, lp, result);
}

/* ------------------------------------------------------------------------- */
/* winTabs - Methods and Init Class                                          */
/* ------------------------------------------------------------------------- */

static void winTabsChildAddedMethod(Ihandle* ih, Ihandle* child)
{
  if (IupGetName(child) == NULL)
    iupAttribSetHandleName(child);

  if (ih->handle)
  {
    TCITEM tie;
    HWND tab_page;
    char *tabtitle, *tabimage;
    int pos, old_rowcount;
    RECT rect; 

    pos = IupGetChildPos(ih, child);

    tab_page = winTabCreatePageWindow(ih);

    if (pos == 0)
      ShowWindow(tab_page, SW_SHOW);

    tabtitle = iupTabsAttribGetStrId(ih, "TABTITLE", pos);
    if (!tabtitle) 
    {
      tabtitle = iupAttribGet(child, "TABTITLE");
      if (tabtitle)
        iupTabsAttribSetStrId(ih, "TABTITLE", pos, tabtitle);
    }
    tabimage = iupTabsAttribGetStrId(ih, "TABIMAGE", pos);
    if (!tabimage) 
    {
      tabimage = iupAttribGet(child, "TABIMAGE");
      if (tabimage)
        iupTabsAttribSetStrId(ih, "TABIMAGE", pos, tabimage);
    }
    if (!tabtitle && !tabimage)
      tabtitle = "     ";

    old_rowcount = (int)SendMessage(ih->handle, TCM_GETROWCOUNT, 0, 0);

    tie.mask = TCIF_PARAM;

    if (tabtitle)
    {
      tie.mask |= TCIF_TEXT;
      tie.pszText = tabtitle;
      tie.cchTextMax = strlen(tabtitle);
    }

    if (tabimage)
    {
      tie.mask |= TCIF_IMAGE;
      tie.iImage = winTabsGetImageIndex(ih, tabimage);
    }

    /* create tabs and label them */
    tie.lParam = (LPARAM)tab_page;
    SendMessage(ih->handle, TCM_INSERTITEM, pos, (LPARAM)&tie);

    /* Calculate the display rectangle, assuming the 
       tab control is the size of the client area. */
    GetClientRect(ih->handle, &rect);
    SendMessage(ih->handle, TCM_ADJUSTRECT, FALSE, (LPARAM)&rect);

    SetWindowPos(tab_page, NULL, 
                 rect.left, rect.top,  
                 rect.right - rect.left, rect.bottom - rect.top, 
                 SWP_NOACTIVATE|SWP_NOZORDER);

    iupAttribSetStr(child, "_IUPTAB_CONTAINER", (char*)tab_page);

    if (ih->data->is_multiline)
    {
      if (ih->data->type == ITABS_LEFT || ih->data->type == ITABS_RIGHT)
      {
        int rowcount = (int)SendMessage(ih->handle, TCM_GETROWCOUNT, 0, 0);
        if (rowcount != old_rowcount)
        {
          GetClientRect(ih->handle, &rect);
          winTabsPlacePageWindows(ih, rect.right - rect.left, rect.bottom - rect.top);
        }
      }

      iupdrvRedrawNow(ih);
    }
  }
}

static void winTabsChildRemovedMethod(Ihandle* ih, Ihandle* child)
{
  if (ih->handle)
  {
    HWND tab_page = (HWND)iupAttribGet(child, "_IUPTAB_CONTAINER");
    if (tab_page)
    {
      int pos = winTabsGetPageWindowPos(ih, tab_page);
      iupTabsTestRemoveTab(ih, pos);

      SendMessage(ih->handle, TCM_DELETEITEM, pos, 0);
      iupwinHandleRemove(tab_page);
      DestroyWindow(tab_page);

      iupAttribSetStr(child, "_IUPTAB_CONTAINER", NULL);
    }
  }
}

static int winTabsMapMethod(Ihandle* ih)
{
  DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | TCS_HOTTRACK | WS_TABSTOP,
      dwExStyle = 0;

  if (!ih->parent)
    return IUP_ERROR;

  if (ih->data->type == ITABS_BOTTOM)
    dwStyle |= TCS_BOTTOM;
  else if (ih->data->type == ITABS_RIGHT)
    dwStyle |= TCS_VERTICAL|TCS_RIGHT;  
  else if (ih->data->type == ITABS_LEFT)
    dwStyle |= TCS_VERTICAL;

  if (ih->data->is_multiline)
    dwStyle |= TCS_MULTILINE;

  iupwinGetNativeParentStyle(ih, &dwExStyle, &dwStyle);

  if (dwExStyle & WS_EX_COMPOSITED && !ih->data->is_multiline && iupwinIsVistaOrNew())
  {
    /* workaround for composite bug in Vista */
    ih->data->is_multiline = 1;  
    dwStyle |= TCS_MULTILINE;
  }

  if (!iupwinCreateWindowEx(ih, WC_TABCONTROL, dwExStyle, dwStyle))
    return IUP_ERROR;

  /* replace the WinProc to handle other messages */
  IupSetCallback(ih, "_IUPWIN_CTRLPROC_CB", (Icallback)winTabsProc);

  /* Process WM_NOTIFY */
  IupSetCallback(ih, "_IUPWIN_NOTIFY_CB", (Icallback)winTabsWmNotify);

  /* Process background color */
  IupSetCallback(ih, "_IUPWIN_CTLCOLOR_CB", (Icallback)winTabsCtlColor);

  if (iupwin_comctl32ver6 && ih->data->type != ITABS_TOP)
  {
    /* XP Styles support only TABTYPE=TOP */ 
    iupwinDrawRemoveTheme(ih->handle);
  }

  /* Change children background */
  if (winTabsUsingXPStyles(ih))
  {
    char* color = iupAttribGetInheritNativeParent(ih, "BGCOLOR");
    if (!color) 
      color = iupAttribGetInheritNativeParent(ih, "BACKGROUND");
    if (!color)
    {
      COLORREF cr;
      if (iupwinDrawGetThemeTabsBgColor(ih->handle, &cr))
        iupAttribSetStrf(ih, "BACKGROUND", "%d %d %d", (int)GetRValue(cr), (int)GetGValue(cr), (int)GetBValue(cr));
    }
  }

  /* Create pages and tabs */
  if (ih->firstchild)
  {
    Ihandle* child;
    for (child = ih->firstchild; child; child = child->brother)
      winTabsChildAddedMethod(ih, child);
  }

  return IUP_NOERROR;
}

static void winTabsUnMapMethod(Ihandle* ih)
{
  Iarray* bmp_array;

  HIMAGELIST image_list = (HIMAGELIST)SendMessage(ih->handle, TCM_GETIMAGELIST, 0, 0);
  if (image_list)
    ImageList_Destroy(image_list);

  bmp_array = (Iarray*)iupAttribGet(ih, "_IUPWIN_BMPARRAY");
  if (bmp_array)
    iupArrayDestroy(bmp_array);

  iupdrvBaseUnMapMethod(ih);
}

static void winTabsRegisterClass(void)
{
  WNDCLASS wndclass;
  ZeroMemory(&wndclass, sizeof(WNDCLASS));
  
  wndclass.hInstance      = iupwin_hinstance;
  wndclass.lpszClassName  = "IupTabsPage";
  wndclass.lpfnWndProc    = (WNDPROC)winTabsPageWinProc;
  wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wndclass.style          = CS_PARENTDC;
  wndclass.hbrBackground  = NULL;  /* remove the background to optimize redraw */
   
  RegisterClass(&wndclass);
}

void iupdrvTabsInitClass(Iclass* ic)
{
  if (!iupwinClassExist("IupTabsPage"))
    winTabsRegisterClass();

  /* Driver Dependent Class functions */
  ic->Map = winTabsMapMethod;
  ic->UnMap = winTabsUnMapMethod;
  ic->ChildAdded     = winTabsChildAddedMethod;
  ic->ChildRemoved   = winTabsChildRemovedMethod;

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", winTabsGetBgColorAttrib, winTabsSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_NO_SAVE|IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_NOT_MAPPED);

  /* IupTabs only */
  iupClassRegisterAttribute(ic, "TABTYPE", iupTabsGetTabTypeAttrib, winTabsSetTabTypeAttrib, IUPAF_SAMEASSYSTEM, "TOP", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABORIENTATION", iupTabsGetTabOrientationAttrib, NULL, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);  /* can not be set, depends on TABTYPE in Windows */
  iupClassRegisterAttribute(ic, "MULTILINE", winTabsGetMultilineAttrib, winTabsSetMultilineAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABTITLE", NULL, winTabsSetTabTitleAttrib, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABIMAGE", NULL, winTabsSetTabImageAttrib, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iupTabsGetPaddingAttrib, winTabsSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* necessary because transparent background does not work when not using visual styles */
  if (!iupwin_comctl32ver6)  /* Used by iupdrvImageCreateImage */
    iupClassRegisterAttribute(ic, "FLAT_ALPHA", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
}
