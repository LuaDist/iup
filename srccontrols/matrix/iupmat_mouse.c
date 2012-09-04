/** \file
 * \brief iupmatrix control
 * mouse events
 *
 * See Copyright Notice in "iup.h"
 */

/**************************************************************************/
/* Functions to handle mouse events                                       */
/**************************************************************************/

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
#include "iupmat_aux.h"
#include "iupmat_mouse.h"
#include "iupmat_key.h"
#include "iupmat_mark.h"
#include "iupmat_edit.h"
#include "iupmat_draw.h"
#include "iupmat_getset.h"
#include "iupmat_scroll.h"


#define IMAT_DRAG_SCROLL_DELTA 5

static void iMatrixMouseCallMoveCb(Ihandle* ih, int lin, int col)
{
  IFnii cb = (IFnii)IupGetCallback(ih, "MOUSEMOVE_CB");
  if (cb)
    cb(ih, lin, col);
}

static int iMatrixMouseCallClickCb(Ihandle* ih, int press, int lin, int col, char* r)
{
  IFniis cb;

  if (press)
    cb = (IFniis)IupGetCallback(ih, "CLICK_CB");
  else
    cb = (IFniis)IupGetCallback(ih, "RELEASE_CB");

  if (cb)
    return cb(ih, lin, col, r);
                       
  return IUP_DEFAULT;
}

static void iMatrixMouseEdit(Ihandle* ih)
{
  if (iupMatrixEditShow(ih))
  {
    if (ih->data->datah == ih->data->droph)
      IupSetAttribute(ih->data->datah, "SHOWDROPDOWN", "YES");

    if (IupGetGlobal("MOTIFVERSION"))
    {
      /* Sequece of focus_cb in Motif from here:
            Matrix-Focus(0) - ok
            Edit-KillFocus - weird, must avoid using _IUPMAT_DOUBLECLICK
         Since OpenMotif version 2.2.3 this is not necessary anymore. */
      if (atoi(IupGetGlobal("MOTIFNUMBER")) < 2203) 
        iupAttribSetStr(ih, "_IUPMAT_DOUBLECLICK", "1");
    }
  }

  /* reset mouse flags */
  ih->data->dclick = 0;
  ih->data->leftpressed = 0;
}

static int iMatrixIsDropArea(Ihandle* ih, int lin, int col, int x, int y)
{
  IFnii dropcheck_cb = (IFnii)IupGetCallback(ih, "DROPCHECK_CB");
  if (dropcheck_cb && dropcheck_cb(ih, lin, col) == IUP_DEFAULT)
  {
    int x1, y1, x2, y2;

    iupMatrixGetVisibleCellDim(ih, lin, col, &x1, &y1, &x2, &y2);
    x2 += x1;  /* the previous fucntion returns w and h */
    y2 += y1;

    iupMatrixDrawSetDropFeedbackArea(&x1, &y1, &x2, &y2);

    if (x > x1 && x < x2 &&
        y > y1 && y < y2)
      return 1;
  }
  return 0;
}

static void iMatrixMouseLeftPress(Ihandle* ih, int lin, int col, int shift, int ctrl, int dclick, int x, int y)
{
  if (dclick)
  {
    iupMatrixMarkMouseReset(ih);

    if (lin==0 || col==0)
      return;

    /* if a double click NOT in the current cell */
    if (lin != ih->data->lines.focus_cell || col != ih->data->columns.focus_cell)
    {
      /* leave the previous cell if the matrix previously had the focus */
      if (ih->data->has_focus && iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        return;

      ih->data->lines.focus_cell = lin;
      ih->data->columns.focus_cell = col;

      iupMatrixAuxCallEnterCellCb(ih);
    }
    
    ih->data->dclick = 1; /* prepare for double click action */
  }
  else /* single click */
  {
    if (shift && ih->data->mark_multiple && ih->data->mark_mode != IMAT_MARK_NO)
    {
      iupMatrixMarkMouseBlock(ih, lin, col);
    }
    else
    {
      ih->data->leftpressed = 1;

      if (lin>0 && col>0)
      {
        if (iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
          return;

        ih->data->lines.focus_cell = lin;
        ih->data->columns.focus_cell = col;

        /* process mark before EnterCell */
        if (ih->data->mark_mode != IMAT_MARK_NO)
          iupMatrixMarkMouseItem(ih, ctrl, lin, col);

        iupMatrixAuxCallEnterCellCb(ih);

        if (iMatrixIsDropArea(ih, lin, col, x, y))
          iMatrixMouseEdit(ih);
      }
      else
      {
        /* only process marks if at titles */
        if (ih->data->mark_mode != IMAT_MARK_NO)
          iupMatrixMarkMouseItem(ih, ctrl, lin, col);
      }
    }
  }
}

int iupMatrixMouseButton_CB(Ihandle* ih, int b, int press, int x, int y, char* r)
{
  int lin=-1, col=-1;

  if (!iupMatrixIsValid(ih, 0))
    return IUP_DEFAULT;

  /* reset press state */
  ih->data->leftpressed = 0;

  if (press)
  {
    ih->data->dclick = 0;

    /* The edit Focus callback is not called when the user clicks in the parent canvas. 
       so we have to compensate that. */
    iupMatrixEditForceHidden(ih);
    ih->data->has_focus = 1;
  }

  iupMatrixGetCellFromOffset(ih, x, y, &lin, &col);

  if (b == IUP_BUTTON1)
  {
    if (press)
    {
      iupMatrixKeyResetHomeEndCount(ih);

      if (iupMatrixColResStart(ih, x, y))
        return IUP_DEFAULT;  /* Resize of the width a of a column was started */

      if (lin!=-1 && col!=-1)
        iMatrixMouseLeftPress(ih, lin, col, isshift(r), iscontrol(r), isdouble(r), x, y);
    }
    else
    {
      if (iupMatrixColResIsResizing(ih))  /* If it was made a column resize, finish it */
        iupMatrixColResFinish(ih, x);

      if (ih->data->dclick)  /* when releasing the button from a double click */
        iMatrixMouseEdit(ih);
    }
  }
  else
    iupMatrixMarkMouseReset(ih);

  if (lin!=-1 && col!=-1)
  {
    if (iMatrixMouseCallClickCb(ih, press, lin, col, r) == IUP_IGNORE)
      return IUP_DEFAULT;
  }

  iupMatrixDrawUpdate(ih);
  return IUP_DEFAULT;
}

int iupMatrixMouseMove_CB(Ihandle* ih, int x, int y)
{
  int lin, col;

  if (!iupMatrixIsValid(ih, 0))
    return IUP_DEFAULT;

  if (ih->data->leftpressed && ih->data->mark_multiple && ih->data->mark_mode != IMAT_MARK_NO)
  {
    if ((x < ih->data->columns.sizes[0] || x < IMAT_DRAG_SCROLL_DELTA) && (ih->data->columns.first > ih->data->columns.num_noscroll))
      iupMATRIX_ScrollLeft(ih);
    else if ((x > ih->data->w - IMAT_DRAG_SCROLL_DELTA) && (ih->data->columns.last < ih->data->columns.num-1))
      iupMATRIX_ScrollRight(ih);

    if ((y < ih->data->lines.sizes[0] || y < IMAT_DRAG_SCROLL_DELTA) && (ih->data->lines.first > ih->data->lines.num_noscroll))
      iupMATRIX_ScrollUp(ih);
    else if ((y > ih->data->h - IMAT_DRAG_SCROLL_DELTA) && (ih->data->lines.last < ih->data->lines.num-1))
      iupMATRIX_ScrollDown(ih);

    if (iupMatrixGetCellFromOffset(ih, x, y, &lin, &col))
    {
      iupMatrixMarkMouseBlock(ih, lin, col);
      iupMatrixDrawUpdate(ih);

      iMatrixMouseCallMoveCb(ih, lin, col);
    }
    return IUP_DEFAULT;
  }
  else if(iupMatrixColResIsResizing(ih)) /* Make a resize in a column size */
    iupMatrixColResMove(ih, x);
  else /* Change cursor when it is passed on a join involving column titles */
    iupMatrixColResCheckChangeCursor(ih, x, y);

  if (iupMatrixGetCellFromOffset(ih, x, y, &lin, &col))
    iMatrixMouseCallMoveCb(ih, lin, col);

  return IUP_DEFAULT;
}
