/*  IupMask: Example in C 
    Creates an IupText that accepts only numbers.
*/

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "iupcontrols.h"

int main(int argc, char **argv)
{
  Ihandle *text, *dg;

  IupOpen(&argc, &argv);

  text = IupText("");
  IupSetAttribute(text, "MASK", "/d*");

  IupSetAttribute(text, "SIZE",   "100x");
  IupSetAttribute(text, "EXPAND", "HORIZONTAL");

  dg = IupDialog(text);
  IupSetAttribute(dg, "TITLE", "IupMask");

  IupShow(dg);
  IupMainLoop();
  IupClose();
  return EXIT_SUCCESS;
}
