/** \file
 * \brief iupmatrix setget control
 * attributes set and get
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
#include "iup_drvfont.h"
#include "iup_str.h"
#include "iup_stdcontrols.h"
#include "iup_controls.h"
#include "iup_childtree.h"

#include "iupmat_def.h"
#include "iupmat_getset.h"
#include "iupmat_edit.h"
#include "iupmat_draw.h"
#include "iupmat_aux.h"


int iupMatrixCheckCellPos(Ihandle* ih, int lin, int col)
{
  /* Check if the cell exists */
  if (!iupMATRIX_CHECK_COL(ih, col) ||
      !iupMATRIX_CHECK_LIN(ih, lin))
    return 0;

  return 1;
}

void iupMatrixCellSetValue(Ihandle* ih, int lin, int col, const char* value)
{
  if (ih->data->callback_mode)
    return;

  if (ih->data->cells[lin][col].value)
    free(ih->data->cells[lin][col].value);

  ih->data->cells[lin][col].value = iupStrDup(value);

  ih->data->need_redraw = 1;
  if (lin==0 || col==0)
    ih->data->need_calcsize = 1;
}

char* iupMatrixCellGetValue (Ihandle* ih, int lin, int col)
{
  if (!ih->handle)
  {
    char str[100];
    sprintf(str, "%d:%d", lin, col);
    return iupAttribGet(ih, str);
  }
  else
  {
    if (ih->data->callback_mode)
    {
      sIFnii value_cb = (sIFnii)IupGetCallback(ih, "VALUE_CB");
      return value_cb(ih, lin, col);
    }
    else
      return ih->data->cells[lin][col].value;
  }
}

void iupMatrixCellSetFlag(Ihandle* ih, int lin, int col, unsigned char attr, int set)
{
  if (!ih->handle)
    return;

  if (lin==IUP_INVALID_ID)
  {
    if (!iupMATRIX_CHECK_COL(ih, col))
      return;

    if (set)
      ih->data->columns.flags[col] |= attr;
    else
      ih->data->columns.flags[col] &= ~attr;
  }
  else if (col==IUP_INVALID_ID)
  {
    if (!iupMATRIX_CHECK_LIN(ih, lin))
      return;

    if (set)
      ih->data->lines.flags[lin] |= attr;
    else
      ih->data->lines.flags[lin] &= ~attr;
  }
  else
  {
    if (!ih->data->callback_mode)
    {
      if (!iupMATRIX_CHECK_COL(ih, col) ||
          !iupMATRIX_CHECK_LIN(ih, lin))
        return;

      if (set)
        ih->data->cells[lin][col].flags |= attr;
      else
        ih->data->cells[lin][col].flags &= ~attr;
    }
  }
}

void iupMatrixCellUpdateValue(Ihandle* ih)
{
  IFniis value_edit_cb;
  char *value = iupMatrixEditGetValue(ih);

  iupMatrixCellSetValue(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell, value);

  value_edit_cb = (IFniis)IupGetCallback(ih, "VALUE_EDIT_CB");
  if (value_edit_cb)
    value_edit_cb(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell, value);

  iupMatrixPrepareDrawData(ih);
  iupMatrixDrawCells(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell, ih->data->lines.focus_cell, ih->data->columns.focus_cell);
}

void iupMatrixPrepareDrawData(Ihandle* ih)
{
  /* FGCOLOR, BGCOLOR and FONT */
  iupAttribStoreStr(ih, "_IUPMAT_BGCOLOR_PARENT", iupControlBaseGetParentBgColor(ih));
  iupAttribStoreStr(ih, "_IUPMAT_BGCOLOR", IupGetAttribute(ih, "BGCOLOR"));
  iupAttribStoreStr(ih, "_IUPMAT_FGCOLOR", IupGetAttribute(ih, "FGCOLOR"));
  iupAttribStoreStr(ih, "_IUPMAT_FONT", IupGetAttribute(ih, "FONT"));

  ih->data->bgcolor_parent = iupAttribGet(ih, "_IUPMAT_BGCOLOR_PARENT");
  ih->data->bgcolor = iupAttribGet(ih, "_IUPMAT_BGCOLOR");
  ih->data->fgcolor = iupAttribGet(ih, "_IUPMAT_FGCOLOR");
  ih->data->font = iupAttribGet(ih, "_IUPMAT_FONT");

  ih->data->font_cb = (sIFnii)IupGetCallback(ih, "FONT_CB");
  ih->data->fgcolor_cb = (IFniiIII)IupGetCallback(ih, "FGCOLOR_CB");
  ih->data->bgcolor_cb = (IFniiIII)IupGetCallback(ih, "BGCOLOR_CB");
}

static char* iMatrixGetCellAttrib(Ihandle* ih, unsigned char attr, int lin, int col, int native_parent)
{
  char* value = NULL;
  char attrib_id[100];
  const char* attrib = NULL;
  char* attrib_global = NULL; 

  if (attr == IMAT_HAS_FONT)
  {
    attrib = "FONT";
    attrib_global = ih->data->font;
  }
  else if (attr == IMAT_HAS_BGCOLOR)
  {
    attrib = "BGCOLOR";
    attrib_global = ih->data->bgcolor;
  }
  else if (attr == IMAT_HAS_FGCOLOR)
  {
    attrib = "FGCOLOR";
    attrib_global = ih->data->fgcolor;
  }

  /* 1 -  check for this cell */
  if (ih->data->callback_mode || ih->data->cells[lin][col].flags & attr)
  {
    sprintf(attrib_id, "%s%d:%d", attrib, lin, col);
    value = iupAttribGet(ih, attrib_id);
  }
  if (!value)
  {
    /* 2 - check for this line, if not title col */
    if (col != 0)
    {
      if (ih->data->lines.flags[lin] & attr)
      {
        sprintf(attrib_id, "%s%d:*", attrib, lin);
        value = iupAttribGet(ih, attrib_id);
      }
    }

    if (!value)
    {
      /* 3 - check for this column, if not title line */
      if (lin != 0)
      {
        if (ih->data->columns.flags[col] & attr)
        {
          sprintf(attrib_id,"%s*:%d", attrib, col);
          value = iupAttribGet(ih, attrib_id);
        }
      }

      if (!value)
      {
        /* 4 - check for the matrix or native parent */
        if (native_parent)
          value = ih->data->bgcolor_parent;
        else
          value = attrib_global;
      }
    }
  }

  return value;
}

static int iMatrixCallColorCB(Ihandle* ih, IFniiIII cb, int lin, int col, unsigned char *r, unsigned char *g, unsigned char *b)
{
  int ir, ig, ib, ret;
  ret = cb(ih, lin, col, &ir, &ig, &ib);
  *r = iupBYTECROP(ir);
  *g = iupBYTECROP(ig);
  *b = iupBYTECROP(ib);
  return ret;
}

char* iupMatrixGetFgColor(Ihandle* ih, int lin, int col)
{
  unsigned char r = 0, g = 0, b = 0;
  /* called from Edit only */
  if (!ih->data->fgcolor_cb || (iMatrixCallColorCB(ih, ih->data->fgcolor_cb, lin, col, &r, &g, &b) == IUP_IGNORE))
  {
    char* fgcolor = iMatrixGetCellAttrib(ih, IMAT_HAS_FGCOLOR, lin, col, 0);
    if (!fgcolor) 
    {
      if (lin ==0 || col == 0)
        fgcolor = IupGetGlobal("DLGFGCOLOR");
      else
        fgcolor = IupGetGlobal("TXTFGCOLOR");
    }
    return fgcolor;
  }
  else
  {
    char* buffer = iupStrGetMemory(30);
    sprintf(buffer, "%d %d %d", r, g, b);
    return buffer;
  }
}

void iupMatrixGetFgRGB(Ihandle* ih, int lin, int col, unsigned char *r, unsigned char *g, unsigned char *b)
{
  /* called from Draw only */
  if (!ih->data->fgcolor_cb || (iMatrixCallColorCB(ih, ih->data->fgcolor_cb, lin, col, r, g, b) == IUP_IGNORE))
  {
    char* fgcolor = iMatrixGetCellAttrib(ih, IMAT_HAS_FGCOLOR, lin, col, 0);
    if (!fgcolor) 
    {
      if (lin ==0 || col == 0)
        fgcolor = IupGetGlobal("DLGFGCOLOR");
      else
        fgcolor = IupGetGlobal("TXTFGCOLOR");
    }
    iupStrToRGB(fgcolor, r, g, b);
  }
}

char* iupMatrixGetBgColor(Ihandle* ih, int lin, int col)
{
  unsigned char r = 0, g = 0, b = 0;
  /* called from Edit only */
  if (!ih->data->bgcolor_cb || (iMatrixCallColorCB(ih, ih->data->bgcolor_cb, lin, col, &r, &g, &b) == IUP_IGNORE))
  {
    int native_parent = 0;
    if (lin == 0 || col == 0)
      native_parent = 1;
    return iMatrixGetCellAttrib(ih, IMAT_HAS_BGCOLOR, lin, col, native_parent);
  }
  else
  {
    char* buffer = iupStrGetMemory(30);
    sprintf(buffer, "%d %d %d", r, g, b);
    return buffer;
  }
}

#define IMAT_DARKER(_x)    (((_x)*9)/10)

void iupMatrixGetBgRGB(Ihandle* ih, int lin, int col, unsigned char *r, unsigned char *g, unsigned char *b)
{
  /* called from Draw only */
  if (!ih->data->bgcolor_cb || (iMatrixCallColorCB(ih, ih->data->bgcolor_cb, lin, col, r, g, b) == IUP_IGNORE))
  {
    int native_parent = 0;
    if (lin == 0 || col == 0)
      native_parent = 1;
    iupStrToRGB(iMatrixGetCellAttrib(ih, IMAT_HAS_BGCOLOR, lin, col, native_parent), r, g, b);
    if (native_parent)
    {
      int ir = IMAT_DARKER(*r), ig=IMAT_DARKER(*g), ib=IMAT_DARKER(*b); /* use a darker version of the background by 10% */
      *r = iupBYTECROP(ir);
      *g = iupBYTECROP(ig);
      *b = iupBYTECROP(ib);
    }
  }
}

char* iupMatrixGetFont(Ihandle* ih, int lin, int col)
{
  char* font = NULL;
  /* called from Draw and Edit only */
  if (ih->data->font_cb)
    font = ih->data->font_cb(ih, lin, col);
  if (!font)
    font = iMatrixGetCellAttrib(ih, IMAT_HAS_FONT, lin, col, 0);
  return font;
}

int iupMatrixGetColumnWidth(Ihandle* ih, int col, int use_value)
{
  int width = 0, pixels = 0;
  char str[100];
  char* value;

  /* can be called for invalid columns (col>numcol) */

  sprintf(str, "WIDTH%d", col);
  value = iupAttribGet(ih, str);
  if (!value)
  {
    sprintf(str, "RASTERWIDTH%d", col);
    value = iupAttribGet(ih, str);
    if (value)
      pixels = 1;
  }

  if (use_value && !value)
  {
    /* Use the titles to define the size */
    if (col == 0)
    {
      if (!ih->data->callback_mode || ih->data->use_title_size)
      {
        /* find the largest title */
        int lin, max_width = 0;
        for(lin = 0; lin < ih->data->lines.num; lin++)
        {
          char* title_value = iupMatrixCellGetValue(ih, lin, 0);
          if (title_value)
          {
            iupdrvFontGetMultiLineStringSize(ih, title_value, &width, NULL);
            if (width > max_width)
              max_width = width;
          }
        }
        width = max_width;
      }
    }
    else if (ih->data->use_title_size && (col>0 && col<ih->data->columns.num))
    {
      char* title_value = iupMatrixCellGetValue(ih, 0, col);
      if (title_value)
        iupdrvFontGetMultiLineStringSize(ih, title_value, &width, NULL);
    }
    if (width)
      return width + IMAT_PADDING_W + IMAT_FRAME_W;

    if (col != 0)
      value = iupAttribGetStr(ih, "WIDTHDEF");
  }

  if (iupStrToInt(value, &width))
  {
    if (width <= 0)
      return 0;
    else
    {
      if (pixels)
        return width + IMAT_PADDING_W + IMAT_FRAME_W;
      else
      {
        int charwidth;
        iupdrvFontGetCharSize(ih, &charwidth, NULL);
        return iupWIDTH2RASTER(width, charwidth) + IMAT_PADDING_W + IMAT_FRAME_W;
      }
    }
  }
  return 0;
}

int iupMatrixGetLineHeight(Ihandle* ih, int lin, int use_value)
{
  int height = 0, pixels = 0;
  char str[100];
  char* value;

  /* can be called for invalid lines (lin>numlin) */

  sprintf(str, "HEIGHT%d", lin);
  value = iupAttribGet(ih, str);
  if(!value)
  {
    sprintf(str, "RASTERHEIGHT%d", lin);
    value = iupAttribGet(ih, str);
    if(value)
      pixels = 1;
  }

  if (use_value && !value)
  {
    /* Use the titles to define the size */
    if (lin == 0)
    {
      if (!ih->data->callback_mode || ih->data->use_title_size)
      {
        /* find the highest title */
        int col, max_height = 0;
        for(col = 0; col < ih->data->columns.num; col++)
        {
          char* title_value = iupMatrixCellGetValue(ih, 0, col);
          if (title_value && title_value[0])
          {
            iupdrvFontGetMultiLineStringSize(ih, title_value, NULL, &height);
            if (height > max_height)
              max_height = height;
          }
        }
        height = max_height;
      }
    }
    else if (ih->data->use_title_size && (lin>0 && lin<ih->data->lines.num))
    {
      char* title_value = iupMatrixCellGetValue(ih, lin, 0);
      if (title_value && title_value[0])
        iupdrvFontGetMultiLineStringSize(ih, title_value, NULL, &height);
    }
    if (height)
      return height + IMAT_PADDING_H + IMAT_FRAME_H;

    if (lin != 0)
      value = iupAttribGetStr(ih, "HEIGHTDEF");
  }
  
  if (iupStrToInt(value, &height))
  {
    if (height <= 0)
      return 0;
    else
    {
      if (pixels)
        return height + IMAT_PADDING_H + IMAT_FRAME_H;
      else
      {
        int charheight;
        iupdrvFontGetCharSize(ih, NULL, &charheight);
        return iupHEIGHT2RASTER(height, charheight) + IMAT_PADDING_H + IMAT_FRAME_H;
      }
    }
  }
  return 0;
}

char *iupMatrixGetSize(Ihandle* ih, int index, int m, int pixels_unit)
{
  char* str;
  int size;
  ImatLinColData *p;

  if(m == IMAT_PROCESS_LIN)
    p = &(ih->data->lines);
  else
    p = &(ih->data->columns);

  if (index < 0 || index > p->num-1)
    return NULL;

  if (m == IMAT_PROCESS_LIN)
    size = iupMatrixGetLineHeight(ih, index, 1);
  else
    size = iupMatrixGetColumnWidth(ih, index, 1);

  if (size)
  {
    /* remove the decoration added in iupMatrixAuxGet */
    if (m == IMAT_PROCESS_COL)
      size -= IMAT_PADDING_W + IMAT_FRAME_W;
    else
      size -= IMAT_PADDING_H + IMAT_FRAME_H;

    if (!pixels_unit)
    {
      int charwidth, charheight;
      iupdrvFontGetCharSize(ih, &charwidth, &charheight);
      if (m == IMAT_PROCESS_COL)
        size = iupRASTER2WIDTH(size, charwidth);
      else
        size = iupRASTER2HEIGHT(size, charheight);
    }
  }

  str = iupStrGetMemory(100);
  sprintf(str, "%d", size);
  return str;
}

static int iMatrixGetOffset(int index, int *offset, ImatLinColData *p)
{
  int i;

  *offset = 0;

  /* check if the cell is not empty */
  if (!p->sizes[index])
    return 0;

  if (index < p->num_noscroll)
  {
    for(i = 0; i < index; i++)
      *offset += p->sizes[i];
  }
  else
  {
    if (index < p->first ||
        index > p->last)
      return 0;

    for(i = 0; i < p->num_noscroll; i++)
      *offset += p->sizes[i];

    /* Find the initial position */
    *offset -= p->first_offset;  /* index is always greater or equal to first */
    for(i = p->first; i < index; i++)
      *offset += p->sizes[i];
  }

  return 1;
}

int iupMatrixGetCellOffset(Ihandle* ih, int lin, int col, int *x, int *y)
{
  if (!iMatrixGetOffset(col, x, &(ih->data->columns)))
    return 0;

  if (!iMatrixGetOffset(lin, y, &(ih->data->lines)))
    return 0;

  return 1;
}

static int iMatrixGetIndexFromOffset(int pos, ImatLinColData *p)
{
  int offset = 0, i;

  if (pos < 0)
    return -1;  /* invalid */

  for(i = 0; i < p->num_noscroll; i++)  /* for all non scrollable cells */
  {
    offset += p->sizes[i];

    if (pos < offset)
      break;
  }

  if (pos >= offset)
  {
    for(i = p->first; i <= p->last; i++)  /* for all visible cells */
    {
      offset += p->sizes[i];
      if (i == p->first)
        offset -= p->first_offset;

      if (pos < offset)
        break;
    }

    if (i > p->last)
      i = -1;    /* invisible */
  }

  return i;
}

int iupMatrixGetCellFromOffset(Ihandle* ih, int x, int y, int* l, int* c)
{
  int col = iMatrixGetIndexFromOffset(x, &(ih->data->columns));
  int lin = iMatrixGetIndexFromOffset(y, &(ih->data->lines));

  if (col < 0 || lin < 0)
    return 0;

  *l = lin;
  *c = col;
  return 1;
}

static void iMatrixGetCellDim(int index, int* offset, int* size, ImatLinColData *p)
{
  int i;

  *offset = 0;
  if (index < p->num_noscroll)
  {
    for(i = 0; i < index; i++)
      *offset += p->sizes[i];
  }
  else
  {
    for(i = 0; i < p->num_noscroll; i++)
      *offset += p->sizes[i];

    for(i = p->first; i < index; i++)
    {
      *offset += p->sizes[i];
      if (i == p->first)
        *offset -= p->first_offset;  /* add only when index greater than first */
    }
  }

  *size = p->sizes[index] - 1;
  if (index == p->first)
    *size -= p->first_offset;
}

void iupMatrixGetVisibleCellDim(Ihandle* ih, int lin, int col, int* x, int* y, int* w, int* h)
{
  iMatrixGetCellDim(col, x, w, &(ih->data->columns));
  iMatrixGetCellDim(lin, y, h, &(ih->data->lines));
}

