/** \file
 * \brief Simple Draw API.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_DRAW_H 
#define __IUP_DRAW_H

#ifdef __cplusplus
extern "C"
{
#endif

/** \defgroup draw Simple Draw API
 * \par
 * See \ref iup_draw.h
 * \ingroup util */



struct _IdrawCanvas;
typedef struct _IdrawCanvas IdrawCanvas;

enum{IUP_DRAW_FILL, IUP_DRAW_STROKE, IUP_DRAW_STROKE_DASH};

/** Creates a draw canvas based on an IupCanvas.
 * This will create an image for offscreen drawing.
 * \ingroup draw */
IdrawCanvas* iupDrawCreateCanvas(Ihandle* ih);

/** Destroys the IdrawCanvas.
 * \ingroup draw */
void iupDrawKillCanvas(IdrawCanvas* dc);

/** Draws the ofscreen image on the screen.
 * \ingroup draw */
void iupDrawFlush(IdrawCanvas* dc);

/** Rebuild the offscreen image if the canvas size has changed.
 * Automatically done in iupDrawCreateCanvas.
 * \ingroup draw */
void iupDrawUpdateSize(IdrawCanvas* dc);

/** Returns the canvas size available for drawing.
 * \ingroup draw */
void iupDrawGetSize(IdrawCanvas* dc, int *w, int *h);

/** Draws the parent background.
 * \ingroup draw */
void iupDrawParentBackground(IdrawCanvas* dc);

/** Draws a line.
 * \ingroup draw */
void iupDrawLine(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style);

/** Draws a filled/hollow rectangle.
 * \ingroup draw */
void iupDrawRectangle(IdrawCanvas* dc, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, int style);

/** Draws a rectangle inverting the color in the canvas.
 * \ingroup draw */
void iupDrawRectangleInvert(IdrawCanvas* dc, int x1, int y1, int x2, int y2);

/** Draws a filled/hollow arc.
 * \ingroup draw */
void iupDrawArc(IdrawCanvas* dc, int x1, int y1, int x2, int y2, double a1, double a2, unsigned char r, unsigned char g, unsigned char b, int style);

/** Draws a filled/hollow polygon.
 * points are arranged xyxyxy...
 * \ingroup draw */
void iupDrawPolygon(IdrawCanvas* dc, int* points, int count, unsigned char r, unsigned char g, unsigned char b, int style);

/** Draws a text.
 * x,y is at left,top corner of the text.
 * \ingroup draw */
void iupDrawText(IdrawCanvas* dc, const char* text, int len, int x, int y, unsigned char r, unsigned char g, unsigned char b, const char* font);

/** Draws an image.
 * x,y is at left,top corner of the image.
 * Returns the image size.
 * \ingroup draw */
void iupDrawImage(IdrawCanvas* dc, const char* name, int make_inactive, int x, int y, int *img_w, int *img_h);

/** Sets a rectangle clipping area.
 * \ingroup draw */
void iupDrawSetClipRect(IdrawCanvas* dc, int x1, int y1, int x2, int y2);

/** Removes clipping.
 * \ingroup draw */
void iupDrawResetClip(IdrawCanvas* dc);

/*
TO DO:
- check position and size of primitives
*/


#ifdef __cplusplus
}
#endif

#endif

