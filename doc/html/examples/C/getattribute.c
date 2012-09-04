/*
  IupGetAttribute/Int/Float: Example in C 
  
  Uses IupGetAttribute/IupGetInt and IupGetFloat to print a number entered by the user.
  The printing will occur when the user exits the program. The number will be printed as an integer,
  a floating point and a text.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "iup.h"
#include "iupkey.h"

int k_any(Ihandle *self, int c)
{
  if (c == K_CR)
    return IUP_CLOSE;
  return IUP_CONTINUE;
}

int number_action (Ihandle *self, int c)
{
  static char caracteres_validos[]="0123456789.+-Ee";

  if (strchr(caracteres_validos, c))  /* c is a valid character */
    return IUP_DEFAULT;
  else if (iscntrl(c))  /* c is a control character (TAB, BACKSPACE, ...) */
    return IUP_DEFAULT;

  return IUP_IGNORE;
}

int main(int argc, char **argv)
{
  Ihandle *field;    /* TEXT interface element */
  Ihandle *message; /* LABEL interface element */
  Ihandle *prompt;   /* box containing the message and the field */
  Ihandle* dlg;

  IupOpen(&argc, &argv);  /* initializes IUP */

  field = IupText(NULL);                           /* creates TEXT field */
  IupSetCallback(field, "ACTION", (Icallback)number_action);   /* registers callback */
  IupSetCallback(field, "K_ANY", (Icallback)k_any);   /* registers callback */
  
  IupSetAttribute(field, "VALUE", "1.0");                /* defines initial value */

  message = IupLabel("Please, type any number: "); /* creates message */
  prompt = IupHbox(message, field, NULL);  /* creates prompt */

  dlg = IupDialog(prompt);
  IupShow(dlg); /* displays and creates dialog */
  IupMainLoop ();               /* interacts with user */

  printf ("Integer number: %d\n", IupGetInt (field, "VALUE"));   /* prints the integer number entered by the user */
  printf ("Real number...: %g\n", IupGetFloat (field, "VALUE"));       /* prints the real number entered by the user */
  printf ("Text entered..: %s\n", IupGetAttribute(field, "VALUE"));  /* prints the text entered by the user */

  IupClose ();  /* ends IUP */  
  return EXIT_SUCCESS;
}
