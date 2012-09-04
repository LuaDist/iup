#include <stdlib.h>
#include <stdio.h>
#include <iup.h>

void VboxTest(void)
{
  /* IUP identifiers */
  Ihandle *dlg;
  Ihandle *fr1, *fr2, *fr3;
  Ihandle *btn_11, *btn_12, *btn_13;
  Ihandle *btn_21, *btn_22, *btn_23;
  Ihandle *btn_31, *btn_32, *btn_33;
  Ihandle *vbox_1, *vbox_2, *vbox_3;

  /* Creates frame 1 */
  fr1 = IupFrame
  (
    vbox_1 = IupVbox
    (
      IupFill(),
      btn_11=IupButton("1", NULL),
      btn_12=IupButton("2", NULL),
      btn_13=IupButton("3", NULL),
      IupFill(),
      NULL
    )
  );

  IupSetAttribute(fr1, "TITLE", "ALIGNMENT=ALEFT");
  IupSetAttribute(btn_11, "SIZE", "20x10");
//  IupSetAttribute(btn_12, "SIZE", "30x20");
  IupSetAttribute(btn_12, "EXPAND", "VERTICAL");
  IupSetAttribute(btn_12, "EXPANDWEIGHT", "1.3");
//  IupSetAttribute(btn_13, "SIZE", "40x30");
  IupSetAttribute(btn_13, "EXPAND", "VERTICAL");
  IupSetAttribute(btn_13, "EXPANDWEIGHT", "0.7");
  IupSetAttribute(vbox_1, "ALIGNMENT", "ALEFT");
//  IupSetAttribute(vbox_1, "MARGIN", "20x20");
//  IupSetAttribute(vbox_1, "GAP", "10");
//  IupSetAttribute(vbox_1, "HOMOGENEOUS", "YES");
//  IupSetAttribute(vbox_1, "EXPANDCHILDREN", "YES");
//  IupSetAttribute(btn_12, "VISIBLE", "NO");
//  IupSetAttribute(btn_12, "FLOATING", "YES");

  /* Creates frame 2 */
  fr2 = IupFrame
  (
    vbox_2 = IupVbox
    (
      IupFill(),
      btn_21 = IupButton ("1", NULL),
      btn_22 = IupButton ("2", NULL),
      btn_23 = IupButton ("3", NULL),
      IupFill(),
      NULL
    )
  );

  IupSetAttribute(fr2, "TITLE", "ALIGNMENT=ACENTER");
  IupSetAttribute(btn_21, "SIZE", "20x30");
  IupSetAttribute(btn_22, "SIZE", "30x30");
  IupSetAttribute(btn_23, "SIZE", "40x30");
  IupSetAttribute(vbox_2, "ALIGNMENT", "ACENTER");

  /* Creates frame 3 */
  fr3 = IupFrame
  (
    vbox_3 = IupVbox
    (
      IupFill (),
      btn_31=IupButton ("1", NULL),
      btn_32=IupButton ("2", NULL),
      btn_33=IupButton ("3", NULL),
      IupFill (),
      NULL
    )
  );

  IupSetAttribute(fr3, "TITLE", "ALIGNMENT=ARIGHT");
  IupSetAttribute(btn_31, "SIZE", "20x30");
  IupSetAttribute(btn_32, "SIZE", "30x30");
  IupSetAttribute(btn_33, "SIZE", "40x30");
  IupSetAttribute(vbox_3, "ALIGNMENT", "ARIGHT");

  dlg = IupDialog
  (
    IupHbox
    (
      fr1,
      IupFill (),
      fr2,
      IupFill (),
      fr3,
      IupFill (),
      NULL
    )
  );

  IupSetAttribute(dlg, "TITLE", "IupVbox Test"); /* Sets dlg's title */
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "10");
  IupSetAttribute(fr1, "MARGIN", "0x0");   /* avoid attribute propagation */
  IupSetAttribute(fr2, "MARGIN", "0x0");
  IupSetAttribute(fr3, "MARGIN", "0x0");
  IupSetAttribute(fr1, "GAP", "0");
  IupSetAttribute(fr2, "GAP", "0");
  IupSetAttribute(fr3, "GAP", "0");

  /* Shows dlg in the center of the screen */
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  VboxTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
