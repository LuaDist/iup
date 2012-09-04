/** \file
 * \brief IupColorDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>
#include <stdio.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"


#define IUP_COLOR_RED        706

static UINT_PTR winColorDlgHookProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  (void)wParam;
  if (uiMsg == WM_INITDIALOG)
  {
    HWND hWndItem;
    CHOOSECOLOR* choosecolor = (CHOOSECOLOR*)lParam;
    Ihandle* ih = (Ihandle*)choosecolor->lCustData;

    char* value = iupAttribGet(ih, "TITLE");
    if (value)
      SetWindowText(hWnd, value);

    ih->handle = hWnd;
    iupDialogUpdatePosition(ih);
    ih->handle = NULL;  /* reset handle */
    iupAttribSetStr(ih, "HWND", (char*)hWnd);  /* used by HELP_CB in winDialogBaseProc */

    hWndItem = GetDlgItem(hWnd, IUP_COLOR_RED);
    SetFocus(hWndItem);
  }
  return 0;
}

static char* winColorDlgColorsToString(COLORREF* lpCustColors)
{
  int i, inc, off = 0;
  char iup_str[20];
  char* str = iupStrGetMemory(300);
  for (i=0; i < 16; i++)
  {
    inc = sprintf(iup_str, "%d %d %d;", (int)GetRValue(*lpCustColors), (int)GetGValue(*lpCustColors), (int)GetBValue(*lpCustColors));
    memcpy(str+off, iup_str, inc);
    off += inc;
    lpCustColors++;
  }
  str[off-1] = 0; /* remove last separator */
  return str;
}

static void winColorDlgStringToColors(char* str, COLORREF* lpCustColors)
{
  int i = 0;
  unsigned char r, g, b;

  while (str && *str && i < 16)
  {
    if (iupStrToRGB(str, &r, &g, &b))
      *lpCustColors = RGB(r,g,b);

    str = strchr(str, ';');
    if (str) str++;
    i++;
    lpCustColors++;
  }
}

static int winColorDlgPopup(Ihandle* ih, int x, int y)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  CHOOSECOLOR choosecolor;
  unsigned char r, g, b;
  COLORREF lpCustColors[16];
  char* value;

  iupAttribSetInt(ih, "_IUPDLG_X", x);   /* used in iupDialogUpdatePosition */
  iupAttribSetInt(ih, "_IUPDLG_Y", y);

  if (!parent)
    parent = GetActiveWindow();  /* if NOT set will NOT be Modal */
                                 /* anyway it will be modal only relative to its parent */

  iupStrToRGB(iupAttribGet(ih, "VALUE"), &r, &g, &b);

  ZeroMemory(lpCustColors, 16*sizeof(COLORREF));

  value = iupAttribGetStr(ih, "COLORTABLE");
  if (value)
    winColorDlgStringToColors(value, lpCustColors);

  ZeroMemory(&choosecolor, sizeof(CHOOSECOLOR));
  choosecolor.lStructSize = sizeof(CHOOSECOLOR);
  choosecolor.hwndOwner = parent;
  choosecolor.rgbResult = RGB(r, g, b);
  choosecolor.lpCustColors = lpCustColors;
  choosecolor.lCustData = (LPARAM)ih;

  choosecolor.Flags = CC_RGBINIT|CC_FULLOPEN;
  if (IupGetCallback(ih, "HELP_CB"))
    choosecolor.Flags |= CC_SHOWHELP;

  choosecolor.Flags |= CC_ENABLEHOOK;
  choosecolor.lpfnHook = (LPCCHOOKPROC)winColorDlgHookProc;

  if (!ChooseColor(&choosecolor))
  {
    iupAttribSetStr(ih, "VALUE", NULL);
    iupAttribSetStr(ih, "COLORTABLE", NULL);
    iupAttribSetStr(ih, "STATUS", NULL);
    return IUP_NOERROR;
  }

  iupAttribSetStrf(ih, "VALUE", "%d %d %d", GetRValue(choosecolor.rgbResult),
                                            GetGValue(choosecolor.rgbResult),
                                            GetBValue(choosecolor.rgbResult));
  iupAttribSetStr(ih, "COLORTABLE", winColorDlgColorsToString(lpCustColors));
  iupAttribSetStr(ih, "STATUS", "1");

  return IUP_NOERROR;
}

void iupdrvColorDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = winColorDlgPopup;
}
