#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

void FrameTest(void)
{
  Ihandle *dlg, *frame1, *frame2;

  frame1 = IupFrame
          (
            IupVbox
            (
              IupLabel("Label1"),
              IupSetAttributes(IupLabel("Label2"), "SIZE=70x"),
              IupLabel("Label3"),
              NULL
            )
          );

  frame2 = IupFrame
          (
            IupVbox
            (
              IupSetAttributes(IupLabel("Label4"), "EXPAND=HORIZONTAL"),
              IupLabel("Label5"),
              IupLabel("Label6"),
              NULL
            )
          );

  IupSetAttribute(frame1, "TITLE", "Title Text");
  IupSetAttribute(frame1, "MARGIN", "0x0");
//  IupSetAttribute(frame1, "FGCOLOR", "255 0 0");
//  IupSetAttribute(frame2, "SUNKEN", "YES");

  IupSetAttribute(frame2, "MARGIN", "0x0");
//  IupSetAttribute(frame2, "BGCOLOR", "0 128 0");
  dlg = IupDialog(IupHbox(frame1, frame2, NULL));

  IupSetAttribute(dlg, "TITLE", "IupFrame Test");
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "5");
  IupSetAttribute(dlg, "FONTSIZE", "14");
//  IupSetAttribute(dlg, "RASTERSIZE", "300x200");
//  IupSetAttribute(dlg, "BGCOLOR", "128 0 0");

  IupShow(dlg);
  printf("RASTERSIZE(%s)\n", IupGetAttribute(frame1, "RASTERSIZE"));
  printf("CLIENTSIZE(%s)\n", IupGetAttribute(frame1, "CLIENTSIZE"));
  printf("RASTERSIZE(%s)\n", IupGetAttribute(frame2, "RASTERSIZE"));
  printf("CLIENTSIZE(%s)\n", IupGetAttribute(frame2, "CLIENTSIZE"));
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  FrameTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
