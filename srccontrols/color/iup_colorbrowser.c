/** \file
* \brief ColorBrowser Control.
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

#include <cd.h>
#include <cdiup.h>
#include <cdirgb.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_controls.h"
#include "iup_cdutil.h"
#include "iup_register.h"

#include "iup_colorhsi.h"


#ifndef min
#define min(a, b) ( (a < b) ? (a) : (b) )
#endif

#define ICB_DEG2RAD  0.01745329252f  /* degrees to radians (rad = ICB_DEG2RAD * deg) */
#define ICB_DEFAULTSIZE   181  /* default size */
#define ICB_SPACE  4           /* size of the spacing */
#define ICB_HUEWIDTH  18       /* width of the hue ring */
#define ICB_MARKSIZE  6        /* size of the cursor mark */
enum {ICB_INSIDE_NONE, ICB_INSIDE_HUE, ICB_INSIDE_SI};


struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  /* mouse interaction state */ 
  int h_down,
      si_down;

  /* cursor positioning */
  int h_x, h_y,
      si_x, si_y;

  /* HSI-XY coordinate convertion */
  int xc, yc,  /* center */
      R,       /* maximum radius available inside the size of the control */
      Ix,      /* x coordinate where S is 0 */
      Iy1,     /* y coordinate where I is 0 */
      Iy2,     /* y coordinate where I is 1 */
      SxMax;   /* x coordinate where S is 1 and I = 0.5 */

  /* visual appearance control */
  int w, h;
  int has_focus; 
  long bgcolor;

  /* attributes */
  float hue,          /* 0<=H<=359 */
        saturation,   /* 0<=S<=1 */
        intensity;    /* 0<=I<=1 */
  unsigned char red, green, blue;  /* 0<=x<=255 */

  cdCanvas *cddbuffer;
  cdCanvas *cdcanvas;
};


static float iColorBrowserSXmax(Ihandle* ih, int y)
{
  if (y == ih->data->yc)
    return (float)(ih->data->SxMax - ih->data->Ix);
  else if (y < ih->data->yc)
  {
    float D2 = (ih->data->Iy2 - ih->data->Iy1)/2.0f;
    return  ((float)(ih->data->SxMax - ih->data->Ix)*(y-ih->data->yc + D2))/D2;
  }
  else
  {
    float D2 = (ih->data->Iy2 - ih->data->Iy1)/2.0f;
    return -((float)(ih->data->SxMax - ih->data->Ix)*(y-ih->data->yc - D2))/D2;
  }
}

static float iColorBrowserCalcIntensity(Ihandle* ih, int y)
{
  return (float)(y - ih->data->Iy1)/(float)(ih->data->Iy2 - ih->data->Iy1);
}

static float iColorBrowserCalcSaturation(Ihandle* ih, int x, float sx_max)
{
  if (sx_max == 0)
    return 0;
  else
    return (float)(x - ih->data->Ix)/sx_max;
}

/* Rotate points of 60 degrees */
static void iColorBrowserRotatePoints(float *x1, float *y1, float *x2, float *y2, int xc, int yc)
{
  float xt, yt;
  float nxt, nyt;
  static const float s60 = 0.8660254f;
  static const float c60 = 0.5f;

  xt = *x1 - xc; 
  yt = *y1 - yc;
  nxt = xt * c60 - yt * s60;
  nyt = xt * s60 + yt * c60;
  *x1 = nxt + xc; 
  *y1 = nyt + yc;

  xt = *x2 - xc; 
  yt = *y2 - yc;
  nxt = xt * c60 - yt * s60;
  nyt = xt * s60 + yt * c60;
  *x2 = nxt + xc;
  *y2 = nyt + yc;
}

static void iColorBrowserRenderImageHue(Ihandle* ih)
{
  int x, y, active = 1;
  unsigned char *red, *green, *blue;
  unsigned char bg_red, bg_green, bg_blue;
  if (!ih->data->cddbuffer)
    return;

  cdCanvasBackground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasClear(ih->data->cddbuffer);

  if (!iupdrvIsActive(ih))
    active = 0;

  if (ih->data->has_focus)
    cdDrawFocusRect(ih->data->cddbuffer, 0, 0, ih->data->w-1, ih->data->h-1);

  red = cdRedImage(ih->data->cddbuffer);
  green = cdGreenImage(ih->data->cddbuffer);
  blue = cdBlueImage(ih->data->cddbuffer);

  cdDecodeColor(ih->data->bgcolor, &bg_red, &bg_green, &bg_blue);

  for (y = 0; y < ih->data->h; y++)
  {
    float sx_max = iColorBrowserSXmax(ih, y);

    for (x = 0; x < ih->data->w; x++)
    {
      int xl, yl;
      float radius, diff1, diff2;

      if (y > ih->data->Iy1 && 
          y < ih->data->Iy2 &&
          x > ih->data->Ix  &&
          x < ih->data->Ix+(int)sx_max)
        continue;

      xl = x - ih->data->xc;
      yl = y - ih->data->yc;
      radius = sqrtf(xl*xl + yl*yl);

      diff1 = radius - (ih->data->R-ICB_SPACE-ICB_HUEWIDTH);
      diff2 = (ih->data->R-ICB_SPACE) - radius;

      if (diff1>0 && diff2>0)
      {
        float h, s, i;
        int offset = y*ih->data->w + x;
        unsigned char* r = red + offset;
        unsigned char* g = green + offset;
        unsigned char* b = blue + offset;

        h = atan2f(yl, xl);
        h = (float)(h * CD_RAD2DEG);
        s = 1.0f;   /* maximum saturation */
        i = 0.5f;   /* choose I where S is maximum */

        iupColorHSI2RGB(h, s, i, r, g, b);

        if (diff1<1 || diff2<1)  /* anti-aliasing */
        {
          float diff = (float)(diff1<1? diff1: diff2);
          *r = (unsigned char)((*r)*diff + bg_red*(1.0f-diff));
          *g = (unsigned char)((*g)*diff + bg_green*(1.0f-diff));
          *b = (unsigned char)((*b)*diff + bg_blue*(1.0f-diff));
        }

        if (!active)
        {
          *r = cdIupLIGTHER(*r);
          *g = cdIupLIGTHER(*g);
          *b = cdIupLIGTHER(*b);
        }
      }
    }
  }

  if (active)
  {
    float x1, x2, y1, y2;
    unsigned char shade_lr, shade_lg, shade_lb,
                  shade_dr, shade_dg, shade_db;
    shade_dr = (unsigned char)((2 * bg_red) / 3);
    shade_dg = (unsigned char)((2 * bg_green) / 3);
    shade_db = (unsigned char)((2 * bg_blue) / 3);
    shade_lr = (unsigned char)((255 + bg_red) / 2);
    shade_lg = (unsigned char)((255 + bg_green) / 2);
    shade_lb = (unsigned char)((255 + bg_blue) / 2);
    cdCanvasForeground(ih->data->cddbuffer, cdEncodeColor(shade_dr, shade_dg, shade_db));
    x1 = (float)(ih->data->xc-ih->data->R+ICB_SPACE); y1 = (float)ih->data->yc; x2 = (float)(x1+ICB_HUEWIDTH/2); y2 = (float)ih->data->yc;
    cdCanvasLine(ih->data->cddbuffer, (int) x1, (int) y1, (int) x2, (int) y2);
    iColorBrowserRotatePoints(&x1, &y1, &x2, &y2, ih->data->xc, ih->data->yc);
    cdCanvasForeground(ih->data->cddbuffer, cdEncodeColor(shade_lr, shade_lg, shade_lb));
    cdCanvasLine(ih->data->cddbuffer, (int) x1, (int) y1, (int) x2, (int) y2);
    iColorBrowserRotatePoints(&x1, &y1, &x2, &y2, ih->data->xc, ih->data->yc);
    cdCanvasForeground(ih->data->cddbuffer, cdEncodeColor(shade_dr, shade_dg, shade_db));
    cdCanvasLine(ih->data->cddbuffer, (int) x1, (int) y1, (int) x2, (int) y2);
    iColorBrowserRotatePoints(&x1, &y1, &x2, &y2, ih->data->xc, ih->data->yc);
    cdCanvasForeground(ih->data->cddbuffer, cdEncodeColor(shade_lr, shade_lg, shade_lb));
    cdCanvasLine(ih->data->cddbuffer, (int) x1, (int) y1, (int) x2, (int) y2);
    iColorBrowserRotatePoints(&x1, &y1, &x2, &y2, ih->data->xc, ih->data->yc);
    cdCanvasForeground(ih->data->cddbuffer, cdEncodeColor(shade_dr, shade_dg, shade_db));
    cdCanvasLine(ih->data->cddbuffer, (int) x1, (int) y1, (int) x2, (int) y2);
    iColorBrowserRotatePoints(&x1, &y1, &x2, &y2, ih->data->xc, ih->data->yc);
    cdCanvasLine(ih->data->cddbuffer, (int) x1, (int) y1, (int) x2, (int) y2);
  }
}

static void iColorBrowserRenderImageSI(Ihandle* ih)
{
  int x, y, active = 1;
  unsigned char *red, *green, *blue;
  unsigned char bg_red, bg_green, bg_blue;
  float angle, cos_angle, sin_angle;
  if (!ih->data->cddbuffer)
    return;

  if (!iupdrvIsActive(ih))
    active = 0;

  red = cdRedImage(ih->data->cddbuffer);
  green = cdGreenImage(ih->data->cddbuffer);
  blue = cdBlueImage(ih->data->cddbuffer);

  cdDecodeColor(ih->data->bgcolor, &bg_red, &bg_green, &bg_blue);

  angle = ih->data->hue * ICB_DEG2RAD;
  cos_angle = cosf(angle);
  sin_angle = sinf(angle);

  for (y = 0; y < ih->data->h; y++)
  {
    float sx_max, i;

    if (y < ih->data->Iy1)
      continue;
    else if (y > ih->data->Iy2)
      continue;

    sx_max = iColorBrowserSXmax(ih, y);
    i = iColorBrowserCalcIntensity(ih, y);

    for (x = 0; x < ih->data->w; x++)
    {
      if (x < ih->data->Ix)
        continue;
      else if (x > ih->data->Ix+(int)sx_max)
        continue;

      {
        int offset = y*ih->data->w + x;
        unsigned char* r = red + offset;
        unsigned char* g = green + offset;
        unsigned char* b = blue + offset;
        float s, diff;

        s = iColorBrowserCalcSaturation(ih, x, sx_max);

        iupColorHSI2RGB(ih->data->hue, s, i, r, g, b);

        diff = sx_max - (float)(x - ih->data->Ix);
        if (diff<1.0f)  /* anti-aliasing */
        {
          *r = (unsigned char)((*r)*diff + bg_red*(1.0f-diff));
          *g = (unsigned char)((*g)*diff + bg_green*(1.0f-diff));
          *b = (unsigned char)((*b)*diff + bg_blue*(1.0f-diff));
        }

        if (!active)
        {
          *r = cdIupLIGTHER(*r);
          *g = cdIupLIGTHER(*g);
          *b = cdIupLIGTHER(*b);
        }
      }
    }
  }
}

static void iColorBrowserUpdateCursorSI(Ihandle* ih)
{
  int x;
  int y = (int)(ih->data->intensity*(ih->data->Iy2 - ih->data->Iy1)) + ih->data->Iy1;
  ih->data->si_y = iupROUND(y);
  x = (int)(ih->data->saturation*iColorBrowserSXmax(ih, ih->data->si_y)) + ih->data->Ix;
  ih->data->si_x = iupROUND(x);
}

static void iColorBrowserSetCursorSI(Ihandle* ih, int x, int y)
{
  float sx_max;

  if (y < ih->data->Iy1)
    ih->data->si_y = ih->data->Iy1;
  else if (y > ih->data->Iy2)
    ih->data->si_y = ih->data->Iy2;
  else
    ih->data->si_y = y;

  sx_max = iColorBrowserSXmax(ih, ih->data->si_y);

  if (x < ih->data->Ix)
    ih->data->si_x = ih->data->Ix;
  else if (x > ih->data->Ix+sx_max)
    ih->data->si_x = ih->data->Ix+(int)sx_max;
  else
    ih->data->si_x = x;

  ih->data->intensity = iColorBrowserCalcIntensity(ih, ih->data->si_y);
  ih->data->saturation = iColorBrowserCalcSaturation(ih, ih->data->si_x, sx_max);

  if (ih->data->saturation == -0.0f)
    ih->data->saturation = 0;
  if (ih->data->intensity == -0.0f)
    ih->data->intensity = 0;
}

static void iColorBrowserUpdateCursorHue(Ihandle* ih)
{
  int rc = ih->data->R-ICB_SPACE-ICB_HUEWIDTH/2;
  float angle = ih->data->hue * ICB_DEG2RAD;
  float cos_angle = cosf(angle);
  float sin_angle = sinf(angle);
  float x = rc*cos_angle + ih->data->xc;
  float y = rc*sin_angle + ih->data->yc;
  ih->data->h_x = iupROUND(x);
  ih->data->h_y = iupROUND(y);
}

static void iColorBrowserSetCursorHue(Ihandle* ih, int x, int y)
{
  int xl = x - ih->data->xc;
  int yl = y - ih->data->yc;
  ih->data->hue = (float)(atan2f(yl, xl) * CD_RAD2DEG);
  ih->data->hue = fmodf(ih->data->hue, 360.0f);
  if (ih->data->hue < 0.0f)
    ih->data->hue += 360.0f;

  iColorBrowserUpdateCursorHue(ih);
}

static int iColorBrowserCheckInside(Ihandle* ih, int x, int y)
{
  int xl = x - ih->data->xc;
  int yl = y - ih->data->yc;
  float radius = sqrtf(xl*xl + yl*yl);

  if (radius < ih->data->R-ICB_SPACE-ICB_HUEWIDTH-ICB_SPACE)
    return ICB_INSIDE_SI;

  if (radius > ih->data->R-ICB_SPACE-ICB_HUEWIDTH &&
      radius < ih->data->R-ICB_SPACE)
    return ICB_INSIDE_HUE;

  return ICB_INSIDE_NONE;
}

static void iColorBrowserHSI2RGB(Ihandle* ih)
{
  iupColorHSI2RGB(ih->data->hue, ih->data->saturation, ih->data->intensity, 
                      &(ih->data->red), &(ih->data->green), &(ih->data->blue));
}

static void iColorBrowserRGB2HSI(Ihandle* ih)
{
  iupColorRGB2HSI(ih->data->red, ih->data->green, ih->data->blue,
                      &(ih->data->hue), &(ih->data->saturation), &(ih->data->intensity));
}

static void iColorBrowserUpdateDisplay(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
    return;

  cdCanvasFlush(ih->data->cddbuffer);  /* swap the RGB to the display canvas */

  if (iupdrvIsActive(ih))
  {
    cdCanvasForeground(ih->data->cdcanvas, CD_GRAY);
    cdCanvasArc(ih->data->cdcanvas, ih->data->h_x+1, ih->data->h_y, ICB_MARKSIZE, ICB_MARKSIZE, 0, 360);
    cdCanvasArc(ih->data->cdcanvas, ih->data->si_x+1, ih->data->si_y, ICB_MARKSIZE, ICB_MARKSIZE, 0, 360);
    cdCanvasForeground(ih->data->cdcanvas, CD_WHITE);
    cdCanvasArc(ih->data->cdcanvas, ih->data->h_x, ih->data->h_y, ICB_MARKSIZE, ICB_MARKSIZE, 0, 360);
    cdCanvasArc(ih->data->cdcanvas, ih->data->si_x, ih->data->si_y, ICB_MARKSIZE, ICB_MARKSIZE, 0, 360);
  }
  else
  {
    cdCanvasForeground(ih->data->cdcanvas, CD_DARK_GRAY);
    cdCanvasSector(ih->data->cdcanvas, ih->data->h_x, ih->data->h_y, ICB_MARKSIZE+1, ICB_MARKSIZE+1, 0, 360);
    cdCanvasSector(ih->data->cdcanvas, ih->data->si_x, ih->data->si_y, ICB_MARKSIZE+1, ICB_MARKSIZE+1, 0, 360);
  }
}

static void iColorBrowserCallChangeCb(Ihandle* ih)
{
  IFnccc change_cb = (IFnccc) IupGetCallback(ih, "CHANGE_CB");
  if (change_cb)
    change_cb(ih, ih->data->red, ih->data->green, ih->data->blue);
  
  iupBaseCallValueChangedCb(ih);
}
  
static void iColorBrowserCallDragCb(Ihandle* ih)
{
  IFnccc drag_cb = (IFnccc) IupGetCallback(ih, "DRAG_CB");
  if (drag_cb)
    drag_cb(ih, ih->data->red, ih->data->green, ih->data->blue);

  iupBaseCallValueChangedCb(ih);
}

static int iColorBrowserHmouse(Ihandle* ih, int x, int y, int drag)
{
  iColorBrowserSetCursorHue(ih, x, y);
  iColorBrowserHSI2RGB(ih);
  /* must update the Si area */
  iColorBrowserRenderImageSI(ih);
  iColorBrowserUpdateDisplay(ih);

  if (drag)
    iColorBrowserCallDragCb(ih);
  else
    iColorBrowserCallChangeCb(ih);

  return IUP_DEFAULT;
}

static int iColorBrowserSImouse(Ihandle* ih, int x, int y, int drag)
{
  iColorBrowserSetCursorSI(ih, x, y);
  iColorBrowserHSI2RGB(ih);
  iColorBrowserUpdateDisplay(ih);

  if (drag)
    iColorBrowserCallDragCb(ih);
  else
    iColorBrowserCallChangeCb(ih);

  return IUP_DEFAULT;
}


/******************************************************************/


static int iColorBrowserButton_CB(Ihandle* ih, int b, int press, int x, int y)
{
  if (b != IUP_BUTTON1)
    return IUP_DEFAULT;

  cdCanvasUpdateYAxis(ih->data->cdcanvas, &y);

  if (press)
  {
    int inside = iColorBrowserCheckInside(ih, x, y);

    if (!ih->data->h_down && inside==ICB_INSIDE_HUE)
    {
      iColorBrowserHmouse(ih, x, y, 1);
      ih->data->h_down = 1;
    } 

    if (!ih->data->si_down && inside==ICB_INSIDE_SI)
    {
      iColorBrowserSImouse(ih, x, y, 1);
      ih->data->si_down = 1;
    } 
  }
  else
  {
    if (ih->data->h_down)
    {
      iColorBrowserHmouse(ih, x, y, 0);
      ih->data->h_down = 0;
    }

    if (ih->data->si_down)
    {
      iColorBrowserSImouse(ih, x, y, 0);
      ih->data->si_down = 0;
    }
  }

  return IUP_DEFAULT;
}

/* Callback for the mouse motion in the canvas */
static int iColorBrowserMotion_CB(Ihandle* ih, int x, int y, char *status)
{
  if (!iup_isbutton1(status))
  {
    ih->data->h_down = 0;
    ih->data->si_down = 0;
    return IUP_DEFAULT;
  }

  if (ih->data->h_down)
  {
    cdCanvasUpdateYAxis(ih->data->cdcanvas, &y);
    iColorBrowserHmouse(ih, x, y, 1);
  }
  else if (ih->data->si_down)
  {
    cdCanvasUpdateYAxis(ih->data->cdcanvas, &y);
    iColorBrowserSImouse(ih, x, y, 1);
  }

  return IUP_DEFAULT;
}

static int iColorBrowserFocus_CB(Ihandle* ih, int focus)
{
  ih->data->has_focus = focus;
  iColorBrowserRenderImageHue(ih);
  iColorBrowserRenderImageSI(ih);
  iColorBrowserUpdateDisplay(ih);
  return IUP_DEFAULT;
}

static void iColorBrowserUpdateSize(Ihandle* ih)
{
  int T, D;

  /* update canvas size */
  cdCanvasActivate(ih->data->cddbuffer);
  cdCanvasGetSize(ih->data->cddbuffer, &ih->data->w, &ih->data->h, NULL, NULL);

  ih->data->R = min(ih->data->w, ih->data->h)/2;
  ih->data->xc = ih->data->w/2;
  ih->data->yc = ih->data->h/2;
  T = ih->data->R-ICB_SPACE-ICB_HUEWIDTH-ICB_SPACE;
  ih->data->Ix = ih->data->xc - T/2; /* cos(60)=0.5 */
  D = (int)(2*T*0.866);              /* sin(60)=0.866 */
  ih->data->Iy1 = ih->data->yc - D/2;
  ih->data->Iy2 = ih->data->Iy1 + D;
  ih->data->SxMax = ih->data->xc + T;
}

static int iColorBrowserResize_CB(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
  {
    /* update canvas size */
    cdCanvasActivate(ih->data->cdcanvas);

    /* this can fail if canvas size is zero */
    ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFERRGB, ih->data->cdcanvas);
  }

  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  /* update size */
  iColorBrowserUpdateSize(ih);

  iColorBrowserUpdateCursorHue(ih);
  iColorBrowserUpdateCursorSI(ih);

  /* update render */
  iColorBrowserRenderImageHue(ih);
  iColorBrowserRenderImageSI(ih);

  return IUP_DEFAULT;
}

static int iColorBrowserRedraw_CB(Ihandle* ih)
{
  iColorBrowserUpdateDisplay(ih);
  return IUP_DEFAULT;
}

static int iColorBrowserWheel_CB(Ihandle* ih, float delta)
{
  ih->data->hue += delta;

  iColorBrowserUpdateCursorHue(ih);
  iColorBrowserHSI2RGB(ih);
  /* must update the Si area */
  iColorBrowserRenderImageSI(ih);
  iColorBrowserUpdateDisplay(ih);
  iColorBrowserCallChangeCb(ih);

  return IUP_DEFAULT;
}

static int iColorBrowserKeypress_CB(Ihandle* ih, int c, int press)
{
  int x, y, changing_hue = 0;

  if (!press)
    return IUP_DEFAULT;

  x = ih->data->si_x; 
  y = ih->data->si_y; 

  switch (c)
  {
    case K_UP:
      y++; 
      break;
    case K_DOWN:
      y--;
      break;
    case K_RIGHT:
      x++;
      break;
    case K_LEFT:
      x--;
      break;
    case K_PGUP:
      ih->data->hue += 1.0f; 
      changing_hue = 1; 
      break;
    case K_PGDN:
      ih->data->hue -= 1.0f; 
      changing_hue = 1; break;
    case K_HOME:
      ih->data->hue = 0.0f; 
      changing_hue = 1; 
      break;
    case K_END:
      ih->data->hue = 180.0f; 
      changing_hue = 1; 
      break;
    default:
      return IUP_DEFAULT;
  }

  if (changing_hue)
  {
    iColorBrowserUpdateCursorHue(ih);
    /* must update the Si area */
    iColorBrowserRenderImageSI(ih);
  }
  else
    iColorBrowserSetCursorSI(ih, x, y);

  iColorBrowserHSI2RGB(ih);

  iColorBrowserUpdateDisplay(ih);
  iColorBrowserCallChangeCb(ih);

  return IUP_IGNORE;  /* to avoid arrow keys being processed by the system */
}


/*********************************************************************************/


static char* iColorBrowserGetHSIAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  sprintf(buffer, "%g %g %g", (double)ih->data->hue, (double)ih->data->saturation, (double)ih->data->intensity);
  return buffer;
}

static int iColorBrowserSetHSIAttrib(Ihandle* ih, const char* value)
{
  float old_hue = ih->data->hue,
        old_saturation = ih->data->saturation,
        old_intensity = ih->data->intensity;

  if (!iupStrToHSI(value, &ih->data->hue, &ih->data->saturation, &ih->data->intensity))
    return 0;
  
  if (ih->data->cddbuffer)
  {
    if (old_hue != ih->data->hue) 
      iColorBrowserUpdateCursorHue(ih);
    if (old_saturation != ih->data->saturation || old_intensity != ih->data->intensity) 
      iColorBrowserUpdateCursorSI(ih);
    iColorBrowserHSI2RGB(ih);
    iColorBrowserRenderImageSI(ih);
    iColorBrowserUpdateDisplay(ih);
  }

  return 0;
}

static char* iColorBrowserGetRGBAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(20);
  sprintf(buffer, "%d %d %d", (int) ih->data->red, (int) ih->data->green, (int) ih->data->blue);
  return buffer;
}

static int iColorBrowserSetRGBAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;
  
  ih->data->red = r;
  ih->data->green = g;
  ih->data->blue = b;
  iColorBrowserRGB2HSI(ih);

  if (ih->data->cddbuffer)
  {
    iColorBrowserUpdateCursorHue(ih);
    iColorBrowserUpdateCursorSI(ih);
    iColorBrowserRenderImageSI(ih);
    iColorBrowserUpdateDisplay(ih);
  }

  return 0;
}

static int iColorBrowserSetBgColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = iupControlBaseGetParentBgColor(ih);
  ih->data->bgcolor = cdIupConvertColor(value);

  if (ih->data->cddbuffer)
  {
    iColorBrowserRenderImageHue(ih);
    iColorBrowserRenderImageSI(ih);
    iColorBrowserUpdateDisplay(ih);
  }
  return 1;
}

static int iColorBrowserSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);
  iColorBrowserUpdateDisplay(ih);
  return 0;   /* do not store value in hash table */
}


/****************************************************************************/


static int iColorBrowserMapMethod(Ihandle* ih)
{
  ih->data->cdcanvas = cdCreateCanvas(CD_IUP, ih);
  if (!ih->data->cdcanvas)
    return IUP_ERROR;

  /* this can fail if canvas size is zero */
  ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFERRGB, ih->data->cdcanvas);

  if (ih->data->cddbuffer)
    iColorBrowserUpdateSize(ih);

  return IUP_NOERROR;
}

static void iColorBrowserUnMapMethod(Ihandle* ih)
{
  if (ih->data->cddbuffer)
  {
    cdKillCanvas(ih->data->cddbuffer);
    ih->data->cddbuffer = NULL;
  }

  if (ih->data->cdcanvas)
  {
    cdKillCanvas(ih->data->cdcanvas);
    ih->data->cdcanvas = NULL;
  }
}

static int iColorBrowserCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  IupSetfAttribute(ih, "RASTERSIZE", "%dx%d", ICB_DEFAULTSIZE, ICB_DEFAULTSIZE);
  iupAttribSetStr(ih, "BORDER", "NO");
  ih->expand = IUP_EXPAND_NONE;

  /* IupCanvas callbacks */
  IupSetCallback(ih, "ACTION",      (Icallback)iColorBrowserRedraw_CB);
  IupSetCallback(ih, "RESIZE_CB",   (Icallback)iColorBrowserResize_CB);
  IupSetCallback(ih, "BUTTON_CB",   (Icallback)iColorBrowserButton_CB);
  IupSetCallback(ih, "MOTION_CB",   (Icallback)iColorBrowserMotion_CB);
  IupSetCallback(ih, "FOCUS_CB",    (Icallback)iColorBrowserFocus_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iColorBrowserKeypress_CB);
  IupSetCallback(ih, "WHEEL_CB",    (Icallback)iColorBrowserWheel_CB);

  return IUP_NOERROR;
}

Iclass* iupColorBrowserNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "colorbrowser";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupColorBrowserNewClass;
  ic->Create  = iColorBrowserCreateMethod;
  ic->Map     = iColorBrowserMapMethod;
  ic->UnMap   = iColorBrowserUnMapMethod;

  /* IupColorBrowser Callbacks */
  iupClassRegisterCallback(ic, "DRAG_CB",   "ccc");
  iupClassRegisterCallback(ic, "CHANGE_CB", "ccc");
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");

  /* IupColorBrowser only */
  iupClassRegisterAttribute(ic, "RGB", iColorBrowserGetRGBAttrib, iColorBrowserSetRGBAttrib, "255 0 0", NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);    /* force new default value */
  iupClassRegisterAttribute(ic, "HSI", iColorBrowserGetHSIAttrib, iColorBrowserSetHSIAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* Overwrite IupCanvas Attributes */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iColorBrowserSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", iupControlBaseGetBgColorAttrib, iColorBrowserSetBgColorAttrib, NULL, "255 255 255", IUPAF_NO_INHERIT);    /* overwrite canvas implementation, set a system default to force a new default */

  return ic;
}

Ihandle *IupColorBrowser(void)
{
  return IupCreate("colorbrowser");
}
