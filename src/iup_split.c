/** \file
 * \brief iupsplit control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_childtree.h"
#include "iup_draw.h"


enum { ISPLIT_VERT, ISPLIT_HORIZ };

struct _IcontrolData
{
  /* aux */
  int is_holding;
  int start_pos, start_bar, start_size;

  /* attributes */
  int layoutdrag, autohide, showgrip, barsize;
  int orientation;  /* one of the types: ISPLIT_VERT, ISPLIT_HORIZ */
  int val;  /* split value: 0-1000, default 500 */
  int min, max;  /* used only to crop val */
};


static void iSplitCropVal(Ihandle* ih)
{
  if (ih->data->val < ih->data->min) ih->data->val = ih->data->min;
  if (ih->data->val > ih->data->max) ih->data->val = ih->data->max;
}

static int iSplitGetWidth1(Ihandle* ih)
{
  Ihandle *child1;

  int width1 = ((ih->currentwidth-ih->data->barsize)*ih->data->val)/1000;
  if (width1 < 0) width1 = 0;

  child1 = ih->firstchild->brother;
  if (child1)
  {
    Ihandle *child2 = child1->brother;
    if (child2)
    {
      int width2 = (ih->currentwidth-ih->data->barsize)-width1;
      if (width2 < 0) width2 = 0;

      iupLayoutSetMinMaxSize(child2, &width2, NULL);

      /* only check if child2 has a minsize */
      if (width2 > (ih->currentwidth-ih->data->barsize)-width1)
        width1 = (ih->currentwidth-ih->data->barsize)-width2;
    }

    iupLayoutSetMinMaxSize(child1, &width1, NULL);
  }

  return width1;
}

static int iSplitGetHeight1(Ihandle* ih)
{
  Ihandle *child1;

  int height1 = ((ih->currentheight-ih->data->barsize)*ih->data->val)/1000;
  if (height1 < 0) height1 = 0;

  child1 = ih->firstchild->brother;
  if (child1)
  {
    Ihandle *child2 = child1->brother;
    if (child2)
    {
      int height2 = (ih->currentheight-ih->data->barsize)-height1;
      if (height2 < 0) height2 = 0;

      iupLayoutSetMinMaxSize(child2, &height2, NULL);

      /* only check if child2 has a minsize */
      if (height2 > (ih->currentheight-ih->data->barsize)-height1)
        height1 = (ih->currentheight-ih->data->barsize)-height2;
    }

    iupLayoutSetMinMaxSize(child1, NULL, &height1);
  }

  return height1;
}

static void iSplitSetBarPosition(Ihandle* ih, int cur_x, int cur_y, int update)
{
  if (ih->data->orientation == ISPLIT_VERT)
  {
    ih->firstchild->x = ih->data->start_bar + (cur_x - ih->data->start_pos);
    if (ih->firstchild->x < ih->x)
      ih->firstchild->x = ih->x;
    if (ih->firstchild->x > ih->x+ih->currentwidth)
      ih->firstchild->x = ih->x+ih->currentwidth;
  }
  else /* ISPLIT_HORIZ */
  {
    ih->firstchild->y = ih->data->start_bar + (cur_y - ih->data->start_pos);
    if (ih->firstchild->y < ih->y)
      ih->firstchild->y = ih->y;
    if (ih->firstchild->y > ih->y+ih->currentheight)
      ih->firstchild->y = ih->y+ih->currentheight;
  }

  if (update)
  {
    IupSetAttribute(ih->firstchild, "ZORDER", "TOP");
    iupClassObjectLayoutUpdate(ih->firstchild);
  }
}

static void iSplitShowHide(Ihandle* child, int hide)
{
  if (hide)
  {
    IupSetAttribute(child, "FLOATING", "IGNORE");
    IupSetAttribute(child, "VISIBLE", "NO");
  }
  else if (!IupGetInt(child, "VISIBLE"))
  {
    IupSetAttribute(child, "FLOATING", "NO");
    IupSetAttribute(child, "VISIBLE", "YES");
  }
}

static void iSplitAutoHideXY(Ihandle* ih)
{
  if (ih->data->orientation == ISPLIT_VERT)
  {
    Ihandle *child1 = ih->firstchild->brother;
    if (child1)
    {
      Ihandle *child2 = child1->brother;

      iSplitShowHide(child1, ih->firstchild->x < ih->x+ih->data->barsize);

      if (child2)
        iSplitShowHide(child2, ih->firstchild->x > ih->x+ih->currentwidth-ih->data->barsize);
    }
  }
  else /* ISPLIT_HORIZ */
  {
    Ihandle *child1 = ih->firstchild->brother;
    if (child1)
    {
      Ihandle *child2 = child1->brother;

      iSplitShowHide(child1, ih->firstchild->y < ih->y+ih->data->barsize);

      if (child2)
        iSplitShowHide(child2, ih->firstchild->y > ih->y+ih->currentheight-ih->data->barsize);
    }
  }
}

static void iSplitAutoHideVal(Ihandle* ih)
{
  Ihandle *child1 = ih->firstchild->brother;
  if (child1)
  {
    int tol;
    Ihandle *child2 = child1->brother;

    if (ih->data->orientation == ISPLIT_VERT)
    {
      if (ih->currentwidth <= ih->data->barsize)
        return;

      tol = (1000*ih->data->barsize)/ih->currentwidth;
    }
    else
    {
      if (ih->currentheight <= ih->data->barsize)
        return;

      tol = (1000*ih->data->barsize)/ih->currentheight;
    }

    iSplitShowHide(child1, ih->data->val<tol);

    if (child2)
      iSplitShowHide(child2, ih->data->val > 1000-tol);
  }
}


/*****************************************************************************\
|* Callbacks of canvas bar                                                   *|
\*****************************************************************************/


static int iSplitAction_CB(Ihandle* bar)
{
  Ihandle* ih = bar->parent;
  IdrawCanvas* dc = iupDrawCreateCanvas(bar);

  iupDrawParentBackground(dc);

  if (ih->data->showgrip)
  {
    int i, w, h, x, y, count;
    unsigned char r = 160, g = 160, b = 160, bg_r, bg_g, bg_b;
    iupDrawGetSize(dc, &w, &h);

    iupStrToRGB(IupGetAttribute(ih, "COLOR"), &r, &g, &b);
    if (r+g+b > 3*190)
      { bg_r = 100; bg_g = 100; bg_b = 100; }
    else
      { bg_r = 255; bg_g = 255; bg_b = 255; }

    if (ih->data->orientation == ISPLIT_VERT)
    {
      x = ih->data->barsize/2-1;
      y = 2;
      count = (h-2)/5;
    }
    else
    {
      x = 2;
      y = ih->data->barsize/2-1;
      count = (w-2)/5;
    }

    for (i = 0; i < count; i++)
    {
      iupDrawRectangle(dc, x+1, y+1, x+2, y+2, bg_r, bg_g, bg_b, IUP_DRAW_FILL);
      iupDrawRectangle(dc, x, y, x+1, y+1, r, g, b, IUP_DRAW_FILL);
      if (ih->data->orientation == ISPLIT_VERT)
        y += 5;
      else
        x += 5;
    }
  }

  iupDrawFlush(dc);

  iupDrawKillCanvas(dc);

  return IUP_DEFAULT;
}

static int iSplitMotion_CB(Ihandle* bar, int x, int y, char *status)
{
  Ihandle* ih = bar->parent;

  if (ih->data->is_holding)
  {
    if (iup_isbutton1(status))
    {
      int cur_x, cur_y;

      iupStrToIntInt(IupGetGlobal("CURSORPOS"), &cur_x, &cur_y, 'x');

      if (ih->data->orientation == ISPLIT_VERT)
      {
        int width1 = iSplitGetWidth1(ih);
        width1 = ih->data->start_size + (cur_x - ih->data->start_pos);
        ih->data->val = (width1*1000)/(ih->currentwidth-ih->data->barsize);
      }
      else
      {
        int height1 = iSplitGetHeight1(ih);
        height1 = ih->data->start_size + (cur_y - ih->data->start_pos);
        ih->data->val = (height1*1000)/(ih->currentheight-ih->data->barsize);
      }

      iSplitCropVal(ih);

      if (ih->data->layoutdrag)
      {
        if (ih->data->autohide)
        {
          iSplitSetBarPosition(ih, cur_x, cur_y, 0);
          iSplitAutoHideXY(ih);
        }

        IupRefreshChildren(ih);  
      }
      else
        iSplitSetBarPosition(ih, cur_x, cur_y, 1);
    }
    else
      ih->data->is_holding = 0;
  }

  (void)x;
  (void)y;
  return IUP_DEFAULT;
}

static int iSplitButton_CB(Ihandle* bar, int button, int pressed, int x, int y, char* status)
{
  Ihandle* ih = bar->parent;

  if (button!=IUP_BUTTON1)
    return IUP_DEFAULT;

  if (!ih->data->is_holding && pressed)
  {
    int cur_x, cur_y;

    ih->data->is_holding = 1;

    iupStrToIntInt(IupGetGlobal("CURSORPOS"), &cur_x, &cur_y, 'x');

    /* Save the cursor position and size */
    if (ih->data->orientation == ISPLIT_VERT)
    {
      ih->data->start_bar = ih->firstchild->x;
      ih->data->start_pos = cur_x;
      ih->data->start_size = iSplitGetWidth1(ih);
    }
    else
    {
      ih->data->start_bar = ih->firstchild->y;
      ih->data->start_pos = cur_y;
      ih->data->start_size = iSplitGetHeight1(ih);
    }
  }
  else if (ih->data->is_holding && !pressed)
  {
    ih->data->is_holding = 0;

    if (!ih->data->layoutdrag)
    {
      if (ih->data->autohide)
      {
        int cur_x, cur_y;
        iupStrToIntInt(IupGetGlobal("CURSORPOS"), &cur_x, &cur_y, 'x');
        iSplitSetBarPosition(ih, cur_x, cur_y, 0);
        iSplitAutoHideXY(ih);
      }

      IupRefreshChildren(ih);  
    }
  }

  (void)x;
  (void)y;
  (void)status;
  return IUP_DEFAULT;
}

static int iSplitFocus_CB(Ihandle* bar, int focus)
{
  Ihandle* ih = bar->parent;

  if (!ih || focus) /* use only kill focus */
    return IUP_DEFAULT;

  if (ih->data->is_holding)
    ih->data->is_holding = 0;

  return IUP_DEFAULT;
}


/*****************************************************************************\
|* Attributes                                                                *|
\*****************************************************************************/


static int iSplitSetColorAttrib(Ihandle* ih, const char* value)
{
  (void)value;
  iupdrvPostRedraw(ih);
  return 1;  /* store value in hash table */
}

static int iSplitSetOrientationAttrib(Ihandle* ih, const char* value)
{
  if (ih->handle) /* only before map */
    return 0;

  if (iupStrEqualNoCase(value, "HORIZONTAL"))
    ih->data->orientation = ISPLIT_HORIZ;
  else  /* Default = VERTICAL */
    ih->data->orientation = ISPLIT_VERT;

  if (ih->data->orientation == ISPLIT_VERT)
    IupSetAttribute(ih->firstchild, "CURSOR", "SPLITTER_VERT");
  else
    IupSetAttribute(ih->firstchild, "CURSOR", "SPLITTER_HORIZ");

  return 0;  /* do not store value in hash table */
}

static int iSplitSetValueAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->data->val = -1;  /* reset to calculate in Natural size */

    if (ih->data->autohide)
      iSplitAutoHideVal(ih);

    if (ih->handle)
      IupRefreshChildren(ih);  
  }
  else
  {
    int val;
    if (iupStrToInt(value, &val))
    {
      ih->data->val = val;
      iSplitCropVal(ih);

      if (ih->data->autohide)
        iSplitAutoHideVal(ih);

      if (ih->handle)
        IupRefreshChildren(ih);  
    }
  }

  return 0; /* do not store value in hash table */
}

static char* iSplitGetValueAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(30);
  sprintf(str, "%d", ih->data->val);
  return str;
}

static int iSplitSetBarSizeAttrib(Ihandle* ih, const char* value)
{
  if (iupStrToInt(value, &ih->data->barsize))
  {
    if (ih->data->autohide)
      iSplitAutoHideVal(ih);

    if (ih->handle)
      IupRefreshChildren(ih);  
  }
  return 0; /* do not store value in hash table */
}

static char* iSplitGetBarSizeAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(30);
  sprintf(str, "%d", ih->data->barsize);
  return str;
}

static int iSplitSetMinMaxAttrib(Ihandle* ih, const char* value)
{
  if (iupStrToIntInt(value, &ih->data->min, &ih->data->max, ':'))
  {
    if (ih->data->min > ih->data->max)
    {
      int t = ih->data->min;
      ih->data->min = ih->data->max;
      ih->data->max = t;
    }
    if (ih->data->min < 0) ih->data->min = 0;
    if (ih->data->max > 1000) ih->data->max = 1000;

    if (ih->data->val != -1)
      iSplitCropVal(ih);

    if (ih->data->autohide)
      iSplitAutoHideVal(ih);

    if (ih->handle)
      IupRefreshChildren(ih);  
  }
  return 0; /* do not store value in hash table */
}

static char* iSplitGetMinMaxAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(30);
  sprintf(str, "%d:%d", ih->data->min, ih->data->max);
  return str;
}

static int iSplitSetLayoutDragAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->layoutdrag = 1;
  else
    ih->data->layoutdrag = 0;

  return 0; /* do not store value in hash table */
}

static char* iSplitGetLayoutDragAttrib(Ihandle* ih)
{
  if (ih->data->layoutdrag)
    return "YES";
  else
    return "NO";
}

static int iSplitSetShowGripAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->showgrip = 1;
  else
  {
    ih->data->showgrip = 0;

    if (ih->data->barsize == 5)
      iSplitSetBarSizeAttrib(ih, "3");
  }

  return 0; /* do not store value in hash table */
}

static char* iSplitGetShowGripAttrib(Ihandle* ih)
{
  if (ih->data->showgrip)
    return "YES";
  else
    return "NO";
}

static int iSplitSetAutoHideAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
  {
    ih->data->autohide = 1;
    iSplitAutoHideVal(ih);
  }
  else
  {
    Ihandle *child1 = ih->firstchild->brother;
    if (child1)
    {
      Ihandle *child2 = child1->brother;
      iSplitShowHide(child1, 0);
      if (child2)
        iSplitShowHide(child2, 0);
    }

    ih->data->autohide = 0;
  }

  return 0; /* do not store value in hash table */
}

static char* iSplitGetAutoHideAttrib(Ihandle* ih)
{
  if (ih->data->autohide)
    return "YES";
  else
    return "NO";
}


/*****************************************************************************\
|* Methods                                                                   *|
\*****************************************************************************/


static void iSplitComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int natural_w = 0, 
      natural_h = 0;
  Ihandle *child1, *child2 = NULL;
  child1 = ih->firstchild->brother;
  if (child1)
    child2 = child1->brother;

  /* always has at least one child, the bar, not necessary to compute its natural size */
  if (ih->data->orientation == ISPLIT_VERT)
    natural_w += ih->data->barsize;
  else
    natural_h += ih->data->barsize;

  if (child1)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(child1);

    if (ih->data->orientation == ISPLIT_VERT)
    {
      natural_w += child1->naturalwidth;
      natural_h = iupMAX(natural_h, child1->naturalheight);
    }
    else
    {
      natural_w = iupMAX(natural_w, child1->naturalwidth);
      natural_h += child1->naturalheight;
    }

    *expand = child1->expand;

    if (child2)
    {
      /* update child natural size first */
      iupBaseComputeNaturalSize(child2);

      if (ih->data->orientation == ISPLIT_VERT)
      {
        natural_w += child2->naturalwidth;
        natural_h = iupMAX(natural_h, child2->naturalheight);
      }
      else
      {
        natural_w = iupMAX(natural_w, child2->naturalwidth);
        natural_h += child2->naturalheight;
      }

      *expand |= child2->expand;
    }
  }

  if (ih->data->val == -1)
  {
    if (child1)
    {
      if (ih->data->orientation == ISPLIT_VERT)
        ih->data->val = (child1->naturalwidth*1000)/(natural_w-ih->data->barsize);
      else
        ih->data->val = (child1->naturalheight*1000)/(natural_h-ih->data->barsize);

      iSplitCropVal(ih);
    }
    else
      ih->data->val = ih->data->min;
  }

  *w = natural_w;
  *h = natural_h;
}

static void iSplitSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle *child1, *child2 = NULL;
  child1 = ih->firstchild->brother;
  if (child1)
    child2 = child1->brother;

  if (ih->data->orientation == ISPLIT_VERT)
  {
    int width2 = 0;

    if (child1)
    {
      int width1 = iSplitGetWidth1(ih);
      iupBaseSetCurrentSize(child1, width1, ih->currentheight, shrink);

      width2 = (ih->currentwidth-ih->data->barsize)-width1;
      if (width2 < 0) width2 = 0;
    }

    /* bar */
    ih->firstchild->currentwidth  = ih->data->barsize;
    ih->firstchild->currentheight = ih->currentheight;

    if (child2)
      iupBaseSetCurrentSize(child2, width2, ih->currentheight, shrink);
  }
  else /* ISPLIT_HORIZ */
  {
    int height2 = 0;

    if (child1)
    {
      int height1 = iSplitGetHeight1(ih);
      iupBaseSetCurrentSize(child1, ih->currentwidth, height1, shrink);

      height2 = (ih->currentheight-ih->data->barsize)-height1;
      if (height2 < 0) height2 = 0;
    }

    /* bar */
    ih->firstchild->currentwidth  = ih->currentwidth;
    ih->firstchild->currentheight = ih->data->barsize;

    if (child2)
      iupBaseSetCurrentSize(child2, ih->currentwidth, height2, shrink);
  }
}

static void iSplitSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  Ihandle *child1, *child2 = NULL;
  child1 = ih->firstchild->brother;
  if (child1)
    child2 = child1->brother;

  if (ih->data->orientation == ISPLIT_VERT)
  {
    if (child1)
      iupBaseSetPosition(child1, x, y);
    x += iSplitGetWidth1(ih);

    /* bar */
    iupBaseSetPosition(ih->firstchild, x, y);
    x += ih->data->barsize;

    if (child2)
      iupBaseSetPosition(child2, x, y);
  }
  else /* ISPLIT_HORIZ */
  {
    if (child1)
      iupBaseSetPosition(child1, x, y);
    y += iSplitGetHeight1(ih);

    /* bar */
    iupBaseSetPosition(ih->firstchild, x, y);
    y += ih->data->barsize;

    if (child2)
      iupBaseSetPosition(child2, x, y);
  }
}

static int iSplitCreateMethod(Ihandle* ih, void** params)
{
  Ihandle* bar;

  ih->data = iupALLOCCTRLDATA();

  ih->data->orientation = ISPLIT_VERT;
  ih->data->val = -1;
  ih->data->layoutdrag = 1;
  ih->data->autohide = 0;
  ih->data->barsize = 5;
  ih->data->showgrip = 1;
  ih->data->min = 0; 
  ih->data->max = 1000;

  bar = IupCanvas(NULL);
  iupChildTreeAppend(ih, bar);  /* bar will always be the firstchild */
  bar->flags |= IUP_INTERNAL;

  IupSetAttribute(bar, "CANFOCUS", "NO");
  IupSetAttribute(bar, "BORDER", "NO");
  IupSetAttribute(bar, "EXPAND", "NO");
  IupSetAttribute(bar, "CURSOR", "SPLITTER_VERT");

  /* Setting callbacks */
  IupSetCallback(bar, "BUTTON_CB", (Icallback) iSplitButton_CB);
  IupSetCallback(bar, "FOCUS_CB",  (Icallback) iSplitFocus_CB);
  IupSetCallback(bar, "MOTION_CB", (Icallback) iSplitMotion_CB);
  IupSetCallback(bar, "ACTION", (Icallback) iSplitAction_CB);

  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (iparams[0]) IupAppend(ih, iparams[0]);
    if (iparams[1]) IupAppend(ih, iparams[1]);
  }

  return IUP_NOERROR;
}

Iclass* iupSplitNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name   = "split";
  ic->format = "hh";   /* two ihandle(s) */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype  = IUP_CHILDMANY+3;  /* canvas+child+child */
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupSplitNewClass;
  ic->Create  = iSplitCreateMethod;
  ic->Map     = iupBaseTypeVoidMapMethod;

  ic->ComputeNaturalSize = iSplitComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iSplitSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition    = iSplitSetChildrenPositionMethod;

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iupBaseGetRasterSizeAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iupBaseGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* IupSplit only */
  iupClassRegisterAttribute(ic, "COLOR",     NULL, iSplitSetColorAttrib,     IUPAF_SAMEASSYSTEM, "160 160 160", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ORIENTATION", NULL, iSplitSetOrientationAttrib, IUPAF_SAMEASSYSTEM, "VERTICAL", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DIRECTION", NULL, iSplitSetOrientationAttrib, IUPAF_SAMEASSYSTEM, "VERTICAL", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", iSplitGetValueAttrib, iSplitSetValueAttrib, IUPAF_SAMEASSYSTEM, "500", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LAYOUTDRAG", iSplitGetLayoutDragAttrib, iSplitSetLayoutDragAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWGRIP", iSplitGetShowGripAttrib, iSplitSetShowGripAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AUTOHIDE", iSplitGetAutoHideAttrib, iSplitSetAutoHideAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BARSIZE", iSplitGetBarSizeAttrib, iSplitSetBarSizeAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MINMAX", iSplitGetMinMaxAttrib, iSplitSetMinMaxAttrib, IUPAF_SAMEASSYSTEM, "0:1000", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupSplit(Ihandle* child1, Ihandle* child2)
{
  void *children[3];
  children[0] = (void*)child1;
  children[1] = (void*)child2;
  children[2] = NULL;
  return IupCreatev("split", children);
}
