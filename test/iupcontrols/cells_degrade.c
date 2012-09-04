#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "cd.h"
#include "iupcontrols.h"


static int nlines_cb(Ihandle* ih)
{
  return 7;
}

static int ncols_cb(Ihandle* ih)
{
  return 7;
}

static int height_cb(Ihandle* ih, int i)
{
  return (int)(30 + i * 1.5);
}

static int width_cb(Ihandle* ih, int j)
{
  return (int)(50 + j * 1.5);
}

static int mouseclick_cb(Ihandle* ih, int b, char* m, int i, int j, int x, int y, char* r)
{
  printf("CLICK: %d: (%02d, %02d)\n", b, i, j);
  return IUP_DEFAULT;
}

static int scrolling_cb(Ihandle* ih, int i, int j)
{
  printf("SCROLL: (%02d, %02d)\n", i, j);
  return IUP_DEFAULT;
}

static int vspan_cb(Ihandle* ih, int i, int j)
{
  if (i == 1 && j == 1)
    return 2;

  if (i == 5 && j == 5)
    return 2;

  return 1;
}

static int hspan_cb(Ihandle* ih, int i, int j)
{
  if (i == 1 && j == 1)
    return 2;

  if (i == 5 && j == 5)
    return 2;

  return 1;
}

static int draw_cb(Ihandle* ih, int i, int j, int xmin, int xmax, int ymin, int ymax)
{
  static char buffer[64];
  int xm = (xmax + xmin) / 2;
  int ym = (ymax + ymin) / 2;

  if (i == 1 && j == 2)
    return IUP_DEFAULT;
  if (i == 2 && j == 1)
    return IUP_DEFAULT;
  if (i == 2 && j == 2)
    return IUP_DEFAULT;
  if (i == 5 && j == 6)
    return IUP_DEFAULT;
  if (i == 6 && j == 5)
    return IUP_DEFAULT;
  if (i == 6 && j == 6)
    return IUP_DEFAULT;

  if (i == 1 && j == 1)
    cdForeground(CD_WHITE);
  else
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
  IupSetCallback(cells, "HSPAN_CB",      (Icallback)hspan_cb);
  IupSetCallback(cells, "VSPAN_CB",      (Icallback)vspan_cb);
  IupSetCallback(cells, "SCROLLING_CB",  (Icallback)scrolling_cb);

  IupSetAttribute(cells, "BOXED",      "NO");
  IupSetAttribute(cells, "RASTERSIZE", "395x255");

  return cells;
}

void CellsDegradeTest(void)
{
  Ihandle* dlg = NULL;
  Ihandle* cells  = NULL;
  Ihandle* box    = NULL;

  cells = create();

  box = IupVbox(cells, NULL);
  IupSetAttribute(box, "MARGIN", "10x10");

  dlg = IupDialog(box);

  IupSetAttribute(dlg, "TITLE", "IupCells");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER) ;
  IupSetAttribute(cells, "RASTERSIZE", NULL);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  CellsDegradeTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
