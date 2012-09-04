/** \file
 * \brief Cells Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef CD_NO_OLD_INTERFACE

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


#define ICELLS_OUT -999

struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  cdCanvas* cdcanvas;        /* cd canvas for drawing                 */
  cdCanvas* cddbuffer;       /* image canvas for double buffering     */
  int w;                     /* control width (pixels)                */
  int h;                     /* control height (pixels)               */
  int clipped;               /* cells bounding box clipping activated */
  int boxed;                 /* draw cells bounding box activated     */
  int bufferize;             /* bufferize on                          */
  int non_scrollable_lins;   /* number of non-scrollable lines        */
  int non_scrollable_cols;   /* number of non-scrollable columns      */
  long int bgcolor;          /* Empty area color and bgcolor          */
};


/* Function to inquire application cell data (line height) */
static int iCellsGetHeight(Ihandle* ih, int i)
{
  int size; 

  IFni cb = (IFni)IupGetCallback(ih, "HEIGHT_CB");
  if (!cb)
    return 30;   /* default value */

  size = cb(ih, i);
  if (size < 0)
    size = 0;

  return size;
}

/* Function to inquire application cell data (column width) */
static int iCellsGetWidth(Ihandle* ih, int j)
{
  int size; 

  IFni cb = (IFni)IupGetCallback(ih, "WIDTH_CB");
  if (!cb)
    return 60;   /* default value */

  size = cb(ih, j);
  if (size < 0)
    size = 0;

  return size;
}

/* Function used to calculate a cell limits */
static int iCellsGetLimits(Ihandle* ih, int i, int j, int* xmin, int* xmax, int* ymin, int* ymax)
{
  int result = 1;
  int xmin_sum = 0;
  int ymin_sum = 0;
  int w = ih->data->w;
  int h = ih->data->h;
  int _xmin, _xmax, _ymin, _ymax;

  /* Adjusting the inital position according to the cell's type. If it
   * is non-scrollable, the origin is always zero, otherwise the origin 
   * is the scrollbar position */
  int posx = (j <= ih->data->non_scrollable_cols)? 0: IupGetInt(ih, "POSX");
  int posy = (i <= ih->data->non_scrollable_lins)? 0: IupGetInt(ih, "POSY");
  int idx;

  /* Adding to the origin, the cells' width and height */
  for (idx = 1; idx < j; idx++)
    xmin_sum += iCellsGetWidth(ih, idx);
  
  for (idx = 1; idx < i; idx++)
    ymin_sum += iCellsGetHeight(ih, idx);

  /* Finding the cell origin */
  _xmin = xmin_sum - posx;
  _ymax = h - (ymin_sum - posy) - 1;

  /* Computing the cell limit, based on its origin and size */
  _xmax = _xmin + iCellsGetWidth(ih, j);
  _ymin = _ymax - iCellsGetHeight(ih, i);

  /* Checking if the cell is visible */
  if (_xmax < 0 || _xmin > w || _ymin > h || _ymax < 0)
    result = 0;

  if (xmin != NULL)
    *xmin = _xmin;

  if (xmax != NULL)
    *xmax = _xmax;

  if (ymin != NULL)
    *ymin = _ymin;

  if (ymax != NULL)
    *ymax = _ymax;

  return result;
}

/* Function to inquire application cell data (vertical span) */
static int iCellsGetVspan(Ihandle* ih, int i, int j)
{
  int result = 1;  /* default value */

  IFnii cb = (IFnii)IupGetCallback(ih, "VSPAN_CB");
  if (cb)
    result = cb(ih, i, j);

  if (result < 0)
    result = 1;

  return result;
}

/* Function to inquire application cell data (horizontal span) */
static int iCellsGetHspan(Ihandle* ih, int i, int j)
{
  int result = 1;  /* default value */

  IFnii cb = (IFnii)IupGetCallback(ih, "HSPAN_CB");
  if (cb)
    result = cb(ih, i, j);
  
  if (result < 0)
    result = 1;

  return result;
}

/* Function to inquire application cell data (#columns) */
static int iCellsGetNCols(Ihandle* ih)
{
  int result = 10;  /* default value */

  Icallback cb = IupGetCallback(ih, "NCOLS_CB");
  if (cb)
    result = cb(ih);

  if (result < 0)
    result = 0;

  return result;
}

/* Function to inquire application cell data (# lines) */
static int iCellsGetNLines(Ihandle* ih)
{
  int result = 10;  /* default value */

  Icallback cb = IupGetCallback(ih, "NLINES_CB");
  if (cb)
    result = cb(ih);

  if (result < 0)
    result = 0;

  return result;
}

/* Recalculation of first visible line */
static int iCellsGetFirstLine(Ihandle* ih)
{
  int i, j;
  int nlines = iCellsGetNLines(ih);
  int ncols  = iCellsGetNCols(ih);

  if (ih->data->non_scrollable_lins >= nlines)
    return 1;

  /* Looping the lines until a visible one is found */
  for (i = 1; i <= nlines; i++) { 
     for (j = 1; j <= ncols; j++) { 
        if (iCellsGetLimits(ih, i, j, NULL, NULL, NULL, NULL))
          return i; 
     }
  }
  return ICELLS_OUT;
}

/* Recalculation of first visible column */
static int iCellsGetFirstCol(Ihandle* ih)
{
  int i, j;
  int ncols  = iCellsGetNCols(ih);
  int nlines = iCellsGetNLines(ih);

  if (ih->data->non_scrollable_cols >= ncols)
    return 1;

  /* Looping the columns until a visible one is found */
  for (j = 1; j <= ncols; j++) { 
     for (i = 1; i <= nlines; i++) { 
        if (iCellsGetLimits(ih, i, j, NULL, NULL, NULL, NULL))
          return j;
     }
  }
  return ICELLS_OUT;
}

/* Function used to get the cells groups virtual size */
static void iCellsGetVirtualSize(Ihandle* ih, int* wi, int* he)
{
  int i, j;
  *wi = 0; *he = 0;

  /* Looping through all lines and columns, adding its width and heights
   * to the return values. So, the cells virtual size is computed */
  for (i = 1; i <= iCellsGetNLines(ih); i++)
    *he = *he + iCellsGetHeight(ih, i);
  for (j = 1; j <= iCellsGetNCols(ih); j++)
    *wi = *wi + iCellsGetWidth(ih, j);
}

/* Function used to calculate a group of columns height */
static int iCellsGetRangedHeight(Ihandle* ih, int from, int to)
{
  int i;
  int result = 0;
  
  /* Looping through a column adding the line height */
  for (i = from; i <= to; i++)
    result += iCellsGetHeight(ih, i);

  return result;
}

/* Function used to calculate a group of lines width */
static int iCellsGetRangedWidth(Ihandle* ih, int from, int to)
{
  int j;
  int result = 0;

  /* Looping through a column adding the column width */
  for (j = from; j <= to; j++)
    result += iCellsGetWidth(ih, j);

  return result;
}

/* Function used to turn a cell visible  */
static void iCellsSetFullVisible(Ihandle* ih, int i, int j)
{
  int xmin, xmax, ymin, ymax;
  int posx = IupGetInt(ih, "POSX");
  int posy = IupGetInt(ih, "POSY");
  int dx = 0;
  int dy = 0;
  
  /* Getting the frontiers positions for the visible cell */
  int min_x = iCellsGetRangedWidth(ih, 1, ih->data->non_scrollable_cols);
  int max_y = ih->data->h - iCellsGetRangedHeight(ih, 1, ih->data->non_scrollable_lins); 

  /* Getting the cell's area limit */
  iCellsGetLimits(ih, i, j, &xmin, &xmax, &ymin, &ymax);

  /* Adjusting the diference of the scrollbars' position (horizontal) */
  if (xmax > ih->data->w)
    dx = xmax - ih->data->w;

  /* Giving priority to xmin position. This can be seen by the usage
   * of dx at the left part of the expression (using the last dx).
   * This is the case wher the cell cannot be fitted. */
  if (xmin - dx < min_x)
    dx = - (min_x - xmin); 

  /* Adjusting the diference of the scrollbars' position (horizontal) */
  if (ymax > max_y)
    dy = - (ymax - max_y);
  if (ymin < 0)
    dy = -ymin;

  /* Adding the diference to scrollbars' position */
  posx += dx;
  posy += dy;

  /* Setting iup scrollbars' attributes */
  IupSetfAttribute(ih, "POSX", "%d", posx); 
  IupSetfAttribute(ih, "POSY", "%d", posy); 
}

/* Function used to make a cell the first visible one */
static void iCellsAdjustOrigin(Ihandle* ih, int lin, int col)
{ 
  int xmin_sum, ymin_sum;

  /* If the origin line is a non-scrollable one, the scrollbar position is
   * set to zero. Otherwise, the sum of the previous widths will be
   * set to the scrollbar position. This algorithm is applied to both
   * scrollbars */
  if (lin <= ih->data->non_scrollable_lins)
  {
     IupSetAttribute(ih, "POSY", "0"); 
  }
  else if (lin <= iCellsGetNLines(ih))
  { 
     ymin_sum = iCellsGetRangedHeight(ih, ih->data->non_scrollable_lins+1, lin-1);
     IupSetfAttribute(ih, "POSY", "%d", ymin_sum); 
  }

  /* As said before... */
  if (col <= ih->data->non_scrollable_cols)
  {
     IupSetAttribute(ih, "POSX", "0"); 
  }
  else if (col <= iCellsGetNCols(ih))
  { 
     xmin_sum = iCellsGetRangedWidth(ih, ih->data->non_scrollable_cols+1, col-1);
     IupSetfAttribute(ih, "POSX", "%d", xmin_sum); 
  }
}

/* Function used for the scrollbar's update; usually needed when the
 * object has modified its size or the cells sizes has changed.   */
static void iCellsAdjustScrolls(Ihandle* ih)
{ 
  int virtual_height, virtual_width;

  /* Getting the virtual size */
  iCellsGetVirtualSize(ih, &virtual_width, &virtual_height); 

  IupSetfAttribute(ih, "YMAX", "%d", virtual_height-1);
  IupSetfAttribute(ih, "XMAX", "%d", virtual_width-1);

  /* Setting the object scrollbar position */
  IupSetfAttribute(ih, "DY",   "%d", ih->data->h);
  IupSetfAttribute(ih, "DX",   "%d", ih->data->w);
}

/* Function used to call the client; is used when a cell must be repainted. */
static void iCellsCallDrawCb(Ihandle* ih, int xmin, int xmax, int ymin, int ymax, int i, int j)
{
  int cxmin, cxmax, cymin, cymax;
  int oldxmin, oldxmax, oldymin, oldymax, oldclip;
  int w = ih->data->w;
  int h = ih->data->h;
  IFniiiiiiC draw_cb;
  cdCanvas* old_cnv = cdActiveCanvas();

  /* Getting clipping area for post restore */
  oldclip = cdCanvasClip(ih->data->cddbuffer, CD_QUERY);
  cdCanvasGetClipArea(ih->data->cddbuffer, &oldxmin, &oldxmax, &oldymin, &oldymax);

  if (ih->data->clipped)  /* Clipping the cell area */
  { 
     cdCanvasClip(ih->data->cddbuffer, CD_CLIPAREA);
     cxmin = xmin < 0 ? 0 : xmin;
     cymin = ymin < 0 ? 0 : ymin;
     cxmax = xmax > w ? w : xmax;
     cymax = ymax > h ? h : ymax;
     cdCanvasClipArea(ih->data->cddbuffer, xmin, xmax, ymin, ymax);
  }

  draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");
  if (draw_cb)
  {
    if (old_cnv != ih->data->cddbuffer) /* backward compatibility code */
      cdActivate(ih->data->cddbuffer);

    draw_cb(ih, i, j, xmin, xmax, ymin, ymax, ih->data->cddbuffer);

    if (old_cnv && old_cnv != ih->data->cddbuffer)
    {
      cdActivate(old_cnv);
      cdCanvasActivate(ih->data->cddbuffer);
    }
  }

  cdCanvasClip(ih->data->cddbuffer, oldclip);
  cdCanvasClipArea(ih->data->cddbuffer, oldxmin, oldxmax, oldymin, oldymax);
}

/* Render function for one cell in a given coordinate.  */
static void iCellsRenderCellIn(Ihandle* ih, int i, int j, int xmin, int xmax, int ymin, int ymax)
{
  int k;
  int w = ih->data->w;
  int h = ih->data->h;
  int hspan = 1;
  int vspan = 1;

  /* Checking if the cells is out of range. (no span will affect it!) */
  if (xmin > w || ymax < 0)
    return;
 
  /* Calculating cell spans */
  hspan = iCellsGetHspan(ih, i, j);
  vspan = iCellsGetVspan(ih, i, j);

  /* if any span is set to zero, then another cell invaded its space and
   * the cell does not need to draw itself */
  if (hspan == 0 || vspan == 0)
    return;

  /* Increasing cell's width and height according to its spans */
  for(k = 1; k < hspan; k++)
    xmax += iCellsGetWidth(ih, j+k);
  for(k = 1; k < vspan; k++)
    ymin -= iCellsGetHeight(ih, i+k);

  /* Checking if the cell expanded enough to appear inside the canvas */
  if (xmax < 0 || ymin > h)
    return;

  /* Calling application's draw callback */
  iCellsCallDrawCb(ih, xmin, xmax, ymin, ymax, i, j);

  /* Drawing a box in cell's area */
  if (ih->data->boxed)
  { 
     cdCanvasForeground(ih->data->cddbuffer, CD_BLACK);
     cdCanvasRect(ih->data->cddbuffer, xmin, xmax, ymin, ymax);
  }
}

/* Repaint function for all cells in a given range */
static void iCellsRenderRangedCells(Ihandle* ih, int linfrom, int linto, int colfrom, int colto)
{
  int i, j;
  int xmin, xmax, ymin, ymax;
  int refxmin, refxmax;

  /* Getting first cell limits: this function is slow to be called everytime */
  iCellsGetLimits(ih, linfrom, colfrom, &xmin, &xmax, &ymin, &ymax);

  /* Initializing current reference position */
  refxmin = xmin; 
  refxmax = xmax;

  /* Looping through the cells adding next cell width and height */
  for (i = linfrom; i <= linto; i++)
  { 
    xmin = refxmin; 
    xmax = refxmax;
    for (j = colfrom; j <= colto; j++)
    { 
      iCellsRenderCellIn(ih, i, j, xmin, xmax, ymin, ymax);
      xmin = xmax;
      xmax = xmin + (j == colto ? 0 : iCellsGetWidth(ih, j+1));
    }
    ymax = ymin;
    ymin = ymax - (i == linto ? 0 : iCellsGetHeight(ih, i+1));
  }
}

/* Repaint function for all cells */
static void iCellsRenderCells(Ihandle* ih)
{
  int sline  = ih->data->non_scrollable_lins;
  int scol   = ih->data->non_scrollable_cols;
  int nlines = iCellsGetNLines(ih);
  int ncols  = iCellsGetNCols(ih);

  cdCanvasBackground(ih->data->cddbuffer, ih->data->bgcolor); 
  cdCanvasClear(ih->data->cddbuffer);

  /* Repainting the four parts of the cells: common cells, non-scrollable
   * columns, non-scrollable lines, and non-scrollable margin 
   * (line and column) */
  iCellsRenderRangedCells(ih, sline+1, nlines, scol+1, ncols);
  iCellsRenderRangedCells(ih, sline+1, nlines, 1, scol);
  iCellsRenderRangedCells(ih, 1, sline, scol+1, ncols);
  iCellsRenderRangedCells(ih, 1, sline, 1, scol);
}

static void iCellsRepaint(Ihandle* ih)
{
  if (ih->data->cddbuffer == NULL)
    return;

  /* If object is buffering, it will not be drawn */
  if (ih->data->bufferize == 1)
    return;

  /* update render */
  iCellsRenderCells(ih);

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
}

static int iCellsRedraw_CB(Ihandle* ih)
{
  if (ih->data->cddbuffer == NULL)
    return IUP_DEFAULT;

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
  return IUP_DEFAULT;
}

/* Function used to calculate the cell coordinates limited by a hint */
static int iCellsGetRangedCoord(Ihandle* ih, int x, int y, int* lin, int* col, int linfrom, int linto, int colfrom, int colto)
{
  int i, j, k;
  int hspan, vspan;
  int rxmax, rymin;
  int xmin, xmax, ymin, ymax;
  int refxmin, refxmax;
  int w = ih->data->w;
 
  /* Getting the first cell's  limit -- based on the range */
  iCellsGetLimits(ih, linfrom, colfrom, &xmin, &xmax, &ymin, &ymax);

  /* Initializing reference position */
  refxmin = xmin; 
  refxmax = xmax;
  
  /* Looping through the cells adding next cell width and height */
  for (i = linfrom; i <= linto; i++)
  { 
    xmin = refxmin; 
    xmax = refxmax;
    if (!(ymax < 0))
    { 
      for (j = colfrom; j <= colto; j++)
      { 
        hspan = 1;
        vspan = 1;
        if (!(xmin > w))
        { 
          hspan = iCellsGetHspan(ih, i, j);
          vspan = iCellsGetVspan(ih, i, j);
          if (hspan != 0 && vspan != 0)
          { 
            rxmax = xmax; 
            rymin = ymin;
            for(k = 1; k < hspan; k++)
              rxmax += iCellsGetWidth(ih, j+k);
            for(k = 1; k < vspan; k++)
              rymin -= iCellsGetHeight(ih, i+k);

            /* A cell was found */
            if (x >= xmin && x <= rxmax && y >= rymin && y <= ymax)
            { 
              *lin = i; 
              *col = j; 
              return 1; 
            }
          }
          xmin = xmax;
          xmax = xmin + (j == colto ? 0 : iCellsGetWidth(ih, j+1));
        }
      }
    }
    ymax = ymin;
    ymin = ymax - (i == linto ? 0 : iCellsGetHeight(ih, i+1));
  }

  /* No cell selected... */
  *lin = ICELLS_OUT;
  *col = ICELLS_OUT;
  return 0;
}

/* Function used to calculate the cell coordinates */
static int iCellsGetCoord(Ihandle* ih, int x, int y, int* lin, int* col)
{
  int pck = 0;
  int sline  = ih->data->non_scrollable_lins;
  int scol   = ih->data->non_scrollable_cols;
  int nlines = iCellsGetNLines(ih);
  int ncols  = iCellsGetNCols(ih);

  /* Trying to pick a cell (raster coordinates) at the four 
   * parts of the cells (reverse order of the repainting):
   * non-scrollable margin (line and column), non-scrollable 
   * columns, non-scrollable lines, and common cells. */
  pck = iCellsGetRangedCoord(ih, x, y, lin, col, 1, sline, 1, scol);
  if (pck)
    return 1;

  pck = iCellsGetRangedCoord(ih, x, y, lin, col, 1, sline, scol+1, ncols);
  if (pck)
    return 1;

  pck = iCellsGetRangedCoord(ih, x, y, lin, col, sline+1, nlines, 1, scol);
  if (pck)
    return 1;

  pck = iCellsGetRangedCoord(ih, x, y, lin, col, 1, nlines, 1, ncols);
  return pck;
}

static int iCellsScroll_CB(Ihandle* ih)
{
  IFnii cb;
  int ret = IUP_DEFAULT;

  /* Checking the existence of a scroll bar callback. If the application
   * has set one, it will be called now. If the application returns
   * IUP_DEFAULT, the cells will be repainted. */
  cb = (IFnii)IupGetCallback(ih, "SCROLLING_CB");
  if (cb)
  {
    int fline = iCellsGetFirstLine(ih);
    int fcol  = iCellsGetFirstCol(ih);
    ret = cb(ih, fline, fcol);
  }
  
  if (ret == IUP_DEFAULT)
    iCellsRepaint(ih);

  return IUP_DEFAULT;
}

static int iCellsMotion_CB(Ihandle* ih, int x, int y, char* r)
{
  int i, j;

  /* Checking the existence of a motion bar callback. If the application
   * has set one, it will be called now. However, before calling the
   * callback, we need to find out which cell is under the mouse
   * position. */
  IFniiiis cb = (IFniiiis)IupGetCallback(ih, "MOUSEMOTION_CB");
  if (!cb)
    return IUP_DEFAULT;

  iCellsGetCoord(ih, x, y, &i, &j); 
  return cb(ih, i, j, x, y, r);
}

static int iCellsResize_CB(Ihandle* ih)
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

  /* update canvas size */
  cdCanvasActivate(ih->data->cddbuffer);
  cdCanvasGetSize(ih->data->cddbuffer, &ih->data->w, &ih->data->h, NULL, NULL);

  /* recalculate scrollbars positions and size */
  iCellsAdjustScrolls(ih);  

  /* update render */
  iCellsRenderCells(ih);

  return IUP_DEFAULT;
}

static int iCellsButton_CB(Ihandle* ih, int b, int m, int x, int y, char* r)
{
  int i, j;
  IFniiiiiis cb;

  y = cdIupInvertYAxis(y, ih->data->h);

  /* Treating the button event. The application will receive
   * a button press callback. */
  cb = (IFniiiiiis)IupGetCallback(ih, "MOUSECLICK_CB");
  if (!cb)
    return IUP_DEFAULT;

  iCellsGetCoord(ih, x, y, &i, &j);
  return cb(ih, b, m, i, j, x, y, r);
}

static char* iCellsGetImageCanvasAttrib(Ihandle* ih)
{
  return (char*)ih->data->cddbuffer;
}

static char* iCellsGetCanvasAttrib(Ihandle* ih)
{
  return (char*)ih->data->cdcanvas;
}

static char* iCellsGetFirstLineAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(80);
  sprintf( buffer, "%d", iCellsGetFirstLine(ih) );
  return buffer;
}

static char* iCellsGetFirstColAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(80);
  sprintf(buffer, "%d", iCellsGetFirstCol(ih) );
  return buffer;
}

static char* iCellsGetLimitsAttrib(Ihandle* ih, int i, int j)
{
  char* buffer = iupStrGetMemory(80);
  int xmin, xmax, ymin, ymax;
  iCellsGetLimits(ih, i, j, &xmin, &xmax, &ymin, &ymax);
  sprintf(buffer, "%d:%d:%d:%d", xmin, xmax, ymin, ymax);
  return buffer;
}

static int iCellsSetBufferizeAttrib(Ihandle* ih, const char* value)
{
  if (value == NULL || iupStrEqualNoCase(value, "NO"))
  { 
    ih->data->bufferize = 0;
    iCellsAdjustScrolls(ih);
    iCellsRepaint(ih);
  }
  else
    ih->data->bufferize = 1;

  return 0;
}

static char* iCellsGetBufferizeAttrib(Ihandle* ih)
{
  if (ih->data->bufferize)
    return "YES";
  else
    return "NO";
}

static int iCellsSetRepaintAttrib(Ihandle* ih, const char* value)
{
  (void)value;  /* not used */
  ih->data->bufferize = 0;
  iCellsAdjustScrolls(ih);
  iCellsRepaint(ih);
  return 0;  /* do not store value in hash table */
}

static int iCellsSetBgColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = iupControlBaseGetParentBgColor(ih);

  ih->data->bgcolor = cdIupConvertColor(value);
  
  iCellsRepaint(ih); 
  return 1;
}

static int iCellsSetOriginAttrib(Ihandle* ih, const char* value)
{
  int lin = -9;
  int col = -9;
  iupStrToIntInt(value, &lin, &col, ':');
  iCellsAdjustOrigin(ih, lin, col);
  iCellsRepaint(ih);
  return 1;
}

static int iCellsSetNonScrollableColsAttrib(Ihandle* ih, const char* value)
{
  if (iupStrToInt(value, &ih->data->non_scrollable_cols))
    iCellsRepaint(ih);
  return 0;
}

static char* iCellsGetNonScrollableColsAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(80);
  sprintf(buffer, "%d", ih->data->non_scrollable_cols);
  return buffer;
}

static int iCellsSetNonScrollableLinesAttrib(Ihandle* ih, const char* value)
{
  if (iupStrToInt(value, &ih->data->non_scrollable_lins))
    iCellsRepaint(ih);
  return 0;
}

static char* iCellsGetNonScrollableLinesAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(80);
  sprintf(buffer, "%d", ih->data->non_scrollable_lins);
  return buffer;
}

static int iCellsSetBoxedAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NO"))
    ih->data->boxed = 0;
  else
    ih->data->boxed = 1;

  iCellsRepaint(ih);
  return 0;
}

static char* iCellsGetBoxedAttrib(Ihandle* ih)
{
  if (ih->data->boxed) 
    return "YES";
  else 
    return "NO";
}

static int iCellsSetClippedAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NO"))
    ih->data->clipped = 0;
  else
    ih->data->clipped = 1;

  iCellsRepaint(ih);
  return 0;
}

static char* iCellsGetClippedAttrib(Ihandle* ih)
{
  if (ih->data->clipped) 
    return "YES";
  else 
    return "NO";
}

static int iCellsSetFullVisibleAttrib(Ihandle* ih, const char* value)
{
  int i, j;
  if (iupStrToIntInt(value, &i, &j, ':') != 2)
    return 0;

  iCellsSetFullVisible(ih, i, j);
  iCellsRepaint(ih);
  return 0;  /* do not store value in hash table */
}


/****************************************************************************/


static int iCellsMapMethod(Ihandle* ih)
{
  ih->data->cdcanvas = cdCreateCanvas(CD_IUP, ih);
  if (!ih->data->cdcanvas)
    return IUP_ERROR;

  /* this can fail if canvas size is zero */
  ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);

  return IUP_NOERROR;
}

static void iCellsUnMapMethod(Ihandle* ih)
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

static int iCellsCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  iupAttribSetStr(ih, "SCROLLBAR", "YES");
  iupAttribSetStr(ih, "BORDER", "NO");

  /* default values */
  ih->data->boxed = 1;
  ih->data->clipped = 1;

  /* IupCanvas callbacks */
  IupSetCallback(ih, "RESIZE_CB", (Icallback)iCellsResize_CB);
  IupSetCallback(ih, "ACTION",    (Icallback)iCellsRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB", (Icallback)iCellsButton_CB);
  IupSetCallback(ih, "MOTION_CB", (Icallback)iCellsMotion_CB);
  IupSetCallback(ih, "SCROLL_CB", (Icallback)iCellsScroll_CB);

  return IUP_NOERROR;
}

Iclass* iupCellsNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "cells";
  ic->format = NULL; /* no parameters */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;
  ic->has_attrib_id = 2;  /* has attributes with two IDs that must be parsed */

  /* Class functions */
  ic->New = iupCellsNewClass;
  ic->Create  = iCellsCreateMethod;
  ic->Map     = iCellsMapMethod;
  ic->UnMap   = iCellsUnMapMethod;

  /* Do not need to set base attributes because they are inherited from IupCanvas */

  /* IupCells Callbacks */
  iupClassRegisterCallback(ic, "SCROLLING_CB", "ii");
  iupClassRegisterCallback(ic, "MOUSEMOTION_CB", "iiiis");
  iupClassRegisterCallback(ic, "MOUSECLICK_CB", "iiiiiis");
  iupClassRegisterCallback(ic, "DRAW_CB", "iiiiiiv");
  iupClassRegisterCallback(ic, "VSPAN_CB", "ii");
  iupClassRegisterCallback(ic, "HSPAN_CB", "ii");
  iupClassRegisterCallback(ic, "NCOLS_CB", "");
  iupClassRegisterCallback(ic, "NLINES_CB", "");
  iupClassRegisterCallback(ic, "HEIGHT_CB", "i");
  iupClassRegisterCallback(ic, "WIDTH_CB", "i");

  /* IupCells only */
  iupClassRegisterAttribute(ic, "BOXED", iCellsGetBoxedAttrib, iCellsSetBoxedAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CLIPPED", iCellsGetClippedAttrib, iCellsSetClippedAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "NON_SCROLLABLE_LINES", iCellsGetNonScrollableLinesAttrib, iCellsSetNonScrollableLinesAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "NON_SCROLLABLE_COLS", iCellsGetNonScrollableColsAttrib, iCellsSetNonScrollableColsAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "NO_COLOR", iupControlBaseGetBgColorAttrib, iCellsSetBgColorAttrib, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BUFFERIZE", iCellsGetBufferizeAttrib, iCellsSetBufferizeAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "ORIGIN", NULL, iCellsSetOriginAttrib, NULL, NULL, IUPAF_DEFAULT);

  iupClassRegisterAttribute(ic, "FULL_VISIBLE", NULL, iCellsSetFullVisibleAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REPAINT", NULL, iCellsSetRepaintAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId2(ic, "LIMITS", iCellsGetLimitsAttrib, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FIRST_COL", iCellsGetFirstColAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FIRST_LINE", iCellsGetFirstLineAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE_CANVAS", iCellsGetImageCanvasAttrib, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CANVAS", iCellsGetCanvasAttrib, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_READONLY|IUPAF_NO_INHERIT);

  /* Overwrite IupCanvas Attributes */
  iupClassRegisterAttribute(ic, "BGCOLOR", iupControlBaseGetBgColorAttrib, iCellsSetBgColorAttrib, NULL, "255 255 255", IUPAF_NO_INHERIT);    /* overwrite canvas implementation, set a system default to force a new default */

  return ic;
}

Ihandle* IupCells(void)
{
  return IupCreate("cells");
}
