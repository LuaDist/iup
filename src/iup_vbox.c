/** \file
 * \brief Vbox Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_box.h"


static int iVboxSetRasterSizeAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->userwidth = 0;
    ih->userheight = 0;
  }
  else
  {
    int s = 0, d = 0;
    iupStrToIntInt(value, &s, &d, 'x');  /* first value will be ignored if second defined, can NOT set width */
    if (d != 0) s = d;
    if (s > 0) 
    {
      ih->userheight = s;
      ih->userwidth = 0;
    }
  }
  iupAttribSetStr(ih, "SIZE", NULL); /* clear SIZE in hash table */
  return 0;
}

static int iVboxSetSizeAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->userwidth = 0;
    ih->userheight = 0;
  }
  else
  {
    int s = 0, d = 0;
    iupStrToIntInt(value, &s, &d, 'x');  /* first value will be ignored if second defined, can NOT set width */
    if (d != 0) s = d;
    if (s > 0) 
    {
      int charwidth, charheight;
      iupdrvFontGetCharSize(ih, &charwidth, &charheight);
      ih->userheight = iupHEIGHT2RASTER(s, charheight);
      ih->userwidth = 0;
    }
  }
  return 1;
}

static int iVboxSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "ARIGHT"))
    ih->data->alignment = IUP_ALIGN_ARIGHT;
  else if (iupStrEqualNoCase(value, "ACENTER"))
    ih->data->alignment = IUP_ALIGN_ACENTER;
  else if (iupStrEqualNoCase(value, "ALEFT"))
    ih->data->alignment = IUP_ALIGN_ALEFT;
  return 0;
}

static char* iVboxGetAlignmentAttrib(Ihandle* ih)
{
  char* align2str[3] = {"ALEFT", "ACENTER", "ARIGHT"};
  char *str = iupStrGetMemory(50);
  sprintf(str, "%s", align2str[ih->data->alignment]);
  return str;
}

static void iVboxComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  Ihandle* child;
  int children_naturalwidth, children_naturalheight;

  /* calculate total children natural size */
  int children_expand = 0;
  int children_count = 0;
  int children_natural_maxwidth = 0;
  int children_natural_maxheight = 0;
  int children_natural_totalheight = 0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    /* update child natural size first */
    if (!(child->flags & IUP_FLOATING_IGNORE))
      iupBaseComputeNaturalSize(child);

    if (!(child->flags & IUP_FLOATING))
    {
      children_expand |= child->expand;
      children_natural_maxwidth = iupMAX(children_natural_maxwidth, child->naturalwidth);
      children_natural_maxheight = iupMAX(children_natural_maxheight, child->naturalheight);
      children_count++;
    }
  }

  /* reset to max natural width and/or height if NORMALIZESIZE is defined */
  if (ih->data->normalize_size)
    iupNormalizeSizeBoxChild(ih, ih->data->normalize_size, children_natural_maxwidth, children_natural_maxheight);

  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
      children_natural_totalheight += child->naturalheight;
  }

  /* leave room at the element for the maximum natural size of the children when is_homogeneous */
  if (ih->data->is_homogeneous)
    children_natural_totalheight = children_natural_maxheight*children_count;

  /* compute the Vbox contents natural size */
  children_naturalwidth  = children_natural_maxwidth + 2*ih->data->margin_x;
  children_naturalheight = children_natural_totalheight + (children_count-1)*ih->data->gap + 2*ih->data->margin_y;

  /* Store to be used in iVboxCalcEmptyHeight */
  ih->data->children_naturalsize = children_naturalheight;

  *expand = children_expand;
  *w = children_naturalwidth;
  *h = children_naturalheight;
}

static int iHboxCalcHomogeneousHeight(Ihandle *ih)
{
  /* This is the space that the child can be expanded. */
  Ihandle* child;
  int homogeneous_height;

  int children_count=0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
      children_count++;
  }
  if (children_count == 0)
    return 0;

  /* equal spaces for all elements */
  homogeneous_height = (ih->currentheight - (children_count-1)*ih->data->gap - 2*ih->data->margin_y)/children_count;
  if (homogeneous_height < 0) homogeneous_height = 0;
  return homogeneous_height;
}

static int iVboxCalcEmptyHeight(Ihandle *ih, int expand)
{
  /* This is the space that the child can be expanded. */
  Ihandle* child;
  int empty_height;

  int expand_count=0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING) && child->expand & expand)
      expand_count++;
  }
  if (expand_count == 0)
    return 0;

  /* equal spaces for all expandable elements */
  empty_height = (ih->currentheight - ih->data->children_naturalsize)/expand_count;  
  if (empty_height < 0) empty_height = 0;
  return empty_height;
}

static void iVboxSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle* child;
  int empty_h0 = 0, empty_h1 = 0, client_width;

  if (ih->data->expand_children)
    ih->expand |= ih->data->expand_children;

  if (ih->data->is_homogeneous)
    ih->data->homogeneous_size = iHboxCalcHomogeneousHeight(ih);
  else
  {
    ih->data->homogeneous_size = 0;

    /* must calculate the space left for each control to grow inside the container */
    /* H1 means there is an EXPAND enabled inside */
    if (ih->expand & IUP_EXPAND_H1)
      empty_h1 = iVboxCalcEmptyHeight(ih, IUP_EXPAND_H1);
    /* Not H1 and H0 means that EXPAND is not enabled, but there are some IupFill inside */
    else if (ih->expand & IUP_EXPAND_H0)
      empty_h0 = iVboxCalcEmptyHeight(ih, IUP_EXPAND_H0);
  }

  client_width = ih->currentwidth - 2*ih->data->margin_x;
  if (client_width<0) client_width=0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int old_expand = child->expand;
      if (ih->data->expand_children)
        child->expand |= ih->data->expand_children;

      if (ih->data->homogeneous_size)
        iupBaseSetCurrentSize(child, client_width, ih->data->homogeneous_size, shrink);
      else
      {
        int empty = (child->expand & IUP_EXPAND_H1)? empty_h1: ((child->expand & IUP_EXPAND_H0)? empty_h0: 0);
        char* weight_str = iupAttribGet(child, "EXPANDWEIGHT");
        if (weight_str)
        {
          float weight; 
          if (iupStrToFloat(weight_str, &weight))
            empty = iupROUND(empty * weight);
        }
        iupBaseSetCurrentSize(child, client_width, child->naturalheight+empty, shrink);
      }

      if (ih->data->expand_children)
        child->expand = old_expand;
    }
    else if (!(child->flags & IUP_FLOATING_IGNORE))
    {
      /* update children to their own natural size */
      iupBaseSetCurrentSize(child, child->naturalwidth, child->naturalheight, shrink);
    }
  }
}

static void iVboxSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  int dx, client_width;
  Ihandle* child;

  x += ih->data->margin_x;
  y += ih->data->margin_y;

  client_width = ih->currentwidth - 2*ih->data->margin_x;
  if (client_width<0) client_width=0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      if (ih->data->alignment == IUP_ALIGN_ACENTER)
        dx = (client_width - child->currentwidth)/2;
      else if (ih->data->alignment == IUP_ALIGN_ARIGHT)
        dx = client_width - child->currentwidth;
      else  /* IUP_ALIGN_ALEFT */
        dx = 0;
      if (dx<0) dx = 0;
                      
      /* update child */
      iupBaseSetPosition(child, x+dx, y);

      /* calculate next */
      if (ih->data->homogeneous_size)
        y += ih->data->homogeneous_size + ih->data->gap;
      else
        y += child->currentheight + ih->data->gap;
    }
  }
}


/******************************************************************************/


Ihandle *IupVboxv(Ihandle **children)
{
  return IupCreatev("vbox", (void**)children);
}

Ihandle *IupVbox(Ihandle* child, ...)
{
  Ihandle **children;
  Ihandle *ih;

  va_list arglist;
  va_start(arglist, child);
  children = (Ihandle **)iupObjectGetParamList(child, arglist);
  va_end(arglist);

  ih = IupCreatev("vbox", (void**)children);
  free(children);

  return ih;
}

Iclass* iupVboxNewClass(void)
{
  Iclass* ic = iupBoxNewClassBase();

  ic->name = "vbox";

  /* Class functions */
  ic->New = iupVboxNewClass;
  ic->ComputeNaturalSize = iVboxComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iVboxSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iVboxSetChildrenPositionMethod;

  /* Overwrite Common */
  iupClassRegisterAttribute(ic, "SIZE", iupBaseGetSizeAttrib, iVboxSetSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RASTERSIZE", iupBaseGetRasterSizeAttrib, iVboxSetRasterSizeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* Vbox only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", iVboxGetAlignmentAttrib, iVboxSetAlignmentAttrib, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}
