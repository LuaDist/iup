#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

static int list_cb (Ihandle *h, char *t, int o, int selected)
{
  if (selected == 1)
  {
    Ihandle* zbox = IupGetHandle ("zbox");
    IupSetAttribute (zbox, "VALUE", t);
  }
  
  return IUP_DEFAULT;
}

void ZboxTest(void)
{
  Ihandle *dlg;
  Ihandle *frm;
  Ihandle *zbox;
  Ihandle *text;
  Ihandle *list;
  Ihandle *lbl;
  Ihandle *btn;
  Ihandle *frame;

  frame = IupFrame(IupSetAttributes(IupList(NULL), "DROPDOWN=YES, 1=Test, 2=XXX, VALUE=1"));
  IupSetAttribute (frame, "TITLE", "List");

  text = IupText("");

  IupSetAttributes (text, "EXPAND=YES, VALUE=\"Enter your text here\"");
  
  /* Creates a label */
  lbl = IupLabel("This element is a label");

  /* Creatas a button */
  btn = IupButton ("This button does nothing", "");

  /* Creates handles for manipulating the zbox VALUE */
  IupSetHandle ("frame", frame);
  IupSetHandle ("text", text);
  IupSetHandle ("lbl", lbl);
  IupSetHandle ("btn", btn);

  /* Creates zbox with four elements */
  zbox = IupZbox (frame, text, lbl, btn, NULL);
//  zbox = IupZbox (IupZbox(IupSetAttributes(IupCanvas(NULL), "RASTERSIZE=100x100"), NULL), NULL);

  /* Associates handle "zbox" with zbox */
  IupSetHandle ("zbox", zbox);

  /* Sets zbox alignment */
  IupSetAttribute (zbox, "ALIGNMENT", "ACENTER");
  IupSetAttribute (zbox, "VALUE", "text");

  /* Creates frame */
  frm = IupFrame
  (
    IupHbox
    (
      list = IupList(NULL),
      NULL
    )
  );

  /* Creates dialog */
  dlg = IupDialog
  (
    IupVbox
    (
      frm,
      zbox,
      NULL
    )
  );

  IupSetAttributes (list, "1 = frame, 2 = text, 3 = lbl, 4 = btn, VALUE=2");
  IupSetAttribute (frm, "TITLE", "Select an element");
  IupSetAttributes (dlg, "MARGIN=10x10, GAP=10, TITLE = \"IupZbox Example\"");
  IupSetCallback (list, "ACTION", (Icallback) list_cb);

  IupShowXY (dlg, IUP_CENTER, IUP_CENTER );
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ZboxTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
