/*IupRadio Example in C 
Creates a dialog for the user to select his/her gender. 
In this case, the radio element is essential to prevent the user from selecting both options. */

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"

int main(int argc, char **argv)
{
  Ihandle *male, *female, *exclusive, *frame, *dialog;

  IupOpen(&argc, &argv);

  male	= IupToggle ("Male", "");
  female = IupToggle ("Female", "");

  exclusive = IupRadio
  (
    IupVbox
    (
      male,
      female,
      NULL
    )
  );
  IupSetHandle("male", male);
  IupSetHandle("female", female);
  IupSetAttribute(exclusive, "VALUE", "female");
  IupSetAttribute(male, "TIP",   "Two state button - Exclusive - RADIO");
  IupSetAttribute(female, "TIP",   "Two state button - Exclusive - RADIO");

  frame = IupFrame
  (
    exclusive
  );
  IupSetAttribute (frame, "TITLE", "Gender");

  dialog = IupDialog
  (
    IupHbox
    (
      IupFill(),
      frame,
      IupFill(),
      NULL
    )
  );

  IupSetAttributes(dialog, "SIZE=140, TITLE=IupRadio, RESIZE=NO, MINBOX=NO, MAXBOX=NO");

  IupShow(dialog);
  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;
 
}