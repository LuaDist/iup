
#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "cd.h"
#include "cdiup.h"
#include "iupcontrols.h"

static Ihandle  *canvas = NULL;
static cdCanvas *cdcanvas = NULL;

static int redraw_cb(Ihandle* ih)
{
  if (!cdcanvas)
    return IUP_DEFAULT;

  /* Activates canvas cdcanvas */
  cdActivate(cdcanvas);
  cdClear();
  
  /* Draws a rectangle on the canvas */
  cdBegin(CD_FILL);
    cdVertex(50, 50);
    cdVertex(150, 50);
    cdVertex(100, 150);
  cdEnd();
  
  /* Function executed sucessfully */
  return IUP_DEFAULT;
}

static int extended_cb(Ihandle* ih, int cell)
{
  printf("extended_cb(%d)\n", cell);
  return IUP_DEFAULT;
}

static char* cell_cb(Ihandle* ih, int cell)
{
  int ri, gi, bi;
  static char str[30];

  sprintf(str, "CELL%d", cell);
  sscanf(IupGetAttribute(ih, str), "%d %d %d", &ri, &gi, &bi);
  printf("cell_cb(%d): %d, %d, %d\n", cell, ri, gi, bi);

/*  
  unsigned char r, g, b;
  r = (unsigned char)ri;
  g = (unsigned char)gi;
  b = (unsigned char)bi;
  if (IupGetColor(IUP_MOUSEPOS, IUP_MOUSEPOS, &r, &g, &b))
  {
    sprintf(str, "%d %d %d", (int)r, (int)g, (int)b);
    cdActivate( cdcanvas ) ;
    cdForeground(cdEncodeColor(r, g, b));
    redraw_cb(canvas);
    return str;
  }
*/
  return NULL;
}

static int select_cb(Ihandle* ih, int cell, int type)
{
  long cd_color;
  char str[30];
  int ri, gi, bi;

  sprintf(str, "CELL%d", cell);
  sscanf(IupGetAttribute(ih, str), "%d %d %d", &ri, &gi, &bi);
  printf("select_cb(%d, %d): %d, %d, %d\n", cell, type, ri, gi, bi);

  cd_color = cdEncodeColor((unsigned char)ri,(unsigned char)gi,(unsigned char)bi);

  if (!cdcanvas)
    return IUP_DEFAULT;
  
  cdActivate( cdcanvas ) ;
  if (type == -1)
    cdForeground(cd_color);
  else
    cdBackground(cd_color);

  redraw_cb(canvas);

  return IUP_DEFAULT;
}

static int switch_cb(Ihandle* ih, int primcell, int seccell)
{
  long fgcolor;
  if (!cdcanvas)
    return IUP_DEFAULT;
  printf("switch_cb(%d, %d)\n", primcell, seccell);
  cdActivate(cdcanvas) ;
  fgcolor = cdForeground(CD_QUERY);
  cdForeground(cdBackground(CD_QUERY));
  cdBackground(fgcolor);
  redraw_cb(canvas);
  return IUP_DEFAULT;
}

void ColorbarTest(void)
{
  Ihandle *dlg, *cb;
  
  IupControlsOpen();

  /* Creates a canvas associated with the redraw action */
  canvas = IupCanvas(NULL) ;
  IupSetCallback(canvas, "ACTION", (Icallback)redraw_cb);
  IupSetAttribute(canvas, "RASTERSIZE", "200x300");

  cb = IupColorbar(); 
  IupSetAttribute(cb, "RASTERSIZE",     "70x");
  IupSetAttribute(cb, "EXPAND",         "VERTICAL");
  IupSetAttribute(cb, "NUM_PARTS",      "2");
  IupSetAttribute(cb, "SHOW_SECONDARY", "YES");
  IupSetAttribute(cb, "PREVIEW_SIZE",   "60");
//  IupSetAttribute(cb, "ACTIVE",   "NO");
//  IupSetAttribute(cb, "BGCOLOR",   "128 0 255");

  IupSetCallback(cb, "SELECT_CB", (Icallback)select_cb);
  IupSetCallback(cb, "CELL_CB",   (Icallback)cell_cb);
  IupSetCallback(cb, "SWITCH_CB", (Icallback)switch_cb);
  IupSetCallback(cb, "EXTENDED_CB", (Icallback)extended_cb);

  dlg = IupDialog(IupHbox(canvas, cb, NULL));
  IupSetAttribute(dlg, "MARGIN", "5x5");
  
  IupSetAttribute(dlg, "TITLE", "IupColorbar");
  
  /* Maps the dlg. This must be done before the creation of the CD canvas.
     Could also use MAP_CB callback. */
  IupMap(dlg);
 
  cdcanvas = cdCreateCanvas(CD_IUP, canvas);
   
  IupPopup(dlg, IUP_CENTER, IUP_CENTER);

  cdKillCanvas(cdcanvas);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ColorbarTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
