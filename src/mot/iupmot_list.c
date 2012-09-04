/** \file
 * \brief List Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Xm/ComboBox.h>
#include <Xm/ScrolledW.h>
#include <Xm/TextF.h>
#include <X11/keysym.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_attrib.h"
#include "iup_dialog.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_mask.h"
#include "iup_key.h"
#include "iup_list.h"

#include "iupmot_drv.h"
#include "iupmot_color.h"


static void motListComboBoxSelectionCallback(Widget w, Ihandle* ih, XmComboBoxCallbackStruct* call_data);


void iupdrvListAddItemSpace(Ihandle* ih, int *h)
{
  if (ih->data->has_editbox)
    *h += 1;
  else
    *h += 3;
}

void iupdrvListAddBorders(Ihandle* ih, int *x, int *y)
{
  int border_size = 2*4;
  (*x) += border_size;
  (*y) += border_size;

  if (ih->data->is_dropdown)
  {
    if (ih->data->has_editbox)
    {
      /* extra border for the editbox */
      int internal_border_size = 2*2;
      (*x) += internal_border_size; 
      (*y) += internal_border_size;
    }
  }
  else
  {
    if (ih->data->has_editbox)
      (*y) += 2*2; /* internal border between editbox and list */
    else
      (*x) += 2; /* extra border for the simple list */
  }
}

static int motListConvertXYToPos(Ihandle* ih, int x, int y)
{
  (void)x;
  if (ih->data->has_editbox)
  {
    Widget cblist;
    XtVaGetValues(ih->handle, XmNlist, &cblist, NULL);
    return XmListYToPos(cblist, (Position)y);   /* XmListYToPos returns start at 1 */
  }
  else
    return XmListYToPos(ih->handle, (Position)y);
}

int iupdrvListGetCount(Ihandle* ih)
{
  int count;
  XtVaGetValues(ih->handle, XmNitemCount, &count, NULL);
  return count;
}

static void motListAddItem(Ihandle* ih, int pos, const char* value)
{
  XmString str = XmStringCreateLocalized((String)value);
  /* The utility functions use 0=last 1=first */
  if (ih->data->is_dropdown || ih->data->has_editbox)
    XmComboBoxAddItem(ih->handle, str, pos+1, False);
  else
    XmListAddItem(ih->handle, str, pos+1);
  XmStringFree(str);
}

static void motListAddSortedItem(Ihandle* ih, const char *value)
{
  char *text;
  XmString *strlist;
  int u_bound, l_bound = 0;

  XtVaGetValues(ih->handle, XmNitemCount, &u_bound, XmNitems, &strlist, NULL);

  u_bound--;
  /* perform binary search */
  while (u_bound >= l_bound) 
  {
    int i = l_bound + (u_bound - l_bound)/2;
    text = (char*)XmStringUnparse(strlist[i], NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    if (!text)
      break;
    if (strcmp (text, value) > 0)
      u_bound = i-1; /* newtext comes before item */
    else
      l_bound = i+1; /* newtext comes after item */
    XtFree(text);
  }

  motListAddItem(ih, l_bound, value);
}

void iupdrvListAppendItem(Ihandle* ih, const char* value)
{
  if (iupAttribGetBoolean(ih, "SORT"))
    motListAddSortedItem(ih, value);
  else
    motListAddItem(ih, -1, value);
}

void iupdrvListInsertItem(Ihandle* ih, int pos, const char* value)
{
  if (iupAttribGetBoolean(ih, "SORT"))
    motListAddSortedItem(ih, value);
  else
    motListAddItem(ih, pos, value);

  iupListUpdateOldValue(ih, pos, 0);
}

void iupdrvListRemoveItem(Ihandle* ih, int pos)
{
  /* The utility functions use 0=last 1=first */
  if (ih->data->is_dropdown || ih->data->has_editbox)
  {
    if (ih->data->is_dropdown && !ih->data->has_editbox)
    {
      /* must check if removing the current item */
      int curpos;
      XtVaGetValues(ih->handle, XmNselectedPosition, &curpos, NULL);
      if (pos == curpos && iupdrvListGetCount(ih)>1)
      {
        if (curpos > 0) curpos--;
        else curpos++;

        XtRemoveCallback(ih->handle, XmNselectionCallback, (XtCallbackProc)motListComboBoxSelectionCallback, (XtPointer)ih);
        XtVaSetValues(ih->handle, XmNselectedPosition, curpos, NULL);  
        XtAddCallback(ih->handle, XmNselectionCallback, (XtCallbackProc)motListComboBoxSelectionCallback, (XtPointer)ih);
      }
    }
    XmComboBoxDeletePos(ih->handle, pos+1);
  }
  else
    XmListDeletePos(ih->handle, pos+1);

  iupListUpdateOldValue(ih, pos, 1);
}

void iupdrvListRemoveAllItems(Ihandle* ih)
{
  if (ih->data->is_dropdown || ih->data->has_editbox)
  {
    Widget cblist;
    XtVaGetValues(ih->handle, XmNlist, &cblist, NULL);
    XmListDeleteAllItems(cblist);
    XmComboBoxUpdate(ih->handle);
  }
  else
    XmListDeleteAllItems(ih->handle);
}


/*********************************************************************************/


static char* motListGetIdValueAttrib(Ihandle* ih, int id)
{
  int pos = iupListGetPos(ih, id);
  if (pos >= 0)
  {
    XmString* items;
    XtVaGetValues(ih->handle, XmNitems, &items, NULL);
    return iupmotConvertString(items[pos]);
  }
  return NULL;
}

static int motListSetBgColorAttrib(Ihandle* ih, const char* value)
{
  Widget sb_win = (Widget)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (sb_win)
  {
    Pixel color;

    /* ignore given value for the scrollbars, must use only from parent */
    char* parent_value = iupBaseNativeParentGetBgColor(ih);

    color = iupmotColorGetPixelStr(parent_value);
    if (color != (Pixel)-1)
    {
      Widget sb = NULL;

      iupmotSetBgColor(sb_win, color);

      XtVaGetValues(sb_win, XmNverticalScrollBar, &sb, NULL);
      if (sb) iupmotSetBgColor(sb, color);

      XtVaGetValues(sb_win, XmNhorizontalScrollBar, &sb, NULL);
      if (sb) iupmotSetBgColor(sb, color);
    }

    return iupdrvBaseSetBgColorAttrib(ih, value);    /* use given value for contents */
  }
  else
  {
    char* parent_value;

    /* use given value for Edit and List also */
    Pixel color = iupmotColorGetPixelStr(value);
    if (color != (Pixel)-1)
    {
      Widget cbedit, cblist, sb;

      iupmotSetBgColor(ih->handle, color);

      XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
      if (cbedit) iupmotSetBgColor(cbedit, color);

      XtVaGetValues(ih->handle, XmNlist, &cblist, NULL);
      if (cblist) iupmotSetBgColor(cblist, color);

      XtVaGetValues(cblist, XmNverticalScrollBar, &sb, NULL);
      if (sb) iupmotSetBgColor(sb, color);

      XtVaGetValues(cblist, XmNhorizontalScrollBar, &sb, NULL);
      if (sb) iupmotSetBgColor(sb, color);
    }

    /* but reset just the background, so the combobox will look like a button */
    parent_value = iupBaseNativeParentGetBgColor(ih);

    color = iupmotColorGetPixelStr(parent_value);
    if (color != (Pixel)-1)
      XtVaSetValues(ih->handle, XmNbackground, color, NULL);

    return 1;
  }
}

static int motListSetFgColorAttrib(Ihandle* ih, const char* value)
{
  Pixel color = iupmotColorGetPixelStr(value);
  if (color != (Pixel)-1)
  {
    XtVaSetValues(ih->handle, XmNforeground, color, NULL);

    if (ih->data->is_dropdown || ih->data->has_editbox)
    {
      Widget w;
      XtVaGetValues(ih->handle, XmNtextField, &w, NULL);
      XtVaSetValues(w, XmNforeground, color, NULL);

      XtVaGetValues(ih->handle, XmNlist, &w, NULL);
      XtVaSetValues(w, XmNforeground, color, NULL);
    }
  }

  return 1;
}

static char* motListGetValueAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    char *str, *xstr;
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    xstr = XmTextFieldGetString(cbedit);
    str = iupStrGetMemoryCopy(xstr);
    XtFree(xstr);
    return str;
  }
  else 
  {
    if (ih->data->is_dropdown)
    {
      char* str;
      int pos;
      XtVaGetValues(ih->handle, XmNselectedPosition, &pos, NULL);
      str = iupStrGetMemory(50);
      sprintf(str, "%d", pos+1);  /* IUP starts at 1 */
      return str;
    }
    else
    {
      int *pos, sel_count;
      if (XmListGetSelectedPos(ih->handle, &pos, &sel_count))  /* XmListGetSelectedPos starts at 1 */
      {
        if (!ih->data->is_multiple)
        {
          char* str = iupStrGetMemory(50);
          sprintf(str, "%d", pos[0]);  
          XtFree((char*)pos);
          return str;
        }
        else
        {
          int i, count;
          char* str;
          XtVaGetValues(ih->handle, XmNitemCount, &count, NULL);
          str = iupStrGetMemory(count+1);
          memset(str, '-', count);
          str[count]=0;
          for (i=0; i<sel_count; i++)
            str[pos[i]-1] = '+';
          XtFree((char*)pos);
          return str;
        }
      }
    }
  }

  return NULL;
}

static int motListSetValueAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->has_editbox)
  {
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    if (!value) value = "";

    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", "1"); /* disable callbacks */

    XmTextFieldSetString(cbedit, (char*)value);

    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", NULL);
  }
  else 
  {
    if (ih->data->is_dropdown)
    {
      int pos;
      if (iupStrToInt(value, &pos)==1)
      {
        XtRemoveCallback(ih->handle, XmNselectionCallback, (XtCallbackProc)motListComboBoxSelectionCallback, (XtPointer)ih);

        XtVaSetValues(ih->handle, XmNselectedPosition, pos-1, NULL);  /* IUP starts at 1 */
        iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);

        XtAddCallback(ih->handle, XmNselectionCallback, (XtCallbackProc)motListComboBoxSelectionCallback, (XtPointer)ih);
      }
    }
    else
    {
      if (!ih->data->is_multiple)
      {
        int pos;
        if (iupStrToInt(value, &pos)==1)
        {
          XmListSelectPos(ih->handle, pos, FALSE);   /* XmListSelectPos starts at 1 */
          iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);
        }
        else
        {
          XmListDeselectAllItems(ih->handle);
          iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
        }
      }
      else
      {
        /* User has changed a multiple selection on a simple list. */
	      int i, count, len;

        /* Clear all selections */
        XmListDeselectAllItems(ih->handle);

        if (!value)
        {
          iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
          return 0;
        }

        XtVaGetValues(ih->handle, XmNitemCount, &count, NULL);
        len = strlen(value);
        if (len < count) 
          count = len;

        XtVaSetValues(ih->handle, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);

        /* update selection list */
        for (i = 0; i<count; i++)
        {
          if (value[i]=='+')
            XmListSelectPos(ih->handle, i+1, False);  /* XmListSelectPos starts at 1 */
        }

        XtVaSetValues(ih->handle, XmNselectionPolicy, XmEXTENDED_SELECT, 
                                  XmNselectionMode, XmNORMAL_MODE, NULL);  /* must also restore this */
        iupAttribStoreStr(ih, "_IUPLIST_OLDVALUE", value);
      }
    }
  }

  return 0;
}

static int motListSetVisibleItemsAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->is_dropdown)
  {
    int count;
    if (iupStrToInt(value, &count)==1)
      XtVaSetValues(ih->handle, XmNvisibleItemCount, count, NULL);
  }
  return 1;
}

static int motListSetShowDropdownAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->is_dropdown)
  {
    if (iupStrBoolean(value))
    {
      XButtonEvent ev;
      memset(&ev, 0, sizeof(XButtonEvent));
      ev.type = ButtonPress;
      ev.display = XtDisplay(ih->handle);
      ev.send_event = True;
      ev.root = RootWindow(iupmot_display, iupmot_screen);
      ev.time = clock()*CLOCKS_PER_SEC;
      ev.window = XtWindow(ih->handle);
      ev.state = Button1Mask;
      ev.button = Button1;
      ev.same_screen = True;
      XtCallActionProc(ih->handle, "CBDropDownList", (XEvent*)&ev, 0, 0 ); 
    }
    else
      XtCallActionProc(ih->handle, "CBDisarm", 0, 0, 0 );
  }
  return 0;
}

static int motListSetTopItemAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->is_dropdown)
  {
    int pos = 1;
    if (iupStrToInt(value, &pos))
    {
      if (ih->data->has_editbox)
      {
        Widget cblist;
        XtVaGetValues(ih->handle, XmNlist, &cblist, NULL);
        XtVaSetValues(cblist, XmNtopItemPosition, pos, NULL);
      }
      else
        XtVaSetValues(ih->handle, XmNtopItemPosition, pos, NULL);
    }
  }
  return 0;
}

static int motListSetSpacingAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->is_dropdown)
    return 0;

  if (!iupStrToInt(value, &ih->data->spacing))
    ih->data->spacing = 0;

  if (ih->handle)
  {
    Widget list = ih->handle;
    if (ih->data->has_editbox)
      XtVaGetValues(ih->handle, XmNlist, &list, NULL);

    XtVaSetValues(list, XmNlistSpacing, ih->data->spacing*2, 
                        XmNlistMarginWidth, ih->data->spacing, 
                        XmNlistMarginHeight, ih->data->spacing, 
                        NULL);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int motListSetPaddingAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  if (ih->handle)
  {
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XtVaSetValues(cbedit, XmNmarginHeight, ih->data->vert_padding,
                          XmNmarginWidth, ih->data->horiz_padding, NULL);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int motListSetReadOnlyAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->has_editbox)
  {
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XtVaSetValues(cbedit, XmNeditable, iupStrBoolean(value)? False: True, NULL);
  }
  return 0;
}

static char* motListGetReadOnlyAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    Boolean editable;
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XtVaGetValues(cbedit, XmNeditable, &editable, NULL);
    if (editable)
      return "YES";
    else
      return "NO";
  }
  else
    return NULL;
}

static int motListSetInsertAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    return 0;

  if (ih->data->has_editbox)
  {
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", "1"); /* disable callbacks */
    XmTextFieldRemove(cbedit);
    XmTextFieldInsert(cbedit, XmTextFieldGetInsertionPosition(cbedit), (char*)value);
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", NULL);
  }

  return 0;
}

static int motListSetSelectedTextAttrib(Ihandle* ih, const char* value)
{
  XmTextPosition start, end;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  if (XmTextFieldGetSelectionPosition(cbedit, &start, &end) && start!=end)
  {
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", "1"); /* disable callbacks */
    XmTextFieldReplace(cbedit, start, end, (char*)value);
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", NULL);
  }

  return 0;
}

static char* motListGetSelectedTextAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    char* selectedtext, *str;
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    selectedtext = XmTextFieldGetSelection(cbedit);
    str = iupStrGetMemoryCopy(selectedtext);
    XtFree(selectedtext);
    return str;
  }
  else
    return NULL;
}

static int motListSetAppendAttrib(Ihandle* ih, const char* value)
{
	if (value && ih->data->has_editbox)
  {
    XmTextPosition pos;
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    pos = XmTextFieldGetLastPosition(cbedit);
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", "1"); /* disable callbacks */
    XmTextFieldInsert(cbedit, pos+1, (char*)value);
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", NULL);
  }
  return 0;
}

static int motListSetSelectionAttrib(Ihandle* ih, const char* value)
{
  int start=1, end=1;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XmTextFieldClearSelection(cbedit, CurrentTime);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XmTextFieldSetSelection(cbedit, (XmTextPosition)0, (XmTextPosition)XmTextFieldGetLastPosition(cbedit), CurrentTime);
    return 0;
  }

  if (iupStrToIntInt(value, &start, &end, ':')!=2) 
    return 0;

  if(start<1 || end<1) 
    return 0;

  start--; /* IUP starts at 1 */
  end--;

  /* end is inside the selection, in IUP is outside */
  end--;

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  XmTextFieldSetSelection(cbedit, (XmTextPosition)start, (XmTextPosition)end, CurrentTime);

  return 0;
}

static char* motListGetSelectionAttrib(Ihandle* ih)
{
  XmTextPosition start = 0, end = 0;
  char* str;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return NULL;

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  if (!XmTextFieldGetSelectionPosition(cbedit, &start, &end) || start==end)
    return NULL;

  str = iupStrGetMemory(100);

  /* end is inside the selection, in IUP is outside */
  end++;

  start++; /* IUP starts at 1 */
  end++;
  sprintf(str, "%d:%d", (int)start, (int)end);

  return str;
}

static int motListSetSelectionPosAttrib(Ihandle* ih, const char* value)
{
  int start=0, end=0;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value || iupStrEqualNoCase(value, "NONE"))
  {
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XmTextFieldClearSelection(cbedit, CurrentTime);
    return 0;
  }

  if (iupStrEqualNoCase(value, "ALL"))
  {
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XmTextFieldSetSelection(cbedit, (XmTextPosition)0, (XmTextPosition)XmTextFieldGetLastPosition(cbedit), CurrentTime);
    return 0;
  }

  if (iupStrToIntInt(value, &start, &end, ':')!=2) 
    return 0;

  if(start<0 || end<0) 
    return 0;

  /* end is inside the selection, in IUP is outside */
  end--;

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  XmTextFieldSetSelection(cbedit, (XmTextPosition)start, (XmTextPosition)end, CurrentTime);

  return 0;
}

static char* motListGetSelectionPosAttrib(Ihandle* ih)
{
  XmTextPosition start = 0, end = 0;
  char* str;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return NULL;

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  if (!XmTextFieldGetSelectionPosition(cbedit, &start, &end) || start==end)
    return NULL;

  str = iupStrGetMemory(100);

  /* end is inside the selection, in IUP is outside */
  end++;

  sprintf(str, "%d:%d", (int)start, (int)end);

  return str;
}

static int motListSetCaretAttrib(Ihandle* ih, const char* value)
{
  int pos = 1;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  pos--; /* IUP starts at 1 */

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  XmTextFieldSetInsertionPosition(cbedit, (XmTextPosition)pos);
  XmTextFieldShowPosition(cbedit, (XmTextPosition)pos);

  return 0;
}

static char* motListGetCaretAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    XmTextPosition pos;
    Widget cbedit;
    char* str = iupStrGetMemory(50);
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    pos = XmTextFieldGetInsertionPosition(cbedit);
    pos++; /* IUP starts at 1 */
    sprintf(str, "%d", (int)pos);
    return str;
  }
  else
    return NULL;
}

static int motListSetCaretPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 0) pos = 0;

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  XmTextFieldSetInsertionPosition(cbedit, (XmTextPosition)pos);
  XmTextFieldShowPosition(cbedit, (XmTextPosition)pos);

  return 0;
}

static char* motListGetCaretPosAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    XmTextPosition pos;
    Widget cbedit;
    char* str = iupStrGetMemory(50);
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    pos = XmTextFieldGetInsertionPosition(cbedit);
    sprintf(str, "%d", (int)pos);
    return str;
  }
  else
    return NULL;
}

static int motListSetScrollToAttrib(Ihandle* ih, const char* value)
{
  int pos = 1;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 1) pos = 1;
  pos--;  /* return to Motif referece */

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  XmTextFieldShowPosition(cbedit, (XmTextPosition)pos);

  return 0;
}

static int motListSetScrollToPosAttrib(Ihandle* ih, const char* value)
{
  int pos = 0;
  Widget cbedit;
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
    return 0;

  sscanf(value,"%i",&pos);
  if (pos < 0) pos = 0;

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
  XmTextFieldShowPosition(cbedit, (XmTextPosition)pos);

  return 0;
}

static int motListSetNCAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  if (!iupStrToInt(value, &ih->data->nc))
    ih->data->nc = INT_MAX;

  if (ih->handle)
  {
    Widget cbedit;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XtVaSetValues(cbedit, XmNmaxLength, ih->data->nc, NULL);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static int motListSetClipboardAttrib(Ihandle *ih, const char *value)
{
  Widget cbedit;
  if (!ih->data->has_editbox)
    return 0;

  XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);

  if (iupStrEqualNoCase(value, "COPY"))
  {
    Ihandle* clipboard;
    char *str = XmTextFieldGetSelection(cbedit);
    if (!str) return 0;

    clipboard = IupClipboard();
    IupSetAttribute(clipboard, "TEXT", str);
    IupDestroy(clipboard);

    XtFree(str);
  }
  else if (iupStrEqualNoCase(value, "CUT"))
  {
    Ihandle* clipboard;
    char *str = XmTextFieldGetSelection(cbedit);
    if (!str) return 0;

    clipboard = IupClipboard();
    IupSetAttribute(clipboard, "TEXT", str);
    IupDestroy(clipboard);

    XtFree(str);

    /* disable callbacks */
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", "1");
    XmTextFieldRemove(cbedit);
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", NULL);
  }
  else if (iupStrEqualNoCase(value, "PASTE"))
  {
    Ihandle* clipboard;
    char *str;

    clipboard = IupClipboard();
    str = IupGetAttribute(clipboard, "TEXT");

    /* disable callbacks */
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", "1");
    XmTextFieldRemove(cbedit);
    XmTextFieldInsert(cbedit, XmTextFieldGetInsertionPosition(cbedit), str);
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", NULL);
  }
  else if (iupStrEqualNoCase(value, "CLEAR"))
  {
    /* disable callbacks */
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", "1");
    XmTextFieldRemove(cbedit);
    iupAttribSetStr(ih, "_IUPMOT_DISABLE_TEXT_CB", NULL);
  }
  return 0;
}


/*********************************************************************************/


static void motListEditModifyVerifyCallback(Widget cbedit, Ihandle *ih, XmTextVerifyPtr text)
{
  int start, end, key = 0;
  char *value, *new_value, *insert_value;
  KeySym motcode = 0;
  IFnis cb;

  if (iupAttribGet(ih, "_IUPMOT_DISABLE_TEXT_CB"))
    return;

  cb = (IFnis)IupGetCallback(ih, "EDIT_CB");
  if (!cb && !ih->data->mask)
    return;

  if (text->event && text->event->type == KeyPress)
  {
    unsigned int state = ((XKeyEvent*)text->event)->state;
    if (state & ControlMask ||  /* Ctrl */
        state & Mod1Mask || 
        state & Mod5Mask ||  /* Alt */
        state & Mod4Mask) /* Apple/Win */
      return;

    motcode = XKeycodeToKeysym(iupmot_display, ((XKeyEvent*)text->event)->keycode, 0);
  }

  value = XmTextFieldGetString(cbedit);
  start = text->startPos;
  end = text->endPos;
  insert_value = text->text->ptr;

  if (motcode == XK_Delete)
  {
    new_value = value;
    iupStrRemove(value, start, end, 1);
  }
  else if (motcode == XK_BackSpace)
  {
    new_value = value;
    iupStrRemove(value, start, end, -1);
  }
  else
  {
    if (!value)
      new_value = iupStrDup(insert_value);
    else if (insert_value)
      new_value = iupStrInsert(value, insert_value, start, end);
    else
      new_value = value;
  }

  if (insert_value && insert_value[0]!=0 && insert_value[1]==0)
    key = insert_value[0];

  if (ih->data->mask && iupMaskCheck(ih->data->mask, new_value)==0)
  {
    if (new_value != value) free(new_value);
    XtFree(value);
    text->doit = False;     /* abort processing */
    return;
  }

  if (cb)
  {
    int cb_ret = cb(ih, key, (char*)new_value);
    if (cb_ret==IUP_IGNORE)
      text->doit = False;     /* abort processing */
    else if (cb_ret==IUP_CLOSE)
    {
      IupExitLoop();
      text->doit = False;     /* abort processing */
    }
    else if (cb_ret!=0 && key!=0 && 
             cb_ret != IUP_DEFAULT && cb_ret != IUP_CONTINUE)  
    {
      insert_value[0] = (char)cb_ret;  /* replace key */
    }
  }

  if (new_value != value) free(new_value);
  XtFree(value);
}

static void motListEditMotionVerifyCallback(Widget w, Ihandle* ih, XmTextVerifyCallbackStruct* textverify)
{
  int pos;

  IFniii cb = (IFniii)IupGetCallback(ih, "CARET_CB");
  if (!cb) return;

  pos = textverify->newInsert;

  if (pos != ih->data->last_caret_pos)
  {
    ih->data->last_caret_pos = pos;
    cb(ih, 1, pos+1, pos);
  }

  (void)w;
}

static void motListEditKeyPressEvent(Widget cbedit, Ihandle *ih, XKeyEvent *evt, Boolean *cont)
{
  *cont = True;
  iupmotKeyPressEvent(cbedit, ih, (XEvent*)evt, cont);
  if (*cont == False)
    return;

  if (evt->state & ControlMask)   /* Ctrl */
  {
    KeySym motcode = XKeycodeToKeysym(iupmot_display, evt->keycode, 0);
    if (motcode == XK_c)
    {
      motListSetClipboardAttrib(ih, "COPY");
      *cont = False;
      return;
    }
    else if (motcode == XK_x)
    {
      motListSetClipboardAttrib(ih, "CUT");
      *cont = False;
      return;
    }
    else if (motcode == XK_v)
    {
      motListSetClipboardAttrib(ih, "PASTE");
      *cont = False;
      return;
    }
    else if (motcode == XK_a)
    {
      XmTextFieldSetSelection(cbedit, 0, XmTextFieldGetLastPosition(cbedit), CurrentTime);
      *cont = False;
      return;
    }
  }
}

static void motListEditValueChangedCallback(Widget w, Ihandle* ih, XmAnyCallbackStruct* valuechanged)
{
  if (iupAttribGet(ih, "_IUPMOT_DISABLE_TEXT_CB"))
    return;

  iupBaseCallValueChangedCb(ih);

  (void)valuechanged;
  (void)w;
}

static void motListDropDownPopupCallback(Widget w, Ihandle* ih, XtPointer call_data)
{
  IFni cb = (IFni)IupGetCallback(ih, "DROPDOWN_CB");
  if (cb)
    cb(ih, 1);
  (void)w;
  (void)call_data;
}

static void motListDropDownPopdownCallback(Widget w, Ihandle* ih, XtPointer call_data)
{
  IFni cb = (IFni)IupGetCallback(ih, "DROPDOWN_CB");
  if (cb)
    cb(ih, 0);
  (void)w;
  (void)call_data;
}

static void motListDefaultActionCallback(Widget w, Ihandle* ih, XmListCallbackStruct* call_data)
{         
  if (call_data->event->type == ButtonPress || call_data->event->type == ButtonRelease)
  {
    IFnis cb = (IFnis) IupGetCallback(ih, "DBLCLICK_CB");
    if (cb)
    {
      int pos = call_data->item_position;  /* Here Motif already starts at 1 */
      iupListSingleCallDblClickCallback(ih, cb, pos);
    }
  }

  (void)w;
}

static void motListComboBoxSelectionCallback(Widget w, Ihandle* ih, XmComboBoxCallbackStruct* call_data)
{         
  IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
  if (cb)
  {
    int pos = call_data->item_position;
    if (pos==0)
    {
      /* must check if it is really checked or it is for the edit box */
      XmString* items;
      XtVaGetValues(ih->handle, XmNitems, &items, NULL);
      if (!XmStringCompare(call_data->item_or_text, items[0]))
       return;
    }
    pos++;  /* IUP starts at 1 */
    iupListSingleCallActionCallback(ih, cb, pos);
  }

  if (!ih->data->has_editbox)
    iupBaseCallValueChangedCb(ih);

  (void)w;
}

static void motListBrowseSelectionCallback(Widget w, Ihandle* ih, XmListCallbackStruct* call_data)
{         
  IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
  if (cb)
  {
    int pos = call_data->item_position;  /* Here Motif already starts at 1 */
    iupListSingleCallActionCallback(ih, cb, pos);
  }

  if (!ih->data->has_editbox)
    iupBaseCallValueChangedCb(ih);

  (void)w;
}

static void motListExtendedSelectionCallback(Widget w, Ihandle* ih, XmListCallbackStruct* call_data)
{         
  IFns multi_cb = (IFns)IupGetCallback(ih, "MULTISELECT_CB");
  IFnsii cb = (IFnsii) IupGetCallback(ih, "ACTION");
  if (multi_cb || cb)
  {
    int* pos = call_data->selected_item_positions;
    int sel_count = call_data->selected_item_count;
    int i;

    /* In Motif, the position of item is "plus one".
       "iupListMultipleCallActionCallback" works with the list of selected items from the zero position.
       So, "minus one" here. */
    for (i = 0; i < sel_count; i++)
      pos[i] -= 1;

    iupListMultipleCallActionCallback(ih, cb, multi_cb, pos, sel_count);
  }

  if (!ih->data->has_editbox)
    iupBaseCallValueChangedCb(ih);

  (void)w;
}


/*********************************************************************************/


static int motListMapMethod(Ihandle* ih)
{
  int num_args = 0;
  Arg args[30];
  Widget parent = iupChildTreeGetNativeParentHandle(ih);
  char* child_id = iupDialogGetChildIdStr(ih);

  if (ih->data->is_dropdown || ih->data->has_editbox)
  {
    /* could not set XmNmappedWhenManaged to False because the list and the edit box where not displayed */
    /* iupMOT_SETARG(args, num_args, XmNmappedWhenManaged, False); */
    iupMOT_SETARG(args, num_args, XmNx, 0);  /* x-position */
    iupMOT_SETARG(args, num_args, XmNy, 0);  /* y-position */
    iupMOT_SETARG(args, num_args, XmNwidth, 10);  /* default width to avoid 0 */
    iupMOT_SETARG(args, num_args, XmNheight, 10); /* default height to avoid 0 */
    iupMOT_SETARG(args, num_args, XmNmarginHeight, 0);
    iupMOT_SETARG(args, num_args, XmNmarginWidth, 0);

    if (iupAttribGetBoolean(ih, "CANFOCUS"))
      iupMOT_SETARG(args, num_args, XmNtraversalOn, True);
    else
      iupMOT_SETARG(args, num_args, XmNtraversalOn, False);

    iupMOT_SETARG(args, num_args, XmNnavigationType, XmTAB_GROUP);
    iupMOT_SETARG(args, num_args, XmNhighlightThickness, 2);
    iupMOT_SETARG(args, num_args, XmNshadowThickness, 2);

    if (ih->data->has_editbox)
    {
      if (ih->data->is_dropdown)
        iupMOT_SETARG(args, num_args, XmNcomboBoxType, XmDROP_DOWN_COMBO_BOX);  /* hidden-list+edit */
      else
        iupMOT_SETARG(args, num_args, XmNcomboBoxType, XmCOMBO_BOX);  /* visible-list+edit */
    }
    else
      iupMOT_SETARG(args, num_args, XmNcomboBoxType, XmDROP_DOWN_LIST);   /* hidden-list */

    /* XmComboBoxWidget inherits from XmManager, 
       so it is a container with the actual list inside */

    ih->handle = XtCreateManagedWidget(
      child_id,  /* child identifier */
      xmComboBoxWidgetClass, /* widget class */
      parent,                      /* widget parent */
      args, num_args);
  }
  else
  {
    Widget sb_win;

    /* Create the scrolled window */

    iupMOT_SETARG(args, num_args, XmNmappedWhenManaged, False);  /* not visible when managed */
    iupMOT_SETARG(args, num_args, XmNscrollingPolicy, XmAPPLICATION_DEFINED);
    iupMOT_SETARG(args, num_args, XmNvisualPolicy, XmVARIABLE);
    iupMOT_SETARG(args, num_args, XmNscrollBarDisplayPolicy, XmSTATIC);   /* can NOT be XmAS_NEEDED because XmAPPLICATION_DEFINED */
    iupMOT_SETARG(args, num_args, XmNspacing, 0); /* no space between scrollbars and text */
    iupMOT_SETARG(args, num_args, XmNborderWidth, 0);
    iupMOT_SETARG(args, num_args, XmNshadowThickness, 0);
    
    sb_win = XtCreateManagedWidget(
      child_id,  /* child identifier */
      xmScrolledWindowWidgetClass, /* widget class */
      parent,                      /* widget parent */
      args, num_args);

    if (!sb_win)
      return IUP_ERROR;

    parent = sb_win;
    child_id = "list";

    /* Create the list */

    num_args = 0;
    iupMOT_SETARG(args, num_args, XmNx, 0);  /* x-position */
    iupMOT_SETARG(args, num_args, XmNy, 0);  /* y-position */
    iupMOT_SETARG(args, num_args, XmNwidth, 10);  /* default width to avoid 0 */
    iupMOT_SETARG(args, num_args, XmNheight, 10); /* default height to avoid 0 */

    if (iupAttribGetBoolean(ih, "CANFOCUS"))
      iupMOT_SETARG(args, num_args, XmNtraversalOn, True);
    else
      iupMOT_SETARG(args, num_args, XmNtraversalOn, False);

    iupMOT_SETARG(args, num_args, XmNnavigationType, XmTAB_GROUP);
    iupMOT_SETARG(args, num_args, XmNhighlightThickness, 2);
    iupMOT_SETARG(args, num_args, XmNshadowThickness, 2);

    iupMOT_SETARG(args, num_args, XmNlistMarginHeight, 0);  /* default padding */
    iupMOT_SETARG(args, num_args, XmNlistMarginWidth, 0);
    iupMOT_SETARG(args, num_args, XmNlistSpacing, 0);
    iupMOT_SETARG(args, num_args, XmNlistSizePolicy, XmCONSTANT);  /* don't grow to fit, add scrollbar */

    if (ih->data->is_multiple)
      iupMOT_SETARG(args, num_args, XmNselectionPolicy, XmEXTENDED_SELECT);
    else
      iupMOT_SETARG(args, num_args, XmNselectionPolicy, XmBROWSE_SELECT);

    if (iupAttribGetBoolean(ih, "AUTOHIDE"))
      iupMOT_SETARG(args, num_args, XmNscrollBarDisplayPolicy, XmAS_NEEDED);
    else
      iupMOT_SETARG(args, num_args, XmNscrollBarDisplayPolicy, XmSTATIC);

    ih->handle = XtCreateManagedWidget(
      child_id,          /* child identifier */
      xmListWidgetClass, /* widget class */
      parent,            /* widget parent */
      args, num_args);
  }

  if (!ih->handle)
    return IUP_ERROR;

  ih->serial = iupDialogGetChildId(ih); /* must be after using the string */

  if (ih->data->is_dropdown || ih->data->has_editbox)
  {
    Widget cbedit, cblist;
    XtVaGetValues(ih->handle, XmNtextField, &cbedit, NULL);
    XtVaGetValues(ih->handle, XmNlist, &cblist, NULL);

    XtAddEventHandler(cbedit, FocusChangeMask, False, (XtEventHandler)iupmotFocusChangeEvent, (XtPointer)ih);
    XtAddEventHandler(cbedit, EnterWindowMask, False, (XtEventHandler)iupmotEnterLeaveWindowEvent, (XtPointer)ih);
    XtAddEventHandler(cbedit, LeaveWindowMask, False, (XtEventHandler)iupmotEnterLeaveWindowEvent, (XtPointer)ih);
    XtAddCallback(cbedit, XmNhelpCallback, (XtCallbackProc)iupmotHelpCallback, (XtPointer)ih);

    XtAddCallback(ih->handle, XmNselectionCallback, (XtCallbackProc)motListComboBoxSelectionCallback, (XtPointer)ih);

    if (iupStrBoolean(IupGetGlobal("INPUTCALLBACKS")))
      XtAddEventHandler(cbedit, PointerMotionMask, False, (XtEventHandler)iupmotDummyPointerMotionEvent, NULL);

    if (ih->data->has_editbox)
    {
      XtAddEventHandler(cbedit, KeyPressMask, False, (XtEventHandler)motListEditKeyPressEvent, (XtPointer)ih);
      XtAddCallback(cbedit, XmNmodifyVerifyCallback, (XtCallbackProc)motListEditModifyVerifyCallback, (XtPointer)ih);
      XtAddCallback(cbedit, XmNmotionVerifyCallback, (XtCallbackProc)motListEditMotionVerifyCallback, (XtPointer)ih);
      XtAddCallback(cbedit, XmNvalueChangedCallback, (XtCallbackProc)motListEditValueChangedCallback, (XtPointer)ih);

      iupAttribSetStr(ih, "_IUPMOT_DND_WIDGET", (char*)cbedit);
    }
    else
      XtAddEventHandler(cbedit, KeyPressMask, False, (XtEventHandler)iupmotKeyPressEvent, (XtPointer)ih);

    if (ih->data->is_dropdown)
    {
      XtVaSetValues(ih->handle, XmNvisibleItemCount, 5, NULL);
      XtAddCallback(XtParent(XtParent(cblist)), XmNpopupCallback, (XtCallbackProc)motListDropDownPopupCallback, (XtPointer)ih);
      XtAddCallback(XtParent(XtParent(cblist)), XmNpopdownCallback, (XtCallbackProc)motListDropDownPopdownCallback, (XtPointer)ih);
    }
    else
    {
      XtAddCallback(cblist, XmNdefaultActionCallback, (XtCallbackProc)motListDefaultActionCallback, (XtPointer)ih);
      XtAddEventHandler(cblist, PointerMotionMask, False, (XtEventHandler)iupmotPointerMotionEvent, (XtPointer)ih);
      XtAddEventHandler(cblist, ButtonPressMask|ButtonReleaseMask, False, (XtEventHandler)iupmotButtonPressReleaseEvent, (XtPointer)ih);

      /* Disable Drag Source */
      iupmotDisableDragSource(cblist);
  }
  }
  else
  {
    iupAttribSetStr(ih, "_IUP_EXTRAPARENT", (char*)parent);
    XtVaSetValues(parent, XmNworkWindow, ih->handle, NULL);

    XtAddEventHandler(ih->handle, FocusChangeMask, False, (XtEventHandler)iupmotFocusChangeEvent, (XtPointer)ih);
    XtAddEventHandler(ih->handle, EnterWindowMask, False, (XtEventHandler)iupmotEnterLeaveWindowEvent, (XtPointer)ih);
    XtAddEventHandler(ih->handle, LeaveWindowMask, False, (XtEventHandler)iupmotEnterLeaveWindowEvent, (XtPointer)ih);
    XtAddEventHandler(ih->handle, KeyPressMask, False, (XtEventHandler)iupmotKeyPressEvent, (XtPointer)ih);
    XtAddEventHandler(ih->handle, PointerMotionMask, False, (XtEventHandler)iupmotPointerMotionEvent, (XtPointer)ih);
    XtAddEventHandler(ih->handle, ButtonPressMask|ButtonReleaseMask, False, (XtEventHandler)iupmotButtonPressReleaseEvent, (XtPointer)ih);

    XtAddCallback(ih->handle, XmNhelpCallback, (XtCallbackProc)iupmotHelpCallback, (XtPointer)ih);
    XtAddCallback (ih->handle, XmNbrowseSelectionCallback, (XtCallbackProc)motListBrowseSelectionCallback, (XtPointer)ih);
    XtAddCallback (ih->handle, XmNextendedSelectionCallback, (XtCallbackProc)motListExtendedSelectionCallback, (XtPointer)ih);
    XtAddCallback (ih->handle, XmNdefaultActionCallback, (XtCallbackProc)motListDefaultActionCallback, (XtPointer)ih);
  }

  /* Disable Drag Source */
  iupmotDisableDragSource(ih->handle);

  /* initialize the widget */
  if (ih->data->is_dropdown || ih->data->has_editbox)
    XtRealizeWidget(ih->handle);
  else
    XtRealizeWidget(parent);

  if (IupGetGlobal("_IUP_RESET_TXTCOLORS"))
  {
    iupmotSetGlobalColorAttrib(ih->handle, XmNbackground, "TXTBGCOLOR");
    iupmotSetGlobalColorAttrib(ih->handle, XmNforeground, "TXTFGCOLOR");
    IupSetGlobal("_IUP_RESET_TXTCOLORS", NULL);
  }

  IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)motListConvertXYToPos);

  iupListSetInitialItems(ih);

  return IUP_NOERROR;
}

void iupdrvListInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = motListMapMethod;

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, motListSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, motListSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupList only */
  iupClassRegisterAttributeId(ic, "IDVALUE", motListGetIdValueAttrib, iupListSetIdValueAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", motListGetValueAttrib, motListSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VISIBLE_ITEMS", NULL, motListSetVisibleItemsAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "TOPITEM", NULL, motListSetTopItemAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWDROPDOWN", NULL, motListSetShowDropdownAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPACING", iupListGetSpacingAttrib, motListSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);

  iupClassRegisterAttribute(ic, "PADDING", iupListGetPaddingAttrib, motListSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SELECTEDTEXT", motListGetSelectedTextAttrib, motListSetSelectedTextAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", motListGetSelectionAttrib, motListSetSelectionAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTIONPOS", motListGetSelectionPosAttrib, motListSetSelectionPosAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARET", motListGetCaretAttrib, motListSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CARETPOS", motListGetCaretPosAttrib, motListSetCaretPosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INSERT", NULL, motListSetInsertAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPEND", NULL, motListSetAppendAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "READONLY", motListGetReadOnlyAttrib, motListSetReadOnlyAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "NC", iupListGetNCAttrib, motListSetNCAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPBOARD", NULL, motListSetClipboardAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTO", NULL, motListSetScrollToAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SCROLLTOPOS", NULL, motListSetScrollToPosAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* Not Supported */
  iupClassRegisterAttributeId(ic, "IMAGE", NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWIMAGE", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DROPEXPAND", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AUTOREDRAW", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}
