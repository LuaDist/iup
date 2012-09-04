#include <stdlib.h>
#include <stdio.h>

#include "iup.h"


static int spin_cb(Ihandle* ih, int inc)
{
  (void)ih;
  /* does nothing, just print the call */
  printf("SPIN_CB(%d)\n", inc);
  return IUP_DEFAULT;
}

void SpinTest(void)
{
  Ihandle *dlg, *spinbox;

  spinbox = IupSpinbox(IupSetAttributes(IupText(NULL), "SIZE=50x, EXPAND=HORIZONTAL"));

  IupSetCallback(spinbox, "SPIN_CB", (Icallback)spin_cb);

  dlg = IupDialog(IupVbox(spinbox, NULL));
  IupSetAttribute(dlg, "MARGIN", "10x10");

  IupSetAttribute(dlg, "TITLE", "IupSpin Test");
  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  SpinTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
