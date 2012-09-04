/** \file
 * \brief iupmatrix column resize
 *
 * See Copyright Notice in "iup.h"
 */

/**************************************************************************/
/* Interactive Column Resize Functions and WIDTH/HEIGHT change            */
/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include <cd.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_stdcontrols.h"

#include "iupmat_def.h"
#include "iupmat_colres.h"
#include "iupmat_draw.h"


#define IMAT_COLRES_TOL       3
#define IMAT_RESIZE_COLOR  0x666666L

static int iMatrixGetColResCheck(Ihandle* ih, int x, int y)
{
  if (ih->data->lines.sizes[0] && 
      y < ih->data->lines.sizes[0] && 
      iupAttribGetBoolean(ih, "RESIZEMATRIX"))
  {
    int x_col = 0, col;
   
    /* Check if it is in the non scrollable columns */
    for(col = 0; col < ih->data->columns.num_noscroll; col++)
    {
      x_col += ih->data->columns.sizes[col];
      if (abs(x_col-x) < IMAT_COLRES_TOL)
        return col;
    }

    /* Check if it is in the visible columns */
    for(col = ih->data->columns.first; col <= ih->data->columns.last; col++)
    {
      x_col += ih->data->columns.sizes[col];
      if (col == ih->data->columns.first)
        x_col -= ih->data->columns.first_offset;

      if (abs(x_col-x) < IMAT_COLRES_TOL)
        return col;
    }
  }

  return -1;
}

/* Verify if the mouse is in the intersection between two of column titles,
   if so the resize is started */
int iupMatrixColResStart(Ihandle* ih, int x, int y)
{
  int col = iMatrixGetColResCheck(ih, x, y);
  if (col != -1)
  {
    ih->data->colres_drag_col_start_x = x;
    ih->data->colres_dragging =  1;
    ih->data->colres_drag_col_last_x = -1;
    ih->data->colres_drag_col = col;
    return 1;
  }
  else
    return 0;
}

void iupMatrixColResFinish(Ihandle* ih, int x)
{
  char str[100];
  int delta = x - ih->data->colres_drag_col_start_x;
  int width = ih->data->columns.sizes[ih->data->colres_drag_col] + delta;
  if (width < 0)
    width = 0;

  /* delete feedback */
  if (ih->data->colres_drag_col_last_x != -1)
  {
    int y1 = ih->data->lines.sizes[0];  /* from the bottom of the line of titles */
    int y2 = ih->data->h-1;             /* to the bottom of the matrix */

    cdCanvasWriteMode(ih->data->cdcanvas, CD_XOR);
    cdCanvasForeground(ih->data->cdcanvas, IMAT_RESIZE_COLOR);               
    cdCanvasLine(ih->data->cdcanvas, ih->data->colres_drag_col_last_x, iupMATRIX_INVERTYAXIS(ih, y1), 
                                     ih->data->colres_drag_col_last_x, iupMATRIX_INVERTYAXIS(ih, y2));
    cdCanvasWriteMode(ih->data->cdcanvas, CD_REPLACE);
  }

  ih->data->colres_dragging = 0;

  sprintf(str, "RASTERWIDTH%d", ih->data->colres_drag_col);
  iupAttribSetInt(ih, str, width-IMAT_PADDING_W-IMAT_FRAME_W);
  sprintf(str, "WIDTH%d", ih->data->colres_drag_col);
  iupAttribSetStr(ih, str, NULL);

  ih->data->need_calcsize = 1;
  iupMatrixDraw(ih, 0);
}

/* Change the column width interactively, just change the line in the screen.
   When the user finishes the drag, the iupMatrixColResFinish function is called
   to truly change the column width. */
void iupMatrixColResMove(Ihandle* ih, int x)
{
  int y1, y2;

  int delta = x - ih->data->colres_drag_col_start_x;
  int width = ih->data->columns.sizes[ih->data->colres_drag_col] + delta;
  if (width < 0)
    return;

  y1 = ih->data->lines.sizes[0];  /* from the bottom of the line of titles */
  y2 = ih->data->h-1;             /* to the bottom of the matrix */

  cdCanvasWriteMode(ih->data->cdcanvas, CD_XOR);
  cdCanvasForeground(ih->data->cdcanvas, IMAT_RESIZE_COLOR);

  /* If it is not the first time, move old line */
  if (ih->data->colres_drag_col_last_x != -1)
  {
    cdCanvasLine(ih->data->cdcanvas, ih->data->colres_drag_col_last_x, iupMATRIX_INVERTYAXIS(ih, y1), 
                                     ih->data->colres_drag_col_last_x, iupMATRIX_INVERTYAXIS(ih, y2));
  }

  cdCanvasLine(ih->data->cdcanvas, x, iupMATRIX_INVERTYAXIS(ih, y1), 
                                   x, iupMATRIX_INVERTYAXIS(ih, y2));

  ih->data->colres_drag_col_last_x = x;
  cdCanvasWriteMode(ih->data->cdcanvas, CD_REPLACE);
}


static void iMatrixColResResetMatrixCursor(Ihandle* ih)
{
  char *cursor = iupAttribGet(ih, "_IUPMAT_CURSOR");
  if (cursor)
  {
    IupStoreAttribute(ih, "CURSOR", cursor);
    iupAttribSetStr(ih, "_IUPMAT_CURSOR", NULL);
  }
}

/* Change the cursor when it passes over a group of the column titles. */
void iupMatrixColResCheckChangeCursor(Ihandle* ih, int x, int y)
{
  int col = iMatrixGetColResCheck(ih, x, y);
  if (col != -1)
  {
    if (!iupAttribGet(ih, "_IUPMAT_CURSOR"))
      iupAttribStoreStr(ih, "_IUPMAT_CURSOR", IupGetAttribute(ih, "CURSOR"));
    IupSetAttribute(ih, "CURSOR", "RESIZE_W");
  }
  else /* It is in the empty area after the last column, or inside a cell */
    iMatrixColResResetMatrixCursor(ih);
}

int iupMatrixColResIsResizing(Ihandle* ih)
{
  return ih->data->colres_dragging;
}

