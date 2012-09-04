#include <stdlib.h>
#include <stdio.h>
#include <iup.h>

int quit_cb(void)
{
  return IUP_CLOSE;
}

int main(int argc, char **argv)
{
  Ihandle *dialog, *quit_bt, *vbox;

  IupOpen(&argc, &argv);

  /* Creating the button */ 
  quit_bt = IupButton("Quit", 0);
  IupSetCallback(quit_bt, "ACTION", (Icallback)quit_cb);

  /* the container with a label and the button */
  vbox = IupVbox(
           IupSetAttributes(IupLabel("Very Long Text Label"), "EXPAND=YES, ALIGNMENT=ACENTER"), 
           quit_bt, 
           0);
  IupSetAttribute(vbox, "ALIGNMENT", "ACENTER");
  IupSetAttribute(vbox, "MARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "5");

  /* Creating the dialog */ 
  dialog = IupDialog(vbox);
  IupSetAttribute(dialog, "TITLE", "Dialog Title");
  IupSetAttributeHandle(dialog, "DEFAULTESC", quit_bt);

  IupShow(dialog);

  IupMainLoop();
  
  IupDestroy(dialog);
  IupClose();

  return EXIT_SUCCESS;

}
