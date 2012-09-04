#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "cd.h"
#include "iupcontrols.h"


static int nlines_cb(Ihandle* ih)
{
  return 20;
}

static int ncols_cb(Ihandle* ih)
{
  return 50;
}

static int height_cb(Ihandle* ih, int i)
{
  return 30;
}

static int width_cb(Ihandle* ih, int j)
{
  return 70;
}

static int mouseclick_cb(Ihandle* ih, int b, char* m, int i, int j, int x, int y, char* r)
{
  static char buffer[128];
  sprintf(buffer, "CLICK: %d: (%02d, %02d)\n", b, i, j);

  IupMessage("Hi!", buffer);
  return IUP_DEFAULT;
}

static int draw_cb(Ihandle* ih, int i, int j, int xmin, int xmax, int ymin, int ymax)
{
  static char buffer[64];
  int xm = (xmax + xmin) / 2;
  int ym = (ymax + ymin) / 2;

  cdForeground(cdEncodeColor((unsigned char)(i*20), (unsigned char)(j*100), (unsigned char)(i+100)));

  cdBox(xmin, xmax, ymin, ymax);
  cdTextAlignment(CD_CENTER);
  cdForeground(CD_BLACK);
  sprintf(buffer, "(%02d, %02d)", i, j);
  cdText(xm, ym, buffer);

  return IUP_DEFAULT;
}

static Ihandle* create(void)
{
  Ihandle* cells = IupCells(); 

  IupSetCallback(cells, "MOUSECLICK_CB", (Icallback)mouseclick_cb);
  IupSetCallback(cells, "DRAW_CB",       (Icallback)draw_cb);
  IupSetCallback(cells, "WIDTH_CB",      (Icallback)width_cb);
  IupSetCallback(cells, "HEIGHT_CB",     (Icallback)height_cb);
  IupSetCallback(cells, "NLINES_CB",     (Icallback)nlines_cb);
  IupSetCallback(cells, "NCOLS_CB",      (Icallback)ncols_cb);

  IupSetAttribute(cells, "BOXED", "NO");

  return cells;
}

void CellsNumberingTest(void)
{
  Ihandle* dlg = NULL;
  Ihandle* cells  = NULL;

  IupControlsOpen();

  cells  = create();
  dlg = IupDialog(cells);

  IupSetAttribute(cells, "RASTERSIZE", "500x500");
  IupSetAttribute(dlg, "TITLE", "IupCells");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER) ;
  IupSetAttribute(cells, "RASTERSIZE", NULL);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  CellsNumberingTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
