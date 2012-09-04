#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

void HboxTest(void)
{
  /* IUP identifiers */
  Ihandle *fr1, *fr2, *fr3, *dlg;
  Ihandle *b11, *b12, *b13;
  Ihandle *b21, *b22, *b23;
  Ihandle *b31, *b32, *b33;
  Ihandle *h1,  *h2,  *h3;

  /* Creates frame with three top aligned buttons */
  fr1 = IupFrame
  (
    h1 = IupHbox
    (
//      IupFill(),
      b11=IupButton("1", NULL),
      b12=IupButton("2", NULL),
      b13=IupButton("3", NULL),
//      IupFill(),
      NULL
    )
  );
//  IupSetAttribute(fr1, "TITLE", "ALIGNMENT=ATOP");
  IupSetAttribute(fr1, "TITLE", "IupHbox");
  IupSetAttribute(b11, "SIZE", "10x10");
  IupSetAttribute(b12, "SIZE", "20x16");
  IupSetAttribute(b13, "SIZE", "30x20");
  IupSetAttributes(h1, "ALIGNMENT=ATOP"); /* Sets hbox's alignment, gap and size */
  IupSetAttribute(h1, "HOMOGENEOUS", "YES");
  IupSetAttribute(h1, "EXPANDCHILDREN", "YES");
//  IupSetAttribute(b12, "VISIBLE", "NO");
//  IupSetAttribute(b12, "FLOATING", "YES");

  /* Creates frame with three buttons */
  fr2 = IupFrame
  (
    h2=IupHbox
    (
      IupFill(),
      b21=IupButton("1", NULL),
      b22=IupButton("2", NULL),
      b23=IupButton("3", NULL),
      IupFill(),
      NULL
    )
  );
  IupSetAttribute(fr2, "TITLE", "ALIGNMENT=ACENTER, GAP=20");
  IupSetAttribute(b21, "SIZE", "30x30");
  IupSetAttribute(b22, "SIZE", "30x40");
  IupSetAttribute(b23, "SIZE", "30x50");
  IupSetAttributes(h2, "ALIGNMENT=ACENTER, GAP=20"); /* Sets hbox's alignment and gap */

  /* Creates frame with three bottom aligned buttons */
  fr3 = IupFrame
  (
    h3=IupHbox
    (
      IupFill(),
      b31=IupButton ("1", NULL),
      b32=IupButton ("2", NULL),
      b33=IupButton ("3", NULL),
      IupFill(),
      NULL
    )
  );
  IupSetAttribute(fr3, "TITLE", "ALIGNMENT=ABOTTOM, MARGIN=10x10");
  IupSetAttribute(b31, "SIZE", "30x30");
  IupSetAttribute(b32, "SIZE", "30x40");
  IupSetAttribute(b33, "SIZE", "30x50");
  IupSetAttributes(h3, "ALIGNMENT=ABOTTOM, MARGIN=10x10"); /* Sets hbox's alignment and size */

  /* Creates dlg with the three frames */
  dlg = IupDialog
  (
    IupVbox
    (
      fr1,
      fr2,
      fr3,
      NULL
    )
  );

  IupSetAttribute(dlg, "TITLE", "IupHbox Test"); /* Sets dlg's title */
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "10");
  IupSetAttribute(fr1, "MARGIN", "0x0");   /* avoid attribute propagation */
  IupSetAttribute(fr2, "MARGIN", "0x0");
  IupSetAttribute(fr3, "MARGIN", "0x0");
  IupSetAttribute(fr1, "GAP", "0");
  IupSetAttribute(fr2, "GAP", "0");
  IupSetAttribute(fr3, "GAP", "0");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER); /* Shows dlg in the center of the screen */
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  HboxTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
