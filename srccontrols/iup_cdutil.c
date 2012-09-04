/** \file
 * \brief cdiuputil. CD and IUP utilities for the IupControls
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdlib.h>
#include <stdarg.h>

#include <cd.h>

#include "iup.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_cdutil.h"


long cdIupConvertColor(const char *color)
{
  unsigned char r, g, b;
  if (iupStrToRGB(color, &r, &g, &b))
    return cdEncodeColor(r, g, b);
  else
    return 0;
}

void cdIupCalcShadows(long bgcolor, long *light_shadow, long *mid_shadow, long *dark_shadow)
{
  int r, bg_r = cdRed(bgcolor);
  int g, bg_g = cdGreen(bgcolor);
  int b, bg_b = cdBlue(bgcolor);

  /* light_shadow */

  int max = bg_r;
  if (bg_g > max) max = bg_g;
  if (bg_b > max) max = bg_b;

  if (255-max < 64)
  {
    r = 255;
    g = 255;
    b = 255;
  }
  else
  {
    /* preserve some color information */
    if (bg_r == max) r = 255;
    else             r = bg_r + (255-max);
    if (bg_g == max) g = 255;
    else             g = bg_g + (255-max);
    if (bg_b == max) b = 255;
    else             b = bg_b + (255-max);
  }

  if (light_shadow) *light_shadow = cdEncodeColor((unsigned char)r, (unsigned char)g, (unsigned char)b); 

  /* dark_shadow */
  r = bg_r - 128;
  g = bg_g - 128;
  b = bg_b - 128;
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;

  if (dark_shadow) *dark_shadow = cdEncodeColor((unsigned char)r, (unsigned char)g, (unsigned char)b); 

  /*   mid_shadow = (dark_shadow+bgcolor)/2    */
  if (mid_shadow) *mid_shadow = cdEncodeColor((unsigned char)((bg_r+r)/2), (unsigned char)((bg_g+g)/2), (unsigned char)((bg_b+b)/2));
}

void cdIupDrawSunkenRect(cdCanvas *canvas, int x1, int y1, int x2, int y2, long light_shadow, long mid_shadow, long dark_shadow)
{
  cdCanvasForeground(canvas, mid_shadow);
  cdCanvasLine(canvas, x1, y1+1,   x1, y2);
  cdCanvasLine(canvas, x1,  y2, x2-1, y2);

  cdCanvasForeground(canvas, dark_shadow);
  cdCanvasLine(canvas, x1+1, y1+2, x1+1, y2-1);
  cdCanvasLine(canvas, x1+1, y2-1, x2-2, y2-1);

  cdCanvasForeground(canvas, light_shadow);
  cdCanvasLine(canvas, x1, y1, x2, y1);
  cdCanvasLine(canvas, x2, y1, x2, y2);
}

void cdIupDrawRaisenRect(cdCanvas *canvas, int x1, int y1, int x2, int y2, long light_shadow, long mid_shadow, long dark_shadow)
{
  cdCanvasForeground(canvas, light_shadow);
  cdCanvasLine(canvas, x1, y1+1,   x1, y2);
  cdCanvasLine(canvas, x1,  y2, x2-1, y2);

  cdCanvasForeground(canvas, dark_shadow);
  cdCanvasLine(canvas, x1, y1, x2, y1);
  cdCanvasLine(canvas, x2, y1, x2, y2);

  cdCanvasForeground(canvas, mid_shadow);
  cdCanvasLine(canvas, x1+1, y1+1, x2-1, y1+1);
  cdCanvasLine(canvas, x2-1, y1+2, x2-1, y2-1);
}

void cdIupDrawVertSunkenMark(cdCanvas *canvas, int x, int y1, int y2, long light_shadow, long dark_shadow)
{
  cdCanvasForeground(canvas, dark_shadow);
  cdCanvasLine(canvas, x-1, y1, x-1, y2);
  cdCanvasForeground(canvas, light_shadow);
  cdCanvasLine(canvas,   x, y1,   x, y2);
}

void cdIupDrawHorizSunkenMark(cdCanvas *canvas, int x1, int x2, int y, long light_shadow, long dark_shadow)
{
  cdCanvasForeground(canvas, dark_shadow);
  cdCanvasLine(canvas, x1, y+1, x2, y+1);
  cdCanvasForeground(canvas, light_shadow);
  cdCanvasLine(canvas, x1, y, x2, y);
}

void cdIupDrawFocusRect(Ihandle* ih, cdCanvas *canvas, int x1, int y1, int x2, int y2)
{
  int y, x, w, h;
#ifdef WIN32
  void* gc = cdCanvasGetAttribute(canvas, "HDC");
#else
  void* gc = cdCanvasGetAttribute(canvas, "GC");
#endif

  cdCanvasUpdateYAxis(canvas, &y1);
  cdCanvasUpdateYAxis(canvas, &y2);
  y = y1;
  if (y2<y1) y = y2;
  x = x1;
  if (x2<x1) x = x2;

  w = abs(x2-x1)+1;
  h = abs(y2-y1)+1;

  iupdrvDrawFocusRect(ih, gc, x, y, w, h);
}

void cdDrawFocusRect(cdCanvas *canvas, int x1, int y1, int x2, int y2)
{
  int old_linestyle = cdCanvasLineStyle(canvas, CD_DOTTED);
  int old_foreground = cdCanvasForeground(canvas, CD_WHITE);
  int old_writemode = cdCanvasWriteMode(canvas, CD_XOR);

  cdCanvasRect(canvas, x1, x2, y1, y2);

  cdCanvasWriteMode(canvas, old_writemode);
  cdCanvasForeground(canvas, old_foreground);
  cdCanvasLineStyle(canvas, old_linestyle);
}

void cdIupSetFont(Ihandle* ih, cdCanvas *canvas, const char* font)
{
  char* lastfont = iupAttribGetStr(ih, "_IUPLAST_FONT");
  if (!lastfont || !iupStrEqual(lastfont, font))
  {
    iupAttribStoreStr(ih, "_IUPLAST_FONT", font);
    cdCanvasNativeFont(canvas, font);
  }
}

