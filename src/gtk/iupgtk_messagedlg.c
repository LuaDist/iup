/** \file
 * \brief GTK IupMessageDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"

#include "iupgtk_drv.h"

/* Sometimes GTK decides to invert the buttons position because of the GNOME Guidelines.
   To avoid that we define different Ids for the buttons. */
#define IUP_RESPONSE_1 -100
#define IUP_RESPONSE_2 -200
#define IUP_RESPONSE_HELP -300

#ifndef GTK_MESSAGE_OTHER
#define GTK_MESSAGE_OTHER GTK_MESSAGE_INFO
#endif

static int gtkMessageDlgPopup(Ihandle* ih, int x, int y)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  GtkMessageType type = GTK_MESSAGE_OTHER;
  GtkWidget* dialog;
  char *icon, *buttons, *title;
  int response, num_but = 2;

  iupAttribSetInt(ih, "_IUPDLG_X", x);   /* used in iupDialogUpdatePosition */
  iupAttribSetInt(ih, "_IUPDLG_Y", y);

  icon = iupAttribGetStr(ih, "DIALOGTYPE");
  if (iupStrEqualNoCase(icon, "ERROR"))
    type = GTK_MESSAGE_ERROR;
  else if (iupStrEqualNoCase(icon, "WARNING"))
    type = GTK_MESSAGE_WARNING;
  else if (iupStrEqualNoCase(icon, "INFORMATION"))
    type = GTK_MESSAGE_INFO;
  else if (iupStrEqualNoCase(icon, "QUESTION"))
    type = GTK_MESSAGE_QUESTION;

  dialog = gtk_message_dialog_new((GtkWindow*)parent,
                                  0,
                                  type,
                                  GTK_BUTTONS_NONE,
                                  "%s",
                                  iupgtkStrConvertToUTF8(iupAttribGet(ih, "VALUE")));
  if (!dialog)
    return IUP_ERROR;

  title = iupAttribGet(ih, "TITLE");
  if (title)
    gtk_window_set_title(GTK_WINDOW(dialog), iupgtkStrConvertToUTF8(title));

  buttons = iupAttribGetStr(ih, "BUTTONS");
  if (iupStrEqualNoCase(buttons, "OKCANCEL"))
  {
    gtk_dialog_add_button(GTK_DIALOG(dialog),
                          GTK_STOCK_OK,
                          IUP_RESPONSE_1);
    gtk_dialog_add_button(GTK_DIALOG(dialog),
                          GTK_STOCK_CANCEL,
                          IUP_RESPONSE_2);
  }
  else if (iupStrEqualNoCase(buttons, "YESNO"))
  {
    gtk_dialog_add_button(GTK_DIALOG(dialog),
                          GTK_STOCK_YES,
                          IUP_RESPONSE_1);
    gtk_dialog_add_button(GTK_DIALOG(dialog),
                          GTK_STOCK_NO,
                          IUP_RESPONSE_2);
  }
  else /* OK */
  {
    gtk_dialog_add_button(GTK_DIALOG(dialog),
                          GTK_STOCK_OK,
                          IUP_RESPONSE_1);
    num_but = 1;
  }

  if (IupGetCallback(ih, "HELP_CB"))
    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_HELP, IUP_RESPONSE_HELP);

  if (num_but == 2 && iupAttribGetInt(ih, "BUTTONDEFAULT") == 2)
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), IUP_RESPONSE_2);
  else
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), IUP_RESPONSE_1);
  
  /* initialize the widget */
  gtk_widget_realize(dialog);
  
  ih->handle = dialog;
  iupDialogUpdatePosition(ih);
  ih->handle = NULL;

  do 
  {
    response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == IUP_RESPONSE_HELP)
    {
      Icallback cb = IupGetCallback(ih, "HELP_CB");
      if (cb && cb(ih) == IUP_CLOSE)
        response = (num_but == 2)? IUP_RESPONSE_2: IUP_RESPONSE_1;
    }
  } while (response == IUP_RESPONSE_HELP);

  if (response == IUP_RESPONSE_1)
    IupSetAttribute(ih, "BUTTONRESPONSE", "1");
  else
    IupSetAttribute(ih, "BUTTONRESPONSE", "2");

  gtk_widget_destroy(dialog);  

  return IUP_NOERROR;
}

void iupdrvMessageDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = gtkMessageDlgPopup;
}
