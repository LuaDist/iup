#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcontrols.h"

//#define BIG_MATRIX
#ifndef BIG_MATRIX
static char data[3][3][50] = 
{
  {"1:1", "1:2", "1:3"}, 
  {"2:1", "2:2", "2:3"}, 
  {"3:1", "3:2", "3:3"}, 
};
#endif

static int dropcheck_cb(Ihandle *self, int lin, int col)
{
  if (lin == 3 && col == 1)
    return IUP_DEFAULT;
  return IUP_IGNORE;
}

static int drop(Ihandle *self, Ihandle *drop, int lin, int col)
{
  printf("drop_cb(%d, %d)\n", lin, col);
  if(lin == 3 && col == 1)
  {
    IupSetAttribute(drop, "1", "A - Test of Very Big String for Dropdown!");
    IupSetAttribute(drop, "2", "B");
    IupSetAttribute(drop, "3", "C");
    IupSetAttribute(drop, "4", "XXX");
    IupSetAttribute(drop, "5", "5");
    IupSetAttribute(drop, "6", "6");
    IupSetAttribute(drop, "7", "7");
    IupSetAttribute(drop, "8", NULL);
    return IUP_DEFAULT;
  }
  return IUP_IGNORE;
}

static char* value_cb(Ihandle *self, int lin, int col)
{
#ifdef BIG_MATRIX
  if (lin == 0 || col == 0)
    return "Title";
  return "cell";
#else
  if (lin == 0 || col == 0)
    return NULL;
  return data[lin-1][col-1];
#endif
}

static int value_edit_cb(Ihandle *self, int lin, int col, char* newvalue)
{
#ifndef BIG_MATRIX
  strcpy(data[lin-1][col-1], newvalue);
#endif
  return IUP_DEFAULT;
}

static int edition_cb(Ihandle *self, int lin, int col, int mode, int update)
{
  if (mode == 0)
  {
    char* value = IupGetAttribute(self, "VALUE");
#ifndef BIG_MATRIX
    if (value && strcmp(value, data[lin-1][col-1])!=0)
#else
    char* cell = IupMatGetAttribute(self, "", lin, col);
    if (value && cell && strcmp(value, cell)!=0)
#endif
      return IUP_IGNORE;
  }
  return IUP_DEFAULT;
}

static Ihandle* create_matrix(void)
{
  Ihandle* mat = IupMatrix(NULL); 
  int lin, col;
  
#ifdef BIG_MATRIX
  IupSetAttribute(mat, "NUMCOL", "20"); 
  IupSetAttribute(mat, "NUMLIN", "5000"); 
#else
  IupSetAttribute(mat, "NUMCOL", "3"); 
  IupSetAttribute(mat, "NUMLIN", "3"); 
#endif
  
  IupSetAttribute(mat, "NUMCOL_VISIBLE", "3");
  IupSetAttribute(mat, "NUMLIN_VISIBLE", "3");
  
//  IupSetAttribute(mat, "WIDTH2", "90");
//  IupSetAttribute(mat, "HEIGHT2", "30");
//  IupSetAttribute(mat, "WIDTHDEF", "34");
//  IupSetAttribute(mat,"RESIZEMATRIX", "YES");
  IupSetAttribute(mat,"SCROLLBAR", "YES");
  IupSetCallback(mat,"VALUE_CB",(Icallback)value_cb);
//  IupSetCallback(mat,"VALUE_EDIT_CB",(Icallback)value_edit_cb);
//  IupSetCallback(mat,"EDITION_CB",(Icallback)edition_cb);
  
//  IupSetCallback(mat, "DROPCHECK_CB", (Icallback)dropcheck_cb);
//  IupSetCallback(mat,"DROP_CB",(Icallback)drop);

#ifdef BIG_MATRIX
  IupSetAttribute(mat, "HEIGHT0", "10");
  IupSetAttribute(mat, "WIDTH0", "90");
#endif
//  IupSetAttribute(mat,"MARKMODE","LIN");
//  IupSetAttribute(mat,"MARKMULTIPLE","NO");
  IupSetAttribute(mat,"MARKMODE","CELL");
  IupSetAttribute(mat,"MARKMULTIPLE","YES");
//  IupSetAttribute(mat,"USETITLESIZE","YES");
//  IupSetAttribute(mat, "NUMCOL_NOSCROLL", "1");

//  IupSetAttribute(mat,"RASTERSIZE","x200");
//  IupSetAttribute(mat,"FITTOSIZE","LINES");

  //IupSetAttribute(mat, "NUMCOL_VISIBLE_LAST", "YES");
  //IupSetAttribute(mat, "NUMLIN_VISIBLE_LAST", "YES");
//  IupSetAttribute(mat, "WIDTHDEF", "15");

  IupSetAttribute(mat,"FRAMEVERTCOLOR1:2","BGCOLOR");
  IupSetAttribute(mat,"FRAMEHORIZCOLOR2:1","BGCOLOR");
//  IupSetAttribute(mat,"FRAMEVERTCOLOR1:2","255 255 255");
//  IupSetAttribute(mat,"FRAMEHORIZCOLOR2:1","255 255 255");

#ifdef XXXBIG_MATRIX
  for (lin = 0; lin < 3000; lin++)
  {
    for (col = 0; col < 20; col++)
    {
      IupMatSetAttribute(mat,"BGCOLOR", lin, col, "192 192 192");
      IupMatSetAttribute(mat,"FGCOLOR", lin, col, "1 1 1");
    }
  }
#endif

  return mat;
}

void MatrixCbModeTest(void)
{
  Ihandle* dlg, *box;

  box = IupVbox(create_matrix(), NULL);
  IupSetAttribute(box, "MARGIN", "10x10");

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupMatrix Simple Test");
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  MatrixCbModeTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
