/** \file
 * \brief IupFontDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>

#include <string.h>
#include <memory.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"

#include "iupgtk_drv.h"


static int gtkFontDlgPopup(Ihandle* ih, int x, int y)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  GtkFontSelectionDialog* dialog;
  int response;
  char* preview_text, *standardfont;

  iupAttribSetInt(ih, "_IUPDLG_X", x);   /* used in iupDialogUpdatePosition */
  iupAttribSetInt(ih, "_IUPDLG_Y", y);

  dialog = (GtkFontSelectionDialog*)gtk_font_selection_dialog_new(iupgtkStrConvertToUTF8(iupAttribGet(ih, "TITLE")));
  if (!dialog)
    return IUP_ERROR;

  if (parent)
    gtk_window_set_transient_for((GtkWindow*)dialog, (GtkWindow*)parent);

  standardfont = iupAttribGet(ih, "VALUE");
  if (!standardfont)
    standardfont = IupGetGlobal("DEFAULTFONT");
  gtk_font_selection_dialog_set_font_name(dialog, standardfont);

  preview_text = iupAttribGet(ih, "PREVIEWTEXT");
  if (preview_text)
    gtk_font_selection_dialog_set_preview_text(dialog, preview_text);

  if (IupGetCallback(ih, "HELP_CB"))
    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_HELP, GTK_RESPONSE_HELP);
  
  /* initialize the widget */
  gtk_widget_realize(GTK_WIDGET(dialog));
  
  ih->handle = GTK_WIDGET(dialog);
  iupDialogUpdatePosition(ih);
  ih->handle = NULL;
                                    
  do 
  {
    response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_HELP)
    {
      Icallback cb = IupGetCallback(ih, "HELP_CB");
      if (cb && cb(ih) == IUP_CLOSE)
        response = GTK_RESPONSE_CANCEL;
    }
  } while (response == GTK_RESPONSE_HELP);

  if (response == GTK_RESPONSE_OK)
  {
    char* fontname = gtk_font_selection_dialog_get_font_name(dialog);
    iupAttribStoreStr(ih, "VALUE", fontname);
    g_free(fontname);
    iupAttribSetStr(ih, "STATUS", "1");
  }
  else
  {
    iupAttribSetStr(ih, "VALUE", NULL);
    iupAttribSetStr(ih, "STATUS", NULL);
  }

  gtk_widget_destroy(GTK_WIDGET(dialog));  

  return IUP_NOERROR;
}

void iupdrvFontDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = gtkFontDlgPopup;

  /* IupFontDialog GTK Only */
  iupClassRegisterAttribute(ic, "PREVIEWTEXT", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
