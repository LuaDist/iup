/* IupMultiline: Simple Example in C 
   Shows a multiline that ignores the treatment of the DEL key, canceling its effect.
*/

#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "iupkey.h"

int mlaction(Ihandle *self, int c, char* after)
{
  if (c == K_g)
    return IUP_IGNORE;
  else
    return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *ml;

  IupOpen(&argc, &argv);

  ml = IupMultiLine(NULL);
  IupSetCallback(ml, "ACTION", (Icallback)mlaction);
  IupSetAttribute(ml, "EXPAND", "YES");
  IupSetAttribute(ml, "VALUE",  "I ignore the \"g\" key!");
  IupSetAttribute(ml, "BORDER", "YES");

  dlg = IupDialog(ml);
  IupSetAttribute(dlg, "TITLE", "IupMultiline");
  IupSetAttribute(dlg, "SIZE",  "QUARTERxQUARTER");

  IupShow(dlg);
  IupMainLoop();
  IupClose();
  return EXIT_SUCCESS;
}
