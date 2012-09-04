#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "cd.h"
#include "iupcontrols.h"


static int nlines_cb(Ihandle* ih)
{
   return 8;
}

static int ncols_cb(Ihandle* ih)
{
   return 8;
}

static int height_cb(Ihandle* ih, int i)
{
   return 50;
}

static int width_cb(Ihandle* ih, int j)
{
   return 50;
}

static int draw_cb(Ihandle* ih, int i, int j, int xmin, int xmax, int ymin, int ymax)
{
  if (((i%2) && (j%2)) || (((i+1)%2) && ((j+1)%2)))
    cdForeground(CD_WHITE);
  else
    cdForeground(CD_BLACK);

  cdBox(xmin, xmax, ymin, ymax);
  return IUP_DEFAULT;
}

static Ihandle* create(void)
{
   Ihandle* cells = IupCells();

   IupSetCallback(cells, "DRAW_CB",   (Icallback)draw_cb);
   IupSetCallback(cells, "WIDTH_CB",  (Icallback)width_cb);
   IupSetCallback(cells, "HEIGHT_CB", (Icallback)height_cb);
   IupSetCallback(cells, "NLINES_CB", (Icallback)nlines_cb);
   IupSetCallback(cells, "NCOLS_CB",  (Icallback)ncols_cb);

   return cells;
}

void CellsCheckboardTest(void)
{
  Ihandle* dlg = NULL;
  Ihandle* cells  = NULL;

  cells  = create();
  dlg = IupDialog(cells);

  IupSetAttribute(dlg, "RASTERSIZE", "400x400");
  IupSetAttribute(dlg, "TITLE", "IupCells");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER) ;
  IupSetAttribute(dlg, "RASTERSIZE", NULL);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  CellsCheckboardTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
