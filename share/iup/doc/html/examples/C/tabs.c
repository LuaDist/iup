/* IupTabs: Example in C 
   Creates a IupTabs control.
*/

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

int main(int argc, char **argv)
{
  Ihandle *dlg, *vbox1, *vbox2, *tabs1, *tabs2, *box;

  IupOpen(&argc, &argv);

  vbox1 = IupVbox(IupLabel("Inside Tab A"), IupButton("Button A", ""), NULL);
  vbox2 = IupVbox(IupLabel("Inside Tab B"), IupButton("Button B", ""), NULL);

  IupSetAttribute(vbox1, "TABTITLE", "Tab A");
  IupSetAttribute(vbox2, "TABTITLE", "Tab B");

  tabs1 = IupTabs(vbox1, vbox2, NULL);

  vbox1 = IupVbox(IupLabel("Inside Tab C"), IupButton("Button C", ""), NULL);
  vbox2 = IupVbox(IupLabel("Inside Tab D"), IupButton("Button D", ""), NULL);

  IupSetAttribute(vbox1, "TABTITLE", "Tab C");
  IupSetAttribute(vbox2, "TABTITLE", "Tab D");

  tabs2 = IupTabs(vbox1, vbox2, NULL);
  IupSetAttribute(tabs2, "TABTYPE", "LEFT");

  box = IupHbox(tabs1, tabs2, NULL);
  IupSetAttribute(box, "MARGIN", "10x10");
  IupSetAttribute(box, "GAP", "10");

  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupTabs");
  IupSetAttribute(dlg, "SIZE", "200x80");

  IupShowXY (dlg, IUP_CENTER, IUP_CENTER);
  IupMainLoop ();
  IupClose ();

  return EXIT_SUCCESS;

}