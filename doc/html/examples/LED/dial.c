/* IupDial Example in C */

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "iupcontrols.h"

static void printdial(Ihandle *c, double a, char* color)
{
  char *type=NULL;
  Ihandle* label = NULL;

  type = IupGetAttribute(c, "ORIENTATION");

  switch(type[0])
  {
  case 'V':
    label = IupGetHandle("lbl_v");
    break;

  case 'H':
    label = IupGetHandle("lbl_h");
    break;

  case 'C':
    label = IupGetHandle("lbl_c");
    break;
  }

  if (label)
  {
    IupSetfAttribute(label, "TITLE", "%.3g", a);
    IupSetAttribute(label, "BGCOLOR", color);
  }
} 

static int mousemove(Ihandle *c, double a)
{
  printdial(c, a, "0 255 0");
  return IUP_DEFAULT;
} 

static int button_press(Ihandle *c,double a)
{
  printdial(c, a, "255 0 0");
  return IUP_DEFAULT;
} 

static int button_release(Ihandle *c,double a)
{
  printdial(c, a, NULL);
  return IUP_DEFAULT;
} 


void main(int argc, char* argv[])
{
  char *error=NULL;
  Ihandle *dlg=NULL,
    *dial_h=NULL,
    *dial_v=NULL,
    *dial_c=NULL;

  IupOpen(&argc, &argv);
  IupControlsOpen () ;

  if((error = IupLoad("iupdial.led")) != NULL)
  {
    IupMessage("%s\n", error);
    return;
  }

  dlg = IupGetHandle("dlg");
  dial_h = IupGetHandle("dial_h");
  dial_v = IupGetHandle("dial_v");
  dial_c = IupGetHandle("dial_c");

  /* sets mouse-movement callback */

  IupSetFunction("mousemove", (Icallback) mousemove);
  IupSetFunction("button_press", (Icallback) button_press);
  IupSetFunction("button_release", (Icallback) button_release);

  IupSetAttribute(dial_v, ICTL_BUTTON_PRESS_CB, "button_press"); 
  IupSetAttribute(dial_c, ICTL_BUTTON_PRESS_CB, "button_press"); 
/*IupSetAttribute(dial_h, ICTL_BUTTON_PRESS_CB, "button_press"); This is done in LED */

  IupSetAttribute(dial_v, ICTL_BUTTON_RELEASE_CB, "button_release"); 
  IupSetAttribute(dial_h, ICTL_BUTTON_RELEASE_CB, "button_release"); 
/*IupSetAttribute(dial_c, ICTL_BUTTON_RELEASE_CB, "button_release"); This is done in LED */

  IupSetAttribute(dial_c, ICTL_MOUSEMOVE_CB, "mousemove"); 
  IupSetAttribute(dial_h, ICTL_MOUSEMOVE_CB, "mousemove"); 
/*IupSetAttribute(dial_v, ICTL_MOUSEMOVE_CB, "mousemove"); This is done in LED */

  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);

  IupMainLoop();

  IupDestroy(dlg);

  IupControlsClose() ;
  IupClose();  
}
