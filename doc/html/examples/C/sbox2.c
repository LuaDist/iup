
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "iup.h"
#include "iupcontrols.h"

Ihandle *createtree()
{
  Ihandle *tree = IupTree();
  IupSetAttribute(tree, "SIZE", "80x80");
  IupSetAttributes(tree, "FONT=COURIER_NORMAL_10, \
                          NAME=Figures, \
                          ADDBRANCH=3D, \
                          ADDBRANCH=2D, \
                          ADDLEAF1=trapeze, \
                          ADDBRANCH1=parallelogram, \
                          ADDLEAF2=diamond, \
                          ADDLEAF2=square, \
                          ADDBRANCH4=triangle, \
                          ADDLEAF5=scalenus, \
                          ADDLEAF5=isoceles, \
                          ADDLEAF5=equilateral, \
                          VALUE=6, \
                          ADDEXPANDED=NO");
  return tree;
}

int main(int argc, char **argv)
{
  Ihandle *dg, *tree, *sbox, *ml, *cv, *sbox2, *vbox, *lb, *sbox3;

  IupOpen(&argc, &argv);
  IupControlsOpen();

  tree = createtree();
  IupSetAttribute(tree, "EXPAND", "YES");

  sbox = IupSbox(tree);
  IupSetAttribute(sbox, "DIRECTION", "EAST");

  cv = IupCanvas(NULL);
  IupSetAttribute(cv, "EXPAND", "YES");

  ml = IupMultiLine("");
  IupSetAttribute(ml, "EXPAND", "YES");
  sbox2 = IupSbox(ml);
  IupSetAttribute(sbox2, "DIRECTION", "WEST");

  vbox = IupHbox(sbox, cv, sbox2, NULL);

  lb = IupLabel("This is a label");
  IupSetAttribute(lb, "EXPAND", "NO");
  sbox3 = IupSbox(lb);
  IupSetAttribute(sbox3, "DIRECTION", "NORTH");
  dg = IupDialog(IupVbox(vbox, sbox3, NULL));
  IupSetAttribute(dg, "TITLE", "IupSbox Example");

  IupShow(dg);
  IupMainLoop();
  IupClose();
  return EXIT_SUCCESS;
}
