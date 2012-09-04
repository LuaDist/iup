/** \file
 * \brief Dial Control.
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
#include <cddbuf.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_controls.h"
#include "iup_cdutil.h"
#include "iup_register.h"


#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define IDIAL_SPACE 3
#define IDIAL_NCOLORS 10
#define IDIAL_DEFAULT_DENSITY 0.2
#define IDIAL_DEFAULT_DENSITY_STR "0.2"
#define IDIAL_DEFAULT_FGCOLOR "64 64 64"
#define IDIAL_DEFAULT_FGCOLOR_COMP 64

enum{IDIAL_VERTICAL, IDIAL_HORIZONTAL, IDIAL_CIRCULAR};

#define dialmin(a,b)  ((a)<(b)?(a):(b))


struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  /* attributes */
  double angle;
  int orientation;
  double unit;
  double density;

  /* mouse interaction control */
  int px;
  int py;
  int pressing;

  /* visual appearance control */
  void (*Draw)(Ihandle* ih);
  int w, h;
  int has_focus; 
  int num_div;      /* number of sections in the dial wheel */
  double radius;    /* radius size of the dial wheel */
  long fgcolor[IDIAL_NCOLORS+1];
  long bgcolor,
       light_shadow,
       mid_shadow,
       dark_shadow;

  /* drawing canvas */
  cdCanvas *cddbuffer;
  cdCanvas *cdcanvas;
};


static long int iDialGetFgColor(Ihandle* ih, double a, double amin)
{
  double nu = fabs(a - 0.5 * M_PI);
  double de = fabs(0.5 * M_PI - amin);
  double fr = nu / de;
  int i = (int)(IDIAL_NCOLORS * fr);
  return ih->data->fgcolor[IDIAL_NCOLORS - i];
}

static void iDialDrawVerticalBackground(Ihandle* ih, double amin, double amax, int *ymin, int *ymax)
{
  double delta  = (0.5 * M_PI - amin) / IDIAL_NCOLORS;
  double a, yc  = ih->data->h / 2.0;
  *ymin = *ymax = ih->data->h / 2;
  for (a = amin; a < 0.5 * M_PI; a += delta)    /* shading */
  {
    int y0 = (int)(yc - ih->data->radius * cos(a));
    int y1 = (int)(yc - ih->data->radius * cos(a+delta));
    cdCanvasForeground(ih->data->cddbuffer, iDialGetFgColor(ih, a, amin));
    cdCanvasBox(ih->data->cddbuffer, IDIAL_SPACE+1, ih->data->w-1-IDIAL_SPACE-2, y0, y1);

    if (y0 < *ymin) *ymin = y0;

    if (abs(y1-y0) < 2)
      continue;
  }
  for (a = 0.5 * M_PI; a < amax; a += delta)
  {
    int y0 = (int)(yc + ih->data->radius * fabs(cos(a)));
    int y1 = (int)(yc + ih->data->radius * fabs(cos(a+delta)));
    cdCanvasForeground(ih->data->cddbuffer, iDialGetFgColor(ih, a, amin));
    cdCanvasBox(ih->data->cddbuffer, IDIAL_SPACE+1, ih->data->w-1-IDIAL_SPACE-2, y0, y1);

    if (y1 > *ymax) *ymax = y1;

    if (abs(y1-y0) < 2)
      continue;
  }
}

static void iDialDrawVertical(Ihandle* ih)
{
  double delta = 2 * M_PI / ih->data->num_div;
  double a, amin, amax;
  int ymin, ymax;

  ih->data->radius = (ih->data->h - 2 * IDIAL_SPACE - 2) / 2.0;

  amin = 0.0;
  amax = M_PI;
  if(ih->data->angle < amin)
  {
    for (a = ih->data->angle; a < amin; a += delta)
      ;
  }
  else
  {
    for (a = ih->data->angle; a > amin; a -= delta)
      ;
    a += delta;
  }

  iDialDrawVerticalBackground(ih, amin, amax, &ymin, &ymax);

  cdIupDrawRaisenRect(ih->data->cddbuffer, IDIAL_SPACE, ymin, ih->data->w-1-IDIAL_SPACE, ymax,
                      ih->data->light_shadow, ih->data->mid_shadow, ih->data->dark_shadow);

  for ( ; a < amax; a += delta)    /* graduation */
  {
    int y;
    if (a < 0.5 * M_PI) y = (int)(ih->data->h / 2.0 - ih->data->radius * cos(a));
    else              y = (int)(ih->data->h / 2.0 + ih->data->radius * fabs(cos(a)));

    if (abs(y-ymin) < 3 || abs(ymax-y) < 3)
      continue;

    cdIupDrawHorizSunkenMark(ih->data->cddbuffer, IDIAL_SPACE+1, ih->data->w-1-IDIAL_SPACE-2, y, ih->data->light_shadow, ih->data->dark_shadow);
  }
}

static void iDialDrawHorizontalBackground(Ihandle* ih,double amin,double amax, int *xmin, int *xmax)
{
  double delta = (0.5 * M_PI - amin) / IDIAL_NCOLORS;
  double a, xc = ih->data->w / 2.0;
  *xmin = *xmax = ih->data->w / 2;
  for (a = amin; a < 0.5 * M_PI; a += delta)
  {
    int x0=(int)(xc - ih->data->radius * cos(a));
    int x1=(int)(xc - ih->data->radius * cos(a + delta));
    cdCanvasForeground(ih->data->cddbuffer,iDialGetFgColor(ih, a, amin));
    cdCanvasBox(ih->data->cddbuffer, x0, x1, IDIAL_SPACE+2, ih->data->h-1-IDIAL_SPACE-1);

    if (x0 < *xmin) *xmin = x0;

    if (abs(x1 - x0) < 2)
      continue;
  }
  for (a = 0.5 * M_PI; a < amax; a += delta)
  {
    int x0 =(int)(xc + ih->data->radius * fabs(cos(a)));
    int x1 =(int)(xc + ih->data->radius * fabs(cos(a + delta)));
    cdCanvasForeground(ih->data->cddbuffer, iDialGetFgColor(ih, a, amin));
    cdCanvasBox(ih->data->cddbuffer, x0, x1, IDIAL_SPACE+2, ih->data->h-1-IDIAL_SPACE-1);

    if (x1 > *xmax) *xmax = x1;

    if (abs(x1-x0) < 2)
      continue;
  }
}

static void iDialDrawHorizontal(Ihandle* ih)
{
  double delta = 2 * M_PI / ih->data->num_div;
  int x;
  double a, amin, amax;
  int xmin, xmax;
  ih->data->radius = (ih->data->w - 2 * IDIAL_SPACE - 2) / 2.0;
  amin = 0.0;
  amax = M_PI;
  if(ih->data->angle < amin)
  {
    for (a = ih->data->angle; a < amin; a += delta) ;
  }
  else
  {
    for (a = ih->data->angle; a > amin; a-= delta) ;
    a += delta;
  }

  iDialDrawHorizontalBackground(ih, amin, amax, &xmin, &xmax);

  cdIupDrawRaisenRect(ih->data->cddbuffer, xmin, IDIAL_SPACE, xmax, ih->data->h-1-IDIAL_SPACE,
                      ih->data->light_shadow, ih->data->mid_shadow, ih->data->dark_shadow);

  for ( ; a < amax; a += delta)
  {
    if (a < 0.5 * M_PI) x = (int)(ih->data->w / 2.0 - ih->data->radius * cos(a));
    else              x = (int)(ih->data->w / 2.0 + ih->data->radius * fabs(cos(a)));

    if (abs(x - xmin) < 3 || abs(xmax - x) < 3)
      continue;

    cdIupDrawVertSunkenMark(ih->data->cddbuffer, x, IDIAL_SPACE+2, ih->data->h-1-IDIAL_SPACE-1, ih->data->light_shadow, ih->data->dark_shadow);
  }
}

static void iDialDrawCircularMark(Ihandle* ih, int x1, int y1)
{
  cdCanvasForeground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasBox(ih->data->cddbuffer,x1, x1+4, y1, y1+4);

  cdCanvasForeground(ih->data->cddbuffer, ih->data->light_shadow);
  cdCanvasLine(ih->data->cddbuffer, x1,   y1+1, x1,   y1+3);
  cdCanvasLine(ih->data->cddbuffer, x1+1, y1+4, x1+3, y1+4);

  cdCanvasForeground(ih->data->cddbuffer, ih->data->mid_shadow);
  cdCanvasLine(ih->data->cddbuffer, x1+1, y1, x1+4, y1);
  cdCanvasLine(ih->data->cddbuffer, x1+4, y1, x1+4, y1+3);

  cdCanvasForeground(ih->data->cddbuffer, ih->data->dark_shadow);
  cdCanvasLine(ih->data->cddbuffer, x1+2, y1,   x1+3, y1);
  cdCanvasLine(ih->data->cddbuffer, x1+4, y1+1, x1+4, y1+2);
}

static void iDialDrawCircular(Ihandle* ih)
{
  double delta = 2 * M_PI / ih->data->num_div, a = ih->data->angle;
  int i, xc = ih->data->w / 2, yc = ih->data->h / 2, wide;
  ih->data->radius = dialmin(ih->data->w, ih->data->h) / 2 - 2 * IDIAL_SPACE;

  wide = (int)(2 * ih->data->radius);
  cdCanvasForeground(ih->data->cddbuffer, ih->data->mid_shadow);
  cdCanvasLineWidth(ih->data->cddbuffer, 2);
  cdCanvasArc(ih->data->cddbuffer, xc, yc, wide-1, wide-1, -135, 45.0);
  cdCanvasLineWidth(ih->data->cddbuffer, 1);
  cdCanvasForeground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasSector(ih->data->cddbuffer, xc, yc, wide-2, wide-2, 0.0, 360.0);
  cdCanvasForeground(ih->data->cddbuffer, ih->data->light_shadow);
  cdCanvasArc(ih->data->cddbuffer, xc, yc, wide, wide, 45, 225);
  cdCanvasForeground(ih->data->cddbuffer, ih->data->dark_shadow);
  cdCanvasArc(ih->data->cddbuffer, xc, yc, wide, wide, -135, 45);

  for (i = 0; i < ih->data->num_div; ++i)
  {
    int x2 = (int)(xc + (ih->data->radius - 6) * cos(a));
    int y2 = (int)(yc + (ih->data->radius - 6) * sin(a));

    if (i == 0)
    {
      cdCanvasForeground(ih->data->cddbuffer, CD_BLACK);
      cdCanvasLine(ih->data->cddbuffer, xc, yc, x2, y2);
    }

    iDialDrawCircularMark(ih, x2-2, y2-2);
    a += delta;
  }

  iDialDrawCircularMark(ih, xc-2, yc-2);
}

static void iDialRepaint(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
    return;

  /* update render */
  cdCanvasBackground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasClear(ih->data->cddbuffer);
  ih->data->Draw(ih);

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
  if (ih->data->has_focus)
    cdIupDrawFocusRect(ih, ih->data->cdcanvas, 0, 0, ih->data->w-1, ih->data->h-1);
}

static void iDialUpdateFgColors(Ihandle* ih, unsigned char r, unsigned char g, unsigned char b)
{
  int i, max, deltar, deltag, deltab;

  /* this function is also called before mapping */
  max = (ih->handle && iupdrvIsActive(ih))? 255 : 192;
  deltar = (max-r) / IDIAL_NCOLORS;
  deltag = (max-g) / IDIAL_NCOLORS;
  deltab = (max-b) / IDIAL_NCOLORS;

  for (i=0; i<=IDIAL_NCOLORS; i++)
  {
    ih->data->fgcolor[i] = cdEncodeColor(r, g, b);
    r = (unsigned char)(r + deltar);
    g = (unsigned char)(g + deltag);
    b = (unsigned char)(b + deltab);
  }
}

static int iDialButtonPress(Ihandle* ih, int button, int x, int y)
{
  IFn cb;
  
  if (button!=IUP_BUTTON1)    
    return IUP_DEFAULT;

  y = cdIupInvertYAxis(y, ih->data->h);
  ih->data->px = x;
  ih->data->py = y;

  if (ih->data->orientation != IDIAL_CIRCULAR)
    ih->data->angle=0;

  cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
  if (cb)
    cb(ih);
  else
  {
    IFnd cb_old = (IFnd) IupGetCallback(ih, "BUTTON_PRESS_CB");
    if (cb_old)
      cb_old(ih, ih->data->angle * ih->data->unit);
  }

  return IUP_DEFAULT;
}

static int iDialButtonRelease(Ihandle* ih, int button)
{
  IFn cb;
  
  if (button!=IUP_BUTTON1)    
    return IUP_DEFAULT;

  iDialRepaint(ih);

  cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
  if (cb)
    cb(ih);
  else
  {
    IFnd cb_old = (IFnd) IupGetCallback(ih, "BUTTON_RELEASE_CB");
    if (cb_old)
      cb_old(ih, ih->data->angle * ih->data->unit);
  }

  return IUP_DEFAULT;
}


/******************************************************************/


static int iDialMotionVertical_CB(Ihandle* ih, int x, int y, char *status)
{
  IFn cb;
  (void)x; /* not used */
  
  if (!iup_isbutton1(status))     
    return IUP_DEFAULT;

  y = cdIupInvertYAxis(y, ih->data->h);
  ih->data->angle += (double)(y-ih->data->py) / ih->data->radius;
  ih->data->py = y;

  iDialRepaint(ih);

  cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
  if (cb)
    cb(ih);
  else
  {
    IFnd cb_old = (IFnd) IupGetCallback(ih, "MOUSEMOVE_CB");
    if (cb_old)
      cb_old(ih, ih->data->angle * ih->data->unit);
  }

  return IUP_DEFAULT;
}

static int iDialMotionHorizontal_CB(Ihandle* ih, int x, int y, char *status)
{
  IFn cb;
  
  if (!iup_isbutton1(status))     
    return IUP_DEFAULT;

  y = cdIupInvertYAxis(y, ih->data->h);
  ih->data->angle += (double)(x-ih->data->px) / ih->data->radius;
  ih->data->px = x;

  iDialRepaint(ih);

  cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
  if (cb)
    cb(ih);
  else
  {
    IFnd cb_old = (IFnd) IupGetCallback(ih, "MOUSEMOVE_CB");
    if (cb_old)
      cb_old(ih, ih->data->angle * ih->data->unit);
  }

  return IUP_DEFAULT;
}

static int iDialMotionCircular_CB(Ihandle* ih, int x, int y, char *status)
{
  int cx = ih->data->w / 2;
  int cy = ih->data->h / 2;
  double vet, xa, ya, xb, yb, ma, mb, ab;
  IFn cb;

  if (!iup_isbutton1(status))     
    return IUP_DEFAULT;

  y = cdIupInvertYAxis(y, ih->data->h);

  xa = ih->data->px-cx; 
  ya = ih->data->py-cy; 
  ma = sqrt(xa * xa + ya * ya);

  xb = x - cx;  
  yb = y - cy;  
  mb = sqrt(xb * xb + yb * yb);

  ab  = xa * xb + ya * yb;
  vet = xa * yb - xb * ya;

  ab = ab / (ma * mb);

  /* if the mouse is in the center of the dial, ignore it */
  if (ma == 0 || mb == 0 || ab < -1 || ab > 1)
    return IUP_DEFAULT;

  if (vet>0) ih->data->angle += acos(ab);
  else       ih->data->angle -= acos(ab);

  iDialRepaint(ih);
  ih->data->px = x;
  ih->data->py = y;

  cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
  if (cb)
    cb(ih);
  else
  {
    IFnd cb_old = (IFnd) IupGetCallback(ih, "MOUSEMOVE_CB");
    if (cb_old)
      cb_old(ih, ih->data->angle * ih->data->unit);
  }

  return IUP_DEFAULT;
}

static int iDialButton_CB(Ihandle* ih, int button, int pressed, int x, int y)
{
  if (pressed) 
    return iDialButtonPress(ih, button, x, y);
  else   
    return iDialButtonRelease(ih, button);
}

static int iDialResize_CB(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
  {
    /* update canvas size */
    cdCanvasActivate(ih->data->cdcanvas);

    /* this can fail if canvas size is zero */
    ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);
  }

  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  /* update size */
  cdCanvasActivate(ih->data->cddbuffer);
  cdCanvasGetSize(ih->data->cddbuffer, &ih->data->w, &ih->data->h, NULL, NULL);

  /* update number of divisions */
  switch(ih->data->orientation)
  {
  case IDIAL_VERTICAL:
    ih->data->num_div = (int)((ih->data->h-2 * IDIAL_SPACE-2) * ih->data->density);
    break;

  case IDIAL_HORIZONTAL:
    ih->data->num_div = (int)((ih->data->w-2 * IDIAL_SPACE-2) * ih->data->density);
    break;

  case IDIAL_CIRCULAR:
    ih->data->num_div = (int)((dialmin(ih->data->w, ih->data->h)-2 * IDIAL_SPACE-2) * ih->data->density);
    break;
  }

  if (ih->data->num_div < 3) ih->data->num_div = 3;

  /* update render */
  cdCanvasBackground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasClear(ih->data->cddbuffer);
  ih->data->Draw(ih);
  
  return IUP_DEFAULT;
}

static int iDialRedraw_CB(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
  if (ih->data->has_focus)
    cdIupDrawFocusRect(ih, ih->data->cdcanvas, 0, 0, ih->data->w-1, ih->data->h-1);

  return IUP_DEFAULT;
}

static int iDialFocus_CB(Ihandle* ih, int focus)
{
  ih->data->has_focus = focus;
  iDialRepaint(ih);
  return IUP_DEFAULT;
}

static int iDialKeyPress_CB(Ihandle* ih, int c, int press)
{
  IFn cb;
  char* cb_name;

  if (c != K_LEFT   && c != K_UP   &&
      c != K_sLEFT  && c != K_sUP  &&
      c != K_RIGHT  && c != K_DOWN &&
      c != K_sRIGHT && c != K_sDOWN &&
      c != K_HOME)
    return IUP_DEFAULT;

  if (press && ih->data->pressing)
  {
    switch(c)
    {
    case K_sLEFT:
    case K_sDOWN:
      ih->data->angle -= M_PI / 100.0;
      break;
    case K_LEFT:
    case K_DOWN:
      ih->data->angle -= M_PI / 10.0;
      break;
    case K_sRIGHT:
    case K_sUP:
      ih->data->angle += M_PI / 100.0;
      break;
    case K_RIGHT:
    case K_UP:
      ih->data->angle += M_PI / 10.0;
      break;
    }
  }

  if (c == K_HOME)
    ih->data->angle = 0;

  if (press)
  {
    if (ih->data->pressing)
    {
      cb_name = "MOUSEMOVE_CB";
    }
    else
    {
      ih->data->pressing = 1;
      if (ih->data->orientation != IDIAL_CIRCULAR)
        ih->data->angle = 0;
      cb_name = "BUTTON_PRESS_CB";
    }
  }
  else
  {
    ih->data->pressing = 0;
    cb_name = "RELEASE_CB";
  }

  iDialRepaint(ih);

  cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
  if (cb)
    cb(ih);
  else
  {
    IFnd cb_old = (IFnd) IupGetCallback(ih, cb_name);
    if (cb_old)
      cb_old(ih, ih->data->angle * ih->data->unit);
  }

  return IUP_IGNORE;  /* to avoid arrow keys being processed by the system */
}

static int iDialWheel_CB(Ihandle* ih, float delta)
{
  IFn cb;
  
  ih->data->angle += ((double)delta) * (M_PI / 10.0);

  if (fabs(ih->data->angle) < M_PI / 10.1)
    ih->data->angle = 0;

  iDialRepaint(ih);

  cb = (IFn)IupGetCallback(ih, "VALUECHANGED_CB");
  if (cb)
    cb(ih);
  else
  {
    IFnd cb_old = (IFnd) IupGetCallback(ih, "MOUSEMOVE_CB");
    if (cb_old)
      cb_old(ih, ih->data->angle * ih->data->unit);
  }

  return IUP_DEFAULT;
}


/*********************************************************************************/


static char* iDialGetValueAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%g", ih->data->angle);
  return str;
}

static int iDialSetValueAttrib(Ihandle* ih, const char* value)
{
  if (!value) /* reset to default */
    ih->data->angle = 0;
  else
    ih->data->angle = atof(value);

  iDialRepaint(ih);
  return 0; /* do not store value in hash table */
}

static int iDialSetDensityAttrib(Ihandle* ih, const char* value)
{
  ih->data->density = atof(value);
  iDialRepaint(ih);
  return 0;   /* do not store value in hash table */
}

static char* iDialGetDensityAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%g", ih->data->density);
  return str;
}

static int iDialSetBgColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = iupControlBaseGetParentBgColor(ih);

  ih->data->bgcolor = cdIupConvertColor(value);

  cdIupCalcShadows(ih->data->bgcolor, &ih->data->light_shadow, &ih->data->mid_shadow, &ih->data->dark_shadow);
  if (!iupdrvIsActive(ih))
    ih->data->light_shadow = ih->data->mid_shadow;
  
  iDialRepaint(ih);
  return 1;
}

static int iDialSetFgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;
  iDialUpdateFgColors(ih, r, g, b);

  iDialRepaint(ih);
  return 1;
}

static int iDialSetActiveAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;

  iupBaseSetActiveAttrib(ih, value);

  value = iupAttribGetStr(ih, "FGCOLOR");
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;
  iDialUpdateFgColors(ih, r, g, b);

  cdIupCalcShadows(ih->data->bgcolor, &ih->data->light_shadow, &ih->data->mid_shadow, &ih->data->dark_shadow);
  if (!iupdrvIsActive(ih))
    ih->data->light_shadow = ih->data->mid_shadow;

  iDialRepaint(ih);
  return 0;   /* do not store value in hash table */
}

static int iDialSetUnitAttrib(Ihandle* ih, const char* value)
{
  ih->data->unit = iupStrEqualNoCase(value, "DEGREES")? CD_RAD2DEG : 1.0;
  return 1;
}

static int iDialSetOrientationAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (iupStrEqualNoCase(value, "VERTICAL"))
  {
    ih->data->Draw = iDialDrawVertical;
    ih->data->orientation = IDIAL_VERTICAL;
    IupSetCallback(ih, "MOTION_CB", (Icallback)iDialMotionVertical_CB);
    IupSetAttribute(ih, "SIZE", "16x80");
  }
  else if (iupStrEqualNoCase(value, "CIRCULAR"))
  {
    ih->data->Draw = iDialDrawCircular;
    ih->data->orientation = IDIAL_CIRCULAR;
    IupSetCallback(ih, "MOTION_CB", (Icallback)iDialMotionCircular_CB);
    IupSetAttribute(ih, "SIZE", "40x36");
  }
  else /* "HORIZONTAL" */
  {
    ih->data->Draw = iDialDrawHorizontal;
    ih->data->orientation = IDIAL_HORIZONTAL;
    IupSetCallback(ih, "MOTION_CB", (Icallback)iDialMotionHorizontal_CB);
    IupSetAttribute(ih, "SIZE", "80x16");
  }
  return 0; /* do not store value in hash table */
}

static char* iDialGetOrientationAttrib(Ihandle* ih)
{
  if (ih->data->orientation == IDIAL_HORIZONTAL)
    return "HORIZONTAL";
  else if (ih->data->orientation == IDIAL_VERTICAL)
    return "VERTICAL";
  else /* (ih->data->orientation == IDIAL_CIRCULAR) */
    return "CIRCULAR";
}


/****************************************************************************/


static int iDialMapMethod(Ihandle* ih)
{
  ih->data->cdcanvas = cdCreateCanvas(CD_IUP, ih);
  if (!ih->data->cdcanvas)
    return IUP_ERROR;

  /* this can fail if canvas size is zero */
  ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);

  return IUP_NOERROR;
}

static void iDialUnMapMethod(Ihandle* ih)
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

static int iDialCreateMethod(Ihandle* ih, void **params)
{
  char* orientation = "HORIZONTAL";
  if (params && params[0])
    orientation = params[0];

  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  iupAttribSetStr(ih, "BORDER", "NO");
  ih->expand = IUP_EXPAND_NONE;

  /* default values */
  iDialSetOrientationAttrib(ih, orientation);
  ih->data->density = IDIAL_DEFAULT_DENSITY; 
  ih->data->unit = 1.0;  /* RADIANS */
  ih->data->num_div = 3;
  iDialUpdateFgColors(ih, IDIAL_DEFAULT_FGCOLOR_COMP, IDIAL_DEFAULT_FGCOLOR_COMP, IDIAL_DEFAULT_FGCOLOR_COMP);

  /* IupCanvas callbacks */
  IupSetCallback(ih, "ACTION", (Icallback)iDialRedraw_CB);
  IupSetCallback(ih, "RESIZE_CB", (Icallback)iDialResize_CB);
  IupSetCallback(ih, "BUTTON_CB", (Icallback)iDialButton_CB);
  IupSetCallback(ih, "FOCUS_CB", (Icallback)iDialFocus_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iDialKeyPress_CB);
  IupSetCallback(ih, "WHEEL_CB", (Icallback)iDialWheel_CB);

  return IUP_NOERROR;
}

Iclass* iupDialNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "dial";
  ic->format = "s"; /* one string */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupDialNewClass;
  ic->Create  = iDialCreateMethod;
  ic->Map     = iDialMapMethod;
  ic->UnMap   = iDialUnMapMethod;

  /* Do not need to set base attributes because they are inherited from IupCanvas */

  /* IupDial Callbacks */
  iupClassRegisterCallback(ic, "MOUSEMOVE_CB", "d");
  iupClassRegisterCallback(ic, "BUTTON_PRESS_CB", "d");
  iupClassRegisterCallback(ic, "BUTTON_RELEASE_CB", "d");
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");

  /* IupDial only */
  iupClassRegisterAttribute(ic, "VALUE", iDialGetValueAttrib, iDialSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TYPE", iDialGetOrientationAttrib, iDialSetOrientationAttrib, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ORIENTATION", iDialGetOrientationAttrib, iDialSetOrientationAttrib, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DENSITY", iDialGetDensityAttrib, iDialSetDensityAttrib, IDIAL_DEFAULT_DENSITY_STR, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iDialSetFgColorAttrib, IDIAL_DEFAULT_FGCOLOR, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "UNIT", NULL, iDialSetUnitAttrib, IUPAF_SAMEASSYSTEM, "RADIANS", IUPAF_NOT_MAPPED);

  /* Overwrite IupCanvas Attributes */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iDialSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", iupControlBaseGetBgColorAttrib, iDialSetBgColorAttrib, NULL, "255 255 255", IUPAF_NO_INHERIT);    /* overwrite canvas implementation, set a system default to force a new default */

  return ic;
}

Ihandle* IupDial(const char* orientation)
{
  void *params[2];
  params[0] = (void*)orientation;
  params[1] = NULL;
  return IupCreatev("dial", params);
}
