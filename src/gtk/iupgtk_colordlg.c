/** \file
 * \brief IupColorDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>

#include <string.h>
#include <memory.h>
#include <stdio.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"

#include "iupgtk_drv.h"


static char* gtkColorDlgPaletteToString(const char* palette)
{
  char iup_str[20], *gtk_str, *palette_p;
  char* str = iupStrGetMemory(300);
  int off = 0, inc;
  GdkColor color;

  gtk_str = iupStrDup(palette);
  iupStrReplace(gtk_str, ':', 0);

  while (palette && *palette)
  {
    if (!gdk_color_parse (gtk_str, &color))
      return NULL;

    inc = sprintf(iup_str, "%d %d %d;", (int)iupCOLOR16TO8(color.red), (int)iupCOLOR16TO8(color.green), (int)iupCOLOR16TO8(color.blue));
    memcpy(str+off, iup_str, inc);
    off += inc;
    palette_p = strchr(palette, ':');
    if (palette_p) 
    {
      palette_p++;
      gtk_str += palette_p-palette;
    }
    palette = palette_p;
  }
  str[off-1] = 0;  /* remove last separator */
  return str;
}

static void gtkColorDlgGetPalette(Ihandle* ih, GtkColorSelection* colorsel)
{
  char *palette, *str;

  GtkSettings *settings = gtk_widget_get_settings(GTK_WIDGET(colorsel));
  g_object_get(settings, "gtk-color-palette", &palette, NULL);
  
  str = gtkColorDlgPaletteToString(palette);
  if (str) iupAttribStoreStr(ih, "COLORTABLE", str);
  g_free(palette);
}

static char* gtkColorDlgStringToPalette(const char* str)
{
  char gtk_str[20];
  char* palette = iupStrGetMemory(200);
  int off = 0;
  unsigned char r, g, b;

  while (str && *str)
  {
    if (!iupStrToRGB(str, &r, &g, &b))
      return NULL;

    sprintf(gtk_str, "#%02X%02X%02X:", (int)r, (int)g, (int)b);
    memcpy(palette+off, gtk_str, 8);
    off += 8;
    str = strchr(str, ';');
    if (str) str++;
  }
  palette[off-1] = 0;  /* remove last separator */
  return palette;
}

static void gtkColorDlgSetPalette(GtkColorSelection* colorsel, char* str)
{
  GtkSettings *settings = gtk_widget_get_settings(GTK_WIDGET(colorsel));
  gchar* palette = gtkColorDlgStringToPalette(str);
  if (palette)
    gtk_settings_set_string_property(settings,
                                     "gtk-color-palette",
                                     palette,
                                     "gtk_color_selection_palette_to_string");
}

static int gtkColorDlgPopup(Ihandle* ih, int x, int y)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  GtkColorSelectionDialog* dialog;
  GtkColorSelection* colorsel;
  GdkColor color;
  char *value;
  unsigned char r = 0, g = 0, b = 0, a = 255;
  int response, ret;

  iupAttribSetInt(ih, "_IUPDLG_X", x);   /* used in iupDialogUpdatePosition */
  iupAttribSetInt(ih, "_IUPDLG_Y", y);

  dialog = (GtkColorSelectionDialog*)gtk_color_selection_dialog_new(iupgtkStrConvertToUTF8(iupAttribGet(ih, "TITLE")));
  if (!dialog)
    return IUP_ERROR;

  if (parent)
    gtk_window_set_transient_for((GtkWindow*)dialog, (GtkWindow*)parent);

  ret = iupStrToRGBA(iupAttribGet(ih, "VALUE"), &r, &g, &b, &a);

  g_object_get(dialog, "color-selection", &colorsel, NULL);
  iupgdkColorSet(&color, r, g, b);
  gtk_color_selection_set_current_color(colorsel, &color);

  value = iupAttribGetStr(ih, "ALPHA");
  if (value)
  {
    int alpha;
    if (iupStrToInt(value, &alpha))
    {
      if (alpha<0) alpha=0;
      if (alpha>255) alpha=255;
      gtk_color_selection_set_has_opacity_control(colorsel, TRUE);
      gtk_color_selection_set_current_alpha(colorsel, iupCOLOR8TO16(alpha));
    }
  }
  else if (iupAttribGetBoolean(ih, "SHOWALPHA") || ret == 4)
  {
    gtk_color_selection_set_has_opacity_control(colorsel, TRUE);
    gtk_color_selection_set_current_alpha(colorsel, iupCOLOR8TO16(a));
  }
  else
    gtk_color_selection_set_has_opacity_control(colorsel, FALSE);

  value = iupAttribGetStr(ih, "COLORTABLE");
  if (value)
  {
    gtk_color_selection_set_has_palette (colorsel, TRUE);
    gtkColorDlgSetPalette(colorsel, value);
  }
  else if (iupAttribGetBoolean(ih, "SHOWCOLORTABLE"))
    gtk_color_selection_set_has_palette (colorsel, TRUE);
  else
    gtk_color_selection_set_has_palette (colorsel, FALSE);

  if (IupGetCallback(ih, "HELP_CB"))
  {
    GtkWidget* help_button;
    g_object_get(dialog, "help-button", &help_button, NULL);
    gtk_widget_show(help_button);
  }
  
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
    GdkColor color;
    gtk_color_selection_get_current_color(colorsel, &color);
    IupSetAttribute(ih, "STATUS", "1");

    if (gtk_color_selection_get_has_opacity_control(colorsel))
    {
      int alpha = gtk_color_selection_get_current_alpha(colorsel);
      iupAttribSetInt(ih, "ALPHA", (int)iupCOLOR16TO8(alpha));
      iupAttribSetStrf(ih, "VALUE", "%d %d %d %d", (int)iupCOLOR16TO8(color.red), (int)iupCOLOR16TO8(color.green), (int)iupCOLOR16TO8(color.blue), (int)iupCOLOR16TO8(alpha));
    }
    else
      iupAttribSetStrf(ih, "VALUE", "%d %d %d", (int)iupCOLOR16TO8(color.red), (int)iupCOLOR16TO8(color.green), (int)iupCOLOR16TO8(color.blue));

    if (gtk_color_selection_get_has_palette(colorsel))
      gtkColorDlgGetPalette(ih, colorsel);
  }
  else
  {
    iupAttribSetStr(ih, "ALPHA", NULL);
    iupAttribSetStr(ih, "VALUE", NULL);
    iupAttribSetStr(ih, "COLORTABLE", NULL);
    iupAttribSetStr(ih, "STATUS", NULL);
  }

  gtk_widget_destroy(GTK_WIDGET(dialog));  

  return IUP_NOERROR;
}

void iupdrvColorDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = gtkColorDlgPopup;
}
