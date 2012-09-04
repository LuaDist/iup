/* IupSubmenu: Example in C 
   Creates a dialog with a menu with three submenus.
   One of the submenus has a submenu, which has another submenu.
*/

/* ANSI C libraries include */
#include <stdlib.h>
#include <stdio.h>

/* IUP libraries include */
#include <iup.h>

/******************************************************************************
 * Function:                                                                  *
 * Item help callback                                                         *
 *                                                                            *
 * Description:                                                               *
 * Shows a message saying that only Help and Exit items performs an operation *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int item_help_cb (void)
{
  IupMessage ("Warning", "Only Help and Exit items performs an operation");
  
  return IUP_DEFAULT;
}


/******************************************************************************
 * Function:                                                                  *
 * Item exit callback                                                         *
 *                                                                            *
 * Description:                                                               *
 * Exits the program                                                          *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int item_exit_cb (void)
{
  return IUP_CLOSE;
}

/* Main program */
int main(int argc, char **argv)
{
  /* IUP identifiers */
  Ihandle *dlg;
  Ihandle *text;
  Ihandle *menu, *menu_file, *menu_edit, *menu_help; 
  Ihandle *menu_create; 
  Ihandle *menu_triangle; 
  Ihandle *submenu_file, *submenu_edit, *submenu_help;
  Ihandle *submenu_create;
  Ihandle *submenu_triangle;
  Ihandle *item_new, *item_open, *item_close, *item_exit;
  Ihandle *item_copy, *item_paste;
  Ihandle *item_line, *item_circle;
  Ihandle *item_equilateral, *item_isoceles, *item_scalenus;
  Ihandle *item_help;
  
  /* Initializes IUP */
  IupOpen(&argc, &argv);

  /* Program begin */
  
  /* Creates a multiline */
  text = IupText (NULL);
  
  /* Sets value of the text and turns on expand */ 
  IupSetAttributes (text, "VALUE = \"This text is here only to compose\", EXPAND = YES");
     
  /* Creates items of menu file */
  item_new = IupItem ("New", NULL);
  item_open = IupItem ("Open", NULL);
  item_close = IupItem ("Close", NULL);
  item_exit = IupItem ("Exit", "item_exit_act");

  /* Creates items of menu edit */
  item_copy = IupItem ("Copy", NULL);
  item_paste = IupItem ("Paste", NULL);

  /* Creates items for menu triangle */
  item_scalenus = IupItem ("Scalenus", NULL);
  item_isoceles = IupItem ("Isoceles", NULL);
  item_equilateral = IupItem ("Equilateral", NULL);

  /* Create menu triangle */
  menu_triangle = IupMenu (item_equilateral, item_isoceles, item_scalenus, NULL);

  /* Creates submenu triangle */
  submenu_triangle = IupSubmenu ("Triangle", menu_triangle);

  /* Creates items for menu create */
  item_line = IupItem ("Line", NULL);
  item_circle = IupItem ("Circle", NULL);

  /* Creates menu create */
  menu_create = IupMenu (item_line, item_circle, submenu_triangle, NULL);

  /* Creates submenu create */
  submenu_create = IupSubmenu ("Create", menu_create);

  /* Creates items of menu help */
  item_help = IupItem ("Help", "item_help_act");
  
  /* Creates three menus */
  menu_file = IupMenu (item_new, item_open, item_close, IupSeparator(), item_exit, NULL);
  menu_edit = IupMenu (item_copy, item_paste, IupSeparator(), submenu_create, NULL);
  menu_help = IupMenu (item_help, NULL);

  /* Creates three submenus */
  submenu_file = IupSubmenu ("File", menu_file);
  submenu_edit = IupSubmenu ("Edit", menu_edit);
  submenu_help = IupSubmenu ("Help", menu_help);
  
  /* Creates main menu with file menu */
  menu = IupMenu (submenu_file, submenu_edit, submenu_help, NULL);

  /* Registers callbacks */
  IupSetCallback (item_help, "ACTION", (Icallback) item_help_cb);
  IupSetCallback (item_exit, "ACTION", (Icallback) item_exit_cb);
 
  /* Associates handle "menu" with menu */
  IupSetHandle ("menu", menu);
                                
  /* Creates dialog with a text */
  dlg = IupDialog (text);

  /* Sets title and size of the dialog and associates a menu to it */
  IupSetAttributes (dlg, "TITLE=\"IupSubmenu Example\", SIZE = QUARTERxEIGHTH, MENU = menu");

  /* Shows dialog in the center of the screen */
  IupShowXY (dlg, IUP_CENTER, IUP_CENTER);

  /* Initializes IUP main loop */
  IupMainLoop ();

  /* Finishes IUP */
  IupClose ();  

  /* Program finished successfully */
  return EXIT_SUCCESS;

}