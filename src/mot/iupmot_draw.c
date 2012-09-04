/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <Xm/Xm.h>
#include <X11/Xlib.h>

#include "iup.h"

#include "iup_attrib.h"
#include "iup_class.h"
#include "iup_str.h"
#include "iup_object.h"
#include "iup_image.h"
#include "iup_draw.h"

#include "iupmot_drv.h"
#include "iupmot_color.h"


struct _IdrawCanvas{
  Ihandle* ih;
  int w, h;

  Window wnd;
  Pixmap pixmap;
  GC pixmap_gc, gc;
};

static void motDrawGetGeometry(Display *dpy, Drawable wnd, int *_w, int *_h, int *_d)
{
  Window root;
  int x, y;
  unsigned int w, h, b, d;
  XGetGeometry(dpy, wnd, &root, &x, &y, &w, &h, &b, &d);
  *_w = w;
  *_h = h;
  *_d = d;
}

IdrawCanvas* iupDrawCreateCanvas(Ihandle* ih)
{
  IdrawCanvas* dc = calloc(1, sizeof(IdrawCanvas));
  int depth;

  dc->ih = ih;
  dc->wnd = XtWindow(ih->handle);
  dc->gc = XCreateGC(iupmot_display, dc->wnd, 0, NULL);

  motDrawGetGeometry(iupmot_display, dc->wnd, &dc->w, &dc->h, &depth);

  dc->pixmap = XCreatePixmap(iupmot_display, dc->wnd, dc->w, dc->h, depth);
  dc->pixmap_gc = XCreateGC(iupmot_display, dc->pixmap, 0, NULL);

  return dc;
}

void iupDrawKillCanvas(IdrawCanvas* dc)
{
  XFreeGC(iupmot_display, dc->pixmap_gc);
  XFreePixmap(iupmot_display, dc->pixmap);
  XFreeGC(iupmot_display, dc->gc);

  free(dc);
}

void iupDrawUpdateSize(IdrawCanvas* dc)
{
  int w, h, depth;
  motDrawGetGeometry(iupmot_display, dc->wnd, &w, &h, &depth);

  if (w != dc->w || h != dc->h)
  {
    XFreeGC(iupmot_display, dc->pixmap_gc);
    XFreePixmap(iupmot_display, dc->pixmap);

    dc->pixmap = XCreatePixmap(iupmot_display, dc->wnd, dc->w, dc->h, depth);
    dc->pixmap_gc = XCreateGC(iupmot_display, dc->pixmap, 0, NULL);
  }
}

void iupDrawFlush(IdrawCanvas* dc)
{
  XCopyArea(iupmot_display, dc->pixmap, dc->wnd, dc->gc, 0, 0, dc->w, dc->h, 0, 0);
}

void iupDrawGetSize(IdrawCanvas* dc, int *w, int *h)
{
  if (w) *w = dc->w;
  if (h) *h = dc->h;
}

void iupDrawParentBackground(IdrawCanvas* dc)
{
  unsigned char r=0, g=0, b=0;
  char* color = iupBaseNativeParentGetBgColorAttrib(dc->ih);
  iupStrToRGB(color, &r, &g, &b);
  iupDrawRectangle(dc, 0, 0, dc->w-1, dc->h-1, r, g, b, IUP_DRAW_FILL);
}

void iupDrawRectangleInvert(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  XSetFunction(iupmot_display, dc->pixmap_gc, GXxor);
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(255, 255, 255));
  XFillRectangle(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2-x1+1, y2-y1+1);
  XSetFunction(iupmot_display, dc->pixmap_gc, GXcopy);
}

void iupDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));

  if (style==IUP_DRAW_FILL)
    XFillRectangle(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2-x1+1, y2-y1+1);
  else
  {
    XGCValues gcval;
    if (style==IUP_DRAW_STROKE_DASH)
      gcval.line_style = LineOnOffDash;
    else
      gcval.line_style = LineSolid;
    XChangeGC(iupmot_display, dc->pixmap_gc, GCLineStyle, &gcval);

    XDrawRectangle(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2-x1, y2-y1);
  }
}

void iupDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  XGCValues gcval;
  if (style==IUP_DRAW_STROKE_DASH)
    gcval.line_style = LineOnOffDash;
  else
    gcval.line_style = LineSolid;
  XChangeGC(iupmot_display, dc->pixmap_gc, GCLineStyle, &gcval);

  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));

  XDrawLine(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2, y2);
}

void iupDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style)
{
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));

  if (style==IUP_DRAW_FILL)
  {
    XSetArcMode(iupmot_display, dc->pixmap_gc, ArcPieSlice);
    XFillArc(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2-x1+1, y2-y1+1, iupROUND(a1*64), iupROUND((a2 - a1)*64));
  }
  else
  {
    XGCValues gcval;
    if (style==IUP_DRAW_STROKE_DASH)
      gcval.line_style = LineOnOffDash;
    else
      gcval.line_style = LineSolid;
    XChangeGC(iupmot_display, dc->pixmap_gc, GCLineStyle, &gcval);

    XDrawArc(iupmot_display, dc->pixmap, dc->pixmap_gc, x1, y1, x2-x1+1, y2-y1+1, iupROUND(a1*64), iupROUND((a2 - a1)*64));
  }
}

void iupDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style)
{
  int i;
  XPoint* pnt = (XPoint*)malloc(count*sizeof(XPoint)); /* XPoint uses short for coordinates */

  for (i = 0; i < count; i++)
  {
    pnt[i].x = (short)points[2*i];
    pnt[i].y = (short)points[2*i+1];
  }

  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));

  if (style==IUP_DRAW_FILL)
    XFillPolygon(iupmot_display, dc->pixmap, dc->pixmap_gc, pnt, count, Complex, CoordModeOrigin);
  else
  {
    XGCValues gcval;
    if (style==IUP_DRAW_STROKE_DASH)
      gcval.line_style = LineOnOffDash;
    else
      gcval.line_style = LineSolid;
    XChangeGC(iupmot_display, dc->pixmap_gc, GCLineStyle, &gcval);

    XDrawLines(iupmot_display, dc->pixmap, dc->pixmap_gc, pnt, count, CoordModeOrigin);
  }

  free(pnt);
}

void iupDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2)
{
  XRectangle rect;
  rect.x      = (short)x1;
  rect.y      = (short)y1;
  rect.width  = (unsigned short)(x2-x1+1);
  rect.height = (unsigned short)(y2-y1+1);
  XSetClipRectangles(iupmot_display, dc->pixmap_gc, 0, 0, &rect, 1, Unsorted);
}

void iupDrawResetClip(IdrawCanvas* dc)
{
  XSetClipMask(iupmot_display, dc->pixmap_gc, None);
}

void iupDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font)
{
  XFontStruct* xfont = (XFontStruct*)iupmotGetFontStruct(font);
  XSetForeground(iupmot_display, dc->pixmap_gc, iupmotColorGetPixel(r, g, b));
  XSetFont(iupmot_display, dc->pixmap_gc, xfont->fid);
  XDrawString(iupmot_display, dc->pixmap, dc->pixmap_gc, x, y+xfont->ascent, text, len);
}

void iupDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y, int *img_w, int *img_h)
{
  int bpp;
  Pixmap pixmap = (Pixmap)iupImageGetImage(name, dc->ih, make_inactive);
  if (!pixmap)
    return;

  /* must use this info, since image can be a driver image loaded from resources */
  iupdrvImageGetInfo((void*)pixmap, img_w, img_h, &bpp);

  XCopyArea(iupmot_display, pixmap, dc->pixmap, dc->pixmap_gc, 0, 0, *img_w, *img_h, x, y);
}
