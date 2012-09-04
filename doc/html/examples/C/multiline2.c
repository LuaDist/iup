/*IupMultiline Advanced Example in C 
Shows a dialog with a multiline, a text, a list and some buttons. You can test the multiline attributes by clicking on the buttons. Each button is related to an attribute. Select if you want to set or get an attribute using the dropdown list. The value in the text will be used as value when a button is pressed. */

/* IupMultiLine example */

/* ANSI C libraries include */
#include <stdlib.h>
#include <stdio.h>

/* IUP libraries include */
#include <iup.h>

/******************************************************************************
 * Function:                                                                  *
 * Set Attribute                                                              *
 *                                                                            *
 * Description:                                                               *
 * Sets an attribute with the value in the text                               *
 *                                                                            *
 * Parameters received:                                                       *
 * attribute - attribute to be set                                            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int set_attribute (char* attribute)
{
  char string_message [50]; 
  Ihandle *mltline;
  Ihandle *text;

  mltline = IupGetHandle ("mltline");
  text = IupGetHandle ("text");
  IupSetAttribute (mltline, attribute, IupGetAttribute (text, "VALUE"));

  sprintf (string_message, "Attribute %s set with value %s", attribute, IupGetAttribute (text, "VALUE"));

  IupMessage ("Set attribute", string_message);

  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Get attribute                                                              *
 *                                                                            *
 * Description:                                                               *
 * Get an attribute of the multiline and shows it in the text                 *
 *                                                                            *
 * Parameters received:                                                       *
 * attribute - attribute to be get                                            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int get_attribute (char* attribute)
{
  char string_message [50]; 
  Ihandle *mltline;
  Ihandle *text;

  mltline = IupGetHandle ("mltline");
  text = IupGetHandle ("text");
  IupSetAttribute (text, "VALUE", IupGetAttribute(mltline, attribute));

  sprintf (string_message, "Attribute %s get with value %s", attribute, IupGetAttribute (text, "VALUE"));

  IupMessage ("Get attribute", string_message);

  return IUP_DEFAULT;
}

/******************************************************************************
 * Functions:                                                                 *
 * Append button callback                                                     *
 *                                                                            *
 * Description:                                                               *
 * Appends text to the multiline. Value: text to be appended                  *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_append_cb (void)
{
  Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("APPEND"); else get_attribute ("APPEND");

  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Insert button callback                                                     *
 *                                                                            *
 * Description:                                                               *
 * Inserts text in the multiline. Value: text to be inserted                  *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_insert_cb (void)
{
  Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("INSERT"); else get_attribute ("INSERT");
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Border button callback                                                     *
 *                                                                            *
 * Description:                                                               *
 * Border of the multiline. Value: "YES" or "NO"                              *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_border_cb (void)
{
   Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("BORDER"); else get_attribute ("BORDER");
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Caret button callback                                                      *
 *                                                                            *
 * Description:                                                               *
 * Position of the caret. Value: lin,col                                      *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_caret_cb (void)
{
    Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("CARET"); else get_attribute ("CARET");
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Read-only button callback                                                  *
 *                                                                            *
 * Description:                                                               *
 * Readonly attribute. Value: "YES" or "NO"                                   *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_readonly_cb (void)
{
  Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("READONLY"); else get_attribute ("READONLY");
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Selection button callback                                                  *
 *                                                                            *
 * Description:                                                               *
 * Changes the selection attribute. Value: lin1,col1:lin2,col2                *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_selection_cb (void)
{
  Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("SELECTION"); else get_attribute ("SELECTION");
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Selected text button callback                                              *
 *                                                                            *
 * Description:                                                               *
 * Changes the selected text attribute. Value: lin1,col1:lin2,col2            *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_selectedtext_cb (void)
{
  Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("SELECTEDTEXT"); else get_attribute ("SELECTEDTEXT");
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Number of characters button callback                                       *
 *                                                                            *
 * Description:                                                               *
 * Limit number of characters in the multiline                                *
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_nc_cb (void)
{
  Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("NC"); else get_attribute ("NC");
  
  return IUP_DEFAULT;
}

/******************************************************************************
 * Function:                                                                  *
 * Vaue button callback                                                       *
 *                                                                            *
 * Description:                                                               *
 * Text in the multiline.
 *                                                                            *
 * Value returned:                                                            *
 * IUP_DEFAULT                                                                *
 ******************************************************************************/
int btn_value_cb (void)
{
  Ihandle *list;

  list = IupGetHandle ("list");

  if (IupGetInt (list, "VALUE") == 1) set_attribute ("VALUE"); else get_attribute ("VALUE");
  
  return IUP_DEFAULT;
}

/* Main program */
int main(int argc, char **argv)
{
 /* IUP identifiers */
  Ihandle *dlg;
  Ihandle *mltline;
  Ihandle *text;
  Ihandle *list;
  Ihandle *btn_append, *btn_insert, *btn_border, *btn_caret, *btn_readonly, *btn_selection, *btn_selectedtext, *btn_nc, *btn_value;

  /* Initializes IUP */
  IupOpen(&argc, &argv);

  /* Program begin */
 
  /* Creates a multiline, a text and a list*/ 
  mltline = IupMultiLine (NULL);  
  text = IupText (NULL);
  list = IupList (NULL);

  /* Turns on multiline expand ans text horizontal expand */
  IupSetAttribute (mltline, "EXPAND", "YES");
  IupSetAttribute (text, "EXPAND", "HORIZONTAL");

  /* Associates handles to multiline, text and list */
  IupSetHandle ("mltline", mltline);
  IupSetHandle ("text", text);
  IupSetHandle ("list", list);

  /* Sets list items and dropdown */
  IupSetAttributes (list, "1 = SET, 2 = GET, DROPDOWN = YES");

  /* Creates buttons */
  btn_append = IupButton ("Append", NULL);
  btn_insert = IupButton ("Insert", NULL);
  btn_border = IupButton ("Border", NULL);
  btn_caret = IupButton ("Caret", NULL);
  btn_readonly = IupButton ("Read only", NULL);
  btn_selection = IupButton ("Selection", NULL);
  btn_selectedtext = IupButton ("Selected Text", NULL);
  btn_nc = IupButton ("Number of characters", NULL);
  btn_value = IupButton ("Value", NULL);

  /* Registers callbacks */
  IupSetCallback(btn_append, "ACTION", (Icallback) btn_append_cb);
  IupSetCallback(btn_insert, "ACTION", (Icallback) btn_insert_cb);
  IupSetCallback(btn_border, "ACTION", (Icallback) btn_border_cb);
  IupSetCallback(btn_caret, "ACTION", (Icallback) btn_caret_cb);
  IupSetCallback(btn_readonly, "ACTION", (Icallback) btn_readonly_cb);
  IupSetCallback(btn_selection, "ACTION", (Icallback) btn_selection_cb);
  IupSetCallback(btn_selectedtext, "ACTION", (Icallback) btn_selectedtext_cb);
  IupSetCallback(btn_nc, "ACTION", (Icallback) btn_nc_cb);
  IupSetCallback(btn_value, "ACTION", (Icallback) btn_value_cb);

  /* Creates dialog */
  dlg = IupDialog (IupVbox (mltline, 
                   IupHbox (text, list, NULL),
                   IupHbox (btn_append, btn_insert, btn_border, btn_caret, btn_readonly, btn_selection, NULL),
                   IupHbox (btn_selectedtext, btn_nc, btn_value, NULL), 
                   NULL));

  /* Sets title and size of the dialog */
  IupSetAttributes (dlg, "TITLE=\"IupMultiLine Example\", SIZE=HALFxQUARTER");

  /* Shows dialog in the center of the screen */
  IupShowXY (dlg, IUP_CENTER, IUP_CENTER);

  /* Initializes IUP main loop */
  IupMainLoop ();

  /* Finishes IUP */
  IupClose ();

  /* Program finished successfully */
  return EXIT_SUCCESS;

}