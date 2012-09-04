/** \file
 * \brief iupmatrix control
 * draw functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef CD_NO_OLD_INTERFACE

#include "iup.h"
#include "iupcbs.h"

#include <cd.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_controls.h"
#include "iup_cdutil.h"
#include "iup_image.h"

#include "iupmat_def.h"
#include "iupmat_cd.h"
#include "iupmat_draw.h"
#include "iupmat_aux.h"
#include "iupmat_getset.h"
#include "iupmat_mark.h"


/* Text alignment that will be draw. Used by iMatrixDrawCellValue */
#define IMAT_T_CENTER  1
#define IMAT_T_LEFT    2
#define IMAT_T_RIGHT   3

#define IMAT_CD_INACTIVE_FGCOLOR  0x666666L

#define IMAT_DROPBOX_W 16



/**************************************************************************/
/*  Private functions                                                     */
/**************************************************************************/

static void iMatrixDrawSetCellClipping(Ihandle* ih, int x1, int x2, int y1, int y2)
{
  int old_clip = cdCanvasClip(ih->data->cddbuffer, CD_QUERY);
  if (old_clip == CD_CLIPAREA)
  {
    cdCanvasGetClipArea(ih->data->cddbuffer, &(ih->data->clip_x1), &(ih->data->clip_x2), &(ih->data->clip_y1), &(ih->data->clip_y2));
    y1 = iupMATRIX_INVERTYAXIS(ih, y1);
    y2 = iupMATRIX_INVERTYAXIS(ih, y2);
    if (x1 > x2) {int tmp = x1; x1 = x2; x2 = tmp;}
    if (y1 > y2) {int tmp = y1; y1 = y2; y2 = tmp;}
    if (x1 < ih->data->clip_x1) x1 = ih->data->clip_x1;
    if (x2 > ih->data->clip_x2) x2 = ih->data->clip_x2;
    if (y1 < ih->data->clip_y1) y1 = ih->data->clip_y1;
    if (y2 > ih->data->clip_y2) y2 = ih->data->clip_y2;
    cdCanvasClipArea(ih->data->cddbuffer, x1, x2, y1, y2);
    cdCanvasClip(ih->data->cddbuffer, CD_CLIPAREA);
  }
}

static void iMatrixDrawResetCellClipping(Ihandle* ih)
{
  int old_clip = cdCanvasClip(ih->data->cddbuffer, CD_QUERY);
  if (old_clip == CD_CLIPAREA)
    cdCanvasClipArea(ih->data->cddbuffer, ih->data->clip_x1, ih->data->clip_x2, ih->data->clip_y1, ih->data->clip_y2);
}

static int iMatrixDrawGetColAlignment(Ihandle* ih, int col, char* str)
{
  char* align;
  sprintf(str, "ALIGNMENT%d", col);
  align = iupAttribGet(ih, str);
  if (!align)
    align = iupAttribGet(ih, "ALIGNMENT");
  if (!align)
  {
    if (col == 0)
      return IMAT_T_LEFT;
    else
      return IMAT_T_CENTER;
  }
  else if (iupStrEqualNoCase(align, "ARIGHT"))
    return IMAT_T_RIGHT;
  else if(iupStrEqualNoCase(align, "ACENTER"))
    return IMAT_T_CENTER;
  else
    return IMAT_T_LEFT;
}

static int iMatrixDrawCallDrawCB(Ihandle* ih, int lin, int col, int x1, int x2, int y1, int y2, IFniiiiiiC draw_cb)
{
  int ret;
  cdCanvas* old_cnv;

  iMatrixDrawSetCellClipping(ih, x1, x2, y1, y2);

  old_cnv = cdActiveCanvas();
  if (old_cnv != ih->data->cddbuffer) /* backward compatibility code */
    cdActivate(ih->data->cddbuffer);

  ret = draw_cb(ih, lin, col, x1, x2, iupMATRIX_INVERTYAXIS(ih, y1), iupMATRIX_INVERTYAXIS(ih, y2), ih->data->cddbuffer);

  iMatrixDrawResetCellClipping(ih);

  if (old_cnv && old_cnv != ih->data->cddbuffer) /* backward compatibility code */
  {
    cdActivate(old_cnv);
    cdCanvasActivate(ih->data->cddbuffer);
  }

  if (ret == IUP_DEFAULT)
    return 0;

  return 1;
}

/* Change the CD foreground color, for the selected color to draw a cell with
   its FOREGROUND COLOR. This involves checking if there is a color attribute
   that cell.  If no, uses a color attribute for the line, else if no for the
   column,  else if no  for the entire matrix.   Finally,  if not find any of
   these, use the default color.
   -> lin, col - cell coordinates, in IUP format - i.e., l,l represents the left
                 top cell of the matrix; lin and col values = 0 represents the
                 title lines and columns.
   -> mark - indicate if a cell is marked. If yes, its color is attenuated.
*/
static unsigned long iMatrixDrawSetFgColor(Ihandle* ih, int lin, int col, int mark)
{
  unsigned char r = 0, g = 0, b = 0;
  iupMatrixGetFgRGB(ih, lin, col, &r, &g, &b);

  if (mark)
  {
    r = IMAT_ATENUATION(r);
    g = IMAT_ATENUATION(g);
    b = IMAT_ATENUATION(b);
  }
  
  return cdCanvasForeground(ih->data->cddbuffer, cdEncodeColor(r, g, b));
}

/* Change the CD foreground color, for the selected color to draw a cell with
   its BACKGROUND COLOR. This involves checking if there is a color attribute
   that cell.  If no, uses a color attribute for the line, else if no for the
   column,  else if no  for the entire matrix.   Finally,  if not find any of
   these, use the default color.
   -> lin, col - cell coordinates, in IUP format - i.e., l,l represents the left
                 top cell of the matrix; lin and col values = 0 represents the
                 title lines and columns.
   -> mark - indicate if a cell is marked. If yes, its color is attenuated.
*/
static unsigned long iMatrixDrawSetBgColor(Ihandle* ih, int lin, int col, int mark, int active)
{
  unsigned char r = 255, g = 255, b = 255;

  iupMatrixGetBgRGB(ih, lin, col, &r, &g, &b);
  
  if (mark)
  {
    r = IMAT_ATENUATION(r);
    g = IMAT_ATENUATION(g);
    b = IMAT_ATENUATION(b);
  }

  if (!active)
  {
    r = cdIupLIGTHER(r);
    g = cdIupLIGTHER(g);
    b = cdIupLIGTHER(b);
  }

  return cdCanvasForeground(ih->data->cddbuffer, cdEncodeColor(r, g, b));
}

static void iMatrixDrawFrameHorizLineCell(Ihandle* ih, int lin, int col, int x1, int x2, int y, long framecolor, char* str)
{
  if (ih->data->checkframecolor && (ih->data->callback_mode || 
                                    ih->data->cells[lin][col].flags & IMAT_HAS_FRAMEHORIZCOLOR ||
                                    ih->data->lines.flags[lin] & IMAT_HAS_FRAMEHORIZCOLOR))
  {
    char* color;
    unsigned char r,g,b;
    sprintf(str, "FRAMEHORIZCOLOR%d:%d", lin, col);
    color = iupAttribGet(ih, str);
    if (!color)
    {
      sprintf(str, "FRAMEHORIZCOLOR%d:*", lin);
      color = iupAttribGet(ih, str);
    }
    if (iupStrEqual(color, "BGCOLOR"))
      return;
    if (iupStrToRGB(color, &r, &g, &b))
      framecolor = cdEncodeColor(r, g, b);
  }

  cdCanvasForeground(ih->data->cddbuffer, framecolor);
  iupMATRIX_LINE(ih, x1, y, x2, y);   /* bottom horizontal line */
}

static void iMatrixDrawFrameVertLineCell(Ihandle* ih, int lin, int col, int x, int y1, int y2, long framecolor, char* str)
{
  if (ih->data->checkframecolor && (ih->data->callback_mode || 
                                    ih->data->cells[lin][col].flags & IMAT_HAS_FRAMEVERTCOLOR ||
                                    ih->data->columns.flags[col] & IMAT_HAS_FRAMEVERTCOLOR))
  {
    char* color;
    unsigned char r,g,b;
    sprintf(str, "FRAMEVERTCOLOR%d:%d", lin, col);
    color = iupAttribGet(ih, str);
    if (!color)
    {
      sprintf(str, "FRAMEVERTCOLOR*:%d", col);
      color = iupAttribGet(ih, str);
    }
    if (iupStrEqual(color, "BGCOLOR"))
      return;
    if (iupStrToRGB(color, &r, &g, &b))
      framecolor = cdEncodeColor(r, g, b);
  }

  cdCanvasForeground(ih->data->cddbuffer, framecolor);
  iupMATRIX_LINE(ih, x, y1, x, y2);    /* right vertical line */
}

static void iMatrixDrawFrameRectTitle(Ihandle* ih, int lin, int col, int x1, int x2, int y1, int y2, long framecolor, char* str)
{
  /* avoid drawing over the frame of the next cell */
  x2 -= IMAT_FRAME_W/2;
  y2 -= IMAT_FRAME_H/2;

  /* right line */
  iMatrixDrawFrameVertLineCell(ih, lin, col, x2, y1, y2, framecolor, str);  
  if (col==0)
  {
    /* left line */
    cdCanvasForeground(ih->data->cddbuffer, framecolor);
    iupMATRIX_LINE(ih, x1, y1, x1, y2);    
    x1++;
  }
  else if (col==1 && ih->data->columns.sizes[0] == 0)
  {
    /* If does not have line titles then draw the >> left line << of the cell frame */
    iMatrixDrawFrameVertLineCell(ih, lin, col-1, x1, y1, y2-1, framecolor, str);
    x1++;
  }

  /* Titles have a white line near the frame, at left */
  cdCanvasForeground(ih->data->cddbuffer, CD_WHITE);  
  iupMATRIX_LINE(ih, x1, y1+1, x1, y2-1);

  /* bottom line */
  iMatrixDrawFrameHorizLineCell(ih, lin, col, x1, x2, y2, framecolor, str);  
  if (lin==0)
  {
    /* top line */
    cdCanvasForeground(ih->data->cddbuffer, framecolor);
    iupMATRIX_LINE(ih, x1, y1, x2, y1);    
    y1++;
  }
  else if (lin==1 && ih->data->lines.sizes[0] == 0)
  {
    /* If does not have column titles then draw the >> top line << of the cell frame */
    iMatrixDrawFrameHorizLineCell(ih, lin-1, col, x1, x2-1, y1, framecolor, str);
    y1++;
  }

  /* Titles have a white line near the frame, at top */
  cdCanvasForeground(ih->data->cddbuffer, CD_WHITE);  
  iupMATRIX_LINE(ih, x1, y1, x2-1, y1);
}

static void iMatrixDrawFrameRectCell(Ihandle* ih, int lin, int col, int x1, int x2, int y1, int y2, long framecolor, char* str)
{
  if (col==1 && ih->data->columns.sizes[0] == 0)
  {
    /* If does not have line titles then draw the >> left line << of the cell frame */
    iMatrixDrawFrameVertLineCell(ih, lin, col-1, x1, y1, y2-1, framecolor, str);
  }

  if (lin==1 && ih->data->lines.sizes[0] == 0)
  {
    /* If does not have column titles then draw the >> top line << of the cell frame */
    iMatrixDrawFrameHorizLineCell(ih, lin-1, col, x1, x2-1, y1, framecolor, str);
  }

  /* bottom line */
  iMatrixDrawFrameHorizLineCell(ih, lin, col, x1, x2-1, y2-1, framecolor, str);
  
  /* right line */
  iMatrixDrawFrameVertLineCell(ih, lin, col, x2-1, y1, y2-2, framecolor, str);
}

static int iMatrixDrawSortSign(Ihandle* ih, int x2, int y1, int y2, int col, int active, char* str)
{
  int yc;
  char* sort;

  sprintf(str, "SORTSIGN%d", col);
  sort = iupAttribGet(ih, str);
  if (!sort || iupStrEqualNoCase(sort, "NO"))
    return 0;

  /* Remove the space between text and cell frame */
  x2 -= IMAT_PADDING_W/2 + IMAT_FRAME_W/2;

  /* Set the color used to draw the text */
  if (active)
    cdCanvasForeground(ih->data->cddbuffer, IMAT_CD_INACTIVE_FGCOLOR);
  else
    iMatrixDrawSetFgColor(ih, 0, col, 0);

  yc = (int)( (y1 + y2 ) / 2.0 - .5);

  cdCanvasBegin(ih->data->cddbuffer, CD_FILL);

  if (iupStrEqualNoCase(sort, "DOWN"))
  {
    iupMATRIX_VERTEX(ih, x2 - 5, yc + 2);
    iupMATRIX_VERTEX(ih, x2 - 1, yc - 2);
    iupMATRIX_VERTEX(ih, x2 - 9, yc - 2);
  }
  else
  {
    iupMATRIX_VERTEX(ih, x2 - 1, yc + 2);
    iupMATRIX_VERTEX(ih, x2 - 9, yc + 2);
    iupMATRIX_VERTEX(ih, x2 - 5, yc - 2);
  }

  cdCanvasEnd(ih->data->cddbuffer);
  return 1;
}

void iupMatrixDrawSetDropFeedbackArea(int *x1, int *y1, int *x2, int *y2)
{
  *x2 -= IMAT_PADDING_W/2 + IMAT_FRAME_W/2;
  *x1  = *x2 - IMAT_DROPBOX_W; 
  *y1 += IMAT_PADDING_H/2 + IMAT_FRAME_H/2;
  *y2 -= IMAT_PADDING_H/2 + IMAT_FRAME_H/2;
}

static void iMatrixDrawDropFeedback(Ihandle* ih, int x2, int y1, int y2, int active, long framecolor)
{
  int xh2, yh2, x1;

  /* feedback area */
  iupMatrixDrawSetDropFeedbackArea(&x1, &y1, &x2, &y2);

  /* feedback background */
  iMatrixDrawSetBgColor(ih, 0, 0, 0, active);
  iupMATRIX_BOX(ih, x1, x2, y1, y2);

  /* feedback frame */
  cdCanvasForeground(ih->data->cddbuffer, framecolor);
  iupMATRIX_RECT(ih, x1, x2, y1, y2);

  /* feedback arrow */
  xh2 = x2 - IMAT_DROPBOX_W / 2;
  yh2 = y2 - (y2 - y1) / 2;

  cdCanvasBegin(ih->data->cddbuffer, CD_FILL);
  iupMATRIX_VERTEX(ih, xh2, yh2 + 3);
  iupMATRIX_VERTEX(ih, xh2 + 4, yh2 - 1);
  iupMATRIX_VERTEX(ih, xh2 - 4, yh2 - 1);
  cdCanvasEnd(ih->data->cddbuffer);
}

static void iMatrixDrawBackground(Ihandle* ih, int x1, int x2, int y1, int y2, int marked, int active, int lin, int col)
{
  /* avoid drawing over the frame of the next cell */
  x2 -= IMAT_FRAME_W/2;
  y2 -= IMAT_FRAME_H/2;

  iMatrixDrawSetBgColor(ih, lin, col, marked, active);
  iupMATRIX_BOX(ih, x1, x2, y1, y2);
}

/* Put the cell contents in the screen, using the specified color and alignment.
   -> y1, y2 : vertical limits of the cell
   -> x1, x2 : horizontal limits of the complete cell
   -> alignment : alignment type (horizontal) assigned to the text. The options are:
                  [IMAT_T_CENTER,IMAT_T_LEFT,IMAT_T_RIGHT]
   -> marked : mark state
   -> lin, col - cell coordinates */
static void iMatrixDrawCellValue(Ihandle* ih, int x1, int x2, int y1, int y2, int alignment, int marked, int active, int lin, int col, IFniiiiiiC draw_cb)
{
  char *text;

  /* avoid drawing over the frame of the next cell */
  x2 -= IMAT_FRAME_W/2;
  y2 -= IMAT_FRAME_H/2;

  /* avoid drawing over the frame of the cell */
  x2 -= IMAT_FRAME_W/2;
  y2 -= IMAT_FRAME_H/2;

  if (lin==0 || col==0)
  {
    /* avoid drawing over the frame of the cell */
    x1 += IMAT_FRAME_W/2;
    y1 += IMAT_FRAME_H/2;

    if (col==0) x1 += IMAT_FRAME_W/2;
    if (lin==0) y1 += IMAT_FRAME_H/2;
  }
  else if ((col==1 && ih->data->columns.sizes[0] == 0) || (lin==1 && ih->data->lines.sizes[0] == 0))
  {
    /* avoid drawing over the frame of the cell */
    x1 += IMAT_FRAME_W/2;
    y1 += IMAT_FRAME_H/2;
  }

  if (draw_cb && !iMatrixDrawCallDrawCB(ih, lin, col, x1, x2, y1, y2, draw_cb))
    return;

  text = iupMatrixCellGetValue(ih, lin, col);

  /* Put the text */
  if (text && *text)
  {
    int num_line, line_height, total_height;
    int charheight, ypos, hidden_text_marks = 0;

    num_line = iupStrLineCount(text);
    iupdrvFontGetCharSize(ih, NULL, &charheight);

    line_height  = charheight;
    total_height = (line_height + IMAT_PADDING_H/2) * num_line - IMAT_PADDING_H/2 - IMAT_FRAME_H/2;

    if (lin==0 || ih->data->hidden_text_marks)
    {
      int text_w;
      iupdrvFontGetMultiLineStringSize(ih, text, &text_w, NULL);
      if (text_w > x2 - x1 + 1 - IMAT_PADDING_W - IMAT_FRAME_W)
      {
        if (lin == 0)
          alignment = IMAT_T_LEFT;

        if (ih->data->hidden_text_marks)
          hidden_text_marks = 1;
      }
    }

    /* Set the color used to draw the text */
    if (!active)
      cdCanvasForeground(ih->data->cddbuffer, IMAT_CD_INACTIVE_FGCOLOR);
    else
      iMatrixDrawSetFgColor(ih, lin, col, marked);

    /* Set the clip area to the cell region informed, the text maybe greatter than the cell */
    if (hidden_text_marks)
    {
      int crop = iupdrvFontGetStringWidth(ih, "...") + 2;
      iMatrixDrawSetCellClipping(ih, x1, x2-crop, y1, y2);
    }
    else
      iMatrixDrawSetCellClipping(ih, x1, x2, y1, y2);

    cdIupSetFont(ih, ih->data->cddbuffer, iupMatrixGetFont(ih, lin, col));

    /* Create an space between text and cell frame */
    x1 += IMAT_PADDING_W/2;       x2 -= IMAT_PADDING_W/2;
    y1 += IMAT_PADDING_H/2;       y2 -= IMAT_PADDING_H/2;

    if (alignment == IMAT_T_CENTER)
      cdCanvasTextAlignment(ih->data->cddbuffer, CD_CENTER);
    else if(alignment == IMAT_T_LEFT)
      cdCanvasTextAlignment(ih->data->cddbuffer, CD_WEST);
    else
      cdCanvasTextAlignment(ih->data->cddbuffer, CD_EAST);

    if (num_line == 1)
    {
      ypos = (int)((y1 + y2) / 2.0 - 0.5);

      /* Put the text */
      if (alignment == IMAT_T_CENTER)
        iupMATRIX_TEXT(ih, (x1 + x2) / 2, ypos, text);
      else if(alignment == IMAT_T_LEFT)
        iupMATRIX_TEXT(ih, x1, ypos, text);
      else
        iupMATRIX_TEXT(ih, x2, ypos, text);
    }
    else
    {
      int i;
      char *p, *q, *newtext;

      newtext = iupStrDup(text);
      p = newtext;

      /* Get the position of the first text to be put in the screen */
      ypos = (int)( (y1 + y2) / 2.0 - 0.5) - total_height/2 + line_height/2;

      for(i = 0; i < num_line; i++)
      {
        q = strchr(p, '\n');
        if (q) *q = 0;  /* Cut the string to contain only one line */

        /* Draw the text */
        if(alignment == IMAT_T_CENTER)
          iupMATRIX_TEXT(ih, (x1 + x2) / 2, ypos, p);
        else if(alignment == IMAT_T_LEFT)
          iupMATRIX_TEXT(ih, x1, ypos, p);
        else
          iupMATRIX_TEXT(ih, x2, ypos, p);

        /* Advance the string */
        if (q) p = q + 1;

        /* Advance a line */
        ypos += line_height + IMAT_PADDING_H/2;
      }

      free(newtext);
    }

    iMatrixDrawResetCellClipping(ih);

    if (hidden_text_marks)
    {
      cdCanvasTextAlignment(ih->data->cddbuffer, CD_EAST);
      ypos = (int)((y1 + y2) / 2.0 - 0.5);
      iupMATRIX_TEXT(ih, x2+IMAT_PADDING_W/2, ypos, "...");
    }
  }
}

static void iMatrixDrawTitleCorner(Ihandle* ih)
{
  if (ih->data->lines.sizes[0] && ih->data->columns.sizes[0])
  {
    char str[100];
    long framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
    int active = iupdrvIsActive(ih);
    IFniiiiiiC draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");

    iMatrixDrawBackground(ih, 0, ih->data->columns.sizes[0], 0, ih->data->lines.sizes[0], 0, active, 0, 0);

    iMatrixDrawFrameRectTitle(ih, 0, 0, 0, ih->data->columns.sizes[0], 0, ih->data->lines.sizes[0], framecolor, str);

    iMatrixDrawCellValue(ih, 0, ih->data->columns.sizes[0], 0, ih->data->lines.sizes[0], IMAT_T_CENTER, 0, active, 0, 0, draw_cb);
  }
}

static void iMatrixDrawMatrix(Ihandle* ih)
{
  iupMatrixPrepareDrawData(ih);

  /* fill the background because there will be empty cells */
  if ((ih->data->lines.num == 1) || (ih->data->columns.num == 1))
  {
    cdCanvasBackground(ih->data->cddbuffer, cdIupConvertColor(ih->data->bgcolor_parent));
    cdCanvasClear(ih->data->cddbuffer);
  }

  /* Draw the corner between line and column titles, if necessary */
  iMatrixDrawTitleCorner(ih);

  /* If there are columns, then draw their titles */
  if (ih->data->columns.num_noscroll>1)
    iupMatrixDrawColumnTitle(ih, 1, ih->data->columns.num_noscroll-1);
  iupMatrixDrawColumnTitle(ih, ih->data->columns.first, ih->data->columns.last);

  /* If there are lines, then draw their titles */
  if (ih->data->lines.num_noscroll>1)
    iupMatrixDrawLineTitle(ih, 1, ih->data->lines.num_noscroll-1);
  iupMatrixDrawLineTitle(ih, ih->data->lines.first, ih->data->lines.last);

  /* If there are ordinary cells, then draw them */
  if (ih->data->columns.num_noscroll>1 && ih->data->lines.num_noscroll>1)
    iupMatrixDrawCells(ih, 1,                              1, 
                           ih->data->lines.num_noscroll-1, ih->data->columns.num_noscroll-1);
  if (ih->data->columns.num_noscroll>1)
    iupMatrixDrawCells(ih, ih->data->lines.first, 1, 
                           ih->data->lines.last,  ih->data->columns.num_noscroll-1);
  if (ih->data->lines.num_noscroll>1)
    iupMatrixDrawCells(ih, 1,                              ih->data->columns.first, 
                           ih->data->lines.num_noscroll-1, ih->data->columns.last);
  iupMatrixDrawCells(ih, ih->data->lines.first, ih->data->columns.first, 
                         ih->data->lines.last,  ih->data->columns.last);
}

static void iMatrixDrawFocus(Ihandle* ih)
{
  int x1, y1, x2, y2, dx, dy;

  if (iupAttribGetBoolean(ih, "HIDEFOCUS"))
    return;

  /* there are no cells that can get the focus */
  if (ih->data->columns.num <= 1 || ih->data->lines.num <= 1)
    return;

  if (!iupMatrixAuxIsCellVisible(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell))
    return;

  iupMatrixGetVisibleCellDim(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell, &x1, &y1, &dx, &dy);

  x2 = x1 + dx - 1;
  y2 = y1 + dy - 1;

  if (ih->data->columns.focus_cell == 1 && ih->data->columns.sizes[0] == 0)
    x1++;
  if (ih->data->lines.focus_cell == 1 && ih->data->lines.sizes[0] == 0)
    y1++;

  cdIupDrawFocusRect(ih, ih->data->cdcanvas, x1, iupMATRIX_INVERTYAXIS(ih, y1), x2, iupMATRIX_INVERTYAXIS(ih, y2));
}


/**************************************************************************/
/* Exported functions                                                     */
/**************************************************************************/


/* Draw the line titles, visible, between lin and lastlin, include it. 
   Line titles marked will be draw with the appropriate feedback.
   -> lin1 - First line to have its title drawn
   -> lin2 - Last line to have its title drawn */
void iupMatrixDrawLineTitle(Ihandle* ih, int lin1, int lin2)
{
  int x1, y1, x2, y2, first_lin;
  int lin, alignment, active;
  char str[100];
  long framecolor;
  IFniiiiiiC draw_cb;

  if (!ih->data->columns.sizes[0])
    return;

  if (ih->data->lines.num_noscroll>1 && lin1==1 && lin2==ih->data->lines.num_noscroll-1)
  {
    first_lin = 0;
    y1 = 0;
  }
  else
  {
    if (lin1 > ih->data->lines.last ||
        lin2 < ih->data->lines.first)
      return;

    if (lin1 < ih->data->lines.first)
      lin1 = ih->data->lines.first;
    if (lin2 > ih->data->lines.last)
      lin2 = ih->data->lines.last;

    first_lin = ih->data->lines.first;
    y1 = 0;
    for (lin = 0; lin< ih->data->lines.num_noscroll; lin++)
      y1 += ih->data->lines.sizes[lin];
  }

  /* Start the position of the line title */
  x1 = 0;
  x2 = ih->data->columns.sizes[0];

  iupMATRIX_CLIPAREA(ih, x1, x2, y1, ih->data->h-1);
  cdCanvasClip(ih->data->cddbuffer, CD_CLIPAREA);

  /* Find the initial position of the first line */
  if (first_lin == ih->data->lines.first)
    y1 -= ih->data->lines.first_offset;
  for(lin = first_lin; lin < lin1; lin++)
    y1 += ih->data->lines.sizes[lin];

  framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
  active = iupdrvIsActive(ih);
  draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");

  alignment = iMatrixDrawGetColAlignment(ih, 0, str);

  /* Draw the titles */
  for(lin = lin1; lin <= lin2; lin++)
  {
    /* If it is a hidden line (size = 0), don't draw the title */
    if(ih->data->lines.sizes[lin] == 0)
      continue;

    y2 = y1 + ih->data->lines.sizes[lin];

    /* If it doesn't have title, the loop just calculate the final position */
    if (ih->data->columns.sizes[0])
    {
      int marked = iupMatrixLineIsMarked(ih, lin);

      iMatrixDrawBackground(ih, x1, x2, y1, y2, marked, active, lin, 0);

      iMatrixDrawFrameRectTitle(ih, lin, 0, x1, x2, y1, y2, framecolor, str);

      iMatrixDrawCellValue(ih, x1, x2, y1, y2, alignment, marked, active, lin, 0, draw_cb);
    }

    y1 = y2;
  }

  cdCanvasClip(ih->data->cddbuffer, CD_CLIPOFF);
}

/* Draw the column titles, visible, between col and lastcol, include it. 
   Column titles marked will be draw with the appropriate feedback.
   -> col1 - First column to have its title drawn
   -> col2 - Last column to have its title drawn */
void iupMatrixDrawColumnTitle(Ihandle* ih, int col1, int col2)
{
  int x1, y1, x2, y2, first_col;
  int col, active;
  char str[100];
  long framecolor;
  IFniiiiiiC draw_cb;

  if (!ih->data->lines.sizes[0])
    return;

  if (ih->data->columns.num_noscroll>1 && col1==1 && col2==ih->data->columns.num_noscroll-1)
  {
    first_col = 0;
    x1 = 0;
  }
  else
  {
    if (col1 > ih->data->columns.last ||
        col2 < ih->data->columns.first)
      return;

    if (col1 < ih->data->columns.first)
      col1 = ih->data->columns.first;
    if (col2 > ih->data->columns.last)
      col2 = ih->data->columns.last;

    first_col = ih->data->columns.first;
    x1 = 0;
    for (col = 0; col< ih->data->columns.num_noscroll; col++)
      x1 += ih->data->columns.sizes[col];
  }

  /* Start the position of the first column title */
  y1 = 0;
  y2 = ih->data->lines.sizes[0];

  iupMATRIX_CLIPAREA(ih, x1, ih->data->w-1, y1, y2);
  cdCanvasClip(ih->data->cddbuffer, CD_CLIPAREA);

  /* Find the initial position of the first column */
  if (first_col==ih->data->columns.first)
    x1 -= ih->data->columns.first_offset;
  for(col = first_col; col < col1; col++)
    x1 += ih->data->columns.sizes[col];

  framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
  active = iupdrvIsActive(ih);
  draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");

  /* Draw the titles */
  for(col = col1; col <= col2; col++)
  {
    /* If it is an hide column (size = 0), no draw the title */
    if(ih->data->columns.sizes[col] == 0)
      continue;

    x2 = x1 + ih->data->columns.sizes[col];

    /* If it doesn't have title, the loop just calculate the final position */
    if (ih->data->lines.sizes[0])
    {
      int sort = 0;
      int marked = iupMatrixColumnIsMarked(ih, col);

      iMatrixDrawBackground(ih, x1, x2, y1, y2, marked, active, 0, col);

      iMatrixDrawFrameRectTitle(ih, 0, col, x1, x2, y1, y2, framecolor, str);

      if (iMatrixDrawSortSign(ih, x2, y1, y2, col, active, str))
        sort = IMAT_DROPBOX_W; /* same space is used by the sort sign */

      iMatrixDrawCellValue(ih, x1, x2-sort, y1, y2, IMAT_T_CENTER, marked, active, 0, col, draw_cb);
    }

    x1 = x2;
  }

  cdCanvasClip(ih->data->cddbuffer, CD_CLIPOFF);
}

/* Redraw a block of cells of the matrix. Handle marked cells, change
   automatically the background color of them.
   - lin1, col1 : cell coordinates that mark the left top corner of the area to be redrawn
   - lin2, col2 : cell coordinates that mark the right bottom corner of the area to be redrawn */
void iupMatrixDrawCells(Ihandle* ih, int lin1, int col1, int lin2, int col2)
{
  int x1, y1, x2, y2, old_x2, old_y1, old_y2;
  int alignment, lin, col, active, first_col, first_lin;
  long framecolor, emptyarea_color = -1;
  char str[100];
  IFnii mark_cb;
  IFnii dropcheck_cb;
  IFniiiiiiC draw_cb;

  x2 = ih->data->w-1;
  y2 = ih->data->h-1;

  old_x2 = x2;
  old_y1 = 0;
  old_y2 = y2;

  if (ih->data->lines.num <= 1 ||
      ih->data->columns.num <= 1)
    return;

  if (ih->data->columns.num_noscroll>1 && col1==1 && col2==ih->data->columns.num_noscroll-1)
  {
    first_col = 0;
    x1 = 0;
  }
  else
  {
    if (col1 > ih->data->columns.last ||
        col2 < ih->data->columns.first)
      return;

    if (col1 < ih->data->columns.first)
      col1 = ih->data->columns.first;
    if (col2 > ih->data->columns.last)
      col2 = ih->data->columns.last;

    first_col = ih->data->columns.first;
    x1 = 0;
    for (col = 0; col< ih->data->columns.num_noscroll; col++)
      x1 += ih->data->columns.sizes[col];
  }

  if (ih->data->lines.num_noscroll>1 && lin1==1 && lin2==ih->data->lines.num_noscroll-1)
  {
    first_lin = 0;
    y1 = 0;
  }
  else
  {
    if (lin1 > ih->data->lines.last ||
        lin2 < ih->data->lines.first)
      return;

    if (lin1 < ih->data->lines.first)
      lin1 = ih->data->lines.first;
    if (lin2 > ih->data->lines.last)
      lin2 = ih->data->lines.last;

    first_lin = ih->data->lines.first;
    y1 = 0;
    for (lin = 0; lin< ih->data->lines.num_noscroll; lin++)
      y1 += ih->data->lines.sizes[lin];
  }

  iupMATRIX_CLIPAREA(ih, x1, x2, y1, y2);
  cdCanvasClip(ih->data->cddbuffer, CD_CLIPOFF);  /* wait for background */

  /* Find the initial position of the first column */
  if (first_col==ih->data->columns.first)
    x1 -= ih->data->columns.first_offset;
  for(col = first_col; col < col1; col++)
    x1 += ih->data->columns.sizes[col];

  /* Find the initial position of the first line */
  if (first_lin == ih->data->lines.first)
    y1 -= ih->data->lines.first_offset;
  for(lin = first_lin; lin < lin1; lin++)
    y1 += ih->data->lines.sizes[lin];

  /* Find the final position of the last column */
  x2 = x1;
  for( ; col <= col2; col++)
    x2 += ih->data->columns.sizes[col];

  /* Find the final position of the last line */
  y2 = y1;
  for( ; lin <= lin2; lin++)
    y2 += ih->data->lines.sizes[lin];

  if ((col2 == ih->data->columns.num-1) && (old_x2 > x2))
  {
    emptyarea_color = cdIupConvertColor(ih->data->bgcolor_parent);
    cdCanvasForeground(ih->data->cddbuffer, emptyarea_color);

    /* If it was drawn until the last column and remains space in the right of it,
       then delete this area with the the background color. */
    iupMATRIX_BOX(ih, x2, old_x2, old_y1, old_y2);
  }

  if ((lin2 == ih->data->lines.num-1) && (old_y2 > y2))
  {
    if (emptyarea_color == -1)
      emptyarea_color = cdIupConvertColor(ih->data->bgcolor_parent);
    cdCanvasForeground(ih->data->cddbuffer, emptyarea_color);

    /* If it was drawn until the last line visible and remains space below it,
       then delete this area with the the background color. */
    iupMATRIX_BOX(ih, 0, old_x2, y2, old_y2);
  }

  /* after the background */
  cdCanvasClip(ih->data->cddbuffer, CD_CLIPAREA);

  /***** Draw the cell values and frame */
  old_y1 = y1;
  framecolor = cdIupConvertColor(iupAttribGetStr(ih, "FRAMECOLOR"));
  active = iupdrvIsActive(ih);

  mark_cb = (IFnii)IupGetCallback(ih, "MARK_CB");
  dropcheck_cb = (IFnii)IupGetCallback(ih, "DROPCHECK_CB");
  draw_cb = (IFniiiiiiC)IupGetCallback(ih, "DRAW_CB");

  for(col = col1; col <= col2; col++)  /* For all the columns in the region */
  {
    if (ih->data->columns.sizes[col] == 0)
      continue;

    alignment = iMatrixDrawGetColAlignment(ih, col, str);

    x2 = x1 + ih->data->columns.sizes[col];

    for(lin = lin1; lin <= lin2; lin++)     /* For all lines in the region */
    {
      int drop = 0;
      int marked = 0;

      if (ih->data->lines.sizes[lin] == 0)
        continue;

      y2 = y1 + ih->data->lines.sizes[lin];

      /* If the cell is marked, then draw it with attenuation color */
      marked = iupMatrixMarkCellGet(ih, lin, col, mark_cb, str);

      iMatrixDrawBackground(ih, x1, x2, y1, y2, marked, active, lin, col);

      iMatrixDrawFrameRectCell(ih, lin, col, x1, x2, y1, y2, framecolor, str);

      if (dropcheck_cb && dropcheck_cb(ih, lin, col) == IUP_DEFAULT)
      {
        drop = IMAT_DROPBOX_W+IMAT_PADDING_W/2;
        iMatrixDrawDropFeedback(ih, x2, y1, y2, active, framecolor);
      }
        
      /* draw the cell contents */
      iMatrixDrawCellValue(ih, x1, x2-drop, y1, y2, alignment, marked, active, lin, col, draw_cb);

      y1 = y2;
    }

    x1 = x2;
    y1 = old_y1;  /* must reset also y */
  }

  cdCanvasClip(ih->data->cddbuffer, CD_CLIPOFF);
}

void iupMatrixDraw(Ihandle* ih, int update)
{
  if (ih->data->need_calcsize)
    iupMatrixAuxCalcSizes(ih);

  iMatrixDrawMatrix(ih);
  ih->data->need_redraw = 0;

  if (update)
    iupMatrixDrawUpdate(ih);
}

void iupMatrixDrawUpdate(Ihandle* ih)
{
  cdCanvasFlush(ih->data->cddbuffer);

  if (ih->data->has_focus)
    iMatrixDrawFocus(ih);
}

int iupMatrixDrawSetRedrawAttrib(Ihandle* ih, const char* value)
{
  int type;

  if (value == NULL)
    type = 0;
  else if(value[0] == 'L' || value[0] == 'l')
    type = IMAT_PROCESS_LIN;
  else if(value[0] == 'C' || value[0] == 'c')
    type = IMAT_PROCESS_COL;
  else
    type = 0;

  if (type)  /* lines or columns, inluding their titles */
  {
    int min = 0, max = 0;
    value++;

    /* compatibility code */
    if (strchr(value, ':')!=NULL)
    {
      if(iupStrToIntInt(value, &min, &max, ':') != 2)
        max = min;
    }
    else
    {
      if(iupStrToIntInt(value, &min, &max, '-') != 2)
        max = min;
    }

    if (min > max)
      return 0;

    iupMatrixPrepareDrawData(ih);

    if (ih->data->need_calcsize)
      iupMatrixAuxCalcSizes(ih);

    /* ignore empty area, draw only cells */

    iMatrixDrawTitleCorner(ih);

    if (type == IMAT_PROCESS_LIN)
    {
      iupMatrixDrawLineTitle(ih, min, max);
      if (ih->data->columns.num_noscroll>1)
        iupMatrixDrawCells(ih, min, 1, max, ih->data->columns.num_noscroll-1);
      iupMatrixDrawCells(ih, min, ih->data->columns.first, max, ih->data->columns.last);
    }
    else
    {
      iupMatrixDrawColumnTitle(ih, min, max);
      if (ih->data->lines.num_noscroll>1)
        iupMatrixDrawCells(ih, 1, min, ih->data->lines.num_noscroll-1, max);
      iupMatrixDrawCells(ih, ih->data->lines.first, min, ih->data->lines.last, max);
    }
  }
  else
  {
    /* Force CalcSize */
    iupMatrixAuxCalcSizes(ih);

    iMatrixDrawMatrix(ih);
  }

  ih->data->need_redraw = 0;
  iupMatrixDrawUpdate(ih);
  return 0;
}
