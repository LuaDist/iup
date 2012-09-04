#include <stdlib.h>
#include <stdio.h>

#include "iup.h"
#include "iupcontrols.h"

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB(%s)\n", IupGetName(ih));
  return IUP_DEFAULT;
}

static void printdial(Ihandle* ih, double a, char* color)
{
  Ihandle* label = NULL;
  char* u = NULL;
  char* type = IupGetAttribute(ih, "TYPE");

  switch(type[0])
  {
    case 'V':
      label = IupGetHandle("lbl_v");
      u = "deg";
      break;
    case 'H':
      label = IupGetHandle("lbl_h");
      u = "rad";
      break;
    case 'C':
      label = IupGetHandle("lbl_c");
      u = "deg";
      break;
  }

  if (label)
  {
    IupSetfAttribute(label, "TITLE", "%.3g %s", a, u);
    IupSetAttribute(label, "BGCOLOR", color);
  }
} 

static int mousemove(Ihandle* ih, double a)
{
  printdial(ih, a, "0 255 0");
  return IUP_DEFAULT;
} 

static int button_press(Ihandle* ih, double a)
{
  printdial(ih, a, "255 0 0");
  return IUP_DEFAULT;
} 

static int button_release(Ihandle* ih, double a)
{
  printdial(ih, a, NULL);
  return IUP_DEFAULT;
} 

void DialTest(void)
{
  char *error = NULL;
  Ihandle *dlg, *dial_h, *dial_v, *dial_c;

  error = IupLoad("dial.led");
  if (error)
  {
    error = IupLoad("../test/dial.led");
    if (error)
    {
      IupMessage("%s\n", error);
      return;
    }
  }

  dlg = IupGetHandle("dlg");
  dial_h = IupGetHandle("dial_h");
  dial_v = IupGetHandle("dial_v");
  dial_c = IupGetHandle("dial_c");

  IupSetFunction("dial_mousemove", (Icallback)mousemove);
  IupSetFunction("dial_buttonpress", (Icallback)button_press);
  IupSetFunction("dial_buttonrelease", (Icallback)button_release);

  IupSetCallback(dial_h, "HELP_CB", help_cb);
  IupSetCallback(dial_v, "HELP_CB", help_cb);

  IupSetAttribute(dial_h, "EXPAND", "HORIZONTAL");
  IupSetAttribute(dial_v, "EXPAND", "VERTICAL");

//  IupSetAttribute(dlg, "HELPBUTTON", "YES");
//  IupSetAttribute(dlg, "DIALOGFRAME", "YES");

  IupShow(dlg);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);
  IupControlsOpen();

  DialTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
