/** \file
 * \brief Label Control
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
#include "iup_label.h"
#include "iup_drv.h"
#include "iup_image.h"
#include "iup_focus.h"

#include "iupgtk_drv.h"


static int gtkLabelSetTitleAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_LABEL_TEXT)
  {
    GtkLabel* label = (GtkLabel*)ih->handle;
    if (iupgtkSetMnemonicTitle(ih, label, value))
    {
      Ihandle* next = iupFocusNextInteractive(ih);
      if (next)
      {
        if (next->handle)
          gtk_label_set_mnemonic_widget(label, next->handle);
        else
          iupAttribSetStr(next, "_IUPGTK_LABELMNEMONIC", (char*)label);  /* used by iupgtkUpdateMnemonic */
      }
    }
    return 1;
  }

  return 0;
}

static int gtkLabelSetWordWrapAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_LABEL_TEXT)
  {
    GtkLabel* label = (GtkLabel*)ih->handle;
    if (iupStrBoolean(value))
      gtk_label_set_line_wrap(label, TRUE);
    else
      gtk_label_set_line_wrap(label, FALSE);
    return 1;
  }
  return 0;
}

static int gtkLabelSetEllipsisAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_LABEL_TEXT)
  {
#if GTK_CHECK_VERSION(2, 6, 0)
    GtkLabel* label = (GtkLabel*)ih->handle;
    if (iupStrBoolean(value))
      gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
    else
      gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_NONE);
#endif
    return 1;
  }
  return 0;
}

static int gtkLabelSetAlignmentAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type != IUP_LABEL_SEP_HORIZ && ih->data->type != IUP_LABEL_SEP_VERT)
  {
    GtkMisc* misc = (GtkMisc*)ih->handle;
    PangoAlignment alignment;
    float xalign, yalign;
    char value1[30]="", value2[30]="";

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

    gtk_misc_set_alignment(misc, xalign, yalign);

    if (ih->data->type == IUP_LABEL_TEXT)
      pango_layout_set_alignment(gtk_label_get_layout((GtkLabel*)ih->handle), alignment);

    return 1;
  }
  else
    return 0;
}

static int gtkLabelSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');

  if (ih->handle && ih->data->type != IUP_LABEL_SEP_HORIZ && ih->data->type != IUP_LABEL_SEP_VERT)
  {
    GtkMisc* misc = (GtkMisc*)ih->handle;
    gtk_misc_set_padding(misc, ih->data->horiz_padding, ih->data->vert_padding);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static char* gtkLabelGetWidgetPangoLayoutAttrib(Ihandle* ih)
{
  if (ih->data->type == IUP_LABEL_TEXT)
    return (char*)gtk_label_get_layout((GtkLabel*)ih->handle);
  else
    return NULL;
}

static void gtkLabelSetPixbuf(Ihandle* ih, const char* name, int make_inactive)
{
  GtkImage* image_label = (GtkImage*)ih->handle;

  if (name)
  {
    GdkPixbuf* pixbuf = iupImageGetImage(name, ih, make_inactive);
    GdkPixbuf* old_pixbuf = gtk_image_get_pixbuf(image_label);
    if (pixbuf != old_pixbuf)
      gtk_image_set_from_pixbuf(image_label, pixbuf);
    return;
  }

  /* if not defined */
#if GTK_CHECK_VERSION(2, 8, 0)
  gtk_image_clear(image_label);
#endif
}

static int gtkLabelSetImageAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_LABEL_IMAGE)
  {
    if (iupdrvIsActive(ih))
      gtkLabelSetPixbuf(ih, value, 0);
    else
    {
      if (!iupAttribGet(ih, "IMINACTIVE"))
      {
        /* if not active and IMINACTIVE is not defined 
           then automaticaly create one based on IMAGE */
        gtkLabelSetPixbuf(ih, value, 1); /* make_inactive */
      }
    }
    return 1;
  }
  else
    return 0;
}

static int gtkLabelSetImInactiveAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->type == IUP_LABEL_IMAGE)
  {
    if (!iupdrvIsActive(ih))
    {
      if (value)
        gtkLabelSetPixbuf(ih, value, 0);
      else
      {
        /* if not defined then automaticaly create one based on IMAGE */
        char* name = iupAttribGet(ih, "IMAGE");
        gtkLabelSetPixbuf(ih, name, 1); /* make_inactive */
      }
    }
    return 1;
  }
  else
    return 0;
}

static int gtkLabelSetActiveAttrib(Ihandle* ih, const char* value)
{
  /* update the inactive image if necessary */
  if (ih->data->type == IUP_LABEL_IMAGE)
  {
    if (!iupStrBoolean(value))
    {
      char* name = iupAttribGet(ih, "IMINACTIVE");
      if (name)
        gtkLabelSetPixbuf(ih, name, 0);
      else
      {
        /* if not defined then automaticaly create one based on IMAGE */
        name = iupAttribGet(ih, "IMAGE");
        gtkLabelSetPixbuf(ih, name, 1); /* make_inactive */
      }
    }
    else
    {
      /* must restore the normal image */
      char* name = iupAttribGet(ih, "IMAGE");
      gtkLabelSetPixbuf(ih, name, 0);
    }
  }

  return iupBaseSetActiveAttrib(ih, value);
}

static int gtkLabelMapMethod(Ihandle* ih)
{
  char* value;
  GtkWidget *label;

  value = iupAttribGet(ih, "SEPARATOR");
  if (value)
  {
    if (iupStrEqualNoCase(value, "HORIZONTAL"))
    {
      ih->data->type = IUP_LABEL_SEP_HORIZ;
      label = gtk_hseparator_new();
    }
    else /* "VERTICAL" */
    {
      ih->data->type = IUP_LABEL_SEP_VERT;
      label = gtk_vseparator_new();
    }
  }
  else
  {
    value = iupAttribGet(ih, "IMAGE");
    if (value)
    {
      ih->data->type = IUP_LABEL_IMAGE;
      label = gtk_image_new();
    }
    else
    {
      ih->data->type = IUP_LABEL_TEXT;
      label = gtk_label_new(NULL);
    }
  }

  if (!label)
    return IUP_ERROR;

  ih->handle = label;

  /* add to the parent, all GTK controls must call this. */
  iupgtkBaseAddToParent(ih);

  g_signal_connect(G_OBJECT(ih->handle), "button-press-event", G_CALLBACK(iupgtkButtonEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "button-release-event",G_CALLBACK(iupgtkButtonEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);

  gtk_widget_realize(label);

  /* configure for DRAG&DROP of files */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSetStr(ih, "DROPFILESTARGET", "YES");

  return IUP_NOERROR;
}

void iupdrvLabelInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkLabelMapMethod;

  /* Driver Dependent Attribute functions */

  /* Common GTK only (when text is in a secondary element) */
  iupClassRegisterAttribute(ic, "WIDGETPANGOLAYOUT", gtkLabelGetWidgetPangoLayoutAttrib, NULL, NULL, NULL, IUPAF_NO_INHERIT);

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, gtkLabelSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, iupdrvBaseSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, gtkLabelSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupLabel only */
  iupClassRegisterAttribute(ic, "ALIGNMENT", NULL, gtkLabelSetAlignmentAttrib, "ALEFT:ACENTER", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkLabelSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iupLabelGetPaddingAttrib, gtkLabelSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);

  /* IupLabel GTK and Motif only */
  iupClassRegisterAttribute(ic, "IMINACTIVE", NULL, gtkLabelSetImInactiveAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupLabel Windows and GTK only */
  iupClassRegisterAttribute(ic, "WORDWRAP", NULL, gtkLabelSetWordWrapAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "ELLIPSIS", NULL, gtkLabelSetEllipsisAttrib, NULL, NULL, IUPAF_DEFAULT);

  /* IupLabel GTK only */
  iupClassRegisterAttribute(ic, "MARKUP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);
}
