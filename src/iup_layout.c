/** \file
 * \brief Abstract Layout Management
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>  
#include <stdio.h>  
#include <stdarg.h>  

#include "iup.h"

#include "iup_object.h"
#include "iup_drv.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_layout.h"
#include "iup_assert.h" 

 
void IupRefreshChildren(Ihandle* ih)
{
  int shrink;
  Ihandle *dialog, *child;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  /* must be mapped */
  if (!ih->handle)
    return;

  /* must have children */
  if (!ih->firstchild)
    return;

  /* NOT a dialog, but inside one */
  dialog = IupGetDialog(ih);
  if (!dialog || dialog==ih)
    return;

  /****** local iupLayoutCompute,
     but ih will not be changed, only its children. */

  shrink = iupAttribGetBoolean(dialog, "SHRINK");

  /* children only iupBaseComputeNaturalSize */
  {
    int w=0, h=0, children_expand=ih->expand;
    iupClassObjectComputeNaturalSize(ih, &w, &h, &children_expand);

    /* If the container natural size changed from inside, simply ignore the change */
  }

  /* children only iupBaseSetCurrentSize */
  iupClassObjectSetChildrenCurrentSize(ih, shrink);

  /* children only iupBaseSetPosition */
  iupClassObjectSetChildrenPosition(ih, ih->x, ih->y);


  /****** local iupLayoutUpdate,
     but ih will not be changed, only its children. */
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (child->handle)
      iupLayoutUpdate(child);
  }
}

void IupRefresh(Ihandle* ih)
{
  Ihandle* dialog;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  dialog = IupGetDialog(ih);
  if (dialog)
  {
    iupLayoutCompute(dialog);
    if (dialog->handle)
      iupLayoutUpdate(dialog);
  }
}

static void iLayoutDisplayUpdateChildren(Ihandle *ih)
{
  Ihandle* child;
  for (child = ih->firstchild; child; child = child->brother)
  {
    iLayoutDisplayUpdateChildren(child);

    if (child->handle && child->iclass->nativetype != IUP_TYPEVOID)
      iupdrvPostRedraw(child);
  }
}

void IupUpdate(Ihandle* ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->handle && ih->iclass->nativetype != IUP_TYPEVOID)
    iupdrvPostRedraw(ih);
}

void IupUpdateChildren(Ihandle* ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  iLayoutDisplayUpdateChildren(ih);
}

static void iLayoutDisplayRedrawChildren(Ihandle *ih)
{
  Ihandle* child;
  for (child = ih->firstchild; child; child = child->brother)
  {
    iLayoutDisplayRedrawChildren(child);

    if (child->handle && child->iclass->nativetype != IUP_TYPEVOID)
      iupdrvRedrawNow(child);
  }
}

void IupRedraw(Ihandle* ih, int children)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->handle && ih->iclass->nativetype != IUP_TYPEVOID)
    iupdrvRedrawNow(ih);

  if (children)
    iLayoutDisplayRedrawChildren(ih);
}

void iupLayoutUpdate(Ihandle* ih)
{
  Ihandle* child;

  if (ih->flags & IUP_FLOATING_IGNORE)
    return;

  /* update size and position of the native control */
  iupClassObjectLayoutUpdate(ih);

  /* update its children */
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (child->handle)
      iupLayoutUpdate(child);
  }
}

void iupLayoutCompute(Ihandle* ih)
{
  /* called only for the dialog */

  int shrink = iupAttribGetBoolean(ih, "SHRINK");

  /* Compute the natural size for all elements in the dialog,   
     using the minimum visible size and the defined user size.
     The minimum visible size is the size where all the controls can display
     all their contents.
     The defined user size is used to increase the value of the minimum visible size for containers,
     for standard controls will replace the minimum visible size.
     So the native size will be the maximum value between 
     minimum visible size and defined user size.
     Also calculates the expand configuration for each element, but expand is used only in SetChildrenCurrentSize.
     SEQUENCE: will first calculate the native size for the children, then for the element. */
  iupBaseComputeNaturalSize(ih);

  /* Set the current size (not reflected in the native element yet) based on
     the natural size and the expand configuration. 
     If shrink is 0 (default) the current size of containers can be only larger than the natural size,
     the result will depend on the EXPAND attribute.
     If shrink is 1 the containers can be resized to sizes smaller than the natural size.
     SEQUENCE: will first calculate the current size of the element, then for the children. */
  iupBaseSetCurrentSize(ih, 0, 0, shrink);

  /* Now that the current size is known, set the position of the elements 
     relative to the parent.
     SEQUENCE: will first set the position of the element, then for the children. */
  iupBaseSetPosition(ih, 0, 0);
}

void iupLayoutSetMinMaxSize(Ihandle* ih, int *w, int *h)
{
  if (ih->flags & IUP_MINSIZE)
  {
    char* value = iupAttribGet(ih, "MINSIZE");
    int min_w = 0, min_h = 0;          /* MINSIZE default value */
    iupStrToIntInt(value, &min_w, &min_h, 'x');
    if (w && *w < min_w) *w = min_w;
    if (h && *h < min_h) *h = min_h;
  }

  if (ih->flags & IUP_MAXSIZE)
  {
    char* value = iupAttribGet(ih, "MAXSIZE");
    int max_w = 65535, max_h = 65535;  /* MAXSIZE default value */
    iupStrToIntInt(value, &max_w, &max_h, 'x');
    if (w && *w > max_w) *w = max_w;
    if (h && *h > max_h) *h = max_h;
  }
}

void iupBaseComputeNaturalSize(Ihandle* ih)
{
  /* always initialize the natural size using the user size */
  ih->naturalwidth = ih->userwidth;
  ih->naturalheight = ih->userheight;

  if (ih->iclass->childtype!=IUP_CHILDNONE || 
      ih->iclass->nativetype == IUP_TYPEDIALOG)  /* pre-defined dialogs can restrict the number of children */
  {
    int w=0, h=0, children_expand;

    /* if a container then update the "expand" member from the EXPAND attribute */
    iupBaseContainerUpdateExpand(ih);
    children_expand = ih->expand; /* use it as default value */

    iupClassObjectComputeNaturalSize(ih, &w, &h, &children_expand);

    if (ih->iclass->nativetype == IUP_TYPEDIALOG)
    {
      /* only update the natural size if user size is not defined. */
      /* IupDialog is the only container where this must be done */ 
      /* if the natural size is bigger than the actual dialog size then
         the dialog will be resized, if smaller then the dialog remains with the same size. */
      ih->expand |= children_expand;
      if (ih->naturalwidth <= 0) ih->naturalwidth = iupMAX(ih->currentwidth, w);
      if (ih->naturalheight <= 0) ih->naturalheight = iupMAX(ih->currentheight, h);
    }
    else
    {
      ih->expand &= children_expand; /* combine but only expand where the element can expand */
      ih->naturalwidth = iupMAX(ih->naturalwidth, w);
      ih->naturalheight = iupMAX(ih->naturalheight, h);
    }
  }
  else 
  {
    /* for non-container only compute if user size is not defined */
    if (ih->naturalwidth <= 0 || ih->naturalheight <= 0)
    {
      int w=0, h=0, children_expand;
      iupClassObjectComputeNaturalSize(ih, &w, &h, &children_expand);

      if (ih->naturalwidth <= 0) ih->naturalwidth = w;
      if (ih->naturalheight <= 0) ih->naturalheight = h;
    }
  }

  /* crop the natural size */
  iupLayoutSetMinMaxSize(ih, &(ih->naturalwidth), &(ih->naturalheight));
}

void iupBaseSetCurrentSize(Ihandle* ih, int w, int h, int shrink)
{
  if (ih->iclass->nativetype == IUP_TYPEDIALOG)
  {
    /* w and h parameters here are ignored, because they are always 0 for the dialog. */

    /* current size is zero before map and when reset by the application */
    /* after that the current size must follow the actual size of the dialog */
    if (!ih->currentwidth)  ih->currentwidth  = ih->naturalwidth;
    if (!ih->currentheight) ih->currentheight = ih->naturalheight;
  }
  else
  {
    if (ih->iclass->childtype!=IUP_CHILDNONE)
    {
      if (shrink)
      {
        /* if expand then use the given size, else use the natural size */
        /* this expand is a combination of the expand defined for the element and its children */
        ih->currentwidth  = (ih->expand & IUP_EXPAND_WIDTH)?  w: ih->naturalwidth;
        ih->currentheight = (ih->expand & IUP_EXPAND_HEIGHT)? h: ih->naturalheight;
      }
      else
      {
        /* if expand then use the given size (if greater than natural size), else use the natural size */
        /* this expand is a combination of the expand defined for the element and its children */
        ih->currentwidth  = (ih->expand & IUP_EXPAND_WIDTH)?  iupMAX(ih->naturalwidth, w):  ih->naturalwidth;
        ih->currentheight = (ih->expand & IUP_EXPAND_HEIGHT)? iupMAX(ih->naturalheight, h): ih->naturalheight;
      }
    }
    else
    {
      /* shrink is only used by containers, usually is 0 */
      /* for non containers is always 1, so they always can be smaller than the natural size */

      /* if expand use the given size, else use the natural size */
      /* this expand is the defined for the element */
      ih->currentwidth = (ih->expand & IUP_EXPAND_WIDTH)? w: ih->naturalwidth;
      ih->currentheight = (ih->expand & IUP_EXPAND_HEIGHT)? h: ih->naturalheight;
    }
  }

  /* crop also the current size if expanded */
  if (ih->expand & IUP_EXPAND_WIDTH || ih->expand & IUP_EXPAND_HEIGHT)
    iupLayoutSetMinMaxSize(ih, &(ih->currentwidth), &(ih->currentheight));

  if (ih->firstchild)
    iupClassObjectSetChildrenCurrentSize(ih, shrink);
}

void iupBaseSetPosition(Ihandle* ih, int x, int y)
{
  ih->x = x;
  ih->y = y;

  if (ih->firstchild)
    iupClassObjectSetChildrenPosition(ih, x, y);
}
