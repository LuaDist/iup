/*IupText Example in C 
Creates a IupText that shows asterisks instead of characters (password-like).*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iup.h"
#include "iupkey.h"

Ihandle *text, *dlg, *pwd;
static char password[100];

static int k_any(Ihandle *self, int c)
{
  switch (c)
  {
  case K_BS:
    {
      int size = strlen(password);
      if (size==0)
        return IUP_IGNORE;
      password[size-1] = 0;
      IupSetAttribute(pwd, "VALUE", password);
      return IUP_DEFAULT;
    }
  case K_CR:
  case K_SP:
  case K_ESC:
  case K_INS:
  case K_DEL:
  case K_TAB:
  case K_HOME:
  case K_UP:
  case K_PGUP:
  case K_LEFT:
  case K_MIDDLE:
  case K_RIGHT:
  case K_END:
  case K_DOWN:
  case K_PGDN:
    return IUP_IGNORE;
  default:
    return IUP_DEFAULT;
  }
}

static int action(Ihandle *self, int c, char *after)
{
  if (c)
  {
    int size = strlen(password);
    password[size] = (char)c;
    password[size+1] = 0;
    IupSetAttribute(pwd, "VALUE", password);
  }
  return K_asterisk;
}

int main(int argc, char **argv)
{
  IupOpen(&argc, &argv);

  memset(password, 0, 100);

  text = IupText(NULL);
  IupSetAttribute(text, "SIZE",  "200x");
  IupSetCallback(text, "ACTION", (Icallback) action);
  IupSetCallback(text, "K_ANY", (Icallback) k_any);

  pwd = IupText(NULL);
  IupSetAttribute(pwd, "READONLY", "YES");
  IupSetAttribute(pwd, "SIZE", "200x");

  dlg = IupDialog(IupVbox(text, pwd, NULL));
  IupSetAttribute(dlg, "TITLE", "IupText");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();
  IupClose();
  return EXIT_SUCCESS;

}
