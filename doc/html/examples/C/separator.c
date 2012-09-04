/* IupSeparator: Example in C 
   Creates a dialog with a menu and some items. A IupSeparator was used to separate the menu items.
*/

/* ANSI C libraries include */
#include <stdlib.h>
#include <stdio.h>

/* IUP libraries include */
#include <iup.h>

/******************************************************************************
 * Function:                                                                  *
 * Show message                                                               *
 *                                                                            *
 * Description:                                                               *
 * Shows a message saying that the this item performs no operation            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
void show_message (void)
{
  IupMessage ("Warning", "This item performs no operation\nIt is here only to compose");
}

/******************************************************************************
 * Function:                                                                  *
 * Item new callback                                                          *
 *                                                                            *
 * Description:                                                               *
 * Shows a message saying that the this item performs no operation            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int item_new_cb (void)
{
  show_message ();
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Item open callback                                                         *
 *                                                                            *
 * Description:                                                               *
 * Shows a message saying that the this item performs no operation            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int item_open_cb (void)
{
  show_message ();
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Item close callback                                                        *
 *                                                                            *
 * Description:                                                               *
 * Shows a message saying that the this item performs no operation            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int item_close_cb (void)
{
  show_message ();
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Item page setup callback                                                   *
 *                                                                            *
 * Description:                                                               *
 * Shows a message saying that the this item performs no operation            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int item_pagesetup_cb (void)
{
  show_message ();
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Item print callback                                                        *
 *                                                                            *
 * Description:                                                               *
 * Shows a message saying that the this item performs no operation            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int item_print_cb (void)
{
  show_message ();
  
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
  Ihandle *menu, *menu_file; 
  Ihandle *submenu_file;
  Ihandle *item_new, *item_open, *item_close, *item_pagesetup, *item_print, *item_exit;

  /* Initializes IUP */
  IupOpen(&argc, &argv);

  /* Program begin */

  /* Creates a text */
  text = IupText (NULL);
  
  /* Sets value of the text and turns on expand */ 
  IupSetAttributes (text, "VALUE = \"This text is here only to compose\", EXPAND = HORIZONTAL");
 
  /* Creates six items */
  item_new = IupItem ("New", NULL);
  item_open = IupItem ("Open", NULL);
  item_close = IupItem ("Close", NULL);
  item_pagesetup = IupItem ("Page Setup", NULL);
  item_print = IupItem ("Page Setup", NULL);
  item_exit = IupItem ("Exit", NULL);

  /* Registers callbacks */
  IupSetCallback (item_new, "ACTION", (Icallback) item_new_cb);
  IupSetCallback (item_open, "ACTION", (Icallback) item_open_cb);
  IupSetCallback (item_close, "ACTION", (Icallback) item_close_cb);
  IupSetCallback (item_pagesetup, "ACTION", (Icallback) item_pagesetup_cb);
  IupSetCallback (item_print, "ACTION", (Icallback) item_print_cb);
  IupSetCallback (item_exit, "ACTION", (Icallback) item_exit_cb);

  /* Creates file menu */
  menu_file = IupMenu (item_new, item_open, item_close, IupSeparator(), item_pagesetup, item_print, IupSeparator(), item_exit, NULL);

  /* Creates file submenu */
  submenu_file = IupSubmenu ("File", menu_file);
  
  /* Creates main menu with file menu */
  menu = IupMenu (submenu_file, NULL);
 
  /* Associates handle "menu" with menu */
  IupSetHandle ("menu", menu);
                                
  /* Creates dialog with a text */
  dlg = IupDialog (text);

  /* Sets title and size of the dialog and associates a menu to it */
  IupSetAttributes (dlg, "TITLE=\"IupSeparator Example\", SIZE = QUARTERxEIGHTH, MENU = menu");

  /* Shows dialog in the center of the screen */
  IupShowXY (dlg, IUP_CENTER, IUP_CENTER);

  /* Initializes IUP main loop */
  IupMainLoop ();

  /* Finishes IUP */
  IupClose ();  

  /* Program finished successfully */
  return EXIT_SUCCESS;

}