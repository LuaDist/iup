/** \file
 * \brief IUP Driver
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>

#include "iup.h"

#include <cd.h>
#include <cd_private.h>
#include <cdiup.h>
#include <cdnative.h>

static void (*cdcreatecanvasNATIVE)(cdCanvas* canvas, void* data) = NULL;

static void cdcreatecanvasIUP(cdCanvas* canvas, Ihandle *ih_canvas)
{
#ifndef WIN32
  char str[50] = "";
#endif
  char* data;

  if (cdBaseDriver()==CD_BASE_GDK)
  {
    data = IupGetAttribute(ih_canvas, "DRAWABLE");  /* new IUP 3 attribute, works for GTK only */
    if (!data)
      return;
  }
  else
  {
#ifdef WIN32
    data = IupGetAttribute(ih_canvas, "HWND");  /* new IUP 3 attribute, works for Windows and GTK */
    if (!data)
      data = IupGetAttribute(ih_canvas, "WID"); /* OLD IUP 2 attribute */
    if (!data)
      return;
#else
    void *dpy = IupGetAttribute(ih_canvas, "XDISPLAY");   /* works for Motif and GTK */
    unsigned long wnd = (unsigned long)IupGetAttribute(ih_canvas, "XWINDOW");
    if (!wnd || !dpy)
      return;
    data = str;
#ifdef SunOS_OLD
    sprintf(str, "%d %lu", (int)dpy, wnd); 
#else
    sprintf(str, "%p %lu", dpy, wnd); 
#endif
#endif
  }
  
  /* Inicializa driver NativeWindow */
  cdcreatecanvasNATIVE(canvas, data);

  IupSetAttribute(ih_canvas, "_CD_CANVAS", (char*)canvas);
}

static cdContext cdIupContext;

cdContext* cdContextIup(void)
{
  /* call cdContextNativeWindow every time, because of ContextPlus */
  cdContext* ctx = cdContextNativeWindow();

  cdcreatecanvasNATIVE = ctx->cxCreateCanvas;

  cdIupContext = *ctx;
  cdIupContext.cxCreateCanvas = cdcreatecanvasIUP;

  return &cdIupContext;
}
