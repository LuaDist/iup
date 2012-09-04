/** \file
 * \brief Button Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"
#include "iup_button.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_image.h"
#include "iup_key.h"

#include "iupgtk_drv.h"


#if !GTK_CHECK_VERSION(2, 6, 0)
static void gtk_button_set_image(GtkButton *button, GtkWidget *image)
{
}
static GtkWidget* gtk_button_get_image(GtkButton *button)
{
  return NULL;
}
#endif

void iupdrvButtonAddBorders(int *x, int *y)
{
#ifdef WIN32
  int border_size = 2*5;
#else
#ifdef HILDON
  int border_size = 2*7+1; /* borders are not symetric */
#else
  int border_size = 2*5+1; /* borders are not symetric */
#endif
#endif
  (*x) += border_size;
  (*y) += border_size;
}

static void gtkButtonChildrenCb(GtkWidget *widget, gpointer client_data)
{
  if (GTK_IS_LABEL(widget))
  {
    GtkLabel **label = (GtkLabel**) client_data;
    *label = (GtkLabel*)widget;
  }
}

static GtkLabel* gtkButtonGetLabel(Ihandle* ih)
{
  if (ih->data->type == IUP_BUTTON_TEXT)  /* text only */
    return (GtkLabel*)gtk_bin_get_child((GtkBin*)ih->handle);
  else if (ih->data->type == IUP_BUTTON_BOTH) /* both */
  {
    /* when both is set, button contains an GtkAlignment, 
       that contains a GtkBox, that contains a label and an image */
    GtkContainer *container = (GtkContainer*)gtk_bin_get_child((GtkBin*)gtk_bin_get_child((GtkBin*)ih->handle));
    GtkLabel* label = NULL;
    gtk_container_foreach(container, gtkButtonChildrenCb, &label);
    return label;
  }
  return NULL;
}

static int gtkButtonSetTitleAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type & IUP_BUTTON_TEXT)  /* text or both */
  {
    GtkLabel* label = gtkButtonGetLabel(ih);
    iupgtkSetMnemonicTitle(ih, label, value);
    return 1;
  }

  return 0;
}

static int gtkButtonSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  GtkButton* button = (GtkButton*)ih->handle;
  PangoAlignment alignment;
  float xalign, yalign;
  char value1[30]="", value2[30]="";

  if (iupAttribGet(ih, "_IUPGTK_EVENTBOX"))
    return 0;

  iupStrToStrStr(value, value1, value2, ':');

  if (iupStrEqualNoCase(value1, "ARIGHT"))
  {
    xalign = 1.0f;
    alignment = PANGO_ALIGN_RIGHT;
  }
  else if (iupStrEqualNoCase(value1, "ACENTER"))
  {
    xalign = 0.5f;
    alignment = PANGO_ALIGN_CENTER;
  }
  else /* "ALEFT" */
  {
    xalign = 0;
    alignment = PANGO_ALIGN_LEFT;
  }

  if (iupStrEqualNoCase(value2, "ABOTTOM"))
    yalign = 1.0f;
  else if (iupStrEqualNoCase(value2, "ATOP"))
    yalign = 0;
  else  /* ACENTER (default) */
    yalign = 0.5f;

  gtk_button_set_alignment(button, xalign, yalign);

  if (ih->data->type == IUP_BUTTON_TEXT && !GTK_IS_COLOR_BUTTON(ih->handle))   /* text only */
  {
    PangoLayout* layout = gtk_label_get_layout(gtkButtonGetLabel(ih));
    if (layout) pango_layout_set_alignment(layout, alignment);
  }

  return 1;
}

static int gtkButtonSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');
  if (ih->handle)
  {
    if (ih->data->type == IUP_BUTTON_TEXT)   /* text only */
    {
      GtkMisc* misc = (GtkMisc*)gtk_bin_get_child((GtkBin*)ih->handle);
      gtk_misc_set_padding(misc, ih->data->horiz_padding, ih->data->vert_padding);
    }
    else
    {
      GtkAlignment* alignment = (GtkAlignment*)gtk_bin_get_child((GtkBin*)ih->handle);
      if (GTK_IS_ALIGNMENT(alignment))
        gtk_alignment_set_padding(alignment, ih->data->vert_padding, ih->data->vert_padding, 
                                             ih->data->horiz_padding, ih->data->horiz_padding);
    }
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

#ifdef WIN32
static int gtkButtonSetBgColorAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_BUTTON_TEXT && GTK_IS_COLOR_BUTTON(ih->handle))
  {
    GdkColor color;
    unsigned char r, g, b;
    if (!iupStrToRGB(value, &r, &g, &b))
      return 0;

    iupgdkColorSet(&color, r, g, b);
    gtk_color_button_set_color((GtkColorButton*)ih->handle, &color);
    return 1;
  }

  return iupdrvBaseSetBgColorAttrib(ih, value);
}
#endif

static int gtkButtonSetFgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  GtkLabel* label = gtkButtonGetLabel(ih);
  if (!label) return 0;

  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iupgtkBaseSetFgColor((GtkWidget*)label, r, g, b);

  return 1;
}

static int gtkButtonSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetStandardFontAttrib(ih, value);

  if (ih->handle)
  {
    GtkLabel* label = gtkButtonGetLabel(ih);
    if (!label) return 1;

    gtk_widget_modify_font((GtkWidget*)label, (PangoFontDescription*)iupgtkGetPangoFontDescAttrib(ih));
    if (ih->data->type == IUP_BUTTON_TEXT && !GTK_IS_COLOR_BUTTON(ih->handle))   /* text only */
      iupgtkFontUpdatePangoLayout(ih, gtk_label_get_layout(label));
  }
  return 1;
}

static void gtkButtonSetPixbuf(Ihandle* ih, const char* name, int make_inactive)
{
  GtkImage* image;
  if (!iupAttribGet(ih, "_IUPGTK_EVENTBOX"))
    image = (GtkImage*)gtk_button_get_image((GtkButton*)ih->handle);
  else
    image = (GtkImage*)gtk_bin_get_child((GtkBin*)ih->handle);

  if (name && image)
  {
    GdkPixbuf* pixbuf = iupImageGetImage(name, ih, make_inactive);
    GdkPixbuf* old_pixbuf = gtk_image_get_pixbuf(image);
    if (pixbuf != old_pixbuf)
      gtk_image_set_from_pixbuf(image, pixbuf);
    return;
  }

  /* if not defined */
#if GTK_CHECK_VERSION(2, 8, 0)
  if (image)
    gtk_image_clear(image);
#endif
}

static int gtkButtonSetImageAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type & IUP_BUTTON_IMAGE)
  {
    if (iupdrvIsActive(ih))
      gtkButtonSetPixbuf(ih, value, 0);
    else
    {
      if (!iupAttribGet(ih, "IMINACTIVE"))
      {
        /* if not active and IMINACTIVE is not defined 
           then automaticaly create one based on IMAGE */
        gtkButtonSetPixbuf(ih, value, 1); /* make_inactive */
      }
    }
    return 1;
  }
  else
    return 0;
}

static int gtkButtonSetImInactiveAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type & IUP_BUTTON_IMAGE)
  {
    if (!iupdrvIsActive(ih))
    {
      if (value)
        gtkButtonSetPixbuf(ih, value, 0);
      else
      {
        /* if not defined then automaticaly create one based on IMAGE */
        char* name = iupAttribGet(ih, "IMAGE");
        gtkButtonSetPixbuf(ih, name, 1); /* make_inactive */
      }
    }
    return 1;
  }
  else
    return 0;
}

static int gtkButtonSetActiveAttrib(Ihandle* ih, const char* value)
{
  /* update the inactive image if necessary */
  if (ih->data->type & IUP_BUTTON_IMAGE)
  {
    if (!iupStrBoolean(value))
    {
      char* name = iupAttribGet(ih, "IMINACTIVE");
      if (name)
        gtkButtonSetPixbuf(ih, name, 0);
      else
      {
        /* if not defined then automaticaly create one based on IMAGE */
        name = iupAttribGet(ih, "IMAGE");
        gtkButtonSetPixbuf(ih, name, 1); /* make_inactive */
      }
    }
    else
    {
      /* must restore the normal image */
      char* name = iupAttribGet(ih, "IMAGE");
      gtkButtonSetPixbuf(ih, name, 0);
    }
  }

  return iupBaseSetActiveAttrib(ih, value);
}

static gboolean gtkButtonEnterLeaveEvent(GtkWidget *widget, GdkEventCrossing *evt, Ihandle *ih)
{
  /* Used only when FLAT=Yes */

  iupgtkEnterLeaveEvent(widget, evt, ih);
  (void)widget;

  if (evt->type == GDK_ENTER_NOTIFY)
    gtk_button_set_relief((GtkButton*)ih->handle, GTK_RELIEF_NORMAL);
  else  if (evt->type == GDK_LEAVE_NOTIFY)               
    gtk_button_set_relief((GtkButton*)ih->handle, GTK_RELIEF_NONE);

  return FALSE;
}

static void gtkButtonClicked(GtkButton *widget, Ihandle* ih)
{
  Icallback cb = IupGetCallback(ih, "ACTION");
  if (cb)
  {
    if (cb(ih) == IUP_CLOSE) 
      IupExitLoop();
  }
  (void)widget;
}

static gboolean gtkButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih)
{
  if (iupgtkButtonEvent(widget, evt, ih)==TRUE)
    return TRUE;

  if (ih->data->type & IUP_BUTTON_IMAGE)
  {
    char* name = iupAttribGet(ih, "IMPRESS");
    if (name)
    {
      if (evt->type == GDK_BUTTON_PRESS)
        gtkButtonSetPixbuf(ih, name, 0);
      else
      {
        name = iupAttribGet(ih, "IMAGE");
        gtkButtonSetPixbuf(ih, name, 0);
      }
    }

    if (evt->type == GDK_BUTTON_RELEASE &&
        iupAttribGet(ih, "_IUPGTK_EVENTBOX"))
    {
      Icallback cb = IupGetCallback(ih, "ACTION");
      if (cb)
      {
        if (cb(ih) == IUP_CLOSE) 
          IupExitLoop();
      }
    }
  }

#ifdef WIN32
  if (evt->type == GDK_BUTTON_RELEASE)
  {
    if (ih->data->type == IUP_BUTTON_TEXT && GTK_IS_COLOR_BUTTON(ih->handle))
    {
      /* Ugly solution, but there is no way how to NOT show the GTK dialog on click action */
      gtkButtonClicked(NULL, ih);
      return TRUE;
    }
  }
#endif
   
  return FALSE;
}

static int gtkButtonMapMethod(Ihandle* ih)
{
  int impress;
  char* value;

  value = iupAttribGet(ih, "IMAGE");
  if (value)
  {
    ih->data->type = IUP_BUTTON_IMAGE;

    value = iupAttribGet(ih, "TITLE");
    if (value && *value!=0)
      ih->data->type |= IUP_BUTTON_TEXT;
  }
  else
    ih->data->type = IUP_BUTTON_TEXT;

  if (ih->data->type == IUP_BUTTON_IMAGE &&
      iupAttribGet(ih, "IMPRESS") &&
      !iupAttribGetBoolean(ih, "IMPRESSBORDER"))
  {
    GtkWidget *img = gtk_image_new();
    ih->handle = gtk_event_box_new();
    gtk_container_add((GtkContainer*)ih->handle, img);
    gtk_widget_show(img);
    iupAttribSetStr(ih, "_IUPGTK_EVENTBOX", "1");
  }
  else
    ih->handle = gtk_button_new();

  if (!ih->handle)
    return IUP_ERROR;

  if (ih->data->type & IUP_BUTTON_IMAGE)
  {
    if (!iupAttribGet(ih, "_IUPGTK_EVENTBOX"))
    {
      gtk_button_set_image((GtkButton*)ih->handle, gtk_image_new());

      if (ih->data->type & IUP_BUTTON_TEXT)
      {
        GtkSettings* settings = gtk_widget_get_settings(ih->handle);
        g_object_set(settings, "gtk-button-images", (int)TRUE, NULL);

        gtk_button_set_label((GtkButton*)ih->handle, iupgtkStrConvertToUTF8(iupAttribGet(ih, "TITLE")));
      
#if GTK_CHECK_VERSION(2, 10, 0)
        gtk_button_set_image_position((GtkButton*)ih->handle, ih->data->img_position);  /* IUP and GTK have the same Ids */
#endif
      }
    }
  }
  else
  {
    char* title = iupAttribGet(ih, "TITLE");
    if (!title) 
    {
#ifdef WIN32
      if (iupAttribGet(ih, "BGCOLOR"))
      {
        gtk_widget_destroy(ih->handle);
        ih->handle = gtk_color_button_new();
      }
      else
        gtk_button_set_label((GtkButton*)ih->handle, "");
#else
      gtk_button_set_label((GtkButton*)ih->handle, "");
#endif
    }
    else
      gtk_button_set_label((GtkButton*)ih->handle, iupgtkStrConvertToUTF8(title));
  }

  /* add to the parent, all GTK controls must call this. */
  iupgtkBaseAddToParent(ih);

  if (!iupAttribGetBoolean(ih, "CANFOCUS"))
    iupgtkSetCanFocus(ih->handle, 0);

  value = iupAttribGet(ih, "IMPRESS");
  impress = (ih->data->type & IUP_BUTTON_IMAGE && value)? 1: 0;
  if (!impress && iupAttribGetBoolean(ih, "FLAT"))
  {
    gtk_button_set_relief((GtkButton*)ih->handle, GTK_RELIEF_NONE);

    g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event", G_CALLBACK(gtkButtonEnterLeaveEvent), ih);
    g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event", G_CALLBACK(gtkButtonEnterLeaveEvent), ih);
  }
  else
  {
    if (!iupAttribGet(ih, "_IUPGTK_EVENTBOX"))
    {
      if (impress && !iupAttribGetStr(ih, "IMPRESSBORDER"))
        gtk_button_set_relief((GtkButton*)ih->handle, GTK_RELIEF_NONE);
      else
        gtk_button_set_relief((GtkButton*)ih->handle, GTK_RELIEF_NORMAL);
    }

    g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
    g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  }

  g_signal_connect(G_OBJECT(ih->handle), "focus-in-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-out-event",    G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "key-press-event",    G_CALLBACK(iupgtkKeyPressEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "show-help",          G_CALLBACK(iupgtkShowHelp), ih);

  if (!iupAttribGet(ih, "_IUPGTK_EVENTBOX"))
    g_signal_connect(G_OBJECT(ih->handle), "clicked", G_CALLBACK(gtkButtonClicked), ih);

  g_signal_connect(G_OBJECT(ih->handle), "button-press-event", G_CALLBACK(gtkButtonEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "button-release-event",G_CALLBACK(gtkButtonEvent), ih);

  gtk_widget_realize(ih->handle);

  return IUP_NOERROR;
}

void iupdrvButtonInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkButtonMapMethod;

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkButtonSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkButtonSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Visual */
#ifdef WIN32
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkButtonSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
#else
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
#endif

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkButtonSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, gtkButtonSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupButton only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkButtonSetAlignmentAttrib, "ACENTER:ACENTER", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkButtonSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkButtonSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMPRESS", NULL, NULL, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "PADDING", iupButtonGetPaddingAttrib, gtkButtonSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
}
