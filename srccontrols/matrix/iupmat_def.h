/** \file
 * \brief iupmatrix. definitions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPMAT_DEF_H 
#define __IUPMAT_DEF_H

#ifdef __cplusplus
extern "C" {
#endif


#define IMAT_PROCESS_COL 1  /* Process the columns */
#define IMAT_PROCESS_LIN 2  /* Process the lines */

/***************************************************************************/
/* Decoration size in pixels                                               */
/***************************************************************************/
#define IMAT_PADDING_W   6
#define IMAT_PADDING_H   6
#define IMAT_FRAME_W   2
#define IMAT_FRAME_H   2

/* Cell flags */
#define IMAT_HAS_FONT    1     /* Has FONTL:C attribute */
#define IMAT_HAS_FGCOLOR 2     /* Has FGCOLORL:C attribute */
#define IMAT_HAS_BGCOLOR 4     /* Has BGCOLORL:C attribute */
#define IMAT_IS_MARKED   8     /* Is marked */
#define IMAT_HAS_FRAMEHORIZCOLOR 16  /* Has FRAMEHORIZCOLORL:C */
#define IMAT_HAS_FRAMEVERTCOLOR  32  /* Has FRAMEVERTCOLORL:C */

enum{IMAT_EDITNEXT_LIN, 
     IMAT_EDITNEXT_COL, 
     IMAT_EDITNEXT_LINCR, 
     IMAT_EDITNEXT_COLCR,
     IMAT_EDITNEXT_NONE};


/***************************************************************************/
/* Structures stored in each matrix                                        */
/***************************************************************************/
typedef struct _ImatCell
{
  char *value;      /* Cell value                              */
  unsigned char flags;  
} ImatCell;


typedef struct _ImatLinColData
{
  int* sizes;            /* Width/height of the columns/lines  (allocated after map)   */
  unsigned char* flags;  /* Attribute flags for the columns/lines (allocated after map) */

  int num;          /* Number of columns/lines, default/minimum=1, always includes the non scrollable cells */
  int num_alloc;    /* Number of columns/lines allocated, default=5 */
  int num_noscroll; /* Number of non scrollable columns/lines, default/minimum=1 */

  int first_offset; /* Scroll offset of the first visible column/line from right to left 
                       (or the invisible part of the first visible cell) 
                       This is how the scrollbar controls scrolling of cells. */
  int first;        /* First visible column/line */
  int last;         /* Last visible column/line  */

  /* used to configure the scrollbar */
  int total_size;   /* Sum of the widths/heights of the columns/lines, not including the non scrollable cells */
  int visible_size; /* Width/height of the visible window, not including the non scrollable cells */

  int focus_cell;   /* index of the current cell */
} ImatLinColData;

struct _IcontrolData
{
  iupCanvas canvas; /* from IupCanvas (must reserve it) */

  ImatCell** cells; /* Cell value, this will be NULL if in callback mode (allocated after map) */

  Ihandle* texth;   /* Text handle                    */
  Ihandle* droph;   /* Dropdown handle                */
  Ihandle* datah;   /* Current active edition element, may be equal to texth or droph */

  cdCanvas* cddbuffer;
  cdCanvas* cdcanvas;

  ImatLinColData lines;
  ImatLinColData columns;

  /* State */
  int has_focus;
  int w, h;         /* canvas size */
  int callback_mode;
  int need_calcsize;
  int need_redraw;
  int inside_markedit_cb;   /* avoid recursion */

  /* attributes */
  int mark_continuous, mark_mode, mark_multiple;
  int checkframecolor, hidden_text_marks, editnext;
  int use_title_size;   /* use title contents when calculating cell size */
  int limit_expand; /* limit expand to maximum size */

  /* Mouse and Keyboard AUX */
  int leftpressed;  /* left mouse button is pressed */
  int dclick;       /* left mouse button was double clicked */
  int homekeycount, endkeycount;  /* numbers of times that key was pressed */

  /* ColRes AUX */
  int colres_dragging,   /* indicates if it is being made a column resize  */
      colres_drag_col,   /* column being resized, handler is at right of the column */
      colres_drag_col_start_x, /* handler start position */
      colres_drag_col_last_x;  /* previous position */

  /* Mark AUX */
  int mark_lin1, mark_col1,  /* used to store the start cell when a block is being marked */
      mark_lin2, mark_col2,  /* used to store the end cell when a block was marked */
      mark_full1,            /* indicate if full lines or columns is being selected */
      mark_full2;

  /* Draw AUX, valid only after iupMatrixPrepareDrawData */
  sIFnii font_cb;
  IFniiIII fgcolor_cb;
  IFniiIII bgcolor_cb;
  char *bgcolor, *bgcolor_parent, *fgcolor, *font;  /* not need to free */

  /* Clipping AUX for cell  */
  int clip_x1, clip_x2, clip_y1, clip_y2;
};


int iupMatrixIsValid(Ihandle* ih, int check_cells);

#define iupMATRIX_INVERTYAXIS(_ih, _y) ((_ih)->data->h-1 - (_y))

#define iupMATRIX_CHECK_COL(_ih, _col) ((_col >= 0) && (_col < (_ih)->data->columns.num))
#define iupMATRIX_CHECK_LIN(_ih, _lin) ((_lin >= 0) && (_lin < (_ih)->data->lines.num))



#ifdef __cplusplus
}
#endif

#endif
