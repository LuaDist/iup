#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

void SboxTest(void)
{
  Ihandle *dlg, *bt, *box, *lbl, *ml, *vbox;

  bt = IupButton("Button", NULL);
  //IupSetAttribute(bt, "EXPAND", "VERTICAL");  /* This is the only necessary EXPAND */
  IupSetAttribute(bt, "EXPAND", "YES");

  box = IupSbox(bt);
  IupSetAttribute(box, "DIRECTION", "SOUTH");  /* place at the bottom of the button */
//  IupSetAttribute(box, "COLOR", "0 255 0");

  ml = IupMultiLine(NULL);
  IupSetAttribute(ml, "EXPAND", "YES");
  IupSetAttribute(ml, "VISIBLELINES", "5");
  vbox = IupVbox(box, ml, NULL);

  lbl = IupLabel("Label");
  IupSetAttribute(lbl, "EXPAND", "VERTICAL");

  dlg = IupDialog(IupHbox(vbox, lbl, NULL));
  IupSetAttribute(dlg, "TITLE", "IupSbox Example");
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "10");

  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  SboxTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
