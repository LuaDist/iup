/** \file
 * \brief IupMatrix control core
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupcontrols.h"


#include <cd.h>
#include <cdiup.h>
#include <cddbuf.h>

#include "iup_cdutil.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_controls.h"
#include "iup_register.h"
#include "iup_assert.h"

#include "iupmat_def.h"
#include "iupmat_getset.h"
#include "iupmat_scroll.h"
#include "iupmat_aux.h"
#include "iupmat_mem.h"
#include "iupmat_mouse.h"
#include "iupmat_key.h"
#include "iupmat_numlc.h"
#include "iupmat_colres.h"
#include "iupmat_mark.h"
#include "iupmat_edit.h"
#include "iupmat_draw.h"


int iupMatrixIsValid(Ihandle* ih, int check_cells)
{
  if (!ih->data->cddbuffer)
    return 0;
  if (check_cells && ((ih->data->columns.num == 0) || (ih->data->lines.num == 0)))
    return 0;
  return 1;
}

static char* iMatrixGetOriginAttrib(Ihandle* ih)
{
  char* val = iupStrGetMemory(100);
  sprintf(val, "%d:%d", ih->data->lines.first, ih->data->columns.first);
  return val;
}

static char* iMatrixGetOriginOffsetAttrib(Ihandle* ih)
{
  char* val = iupStrGetMemory(100);
  sprintf(val, "%d:%d", ih->data->lines.first_offset, ih->data->columns.first_offset);
  return val;
}

static int iMatrixSetOriginAttrib(Ihandle* ih, const char* value)
{
  int lin = IUP_INVALID_ID, col = IUP_INVALID_ID;

 /* Get the parameters. The '*' indicates that want to keep the table in
    the same line or column */
  if (iupStrToIntInt(value, &lin, &col, ':') != 2)
  {
    if (lin != IUP_INVALID_ID)
      col = ih->data->columns.first;
    else if (col != IUP_INVALID_ID)
      lin = ih->data->lines.first;
    else
      return 0;
  }

  /* Check if the cell exists */
  if (!iupMatrixCheckCellPos(ih, lin, col))
    return 0;

  /* Can not be non scrollable cell */
  if ((lin < ih->data->lines.num_noscroll) || (col < ih->data->columns.num_noscroll))
    return 0;

  ih->data->columns.first = col;
  ih->data->columns.first_offset = 0;
  ih->data->lines.first = lin;
  ih->data->lines.first_offset = 0;

  value = iupAttribGet(ih, "ORIGINOFFSET");
  if (value)
  {
    int lin_offset, col_offset;
    if (iupStrToIntInt(value, &lin_offset, &col_offset, ':') == 2)
    {
      if (col_offset < ih->data->columns.sizes[col])
        ih->data->columns.first_offset = col_offset;
      if (lin_offset < ih->data->lines.sizes[lin])
        ih->data->lines.first_offset = lin_offset;
    }
  }

  /* when "first" is changed must update scroll pos */
  iupMatrixAuxUpdateScrollPos(ih, IMAT_PROCESS_COL);
  iupMatrixAuxUpdateScrollPos(ih, IMAT_PROCESS_LIN);

  iupMatrixDraw(ih, 1);
  return 0;
}

static int iMatrixSetShowAttrib(Ihandle* ih, const char* value)
{
  int lin = IUP_INVALID_ID, col = IUP_INVALID_ID;

 /* Get the parameters. The '*' indicates that want to keep the table in
    the same line or column */
  if (iupStrToIntInt(value, &lin, &col, ':') != 2)
  {
    if (lin != IUP_INVALID_ID)
      col = ih->data->columns.first;
    else if (col != IUP_INVALID_ID)
      lin = ih->data->lines.first;
    else
      return 0;
  }

  /* Check if the cell exists */
  if (!iupMatrixCheckCellPos(ih, lin, col))
    return 0;

  /* Can not be a title cell */
  if ((lin < 0) || (col < 0))
    return 0;

  if (!iupMatrixAuxIsCellStartVisible(ih, lin, col))
    iupMatrixScrollToVisible(ih, lin, col);

  return 0;
}

static int iMatrixSetFocusCellAttrib(Ihandle* ih, const char* value)
{
  int lin = IUP_INVALID_ID, col = IUP_INVALID_ID;
  if (iupStrToIntInt(value, &lin, &col, ':') == 2)
  {
    if (!iupMatrixCheckCellPos(ih, lin, col))
      return 0;

    if (lin == 0 || col == 0) /* title can NOT have the focus */
      return 0;
    if (lin >= ih->data->lines.num || col >= ih->data->columns.num)
      return 0;

    ih->data->lines.focus_cell = lin;
    ih->data->columns.focus_cell = col;

    if (ih->data->cddbuffer)
      iupMatrixDrawUpdate(ih);
  }

  return 0;
}

static char* iMatrixGetFocusCellAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(100);
  sprintf(str, "%d:%d", ih->data->lines.focus_cell, ih->data->columns.focus_cell);
  return str;
}

static int iMatrixSetUseTitleSizeAttrib(Ihandle* ih, const char* value)
{
  /* can be set only before map */
  if (ih->handle)
    return 0;

  if (iupStrBoolean(value))
    ih->data->use_title_size = 1;
  else 
    ih->data->use_title_size = 0;

  return 0;
}

static char* iMatrixGetUseTitleSizeAttrib(Ihandle* ih)
{
  if (ih->data->use_title_size)
    return "YES";
  else
    return "NO";
}

static int iMatrixSetLimitExpandAttrib(Ihandle* ih, const char* value)
{
  /* can be set only before map */
  if (ih->handle)
    return 0;

  if (iupStrBoolean(value))
    ih->data->limit_expand = 1;
  else 
    ih->data->limit_expand = 0;

  return 0;
}

static char* iMatrixGetLimitExpandAttrib(Ihandle* ih)
{
  if (ih->data->limit_expand)
    return "YES";
  else
    return "NO";
}

static int iMatrixSetHiddenTextMarksAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->hidden_text_marks = 1;
  else 
    ih->data->hidden_text_marks = 0;
  return 0;
}

static char* iMatrixGetHiddenTextMarksAttrib(Ihandle* ih)
{
  if (ih->data->hidden_text_marks)
    return "YES";
  else
    return "NO";
}

static int iMatrixSetValueAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->columns.num <= 1 || ih->data->lines.num <= 1)
    return 0;
  if (IupGetInt(ih->data->datah, "VISIBLE"))
    IupStoreAttribute(ih->data->datah, "VALUE", value);
  else 
    iupMatrixCellSetValue(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell, value);
  return 0;
}

static char* iMatrixGetValueAttrib(Ihandle* ih)
{
  if (ih->data->columns.num <= 1 || ih->data->lines.num <= 1)
    return NULL;
  if (IupGetInt(ih->data->datah, "VISIBLE"))
    return iupMatrixEditGetValue(ih);
  else 
    return iupMatrixCellGetValue(ih, ih->data->lines.focus_cell, ih->data->columns.focus_cell);
}

static int iMatrixSetCaretAttrib(Ihandle* ih, const char* value)
{
  IupStoreAttribute(ih->data->texth, "CARET", value);
  return 1;
}

static char* iMatrixGetCaretAttrib(Ihandle* ih)
{
  return IupGetAttribute(ih->data->texth, "CARET");
}

static int iMatrixSetSelectionAttrib(Ihandle* ih, const char* value)
{
  IupStoreAttribute(ih->data->texth, "SELECTION", value);
  return 1;
}

static char* iMatrixGetSelectionAttrib(Ihandle* ih)
{
  return IupGetAttribute(ih->data->texth, "SELECTION");
}

static int iMatrixSetMultilineAttrib(Ihandle* ih, const char* value)
{
  IupStoreAttribute(ih->data->texth, "MULTILINE", value);
  if (iupStrBoolean(value))
    IupSetAttribute(ih->data->texth, "SCROLLBAR", "NO");
  return 1;
}

static char* iMatrixGetMultilineAttrib(Ihandle* ih)
{
  return IupGetAttribute(ih->data->texth, "MULTILINE");
}

static char* iMatrixGetCountAttrib(Ihandle* ih)
{
  char* num = iupStrGetMemory(100);
  sprintf(num, "%d", ih->data->lines.num>ih->data->columns.num? ih->data->lines.num: ih->data->columns.num);
  return num;
}

static char* iMatrixGetNumLinAttrib(Ihandle* ih)
{
  char* num = iupStrGetMemory(100);
  sprintf(num, "%d", ih->data->lines.num-1);  /* the attribute does not include the title */
  return num;
}

static char* iMatrixGetNumColAttrib(Ihandle* ih)
{
  char* num = iupStrGetMemory(100);
  sprintf(num, "%d", ih->data->columns.num-1);  /* the attribute does not include the title */
  return num;
}

static int iMatrixSetMarkModeAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "CELL"))
    ih->data->mark_mode = IMAT_MARK_CELL;
  else if (iupStrEqualNoCase(value, "LIN"))
    ih->data->mark_mode = IMAT_MARK_LIN;
  else if (iupStrEqualNoCase(value, "COL"))
    ih->data->mark_mode = IMAT_MARK_COL;
  else if(iupStrEqualNoCase(value, "LINCOL"))
    ih->data->mark_mode = IMAT_MARK_LINCOL;
  else 
    ih->data->mark_mode = IMAT_MARK_NO;

  if (ih->handle)
  {
    iupMatrixMarkClearAll(ih, 0);
    iupMatrixDraw(ih, 1);
  }
  return 0;
}

static char* iMatrixGetMarkModeAttrib(Ihandle* ih)
{
  char* mark2str[] = {"NO", "LIN", "COL", "LINCOL", "CELL"};
  return mark2str[ih->data->mark_mode];
}

static int iMatrixSetMarkAreaAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NOT_CONTINUOUS"))
    ih->data->mark_continuous = 0;
  else 
    ih->data->mark_continuous = 1;

  if (ih->handle)
  {
    iupMatrixMarkClearAll(ih, 0);
    iupMatrixDraw(ih, 1);
  }
  return 0;
}

static char* iMatrixGetMarkAreaAttrib(Ihandle* ih)
{
  if (ih->data->mark_continuous)
    return "CONTINUOUS";
  else
    return "NOT_CONTINUOUS";
}

static int iMatrixSetMarkMultipleAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->mark_multiple = 1;
  else 
    ih->data->mark_multiple = 0;

  if (ih->handle)
  {
    iupMatrixMarkClearAll(ih, 0);
    iupMatrixDraw(ih, 1);
  }
  return 0;
}

static char* iMatrixGetMarkMultipleAttrib(Ihandle* ih)
{
  if (ih->data->mark_multiple)
    return "YES";
  else
    return "NO";
}

static int iMatrixSetEditModeAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    iupMatrixEditShow(ih);
  else
  {
    iupMatrixEditHide(ih);
    iupMatrixDrawUpdate(ih);
  }
  return 1;
}

static char* iMatrixGetEditModeAttrib(Ihandle* ih)
{
  if (iupMatrixEditIsVisible(ih))
    return "YES";
  else
    return "NO";
}

static int iMatrixSetEditNextAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "NONE"))
    ih->data->editnext = IMAT_EDITNEXT_NONE;
  else if (iupStrEqualNoCase(value, "COL"))
    ih->data->editnext = IMAT_EDITNEXT_COL;
  else if (iupStrEqualNoCase(value, "COLCR"))
    ih->data->editnext = IMAT_EDITNEXT_COLCR;
  else if (iupStrEqualNoCase(value, "LINCR"))
    ih->data->editnext = IMAT_EDITNEXT_LINCR;
  else
    ih->data->editnext = IMAT_EDITNEXT_LIN;
  return 0;
}

static char* iMatrixGetEditNextAttrib(Ihandle* ih)
{
  switch (ih->data->editnext)
  {
  case IMAT_EDITNEXT_NONE: return "NONE";
  case IMAT_EDITNEXT_COL: return "COL";
  case IMAT_EDITNEXT_LINCR: return "LINCR";
  case IMAT_EDITNEXT_COLCR: return "COLCR";
  default: return "LIN";
  }
}

static int iMatrixSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);
  iupMatrixDraw(ih, 1);
  return 0;
}

static int iMatrixHasColWidth(Ihandle* ih, int col)
{
  char str[100];
  char* value;

  sprintf(str, "WIDTH%d", col);
  value = iupAttribGet(ih, str);
  if (!value)
  {
    sprintf(str, "RASTERWIDTH%d", col);
    value = iupAttribGet(ih, str);
  }
  return (value!=NULL);
}

static int iMatrixHasLineHeight(Ihandle* ih, int lin)
{
  char str[100];
  char* value;

  sprintf(str, "HEIGHT%d", lin);
  value = iupAttribGet(ih, str);
  if (!value)
  {
    sprintf(str, "RASTERHEIGHT%d", lin);
    value = iupAttribGet(ih, str);
  }
  return (value!=NULL);
}

static void iMatrixFitLines(Ihandle* ih, int height)
{
  /* expand each line height to fit the matrix height */
  int line_height, lin, empty, has_line_height,
      *empty_lines, empty_num=0, visible_num, empty_lin_visible = 0;

  visible_num = iupAttribGetInt(ih, "NUMLIN_VISIBLE")+1;  /* include the title line */

  empty_lines = malloc(ih->data->lines.num*sizeof(int));

  /* ignore the lines that already have HEIGHT or RASTERHEIGHT */
  for(lin = 0; lin < ih->data->lines.num; lin++)
  {
    has_line_height = iMatrixHasLineHeight(ih, lin);
    empty = 1;

    /* when lin==0 the line exists if size is defined, 
       but also exists if some title text is defined in non callback mode. */
    line_height = iupMatrixGetLineHeight(ih, lin, lin==0? 1: 0);

    /* the line does not exists */
    if (lin==0 && !has_line_height && !line_height)
      empty = 0;  /* don't resize this line */

    /* if the height is defined to be 0, it can not be used */
    if (has_line_height && !line_height)
      empty = 0;  /* don't resize this line */

    if (line_height)
    {
      if (lin < visible_num)
        height -= line_height;
    }
    else if (empty)
    {
      if (lin < visible_num)
        empty_lin_visible++;

      empty_lines[empty_num] = lin;
      empty_num++;
    }
  }

  if (empty_num && empty_lin_visible)
  {
    int i;
    char str[100];

    line_height = height/empty_lin_visible - (IMAT_PADDING_H + IMAT_FRAME_H);

    for(i = 0; i < empty_num; i++)
    {
      sprintf(str, "RASTERHEIGHT%d", empty_lines[i]);
      iupAttribSetInt(ih, str, line_height);
    }
  }

  free(empty_lines);
}

static void iMatrixFitColumns(Ihandle* ih, int width)
{
  /* expand each column width to fit the matrix width */
  int column_width, col, empty, has_col_width,
      *empty_columns, empty_num=0, visible_num, empty_col_visible = 0;

  visible_num = iupAttribGetInt(ih, "NUMCOL_VISIBLE")+1;  /* include the title column */

  empty_columns = malloc(ih->data->columns.num*sizeof(int));

  /* ignore the columns that already have WIDTH or RASTERWIDTH */
  for(col = 0; col < ih->data->columns.num; col++)
  {
    has_col_width = iMatrixHasColWidth(ih, col);
    empty = 1;

    /* when col==0 the col exists if size is defined, 
       but also exists if some title text is defined in non callback mode. */
    column_width = iupMatrixGetColumnWidth(ih, col, col==0? 1: 0);

    /* the col does not exists */
    if (col==0 && !has_col_width && !column_width)
      empty = 0;  /* don't resize this col */

    /* if the width is defined to be 0, it can not be used */
    if (has_col_width && !column_width)
      empty = 0;  /* don't resize this col */

    if (column_width)
    {
      if (col < visible_num)
        width -= column_width;
    }
    else if (empty)
    {
      if (col < visible_num)
        empty_col_visible++;

      empty_columns[empty_num] = col;
      empty_num++;
    }
  }

  if (empty_num && empty_col_visible)
  {
    int i;
    char str[100];

    column_width = width/empty_col_visible - (IMAT_PADDING_W + IMAT_FRAME_W);

    for(i = 0; i < empty_num; i++)
    {
      sprintf(str, "RASTERWIDTH%d", empty_columns[i]);
      iupAttribSetInt(ih, str, column_width);
    }
  }

  free(empty_columns);
}

static int iMatrixSetFitToSizeAttrib(Ihandle* ih, const char* value)
{
  int w, h;
  int sb_w = 0, sb_h = 0;

  if (!ih->handle)
    ih->data->canvas.sb = iupBaseGetScrollbar(ih);

  /* add scrollbar */
  if (ih->data->canvas.sb)
  {
    int sb_size = iupdrvGetScrollbarSize();
    if (ih->data->canvas.sb & IUP_SB_HORIZ)
      sb_h += sb_size;  /* sb horizontal affects vertical size */
    if (ih->data->canvas.sb & IUP_SB_VERT)
      sb_w += sb_size;  /* sb vertical affects horizontal size */
  }

  IupGetIntInt(ih, "RASTERSIZE", &w, &h);

  if (iupStrEqualNoCase(value, "LINES"))
    iMatrixFitLines(ih, h-sb_h);
  else if (iupStrEqualNoCase(value, "COLUMNS"))
    iMatrixFitColumns(ih, w-sb_w);
  else
  {
    iMatrixFitLines(ih, h-sb_h);
    iMatrixFitColumns(ih, w-sb_w);
  }

  ih->data->need_calcsize = 1;
  if (ih->handle)
    iupMatrixDraw(ih, 1);
  return 0;
}

static void iMatrixFitColText(Ihandle* ih, int col)
{
  /* find the largest cel in the col */
  int lin, max_width = 0, max;
  char str[100];

  for(lin = 0; lin < ih->data->lines.num; lin++)
  {
    char* title_value = iupMatrixCellGetValue(ih, lin, col);
    if (title_value && title_value[0])
    {
      int w;
      iupdrvFontGetMultiLineStringSize(ih, title_value, &w, NULL);
      if (w > max_width)
        max_width = w;
    }
  }

  sprintf(str, "FITMAXWIDTH%d", col);
  max = iupAttribGetInt(ih, str);
  if (max && max > max_width)
    max_width = max;

  sprintf(str, "RASTERWIDTH%d", col);
  iupAttribSetInt(ih, str, max_width);
}

static void iMatrixFitLineText(Ihandle* ih, int line)
{
  /* find the highest cel in the line */
  int col, max_height = 0, max;
  char str[100];

  for(col = 0; col < ih->data->columns.num; col++)
  {
    char* title_value = iupMatrixCellGetValue(ih, line, col);
    if (title_value && title_value[0])
    {
      int h;
      iupdrvFontGetMultiLineStringSize(ih, title_value, NULL, &h);
      if (h > max_height)
        max_height = h;
    }
  }

  sprintf(str, "FITMAXHEIGHT%d", line);
  max = iupAttribGetInt(ih, str);
  if (max && max > max_height)
    max_height = max;

  sprintf(str, "RASTERHEIGHT%d", line);
  iupAttribSetInt(ih, str, max_height);
}

static int iMatrixSetFitToTextAttrib(Ihandle* ih, const char* value)
{
  if (!value || value[0]==0)
    return 0;

  if (value[0] == 'C')
  {
    int col;
    if (iupStrToInt(value+1, &col))
      iMatrixFitColText(ih, col);
  }
  else if (value[0] == 'L')
  {
    int line;
    if (iupStrToInt(value+1, &line))
      iMatrixFitLineText(ih, line);
  }

  ih->data->need_calcsize = 1;
  if (ih->handle)
    iupMatrixDraw(ih, 1);
  return 0;
}

static int iMatrixSetNumColNoScrollAttrib(Ihandle* ih, const char* value)
{
  int num = 0;
  if (iupStrToInt(value, &num))
  {
    if (num < 0) num = 0;

    num++; /* add room for title column */

    ih->data->columns.num_noscroll = num;
    if (ih->data->columns.num_noscroll > ih->data->columns.num)
      ih->data->columns.num_noscroll = ih->data->columns.num;
    if (ih->data->columns.first < ih->data->columns.num_noscroll)
    {
      ih->data->columns.first = ih->data->columns.num_noscroll;
      ih->data->columns.first_offset = 0;

      /* when "first" is changed must update scroll pos */
      iupMatrixAuxUpdateScrollPos(ih, IMAT_PROCESS_COL);
    }
    ih->data->need_calcsize = 1;

    if (ih->handle)
      iupMatrixDraw(ih, 1);
  }

  return 0;
}

static int iMatrixSetNumLinNoScrollAttrib(Ihandle* ih, const char* value)
{
  int num = 0;
  if (iupStrToInt(value, &num))
  {
    if (num < 0) num = 0;

    num++; /* add room for title line */

    ih->data->lines.num_noscroll = num;
    if (ih->data->lines.num_noscroll > ih->data->lines.num)
      ih->data->lines.num_noscroll = ih->data->lines.num;
    if (ih->data->lines.first < ih->data->lines.num_noscroll)
    {
      ih->data->lines.first = ih->data->lines.num_noscroll;
      ih->data->lines.first_offset = 0;

      /* when "first" is changed must update scroll pos */
      iupMatrixAuxUpdateScrollPos(ih, IMAT_PROCESS_LIN);
    }
    ih->data->need_calcsize = 1;

    if (ih->handle)
      iupMatrixDraw(ih, 1);
  }

  return 0;
}

static char* iMatrixGetNumColNoScrollAttrib(Ihandle* ih)
{
  char* num = iupStrGetMemory(100);
  sprintf(num, "%d", ih->data->columns.num_noscroll-1);  /* the attribute does not include the title */
  return num;
}

static char* iMatrixGetNumLinNoScrollAttrib(Ihandle* ih)
{
  char* num = iupStrGetMemory(100);
  sprintf(num, "%d", ih->data->lines.num_noscroll-1);  /* the attribute does not include the title */
  return num;
}

static int iMatrixSetSizeAttrib(Ihandle* ih, int pos, const char* value)
{
  (void)pos;
  (void)value;
  ih->data->need_calcsize = 1;
  IupUpdate(ih);  /* post a redraw */
  return 1;
}

static char* iMatrixGetWidthAttrib(Ihandle* ih, int col)
{
  return iupMatrixGetSize(ih, col, IMAT_PROCESS_COL, 0);
}

static char* iMatrixGetHeightAttrib(Ihandle* ih, int lin)
{
  return iupMatrixGetSize(ih, lin, IMAT_PROCESS_LIN, 0);
}

static char* iMatrixGetRasterWidthAttrib(Ihandle* ih, int col)
{
  return iupMatrixGetSize(ih, col, IMAT_PROCESS_COL, 1);
}

static char* iMatrixGetRasterHeightAttrib(Ihandle* ih, int lin)
{
  return iupMatrixGetSize(ih, lin, IMAT_PROCESS_LIN, 1);
}

static char* iMatrixGetAlignmentAttrib(Ihandle* ih, int col)
{
  char* align;
  char str[100];
  sprintf(str, "ALIGNMENT%d", col);
  align = iupAttribGet(ih, str);
  if (!align)
  {
    align = iupAttribGet(ih, "ALIGNMENT");
    if (align)
      return align;
    else
    {
      if (col == 0)
        return "ALEFT";
      else
        return "ACENTER";
    }
  }
    
  return NULL;
}

static int iMatrixSetIdValueAttrib(Ihandle* ih, int lin, int col, const char* value)
{
  if (iupMatrixCheckCellPos(ih, lin, col))
    iupMatrixCellSetValue(ih, lin, col, value);
  return 0;
}

static char* iMatrixGetIdValueAttrib(Ihandle* ih, int lin, int col)
{
  if (iupMatrixCheckCellPos(ih, lin, col))
    return iupMatrixCellGetValue(ih, lin, col);
  return NULL;
}

static char* iMatrixGetCellOffsetAttrib(Ihandle* ih, int lin, int col)
{
  if (iupMatrixCheckCellPos(ih, lin, col))
  {
    int x, y;
    if (iupMatrixGetCellOffset(ih, lin, col, &x, &y))
    {
      char* buffer = iupStrGetMemory(50);
      sprintf(buffer, "%dx%d", x, y);
      return buffer;
    }
  }
  return NULL;
}

static char* iMatrixGetCellSizeAttrib(Ihandle* ih, int lin, int col)
{
  if (iupMatrixCheckCellPos(ih, lin, col))
  {
    char* buffer = iupStrGetMemory(50);
    sprintf(buffer, "%dx%d", ih->data->columns.sizes[col], ih->data->lines.sizes[lin]);
    return buffer;
  }
  return NULL;
}

static int iMatrixSetNeedRedraw(Ihandle* ih)
{
  ih->data->need_redraw = 1;
  return 1;
}

static void iMatrixClearAttrib(Ihandle* ih, unsigned char *flags, int lin, int col)
{
  char name[50], id[40];
  int is_marked = (*flags) & IMAT_IS_MARKED;

  if (lin == IUP_INVALID_ID)
    sprintf(id, "*:%d", col);
  else if (col == IUP_INVALID_ID)
    sprintf(id, "%d:*", lin);
  else
    sprintf(id, "%d:%d", lin, col);

  if ((*flags) & IMAT_HAS_FONT)
  {
    sprintf(name, "FONT%s", id);
    iupAttribSetStr(ih, name, NULL);
  }

  if ((*flags) & IMAT_HAS_FGCOLOR)
  {
    sprintf(name, "FGCOLOR%s", id);
    iupAttribSetStr(ih, name, NULL);
  }

  if ((*flags) & IMAT_HAS_BGCOLOR)
  {
    sprintf(name, "BGCOLOR%s", id);
    iupAttribSetStr(ih, name, NULL);
  }

  if ((*flags) & IMAT_HAS_FRAMEHORIZCOLOR)
  {
    sprintf(name, "FRAMEHORIZCOLOR%s", id);
    iupAttribSetStr(ih, name, NULL);
  }

  if ((*flags) & IMAT_HAS_FRAMEVERTCOLOR)
  {
    sprintf(name, "FRAMEHORIZCOLOR%s", id);
    iupAttribSetStr(ih, name, NULL);
  }

  if (lin == IUP_INVALID_ID)
  {
    sprintf(name, "ALIGNMENT%d", col);
    iupAttribSetStr(ih, name, NULL);

    sprintf(name, "SORTSIGN%d", col);
    iupAttribSetStr(ih, name, NULL);
  }

  *flags = 0;
  if (is_marked)
    *flags = IMAT_IS_MARKED;
}

static int iMatrixSetClearAttribAttrib(Ihandle* ih, int lin, int col, const char* value)
{
  if (ih->data->callback_mode)
    return 0;

  if (lin < 0 && col < 0)
  {
    if (iupStrEqualNoCase(value, "ALL"))
    {
      /* all cells attributes */
      for (lin=0; lin<ih->data->lines.num; lin++)
      {
        for (col=0; col<ih->data->columns.num; col++)
          iMatrixClearAttrib(ih, &(ih->data->cells[lin][col].flags), lin, col);
      }

      /* all line attributes */
      for (lin=0; lin<ih->data->lines.num; lin++)
        iMatrixClearAttrib(ih, &(ih->data->lines.flags[lin]), lin, IUP_INVALID_ID);

      /* all column attributes */
      for (col=0; col<ih->data->columns.num; col++)
        iMatrixClearAttrib(ih, &(ih->data->columns.flags[col]), IUP_INVALID_ID, col);
    }
    else if (iupStrEqualNoCase(value, "CONTENTS"))
    {
      for (lin=1; lin<ih->data->lines.num; lin++)
      {
        for (col=1; col<ih->data->columns.num; col++)
          iMatrixClearAttrib(ih, &(ih->data->cells[lin][col].flags), lin, col);
      }
    }
  }
  else
  {
    if (lin==IUP_INVALID_ID)
    {
      int lin1 = 0, lin2 = ih->data->lines.num-1;
      if (!iupStrEqualNoCase(value, "ALL"))
        iupStrToIntInt(value, &lin1, &lin2, '-');

      if (!iupMATRIX_CHECK_COL(ih, col)  ||
          !iupMATRIX_CHECK_LIN(ih, lin1) ||
          !iupMATRIX_CHECK_LIN(ih, lin2))
        return 0;

      if (lin1==0 && lin2==ih->data->lines.num-1)
        iMatrixClearAttrib(ih, &(ih->data->columns.flags[col]), IUP_INVALID_ID, col);

      for (lin=lin1; lin<=lin2; lin++)
        iMatrixClearAttrib(ih, &(ih->data->cells[lin][col].flags), lin, col);
    }
    else if (col==IUP_INVALID_ID)
    {
      int col1 = 0, col2 = ih->data->columns.num-1;
      if (!iupStrEqualNoCase(value, "ALL"))
        iupStrToIntInt(value, &col1, &col2, '-');

      if (!iupMATRIX_CHECK_LIN(ih, lin)  ||
          !iupMATRIX_CHECK_COL(ih, col1) ||
          !iupMATRIX_CHECK_COL(ih, col2))
        return 0;

      if (col1==0 && col2==ih->data->columns.num-1)
        iMatrixClearAttrib(ih, &(ih->data->lines.flags[lin]), lin, IUP_INVALID_ID);

      for (col=col1; col<=col2; col++)
        iMatrixClearAttrib(ih, &(ih->data->cells[lin][col].flags), lin, col);
    }
    else
    {
      int lin1 = lin, lin2 = ih->data->lines.num-1;
      int col1 = col, col2 = ih->data->columns.num-1;
      iupStrToIntInt(value, &lin2, &col2, ':');

      if (!iupMATRIX_CHECK_LIN(ih, lin1) ||
          !iupMATRIX_CHECK_LIN(ih, lin2) ||
          !iupMATRIX_CHECK_COL(ih, col1) ||
          !iupMATRIX_CHECK_COL(ih, col2))
        return 0;

      if (lin1==0 && lin2==ih->data->lines.num-1)
      {
        for (col=0; col<ih->data->columns.num; col++)
          iMatrixClearAttrib(ih, &(ih->data->columns.flags[col]), IUP_INVALID_ID, col);
      }

      if (col1==0 && col2==ih->data->columns.num-1)
      {
        for (lin=0; lin<ih->data->lines.num; lin++)
          iMatrixClearAttrib(ih, &(ih->data->lines.flags[lin]), lin, IUP_INVALID_ID);
      }

      for (lin=lin1; lin<=lin2; lin++)
      {
        for (col=col1; col<=col2; col++)
          iMatrixClearAttrib(ih, &(ih->data->cells[lin][col].flags), lin, col);
      }
    }
  }

  if (ih->handle)
    iupMatrixDraw(ih, 1);

  return 0;
}

static int iMatrixSetClearValueAttrib(Ihandle* ih, int lin, int col, const char* value)
{
  if (ih->data->callback_mode)
    return 0;

  if (lin < 0 && col < 0)
  {
    if (iupStrEqualNoCase(value, "ALL"))
    {
      for (lin=0; lin<ih->data->lines.num; lin++)
      {
        for (col=0; col<ih->data->columns.num; col++)
        {
          if (ih->data->cells[lin][col].value)
            free(ih->data->cells[lin][col].value);
          ih->data->cells[lin][col].value = NULL;
        }
      }
    }
    else if (iupStrEqualNoCase(value, "CONTENTS"))
    {
      for (lin=1; lin<ih->data->lines.num; lin++)
      {
        for (col=1; col<ih->data->columns.num; col++)
        {
          if (ih->data->cells[lin][col].value)
            free(ih->data->cells[lin][col].value);
          ih->data->cells[lin][col].value = NULL;
        }
      }
    }
  }
  else
  {
    if (lin==IUP_INVALID_ID)
    {
      int lin1 = 0, lin2 = ih->data->lines.num-1;
      iupStrToIntInt(value, &lin1, &lin2, '-');

      if (!iupMATRIX_CHECK_COL(ih, col)  ||
          !iupMATRIX_CHECK_LIN(ih, lin1) ||
          !iupMATRIX_CHECK_LIN(ih, lin2))
        return 0;

      for (lin=lin1; lin<=lin2; lin++)
      {
        if (ih->data->cells[lin][col].value)
          free(ih->data->cells[lin][col].value);
        ih->data->cells[lin][col].value = NULL;
      }
    }
    else if (col==IUP_INVALID_ID)
    {
      int col1 = 0, col2 = ih->data->columns.num-1;
      iupStrToIntInt(value, &col1, &col2, '-');

      if (!iupMATRIX_CHECK_LIN(ih, lin)  ||
          !iupMATRIX_CHECK_COL(ih, col1) ||
          !iupMATRIX_CHECK_COL(ih, col2))
        return 0;

      for (col=col1; col<=col2; col++)
      {
        if (ih->data->cells[lin][col].value)
          free(ih->data->cells[lin][col].value);
        ih->data->cells[lin][col].value = NULL;
      }
    }
    else
    {
      int lin1 = lin, lin2 = ih->data->lines.num-1;
      int col1 = col, col2 = ih->data->columns.num-1;
      iupStrToIntInt(value, &lin2, &col2, ':');

      if (!iupMATRIX_CHECK_LIN(ih, lin1) ||
          !iupMATRIX_CHECK_LIN(ih, lin2) ||
          !iupMATRIX_CHECK_COL(ih, col1) ||
          !iupMATRIX_CHECK_COL(ih, col2))
        return 0;

      for (lin=lin1; lin<=lin2; lin++)
      {
        for (col=col1; col<=col2; col++)
        {
          if (ih->data->cells[lin][col].value)
            free(ih->data->cells[lin][col].value);
          ih->data->cells[lin][col].value = NULL;
        }
      }
    }
  }

  if (ih->handle)
    iupMatrixDraw(ih, 1);

  return 0;
}

static int iMatrixSetFlagsAttrib(Ihandle* ih, int lin, int col, const char* value, unsigned char attr)
{
  if (lin >= 0 || col >= 0)
  {
    iupMatrixCellSetFlag(ih, lin, col, attr, value!=NULL);
    ih->data->need_redraw = 1;
  }
  return 1;
}

static int iMatrixSetBgColorAttrib(Ihandle* ih, int lin, int col, const char* value)
{
  return iMatrixSetFlagsAttrib(ih, lin, col, value, IMAT_HAS_BGCOLOR);
}

static int iMatrixSetFgColorAttrib(Ihandle* ih, int lin, int col, const char* value)
{
  return iMatrixSetFlagsAttrib(ih, lin, col, value, IMAT_HAS_FGCOLOR);
}

static int iMatrixSetFontAttrib(Ihandle* ih, int lin, int col, const char* value)
{
  return iMatrixSetFlagsAttrib(ih, lin, col, value, IMAT_HAS_FONT);
}

static int iMatrixSetFrameHorizColorAttrib(Ihandle* ih, int lin, int col, const char* value)
{
  ih->data->checkframecolor = value!=NULL;
  return iMatrixSetFlagsAttrib(ih, lin, col, value, IMAT_HAS_FRAMEHORIZCOLOR);
}

static int iMatrixSetFrameVertColorAttrib(Ihandle* ih, int lin, int col, const char* value)
{
  ih->data->checkframecolor = value!=NULL;
  return iMatrixSetFlagsAttrib(ih, lin, col, value, IMAT_HAS_FRAMEVERTCOLOR);
}

static char* iMatrixGetFontAttrib(Ihandle* ih, int lin, int col)
{
  if (lin==IUP_INVALID_ID && col==IUP_INVALID_ID)  /* empty id */
    return iupGetFontAttrib(ih);
  return NULL;
}

static char* iMatrixGetBgColorAttrib(Ihandle* ih, int lin, int col)
{
  if (lin==IUP_INVALID_ID && col==IUP_INVALID_ID) /* empty id */
  {
    /* check the hash table */
    char *color = iupAttribGet(ih, "BGCOLOR");

    /* If not defined return the default for normal cells */
    if (!color)
      color = IupGetGlobal("TXTBGCOLOR");

    return color;
  }
  return NULL;
}

static char* iMatrixGetCellBgColorAttrib(Ihandle* ih, int lin, int col)
{
  if (iupMatrixCheckCellPos(ih, lin, col))
  {
    char* buffer;
    unsigned char r = 255, g = 255, b = 255;
    int active = iupdrvIsActive(ih);
    char* mark = iupMatrixGetMarkAttrib(ih, lin, col);

    iupMatrixGetBgRGB(ih, lin, col, &r, &g, &b);
    
    if (mark && mark[0]=='1')
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

    buffer = iupStrGetMemory(30);
    sprintf(buffer, "%d %d %d", r, g, b);
    return buffer;
  }
  else
    return NULL;
}

static char* iMatrixGetCellFgColorAttrib(Ihandle* ih, int lin, int col)
{
  if (iupMatrixCheckCellPos(ih, lin, col))
  {
    char* buffer;
    unsigned char r = 255, g = 255, b = 255;
    char* mark = iupMatrixGetMarkAttrib(ih, lin, col);

    iupMatrixGetFgRGB(ih, lin, col, &r, &g, &b);
    
    if (mark && mark[0]=='1')
    {
      r = IMAT_ATENUATION(r);
      g = IMAT_ATENUATION(g);
      b = IMAT_ATENUATION(b);
    }

    buffer = iupStrGetMemory(30);
    sprintf(buffer, "%d %d %d", r, g, b);
    return buffer;
  }
  else
    return NULL;
}

static int iMatrixConvertXYToPos(Ihandle* ih, int x, int y)
{
  int lin, col;
  if (iupMatrixGetCellFromOffset(ih, x, y, &lin, &col))
    return lin*(ih->data->columns.num-1) + col;
  return -1;
}

static char* iMatrixGetNumColVisibleAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(50);
  sprintf(buffer, "%d", ih->data->columns.last - ih->data->columns.first);
  return buffer;
}

static char* iMatrixGetNumLinVisibleAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(50);
  sprintf(buffer, "%d", ih->data->lines.last - ih->data->lines.first);
  return buffer;
}

static char* iMatrixGetMaskDataAttrib(Ihandle* ih)
{
  /* Used only by the OLD iupmask API */
  if (IupGetInt(ih->data->datah, "VISIBLE"))
    return IupGetAttribute(ih->data->datah,"OLD_MASK_DATA");
  else
    return NULL;
}

static int iMatrixSetVisibleAttrib(Ihandle* ih, const char* value)
{
  if (!iupStrBoolean(value))
  {
    if (iupMatrixEditIsVisible(ih))
      iupMatrixEditForceHidden(ih);
  }

  return iupBaseSetVisibleAttrib(ih, value);
}

/*****************************************************************************/
/*   Callbacks registered to the Canvas                                      */
/*****************************************************************************/


static int iMatrixFocus_CB(Ihandle* ih, int focus)
{
  int rc = IUP_DEFAULT;

  if (!iupMatrixIsValid(ih, 1))
    return IUP_DEFAULT;

  if (IupGetGlobal("MOTIFVERSION"))
  {
    if (iupAttribGet(ih, "_IUPMAT_DROPDOWN") ||  /* from iMatrixEditDropDown_CB, in Motif */
        iupAttribGet(ih, "_IUPMAT_DOUBLECLICK"))  /* from iMatrixMouseLeftPress, in Motif */
      return IUP_DEFAULT;
  }

  ih->data->has_focus = focus;
  iupMatrixDrawUpdate(ih);

  if (focus)
    iupMatrixAuxCallEnterCellCb(ih);
  else
    iupMatrixAuxCallLeaveCellCb(ih);

  return rc;
}

static int iMatrixResize_CB(Ihandle* ih)
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

  iupMatrixEditForceHidden(ih);

  ih->data->need_calcsize = 1;

  return IUP_DEFAULT;
}

static int iMatrixRedraw_CB(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  if (ih->data->callback_mode ||  /* in callback mode the values are not changed by attributes, so we can NOT wait for a REDRAW */
      ih->data->need_redraw ||  /* if some of the attributes that do not automatically redraw were set */
      ih->data->need_calcsize)   /* if something changed the matrix size */
    iupMatrixDraw(ih, 0);

  iupMatrixDrawUpdate(ih);

  return IUP_DEFAULT;
}


/***************************************************************************/


static int iMatrixCreateMethod(Ihandle* ih, void **params)
{
  if (params && params[0])
  {
    char* action_cb = (char*)params[0];
    iupAttribStoreStr(ih, "ACTION_CB", action_cb);
  }

  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  iupAttribSetStr(ih, "SCROLLBAR", "YES");
  iupAttribSetStr(ih, "BORDER", "NO");
  iupAttribSetStr(ih, "CURSOR", "IupMatrixCrossCursor");

  /* IupCanvas callbacks */
  IupSetCallback(ih, "ACTION",      (Icallback)iMatrixRedraw_CB);
  IupSetCallback(ih, "RESIZE_CB",   (Icallback)iMatrixResize_CB);
  IupSetCallback(ih, "FOCUS_CB",    (Icallback)iMatrixFocus_CB);
  IupSetCallback(ih, "BUTTON_CB",   (Icallback)iupMatrixMouseButton_CB);
  IupSetCallback(ih, "MOTION_CB",   (Icallback)iupMatrixMouseMove_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iupMatrixKeyPress_CB);
  IupSetCallback(ih, "SCROLL_CB",   (Icallback)iupMatrixScroll_CB);

  /* Create the edit fields */
  iupMatrixEditCreate(ih);

  /* defaults that are non zero */
  ih->data->datah = ih->data->texth;
  ih->data->mark_continuous = 1;
  ih->data->columns.num = 1;
  ih->data->lines.num = 1;
  ih->data->columns.num_noscroll = 1;
  ih->data->lines.num_noscroll = 1;
  ih->data->need_calcsize = 1;
  ih->data->need_redraw = 1;
  ih->data->lines.first = 1;
  ih->data->columns.first = 1;
  ih->data->lines.focus_cell = 1;
  ih->data->columns.focus_cell = 1;
  ih->data->mark_lin1 = -1;
  ih->data->mark_col1 = -1;
  ih->data->mark_lin2 = -1;
  ih->data->mark_col2 = -1;

  return IUP_NOERROR;
}

static int iMatrixMapMethod(Ihandle* ih)
{
  ih->data->cdcanvas = cdCreateCanvas(CD_IUP, ih);
  if (!ih->data->cdcanvas)
    return IUP_ERROR;

  /* this can fail if canvas size is zero */
  ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);

  if (IupGetCallback(ih, "VALUE_CB"))
  {
    ih->data->callback_mode = 1;

    if (!IupGetCallback(ih, "VALUE_EDIT_CB"))
      iupAttribSetStr(ih, "READONLY", "YES");
  }

  iupMatrixMemAlloc(ih);

  IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)iMatrixConvertXYToPos);

  return IUP_NOERROR;
}

static void iMatrixUnMapMethod(Ihandle* ih)
{
  if(ih->data->cddbuffer)
  {
    cdKillCanvas(ih->data->cddbuffer);
    ih->data->cddbuffer = NULL;
  }

  if(ih->data->cdcanvas)
  {
    cdKillCanvas(ih->data->cdcanvas);
    ih->data->cdcanvas = NULL;
  }

  iupMatrixMemRelease(ih);
}

static int iMatrixGetNaturalWidth(Ihandle* ih, int *full_width)
{
  int width = 0, num, col;

  num = iupAttribGetInt(ih, "NUMCOL_VISIBLE")+1;  /* add the title column */

  if (iupAttribGetInt(ih, "NUMCOL_VISIBLE_LAST"))
  {
    int start = ih->data->columns.num - (num-1); /* title is computed apart */
    if (start<1) start=1;
    width += iupMatrixGetColumnWidth(ih, 0, 1); /* compute title */
    for(col = start; col < ih->data->columns.num; col++)
      width += iupMatrixGetColumnWidth(ih, col, 1);

    if (ih->data->limit_expand)
    {
      *full_width = width;
      for(col = 0; col < start; col++)
        (*full_width) += iupMatrixGetColumnWidth(ih, col, 1);
    }
  }
  else
  {
    for(col = 0; col < num; col++)  /* num can be > numcol */
      width += iupMatrixGetColumnWidth(ih, col, 1);

    if (ih->data->limit_expand)
    {
      *full_width = width;
      for(col = num; col < ih->data->columns.num; col++)
        (*full_width) += iupMatrixGetColumnWidth(ih, col, 1);
    }
  }

  return width;
}

static int iMatrixGetNaturalHeight(Ihandle* ih, int *full_height)
{
  int height = 0, num, lin;

  num = iupAttribGetInt(ih, "NUMLIN_VISIBLE")+1;  /* add the title line */

  if (iupAttribGetInt(ih, "NUMLIN_VISIBLE_LAST"))
  {
    int start = ih->data->lines.num - (num-1);   /* title is computed apart */
    if (start<1) start=1;
    height += iupMatrixGetLineHeight(ih, 0, 1);  /* compute title */
    for(lin = start; lin < ih->data->lines.num; lin++)
      height += iupMatrixGetLineHeight(ih, lin, 1);

    if (ih->data->limit_expand)
    {
      *full_height = height;
      for(lin = 0; lin < start; lin++)
        (*full_height) += iupMatrixGetLineHeight(ih, lin, 1);
    }
  }
  else
  {
    for(lin = 0; lin < num; lin++)  /* num can be > numlin */
      height += iupMatrixGetLineHeight(ih, lin, 1);

    if (ih->data->limit_expand)
    {
      *full_height = height;
      for(lin = num; lin < ih->data->lines.num; lin++)
        (*full_height) += iupMatrixGetLineHeight(ih, lin, 1);
    }
  }

  return height;
}

static void iMatrixComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int sb_w = 0, sb_h = 0, full_width, full_height;
  (void)expand; /* unset if not name container */

  if (!ih->handle)
    ih->data->canvas.sb = iupBaseGetScrollbar(ih);

  /* add scrollbar */
  if (ih->data->canvas.sb)
  {
    int sb_size = iupdrvGetScrollbarSize();
    if (ih->data->canvas.sb & IUP_SB_HORIZ)
      sb_h += sb_size;  /* sb horizontal affects vertical size */
    if (ih->data->canvas.sb & IUP_SB_VERT)
      sb_w += sb_size;  /* sb vertical affects horizontal size */
  }

  *w = sb_w + iMatrixGetNaturalWidth(ih, &full_width);
  *h = sb_h + iMatrixGetNaturalHeight(ih, &full_height);

  if (ih->data->limit_expand)
  {
    full_width += sb_w;
    full_height += sb_h;
    IupSetfAttribute(ih, "MAXSIZE", "%dx%d", full_width, full_height);
  }
}

static void iMatrixCreateCursor(void)
{
  Ihandle *imgcursor;
  unsigned char matrx_img_cur_excel[15*15] = 
  {
    0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    1,1,1,1,1,2,2,2,2,1,1,1,1,1,0,
    1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,
    1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,
    0,1,1,1,1,2,2,2,2,1,1,1,1,1,1,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    0,0,0,0,1,2,2,2,2,1,1,0,0,0,0,
    0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,
    0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };

  imgcursor = IupImage(15, 15, matrx_img_cur_excel);
  IupSetAttribute(imgcursor, "0", "BGCOLOR"); 
  IupSetAttribute(imgcursor, "1", "0 0 0"); 
  IupSetAttribute(imgcursor, "2", "255 255 255"); 
  IupSetAttribute(imgcursor, "HOTSPOT", "7:7");     /* Centered Hotspot           */
  IupSetHandle("IupMatrixCrossCursor", imgcursor); 
  IupSetHandle("matrx_img_cur_excel",  imgcursor);  /* for backward compatibility */
}

Iclass* iupMatrixNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("canvas"));

  ic->name = "matrix";
  ic->format = "a"; /* one ACTION_CB callback name */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;
  ic->has_attrib_id = 2;   /* has attributes with IDs that must be parsed */

  /* Class functions */
  ic->New = iupMatrixNewClass;
  ic->Create  = iMatrixCreateMethod;
  ic->Map     = iMatrixMapMethod;
  ic->UnMap   = iMatrixUnMapMethod;
  ic->ComputeNaturalSize = iMatrixComputeNaturalSizeMethod;

  /* Do not need to set base attributes because they are inherited from IupCanvas */

  /* IupMatrix Callbacks */
  /* --- Interaction --- */
  iupClassRegisterCallback(ic, "ACTION_CB",  "iiiis");
  iupClassRegisterCallback(ic, "CLICK_CB",   "iis");
  iupClassRegisterCallback(ic, "RELEASE_CB", "iis");
  iupClassRegisterCallback(ic, "MOUSEMOVE_CB", "ii");
  iupClassRegisterCallback(ic, "ENTERITEM_CB", "ii");
  iupClassRegisterCallback(ic, "LEAVEITEM_CB", "ii");
  iupClassRegisterCallback(ic, "SCROLLTOP_CB", "ii");
  /* --- Drawing --- */
  iupClassRegisterCallback(ic, "BGCOLOR_CB", "iiIII");
  iupClassRegisterCallback(ic, "FGCOLOR_CB", "iiIII");
  iupClassRegisterCallback(ic, "FONT_CB", "ii=s");
  iupClassRegisterCallback(ic, "DRAW_CB", "iiiiiiv");
  iupClassRegisterCallback(ic, "DROPCHECK_CB", "ii");
  /* --- Editing --- */
  iupClassRegisterCallback(ic, "MENUDROP_CB", "nii");
  iupClassRegisterCallback(ic, "DROP_CB", "nii");
  iupClassRegisterCallback(ic, "DROPSELECT_CB", "iinsii");
  iupClassRegisterCallback(ic, "EDITION_CB", "iii");
  /* --- Callback Mode --- */
  iupClassRegisterCallback(ic, "VALUE_CB", "ii=s");
  iupClassRegisterCallback(ic, "VALUE_EDIT_CB", "iis");
  iupClassRegisterCallback(ic, "MARK_CB", "ii");
  iupClassRegisterCallback(ic, "MARKEDIT_CB", "iii");

  iupClassRegisterAttribute(ic, "VISIBLE", iupBaseGetVisibleAttrib, iMatrixSetVisibleAttrib, "YES", "NO", IUPAF_NO_SAVE|IUPAF_DEFAULT);

  /* Change the Canvas default */
  iupClassRegisterReplaceAttribDef(ic, "CURSOR", "IupMatrixCrossCursor", "ARROW");
  iupClassRegisterReplaceAttribDef(ic, "BORDER", "NO", "YES");
  iupClassRegisterReplaceAttribDef(ic, "SCROLLBAR", "YES", NULL);

  /* Change the Canvas flags */
  iupClassRegisterReplaceAttribFlags(ic, "DX", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterReplaceAttribFlags(ic, "DY", IUPAF_NO_SAVE|IUPAF_NO_INHERIT);

  /* IupMatrix Attributes - CELL */
  iupClassRegisterAttributeId2(ic, "IDVALUE", iMatrixGetIdValueAttrib, iMatrixSetIdValueAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FOCUS_CELL", iMatrixGetFocusCellAttrib, iMatrixSetFocusCellAttrib, IUPAF_SAMEASSYSTEM, "1:1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT); /* can be NOT mapped */
  iupClassRegisterAttribute(ic, "VALUE", iMatrixGetValueAttrib, iMatrixSetValueAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId2(ic, "BGCOLOR", iMatrixGetBgColorAttrib, iMatrixSetBgColorAttrib, IUPAF_NOT_MAPPED);
  iupClassRegisterAttributeId2(ic, "FGCOLOR", NULL, iMatrixSetFgColorAttrib, IUPAF_NOT_MAPPED);
  iupClassRegisterAttributeId2(ic, "FONT", iMatrixGetFontAttrib, iMatrixSetFontAttrib, IUPAF_NOT_MAPPED);
  iupClassRegisterAttributeId2(ic, "FRAMEHORIZCOLOR", NULL, iMatrixSetFrameHorizColorAttrib, IUPAF_NOT_MAPPED);
  iupClassRegisterAttributeId2(ic, "FRAMEVERTCOLOR", NULL, iMatrixSetFrameVertColorAttrib, IUPAF_NOT_MAPPED);
  iupClassRegisterAttributeId2(ic, "CELLOFFSET", iMatrixGetCellOffsetAttrib, NULL, IUPAF_READONLY);
  iupClassRegisterAttributeId2(ic, "CELLSIZE", iMatrixGetCellSizeAttrib, NULL, IUPAF_READONLY);
  iupClassRegisterAttributeId2(ic, "CELLBGCOLOR", iMatrixGetCellBgColorAttrib, NULL, IUPAF_READONLY);
  iupClassRegisterAttributeId2(ic, "CELLFGCOLOR", iMatrixGetCellFgColorAttrib, NULL, IUPAF_READONLY);

  /* IupMatrix Attributes - COLUMN */
  iupClassRegisterAttributeId(ic, "ALIGNMENT", iMatrixGetAlignmentAttrib, (IattribSetIdFunc)iMatrixSetNeedRedraw, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "SORTSIGN", NULL, (IattribSetIdFunc)iMatrixSetNeedRedraw, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* IupMatrix Attributes - SIZE */
  iupClassRegisterAttribute(ic, "COUNT", iMatrixGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMLIN", iMatrixGetNumLinAttrib, iupMatrixSetNumLinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMCOL", iMatrixGetNumColAttrib, iupMatrixSetNumColAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMLIN_NOSCROLL", iMatrixGetNumLinNoScrollAttrib, iMatrixSetNumLinNoScrollAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMCOL_NOSCROLL", iMatrixGetNumColNoScrollAttrib, iMatrixSetNumColNoScrollAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMLIN_VISIBLE", iMatrixGetNumLinVisibleAttrib, NULL, IUPAF_SAMEASSYSTEM, "3", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMCOL_VISIBLE", iMatrixGetNumColVisibleAttrib, NULL, IUPAF_SAMEASSYSTEM, "4", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMLIN_VISIBLE_LAST", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMCOL_VISIBLE_LAST", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WIDTHDEF", NULL, NULL, IUPAF_SAMEASSYSTEM, "80", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HEIGHTDEF", NULL, NULL, IUPAF_SAMEASSYSTEM, "8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "WIDTH", iMatrixGetWidthAttrib, iMatrixSetSizeAttrib, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "HEIGHT", iMatrixGetHeightAttrib, iMatrixSetSizeAttrib, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "RASTERWIDTH", iMatrixGetRasterWidthAttrib, iMatrixSetSizeAttrib, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "RASTERHEIGHT", iMatrixGetRasterHeightAttrib, iMatrixSetSizeAttrib, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FITTOSIZE", NULL, iMatrixSetFitToSizeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FITTOTEXT", NULL, iMatrixSetFitToTextAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FITMAXHEIGHT", NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "FITMAXWIDTH", NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* IupMatrix Attributes - MARK */
  iupClassRegisterAttribute(ic, "MARKED", iupMatrixGetMarkedAttrib, iupMatrixSetMarkedAttrib, NULL, NULL, IUPAF_NO_INHERIT);  /* noticed that MARKED must be mapped */
  iupClassRegisterAttributeId2(ic, "MARK", iupMatrixGetMarkAttrib, iupMatrixSetMarkAttrib, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);  /* noticed that for MARK the matrix must be mapped */
  iupClassRegisterAttribute(ic, "MARK_MODE", iMatrixGetMarkModeAttrib, iMatrixSetMarkModeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARKMODE", iMatrixGetMarkModeAttrib, iMatrixSetMarkModeAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AREA", iMatrixGetMarkAreaAttrib, iMatrixSetMarkAreaAttrib, IUPAF_SAMEASSYSTEM, "CONTINUOUS", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARKAREA", iMatrixGetMarkAreaAttrib, iMatrixSetMarkAreaAttrib, IUPAF_SAMEASSYSTEM, "CONTINUOUS", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTIPLE", iMatrixGetMarkMultipleAttrib, iMatrixSetMarkMultipleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARKMULTIPLE", iMatrixGetMarkMultipleAttrib, iMatrixSetMarkMultipleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* IupMatrix Attributes - ACTION (only mapped) */
  iupClassRegisterAttribute(ic, "ADDLIN", NULL, iupMatrixSetAddLinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DELLIN", NULL, iupMatrixSetDelLinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ADDCOL", NULL, iupMatrixSetAddColAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DELCOL", NULL, iupMatrixSetDelColAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ORIGIN", iMatrixGetOriginAttrib, iMatrixSetOriginAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ORIGINOFFSET", iMatrixGetOriginOffsetAttrib, NULL, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOW", NULL, iMatrixSetShowAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EDIT_MODE", iMatrixGetEditModeAttrib, iMatrixSetEditModeAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EDITNEXT", iMatrixGetEditNextAttrib, iMatrixSetEditNextAttrib, IUPAF_SAMEASSYSTEM, "LIN", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "REDRAW", NULL, iupMatrixDrawSetRedrawAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId2(ic, "CLEARVALUE", NULL, iMatrixSetClearValueAttrib, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId2(ic, "CLEARATTRIB", NULL, iMatrixSetClearAttribAttrib, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* IupMatrix Attributes - EDITION */
  iupClassRegisterAttribute(ic, "CARET", iMatrixGetCaretAttrib, iMatrixSetCaretAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTION", iMatrixGetSelectionAttrib, iMatrixSetSelectionAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTILINE", iMatrixGetMultilineAttrib, iMatrixSetMultilineAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MASK", NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* IupMatrix Attributes - GENERAL */
  iupClassRegisterAttribute(ic, "USETITLESIZE", iMatrixGetUseTitleSizeAttrib, iMatrixSetUseTitleSizeAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LIMITEXPAND", iMatrixGetLimitExpandAttrib, iMatrixSetLimitExpandAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIDDENTEXTMARKS", iMatrixGetHiddenTextMarksAttrib, iMatrixSetHiddenTextMarksAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  
  iupClassRegisterAttribute(ic, "FRAMECOLOR", NULL, (IattribSetFunc)iMatrixSetNeedRedraw, IUPAF_SAMEASSYSTEM, "100 100 100", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "READONLY", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RESIZEMATRIX", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HIDEFOCUS", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* Overwrite IupCanvas Attributes */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iMatrixSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* IupMatrix Attributes - MASK */
  iupClassRegisterAttribute(ic, "OLD_MASK_DATA", iMatrixGetMaskDataAttrib, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  if (!IupGetHandle("IupMatrixCrossCursor"))
    iMatrixCreateCursor();

  return ic;
}


/*****************************************************************************************************/


Ihandle* IupMatrix(const char* action)
{
  void *params[2];
  params[0] = (void*)action;
  params[1] = NULL;
  return IupCreatev("matrix", params);
}

/***********************************************************************/

void IupMatSetAttribute(Ihandle* ih, const char* name, int lin, int col, const char* value)
{
  IupSetAttributeId2(ih, name, lin, col, value);
}

void IupMatStoreAttribute(Ihandle* ih, const char* name, int lin, int col, const char* value)
{
  IupStoreAttributeId2(ih, name, lin, col, value);
}

char* IupMatGetAttribute(Ihandle* ih, const char* name, int lin, int col)
{
  return IupGetAttributeId2(ih, name, lin, col);
}

int IupMatGetInt(Ihandle* ih, const char* name, int lin, int col)
{
  return IupGetIntId2(ih, name, lin, col);
}

float IupMatGetFloat(Ihandle* ih, const char* name, int lin, int col)
{
  return IupGetFloatId2(ih, name, lin, col);
}

void IupMatSetfAttribute(Ihandle* ih, const char* name, int lin, int col, const char* f, ...)
{
  int size;
  char* value = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, f);
  vsnprintf(value, size, f, arglist);
  va_end(arglist);
  IupStoreAttributeId2(ih, name, lin, col, value);
}
