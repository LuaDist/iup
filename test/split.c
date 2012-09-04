#include <stdio.h>
#include "iup.h"

void SplitTest(void)
{
  Ihandle *dlg, *bt, *split, *ml, *vbox;

  bt = IupButton("Button", NULL);
  IupSetAttribute(bt, "EXPAND", "YES");
//  IupSetAttribute(bt, "MINSIZE", "30x");

  ml = IupMultiLine(NULL);
  IupSetAttribute(ml, "EXPAND", "YES");
  IupSetAttribute(ml, "VISIBLELINES", "5");
  IupSetAttribute(ml, "VISIBLECOLUMNS", "10");
  
  split = IupSplit(bt, ml);
//  IupSetAttribute(split, "ORIENTATION", "VERTICAL");
//  IupSetAttribute(split, "ORIENTATION", "HORIZONTAL");
//  IupSetAttribute(split, "COLOR", "127 127 255");
//  IupSetAttribute(split, "LAYOUTDRAG", "NO");
  IupSetAttribute(split, "AUTOHIDE", "YES");
//  IupSetAttribute(split, "SHOWGRIP", "NO");
//  IupSetAttribute(split, "MINMAX", "100:800");

  vbox = IupVbox(split, NULL);
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "10");

  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "IupSplit Example");

  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  SplitTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
