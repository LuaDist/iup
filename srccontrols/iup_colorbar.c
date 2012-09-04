/** \file
 * \brief Colorbar Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcontrols.h"
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
#include "iup_image.h"
#include "iup_cdutil.h"
#include "iup_register.h"


#define ICOLORBAR_DEFAULT_NUM_CELLS 16  /* default number of cells */
#define ICOLORBAR_NO_COLOR 0xff000000   /* no color                */
#define ICOLORBAR_DELTA 5               /* preview margin          */
#define ICOLORBAR_RENDER_ALL  -1
#define ICOLORBAR_RENDER_NONE -2


struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  int w;                  /* size of the canvas - width                             */
  int h;                  /* size of the canvas - height                            */
  cdCanvas* cdcanvas;     /* cd canvas for drawing                                  */
  cdCanvas* cddbuffer;    /* image canvas for double buffering                      */
  long int colors[256];   /* CD color vector                                        */
  int num_cells;          /* number of cells at the widgets                         */
  int num_parts;          /* number of sections used to split the colors cells area */
  int vertical;           /* vertical orientation flag                              */
  int squared;            /* square cells attribute flag                            */
  int shadowed;           /* 3D shadowed attribute flag                             */
  long bgcolor;           /* control background color                               */
  long light_shadow;      /* }                                                      */
  long mid_shadow;        /* } 3D shadowed color                                    */
  long dark_shadow;       /* }                                                      */
  int bufferize;          /* bufferation flag                                       */
  long int transparency;  /* transparency color                                     */
  int show_secondary;     /* secondary color selction flag                          */
  int preview_size;       /* preview size (pixels) 0=disabled, -1=automatic         */
  int fgcolor_idx;        /* current primary index selected                         */
  int bgcolor_idx;        /* current secondary index selected                       */
  int focus_cell;         /* cell with focus                                        */
  int has_focus;          /* 1 if the control has the focus, else 0                 */
};

/* Default colors used for a widget */
static struct {
  unsigned int r;
  unsigned int g;
  unsigned int b;
} default_colors[ICOLORBAR_DEFAULT_NUM_CELLS] = {
  {   0,   0,   0 }, { 128,   0,   0 }, {   0, 128,   0 }, { 128, 128,   0 },    
  {   0,   0, 128 }, { 128,   0, 128 }, {   0, 128, 128 }, { 192, 192, 192 },    
  { 128, 128, 128 }, { 255,   0,   0 }, {   0, 255,   0 }, { 255, 255,   0 },
  {   0,   0, 255 }, { 255,   0, 255 }, {   0, 255, 255 }, { 255, 255, 255 }
};


/* This function draw the 3D cell effect. */
static void iColorbarDrawSunken(Ihandle* ih, int xmin, int xmax, int ymin, int ymax)
{
  if (!ih->data->shadowed) return;
  cdIupDrawSunkenRect(ih->data->cddbuffer, xmin, ymin, xmax, ymax, ih->data->light_shadow, ih->data->mid_shadow, ih->data->dark_shadow);
}

/* This function is used to draw a box for a cell. */
static void iColorbarDrawBox(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, int idx)
{
  long int color = ih->data->colors[idx];
  cdCanvasInteriorStyle(ih->data->cddbuffer, CD_SOLID);

  if (color == ih->data->transparency)
  { 
    int xm = (xmin+xmax)/2;
    int ym = (ymin+ymax)/2;
    cdCanvasForeground(ih->data->cddbuffer,0xeeeeee);
    cdCanvasBox(ih->data->cddbuffer,xmin, xm, ymin, ym);
    cdCanvasBox(ih->data->cddbuffer,xm, xmax, ym, ymax);
    cdCanvasForeground(ih->data->cddbuffer,0xcccccc);
    cdCanvasBox(ih->data->cddbuffer,xmin, xm, ym, ymax);
    cdCanvasBox(ih->data->cddbuffer,xm, xmax, ymin, ym);
  }
  else
  {
    if (!iupdrvIsActive(ih))
    {
      unsigned char r, g, b, bg_r, bg_g, bg_b;
      cdDecodeColor(color, &r, &g, &b);
      cdDecodeColor(ih->data->bgcolor, &bg_r, &bg_g, &bg_b);
      iupImageColorMakeInactive(&r, &g, &b, bg_r, bg_g, bg_b);
      color = cdEncodeColor(r, g, b);
    }
    cdCanvasForeground(ih->data->cddbuffer,color);
    cdCanvasBegin(ih->data->cddbuffer,CD_FILL);
    cdCanvasVertex(ih->data->cddbuffer,xmin, ymin); cdCanvasVertex(ih->data->cddbuffer,xmin, ymax);
    cdCanvasVertex(ih->data->cddbuffer,xmax, ymax); cdCanvasVertex(ih->data->cddbuffer,xmax, ymin);
    cdCanvasEnd(ih->data->cddbuffer);
  }

  cdCanvasForeground(ih->data->cddbuffer,CD_BLACK);
  cdCanvasBegin(ih->data->cddbuffer,CD_CLOSED_LINES);
  cdCanvasVertex(ih->data->cddbuffer,xmin, ymin); cdCanvasVertex(ih->data->cddbuffer,xmin, ymax);
  cdCanvasVertex(ih->data->cddbuffer,xmax, ymax); cdCanvasVertex(ih->data->cddbuffer,xmax, ymin);
  cdCanvasEnd(ih->data->cddbuffer);
}

/* This function is used to get the largest square of a cell bounding box. */
static void iColorbarFitSquare(int* xmin, int* xmax, int* ymin, int* ymax)
{
  int mx = (*xmax + *xmin) / 2; 
  int my = (*ymax + *ymin) / 2;
  int dx = (*xmax - *xmin) / 2; 
  int dy = (*ymax - *ymin) / 2;

  if (dx < dy)
  { 
    *ymin = my - dx; 
    *ymax = my + dx; 
  }
  else
  { 
    *xmin = mx - dy; 
    *xmax = mx + dy; 
  }
}

/* This function is used to get the preview area bounding box. */
static void iColorbarGetPreviewLimit(Ihandle* ih, int* xmin, int* xmax, int* ymin, int* ymax)
{
  int num_itens = ih->data->num_cells / ih->data->num_parts + 1;  /* include space for preview area */

  *xmin = 0; *ymin = 0;
  if (ih->data->vertical)
  { 
    *xmax = ih->data->w;
    if (ih->data->preview_size > 0)
      *ymax = *ymin + ih->data->preview_size;
    else
      *ymax = ih->data->h / num_itens;
  }
  else
  { 
    *ymax = ih->data->h;
    if (ih->data->preview_size > 0)
      *xmax = *xmin + ih->data->preview_size;
    else
      *xmax = ih->data->w / num_itens;
  }
  if (ih->data->squared)
    iColorbarFitSquare(xmin, xmax, ymin, ymax);
}

/* This function is used to get a cell bounding box. */
static void iColorbarGetCellLimit(Ihandle* ih, int idx, int* xmin, int* xmax, int* ymin, int* ymax)
{
  int delta, dummy;
  int wcell, hcell;
  int px = 0, py = 0;
  int posx, posy;
  int num_itens = ih->data->num_cells / ih->data->num_parts;

  if (ih->data->preview_size != 0)
    iColorbarGetPreviewLimit(ih, &dummy, &px, &dummy, &py);

  if (ih->data->vertical)  /* Vertical orientation */
  { 
    wcell = ih->data->w / ih->data->num_parts;
    hcell = (ih->data->h - py) / num_itens;
    posx = idx / num_itens;
    posy = idx % num_itens;
    if (ih->data->squared)
    { 
      wcell = wcell < hcell ? wcell : hcell;
      hcell = wcell;
    }
    delta = (ih->data->w - (ih->data->num_parts*wcell)) / 2;
    *xmin = delta + (posx+0)*wcell; 
    *xmax = delta + (posx+1)*wcell;
    *ymin = py + (posy+0)*hcell;
    *ymax = py + (posy+1)*hcell;
  }
  else  /* Horizontal orientation */
  {  
    hcell = ih->data->h / ih->data->num_parts;
    wcell = (ih->data->w - px) / num_itens;
    posx = idx % num_itens;
    posy = idx / num_itens;
    if (ih->data->squared)
    { 
      wcell = wcell < hcell ? wcell : hcell;
      hcell = wcell;
    }
    delta = (ih->data->h - (ih->data->num_parts * hcell)) / 2;
    *xmin = px + (posx + 0) * wcell; 
    *xmax = px + (posx + 1) * wcell;
    *ymin = delta + (posy + 0) * hcell;
    *ymax = delta + (posy + 1) * hcell;
  }
}

/* This function is used to get the index color of a canvas coordinate. */
static int iColorbarGetIndexColor(Ihandle* ih, int x, int y)
{
  int i;
  int xmin, ymin;
  int xmax, ymax;
  int result = -9; 

  for (i = 0; i < ih->data->num_cells; i++)
  { 
    iColorbarGetCellLimit(ih, i, &xmin, &xmax, &ymin, &ymax);
    if (x > xmin && x < xmax && y > ymin && y < ymax)
    { 
      result = i;
      break;
    }
  }
  return result;
}

/* This function is used to repaint the preview area. */
static void iColorbarRenderPreview(Ihandle* ih)
{
  int delta = ICOLORBAR_DELTA;
  int xmin,  ymin;
  int xmax,  ymax;
  int xhalf, yhalf;
  int bg = ih->data->bgcolor_idx;
  int fg = ih->data->fgcolor_idx;

  if (ih->data->preview_size == 0)
    return;

  iColorbarGetPreviewLimit(ih, &xmin, &xmax, &ymin, &ymax);

  if (xmax-xmin < delta || ymax-ymin < delta)
    delta = 0;
  
  if (ih->data->show_secondary)
  { 
    xhalf = 2 * (xmax - xmin - 2 * delta) / 3 + delta;
    yhalf = 2 * (ymax - ymin - 2 * delta) / 3 + delta;

    iColorbarDrawBox(ih, xmax - xhalf, xmax - delta, ymin + delta, ymin + yhalf, bg);
    iColorbarDrawBox(ih, xmin + delta, xmin + xhalf, ymax - yhalf, ymax - delta, fg);
  }
  else
  { 
    iColorbarDrawBox(ih, xmin + delta, xmax - delta, ymin + delta, ymax - delta, fg);
  }
}

static void iColorbarDrawFocusCell(Ihandle* ih)
{
  int delta = 4;
  int xmin, ymin;
  int xmax, ymax;

  iColorbarGetCellLimit(ih, ih->data->focus_cell, &xmin, &xmax, &ymin, &ymax);
  xmin += delta;
  xmax -= delta;
  ymin += delta;
  ymax -= delta;

  cdIupDrawFocusRect(ih, ih->data->cdcanvas, xmin, ymin, xmax, ymax);
}

/* This function is used to repaint a cell. */
static void iColorbarRenderCell(Ihandle* ih, int idx)
{
  int delta = 2;
  int xmin, ymin;
  int xmax, ymax;

  iColorbarGetCellLimit(ih, idx, &xmin, &xmax, &ymin, &ymax);
  xmin += delta;
  xmax -= delta;
  ymin += delta;
  ymax -= delta;
  
  iColorbarDrawBox(ih, xmin, xmax, ymin, ymax, idx);
  iColorbarDrawSunken(ih, xmin, xmax, ymin, ymax);
}

/* This function loops all cells, repainting them. */
static void iColorbarRenderCells(Ihandle* ih)
{
  int i;
  for (i = 0; i < ih->data->num_cells; i++)
    iColorbarRenderCell(ih, i);
}

static void iColorbarRepaint(Ihandle* ih)
{
  /* Checking errors or not initialized conditions */
  if (ih->data->cddbuffer == NULL)
    return;

  /* If object is buffering, it will be not drawn */
  if (ih->data->bufferize == 1)
    return;

  /* update render */
  cdCanvasBackground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasClear(ih->data->cddbuffer);
  iColorbarRenderPreview(ih);
  iColorbarRenderCells(ih);

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
  if (ih->data->has_focus)
    iColorbarDrawFocusCell(ih);
}

static int iColorbarCheckPreview(Ihandle* ih, int x, int y)
{
  int xmin,  ymin;
  int xmax,  ymax;
  int xhalf, yhalf;
  int delta = ICOLORBAR_DELTA;

  iColorbarGetPreviewLimit(ih, &xmin, &xmax, &ymin, &ymax);

  if (ih->data->show_secondary)
  { 
    xhalf = 2 * (xmax - xmin - 2 * delta) / 3 + delta;
    yhalf = 2 * (ymax - ymin - 2 * delta) / 3 + delta;

    if (x > xmin+delta && x < xmin+xhalf && y > ymax-yhalf && y < ymax-delta)
      return IUP_PRIMARY;
    if (x > xmax-xhalf && x < xmax-delta && y > ymin+delta && y < ymin+yhalf)
      return IUP_SECONDARY;
    if (x > xmin && x < xmax && y > ymin && y < ymax)
      return 1;  /* switch */
  }
  else
  { 
    if (x > xmin+delta && x < xmax-delta && y > ymin+delta && y < ymax-delta)
      return IUP_PRIMARY;
  }

  return 0;
}

static int iColorbarSetNumPartsAttrib(Ihandle* ih, const char* value)
{
  if (iupStrToInt(value, &ih->data->num_parts))
    iColorbarRepaint(ih);
  return 0;
}

static char* iColorbarGetNumPartsAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  sprintf(buffer, "%d", ih->data->num_parts);
  return buffer;
}

static int iColorbarSetPrimaryCellAttrib(Ihandle* ih, const char* value)
{
  int new_val;
  if (iupStrToInt(value, &new_val))
  {
    if (new_val > 0 && new_val < ih->data->num_cells)
    { 
      ih->data->fgcolor_idx = new_val;
      iColorbarRepaint(ih);
    }
  }
  return 0;
}

static char* iColorbarGetPrimaryCellAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  sprintf(buffer, "%d", ih->data->fgcolor_idx);
  return buffer;
}

static int iColorbarSetSecondaryCellAttrib(Ihandle* ih, const char* value)
{
  int new_val;
  if (iupStrToInt(value, &new_val))
  {
    if (new_val > 0 && new_val < ih->data->num_cells)
    { 
      ih->data->bgcolor_idx = new_val;
      iColorbarRepaint(ih);
    }
  }

  return 0;
}

static char* iColorbarGetSecondaryCellAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  sprintf(buffer, "%d", ih->data->bgcolor_idx);
  return buffer;
}

static int iColorbarSetBufferizeAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NO"))
  { 
    ih->data->bufferize = 0;
    iColorbarRepaint(ih);
  }
  else
    ih->data->bufferize = 1;

  return 0;
}

static char* iColorbarGetBufferizeAttrib(Ihandle* ih)
{
  if (ih->data->bufferize)
    return "YES";
  else
    return "NO";
}

static int iColorbarSetNumCellsAttrib(Ihandle* ih, const char* value)
{
  int new_val;
  if (iupStrToInt(value, &new_val))
  {
    if (new_val > 0 && new_val <= 256)
    { 
      ih->data->num_cells = new_val;

      if (ih->data->fgcolor_idx >= ih->data->num_cells)
        ih->data->fgcolor_idx = ih->data->num_cells - 1;

      if (ih->data->bgcolor_idx >= ih->data->num_cells)
        ih->data->bgcolor_idx = ih->data->num_cells - 1;

      iColorbarRepaint(ih);
    }
  }
  return 0;
}

static char* iColorbarGetNumCellsAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  sprintf(buffer, "%d", ih->data->num_cells);
  return buffer;
}

static int iColorbarSetOrientationAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "HORIZONTAL"))
    ih->data->vertical = 0;
  else
    ih->data->vertical = 1;

  iColorbarRepaint(ih);
  return 0;
}

static char* iColorbarGetOrientationAttrib(Ihandle* ih)
{
  if (ih->data->vertical) 
    return "VERTICAL";
  else 
    return "HORIZONTAL";
}

static int iColorbarSetSquaredAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NO"))
    ih->data->squared = 0;
  else
    ih->data->squared = 1;
  
  iColorbarRepaint(ih);
  return 0;
}

static char* iColorbarGetSquaredAttrib(Ihandle* ih)
{
  if (ih->data->squared) 
    return "YES";
  else 
    return "NO";
}

static int iColorbarSetShadowedAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NO"))
    ih->data->shadowed = 0;
  else
    ih->data->shadowed = 1;

  iColorbarRepaint(ih);
  return 0;
}

static char* iColorbarGetShadowedAttrib(Ihandle* ih)
{
  if (ih->data->shadowed) 
    return "YES";
  else 
    return "NO";
}

static int iColorbarSetShowSecondaryAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NO"))
    ih->data->show_secondary = 0;
  else
    ih->data->show_secondary = 1;
  
  iColorbarRepaint(ih);
  return 0;
}

static char* iColorbarGetShowSecondaryAttrib(Ihandle* ih)
{
  if (ih->data->show_secondary) 
    return "YES";
  else 
    return "NO";
}

static int iColorbarSetShowPreviewAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NO"))
    ih->data->preview_size = 0;
  else
    ih->data->preview_size = -1;  /* reset to automatic */
  
  iColorbarRepaint(ih);
  return 1;
}

static int iColorbarSetPreviewSizeAttrib(Ihandle* ih, const char* value)
{
  if (iupStrToInt(value, &ih->data->preview_size))
    iColorbarRepaint(ih);
  return 0;
}

static char* iColorbarGetPreviewSizeAttrib(Ihandle* ih)
{
  if (ih->data->preview_size == -1)  /* automatic */
    return NULL;
  else 
  {
    char* buffer = iupStrGetMemory(100);
    sprintf(buffer, "%d", ih->data->preview_size);
    return buffer;
  }
}

static int iColorbarSetCellAttrib(Ihandle* ih, int id, const char* value)
{
  if (id >= 0 || id < ih->data->num_cells)
  { 
    ih->data->colors[id] = cdIupConvertColor(value);
    iColorbarRepaint(ih);
  }

  return 0;
}

static char* iColorbarGetCellAttrib(Ihandle* ih, int id)
{
  char* buffer = iupStrGetMemory(100);
  long color;
  
  if (id < 0 || id >= ih->data->num_cells)
    return NULL;

  color = ih->data->colors[id];
  sprintf(buffer, "%d %d %d", cdRed(color), cdGreen(color), cdBlue(color));
  return buffer;
}

static int iColorbarSetTransparencyAttrib(Ihandle* ih, const char* value)
{
  if (value == NULL)
    ih->data->transparency = ICOLORBAR_NO_COLOR;
  else
    ih->data->transparency = cdIupConvertColor(value);

  iColorbarRepaint(ih);
  return 0;
}

static char* iColorbarGetTransparencyAttrib(Ihandle* ih)
{
  if (ih->data->transparency == ICOLORBAR_NO_COLOR)
    return NULL;
  else
  {
    char* buffer = iupStrGetMemory(100);
    sprintf(buffer, "%d %d %d", cdRed(ih->data->transparency), cdGreen(ih->data->transparency), cdBlue(ih->data->transparency));
    return buffer;
  }
}

static int iColorbarSetBgColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = iupControlBaseGetParentBgColor(ih);

  ih->data->bgcolor = cdIupConvertColor(value);

  cdIupCalcShadows(ih->data->bgcolor, &ih->data->light_shadow, &ih->data->mid_shadow, &ih->data->dark_shadow);
  if (!iupdrvIsActive(ih))
    ih->data->light_shadow = ih->data->mid_shadow;
  
  iColorbarRepaint(ih);
  return 1;
}

static int iColorbarSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);

  cdIupCalcShadows(ih->data->bgcolor, &ih->data->light_shadow, &ih->data->mid_shadow, &ih->data->dark_shadow);
  if (!iupdrvIsActive(ih))
    ih->data->light_shadow = ih->data->mid_shadow;

  iColorbarRepaint(ih);
  return 0;   /* do not store value in hash table */
}

static int iColorbarRedraw_CB(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
  if (ih->data->has_focus)
    iColorbarDrawFocusCell(ih);

  return IUP_DEFAULT;
}

static int iColorbarResize_CB(Ihandle* ih)
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

  /* update render */
  cdCanvasBackground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasClear(ih->data->cddbuffer);
  iColorbarRenderPreview(ih);
  iColorbarRenderCells(ih);

  return IUP_DEFAULT;
}

static void iColorbarRenderPartsRepaint(Ihandle* ih, int preview, int idx)
{
  /* update render */
  if (preview)
    iColorbarRenderPreview(ih);
  
  if (idx != ICOLORBAR_RENDER_NONE)
  {
    if (idx == ICOLORBAR_RENDER_ALL)
      iColorbarRenderCells(ih);
    else
      iColorbarRenderCell(ih, idx);
  }

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
  if (ih->data->has_focus)
    iColorbarDrawFocusCell(ih);
}

static int iColorbarFocus_CB(Ihandle* ih, int focus)
{
  ih->data->has_focus = focus;

  if (ih->data->cddbuffer)
  {
    cdCanvasFlush(ih->data->cddbuffer);
    if (ih->data->has_focus)
      iColorbarDrawFocusCell(ih);
  }

  return IUP_DEFAULT;
}

static void iColorbarCallExtentedCb(Ihandle* ih, int idx)
{
  IFni extended_cb = (IFni)IupGetCallback(ih, "EXTENDED_CB");
  if (!extended_cb)
    return;

  if (extended_cb(ih, idx) == IUP_IGNORE)
    return;

  iColorbarRenderPartsRepaint(ih, 1, ICOLORBAR_RENDER_ALL);   /* the preview and all the cells are rendered */
}
    
static void iColorbarCallSelectCb(Ihandle* ih, int idx, int type)
{ 
  IFnii select_cb;

  if (type == IUP_SECONDARY && !ih->data->show_secondary)
    return;

  select_cb = (IFnii)IupGetCallback(ih, "SELECT_CB");
  if (!select_cb)
    return;

  if (select_cb(ih, idx, type) == IUP_IGNORE)
    return;

  if (type == IUP_PRIMARY)
    ih->data->fgcolor_idx = idx;
  else
    ih->data->bgcolor_idx = idx;

  iColorbarRenderPartsRepaint(ih, 1, ICOLORBAR_RENDER_NONE);   /* only the preview area is rendered */
}

static void iColorbarCallCellCb(Ihandle* ih, int idx)
{
  char* returned;
  sIFni cell_cb = (sIFni)IupGetCallback(ih, "CELL_CB");
  if (!cell_cb)
    return;

  returned = cell_cb(ih, idx);  /* the application can change the color */
  if (returned) 
  {
    int preview = 0;
    /* check if the preview area should be rendered */
    if (idx == ih->data->fgcolor_idx || idx == ih->data->bgcolor_idx)
      preview = 1;

    ih->data->colors[idx] = cdIupConvertColor(returned);
    iColorbarRenderPartsRepaint(ih, preview, idx);   /* the preview and the cell are rendered */
  }
}

static int iColorbarKeyPress_CB(Ihandle* ih, int c, int press)
{
  if (c != K_LEFT && c != K_UP && c != K_RIGHT && c != K_DOWN &&
      c != K_HOME && c != K_END &&
      c != K_SP && c != K_sCR && c != K_sSP && c != K_cSP)
    return IUP_DEFAULT;

  if (!press || !ih->data->has_focus)
    return IUP_DEFAULT;

  switch(c)
  {
  case K_LEFT:
    if (ih->data->vertical)
    {
      int cells_per_line = ih->data->num_cells / ih->data->num_parts;
      if (ih->data->focus_cell > cells_per_line)
        ih->data->focus_cell -= cells_per_line;
    }
    else
    {
      if (ih->data->focus_cell > 0)
        ih->data->focus_cell--;
    }
    break;
  case K_DOWN:
    if (ih->data->vertical)
    {
      if (ih->data->focus_cell > 0)
        ih->data->focus_cell--;
    }
    else
    {
      int cells_per_line = ih->data->num_cells / ih->data->num_parts;
      if (ih->data->focus_cell > cells_per_line)
        ih->data->focus_cell -= cells_per_line;
    }
    break;
  case K_RIGHT:
    if (ih->data->vertical)
    {
      int cells_per_line = ih->data->num_cells / ih->data->num_parts;
      if (ih->data->focus_cell+cells_per_line < ih->data->num_cells-1)
        ih->data->focus_cell += cells_per_line;
    }
    else
    {
      if (ih->data->focus_cell < ih->data->num_cells-1)
        ih->data->focus_cell++;
    }
    break;
  case K_UP:
    if (ih->data->vertical)
    {
      if (ih->data->focus_cell < ih->data->num_cells-1)
        ih->data->focus_cell++;
    }
    else
    {
      int cells_per_line = ih->data->num_cells / ih->data->num_parts;
      if (ih->data->focus_cell+cells_per_line < ih->data->num_cells-1)
        ih->data->focus_cell += cells_per_line;
    }
    break;
  case K_HOME:
    ih->data->focus_cell = 0;
    break;
  case K_END:
    ih->data->focus_cell = ih->data->num_cells-1;
    break;
  case K_sCR:
    iColorbarCallCellCb(ih, ih->data->focus_cell);
    return IUP_DEFAULT;
  case K_SP:
    iColorbarCallSelectCb(ih, ih->data->focus_cell, IUP_PRIMARY);
    return IUP_DEFAULT;
  case K_cSP:
    iColorbarCallSelectCb(ih, ih->data->focus_cell, IUP_SECONDARY);
    return IUP_DEFAULT;
  case K_sSP:
    iColorbarCallExtentedCb(ih, ih->data->focus_cell);
    return IUP_DEFAULT;
  }

  if (ih->data->cddbuffer)
  {
    cdCanvasFlush(ih->data->cddbuffer);
    if (ih->data->has_focus)
      iColorbarDrawFocusCell(ih);
  }

  return IUP_IGNORE;  /* to avoid arrow keys being processed by the system */
}

static int iColorbarButton_CB(Ihandle* ih, int b, int m, int x, int y, char* r)
{
  int idx;

  if (m == 0)
    return IUP_DEFAULT;

  y = cdIupInvertYAxis(y, ih->data->h);

  if (b == IUP_BUTTON1 && iup_isdouble(r)) 
  { 
    idx = iColorbarGetIndexColor(ih, x, y); 
    if (idx < 0  || idx >= ih->data->num_cells) 
    {
      int ret = iColorbarCheckPreview(ih, x, y);
      if (ret)
      {
        if (ret == 1)
        {
          IFnii switch_cb = (IFnii)IupGetCallback(ih, "SWITCH_CB");

          if (!ih->data->show_secondary)
            return IUP_DEFAULT;

          if (switch_cb && switch_cb(ih, ih->data->fgcolor_idx, ih->data->bgcolor_idx) == IUP_IGNORE) 
            return IUP_DEFAULT;

          /* the application allow to switch the indices */
          idx = ih->data->fgcolor_idx;
          ih->data->fgcolor_idx = ih->data->bgcolor_idx;
          ih->data->bgcolor_idx = idx;

          iColorbarRenderPartsRepaint(ih, 1, ICOLORBAR_RENDER_NONE);   /* only the preview area is rendered */
        }
        else
        {
          if (ret == IUP_PRIMARY)
            idx = ih->data->fgcolor_idx;
          else
            idx = ih->data->bgcolor_idx;

          iColorbarCallCellCb(ih, idx);
        }
      }

      return IUP_DEFAULT;
    }

    ih->data->focus_cell = idx;

    iColorbarCallCellCb(ih, idx);
  }
  else if (b == IUP_BUTTON1)
  { 
    idx = iColorbarGetIndexColor(ih, x, y);
    if (idx < 0  || idx >= ih->data->num_cells)
      return IUP_DEFAULT;

    ih->data->focus_cell = idx;

    iColorbarCallSelectCb(ih, idx, IUP_PRIMARY);
  }
  else if (b == IUP_BUTTON3 && iup_isshift(r)) 
  { 
    idx = iColorbarGetIndexColor(ih, x, y); 
    if (idx < 0  || idx >= ih->data->num_cells)
      return IUP_DEFAULT;

    ih->data->focus_cell = idx;

    iColorbarCallExtentedCb(ih, idx);
  }
  else if (b == IUP_BUTTON3) 
  { 
    idx = iColorbarGetIndexColor(ih, x, y); 
    if (idx < 0  || idx >= ih->data->num_cells)
      return IUP_DEFAULT;

    ih->data->focus_cell = idx;

    iColorbarCallSelectCb(ih, idx, IUP_SECONDARY);
  }

  return IUP_DEFAULT;
}


/****************************************************************************/


static int iColorbarMapMethod(Ihandle* ih)
{
  ih->data->cdcanvas = cdCreateCanvas(CD_IUP, ih);
  if (!ih->data->cdcanvas)
    return IUP_ERROR;

  /* this can fail if canvas size is zero */
  ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);

  return IUP_NOERROR;
}

static void iColorbarUnMapMethod(Ihandle* ih)
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

static int iColorbarCreateMethod(Ihandle* ih, void **params)
{
  int i;
  (void)params;

  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  iupAttribSetStr(ih, "BORDER", "NO");

  /* default values */
  ih->data->num_cells = 16;
  ih->data->num_parts = 1;
  ih->data->vertical  = 1;
  ih->data->squared   = 1;
  ih->data->shadowed  = 1;
  ih->data->focus_cell = 0;
  ih->data->preview_size = -1;  /* automatic */
  ih->data->fgcolor_idx  = 0;   /* black */
  ih->data->bgcolor_idx  = 15;  /* white */
  ih->data->transparency = ICOLORBAR_NO_COLOR;
  ih->data->light_shadow = CD_WHITE;
  ih->data->mid_shadow   = CD_GRAY;
  ih->data->dark_shadow  = CD_DARK_GRAY;

  /* Initialization of the color vector */
  for (i = 0; i < ICOLORBAR_DEFAULT_NUM_CELLS; i++)
  {
    ih->data->colors[i] = cdEncodeColor((unsigned char)default_colors[i].r,
                                        (unsigned char)default_colors[i].g,
                                        (unsigned char)default_colors[i].b);
  }

  /* IupCanvas callbacks */
  IupSetCallback(ih, "RESIZE_CB", (Icallback)iColorbarResize_CB);
  IupSetCallback(ih, "ACTION",    (Icallback)iColorbarRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB", (Icallback)iColorbarButton_CB);
  IupSetCallback(ih, "FOCUS_CB", (Icallback)iColorbarFocus_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iColorbarKeyPress_CB);

  return IUP_NOERROR;
}

Iclass* iupColorbarNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "colorbar";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;
  ic->has_attrib_id = 1;   /* has attributes with IDs that must be parsed */

  /* Class functions */
  ic->New = iupColorbarNewClass;
  ic->Create  = iColorbarCreateMethod;
  ic->Map     = iColorbarMapMethod;
  ic->UnMap   = iColorbarUnMapMethod;

  /* Do not need to set base attributes because they are inherited from IupCanvas */

  /* IupColorbar Callbacks */
  iupClassRegisterCallback(ic, "CELL_CB",     "i=s");
  iupClassRegisterCallback(ic, "SWITCH_CB",   "ii");
  iupClassRegisterCallback(ic, "SELECT_CB",   "ii");
  iupClassRegisterCallback(ic, "EXTENDED_CB", "i");

  /* IupColorbar only */
  iupClassRegisterAttributeId(ic, "CELL", iColorbarGetCellAttrib, iColorbarSetCellAttrib, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", iColorbarGetNumCellsAttrib, NULL, IUPAF_SAMEASSYSTEM, "16", IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUM_CELLS", iColorbarGetNumCellsAttrib, iColorbarSetNumCellsAttrib, IUPAF_SAMEASSYSTEM, "16", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUM_PARTS", iColorbarGetNumPartsAttrib, iColorbarSetNumPartsAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PREVIEW_SIZE", iColorbarGetPreviewSizeAttrib, iColorbarSetPreviewSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PRIMARY_CELL", iColorbarGetPrimaryCellAttrib, iColorbarSetPrimaryCellAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SECONDARY_CELL", iColorbarGetSecondaryCellAttrib, iColorbarSetSecondaryCellAttrib, IUPAF_SAMEASSYSTEM, "15", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BUFFERIZE", iColorbarGetBufferizeAttrib, iColorbarSetBufferizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "ORIENTATION", iColorbarGetOrientationAttrib, iColorbarSetOrientationAttrib, IUPAF_SAMEASSYSTEM, "VERTICAL", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TRANSPARENCY", iColorbarGetTransparencyAttrib, iColorbarSetTransparencyAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SHOW_PREVIEW", NULL, iColorbarSetShowPreviewAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SHOW_SECONDARY", iColorbarGetShowSecondaryAttrib, iColorbarSetShowSecondaryAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SQUARED", iColorbarGetSquaredAttrib, iColorbarSetSquaredAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SHADOWED", iColorbarGetShadowedAttrib, iColorbarSetShadowedAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED);

  /* Overwrite IupCanvas Attributes */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iColorbarSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", iupControlBaseGetBgColorAttrib, iColorbarSetBgColorAttrib, NULL, "255 255 255", IUPAF_NO_INHERIT);    /* overwrite canvas implementation, set a system default to force a new default */

  return ic;
}

Ihandle *IupColorbar(void)
{
  return IupCreate("colorbar");
}
