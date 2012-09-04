/** \file
 * \brief List Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>
#include <commctrl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_mask.h"
#include "iup_focus.h"
#include "iup_image.h"
#include "iup_list.h"

#include "iupwin_drv.h"
#include "iupwin_handle.h"
#include "iupwin_draw.h"


/* Not defined in Cygwin and MingW */
#ifndef EM_SETCUEBANNER      
#define ECM_FIRST               0x1500      /* Edit control messages */
#define	EM_SETCUEBANNER	    (ECM_FIRST + 1)
#endif

#define WM_IUPCARET WM_APP+1   /* Custom IUP message */

#define WIN_GETCOUNT(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_GETCOUNT: LB_GETCOUNT)
#define WIN_GETTEXTLEN(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_GETLBTEXTLEN: LB_GETTEXTLEN)
#define WIN_GETTEXT(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_GETLBTEXT: LB_GETTEXT)
#define WIN_ADDSTRING(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_ADDSTRING: LB_ADDSTRING)
#define WIN_DELETESTRING(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_DELETESTRING: LB_DELETESTRING)
#define WIN_INSERTSTRING(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_INSERTSTRING: LB_INSERTSTRING)
#define WIN_RESETCONTENT(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_RESETCONTENT: LB_RESETCONTENT)
#define WIN_SETCURSEL(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_SETCURSEL: LB_SETCURSEL)
#define WIN_GETCURSEL(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_GETCURSEL: LB_GETCURSEL)
#define WIN_SETHORIZONTALEXTENT(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_SETHORIZONTALEXTENT: LB_SETHORIZONTALEXTENT)
#define WIN_GETHORIZONTALEXTENT(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_GETHORIZONTALEXTENT: LB_GETHORIZONTALEXTENT)
#define WIN_SETITEMDATA(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_SETITEMDATA: LB_SETITEMDATA)
#define WIN_GETITEMDATA(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_GETITEMDATA: LB_GETITEMDATA)
#define WIN_SETTOPINDEX(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_SETTOPINDEX: LB_SETTOPINDEX)
#define WIN_SETITEMHEIGHT(_ih) ((_ih->data->is_dropdown || _ih->data->has_editbox)? CB_SETITEMHEIGHT: LB_SETITEMHEIGHT)


typedef struct _winListItemData
{
  int text_width;
  HBITMAP hBitmap;
} winListItemData;

static winListItemData* winListGetItemData(Ihandle* ih, int pos)
{
  LRESULT ret = SendMessage(ih->handle, WIN_GETITEMDATA(ih), pos, 0);
  if (ret == CB_ERR)
    return NULL;
  else
    return (winListItemData*)ret;
}

static int winListRemoveItemData(Ihandle* ih, int pos)
{
  winListItemData* itemdata = winListGetItemData(ih, pos);
  if (itemdata)
  {
    int text_width = itemdata->text_width;
    free(itemdata);
    SendMessage(ih->handle, WIN_SETITEMDATA(ih), pos, (LPARAM)NULL);
    return text_width;
  }
  return 0;
}

static void winListUpdateScrollWidthItem(Ihandle* ih, int item_width, int add);

static void winListSetItemData(Ihandle* ih, int pos, const char* str, HBITMAP hBitmap)
{
  winListItemData* itemdata = winListGetItemData(ih, pos);
  if (!itemdata)
  {
    itemdata = malloc(sizeof(winListItemData));
    SendMessage(ih->handle, WIN_SETITEMDATA(ih), pos, (LPARAM)itemdata);
  }

  itemdata->hBitmap = hBitmap;
  itemdata->text_width = iupdrvFontGetStringWidth(ih, str);
  winListUpdateScrollWidthItem(ih, itemdata->text_width, 1);
}

void iupdrvListAddItemSpace(Ihandle* ih, int *h)
{
  (void)ih;
  (void)h;
}

void iupdrvListAddBorders(Ihandle* ih, int *x, int *y)
{
  int border_size = 2*4;
  (*x) += border_size;
  (*y) += border_size;

  if (ih->data->is_dropdown)
  {
    (*x) += 3; /* extra space for the dropdown button */

    /* IMPORTANT: In Windows the DROPDOWN box is always sized by the system
       to have the height just right to include the borders and the text.
       So the user height from RASTERSIZE or SIZE will be always ignored. */
  }
  else
  {
    if (ih->data->has_editbox)
      (*y) += 2*3; /* internal border between editbox and list */
  }
}

int iupdrvListGetCount(Ihandle* ih)
{
  return SendMessage(ih->handle, WIN_GETCOUNT(ih), 0, 0);
}

static int winListConvertXYToPos(Ihandle* ih, int x, int y)
{
  int pos;

  if (ih->data->has_editbox)
  {
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");

    pos = SendMessage(cbedit, EM_CHARFROMPOS, 0, MAKELPARAM(x, y));
    pos = LOWORD(pos);
  }

  if (ih->data->has_editbox)
  {
    HWND cblist = (HWND)iupAttribGet(ih, "_IUPWIN_LISTBOX");
    pos = SendMessage(cblist, LB_ITEMFROMPOINT, 0, MAKELPARAM(x, y))+1;  /* IUP Starts at 1 */
    pos = LOWORD(pos);
  }
  else
  {
    pos = SendMessage(ih->handle, LB_ITEMFROMPOINT, 0, MAKELPARAM(x, y))+1;
    pos = LOWORD(pos);
  }

  return pos;
}

static int winListGetMaxWidth(Ihandle* ih)
{
  int i, item_w, max_w = 0,
    count = SendMessage(ih->handle, WIN_GETCOUNT(ih), 0, 0);

  for (i=0; i<count; i++)
  { 
    winListItemData* itemdata = winListGetItemData(ih, i);
    item_w = itemdata->text_width;
    if (item_w > max_w)
      max_w = item_w;
  }

  return max_w;
}

static void winListUpdateScrollWidth(Ihandle* ih)
{
  if (ih->data->is_dropdown && iupAttribGetBoolean(ih, "DROPEXPAND"))
  {
    int w = 3+winListGetMaxWidth(ih)+iupdrvGetScrollbarSize()+3;
    SendMessage(ih->handle, CB_SETDROPPEDWIDTH, w, 0);
  }
  else
    SendMessage(ih->handle, WIN_SETHORIZONTALEXTENT(ih), winListGetMaxWidth(ih), 0);
}

static void winListUpdateScrollWidthItem(Ihandle* ih, int item_width, int add)
{
  int max_w;

  if (ih->data->is_dropdown && iupAttribGetBoolean(ih, "DROPEXPAND"))
  {
    max_w = (int)SendMessage(ih->handle, CB_GETDROPPEDWIDTH, 0, 0);
    max_w = max_w -3 -3 -iupdrvGetScrollbarSize();
  }
  else
    max_w = (int)SendMessage(ih->handle, WIN_GETHORIZONTALEXTENT(ih), 0, 0);

  if (add)
  {
    if (item_width > max_w)
    {
      if (ih->data->is_dropdown && iupAttribGetBoolean(ih, "DROPEXPAND"))
      {
        int w = 3+item_width+iupdrvGetScrollbarSize()+3;
        SendMessage(ih->handle, CB_SETDROPPEDWIDTH, w, 0);
      }
      else
        SendMessage(ih->handle, WIN_SETHORIZONTALEXTENT(ih), item_width, 0);
    }
  }
  else
  {
    if (item_width >= max_w)
      winListUpdateScrollWidth(ih);
  }
}

void iupdrvListAppendItem(Ihandle* ih, const char* value)
{
  int pos = SendMessage(ih->handle, WIN_ADDSTRING(ih), 0, (LPARAM)value);
  winListSetItemData(ih, pos, value, NULL);
}

void iupdrvListInsertItem(Ihandle* ih, int pos, const char* value)
{
  SendMessage(ih->handle, WIN_INSERTSTRING(ih), pos, (LPARAM)value);
  winListSetItemData(ih, pos, value, NULL);
  iupListUpdateOldValue(ih, pos, 0);
}

void iupdrvListRemoveItem(Ihandle* ih, int pos)
{
  int text_width;

  if (ih->data->is_dropdown && !ih->data->has_editbox)
  {
    /* must check if removing the current item */
    int curpos = SendMessage(ih->handle, WIN_GETCURSEL(ih), 0, 0);
    if (pos == curpos)
    {
      if (curpos > 0) 
        curpos--;
      else 
      {
        curpos=1;
        if (iupdrvListGetCount(ih)==1)
          curpos = -1; /* remove the selection */
      }

      SendMessage(ih->handle, WIN_SETCURSEL(ih), curpos, 0);
    }
  }

  text_width = winListRemoveItemData(ih, pos);
  SendMessage(ih->handle, WIN_DELETESTRING(ih), pos, 0L);
  winListUpdateScrollWidthItem(ih, text_width, 0);

  iupListUpdateOldValue(ih, pos, 1);
}

static void winListRemoveAllItemData(Ihandle* ih)
{
  int pos, count = iupdrvListGetCount(ih);
  for (pos = 0; pos < count; pos++)
    winListRemoveItemData(ih, pos);
}

void iupdrvListRemoveAllItems(Ihandle* ih)
{
  winListRemoveAllItemData(ih);
  SendMessage(ih->handle, WIN_RESETCONTENT(ih), 0, 0L);
  if (ih->data->is_dropdown && iupAttribGetBoolean(ih, "DROPEXPAND"))
    SendMessage(ih->handle, CB_SETDROPPEDWIDTH, 0, 0);
  else
    SendMessage(ih->handle, WIN_SETHORIZONTALEXTENT(ih), 0, 0);
}

static int winListGetCaretPos(HWND cbedit)
{
  int pos = 0;
  POINT point;

  if (GetFocus() != cbedit || !GetCaretPos(&point))
  {
    /* if does not have the focus, or could not get caret position,
       then use the selection start position */
    SendMessage(cbedit, EM_GETSEL, (WPARAM)&pos, 0);
  }
  else
  {
    pos = SendMessage(cbedit, EM_CHARFROMPOS, 0, MAKELPARAM(point.x, point.y));
    pos = LOWORD(pos);
  }

  return pos;
}

static char* winListGetText(Ihandle* ih, int pos)
{
  int len = SendMessage(ih->handle, WIN_GETTEXTLEN(ih), (WPARAM)pos, 0);
  char* str = calloc(len+1, 1);
  SendMessage(ih->handle, WIN_GETTEXT(ih), (WPARAM)pos, (LPARAM)str);
  return str;
}

/*********************************************************************************/


static void winListUpdateItemWidth(Ihandle* ih)
{
  int i, count = SendMessage(ih->handle, WIN_GETCOUNT(ih), 0, 0);
  for (i=0; i<count; i++)
  { 
    winListItemData* itemdata = winListGetItemData(ih, i);
    char* str = winListGetText(ih, i);
    itemdata->text_width = iupdrvFontGetStringWidth(ih, str);
    free(str);
  }
}

static int winListSetBgColorAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  if (ih->handle)
    iupdrvPostRedraw(ih);
  return 1;
}

static int winListSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetStandardFontAttrib(ih, value);
  if (ih->handle)
  {
    winListUpdateItemWidth(ih);
    winListUpdateScrollWidth(ih);
  }
  return 1;
}

static char* winListGetIdValueAttrib(Ihandle* ih, int id)
{
  int pos = iupListGetPos(ih, id);
  if (pos >= 0)
  {
    char* str = winListGetText(ih, pos);
    char* value = iupStrGetMemoryCopy(str);
    free(str);
    return value;
  }
  return NULL;
}

static char* winListGetValueAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    int nc = GetWindowTextLength(ih->handle);
    if (nc)
    {
      char* str = iupStrGetMemory(nc+1);
      GetWindowText(ih->handle, str, nc+1);
      return str;
    }
  }
  else 
  {
    if (ih->data->is_dropdown || !ih->data->is_multiple)
    {
      int pos = SendMessage(ih->handle, WIN_GETCURSEL(ih), 0, 0);
      char* str = iupStrGetMemory(50);
      sprintf(str, "%d", pos+1);  /* IUP starts at 1 */
      return str;
    }
    else
    {
      int i, count = SendMessage(ih->handle, LB_GETCOUNT, 0, 0);
      int* pos = malloc(sizeof(int)*count);
      int sel_count = SendMessage(ih->handle, LB_GETSELITEMS, count, (LPARAM)pos);
      char* str = iupStrGetMemory(count+1);
      memset(str, '-', count);
      str[count]=0;
      for (i=0; i<sel_count; i++)
        str[pos[i]] = '+';
      str[count]=0;
      return str;
    }
  }

  return NULL;
}

static int winListSetValueAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->has_editbox)
  {
    if (!value) value = "";
    SetWindowText(ih->handle, value);
  }
  else 
  {
    if (ih->data->is_dropdown || !ih->data->is_multiple)
    {
      int pos;
      if (iupStrToInt(value, &pos)==1)
      {
        SendMessage(ih->handle, WIN_SETCURSEL(ih), pos-1, 0);   /* IUP starts at 1 */
        iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);
      }
      else
      {
        SendMessage(ih->handle, WIN_SETCURSEL(ih), (WPARAM)-1, 0);   /* no selection */
        iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
      }
    }
    else
    {
      /* User has changed a multiple selection on a simple list. */
	    int i, len, count;

      /* Clear all selections */
      SendMessage(ih->handle, LB_SETSEL, FALSE, -1);

      if (!value)
      {
        iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
        return 0;
      }

      count = SendMessage(ih->handle, LB_GETCOUNT, 0, 0L);
      len = strlen(value);
      if (len < count) 
        count = len;

      /* update selection list */
      for (i = 0; i<count; i++)
      {
        if (value[i]=='+')
          SendMessage(ih->handle, LB_SETSEL, TRUE, i);
      }

      iupAttribStoreStr(ih, "_IUPLIST_OLDVALUE", value);
    }
  }

  return 0;
}

static int winListSetShowDropdownAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->is_dropdown)
    SendMessage(ih->handle, CB_SHOWDROPDOWN, iupStrBoolean(value), 0); 
  return 0;
}

static int winListSetTopItemAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->is_dropdown)
  {
    int pos = 1;
    if (iupStrToInt(value, &pos))
      SendMessage(ih->handle, WIN_SETTOPINDEX(ih), pos-1, 0);  /* IUP starts at 1 */
  }
  return 0;
}

static int winListSetSpacingAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->is_dropdown)
    return 0;

  if (!iupStrToInt(value, &ih->data->spacing))
    ih->data->spacing = 0;

  if (ih->handle)
  {
    int txt_h;
    iupdrvFontGetCharSize(ih, NULL, &txt_h);
    txt_h += 2*ih->data->spacing;

    /* set for all items */
    if (!ih->data->show_image)
      SendMessage(ih->handle, WIN_SETITEMHEIGHT(ih), 0, txt_h);
    else
    {
      /* must manually set for each item */
      int img_h, i, count = SendMessage(ih->handle, WIN_GETCOUNT(ih), 0, 0);

      for (i=0; i<count; i++)
      { 
        winListItemData* itemdata = winListGetItemData(ih, i);
        if (itemdata->hBitmap)
        {
          iupdrvImageGetInfo(itemdata->hBitmap, NULL, &img_h, NULL);
          img_h += 2*ih->data->spacing;

          if (img_h > txt_h)
            SendMessage(ih->handle, WIN_SETITEMHEIGHT(ih), i, img_h);
          else
            SendMessage(ih->handle, WIN_SETITEMHEIGHT(ih), i, txt_h);
        }
        else
          SendMessage(ih->handle, WIN_SETITEMHEIGHT(ih), i, txt_h);
      }
    }
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int winListSetPaddingAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  iupStrToIntInt(value, &(ih->data->horiz_padding), &(ih->data->vert_padding), 'x');
  ih->data->vert_padding = 0;
  if (ih->handle)
  {
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELPARAM(ih->data->horiz_padding, ih->data->horiz_padding));
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int winListSetFilterAttrib(Ihandle *ih, const char *value)
{
  int style = 0;

  if (!ih->data->has_editbox)
    return 0;

  if (iupStrEqualNoCase(value, "LOWERCASE"))
    style = ES_LOWERCASE;
  else if (iupStrEqualNoCase(value, "NUMBER"))
    style = ES_NUMBER;
  else if (iupStrEqualNoCase(value, "UPPERCASE"))
    style = ES_UPPERCASE;

  if (style)
  {
    HWND old_handle = ih->handle;
    ih->handle = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    iupwinMergeStyle(ih, ES_LOWERCASE|ES_NUMBER|ES_UPPERCASE, style);
    ih->handle = old_handle;
  }

  return 1;
}

static int winListSetCueBannerAttrib(Ihandle *ih, const char *value)
{
  if (ih->data->has_editbox && iupwin_comctl32ver6)
  {
    WCHAR* wstr = iupwinStrChar2Wide(value);
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_SETCUEBANNER, (WPARAM)FALSE, (LPARAM)wstr);
    free(wstr);
    return 1;
  }
  return 0;
}

static int winListSetClipboardAttrib(Ihandle *ih, const char *value)
{
  UINT msg = 0;

  if (!ih->data->has_editbox)
    return 0;

  if (iupStrEqualNoCase(value, "COPY"))
    msg = WM_COPY;
  else if (iupStrEqualNoCase(value, "CUT"))
    msg = WM_CUT;
  else if (iupStrEqualNoCase(value, "PASTE"))
    msg = WM_PASTE;
  else if (iupStrEqualNoCase(value, "CLEAR"))
    msg = WM_CLEAR;
  else if (iupStrEqualNoCase(value, "UNDO"))
    msg = WM_UNDO;

  if (msg)
  {
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, msg, 0, 0);
  }

  return 0;
}

static int winListSetSelectedTextAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  if (value)
  {
    int start = 0, end = 0;
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
    if (start == end)
      return 0;
    SendMessage(cbedit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)value);
  }
  return 0;
}

static char* winListGetSelectedTextAttrib(Ihandle* ih)
{
  int nc;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return 0;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  nc = GetWindowTextLength(cbedit);
  if (nc)
  {
    int start = 0, end = 0;
    char* str;
    
    SendMessage(cbedit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
    if (start == end)
      return NULL;

    str = iupStrGetMemory(nc+1);
    GetWindowText(cbedit, str, nc+1);
    str[end] = 0; /* returns only the selected text */
    str += start;

    return str;
  }
  else
    return NULL;
}

static int winListSetNCAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  if (!iupStrToInt(value, &ih->data->nc))
    ih->data->nc = 0;

  if (ih->handle)
  {
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_LIMITTEXT, ih->data->nc, 0L);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int winListSetSelectionAttrib(Ihandle* ih, const char* value)
{
  HWND cbedit;
  int start=1, end=1;
  if (!ih->data->has_editbox)
    return 0;

  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_SETSEL, (WPARAM)-1, (LPARAM)0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
    return 0;
  }

  if (iupStrToIntInt(value, &start, &end, ':')!=2) 
    return 0;

  if(start<1 || end<1) 
    return 0;

  start--; /* IUP starts at 1 */
  end--;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_SETSEL, (WPARAM)start, (LPARAM)end);

  return 0;
}

static char* winListGetSelectionAttrib(Ihandle* ih)
{
  int start = 0, end = 0;
  char* str;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return NULL;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
  if (start == end)
    return NULL;

  str = iupStrGetMemory(100);

  start++; /* IUP starts at 1 */
  end++;
  sprintf(str, "%d:%d", start, end);

  return str;
}

static int winListSetSelectionPosAttrib(Ihandle* ih, const char* value)
{
  int start=0, end=0;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_SETSEL, (WPARAM)-1, (LPARAM)0);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
    return 0;
  }

  if (iupStrToIntInt(value, &start, &end, ':')!=2) 
    return 0;

  if(start<0 || end<0) 
    return 0;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_SETSEL, (WPARAM)start, (LPARAM)end);

  return 0;
}

static char* winListGetSelectionPosAttrib(Ihandle* ih)
{
  int start = 0, end = 0;
  char* str;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return NULL;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
  if (start == end)
    return NULL;

  str = iupStrGetMemory(100);
  sprintf(str, "%d:%d", start, end);
  return str;
}

static int winListSetInsertAttrib(Ihandle* ih, const char* value)
{
  if (value && ih->data->has_editbox)
  {
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    SendMessage(cbedit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)value);
  }
  return 0;
}

static int winListSetAppendAttrib(Ihandle* ih, const char* value)
{
  int len;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value) value = "";
  
  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  len = GetWindowTextLength(cbedit)+1;
  SendMessage(cbedit, EM_SETSEL, (WPARAM)len, (LPARAM)len);
  SendMessage(cbedit, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)value);

  return 0;
}

static int winListSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
  HWND cbedit;
  if (!ih->data->has_editbox)
    return 0;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_SETREADONLY, (WPARAM)iupStrBoolean(value), 0);
  return 0;
}

static char* winListGetReadOnlyAttrib(Ihandle* ih)
{
  DWORD style;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return NULL;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  style = GetWindowLong(cbedit, GWL_STYLE);
  if (style & ES_READONLY)
    return "YES";
  else
    return "NO";
}

static int winListSetCaretAttrib(Ihandle* ih, const char* value)
{
  int pos = 1;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 1) pos = 1;
  pos--; /* IUP starts at 1 */

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_SETSEL, (WPARAM)pos, (LPARAM)pos);
  SendMessage(cbedit, EM_SCROLLCARET, 0L, 0L);

  return 0;
}

static char* winListGetCaretAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    char* str = iupStrGetMemory(100);
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    sprintf(str, "%d", winListGetCaretPos(cbedit)+1);
    return str;
  }
  else
    return NULL;
}

static int winListSetCaretPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);    /* be permissive in SetCaret, do not abort if invalid */
  if (pos < 0) pos = 0;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_SETSEL, (WPARAM)pos, (LPARAM)pos);
  SendMessage(cbedit, EM_SCROLLCARET, 0L, 0L);

  return 0;
}

static char* winListGetCaretPosAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    char* str = iupStrGetMemory(100);
    HWND cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
    sprintf(str, "%d", winListGetCaretPos(cbedit));
    return str;
  }
  else
    return NULL;
}

static int winListSetScrollToAttrib(Ihandle* ih, const char* value)
{
  int pos = 1;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 1) pos = 1;

  pos--;  /* return to Windows reference */

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_LINESCROLL, (WPARAM)pos, (LPARAM)0);

  return 0;
}

static int winListSetScrollToPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;
  HWND cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 0) pos = 0;

  cbedit = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  SendMessage(cbedit, EM_LINESCROLL, (WPARAM)pos, (LPARAM)0);

  return 0;
}

static int winListSetImageAttrib(Ihandle* ih, int id, const char* value)
{
  winListItemData* itemdata;
  HBITMAP hBitmap = iupImageGetImage(value, ih, 0);
  int pos = iupListGetPos(ih, id);

  if (!ih->data->show_image || pos < 0)
    return 0;

  itemdata = winListGetItemData(ih, pos);
  itemdata->hBitmap = hBitmap;

  if (itemdata->hBitmap)
  {
    int txt_h, img_w, img_h;
    iupdrvFontGetCharSize(ih, NULL, &txt_h);
    iupdrvImageGetInfo(itemdata->hBitmap, &img_w, &img_h, NULL);

    if (img_h > txt_h)
    {
      if (ih->data->is_dropdown && !ih->data->has_editbox && img_h >= ih->data->maximg_h)
        SendMessage(ih->handle, WIN_SETITEMHEIGHT(ih), (WPARAM)-1, img_h);  /* set also for the selection box */

      if (!ih->data->is_dropdown)
        img_h += 2*ih->data->spacing;
      SendMessage(ih->handle, WIN_SETITEMHEIGHT(ih), pos, img_h);
    }

    if (img_w > ih->data->maximg_w)
      ih->data->maximg_w = img_w;
    if (img_h > ih->data->maximg_h)
      ih->data->maximg_h = img_h;
  }

  return 1;
}

/*********************************************************************************/


static int winListCtlColor(Ihandle* ih, HDC hdc, LRESULT *result)
{
  COLORREF cr;

  if (iupwinGetColorRef(ih, "FGCOLOR", &cr))
    SetTextColor(hdc, cr);

  if (iupwinGetColorRef(ih, "BGCOLOR", &cr))
  {
    SetBkColor(hdc, cr);
    SetDCBrushColor(hdc, cr);
    *result = (LRESULT)GetStockObject(DC_BRUSH);
    return 1;
  }
  return 0;
}

static int winListWmCommand(Ihandle* ih, WPARAM wp, LPARAM lp)
{
  (void)lp;

  if (ih->data->is_dropdown || ih->data->has_editbox)
  {
    switch (HIWORD(wp))
    {
    case CBN_EDITCHANGE:
      {
        iupBaseCallValueChangedCb(ih);
        break;
      }
    case CBN_SETFOCUS:
      iupwinWmSetFocus(ih);
      break;
    case CBN_KILLFOCUS:
      iupCallKillFocusCb(ih);
      break;
    case CBN_CLOSEUP:
    case CBN_DROPDOWN:
      {
        IFni cb = (IFni)IupGetCallback(ih, "DROPDOWN_CB");
        if (cb)
          cb(ih, HIWORD(wp)==CBN_DROPDOWN? 1: 0);
        break;
      }
    case CBN_DBLCLK:
      {
        IFnis cb = (IFnis) IupGetCallback(ih, "DBLCLICK_CB");
        if (cb)
        {
          int pos = SendMessage(ih->handle, CB_GETCURSEL, 0, 0);
          pos++;  /* IUP starts at 1 */
          iupListSingleCallDblClickCallback(ih, cb, pos);
        }
        break;
      }
    case CBN_SELCHANGE:
      {
        IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
        if (cb)
        {
          int pos = SendMessage(ih->handle, CB_GETCURSEL, 0, 0);
          pos++;  /* IUP starts at 1 */
          iupListSingleCallActionCallback(ih, cb, pos);
        }

        iupBaseCallValueChangedCb(ih);
        break;
      }
    }
  }
  else
  {
    switch (HIWORD(wp))
    {
    case LBN_DBLCLK:
      {
        IFnis cb = (IFnis) IupGetCallback(ih, "DBLCLICK_CB");
        if (cb)
        {
          int pos = SendMessage(ih->handle, LB_GETCURSEL, 0, 0);
          pos++;  /* IUP starts at 1 */
          iupListSingleCallDblClickCallback(ih, cb, pos);
        }
        break;
      }
    case LBN_SELCHANGE:
      {
        if (!ih->data->is_multiple)
        {
          IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
          if (cb)
          {
            int pos = SendMessage(ih->handle, LB_GETCURSEL, 0, 0);
            pos++;  /* IUP starts at 1 */
            iupListSingleCallActionCallback(ih, cb, pos);
          }
        }
        else
        {
          IFns multi_cb = (IFns)IupGetCallback(ih, "MULTISELECT_CB");
          IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
          if (multi_cb || cb)
          {
            int sel_count = SendMessage(ih->handle, LB_GETSELCOUNT, 0, 0);
            int* pos = malloc(sizeof(int)*sel_count);
            SendMessage(ih->handle, LB_GETSELITEMS, sel_count, (LPARAM)pos);
            iupListMultipleCallActionCallback(ih, cb, multi_cb, pos, sel_count);
            free(pos);
          }
        }

        iupBaseCallValueChangedCb(ih);
        break;
      }
    }
  }

  return 0; /* not used */
}

static void winListCallCaretCb(Ihandle* ih, HWND cbedit)
{
  int pos;

  IFniii cb = (IFniii)IupGetCallback(ih, "CARET_CB");
  if (!cb) return;

  pos = winListGetCaretPos(cbedit);

  if (pos != ih->data->last_caret_pos)
  {
    ih->data->last_caret_pos = pos;
    cb(ih, 1, pos+1, pos);
  }
}

static int winListCallEditCb(Ihandle* ih, HWND cbedit, const char* insert_value, int key, int dir)
{
  int start, end, ret = 1;
  char *value, *new_value;

  IFnis cb = (IFnis)IupGetCallback(ih, "EDIT_CB");
  if (!cb && !ih->data->mask)
    return 1;

  SendMessage(cbedit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

  value = winListGetValueAttrib(ih);

  if (!value)
    new_value = iupStrDup(insert_value);
  else if (insert_value)
    new_value = iupStrInsert(value, insert_value, start, end);
  else
  {
    new_value = value;
    iupStrRemove(value, start, end, dir);
  }

  if (!new_value)
    return 0; /* abort */

  if (ih->data->nc && (int)strlen(new_value) > ih->data->nc)
  {
    if (new_value != value) free(new_value);
    return 0; /* abort */
  }

  if (ih->data->mask && iupMaskCheck(ih->data->mask, new_value)==0)
  {
    if (new_value != value) free(new_value);
    return 0; /* abort */
  }

  if (cb)
  {
    int cb_ret = cb(ih, key, (char*)new_value);
    if (cb_ret==IUP_IGNORE)
      ret = 0;     /* abort processing */
    else if (cb_ret==IUP_CLOSE)
    {
      IupExitLoop();
      ret = 0;     /* abort processing */
    }
    else if (cb_ret!=0 && key!=0 && 
             cb_ret != IUP_DEFAULT && cb_ret != IUP_CONTINUE)  
    {
      WNDPROC oldProc = (WNDPROC)IupGetCallback(ih, "_IUPWIN_EDITOLDPROC_CB");
      CallWindowProc(oldProc, cbedit, WM_CHAR, cb_ret, 0);  /* replace key */
      ret = 0;     /* abort processing */
    }
  }

  if (new_value != value) free(new_value);
  return ret;
}

static int winListEditProc(Ihandle* ih, HWND cbedit, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{
  int ret = 0;

  if (msg==WM_KEYDOWN) /* process K_ANY before text callbacks */
  {
    ret = iupwinBaseProc(ih, msg, wp, lp, result);

    if (ret) 
    {
      iupAttribSetStr(ih, "_IUPWIN_IGNORE_CHAR", "1");
      *result = 0;
      return 1;
    }
    else
      iupAttribSetStr(ih, "_IUPWIN_IGNORE_CHAR", NULL);
  }

  switch (msg)
  {
  case WM_CHAR:
    {
      if (iupAttribGet(ih, "_IUPWIN_IGNORE_CHAR"))
      {
        iupAttribSetStr(ih, "_IUPWIN_IGNORE_CHAR", NULL);
        *result = 0;
        return 1;
      }

      if ((char)wp == '\b')
      {              
        if (!winListCallEditCb(ih, cbedit, NULL, 0, -1))
          ret = 1;
      }
      else if ((char)wp == '\n' || (char)wp == '\r')
      {
        ret = 1;
      }
      else if (!(GetKeyState(VK_CONTROL) & 0x8000 ||
                 GetKeyState(VK_MENU) & 0x8000 ||
                 GetKeyState(VK_LWIN) & 0x8000 || 
                 GetKeyState(VK_RWIN) & 0x8000))
      {
        char insert_value[2];
        insert_value[0] = (char)wp;
        insert_value[1] = 0;

        if (!winListCallEditCb(ih, cbedit, insert_value, wp, 1))
          ret = 1;
      }

      PostMessage(cbedit, WM_IUPCARET, 0, 0L);

      if (wp==VK_TAB)  /* the keys have the same definitions as the chars */
        ret = 1;  /* abort default processing to avoid beep */

      break;
    }
  case WM_KEYDOWN:
    {
      if (wp == VK_DELETE) /* Del does not generates a WM_CHAR */
      {
        if (!winListCallEditCb(ih, cbedit, NULL, 0, 1))
          ret = 1;
      }
      else if (wp == 'A' && GetKeyState(VK_CONTROL) & 0x8000)   /* Ctrl+A = Select All */
      {
        SendMessage(cbedit, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
      }

      PostMessage(cbedit, WM_IUPCARET, 0, 0L);

      break;
    }
  case WM_CLEAR:
    {
      if (!winListCallEditCb(ih, cbedit, NULL, 0, 1))
        ret = 1;

      PostMessage(cbedit, WM_IUPCARET, 0, 0L);

      break;
    }
  case WM_CUT:
    {
      if (!winListCallEditCb(ih, cbedit, NULL, 0, 1))
        ret = 1;

      PostMessage(cbedit, WM_IUPCARET, 0, 0L);

      break;
    }
  case WM_PASTE:
    {
      if (IupGetCallback(ih, "EDIT_CB") || ih->data->mask) /* test before to avoid alocate clipboard text memory */
      {
        char* insert_value = iupwinGetClipboardText(ih);
        if (insert_value)
        {
          if (!winListCallEditCb(ih, cbedit, insert_value, 0, 1))
            ret = 1;
          free(insert_value);
        }
      }

      PostMessage(cbedit, WM_IUPCARET, 0, 0L);

      break;
    }
  case WM_UNDO:
    {
      IFnis cb = (IFnis)IupGetCallback(ih, "EDIT_CB");
      if (cb)
      {
        char* value;
        WNDPROC oldProc = (WNDPROC)IupGetCallback(ih, "_IUPWIN_EDITOLDPROC_CB");
        CallWindowProc(oldProc, cbedit, WM_UNDO, 0, 0);

        value = winListGetValueAttrib(ih);
        cb(ih, 0, (char*)value);

        ret = 1;
      }

      PostMessage(cbedit, WM_IUPCARET, 0, 0L);

      break;
    }
  case WM_KEYUP:
  case WM_LBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
  case WM_LBUTTONUP:
    PostMessage(cbedit, WM_IUPCARET, 0, 0L);
    break;
  case WM_IUPCARET:
    winListCallCaretCb(ih, cbedit);
    break;
  }

  if (ret)       /* if abort processing, then the result is 0 */
  {
    *result = 0;
    return 1;
  }
  else
  {
    if (msg==WM_KEYDOWN)
      return 0;
    else
      return iupwinBaseProc(ih, msg, wp, lp, result);
  }
}

static LRESULT CALLBACK winListEditWinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{   
  int ret = 0;
  LRESULT result = 0;
  WNDPROC oldProc;
  Ihandle *ih;

  ih = iupwinHandleGet(hwnd); 
  if (!ih)
    return DefWindowProc(hwnd, msg, wp, lp);  /* should never happen */

  /* retrieve the control previous procedure for subclassing */
  oldProc = (WNDPROC)IupGetCallback(ih, "_IUPWIN_EDITOLDPROC_CB");

  ret = winListEditProc(ih, hwnd, msg, wp, lp, &result);

  if (ret)
    return result;
  else
    return CallWindowProc(oldProc, hwnd, msg, wp, lp);
}

static int winListComboListProc(Ihandle* ih, HWND cblist, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{
  (void)cblist;

  switch (msg)
  {
  case WM_LBUTTONDBLCLK:
  case WM_MBUTTONDBLCLK:
  case WM_RBUTTONDBLCLK:
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
    if (iupwinButtonDown(ih, msg, wp, lp)==-1)
    {
      *result = 0;
      return 1;
    }
    break;
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
  case WM_LBUTTONUP:
    if (iupwinButtonUp(ih, msg, wp, lp)==-1)
    {
      *result = 0;
      return 1;
    }
    break;
  case WM_MOUSEMOVE:
    iupwinMouseMove(ih, msg, wp, lp);
    iupwinBaseProc(ih, msg, wp, lp, result); /* to process ENTER/LEAVE */
    break;
  case WM_MOUSELEAVE:
    iupwinBaseProc(ih, msg, wp, lp, result); /* to process ENTER/LEAVE */
    break;
  }

  return 0;
}

static LRESULT CALLBACK winListComboListWinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{   
  int ret = 0;
  LRESULT result = 0;
  WNDPROC oldProc;
  Ihandle *ih;

  ih = iupwinHandleGet(hwnd); 
  if (!ih)
    return DefWindowProc(hwnd, msg, wp, lp);  /* should never happen */

  /* retrieve the control previous procedure for subclassing */
  oldProc = (WNDPROC)IupGetCallback(ih, "_IUPWIN_LISTOLDPROC_CB");

  ret = winListComboListProc(ih, hwnd, msg, wp, lp, &result);

  if (ret)
    return result;
  else
    return CallWindowProc(oldProc, hwnd, msg, wp, lp);
}

static int winListProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result)
{
  if (!ih->data->is_dropdown && !ih->data->has_editbox)
  {
    switch (msg)
    {
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      if (iupwinButtonDown(ih, msg, wp, lp)==-1)
      {
        *result = 0;
        return 1;
      }
      break;
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONUP:
      if (iupwinButtonUp(ih, msg, wp, lp)==-1)
      {
        *result = 0;
        return 1;
      }
      break;
    case WM_MOUSEMOVE:
      iupwinMouseMove(ih, msg, wp, lp);
      break;
    }
  }

  switch (msg)
  {
  case WM_CHAR:
    if (GetKeyState(VK_CONTROL) & 0x8000)
    {
      /* avoid item search when Ctrl is pressed */
      *result = 0;
      return 1;
    }
  case WM_SETFOCUS:
  case WM_KILLFOCUS:
  case WM_MOUSELEAVE:
  case WM_MOUSEMOVE:
    if (ih->data->has_editbox)
      return 0;  /* do not call base procedure to avoid duplicate messages */
    break;
  }

  return iupwinBaseProc(ih, msg, wp, lp, result);
}

static void winListDrawItem(Ihandle* ih, DRAWITEMSTRUCT *drawitem)
{
  char* text;
  int txt_w, txt_h;
  winListItemData* itemdata;
  HFONT hFont = (HFONT)iupwinGetHFontAttrib(ih);
  iupwinBitmapDC bmpDC;
  HDC hDC;
  RECT rect;
  COLORREF fgcolor, bgcolor;

  int x = drawitem->rcItem.left;
  int y = drawitem->rcItem.top;
  int width = drawitem->rcItem.right - drawitem->rcItem.left;
  int height = drawitem->rcItem.bottom - drawitem->rcItem.top;

  /* If there are no list box items, skip this message */
  if (drawitem->itemID == -1)
    return;

  hDC = iupwinDrawCreateBitmapDC(&bmpDC, drawitem->hDC, x, y, width, height);

  if (drawitem->itemState & ODS_SELECTED)
    bgcolor = GetSysColor(COLOR_HIGHLIGHT);
  else if (!iupwinGetColorRef(ih, "BGCOLOR", &bgcolor))
    bgcolor = GetSysColor(COLOR_WINDOW);
  SetDCBrushColor(hDC, bgcolor);
  SetRect(&rect, 0, 0, width, height);
  FillRect(hDC, &rect, (HBRUSH)GetStockObject(DC_BRUSH));

  if (iupdrvIsActive(ih))
  {
    if (drawitem->itemState & ODS_SELECTED)
      fgcolor = GetSysColor(COLOR_HIGHLIGHTTEXT);
    else if (!iupwinGetColorRef(ih, "FGCOLOR", &fgcolor))
      fgcolor = GetSysColor(COLOR_WINDOWTEXT);
  }
  else
    fgcolor = GetSysColor(COLOR_GRAYTEXT);

  /* Get the bitmap associated with the item */
  itemdata = winListGetItemData(ih, drawitem->itemID);

  /* Get and draw the string associated with the item */
  text = winListGetText(ih, drawitem->itemID);
  iupdrvFontGetMultiLineStringSize(ih, text, &txt_w, &txt_h);

  x = ih->data->maximg_w + 3; /* spacing between text and image */
  y = (height - txt_h)/2;  /* vertically centered */
  iupwinDrawText(hDC, text, x, y, txt_w, txt_h, hFont, fgcolor, 0);

  /* Draw the bitmap associated with the item */
  if (itemdata->hBitmap)
  {
    int bpp, img_w, img_h;
    HBITMAP hMask = NULL;

    iupdrvImageGetInfo(itemdata->hBitmap, &img_w, &img_h, &bpp);

    if (bpp == 8)
    {
      char name[50];
      sprintf(name, "IMAGE%d", (int)drawitem->itemID+1);
      hMask = iupdrvImageCreateMask(IupGetAttributeHandle(ih, name));
    }

    x = 0;
    y = (height - img_h)/2;  /* vertically centered */
    iupwinDrawBitmap(hDC, itemdata->hBitmap, hMask, x, y, img_w, img_h, bpp);

    if (hMask)
      DeleteObject(hMask);
  }

  /* If the item has the focus, draw the focus rectangle */
  if (drawitem->itemState & ODS_FOCUS)
    iupdrvDrawFocusRect(ih, hDC, 0, 0, width, height);

  free(text);
  iupwinDrawDestroyBitmapDC(&bmpDC);
}


/*********************************************************************************/


static void winListLayoutUpdateMethod(Ihandle *ih)
{
  if (ih->data->is_dropdown)
  {
    /* Must add the dropdown area, or it will not be visible */
    RECT rect;
    int charheight, calc_h, win_h, win_w, voptions;

    voptions = iupAttribGetInt(ih, "VISIBLE_ITEMS");
    if (voptions <= 0)
      voptions = 1;

    iupdrvFontGetCharSize(ih, NULL, &charheight);
    calc_h = ih->currentheight + voptions*charheight;

    SendMessage(ih->handle, CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)&rect);
    win_h = rect.bottom-rect.top;
    win_w = rect.right-rect.left;

    if (ih->currentwidth != win_w || calc_h != win_h)
      SetWindowPos(ih->handle, HWND_TOP, ih->x, ih->y, ih->currentwidth, calc_h, 
                   SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER);
    else
      SetWindowPos(ih->handle, HWND_TOP, ih->x, ih->y, 0, 0, 
                   SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER);
  }                
  else
    iupdrvBaseLayoutUpdateMethod(ih);
}

static void winListUnMapMethod(Ihandle* ih)
{
  HWND handle;

  winListRemoveAllItemData(ih);

  handle = (HWND)iupAttribGet(ih, "_IUPWIN_LISTBOX");
  if (handle)
  {
    iupwinHandleRemove(handle);
    iupAttribSetStr(ih, "_IUPWIN_LISTBOX", NULL);
  }

  handle = (HWND)iupAttribGet(ih, "_IUPWIN_EDITBOX");
  if (handle)
  {
    iupwinHandleRemove(handle);
    iupAttribSetStr(ih, "_IUPWIN_EDITBOX", NULL);
  }

  iupdrvBaseUnMapMethod(ih);
}

static int winListMapMethod(Ihandle* ih)
{
  char* class_name;
  DWORD dwStyle = WS_CHILD|WS_CLIPSIBLINGS,
      dwExStyle = WS_EX_CLIENTEDGE;

  if (!ih->parent)
    return IUP_ERROR;

  if (ih->data->is_dropdown || ih->data->has_editbox)
  {
    class_name = "COMBOBOX";

    dwStyle |= CBS_NOINTEGRALHEIGHT;

    if (ih->data->show_image)
      dwStyle |= CBS_OWNERDRAWVARIABLE|CBS_HASSTRINGS;

    if (ih->data->is_dropdown)
      dwStyle |= WS_VSCROLL|WS_HSCROLL;
    else if (ih->data->sb)
    {
      dwStyle |= WS_VSCROLL|WS_HSCROLL;

      if (!iupAttribGetBoolean(ih, "AUTOHIDE"))
        dwStyle |= CBS_DISABLENOSCROLL;
    }

    if (ih->data->has_editbox)
    {
      dwStyle |= CBS_AUTOHSCROLL;

      if (ih->data->is_dropdown)
        dwStyle |= CBS_DROPDOWN;  /* hidden-list+edit */
      else
        dwStyle |= CBS_SIMPLE;  /* visible-list+edit */
    }
    else
      dwStyle |= CBS_DROPDOWNLIST;  /* hidden-list */

    if (iupAttribGetBoolean(ih, "SORT"))
      dwStyle |= CBS_SORT;
  }
  else
  {
    class_name = "LISTBOX";

    dwStyle |= LBS_NOINTEGRALHEIGHT|LBS_NOTIFY;

    if (ih->data->is_multiple)
      dwStyle |= LBS_EXTENDEDSEL;

    if (ih->data->show_image)
      dwStyle |= LBS_OWNERDRAWVARIABLE|LBS_HASSTRINGS;

    if (ih->data->sb)
    {
      dwStyle |= WS_VSCROLL|WS_HSCROLL;

      if (!iupAttribGetBoolean(ih, "AUTOHIDE"))
        dwStyle |= LBS_DISABLENOSCROLL;
    }

    if (iupAttribGetBoolean(ih, "SORT"))
      dwStyle |= LBS_SORT;
  }

  if (iupAttribGetBoolean(ih, "CANFOCUS"))
    dwStyle |= WS_TABSTOP;

  if (!iupwinCreateWindowEx(ih, class_name, dwExStyle, dwStyle))
    return IUP_ERROR;

  /* Custom Procedure */
  IupSetCallback(ih, "_IUPWIN_CTRLPROC_CB", (Icallback)winListProc);

  /* Process background color */
  IupSetCallback(ih, "_IUPWIN_CTLCOLOR_CB", (Icallback)winListCtlColor);

  /* Process WM_COMMAND */
  IupSetCallback(ih, "_IUPWIN_COMMAND_CB", (Icallback)winListWmCommand);

  if (ih->data->is_dropdown || ih->data->has_editbox)
  {
    COMBOBOXINFO boxinfo;

    ZeroMemory(&boxinfo, sizeof(COMBOBOXINFO));
    boxinfo.cbSize = sizeof(COMBOBOXINFO);

    GetComboBoxInfo(ih->handle, &boxinfo);

    iupwinHandleAdd(ih, boxinfo.hwndList);
    iupAttribSetStr(ih, "_IUPWIN_LISTBOX", (char*)boxinfo.hwndList);

    /* subclass the list box. */
    IupSetCallback(ih, "_IUPWIN_LISTOLDPROC_CB", (Icallback)GetWindowLongPtr(boxinfo.hwndList, GWLP_WNDPROC));
    SetWindowLongPtr(boxinfo.hwndList, GWLP_WNDPROC, (LONG_PTR)winListComboListWinProc);

    if (ih->data->has_editbox)
    {
      iupwinHandleAdd(ih, boxinfo.hwndItem);
      iupAttribSetStr(ih, "_IUPWIN_EDITBOX", (char*)boxinfo.hwndItem);

      /* subclass the edit box. */
      IupSetCallback(ih, "_IUPWIN_EDITOLDPROC_CB", (Icallback)GetWindowLongPtr(boxinfo.hwndItem, GWLP_WNDPROC));
      SetWindowLongPtr(boxinfo.hwndItem, GWLP_WNDPROC, (LONG_PTR)winListEditWinProc);

      /* set defaults */
      SendMessage(ih->handle, CB_LIMITTEXT, 0, 0L);
    }
  }

  if(ih->data->show_image)
    IupSetCallback(ih, "_IUPWIN_DRAWITEM_CB", (Icallback)winListDrawItem);  /* Process WM_DRAWITEM */

  /* configure for DROP of files */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSetStr(ih, "DROPFILESTARGET", "YES");

  IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)winListConvertXYToPos);

  iupListSetInitialItems(ih);

  return IUP_NOERROR;
}

void iupdrvListInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = winListMapMethod;
  ic->LayoutUpdate = winListLayoutUpdateMethod;
  ic->UnMap = winListUnMapMethod;

  /* Driver Dependent Attribute functions */

  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, winListSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, winListSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_NOT_MAPPED);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "AUTOREDRAW", NULL, iupwinSetAutoRedrawAttrib, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* IupList only */
  iupClassRegisterAttributeId(ic, "IDVALUE", winListGetIdValueAttrib, iupListSetIdValueAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", winListGetValueAttrib, winListSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWDROPDOWN", NULL, winListSetShowDropdownAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOPITEM", NULL, winListSetTopItemAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VISIBLE_ITEMS", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "DROPEXPAND", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPACING", iupListGetSpacingAttrib, winListSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);

  iupClassRegisterAttribute(ic, "PADDING", iupListGetPaddingAttrib, winListSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", winListGetSelectedTextAttrib, winListSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", winListGetSelectionAttrib, winListSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", winListGetSelectionPosAttrib, winListSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", winListGetCaretAttrib, winListSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", winListGetCaretPosAttrib, winListSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, winListSetInsertAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, winListSetAppendAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "READONLY", winListGetReadOnlyAttrib, winListSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "NC", iupListGetNCAttrib, winListSetNCAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, winListSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, winListSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, winListSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttributeId(ic, "IMAGE", NULL, winListSetImageAttrib, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CUEBANNER", NULL, winListSetCueBannerAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTER", NULL, winListSetFilterAttrib, NULL, NULL, IUPAF_NO_INHERIT);
}
