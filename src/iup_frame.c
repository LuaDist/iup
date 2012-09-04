/** \file
 * \brief Frame Control.
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
#include "iup_frame.h"


int iupFrameGetTitleHeight(Ihandle* ih)
{
  int charheight;
  iupdrvFontGetCharSize(ih, NULL, &charheight);
  return charheight;
}

static void iFrameGetDecorSize(Ihandle* ih, int *width, int *height)
{
  *width  = 5;
  *height = 5;

  if (iupAttribGet(ih, "_IUPFRAME_HAS_TITLE") || iupAttribGet(ih, "TITLE"))
    (*height) += iupFrameGetTitleHeight(ih);
}

static char* iFrameGetClientSizeAttrib(Ihandle* ih)
{
  int width, height, decorwidth, decorheight;
  char* str = iupStrGetMemory(20);
  width = ih->currentwidth;
  height = ih->currentheight;
  iFrameGetDecorSize(ih, &decorwidth, &decorheight);
  width -= decorwidth;
  height -= decorheight;
  if (width < 0) width = 0;
  if (height < 0) height = 0;
  sprintf(str, "%dx%d", width, height);
  return str;
}

static char* iFrameGetClientOffsetAttrib(Ihandle* ih)
{
  int dx, dy;
  char* str = iupStrGetMemory(20);
  iupdrvFrameGetDecorOffset(&dx, &dy);
  if (iupAttribGet(ih, "_IUPFRAME_HAS_TITLE") || iupAttribGet(ih, "TITLE"))
    dy += iupFrameGetTitleHeight(ih);
  sprintf(str, "%dx%d", dx, dy);
  return str;
}

static int iFrameCreateMethod(Ihandle* ih, void** params)
{
  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    if (*iparams)
      IupAppend(ih, *iparams);
  }

  return IUP_NOERROR;
}

static void iFrameComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int decorwidth, decorheight;
  Ihandle* child = ih->firstchild;

  iFrameGetDecorSize(ih, &decorwidth, &decorheight);
  *w = decorwidth;
  *h = decorheight;

  if (child)
  {
    /* update child natural size first */
    iupBaseComputeNaturalSize(child);

    *expand = child->expand;
    *w += child->naturalwidth;
    *h += child->naturalheight;
  }
}

static void iFrameSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  int width, height, decorwidth, decorheight;

  iFrameGetDecorSize(ih, &decorwidth, &decorheight);

  width = ih->currentwidth-decorwidth;
  height = ih->currentheight-decorheight;
  if (width < 0) width = 0;
  if (height < 0) height = 0;

  if (ih->firstchild)
    iupBaseSetCurrentSize(ih->firstchild, width, height, shrink);
}

static void iFrameSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  int dx=0, dy=0;
  (void)x;
  (void)y;

  /* IupFrame is the native parent of its children,
     so the position is restarted at (0,0).
     In all systems, each frame is a native window covering the client area.
     Child coordinates are relative to client left-top corner of the frame. */

  /* In Windows the position of the child is still 
     relative to the top-left corner of the frame.
     So we must manually add the decorations. */
  if (iupdrvFrameHasClientOffset())
  {
    iupdrvFrameGetDecorOffset(&dx, &dy);
    if (iupAttribGet(ih, "_IUPFRAME_HAS_TITLE") || iupAttribGet(ih, "TITLE"))
      dy += iupFrameGetTitleHeight(ih);
  }

  if (ih->firstchild)
    iupBaseSetPosition(ih->firstchild, dx, dy);
}


/******************************************************************************/


Ihandle* IupFrame(Ihandle* child)
{
  void *children[2];
  children[0] = (void*)child;
  children[1] = NULL;
  return IupCreatev("frame", children);
}

Iclass* iupFrameNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "frame";
  ic->format = "h"; /* one ihandle */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDMANY+1;   /* one child */
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupFrameNewClass;
  ic->Create = iFrameCreateMethod;

  ic->ComputeNaturalSize = iFrameComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iFrameSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iFrameSetChildrenPositionMethod;

  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Common Callbacks */
  iupClassRegisterCallback(ic, "MAP_CB", "");
  iupClassRegisterCallback(ic, "UNMAP_CB", "");

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* Base Container */
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iFrameGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iFrameGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* IupFrame only */
  iupClassRegisterAttribute(ic, "SUNKEN", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  iupdrvFrameInitClass(ic);

  return ic;
}
