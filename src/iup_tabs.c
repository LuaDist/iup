/** \file
* \brief iuptabs control
*
* See Copyright Notice in "iup.h"
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_image.h"
#include "iup_tabs.h"



char* iupTabsGetPaddingAttrib(Ihandle* ih)
{
  char *str = iupStrGetMemory(50);
  sprintf(str, "%dx%d", ih->data->horiz_padding, ih->data->vert_padding);
  return str;
}

char* iupTabsAttribGetStrId(Ihandle* ih, const char* name, int pos)
{
  char str[50];
  sprintf(str, "%s%d", name, pos);
  return iupAttribGet(ih, str);
}

void iupTabsAttribSetStrId(Ihandle* ih, const char* name, int pos, const char* value)
{
  char str[50];
  sprintf(str, "%s%d", name, pos);
  iupAttribStoreStr(ih, str, value);
}

static int iTabsGetMaxWidth(Ihandle* ih)
{
  int max_width = 0, width, pos;
  char *tabtitle, *tabimage;
  Ihandle* child;

  for (pos = 0, child = ih->firstchild; child; child = child->brother, pos++)
  {
    tabtitle = iupTabsAttribGetStrId(ih, "TABTITLE", pos);
    if (!tabtitle) tabtitle = iupAttribGet(child, "TABTITLE");
    tabimage = iupTabsAttribGetStrId(ih, "TABIMAGE", pos);
    if (!tabimage) tabimage = iupAttribGet(child, "TABIMAGE");
    if (!tabtitle && !tabimage)
      tabtitle = "     ";

    width = 0;
    if (tabtitle)
      width += iupdrvFontGetStringWidth(ih, tabtitle);

    if (tabimage)
    {
      void* img = iupImageGetImage(tabimage, ih, 0);
      if (img)
      {
        int w;
        iupdrvImageGetInfo(img, &w, NULL, NULL);
        width += w;
      }
    }

    if (width > max_width) max_width = width;
  }

  return max_width;
}

static int iTabsGetMaxHeight(Ihandle* ih)
{
  int max_height = 0, h, pos;
  char *tabimage;
  Ihandle* child;

  for (pos = 0, child = ih->firstchild; child; child = child->brother, pos++)
  {
    tabimage = iupTabsAttribGetStrId(ih, "TABIMAGE", pos);
    if (!tabimage) tabimage = iupAttribGet(child, "TABIMAGE");

    if (tabimage)
    {
      void* img = iupImageGetImage(tabimage, ih, 0);
      if (img)
      {
        iupdrvImageGetInfo(img, NULL, &h, NULL);
        if (h > max_height) max_height = h;
      }
    }
  }

  iupdrvFontGetCharSize(ih, NULL, &h);
  if (h > max_height) max_height = h;

  return max_height;
}

static void iTabsGetDecorSize(Ihandle* ih, int *width, int *height)
{
  if (ih->data->type == ITABS_LEFT || ih->data->type == ITABS_RIGHT)
  {
    if (ih->data->orientation == ITABS_HORIZONTAL)
    {
      int max_width = iTabsGetMaxWidth(ih);
      *width  = 4 + (3 + max_width + 3) + 2 + 4;
      *height = 4 + 4;

      if (iupdrvTabsExtraDecor(ih))
      {
        int h;
        iupdrvFontGetCharSize(ih, NULL, &h);
        *height += h + 4;
      }
    }
    else
    {
      int max_height = iTabsGetMaxHeight(ih);
      *width  = 4 + (3 + max_height + 3) + 2 + 4;
      *height = 4 + 4;

      if (ih->handle && ih->data->is_multiline)
      {
        int num_lin = iupdrvTabsGetLineCountAttrib(ih);
        *width += (num_lin-1)*(3 + max_height + 3 + 1);
      }
    }
  }
  else /* "BOTTOM" or "TOP" */
  {
    if (ih->data->orientation == ITABS_HORIZONTAL)
    {
      int max_height = iTabsGetMaxHeight(ih);
      *width  = 4 + 4;
      *height = 4 + (3 + max_height + 3) + 2 + 4;

      if (ih->handle && ih->data->is_multiline)
      {
        int num_lin = iupdrvTabsGetLineCountAttrib(ih);
        *height += (num_lin-1)*(3 + max_height + 3 + 1);
      }

      if (iupdrvTabsExtraDecor(ih))
      {
        int h;
        iupdrvFontGetCharSize(ih, NULL, &h);
        *width += h + 4;
      }
    }
    else
    {
      int max_width = iTabsGetMaxWidth(ih);
      *width  = 4 + 4;
      *height = 4 + (3 + max_width + 3) + 2 + 4;
    }
  }

  *width  += ih->data->horiz_padding;
  *height += ih->data->vert_padding;
}

static void iTabsGetDecorOffset(Ihandle* ih, int *dx, int *dy)
{
  if (ih->data->type == ITABS_LEFT || ih->data->type == ITABS_RIGHT)
  {
    if (ih->data->type == ITABS_LEFT)
    {
      if (ih->data->orientation == ITABS_HORIZONTAL)
      {
        int max_width = iTabsGetMaxWidth(ih);
        *dx = 4 + (3 + max_width + 3) + 2;
      }
      else
      {
        int max_height = iTabsGetMaxHeight(ih);
        *dx = 4 + (3 + max_height + 3) + 2;

        if (ih->handle && ih->data->is_multiline)
        {
          int num_lin = iupdrvTabsGetLineCountAttrib(ih);
          *dx += (num_lin-1)*(3 + max_height + 3 + 1);
        }
      }
    }
    else
      *dx = 4;

    *dy = 4;
  }
  else /* "BOTTOM" or "TOP" */
  {
    if (ih->data->type == ITABS_TOP)
    {
      if (ih->data->orientation == ITABS_HORIZONTAL)
      {
        int max_height = iTabsGetMaxHeight(ih);
        *dy = 4 + (3 + max_height + 3) + 2;

        if (ih->handle && ih->data->is_multiline)
        {
          int num_lin = iupdrvTabsGetLineCountAttrib(ih);
          *dy += (num_lin-1)*(3 + max_height + 3 + 1);
        }
      }
      else
      {
        int max_width = iTabsGetMaxWidth(ih);
        *dy = 4 + (3 + max_width + 3) + 2;
      }
    }
    else
      *dy = 4;

    *dx = 4;
  }

  *dx += ih->data->horiz_padding;
  *dy += ih->data->vert_padding;
}

static void iTabsSetTab(Ihandle* ih, Ihandle* child, int pos)
{
  if (ih->handle)
  {
    int cur_pos = iupdrvTabsGetCurrentTab(ih);
    if (cur_pos != pos)
      iupdrvTabsSetCurrentTab(ih, pos);
  }
  else
    iupAttribSetStr(ih, "_IUPTABS_VALUE_HANDLE", (char*)child);
}


/* ------------------------------------------------------------------------- */
/* TABS - Sets and Gets - Accessors                                          */
/* ------------------------------------------------------------------------- */

static int iTabsSetValueHandleAttrib(Ihandle* ih, const char* value)
{
  int pos;
  Ihandle *child;

  child = (Ihandle*)value;

  if (!iupObjectCheck(child))
    return 0;

  pos = IupGetChildPos(ih, child);
  if (pos != -1) /* found child */
    iTabsSetTab(ih, child, pos);
 
  return 0;
}

char* iupTabsGetTabTypeAttrib(Ihandle* ih)
{
  switch(ih->data->type)
  {
  case ITABS_BOTTOM:
    return "BOTTOM";
  case ITABS_LEFT:
    return "LEFT";
  case ITABS_RIGHT:
    return "RIGHT";
  default:
    return "TOP";
  }
}

char* iupTabsGetTabOrientationAttrib(Ihandle* ih)
{
  if (ih->data->orientation == ITABS_HORIZONTAL)
    return "HORIZONTAL";
  else
    return "VERTICAL";
}

static char* iTabsGetValueHandleAttrib(Ihandle* ih)
{
  if (ih->handle)
  {
    int pos = iupdrvTabsGetCurrentTab(ih);
    return (char*)IupGetChild(ih, pos);
  }
  else
    return iupAttribGet(ih, "_IUPTABS_VALUE_HANDLE");
}

static char* iTabsGetCountAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(50);
  sprintf(str, "%d", IupGetChildCount(ih));
  return str;
}

static int iTabsSetValuePosAttrib(Ihandle* ih, const char* value)
{
  Ihandle* child;
  int pos;

  if (!iupStrToInt(value, &pos))
    return 0;

  child = IupGetChild(ih, pos);
  if (child) /* found child */
    iTabsSetTab(ih, child, pos);
 
  return 0;
}

static char* iTabsGetValuePosAttrib(Ihandle* ih)
{
  if (ih->handle)
  {
    int pos = iupdrvTabsGetCurrentTab(ih);
    char *str = iupStrGetMemory(50);
    sprintf(str, "%d", pos);
    return str;
  }
  else
  {
    Ihandle* child = (Ihandle*)iupAttribGet(ih, "_IUPTABS_VALUE_HANDLE");
    int pos = IupGetChildPos(ih, child);
    if (pos != -1) /* found child */
    {
      char *str = iupStrGetMemory(50);
      sprintf(str, "%d", pos);
      return str;
    }
  }

  return NULL;
}

static int iTabsSetValueAttrib(Ihandle* ih, const char* value)
{
  Ihandle *child;

  if (!value)
    return 0;

  child = IupGetHandle(value);
  if (!child)
    return 0;

  iTabsSetValueHandleAttrib(ih, (char*)child);

  return 0;
}

static char* iTabsGetValueAttrib(Ihandle* ih)
{
  Ihandle* child = (Ihandle*)iTabsGetValueHandleAttrib(ih);
  return IupGetName(child);
}

static char* iTabsGetClientSizeAttrib(Ihandle* ih)
{
  int width, height, decorwidth, decorheight;
  char* str = iupStrGetMemory(20);
  width = ih->currentwidth;
  height = ih->currentheight;
  iTabsGetDecorSize(ih, &decorwidth, &decorheight);
  width -= decorwidth;
  height -= decorheight;
  if (width < 0) width = 0;
  if (height < 0) height = 0;
  sprintf(str, "%dx%d", width, height);
  return str;
}

static char* iTabsGetClientOffsetAttrib(Ihandle* ih)
{
  int dx, dy;
  char* str = iupStrGetMemory(20);
  iTabsGetDecorOffset(ih, &dx, &dy);
  sprintf(str, "%dx%d", dx, dy);
  return str;
}

void iupTabsTestRemoveTab(Ihandle* ih, int pos)
{
  int cur_pos = iupdrvTabsGetCurrentTab(ih);
  if (cur_pos == pos)
  {
    if (cur_pos == 0)
    {
      Ihandle* child = IupGetChild(ih, 1);
      if (!child) /* not found child, means only one child, do nothing */
        return;

      cur_pos = 1;
    }
    else
      cur_pos--;

    iupdrvTabsSetCurrentTab(ih, cur_pos);
  }
}

/* ------------------------------------------------------------------------- */
/* TABS - Methods                                                            */
/* ------------------------------------------------------------------------- */

static void iTabsComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  Ihandle* child;
  int children_expand, 
      children_naturalwidth, children_naturalheight;
  int decorwidth, decorheight;

  /* calculate total children natural size (even for hidden children) */
  children_expand = 0;
  children_naturalwidth = 0;
  children_naturalheight = 0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(child);

    children_expand |= child->expand;
    children_naturalwidth = iupMAX(children_naturalwidth, child->naturalwidth);
    children_naturalheight = iupMAX(children_naturalheight, child->naturalheight);
  }

  iTabsGetDecorSize(ih, &decorwidth, &decorheight);

  *expand = children_expand;
  *w = children_naturalwidth + decorwidth;
  *h = children_naturalheight + decorheight;
}

static void iTabsSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle* child;
  int width, height, decorwidth, decorheight;

  iTabsGetDecorSize(ih, &decorwidth, &decorheight);

  width = ih->currentwidth-decorwidth;
  height = ih->currentheight-decorheight;
  if (width < 0) width = 0;
  if (height < 0) height = 0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    iupBaseSetCurrentSize(child, width, height, shrink);
  }
}

static void iTabsSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  /* IupTabs is the native parent of its children,
     so the position is restarted at (0,0).
     In all systems, each tab is a native window covering the client area.
     Child coordinates are relative to client left-top corner of the tab page. */
  Ihandle* child;
  (void)x;
  (void)y;
  for (child = ih->firstchild; child; child = child->brother)
  {
    iupBaseSetPosition(child, 0, 0);
  }
}

static void* iTabsGetInnerNativeContainerHandleMethod(Ihandle* ih, Ihandle* child)
{
  while (child && child->parent != ih)
    child = child->parent;
  if (child)
    return iupAttribGet(child, "_IUPTAB_CONTAINER");
  else
    return NULL;
}

static int iTabsCreateMethod(Ihandle* ih, void **params)
{
  ih->data = iupALLOCCTRLDATA();

  /* add children */
  if(params)
  {
    Ihandle** iparams = (Ihandle**)params;
    while (*iparams) 
    {
      IupAppend(ih, *iparams);
      iparams++;
    }
  }
  return IUP_NOERROR;
}

Iclass* iupTabsNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "tabs";
  ic->format = "g"; /* array of Ihandle */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype  = IUP_CHILDMANY;
  ic->is_interactive = 1;
  ic->has_attrib_id = 1;

  /* Class functions */
  ic->New = iupTabsNewClass;
  ic->Create  = iTabsCreateMethod;
  ic->GetInnerNativeContainerHandle = iTabsGetInnerNativeContainerHandleMethod;

  ic->ComputeNaturalSize = iTabsComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize     = iTabsSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition        = iTabsSetChildrenPositionMethod;

  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* IupTabs Callbacks */
  iupClassRegisterCallback(ic, "TABCHANGE_CB", "nn");
  iupClassRegisterCallback(ic, "TABCHANGEPOS_CB", "ii");

  /* Common Callbacks */
  iupBaseRegisterCommonCallbacks(ic);

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* IupTabs only */
  iupClassRegisterAttribute(ic, "VALUE", iTabsGetValueAttrib, iTabsSetValueAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUEPOS", iTabsGetValuePosAttrib, iTabsSetValuePosAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE_HANDLE", iTabsGetValueHandleAttrib, iTabsSetValueHandleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "COUNT", iTabsGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* Base Container */
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iTabsGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iTabsGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupdrvTabsInitClass(ic);

  return ic;
}

Ihandle* IupTabs(Ihandle* first,...)
{
  Ihandle **children;
  Ihandle *ih;

  va_list arglist;
  va_start(arglist, first);
  children = (Ihandle**)iupObjectGetParamList(first, arglist);
  va_end(arglist);

  ih = IupCreatev("tabs", (void**)children);
  free(children);

  return ih;
}

Ihandle* IupTabsv(Ihandle** params)
{
  return IupCreatev("tabs", (void**)params);
}
