#include <stdlib.h>
#include <stdio.h>

#include "iup.h"
#include "iupkey.h"


static Ihandle *lbl_h=NULL, *lbl_v=NULL;

static void print_value(Ihandle *ih, double a)
{
  char *type = IupGetAttribute(ih, "TYPE");

  switch(type[0])
  {
    case 'V':
      IupSetfAttribute(lbl_v, "TITLE", "VALUE=%.2f", a);
      break;
    case 'H':
      IupSetfAttribute(lbl_h, "TITLE", "VALUE=%.2f", a);
      break;
  }
}

static int mousemove(Ihandle *ih, double a)
{
  printf("mousemove %.2f\n", a);
  print_value(ih, a);
  return IUP_DEFAULT;
}

static int button_press(Ihandle *ih, double a)
{
  char *type = IupGetAttribute(ih, "TYPE");

  printf("button_press %.2f\n", a);

  switch(type[0])
  {
    case 'V':
      IupSetAttribute(lbl_v, "FGCOLOR", "255 0 0");
      break;

    case 'H':
      IupSetAttribute(lbl_h, "FGCOLOR", "255 0 0");
      break;
  }

  print_value(ih, a);

  return IUP_DEFAULT;
}

static int button_release(Ihandle *ih, double a)
{
  char *type = IupGetAttribute(ih, "TYPE");

  printf("button_release %.2f\n", a);

  switch(type[0])
  {
    case 'V':
      IupSetAttribute(lbl_v, "FGCOLOR", "0 0 0");
      break;

    case 'H':
      IupSetAttribute(lbl_h, "FGCOLOR", "0 0 0");
      break;
  }

  print_value(ih, a);

  return IUP_DEFAULT;
}

char *iupKeyCodeToName(int code);

static int k_any(Ihandle *ih, int c)
{
  if (iup_isprint(c))
    printf("K_ANY(%d = %s \'%c\')\n", c, iupKeyCodeToName(c), (char)c);
  else
    printf("K_ANY(%d = %s)\n", c, iupKeyCodeToName(c));
  return IUP_CONTINUE;
}

static int getfocus_cb(Ihandle *ih)
{
  printf("GETFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB()\n");
  return IUP_DEFAULT;
}
     
static int killfocus_cb(Ihandle *ih)
{
  printf("KILLFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int leavewindow_cb(Ihandle *ih)
{
  printf("LEAVEWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int enterwindow_cb(Ihandle *ih)
{
  printf("ENTERWINDOW_CB()\n");
  return IUP_DEFAULT;
}

void ValTest(void)
{
  Ihandle *dlg, *val_h, *val_v;

  lbl_v = IupLabel("VALUE=");
  IupSetAttribute(lbl_v, "SIZE", "70x");

  lbl_h = IupLabel("VALUE=");
  IupSetAttribute(lbl_h, "SIZE", "70x");

  val_v = IupVal("VERTICAL");
  IupSetAttribute(val_v, "MAX", "10.0");
  IupSetAttribute(val_v, "STEP", ".02");
  IupSetAttribute(val_v, "PAGESTEP", ".2");
  IupSetAttribute(val_v, "SHOWTICKS", "5");
//  IupSetAttribute(val_v, "TICKSPOS", "REVERSE");
  IupSetCallback(val_v, "HELP_CB",      (Icallback)help_cb);
  IupSetCallback(val_v, "GETFOCUS_CB",  (Icallback)getfocus_cb); 
  IupSetCallback(val_v, "KILLFOCUS_CB", (Icallback)killfocus_cb);
  IupSetCallback(val_v, "ENTERWINDOW_CB", (Icallback)enterwindow_cb);
  IupSetCallback(val_v, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);
  IupSetCallback(val_v, "K_ANY", (Icallback)k_any);
//  IupSetAttribute(val_v, "INVERTED", "NO");
  IupSetAttribute(val_v, "EXPAND", "VERTICAL");
  IupSetAttribute(val_v, "TIP", "Val Tip");

  val_h = IupVal("HORIZONTAL");
  IupSetAttribute(val_h, "MAX", "100.0");
//  IupSetAttribute(val_h, "SHOWTICKS", "10");
//  IupSetAttribute(val_h, "TICKSPOS", "BOTH");   /* Windows Only */
//  IupSetAttribute(val_h, "INVERTED", "YES");
  IupSetAttribute(val_h, "EXPAND", "HORIZONTAL");
//  IupSetAttribute(val_h, "CANFOCUS", "NO");
  
  dlg = IupDialog
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

  IupSetCallback(val_v, "BUTTON_PRESS_CB",  (Icallback) button_press);
  IupSetCallback(val_v, "BUTTON_RELEASE_CB",  (Icallback) button_release);
  IupSetCallback(val_v, "MOUSEMOVE_CB", (Icallback) mousemove); 

  IupSetCallback(val_h, "BUTTON_PRESS_CB",  (Icallback) button_press);
  IupSetCallback(val_h, "BUTTON_RELEASE_CB",  (Icallback) button_release);
  IupSetCallback(val_h, "MOUSEMOVE_CB", (Icallback) mousemove); 

  IupSetAttribute(dlg, "TITLE", "IupVal Test");
//  IupSetAttribute(IupGetChild(dlg, 0), "BGCOLOR", "50 50 255");
//  IupSetAttribute(IupGetChild(dlg, 0), "ACTIVE", "NO");
  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  ValTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
