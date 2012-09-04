#include <stdlib.h>
#include <stdio.h>

#include "iup.h"
#include "iupkey.h"


static int copy_cb(Ihandle* ih)
{
  Ihandle* text = IupGetDialogChild(ih, "text");
  Ihandle* clipboard = IupClipboard();
  IupSetAttribute(clipboard, "TEXT", IupGetAttribute(text, "VALUE"));
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

static int paste_cb(Ihandle* ih)
{
  Ihandle* text = IupGetDialogChild(ih, "text");
  Ihandle* clipboard = IupClipboard();
  IupSetAttribute(text, "VALUE", IupGetAttribute(clipboard, "TEXT"));
  IupDestroy(clipboard);
  return IUP_DEFAULT;
}

void ClipboardTest(void)
{
  Ihandle *dlg = IupDialog(IupVbox(
    IupSetAttributes(IupText(NULL), "SIZE=80x60, MULTILINE=YES, NAME=text, EXPAND=YES"),
    IupSetCallbacks(IupButton("Copy", NULL), "ACTION", copy_cb, NULL), 
    IupSetCallbacks(IupButton("Paste", NULL), "ACTION", paste_cb, NULL), 
    NULL));
  IupSetAttribute(dlg, "GAP", "10");
  IupSetAttribute(dlg, "MARGIN", "20x20");

  IupSetAttribute(dlg, "TITLE", "Clipboard Test");
  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ClipboardTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
