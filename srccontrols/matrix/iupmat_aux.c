/** \file
 * \brief iupmatrix control
 * auxiliary functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include <cd.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"

#include "iupmat_def.h"
#include "iupmat_aux.h"
#include "iupmat_getset.h"


int iupMatrixAuxIsFullVisibleLast(ImatLinColData *p)
{
  int i, sum = 0;

  for(i = p->first; i <= p->last; i++)
  {
    sum += p->sizes[i];
    if (i==p->first)
      sum -= p->first_offset;
  }

  if (sum > p->visible_size)
    return 0;
  else
    return 1;
}

int iupMatrixAuxIsCellStartVisible(Ihandle* ih, int lin, int col)
{
  if (iupMatrixAuxIsCellVisible(ih, lin, col))
  {
    if (col == ih->data->columns.first && ih->data->columns.first_offset!=0)
      return 0;
    if (lin == ih->data->lines.first && ih->data->lines.first_offset!=0)
      return 0;
    if (col == ih->data->columns.last && !iupMatrixAuxIsFullVisibleLast(&ih->data->columns))
      return 0;
    if (lin == ih->data->lines.last && !iupMatrixAuxIsFullVisibleLast(&ih->data->lines))
      return 0;

    return 1;
  }

  return 0;
}

int iupMatrixAuxIsCellVisible(Ihandle* ih, int lin, int col)
{
  if ((col < ih->data->columns.num_noscroll)  &&
      (lin >= ih->data->lines.first) && (lin <= ih->data->lines.last))
    return 1;

  if ((lin < ih->data->lines.num_noscroll) &&
      (col >= ih->data->columns.first) && (col <= ih->data->columns.last))
    return 1;

  if ((lin >= ih->data->lines.first) && (lin <= ih->data->lines.last) &&
      (col >= ih->data->columns.first) && (col <= ih->data->columns.last))
  {
    return 1;
  }

  return 0;
}

void iupMatrixAuxAdjustFirstFromLast(ImatLinColData* p)
{
  int i, sum = 0;

  /* adjust "first" according to "last" */

  i = p->last;
  sum = p->sizes[i];
  while (i>p->num_noscroll && sum < p->visible_size)
  {
    i--;
    sum += p->sizes[i];
  }

  if (i==p->num_noscroll && sum < p->visible_size)
  {
    /* if there are room for everyone then position at start */
    p->first = p->num_noscroll;
    p->first_offset = 0;
  }
  else
  {
    /* the "while" found an index for first */
    p->first = i;

    /* position at the remaing space */
    p->first_offset = sum - p->visible_size;
  }
}

void iupMatrixAuxAdjustFirstFromScrollPos(ImatLinColData* p, int scroll_pos)
{
  int index, sp, offset = 0;

  sp = 0;
  for(index = p->num_noscroll; index < p->num; index++)
  {
    sp += p->sizes[index];
    if (sp > scroll_pos)
    {
      sp -= p->sizes[index]; /* get the previous value */
      offset = scroll_pos - sp;
      break;
    }
  }

  if (index == p->num)
  {
    if (p->num == p->num_noscroll)
    {
      /* did NOT go trough the "for" above */
      offset = scroll_pos;
      index = p->num_noscroll;
    }
    else
    {
      /* go all the way trough the "for" above, but still sp < scroll_pos */
      offset = scroll_pos - sp;
      index = p->num-1;
    }
  }

  p->first = index;
  p->first_offset = offset;
}

/* Calculate the size, in pixels, of the invisible columns/lines,
   the left/above of the first column/line.
   In fact the start position of the visible area.
   Depends on the first visible column/line.
   -> m : choose will operate on lines or columns [IMAT_PROCESS_LIN|IMAT_PROCESS_COL]
*/
void iupMatrixAuxUpdateScrollPos(Ihandle* ih, int m)
{
  float pos;
  int i, sb, scroll_pos;
  char* POS;
  ImatLinColData *p;

  if (m == IMAT_PROCESS_LIN)
  {
    p = &(ih->data->lines);
    sb = IUP_SB_VERT;
    POS = "POSY";
  }
  else
  {
    p = &(ih->data->columns);
    sb = IUP_SB_HORIZ;
    POS = "POSX";
  }

  /* "first" was changed, so update "last" and the scroll pos */

  if (p->total_size <= p->visible_size)
  {
    /* the matrix is fully visible */
    p->first = p->num_noscroll;
    p->first_offset = 0;
    p->last = p->num==p->num_noscroll? p->num_noscroll: p->num-1;

    if (ih->data->canvas.sb & sb)
      IupSetAttribute(ih, POS, "0");

    return;
  }

  /* must check if it is a valid position */
  scroll_pos = 0;
  for(i = p->num_noscroll; i < p->first; i++)
    scroll_pos += p->sizes[i];
  scroll_pos += p->first_offset;

  if (scroll_pos + p->visible_size > p->total_size)
  {
    /* invalid condition, must recalculate so it is valid */
    scroll_pos = p->total_size - p->visible_size;

    /* position first and first_offset, according to scroll pos */
    iupMatrixAuxAdjustFirstFromScrollPos(p, scroll_pos);
  }

  pos = (float)scroll_pos/(float)p->total_size;

  /* update last */
  iupMatrixAuxUpdateLast(p);

  /* update scroll pos */
  if (ih->data->canvas.sb & sb)
    IupSetfAttribute(ih, POS, "%g", (double)pos);
}

/* Calculate which is the last visible column/line of the matrix. 
   Depends on the first visible column/line.  */
void iupMatrixAuxUpdateLast(ImatLinColData *p)
{
  int i, sum = 0;

  if (p->visible_size > 0)
  {
    /* Find which is the last column/line.
       Start in the first visible and continue adding the widths
       up to the visible size */
    for(i = p->first; i < p->num; i++)
    {
      sum += p->sizes[i];
      if (i==p->first)
        sum -= p->first_offset;

      if(sum >= p->visible_size)
        break;
    }

    if (i == p->num)
    {
      if (p->num == p->num_noscroll)
        p->last = p->num_noscroll;
      else
        p->last = p->num-1;
    }
    else
      p->last = i;
  }
  else
  {
    /* There is no space for any column, set the last column as 0 */
    p->last = 0;
  }
}

/* Fill the sizes vector with the width/heigh of all the columns/lines.
   Calculate the value of total_size */
static void iMatrixAuxFillSizeVec(Ihandle* ih, int m)
{
  int i;
  ImatLinColData *p;

  if (m == IMAT_PROCESS_LIN)
    p = &(ih->data->lines);
  else
    p = &(ih->data->columns);

  /* Calculate total width/height of the matrix and the width/height of each column */
  p->total_size = 0;
  for(i = 0; i < p->num; i++)
  {
    if (m == IMAT_PROCESS_LIN)
      p->sizes[i] = iupMatrixGetLineHeight(ih, i, 1);
    else
      p->sizes[i] = iupMatrixGetColumnWidth(ih, i, 1);

    if (i >= p->num_noscroll)
      p->total_size += p->sizes[i];
  }
}

static void iMatrixAuxUpdateVisibleSize(Ihandle* ih, int m)
{
  char* D;
  ImatLinColData *p;
  int canvas_size, fixed_size, i;

  if (m == IMAT_PROCESS_LIN)
  {
    D = "DY";
    p = &(ih->data->lines);
    canvas_size = ih->data->h;
  }
  else
  {
    D = "DX";
    p = &(ih->data->columns);
    canvas_size = ih->data->w;
  }

  fixed_size = 0;
  for (i=0; i<p->num_noscroll; i++)
    fixed_size += p->sizes[i];

  /* Matrix useful area is the current size minus the title area */
  p->visible_size = canvas_size - fixed_size;
  if (p->visible_size > p->total_size)
    p->visible_size = p->total_size;

  if (p->total_size)
    IupSetfAttribute(ih, D, "%g", (double)p->visible_size/(double)p->total_size);
  else
    IupSetAttribute(ih, D, "1");
}

void iupMatrixAuxCalcSizes(Ihandle* ih)
{
  iMatrixAuxFillSizeVec(ih, IMAT_PROCESS_COL);
  iMatrixAuxFillSizeVec(ih, IMAT_PROCESS_LIN);

  iMatrixAuxUpdateVisibleSize(ih, IMAT_PROCESS_COL);
  iMatrixAuxUpdateVisibleSize(ih, IMAT_PROCESS_LIN);

  /* when removing lines the first can be positioned after the last line */
  if (ih->data->lines.first > ih->data->lines.num-1) 
  {
    ih->data->lines.first_offset = 0;
    if (ih->data->lines.num==ih->data->lines.num_noscroll)
      ih->data->lines.first = ih->data->lines.num_noscroll;
    else
      ih->data->lines.first = ih->data->lines.num-1;
  }
  if (ih->data->columns.first > ih->data->columns.num-1) 
  {
    ih->data->columns.first_offset = 0;
    if (ih->data->columns.num==ih->data->columns.num_noscroll)
      ih->data->columns.first = ih->data->columns.num_noscroll;
    else
      ih->data->columns.first = ih->data->columns.num-1;
  }

  /* make sure scroll pos is consistent */
  iupMatrixAuxUpdateScrollPos(ih, IMAT_PROCESS_COL);
  iupMatrixAuxUpdateScrollPos(ih, IMAT_PROCESS_LIN);

  ih->data->need_calcsize = 0;
}

int iupMatrixAuxCallLeaveCellCb(Ihandle* ih)
{
  if (ih->data->columns.num > 1 && ih->data->lines.num > 1)
  {
    IFnii cb = (IFnii)IupGetCallback(ih, "LEAVEITEM_CB");
    if(cb)
      return cb(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell);
  }
  return IUP_DEFAULT;
}

void iupMatrixAuxCallEnterCellCb(Ihandle* ih)
{
  if (ih->data->columns.num > 1 && ih->data->lines.num > 1)
  {
    IFnii cb = (IFnii)IupGetCallback(ih, "ENTERITEM_CB");
    if (cb)
      cb(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell);
  }
}

int iupMatrixAuxCallEditionCbLinCol(Ihandle* ih, int lin, int col, int mode, int update)
{
  IFniiii cb;

  if (iupAttribGetBoolean(ih, "READONLY"))
    return IUP_IGNORE;

  cb = (IFniiii)IupGetCallback(ih, "EDITION_CB");
  if(cb)
    return cb(ih, lin, col, mode, update);
  return IUP_DEFAULT;
}
