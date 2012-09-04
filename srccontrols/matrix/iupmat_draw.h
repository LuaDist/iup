/** \file
 * \brief iupmatrix control
 * draw functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPMAT_DRAW_H 
#define __IUPMAT_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

void iupMatrixDrawCells(Ihandle* ih, int lin1, int col1, int lin2, int col2);
void iupMatrixDrawColumnTitle(Ihandle* ih, int col1, int col2);
void iupMatrixDrawLineTitle(Ihandle* ih, int lin1, int lin2);

void iupMatrixDrawSetDropFeedbackArea(int *x1, int *y1, int *x2, int *y2);

/* Render the visible cells and update display */
void iupMatrixDraw(Ihandle* ih, int update);

/* Update the display only */
void iupMatrixDrawUpdate(Ihandle* ih);

int iupMatrixDrawSetRedrawAttrib(Ihandle* ih, const char* value);

/* Color attenuation factor in a marked cell, 20% darker */
#define IMAT_ATENUATION(_x)    ((unsigned char)(((_x)*8)/10))


#ifdef __cplusplus
}
#endif

#endif
