/* IupTree: Example in C 
   Creates a tree with some branches and leaves. 
   Two callbacks are registered: one deletes marked nodes when the Del key is pressed, 
   and the other, called when the right mouse button is pressed, opens a menu with options.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iup.h>
#include <iupkey.h>
#include <iupcontrols.h>

/* Callback called when a leaf is added by the menu. */
int addleaf(void)
{
  static char attr[10];
  Ihandle* tree = IupGetHandle("tree");
  int id = IupGetInt(tree,"VALUE");
  sprintf(attr,"ADDLEAF%d",id);
  IupSetAttribute(tree,attr,"");
  return IUP_DEFAULT;
}

/* Callback called when a branch is added by the menu. */
int addbranch(void)
{
  static char attr[10];
  Ihandle* tree = IupGetHandle("tree");
  int id = IupGetInt(tree,"VALUE");
  sprintf(attr,"ADDBRANCH%d",id);
  IupSetAttribute(tree,attr,"");
  return IUP_DEFAULT;
}

/* Callback called when a node is removed by the menu. */
int removenode(void)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree,"DELNODE","MARKED");
  return IUP_DEFAULT;
}

/* from the menu */
int renamenode(void)
{
  return IUP_DEFAULT;
}

int executeleaf_cb(Ihandle* h, int id)
{
  printf("executeleaf_cb (%d)\n", id);
  return IUP_DEFAULT;
}

int rename_cb(Ihandle* h, int id, char* name)
{
  printf("rename_cb (%d=%s)\n", id, name);
  if (strcmp(name, "fool") == 0)
    return IUP_IGNORE;
  return IUP_DEFAULT;
}

int branchopen_cb(Ihandle* h, int id)
{
  printf("branchopen_cb (%d)\n", id);
  return IUP_DEFAULT;
}

int branchclose_cb(Ihandle* h, int id)
{
  printf("branchclose_cb (%d)\n", id);
  return IUP_DEFAULT;
}

int dragdrop_cb(Ihandle* h, int drag_id, int drop_id, int isshift, int iscontrol)
{
  printf("dragdrop_cb (%d)->(%d)\n", drag_id, drop_id);
  return IUP_DEFAULT;
}

/* Callback called when a key is hit */
int k_any_cb(Ihandle* h, int c)
{
  if (c == K_DEL) 
  {
    IupSetAttribute(h,"DELNODE","MARKED");
  }

  return IUP_DEFAULT;
}

int selectnode(Ihandle* h)
{
  Ihandle* tree = IupGetHandle("tree");
  IupSetAttribute(tree,"VALUE",IupGetAttribute(h, "TITLE"));
  return IUP_DEFAULT;
}

/* Callback called when the right mouse button is pressed */
int rightclick_cb(Ihandle* h, int id)
{
  Ihandle *popup_menu;

  static char id_string[10];

  popup_menu = IupMenu(
    IupItem ("Add Leaf","addleaf"),
    IupItem ("Add Branch","addbranch"),
    IupItem ("Rename Node","renamenode"),
    IupItem ("Remove Node","removenode"),
    IupSubmenu("Selection", IupMenu(
      IupItem ("ROOT", "selectnode"),
      IupItem ("LAST", "selectnode"),
      IupItem ("PGUP", "selectnode"),
      IupItem ("PGDN", "selectnode"),
      IupItem ("NEXT", "selectnode"),
      IupItem ("PREVIOUS", "selectnode"),
      IupSeparator(),
      IupItem ("INVERT", "selectnode"),
      IupItem ("BLOCK", "selectnode"),
      IupItem ("CLEARALL", "selectnode"),
      IupItem ("MARKALL", "selectnode"),
      IupItem ("INVERTALL", "selectnode"),
      NULL)),
    NULL);
    
  IupSetFunction("selectnode", (Icallback) selectnode);
  
  IupSetFunction("addleaf", (Icallback)addleaf);
  IupSetFunction("addbranch", (Icallback)addbranch);
  IupSetFunction("removenode", (Icallback)removenode);
  IupSetFunction("renamenode", (Icallback)renamenode);

  sprintf(id_string,"%d",id);
  IupSetAttribute(h, "VALUE", id_string);
  IupPopup(popup_menu,IUP_MOUSEPOS,IUP_MOUSEPOS);

  IupDestroy(popup_menu);

  return IUP_DEFAULT;
}

/* Initializes IupTree and registers callbacks */
void init_tree(void)
{
  Ihandle* tree = IupTree(); 

  IupSetCallback(tree, "EXECUTELEAF_CB", (Icallback) executeleaf_cb);
  IupSetCallback(tree, "RENAME_CB", (Icallback) rename_cb);
  IupSetCallback(tree, "BRANCHCLOSE_CB", (Icallback) branchclose_cb);
  IupSetCallback(tree, "BRANCHOPEN_CB", (Icallback) branchopen_cb);
  IupSetCallback(tree, "DRAGDROP_CB", (Icallback) dragdrop_cb);
  IupSetCallback(tree, "RIGHTCLICK_CB", (Icallback) rightclick_cb);
  IupSetCallback(tree, "K_ANY", (Icallback) k_any_cb);

//  IupSetAttribute(tree, "FONT","COURIER_NORMAL");
  //IupSetAttribute(tree, "CTRL","YES");
  //IupSetAttribute(tree, "SHIFT","YES");
  //IupSetAttribute(tree, "ADDEXPANDED", "NO");
  //IupSetAttribute(tree, "SHOWDRAGDROP", "YES");
  IupSetAttribute(tree, "SHOWRENAME", "YES");

  IupSetHandle("tree",tree);
}

/* Initializes the dialog */
void init_dlg(void)
{
  Ihandle* tree = IupGetHandle("tree");
  Ihandle* box = IupVbox(IupHbox(tree, IupButton("test", NULL), NULL), NULL);
  Ihandle* dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "IupTree");
  IupSetAttribute(box, "MARGIN", "20x20");
  IupSetHandle("dlg",dlg);
}

/* Initializes the IupTree’s attributes */
void init_tree_atributes(void)
{
  Ihandle* tree = IupGetHandle("tree");

  IupSetAttribute(tree, "NAME","Figures");
  IupSetAttribute(tree, "ADDBRANCH","3D");
  IupSetAttribute(tree, "ADDBRANCH","2D");
  IupSetAttribute(tree, "ADDLEAF","test");
  IupSetAttribute(tree, "ADDBRANCH1","parallelogram");
  IupSetAttribute(tree, "ADDLEAF2","diamond");
  IupSetAttribute(tree, "ADDLEAF2","square");
  IupSetAttribute(tree, "ADDBRANCH1","triangle");
  IupSetAttribute(tree, "ADDLEAF2","scalenus");
  IupSetAttribute(tree, "ADDLEAF2","isoceles");
  IupSetAttribute(tree, "ADDLEAF2","equilateral");
  IupSetAttribute(tree, "VALUE","6");
}

/* Main program */
int main(int argc, char **argv)
{
  Ihandle* dlg;
  
  IupOpen(&argc, &argv);                            /* IUP initialization */

  init_tree();                          /* Initializes IupTree */
  init_dlg();                           /* Initializes the dialog */
  dlg = IupGetHandle("dlg");            /* Retrieves the dialog handle */
  IupShowXY(dlg,IUP_CENTER,IUP_CENTER); /* Displays the dialog */
  init_tree_atributes();                /* Initializes attributes, can be done here or anywhere */

  IupMainLoop();                         /* Main loop */
  IupClose();                            /* Ends IUP */

  return EXIT_SUCCESS;

}
