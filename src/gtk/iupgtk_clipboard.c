/** \file
 * \brief Clipboard for the GTK Driver.
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"

#include "iupgtk_drv.h"


static int gtkClipboardSetTextAttrib(Ihandle *ih, const char *value)
{
  GtkClipboard *clipboard = gtk_clipboard_get_for_display(gdk_display_get_default(), gdk_atom_intern("CLIPBOARD", FALSE));
  if (!value)
  {
    gtk_clipboard_clear(clipboard);
    return 0;
  }
  gtk_clipboard_set_text(clipboard, value, -1);
  (void)ih;
  return 0;
}

static char* gtkClipboardGetTextAttrib(Ihandle *ih)
{
  GtkClipboard *clipboard = gtk_clipboard_get(gdk_atom_intern("CLIPBOARD", FALSE));
  (void)ih;
  return iupgtkStrConvertFromUTF8(gtk_clipboard_wait_for_text(clipboard));
}

static int gtkClipboardSetImageAttrib(Ihandle *ih, const char *value)
{
#if GTK_CHECK_VERSION(2, 6, 0)
  GdkPixbuf *pixbuf;
  GtkClipboard *clipboard = gtk_clipboard_get (gdk_atom_intern("CLIPBOARD", FALSE));
  if (!value)
  {
    gtk_clipboard_clear(clipboard);
    return 0;
  }

  pixbuf = (GdkPixbuf*)iupImageGetImage(value, ih, 0);
  if (pixbuf)
    gtk_clipboard_set_image (clipboard, pixbuf);
#endif
  return 0;
}

static int gtkClipboardSetNativeImageAttrib(Ihandle *ih, const char *value)
{
#if GTK_CHECK_VERSION(2, 6, 0)
  GtkClipboard *clipboard = gtk_clipboard_get (gdk_atom_intern("CLIPBOARD", FALSE));
  (void)ih;

  if (!value)
  {
    gtk_clipboard_clear(clipboard);
    return 0;
  }

  gtk_clipboard_set_image (clipboard, (GdkPixbuf*)value);
#endif
  return 0;
}

static char* gtkClipboardGetNativeImageAttrib(Ihandle *ih)
{
#if GTK_CHECK_VERSION(2, 6, 0)
  GtkClipboard *clipboard = gtk_clipboard_get (gdk_atom_intern("CLIPBOARD", FALSE));
  (void)ih;
  return (char*)gtk_clipboard_wait_for_image (clipboard);
#else
  return NULL;
#endif
}

static char* gtkClipboardGetTextAvailableAttrib(Ihandle *ih)
{
  GtkClipboard *clipboard = gtk_clipboard_get (gdk_atom_intern("CLIPBOARD", FALSE));
  (void)ih;
  if (gtk_clipboard_wait_is_text_available(clipboard))
    return "YES";
  else
    return "NO";
}

static char* gtkClipboardGetImageAvailableAttrib(Ihandle *ih)
{
#if GTK_CHECK_VERSION(2, 6, 0)
  GtkClipboard *clipboard = gtk_clipboard_get (gdk_atom_intern("CLIPBOARD", FALSE));
  (void)ih;
  if (gtk_clipboard_wait_is_image_available(clipboard))
    return "YES";
  else
    return "NO";
#else
  return NULL;
#endif
}

/******************************************************************************/

Ihandle* IupClipboard(void)
{
  return IupCreate("clipboard");
}

Iclass* iupClipboardNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "clipboard";
  ic->format = NULL;  /* no parameters */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  ic->New = iupClipboardNewClass;

  /* Attribute functions */
  iupClassRegisterAttribute(ic, "TEXT", gtkClipboardGetTextAttrib, gtkClipboardSetTextAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NATIVEIMAGE", gtkClipboardGetNativeImageAttrib, gtkClipboardSetNativeImageAttrib, NULL, NULL, IUPAF_NO_STRING|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, gtkClipboardSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXTAVAILABLE", gtkClipboardGetTextAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEAVAILABLE", gtkClipboardGetImageAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}
