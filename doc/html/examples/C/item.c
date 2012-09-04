#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iup.h>

int item_save_cb(void)
{
  IupMessage("Test","Saved!");
  return IUP_DEFAULT;
}

int item_autosave_cb(void)
{
  Ihandle *item_autosave = IupGetHandle("item_autosave");
  if (IupGetInt(item_autosave, "VALUE"))
  {
    IupMessage("Auto Save","OFF");
    IupSetAttribute(item_autosave, "VALUE", "OFF");
  }
  else
  {
    IupMessage("Auto Save","ON");
    IupSetAttribute(item_autosave, "VALUE", "ON");
  }
  
  return IUP_DEFAULT;
}

int item_exit_cb(void)
{
  return IUP_CLOSE;
}

int main(int argc, char **argv)
{
  Ihandle *dlg;
  Ihandle *text;
  Ihandle *menu, *menu_file;
  Ihandle *submenu_file;
  Ihandle *item_save, *item_autosave, *item_exit;

  IupOpen(&argc, &argv);
 
  text = IupText(NULL);

  IupSetAttribute(text, "VALUE", "This is an empty text");

  item_save = IupItem("Save\tCtrl+S", NULL);  /* this is NOT related with the Ctrl+S key callback, it will just align the text at right */
  item_autosave = IupItem("&Auto Save", NULL);
  item_exit = IupItem("Exit", "item_exit_act");

  IupSetAttribute(item_exit, "KEY", "K_x");   /* this is NOT related with the Ctrl+X key callback, it will just underline the letter */

  IupSetCallback(item_exit, "ACTION", (Icallback) item_exit_cb);
  IupSetCallback(item_autosave, "ACTION", (Icallback) item_autosave_cb);

  IupSetAttribute(item_autosave, "VALUE", "ON");
  IupSetHandle("item_autosave", item_autosave); /* giving a name to a iup handle */

  menu_file = IupMenu(item_save, item_autosave, item_exit, NULL);
  
  submenu_file = IupSubmenu("File", menu_file);
  
  menu = IupMenu(submenu_file, NULL);
 
  IupSetHandle("menu", menu);
                                
  dlg = IupDialog(IupVbox(text, IupButton("Test", NULL), NULL));

  IupSetAttribute(dlg, "MARGIN", "10x10");
  IupSetAttribute(dlg, "GAP", "10");
  IupSetAttribute(dlg, "TITLE", "IupItem");
  IupSetAttribute(dlg, "MENU", "menu");
  IupSetCallback(dlg, "K_cX", (Icallback) item_exit_cb);   /* this will also affect the IupText if at focus, since it is used for clipboard cut */
  IupSetCallback(dlg, "K_cA", (Icallback) item_autosave_cb);  /* this will also affect the IupText if at focus, since it is used for select all */
  IupSetCallback(dlg, "K_cS", (Icallback) item_save_cb);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;

}
