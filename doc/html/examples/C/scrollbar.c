#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "cd.h"
#include "cdiup.h"

cdCanvas *cdcanvas = NULL;

/* Virtual space size: 600x400
   The canvas will be a window into that space.
   If canvas is smaller than the virtual space, scrollbars are active.
   
   The drawing is a red cross from the corners of the virtual space.
   But CD must draw in the canvas space. So the position of the scrollbar
   will define the convertion between canvas space and virtual space.
*/

int action(Ihandle *ih, float posx, float posy)
{
  int iposx = (int)posx;
  int iposy = (int)posy;

  if (!cdcanvas)
    return IUP_DEFAULT;

  /* update CD canvas size */
  cdCanvasActivate(cdcanvas);

  /* invert scroll reference (YMAX-DY - POSY) */
  iposy = 399-IupGetInt(ih, "DY") - iposy;
  
  cdCanvasClear(cdcanvas);
  
  cdCanvasForeground(cdcanvas, CD_RED);
  cdCanvasLine(cdcanvas, 0-iposx, 0-iposy, 599-iposx, 399-iposy);
  cdCanvasLine(cdcanvas, 0-iposx, 399-iposy, 599-iposx, 0-iposy);

  return IUP_DEFAULT;
}

int scroll_cb(Ihandle *ih, int op, float posx, float posy)
{
  (void)op;
  action(ih, posx, posy);
  return IUP_DEFAULT;
}

int resize_cb(Ihandle *ih, int w, int h)
{
  /* update page size, it is always the client size of the canvas */
  IupSetfAttribute(ih, "DX", "%d", w);
  IupSetfAttribute(ih, "DY", "%d", h);
  
  /* refresh scrollbar in IUP 2.x */
  IupStoreAttribute(ih, "POSX", IupGetAttribute(ih, "POSX"));
  IupStoreAttribute(ih, "POSY", IupGetAttribute(ih, "POSY"));

  if (!cdcanvas)
    return IUP_DEFAULT;

  /* update CD canvas size */
  cdCanvasActivate(cdcanvas);
  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dialog, *canvas;

  IupOpen(&argc, &argv);

  canvas = IupCanvas(NULL);
  IupSetAttribute(canvas, "RASTERSIZE", "300x200"); /* initial size */
  IupSetAttribute(canvas, "SCROLLBAR", "YES");
  IupSetAttribute(canvas, "XMAX", "599");
  IupSetAttribute(canvas, "YMAX", "399");

  IupSetCallback(canvas, "SCROLL_CB",  (Icallback)scroll_cb);
  IupSetCallback(canvas, "RESIZE_CB",  (Icallback)resize_cb);
  IupSetCallback(canvas, "ACTION",  (Icallback)action);
                   
  dialog = IupDialog(canvas);
  IupSetAttribute(dialog, "TITLE", "Scrollbar Test");

  IupMap(dialog);
  cdcanvas = cdCreateCanvas(CD_IUP, canvas);
  IupSetAttribute(canvas, "RASTERSIZE", NULL);  /* release the minimum limitation */
  
  IupShowXY(dialog,IUP_CENTER,IUP_CENTER);

  IupMainLoop();

  cdKillCanvas(cdcanvas);
  IupDestroy(dialog);

  IupClose();

  return EXIT_SUCCESS;

}
