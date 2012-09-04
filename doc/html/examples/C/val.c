/*
  Program to exemplify the IupVal element.
*/
#include <stdlib.h>
#include <stdio.h>

#include "iup.h"

Ihandle *lbl_h=NULL, *lbl_v=NULL;


static int mousemove(Ihandle *c,double a)
{
  char buffer[40];
  char *type=NULL;

  sprintf(buffer, "VALUE=%.2g", a);

  type = IupGetAttribute(c, "ORIENTATION");

  switch(type[0])
  {
    case 'V':
      IupStoreAttribute(lbl_v, "TITLE", buffer);
      break;

    case 'H':
      IupStoreAttribute(lbl_h, "TITLE", buffer);
      break;
  }

  return IUP_DEFAULT;
}

static int button_press(Ihandle *c,double a)
{
  char *type = IupGetAttribute(c, "ORIENTATION");

  switch(type[0])
  {
    case 'V':
      IupSetAttribute(lbl_v, "FGCOLOR", "255 0 0");
      break;

    case 'H':
      IupSetAttribute(lbl_h, "FGCOLOR", "255 0 0");
      break;
  }

  mousemove(c, a);

  return IUP_DEFAULT;
}

static int button_release(Ihandle *c,double a)
{
  char *type = IupGetAttribute(c, "ORIENTATION");

  switch(type[0])
  {
    case 'V':
      IupSetAttribute(lbl_v, "FGCOLOR", "0 0 0");
      break;

    case 'H':
      IupSetAttribute(lbl_h, "FGCOLOR", "0 0 0");
      break;
  }

  mousemove(c, a);

  return IUP_DEFAULT;
}


int main(int argc, char* argv[])
{
  Ihandle *dlg_val, *val_h, *val_v;

  IupOpen(&argc, &argv);

  val_v = IupVal("VERTICAL");
  val_h = IupVal("HORIZONTAL");
  lbl_v = IupLabel("VALUE=");
  lbl_h = IupLabel("VALUE=");
  IupSetAttribute(lbl_v, "SIZE", "50x");
  IupSetAttribute(lbl_h, "SIZE", "50x");
  IupSetAttribute(val_v, "SHOWTICKS", "5");

  dlg_val = IupDialog
  (
    IupHbox
    (
        IupSetAttributes(IupHbox
        (
          val_v,
          lbl_v,
          NULL
        ), "ALIGNMENT=ACENTER"),
        IupSetAttributes(IupVbox
        (
          val_h,
          lbl_h,
          NULL
        ), "ALIGNMENT=ACENTER"),
      NULL
    )
  );

  IupSetCallback(val_v, "BUTTON_PRESS_CB",  (Icallback)button_press);
  IupSetCallback(val_v, "BUTTON_RELEASE_CB",  (Icallback)button_release);
  IupSetCallback(val_v, "MOUSEMOVE_CB", (Icallback)mousemove); 

  IupSetCallback(val_h, "BUTTON_PRESS_CB",  (Icallback)button_press);
  IupSetCallback(val_h, "BUTTON_RELEASE_CB",  (Icallback)button_release);
  IupSetCallback(val_h, "MOUSEMOVE_CB", (Icallback)mousemove); 

  IupSetAttribute(dlg_val, "TITLE", "IupVal");
  IupSetAttribute(dlg_val, "MARGIN", "10x10");
  IupShowXY(dlg_val,IUP_CENTER,IUP_CENTER);

  IupMainLoop();
  IupClose();
  return EXIT_SUCCESS;
}
