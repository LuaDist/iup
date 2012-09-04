#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "cd.h"
#include "cdiup.h"
#include "wd.h"


/* World: (0-600)x(0-400)
   The canvas will be a window into that space.
   If canvas is smaller than the virtual space, scrollbars are active.
   The drawing is a red X connecting the corners of the world.
*/
#define WORLD_W 600
#define WORLD_H 400
static int scale = 1;

static void update_scrollbar(Ihandle* ih, int canvas_w, int canvas_h)
{
  /* update page size, it is always the client size of the canvas,
     but must convert it to world coordinates.
     If you change canvas size or scale must call this function. */
  double ww, wh;
  if (scale > 0)
  {
    ww = (double)canvas_w/scale;
    wh = (double)canvas_h/scale;
  }
  else
  {
    ww = canvas_w*abs(scale);
    wh = canvas_h*abs(scale);
  }
  IupSetfAttribute(ih, "DX", "%g", ww);
  IupSetfAttribute(ih, "DY", "%g", wh);
}

static void update_viewport(Ihandle* ih, cdCanvas *canvas, float posx, float posy)
{
  int view_x, view_y, view_w, view_h;

  /* The CD viewport is the same area represented by the virtual space of the scrollbar,
     but not using the same coordinates. */

  /* posy is top-bottom, CD is bottom-top.
     invert posy reference (YMAX-DY - POSY) */
  posy = WORLD_H-IupGetFloat(ih, "DY") - posy;
  if (posy < 0) posy = 0;

  if (scale > 0)
  {
    view_w = WORLD_W*scale;
    view_h = WORLD_H*scale;
    view_x = (int)(posx*scale);
    view_y = (int)(posy*scale);
  }
  else
  {
    view_w = WORLD_W/abs(scale);
    view_h = WORLD_H/abs(scale);
    view_x = (int)(posx/abs(scale));
    view_y = (int)(posy/abs(scale));
  }

  wdCanvasViewport(canvas, -view_x, view_w-1 - view_x, -view_y, view_h-1 - view_y);
}

/************************************************************************************/

static int action(Ihandle *ih)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
  
  cdCanvasClear(canvas);

  cdCanvasForeground(canvas, CD_RED);
  wdCanvasLine(canvas, 0, 0, WORLD_W, WORLD_H);
  wdCanvasLine(canvas, 0, WORLD_H, WORLD_W, 0);
  wdCanvasArc(canvas, WORLD_W/2, WORLD_H/2+WORLD_H/10, WORLD_W/10, WORLD_H/10, 0, 360);

  return IUP_DEFAULT;
}

static int resize_cb(Ihandle *ih, int canvas_w, int canvas_h)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");

  /* update CD canvas */
  cdCanvasActivate(canvas);
 
  update_scrollbar(ih, canvas_w, canvas_h);
  update_viewport(ih, canvas, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));
  return IUP_DEFAULT;
}

static int scroll_cb(Ihandle *ih, int op, float posx, float posy)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
  update_viewport(ih, canvas, posx, posy);
  IupRedraw(ih, 0);
  (void)op;
  return IUP_DEFAULT;
}

static int wheel_cb(Ihandle *ih,float delta,int x,int y,char* status)
{
  int canvas_w, canvas_h;
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
  (void)x;
  (void)y;
  (void)status;

  if (scale+delta==0) /* skip 0 */
  {
    if (scale > 0) 
      scale = -1;
    else 
      scale = 1;
  }
  else
    scale += (int)delta;

  cdCanvasGetSize(canvas, &canvas_w, &canvas_h, NULL, NULL);
  update_scrollbar(ih, canvas_w, canvas_h);
  update_viewport(ih, canvas, IupGetFloat(ih, "POSX"), IupGetFloat(ih, "POSY"));
  IupRedraw(ih, 0);
  return IUP_DEFAULT;
}

static int map_cb(Ihandle *ih)
{
  cdCanvas *canvas = cdCreateCanvas(CD_IUP, ih);

  /* World size is fixed */
  wdCanvasWindow(canvas, 0, WORLD_W, 0, WORLD_H);

  /* handle scrollbar in world coordinates, so we only have to update DX/DY */
  IupSetAttribute(ih, "XMIN", "0");
  IupSetAttribute(ih, "YMIN", "0");
  IupSetfAttribute(ih, "XMAX", "%d", WORLD_W);
  IupSetfAttribute(ih, "YMAX", "%d", WORLD_H);

  return IUP_DEFAULT;
}

static int unmap_cb(Ihandle *ih)
{
  cdCanvas *canvas = (cdCanvas*)IupGetAttribute(ih, "_CD_CANVAS");
  cdKillCanvas(canvas);
  return IUP_DEFAULT;
}

void CanvasScrollbarTest(void)
{
  Ihandle *dlg, *cnv;

  cnv = IupCanvas(NULL);
  IupSetAttribute(cnv, "RASTERSIZE", "300x200"); /* initial size */
  IupSetAttribute(cnv, "SCROLLBAR", "YES");

  IupSetCallback(cnv, "RESIZE_CB",  (Icallback)resize_cb);
  IupSetCallback(cnv, "ACTION",  (Icallback)action);
  IupSetCallback(cnv, "MAP_CB",  (Icallback)map_cb);
  IupSetCallback(cnv, "UNMAP_CB",  (Icallback)unmap_cb);
  IupSetCallback(cnv, "WHEEL_CB",  (Icallback)wheel_cb);
  IupSetCallback(cnv, "SCROLL_CB",  (Icallback)scroll_cb);
                   
  dlg = IupDialog(IupVbox(cnv, NULL));
  IupSetAttribute(dlg, "TITLE", "Scrollbar Test");
  IupSetAttribute(dlg, "MARGIN", "10x10");

  IupMap(dlg);
  IupSetAttribute(cnv, "RASTERSIZE", NULL);  /* release the minimum limitation */
 
  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  CanvasScrollbarTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
