/** \file
 * \brief List Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_assert.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_mask.h"
#include "iup_image.h"
#include "iup_list.h"


void iupListSingleCallDblClickCallback(Ihandle* ih, IFnis cb, int pos)
{
  char *text;
  char str[30];

  if (pos<=0)
    return;

  sprintf(str, "%d", pos);
  text = IupGetAttribute(ih, str);

  if (cb(ih, pos, text) == IUP_CLOSE)
    IupExitLoop();
}

static void iListCallActionCallback(Ihandle* ih, IFnsii cb, int pos, int state)
{
  char *text;
  char str[30];

  if (pos<=0)
    return;

  sprintf(str, "%d", pos);
  text = IupGetAttribute(ih, str);

  if (cb(ih, text, pos, state) == IUP_CLOSE)
    IupExitLoop();
}

void iupListUpdateOldValue(Ihandle* ih, int pos, int removed)
{
  if (!ih->data->has_editbox)
  {
    char* old_value = iupAttribGet(ih, "_IUPLIST_OLDVALUE");
    if (old_value)
    {
      int old_pos = atoi(old_value)-1; /* was in IUP reference, starting at 1 */
      if (ih->data->is_dropdown || !ih->data->is_multiple)
      {
        if (old_pos >= pos)
        {
          if (removed && old_pos == pos)
          {
            /* when the current item is removed nothing remains selected */
            iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
          }
          else
            iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", removed? old_pos-1: old_pos+1);
        }
      }
      else
      {
        /* multiple selection on a non drop-down list. */
        char* value = IupGetAttribute(ih, "VALUE");
        iupAttribStoreStr(ih, "_IUPLIST_OLDVALUE", value);
      }
    }
  }
}

void iupListSingleCallActionCallback(Ihandle* ih, IFnsii cb, int pos)
{
  char* old_str = iupAttribGet(ih, "_IUPLIST_OLDVALUE");
  if (old_str)
  {
    int oldpos = atoi(old_str);
    if (oldpos != pos)
    {
      iListCallActionCallback(ih, cb, oldpos, 0);
      iListCallActionCallback(ih, cb, pos, 1);
    }
  }
  else
    iListCallActionCallback(ih, cb, pos, 1);
  iupAttribSetInt(ih, "_IUPLIST_OLDVALUE", pos);
}

void iupListMultipleCallActionCallback(Ihandle* ih, IFnsii cb, IFns multi_cb, int* pos, int sel_count)
{
  int i, count = iupdrvListGetCount(ih);

  char* old_str = iupAttribGet(ih, "_IUPLIST_OLDVALUE");
  int old_count = old_str? strlen(old_str): 0;

  char* str = malloc(count+1);
  memset(str, '-', count);
  str[count]=0;
  for (i=0; i<sel_count; i++)
    str[pos[i]] = '+';

  if (old_count != count)
  {
    old_count = 0;
    old_str = NULL;
  }

  if (multi_cb)
  {
    int unchanged = 1;

    for (i=0; i<count && old_str; i++)
    {
      if (str[i] == old_str[i])
        str[i] = 'x';    /* mark unchanged values */
      else
        unchanged = 0;
    }

    if (old_str && unchanged)
    {
      free(str);
      return;
    }

    if (multi_cb(ih, str) == IUP_CLOSE)
      IupExitLoop();

    for (i=0; i<count && old_str; i++)
    {
      if (str[i] == 'x')
        str[i] = old_str[i];    /* restore unchanged values */
    }
  }
  else
  {
    /* must simulate the click on each item */
    for (i=0; i<count; i++)
    {
      if (i >= old_count)  /* new items, if selected then call the callback */
      {
        if (str[i] == '+')
          iListCallActionCallback(ih, cb, i+1, 1);
      }
      else if (str[i] != old_str[i])
      {
        if (str[i] == '+')
          iListCallActionCallback(ih, cb, i+1, 1);
        else
          iListCallActionCallback(ih, cb, i+1, 0);
      }
    }
  }

  iupAttribStoreStr(ih, "_IUPLIST_OLDVALUE", str);
  free(str);
}

int iupListGetPos(Ihandle* ih, int pos)
{
  int count;

  pos--; /* IUP items start at 1 */

  if (pos < 0) 
    return -1;

  count = iupdrvListGetCount(ih);

  if (pos == count) return -2;
  if (pos > count) return -1;

  return pos;
}

void iupListSetInitialItems(Ihandle* ih)
{
  char str[20], *value;
  int i = 1;
  sprintf(str, "%d", i);
  while ((value = iupAttribGet(ih, str))!=NULL)
  {
    iupdrvListAppendItem(ih, value);
    iupAttribSetStr(ih, str, NULL);

    i++;
    sprintf(str, "%d", i);
  }
}

char* iupListGetSpacingAttrib(Ihandle* ih)
{
  if (!ih->data->is_dropdown)
  {
    char *str = iupStrGetMemory(50);
    sprintf(str, "%d", ih->data->spacing);
    return str;
  }
  else
    return NULL;
}

char* iupListGetPaddingAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    char *str = iupStrGetMemory(50);
    sprintf(str, "%dx%d", ih->data->horiz_padding, ih->data->vert_padding);
    return str;
  }
  else
    return NULL;
}

char* iupListGetNCAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
  {
    char* str = iupStrGetMemory(100);
    sprintf(str, "%d", ih->data->nc);
    return str;
  }
  else
    return NULL;
}

int iupListSetIdValueAttrib(Ihandle* ih, int pos, const char* value)
{
  int count = iupdrvListGetCount(ih);

  pos--; /* IUP starts at 1 */

  if (!value)
  {
    if (pos >= 0 && pos <= count-1)
    {
      if (pos == 0)
      {
        iupdrvListRemoveAllItems(ih);
        iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
      }
      else
      {
        int i = pos;
        while (i < count)
        {
          iupdrvListRemoveItem(ih, pos);
          i++;
        }
      }
    }
  }
  else
  {
    if (pos >= 0 && pos <= count-1)
    {
      iupdrvListRemoveItem(ih, pos);
      iupdrvListInsertItem(ih, pos, value);
    }
    else if (pos == count)
      iupdrvListAppendItem(ih, value);
  }
  return 1;
}

static int iListSetAppendItemAttrib(Ihandle* ih, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  if (value)
    iupdrvListAppendItem(ih, value);
  return 0;
}

static int iListSetInsertItemAttrib(Ihandle* ih, int id, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  if (value)
  {
    int pos = iupListGetPos(ih, id);
    if (pos >= 0)
      iupdrvListInsertItem(ih, pos, value);
    else if (pos == -2)
      iupdrvListAppendItem(ih, value);
  }
  return 0;
}

static int iListSetRemoveItemAttrib(Ihandle* ih, const char* value)
{
  if (!ih->handle)  /* do not do the action before map */
    return 0;
  if (!value || iupStrEqualNoCase(value, "ALL"))
  {
    iupdrvListRemoveAllItems(ih);
    iupAttribSetStr(ih, "_IUPLIST_OLDVALUE", NULL);
  }
  else
  {
    int id;
    if (iupStrToInt(value, &id))
    {
      int pos = iupListGetPos(ih, id);
      if (pos >= 0)
        iupdrvListRemoveItem(ih, pos);
    }
  }
  return 0;
}

static int iListGetCount(Ihandle* ih)
{
  int count;
  if (ih->handle)
    count = iupdrvListGetCount(ih);
  else
  {
    char str[20];
    count = 0;
    sprintf(str, "%d", count+1);
    while (iupAttribGet(ih, str))
    {
      count++;
      sprintf(str, "%d", count+1);
    }
  }
  return count;
}

static char* iListGetCountAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(50);
  sprintf(str, "%d", iListGetCount(ih));
  return str;
}

static int iListSetDropdownAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (iupStrBoolean(value))
  {
    ih->data->is_dropdown = 1;
    ih->data->is_multiple = 0;
  }
  else
    ih->data->is_dropdown = 0;

  return 0;
}

static char* iListGetDropdownAttrib(Ihandle* ih)
{
  if (ih->data->is_dropdown)
    return "YES";
  else
    return "NO";
}

static int iListSetMultipleAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (iupStrBoolean(value))
  {
    ih->data->is_multiple = 1;
    ih->data->is_dropdown = 0;
    ih->data->has_editbox = 0;
  }
  else
    ih->data->is_multiple = 0;

  return 0;
}

static char* iListGetMultipleAttrib(Ihandle* ih)
{
  if (ih->data->is_multiple)
    return "YES";
  else
    return "NO";
}

static int iListSetEditboxAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (iupStrBoolean(value))
  {
    ih->data->has_editbox = 1;
    ih->data->is_multiple = 0;
  }
  else
    ih->data->has_editbox = 0;

  return 0;
}

static char* iListGetEditboxAttrib(Ihandle* ih)
{
  if (ih->data->has_editbox)
    return "YES";
  else
    return "NO";
}

static int iListSetScrollbarAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  else if (iupStrBoolean(value))
    ih->data->sb = 1;
  else
    ih->data->sb = 0;

  return 0;
}

static char* iListGetScrollbarAttrib(Ihandle* ih)
{
  if (ih->data->sb)
    return "YES";
  else
    return "NO";
}

static char* iListGetMaskDataAttrib(Ihandle* ih)
{
  if (!ih->data->has_editbox)
    return NULL;

  /* Used only by the OLD iupmask API */
  return (char*)ih->data->mask;
}

static int iListSetMaskAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
  {
    if (ih->data->mask)
      iupMaskDestroy(ih->data->mask);
  }
  else
  {
    int casei = iupAttribGetInt(ih, "MASKCASEI");
    Imask* mask = iupMaskCreate(value,casei);
    if (mask)
    {
      if (ih->data->mask)
        iupMaskDestroy(ih->data->mask);

      ih->data->mask = mask;
      return 1;
    }
  }

  return 0;
}

static int iListSetMaskIntAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
  {
    if (ih->data->mask)
      iupMaskDestroy(ih->data->mask);

    iupAttribSetStr(ih, "MASK", NULL);
  }
  else
  {
    Imask* mask;
    int min, max;

    if (iupStrToIntInt(value, &min, &max, ':')!=2)
      return 0;

    mask = iupMaskCreateInt(min,max);

    if (ih->data->mask)
      iupMaskDestroy(ih->data->mask);

    ih->data->mask = mask;

    if (min < 0)
      iupAttribSetStr(ih, "MASK", IUP_MASK_INT);
    else
      iupAttribSetStr(ih, "MASK", IUP_MASK_UINT);
  }

  return 0;
}

static int iListSetMaskFloatAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->has_editbox)
    return 0;

  if (!value)
  {
    if (ih->data->mask)
      iupMaskDestroy(ih->data->mask);

    iupAttribSetStr(ih, "MASK", NULL);
  }
  else
  {
    Imask* mask;
    float min, max;

    if (iupStrToFloatFloat(value, &min, &max, ':')!=2)
      return 0;

    mask = iupMaskCreateFloat(min,max);

    if (ih->data->mask)
      iupMaskDestroy(ih->data->mask);

    ih->data->mask = mask;

    if (min < 0)
      iupAttribSetStr(ih, "MASK", IUP_MASK_FLOAT);
    else
      iupAttribSetStr(ih, "MASK", IUP_MASK_UFLOAT);
  }

  return 0;
}

static int iListSetShowImageAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (iupStrBoolean(value))
    ih->data->show_image = 1;
  else
    ih->data->show_image = 0;

  return 0;
}

static char* iListGetShowImageAttrib(Ihandle* ih)
{
  if (ih->data->show_image)
    return "YES";
  else
    return "NO";
}


/*****************************************************************************************/


static int iListCreateMethod(Ihandle* ih, void** params)
{
  if (params && params[0])
    iupAttribStoreStr(ih, "ACTION", (char*)(params[0]));

  ih->data = iupALLOCCTRLDATA();
  ih->data->sb = 1;

  return IUP_NOERROR;
}

static void iListGetItemImageInfo(Ihandle *ih, int id, int *img_w, int *img_h)
{
  char str[20];
  char *value;

  *img_w = 0;
  *img_h = 0;

  sprintf(str, "IMAGE%d", id);
  value = iupAttribGet(ih, str);
  if (value)
    iupImageGetInfo(value, img_w, img_h, NULL);
}

static void iListGetNaturalItemsSize(Ihandle *ih, int *w, int *h)
{
  char *value;
  int max_w = 0, max_h = 0;
  int visiblecolumns, i, 
      count = iListGetCount(ih);

  *w = 0;
  *h = 0;

  iupdrvFontGetCharSize(ih, w, h);   /* one line height, and one character width */

  visiblecolumns = iupAttribGetInt(ih, "VISIBLECOLUMNS");
  if (visiblecolumns)
  {
    *w = iupdrvFontGetStringWidth(ih, "WWWWWWWWWW");
    *w = (visiblecolumns*(*w))/10;
  }
  else
  {
    int item_w;

    for (i=1; i<=count; i++)
    {
      item_w = 0;

      value = IupGetAttributeId(ih, "", i);  /* must use IupGetAttribute to check the native system */
      if (value)
        item_w = iupdrvFontGetStringWidth(ih, value);

      if (item_w > *w)
        *w = item_w;
    }

    if (*w == 0) /* default is 5 characters in 1 item */
      *w = iupdrvFontGetStringWidth(ih, "WWWWW");
  }

  if (ih->data->show_image)
  {
    for (i=1; i<=count; i++)
    {
      int img_w, img_h;
      iListGetItemImageInfo(ih, i, &img_w, &img_h);
      if (img_w > max_w)
        max_w = img_w;
      if (img_h > max_h)
        max_h = img_h;
    }

    /* Used only in Windows */
    ih->data->maximg_w = max_w;
    ih->data->maximg_h = max_h;

    *w += max_w;
  }

  /* compute height for multiple lines, dropdown is just 1 line */
  if (!ih->data->is_dropdown)
  {
    int visiblelines, num_lines, 
        edit_line_size = *h;  /* don't include the highest image */

    if (ih->data->show_image && max_h > *h)  /* use the highest image to compute the natural size */
      *h = max_h;

    iupdrvListAddItemSpace(ih, h);  /* this independs from spacing */

    *h += 2*ih->data->spacing;  /* this will be multiplied by the number of lines */
    *w += 2*ih->data->spacing;  /* include also horizontal spacing */

    num_lines = count;
    if (num_lines == 0) num_lines = 1;

    visiblelines = iupAttribGetInt(ih, "VISIBLELINES");
    if (visiblelines)
      num_lines = visiblelines;   

    *h = *h * num_lines;

    if (ih->data->has_editbox) 
      *h += edit_line_size;
  }
  else
  {
    if (!ih->data->has_editbox)
    {
      if (ih->data->show_image && max_h > *h)  /* use the highest image to compute the natural size */
        *h = max_h;
    }
  }
}

static void iListComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int natural_w, natural_h;
  int sb_size = iupdrvGetScrollbarSize();
  (void)expand; /* unset if not a container */

  iListGetNaturalItemsSize(ih, &natural_w, &natural_h);

  /* compute the borders space */
  iupdrvListAddBorders(ih, &natural_w, &natural_h);

  if (ih->data->is_dropdown)
  {
    /* add room for dropdown box */
    natural_w += sb_size;

    if (natural_h < sb_size)
      natural_h = sb_size;
  }
  else
  {
    /* add room for scrollbar */
    if (ih->data->sb)
    {
      natural_h += sb_size;
      natural_w += sb_size;
    }
  }

  if (ih->data->has_editbox)
  {
    natural_w += 2*ih->data->horiz_padding;
    natural_h += 2*ih->data->vert_padding;
  }

  *w = natural_w;
  *h = natural_h;
}

static void iListDestroyMethod(Ihandle* ih)
{
  if (ih->data->mask)
    iupMaskDestroy(ih->data->mask);
}


/******************************************************************************/


Ihandle* IupList(const char* action)
{
  void *params[2];
  params[0] = (void*)action;
  params[1] = NULL;
  return IupCreatev("list", params);
}

Iclass* iupListNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "list";
  ic->format = "a"; /* one ACTION callback name */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;
  ic->has_attrib_id = 1;

  /* Class functions */
  ic->New = iupListNewClass;
  ic->Create = iListCreateMethod;
  ic->Destroy = iListDestroyMethod;
  ic->ComputeNaturalSize = iListComputeNaturalSizeMethod;

  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "ACTION", "sii");
  iupClassRegisterCallback(ic, "MULTISELECT_CB", "s");
  iupClassRegisterCallback(ic, "DROPFILES_CB", "siii");
  iupClassRegisterCallback(ic, "DROPDOWN_CB", "i");
  iupClassRegisterCallback(ic, "DBLCLICK_CB", "is");
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");
  iupClassRegisterCallback(ic, "MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "BUTTON_CB", "iiiis");

  iupClassRegisterCallback(ic, "EDIT_CB", "is");
  iupClassRegisterCallback(ic, "CARET_CB", "iii");

  /* Common Callbacks */
  iupBaseRegisterCommonCallbacks(ic);

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* Drag&Drop */
  iupdrvRegisterDragDropAttrib(ic);

  /* IupList only */
  iupClassRegisterAttribute(ic, "SCROLLBAR", iListGetScrollbarAttrib, iListSetScrollbarAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTIPLE", iListGetMultipleAttrib, iListSetMultipleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DROPDOWN", iListGetDropdownAttrib, iListSetDropdownAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EDITBOX", iListGetEditboxAttrib, iListSetEditboxAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", iListGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "INSERTITEM", NULL, iListSetInsertItemAttrib, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPENDITEM", NULL, iListSetAppendItemAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REMOVEITEM", NULL, iListSetRemoveItemAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AUTOHIDE", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "MASKCASEI", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MASK", NULL, iListSetMaskAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MASKINT", NULL, iListSetMaskIntAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MASKFLOAT", NULL, iListSetMaskFloatAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OLD_MASK_DATA", iListGetMaskDataAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "VISIBLECOLUMNS", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VISIBLELINES", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SHOWIMAGE", iListGetShowImageAttrib, iListSetShowImageAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupdrvListInitClass(ic);

  return ic;
}
