/** \file
 * \brief iupmatrix control
 * keyboard control
 *
 * See Copyright Notice in "iup.h"
 */

/**************************************************************************/
/* Functions to control keys in the matrix and in the text edition        */
/**************************************************************************/

#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include <cd.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_stdcontrols.h"

#include "iupmat_def.h"
#include "iupmat_scroll.h"
#include "iupmat_aux.h"
#include "iupmat_getset.h"
#include "iupmat_key.h"
#include "iupmat_mark.h"
#include "iupmat_edit.h"
#include "iupmat_draw.h"


int iupMatrixProcessKeyPress(Ihandle* ih, int c)
{
  int ret = IUP_IGNORE; /* default for processed keys */

  /* If the focus is not visible, a scroll is done for that the focus to be visible */
  if (!iupMatrixAuxIsCellStartVisible(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell))
    iupMatrixScrollToVisible(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell);

  switch (c)
  {
    case K_cHOME:
    case K_sHOME:
    case K_HOME:
      if(iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        break;
      iupMATRIX_ScrollKeyHome(ih);
      ih->data->homekeycount++;
      iupMatrixAuxCallEnterCellCb(ih);
      break;

    case K_cEND:
    case K_sEND:
    case K_END:
      if(iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        break;
      iupMATRIX_ScrollKeyEnd(ih);
      ih->data->endkeycount++;
      iupMatrixAuxCallEnterCellCb(ih);
      break;

    case K_sTAB:
    case K_TAB:
      return IUP_CONTINUE;  /* do not redraw */

    case K_cLEFT:
    case K_sLEFT:
    case K_LEFT:
      if (iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        break;
      iupMATRIX_ScrollKeyLeft(ih);
      iupMatrixAuxCallEnterCellCb(ih);
      break;

    case K_cRIGHT:
    case K_sRIGHT:
    case K_RIGHT:
      if(iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        break;
      iupMATRIX_ScrollKeyRight(ih);
      iupMatrixAuxCallEnterCellCb(ih);
      break;

    case K_cUP:
    case K_sUP:
    case K_UP:
      if(iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        break;
      iupMATRIX_ScrollKeyUp(ih);
      iupMatrixAuxCallEnterCellCb(ih);
      break ;

    case K_cDOWN:
    case K_sDOWN:
    case K_DOWN:
      if(iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        break;
      iupMATRIX_ScrollKeyDown(ih);
      iupMatrixAuxCallEnterCellCb(ih);
      break;

    case K_sPGUP:
    case K_PGUP:
      if(iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        break;
      iupMATRIX_ScrollKeyPgUp(ih);
      iupMatrixAuxCallEnterCellCb(ih);
      break;

    case K_sPGDN:
    case K_PGDN:
      if(iupMatrixAuxCallLeaveCellCb(ih) == IUP_IGNORE)
        break;
      iupMATRIX_ScrollKeyPgDown(ih);
      iupMatrixAuxCallEnterCellCb(ih);
      break;

    case K_F2:
    case K_SP:
    case K_CR:
    case K_sCR:
      if (iupMatrixEditShow(ih))
        return IUP_IGNORE; /* do not redraw */
      break;

    case K_sDEL:
    case K_DEL:
      {
        int lin, col;
        char str[100];
        IFnii mark_cb = (IFnii)IupGetCallback(ih, "MARK_CB");

        iupMatrixPrepareDrawData(ih);

        for(lin = 1; lin < ih->data->lines.num; lin++)
        {
          for(col = 1; col < ih->data->columns.num; col++)
          {
            if (iupMatrixMarkCellGet(ih, lin, col, mark_cb, str))
            {
              if (iupMatrixAuxCallEditionCbLinCol(ih, lin, col, 1, 1) != IUP_IGNORE)
              {
                IFniis value_edit_cb;

                iupMatrixCellSetValue(ih, lin, col, NULL);

                value_edit_cb = (IFniis)IupGetCallback(ih, "VALUE_EDIT_CB");
                if (value_edit_cb)
                  value_edit_cb(ih, lin, col, NULL);

                iupMatrixDrawCells(ih, lin, col, lin, col);
              }
            }
          }
        }
        break;
      }
    default:
      /* if a valid character is pressed enter edition mode */
      if (iup_isprint(c))
      {
        if (iupMatrixEditShow(ih))
        {
          if (ih->data->datah == ih->data->texth)
          {
            char value[2] = {0,0};
            value[0] = (char)c;
            IupStoreAttribute(ih->data->datah, "VALUEMASKED", value);
            IupSetAttribute(ih->data->datah, "CARET", "2");
          }
          return IUP_IGNORE; /* do not redraw */
        }
      }
      ret = IUP_DEFAULT; /* unprocessed keys */
      break;
  }

  iupMatrixDrawUpdate(ih);  
  return ret;
}

void iupMatrixKeyResetHomeEndCount(Ihandle* ih)
{
  ih->data->homekeycount = 0;
  ih->data->endkeycount = 0;
}

int iupMatrixKeyPress_CB(Ihandle* ih, int c, int press)
{
  int oldc = c;
  IFniiiis cb;

  if (!iupMatrixIsValid(ih, 1))
    return IUP_DEFAULT;

  /* there are no cells that can get the focus */
  if (ih->data->columns.num <= 1 || ih->data->lines.num <= 1)
    return IUP_DEFAULT;

  if (!press)
    return IUP_DEFAULT;

  cb = (IFniiiis)IupGetCallback(ih, "ACTION_CB");
  if (cb)
  {
    if (iup_isprint(c))
    {
      char value[2]={0,0};
      value[0] = (char)c;
      c = cb(ih, c, ih->data->lines.focus_cell, ih->data->columns.focus_cell, 0, value);
    }
    else
    {
      c = cb(ih, c, ih->data->lines.focus_cell, ih->data->columns.focus_cell, 0,
             iupMatrixCellGetValue(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell));
    }

    if (c == IUP_IGNORE || c == IUP_CLOSE || c == IUP_CONTINUE)
      return c;
    else if (c == IUP_DEFAULT)
      c = oldc;
  }

  if (c != K_HOME && c != K_sHOME)
    ih->data->homekeycount = 0;
  if (c != K_END && c != K_sEND)
    ih->data->endkeycount = 0;

  return iupMatrixProcessKeyPress(ih, c);
}
