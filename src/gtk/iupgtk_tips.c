/** \file
 * \brief Windows Driver TIPS management
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>

#include <gtk/gtk.h>

#include "iup.h" 
#include "iupcbs.h" 

#include "iup_object.h" 
#include "iup_str.h" 
#include "iup_attrib.h" 
#include "iup_image.h" 
#include "iup_drv.h"
#include "iup_drvinfo.h"

#include "iupgtk_drv.h"

#if GTK_CHECK_VERSION(2, 12, 0)
#else
static GtkTooltips* gtk_tips = NULL;    /* old TIPS */
#endif

static void gtkTooltipSetTitle(Ihandle* ih, GtkWidget* widget, const char* value)
{
#if GTK_CHECK_VERSION(2, 12, 0)
  if (iupAttribGetBoolean(ih, "TIPMARKUP"))
    gtk_widget_set_tooltip_markup(widget, iupgtkStrConvertToUTF8(value));
  else
    gtk_widget_set_tooltip_text(widget, iupgtkStrConvertToUTF8(value));
#else
  gtk_tooltips_set_tip(gtk_tips, widget, iupgtkStrConvertToUTF8(value), NULL);
#endif
}

#if GTK_CHECK_VERSION(2, 12, 0)
static gboolean gtkQueryTooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, Ihandle* ih)
{
  char* value;

  IFnii cb = (IFnii)IupGetCallback(ih, "TIPS_CB");
  if (cb)
  {
    int x, y;
    iupdrvGetCursorPos(&x, &y);
    iupdrvScreenToClient(ih, &x, &y);
    cb(ih, x, y);

    /* set again because it could has been changed inside the callback */
    gtkTooltipSetTitle(ih, widget, IupGetAttribute(ih, "TIP"));
  }

  value = iupAttribGet(ih, "TIPRECT");
  if (value && !keyboard_mode)
  {
    GdkRectangle rect;
    int x1, x2, y1, y2;
    sscanf(value, "%d %d %d %d", &x1, &y1, &x2, &y2);
    rect.x = x1;
    rect.y = y1;
    rect.width = x2-x1+1;
    rect.height = y2-y1+1;
    gtk_tooltip_set_tip_area(tooltip, &rect);
  }
  else
    gtk_tooltip_set_tip_area(tooltip, NULL);

  value = iupAttribGet(ih, "TIPICON");
  if (!value)
    gtk_tooltip_set_icon(tooltip, NULL);
  else
  {
    GdkPixbuf* icon = (GdkPixbuf*)iupImageGetIcon(value);
    if (icon)
      gtk_tooltip_set_icon(tooltip, icon);
  }

  /* NOTE:
   gtk_widget_get_tooltip_window could be used to set 
   TIPBGCOLOR, TIPFGCOLOR and TIPFONT, but it returns NULL inside the signal. */

  (void)y;
  (void)x;
  (void)widget;
  return FALSE;
}
#endif

int iupdrvBaseSetTipAttrib(Ihandle* ih, const char* value)
{
  GtkWidget* widget = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (!widget)
    widget = ih->handle;

#if GTK_CHECK_VERSION(2, 12, 0)
  g_signal_connect(widget, "query-tooltip", G_CALLBACK(gtkQueryTooltip), ih);
#else
  if (gtk_tips == NULL)
    gtk_tips = gtk_tooltips_new();
#endif

  gtkTooltipSetTitle(ih, widget, value);

  return 1;
}

int iupdrvBaseSetTipVisibleAttrib(Ihandle* ih, const char* value)
{
  GtkWidget* widget = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (!widget)
    widget = ih->handle;
  (void)value;

#if GTK_CHECK_VERSION(2, 12, 0)
  gtk_widget_trigger_tooltip_query(widget);
#endif

  return 0;
}

char* iupdrvBaseGetTipVisibleAttrib(Ihandle* ih)
{
  GtkWindow* tip_window;
  GtkWidget* widget = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (!widget)
    widget = ih->handle;

#if GTK_CHECK_VERSION(2, 12, 0)
  if (!gtk_widget_get_has_tooltip(widget))
    return NULL;

  tip_window = gtk_widget_get_tooltip_window(widget);
#if GTK_CHECK_VERSION(2, 18, 0)
  if (tip_window && gtk_widget_get_visible((GtkWidget*)tip_window))
#else
  if (tip_window && GTK_WIDGET_VISIBLE(tip_window))
#endif
    return "Yes";
  else
    return "No";
#else
  return NULL;
#endif
}
