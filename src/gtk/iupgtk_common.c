/** \file
 * \brief GTK Base Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>              
#include <stdlib.h>
#include <string.h>             
#include <limits.h>             

#include <gtk/gtk.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_key.h"
#include "iup_str.h"
#include "iup_class.h"
#include "iup_attrib.h"
#include "iup_focus.h"
#include "iup_key.h"
#include "iup_image.h"
#include "iup_drv.h"

#include "iupgtk_drv.h"


/* WARNING: in GTK there are many controls that are not native windows, 
   so it GdkWindow will NOT return a native window exclusive of that control,
   in fact it can return a base native window shared by many controls.
   IupCanvas is a special case that uses an exclusive native window. */

/* GTK only has absolute positioning using a GtkFixed container,
   so all elements returned by iupChildTreeGetNativeParentHandle should be a GtkFixed. 
   If not looks in the native parent. */
static GtkFixed* gtkGetFixedParent(Ihandle* ih)
{
  GtkWidget* widget = iupChildTreeGetNativeParentHandle(ih);
  while (widget && !GTK_IS_FIXED(widget))
    widget = gtk_widget_get_parent(widget);
  return (GtkFixed*)widget;
}

const char* iupgtkGetWidgetClassName(GtkWidget* widget)
{
  /* Used for debugging */
  return g_type_name(G_TYPE_FROM_CLASS(GTK_WIDGET_GET_CLASS(widget)));
}

void iupgtkUpdateMnemonic(Ihandle* ih)
{
  GtkLabel* label = (GtkLabel*)iupAttribGet(ih, "_IUPGTK_LABELMNEMONIC");
  if (label) gtk_label_set_mnemonic_widget(label, ih->handle);
}

void iupdrvActivate(Ihandle* ih)
{
  gtk_widget_activate(ih->handle);
}

void iupdrvReparent(Ihandle* ih)
{
  GtkWidget* old_parent;
  GtkWidget* new_parent = (GtkWidget*)gtkGetFixedParent(ih);
  GtkWidget* widget = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");  /* here is used as the native child because is the outmost component of the elemement */
  if (!widget) widget = ih->handle;
  old_parent = gtk_widget_get_parent(widget);
  if (old_parent != new_parent)
    gtk_widget_reparent(widget, new_parent);
}

void iupgtkBaseAddToParent(Ihandle* ih)
{
  GtkFixed* fixed = gtkGetFixedParent(ih);
  GtkWidget* widget = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT"); /* here is used as the native child because is the outmost component of the elemement */
  if (!widget) widget = ih->handle;

  gtk_fixed_put(fixed, widget, 0, 0);
}

void iupdrvBaseLayoutUpdateMethod(Ihandle *ih)
{
  GtkFixed* fixed = gtkGetFixedParent(ih);
  GtkWidget* widget = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (!widget) widget = ih->handle;

  gtk_fixed_move(fixed, widget, ih->x, ih->y);
  gtk_widget_set_size_request(widget, ih->currentwidth, ih->currentheight);
}

void iupdrvBaseUnMapMethod(Ihandle* ih)
{
  GtkWidget* widget = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (!widget) widget = ih->handle;
  gtk_widget_unrealize(widget);
  gtk_widget_destroy(widget);   /* To match the call to gtk_*****_new     */
}

void iupdrvPostRedraw(Ihandle *ih)
{
  /* Post a REDRAW */
  gtk_widget_queue_draw(ih->handle);
}

void iupdrvRedrawNow(Ihandle *ih)
{
  GdkWindow* window = iupgtkGetWindow(ih->handle);
  /* Post a REDRAW */
  gtk_widget_queue_draw(ih->handle);
  /* Force a REDRAW */
  if (window)
    gdk_window_process_updates(window, FALSE);
}

static GtkWidget* gtkGetWindowedParent(GtkWidget* widget)
{
#if GTK_CHECK_VERSION(2, 18, 0)
  while (widget && !gtk_widget_get_has_window(widget))
#else
	while (widget && GTK_WIDGET_NO_WINDOW(widget))
#endif
    widget = gtk_widget_get_parent(widget);
  return widget;
}

void iupdrvScreenToClient(Ihandle* ih, int *x, int *y)
{
  gint win_x = 0, win_y = 0;
  gint dx = 0, dy = 0;
  GtkWidget* wparent = gtkGetWindowedParent(ih->handle);
  if (ih->handle != wparent)
    gtk_widget_translate_coordinates(ih->handle, wparent, 0, 0, &dx, &dy); /* widget origin relative to GDK window */
  gdk_window_get_origin(iupgtkGetWindow(wparent), &win_x, &win_y);  /* GDK window origin relative to screen */
  *x -= win_x + dx;
  *y -= win_y + dy;
}

void iupdrvClientToScreen(Ihandle* ih, int *x, int *y)
{
  gint win_x = 0, win_y = 0;
  gint dx = 0, dy = 0;
  GtkWidget* wparent = gtkGetWindowedParent(ih->handle);
  if (ih->handle != wparent)
    gtk_widget_translate_coordinates(ih->handle, wparent, 0, 0, &dx, &dy); /* widget relative to GDK window */
  gdk_window_get_origin(iupgtkGetWindow(wparent), &win_x, &win_y);  /* GDK window relative to screen */
  *x += win_x + dx;
  *y += win_y + dy;
}

gboolean iupgtkShowHelp(GtkWidget *widget, GtkWidgetHelpType *arg1, Ihandle *ih)
{
  Icallback cb;
  (void)widget;
  (void)arg1;

  cb = IupGetCallback(ih, "HELP_CB");
  if (cb && cb(ih) == IUP_CLOSE) 
    IupExitLoop();

  return FALSE;
}

gboolean iupgtkEnterLeaveEvent(GtkWidget *widget, GdkEventCrossing *evt, Ihandle *ih)
{
  Icallback cb = NULL;
  (void)widget;

  if (evt->type == GDK_ENTER_NOTIFY)
    cb = IupGetCallback(ih, "ENTERWINDOW_CB");
  else  if (evt->type == GDK_LEAVE_NOTIFY)
    cb = IupGetCallback(ih, "LEAVEWINDOW_CB");

  if (cb) 
    cb(ih);

  return FALSE;
}

int iupgtkSetMnemonicTitle(Ihandle* ih, GtkLabel* label, const char* value)
{
  char c = '_';
  char* str;

  if (!value) 
    value = "";

  str = iupStrProcessMnemonic(value, &c, 1);  /* replace & by c, the returned value of c is ignored in GTK */
  if (str != value)
  {
    gtk_label_set_text_with_mnemonic(label, iupgtkStrConvertToUTF8(str));
    free(str);
    return 1;
  }
  else
  {
    if (iupAttribGetBoolean(ih, "MARKUP"))
      gtk_label_set_markup(label, iupgtkStrConvertToUTF8(str));
    else
      gtk_label_set_text(label, iupgtkStrConvertToUTF8(str));
  }
  return 0;
}

int iupdrvBaseSetZorderAttrib(Ihandle* ih, const char* value)
{
  if (iupdrvIsVisible(ih))
  {
    GdkWindow* window = iupgtkGetWindow(ih->handle);
    if (iupStrEqualNoCase(value, "TOP"))
      gdk_window_raise(window);
    else
      gdk_window_lower(window);
  }

  return 0;
}

void iupdrvSetVisible(Ihandle* ih, int visible)
{
  GtkWidget* container = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (visible)
  {
    if (container) gtk_widget_show(container);
    gtk_widget_show(ih->handle);
  }
  else
  {
    if (container) gtk_widget_hide(container);
    gtk_widget_hide(ih->handle);
  }
}

int iupdrvIsVisible(Ihandle* ih)
{
#if GTK_CHECK_VERSION(2, 18, 0)
  if (gtk_widget_get_visible(ih->handle))
#else
  if (GTK_WIDGET_VISIBLE(ih->handle))
#endif
  {
    /* if marked as visible, since we use gtk_widget_hide and NOT gtk_widget_hide_all
       must check its parents. */
    Ihandle* parent = ih->parent;
    while (parent)
    {
      if (parent->iclass->nativetype != IUP_TYPEVOID)
      {
#if GTK_CHECK_VERSION(2, 18, 0)
        if (!gtk_widget_get_visible(parent->handle))
#else
        if (!GTK_WIDGET_VISIBLE(parent->handle))
#endif
          return 0;
      }

      parent = parent->parent;
    }
    return 1;
  }
  else
    return 0;
}

int iupdrvIsActive(Ihandle *ih)
{
#if GTK_CHECK_VERSION(2, 18, 0)
  return gtk_widget_is_sensitive(ih->handle);
#else
  return GTK_WIDGET_IS_SENSITIVE(ih->handle);
#endif
}

void iupdrvSetActive(Ihandle* ih, int enable)
{
  GtkWidget* container = (GtkWidget*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  if (container) gtk_widget_set_sensitive(container, enable);
  gtk_widget_set_sensitive(ih->handle, enable);
}

static GdkColor gtkDarkerColor(GdkColor *color)
{
  GdkColor dark_color = {0L,0,0,0};

  dark_color.red = (color->red*9)/10;
  dark_color.green = (color->green*9)/10;
  dark_color.blue = (color->blue*9)/10;

  return dark_color;
}

static guint16 gtkCROP16(int x)
{
  if (x > 65535) return 65535;
  return (guint16)x;
}

static GdkColor gtkLighterColor(GdkColor *color)
{
  GdkColor light_color = {0L,0,0,0};

  light_color.red = gtkCROP16(((int)color->red*11)/10);
  light_color.green = gtkCROP16(((int)color->green*11)/10);
  light_color.blue = gtkCROP16(((int)color->blue*11)/10);

  return light_color;
}

void iupgtkBaseSetBgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b)
{
  GtkRcStyle *rc_style;  
  GdkColor color;

  iupgdkColorSet(&color, r, g, b);

  rc_style = gtk_widget_get_modifier_style(handle);
  rc_style->base[GTK_STATE_NORMAL] = rc_style->bg[GTK_STATE_NORMAL]   = rc_style->bg[GTK_STATE_INSENSITIVE] = color;
  rc_style->bg[GTK_STATE_ACTIVE]   = rc_style->base[GTK_STATE_ACTIVE] = gtkDarkerColor(&color);
  rc_style->base[GTK_STATE_PRELIGHT] = rc_style->bg[GTK_STATE_PRELIGHT] = rc_style->base[GTK_STATE_INSENSITIVE] = gtkLighterColor(&color);

  rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_BASE | GTK_RC_BG;
  rc_style->color_flags[GTK_STATE_ACTIVE] |= GTK_RC_BASE | GTK_RC_BG;
  rc_style->color_flags[GTK_STATE_PRELIGHT] |= GTK_RC_BASE | GTK_RC_BG;
  rc_style->color_flags[GTK_STATE_INSENSITIVE] |= GTK_RC_BASE | GTK_RC_BG;

  gtk_widget_modify_style(handle, rc_style);
}

int iupdrvBaseSetBgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iupgtkBaseSetBgColor(ih->handle, r, g, b);

  /* DO NOT NEED TO UPDATE GTK IMAGES SINCE THEY DO NOT DEPEND ON BGCOLOR */

  return 1;
}

void iupgtkBaseSetFgGdkColor(InativeHandle* handle, GdkColor *color)
{
  GtkRcStyle *rc_style;  

  rc_style = gtk_widget_get_modifier_style(handle);  
  rc_style->fg[GTK_STATE_ACTIVE] = rc_style->fg[GTK_STATE_NORMAL] = rc_style->fg[GTK_STATE_PRELIGHT] = *color;
  rc_style->text[GTK_STATE_ACTIVE] = rc_style->text[GTK_STATE_NORMAL] = rc_style->text[GTK_STATE_PRELIGHT] = *color;
  rc_style->color_flags[GTK_STATE_NORMAL] |= GTK_RC_TEXT | GTK_RC_FG;
  rc_style->color_flags[GTK_STATE_ACTIVE] |= GTK_RC_TEXT | GTK_RC_FG;
  rc_style->color_flags[GTK_STATE_PRELIGHT] |= GTK_RC_TEXT | GTK_RC_FG;

  /* do not set at CHILD_CONTAINER */
  gtk_widget_modify_style(handle, rc_style);
}

void iupgtkBaseSetFgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b)
{
  GdkColor color;
  iupgdkColorSet(&color, r, g, b);
  iupgtkBaseSetFgGdkColor(handle, &color);
}

int iupdrvBaseSetFgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iupgtkBaseSetFgColor(ih->handle, r, g, b);

  return 1;
}

static GdkCursor* gtkEmptyCursor(Ihandle* ih)
{
  /* creates an empty cursor */
  GdkColor cursor_color = {0L,0,0,0};
  char bitsnull[1] = {0x00};

  GdkWindow* window = iupgtkGetWindow(ih->handle);
  GdkPixmap* pixmapnull = gdk_bitmap_create_from_data(
    (GdkDrawable*)window,
    bitsnull,
    1,1);
  GdkCursor* cur = gdk_cursor_new_from_pixmap(
    pixmapnull,
    pixmapnull,
    &cursor_color,
    &cursor_color,
    0,0);

  g_object_unref(pixmapnull);

  return cur;
}

static GdkCursor* gtkGetCursor(Ihandle* ih, const char* name)
{
  static struct {
    const char* iupname;
    int         sysname;
  } table[] = {
    { "NONE",      0}, 
    { "NULL",      0}, 
    { "ARROW",     GDK_LEFT_PTR},
    { "BUSY",      GDK_WATCH},
    { "CROSS",     GDK_CROSSHAIR},
    { "HAND",      GDK_HAND2},
    { "HELP",      GDK_QUESTION_ARROW},
    { "IUP",       GDK_QUESTION_ARROW},
    { "MOVE",      GDK_FLEUR},
    { "PEN",       GDK_PENCIL},
    { "RESIZE_N",  GDK_TOP_SIDE},
    { "RESIZE_S",  GDK_BOTTOM_SIDE},
    { "RESIZE_NS", GDK_SB_V_DOUBLE_ARROW},
    { "SPLITTER_HORIZ", GDK_SB_V_DOUBLE_ARROW},
    { "RESIZE_W",  GDK_LEFT_SIDE},
    { "RESIZE_E",  GDK_RIGHT_SIDE},
    { "RESIZE_WE", GDK_SB_H_DOUBLE_ARROW},
    { "SPLITTER_VERT", GDK_SB_H_DOUBLE_ARROW},
    { "RESIZE_NE", GDK_TOP_RIGHT_CORNER},
    { "RESIZE_SE", GDK_BOTTOM_RIGHT_CORNER},
    { "RESIZE_NW", GDK_TOP_LEFT_CORNER},
    { "RESIZE_SW", GDK_BOTTOM_LEFT_CORNER},
    { "TEXT",      GDK_XTERM}, 
    { "UPARROW",   GDK_CENTER_PTR} 
  };

  GdkCursor* cur;
  char str[200];
  int i, count = sizeof(table)/sizeof(table[0]);

  /* check the cursor cache first (per control)*/
  sprintf(str, "_IUPGTK_CURSOR_%s", name);
  cur = (GdkCursor*)iupAttribGet(ih, str);
  if (cur)
    return cur;

  /* check the pre-defined IUP names first */
  for (i = 0; i < count; i++)
  {
    if (iupStrEqualNoCase(name, table[i].iupname)) 
    {
      if (table[i].sysname)
        cur = gdk_cursor_new(table[i].sysname);
      else
        cur = gtkEmptyCursor(ih);

      break;
    }
  }

  if (i == count)
  {
    /* check for a name defined cursor */
    cur = iupImageGetCursor(name);
  }

  /* save the cursor in cache */
  iupAttribSetStr(ih, str, (char*)cur);

  return cur;
}

int iupdrvBaseSetCursorAttrib(Ihandle* ih, const char* value)
{
  GdkCursor* cur = gtkGetCursor(ih, value);
  if (cur)
  {
    GdkWindow* window = iupgtkGetWindow(ih->handle);
    if (window)
      gdk_window_set_cursor(window, cur);
    return 1;
  }
  return 0;
}

void iupgdkColorSet(GdkColor* color, unsigned char r, unsigned char g, unsigned char b)
{
  color->red = iupCOLOR8TO16(r);
  color->green = iupCOLOR8TO16(g);
  color->blue = iupCOLOR8TO16(b);
  color->pixel = 0;
}

int iupdrvGetScrollbarSize(void)
{
  static int size = 0;

  if (size == 0)
  {
    GtkRequisition requisition;
    GtkWidget* win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget* sb = gtk_vscrollbar_new(NULL);
    gtk_container_add((GtkContainer*)win, sb);
    gtk_widget_realize(win);
    gtk_widget_size_request(sb, &requisition);
    size = requisition.width+1;
    gtk_widget_destroy(win);
  }

  return size;
}

void iupdrvDrawFocusRect(Ihandle* ih, void* _gc, int x, int y, int w, int h)
{
  GdkWindow* window = iupgtkGetWindow(ih->handle);
  GtkStyle *style = gtk_widget_get_style(ih->handle);
#if GTK_CHECK_VERSION(2, 18, 0)
  GtkStateType state = gtk_widget_get_state(ih->handle);
#else
  GtkStateType state = GTK_WIDGET_STATE(ih->handle);
#endif
  gtk_paint_focus(style, window, state, NULL, ih->handle, NULL, x, y, w, h);
  (void)_gc;
}

void iupdrvBaseRegisterCommonAttrib(Iclass* ic)
{
#ifndef GTK_MAC
#ifdef WIN32                                 
  iupClassRegisterAttribute(ic, "HFONT", iupgtkGetFontIdAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
#else
  iupClassRegisterAttribute(ic, "XFONTID", iupgtkGetFontIdAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
#endif
#endif
  iupClassRegisterAttribute(ic, "PANGOFONTDESC", iupgtkGetPangoFontDescAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "PANGOLAYOUT", iupgtkGetPangoLayoutAttrib, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);

  iupClassRegisterAttribute(ic, "TIPMARKUP", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TIPICON", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED);
}

static int gtkStrIsAscii(const char* str)
{
  while(*str)
  {
    int c = *str;
    if (c < 0)
      return 0;
    str++;
  }
  return 1;
}

static char* gtkStrToUTF8(const char *str, const char* charset)
{
  return g_convert(str, -1, "UTF-8", charset, NULL, NULL, NULL);
}

static char* gtkStrFromUTF8(const char *str, const char* charset)
{
  return g_convert(str, -1, charset, "UTF-8", NULL, NULL, NULL);
}

static char* gktLastConvertUTF8 = NULL;

void iupgtkReleaseConvertUTF8(void)
{
  if (gktLastConvertUTF8)
    g_free(gktLastConvertUTF8);
}

char* iupgtkStrConvertToUTF8(const char* str)  /* From IUP to GTK */
{
  if (!str || *str == 0)
    return (char*)str;

  if (iupgtk_utf8autoconvert)  /* this means str is in current locale */
  {
    const char *charset = NULL;
    if (g_get_charset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, -1, NULL))
        return (char*)str;
      else
      {
        if (gktLastConvertUTF8)
          g_free(gktLastConvertUTF8);
        gktLastConvertUTF8 = gtkStrToUTF8(str, "ISO8859-1");   /* if string is not UTF-8, assume ISO8859-1 */
        if (!gktLastConvertUTF8) return (char*)str;
        return gktLastConvertUTF8;
      }
    }
    else
    {
      if (gtkStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gktLastConvertUTF8)
          g_free(gktLastConvertUTF8);
        gktLastConvertUTF8 = gtkStrToUTF8(str, charset);
        if (!gktLastConvertUTF8) return (char*)str;
        return gktLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

char* iupgtkStrConvertFromUTF8(const char* str)  /* From GTK to IUP */
{
  if (!str || *str == 0)
    return (char*)str;

  if (iupgtk_utf8autoconvert)  /* this means str is in current locale */
  {
    const gchar *charset = NULL;
    if (g_get_charset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, -1, NULL))
        return (char*)str;
      else
      {
        if (gktLastConvertUTF8)
          g_free(gktLastConvertUTF8);
        gktLastConvertUTF8 = gtkStrFromUTF8(str, "ISO8859-1");  /* if string is not UTF-8, assume ISO8859-1 */
        if (!gktLastConvertUTF8) return (char*)str;
        return gktLastConvertUTF8;
      }
    }
    else
    {
      if (gtkStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gktLastConvertUTF8)
          g_free(gktLastConvertUTF8);
        gktLastConvertUTF8 = gtkStrFromUTF8(str, charset);
        if (!gktLastConvertUTF8) return (char*)str;
        return gktLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

static gboolean gtkGetFilenameCharset(const gchar **filename_charset)
{
  const gchar **charsets = NULL;
  gboolean is_utf8 = FALSE;
  
#if GTK_CHECK_VERSION(2, 6, 0)
  is_utf8 = g_get_filename_charsets (&charsets);
#endif

  if (filename_charset && charsets)
    *filename_charset = charsets[0];
  
  return is_utf8;
}

char* iupgtkStrConvertToFilename(const char* str)   /* From IUP to Filename */
{
  if (!str || *str == 0)
    return (char*)str;

  if (iupgtk_utf8autoconvert)  /* this means str is in current locale */
    return (char*)str;
  else
  {
    const gchar *charset = NULL;
    if (gtkGetFilenameCharset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, -1, NULL))
        return (char*)str;
      else
      {
        if (gktLastConvertUTF8)
          g_free(gktLastConvertUTF8);
        gktLastConvertUTF8 = gtkStrFromUTF8(str, "ISO8859-1");  /* if string is not UTF-8, assume ISO8859-1 */
        if (!gktLastConvertUTF8) return (char*)str;
        return gktLastConvertUTF8;
      }
    }
    else
    {
      if (gtkStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gktLastConvertUTF8)
          g_free(gktLastConvertUTF8);
        gktLastConvertUTF8 = gtkStrFromUTF8(str, charset);
        if (!gktLastConvertUTF8) return (char*)str;
        return gktLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

char* iupgtkStrConvertFromFilename(const char* str)   /* From Filename to IUP */
{
  if (!str || *str == 0)
    return (char*)str;

  if (iupgtk_utf8autoconvert)  /* this means str is in current locale */
    return (char*)str;
  else
  {
    const char *charset = NULL;
    if (gtkGetFilenameCharset(&charset)==TRUE)  /* current locale is already UTF-8 */
    {
      if (g_utf8_validate(str, -1, NULL))
        return (char*)str;
      else
      {
        if (gktLastConvertUTF8)
          g_free(gktLastConvertUTF8);
        gktLastConvertUTF8 = gtkStrToUTF8(str, "ISO8859-1");   /* if string is not UTF-8, assume ISO8859-1 */
        if (!gktLastConvertUTF8) return (char*)str;
        return gktLastConvertUTF8;
      }
    }
    else
    {
      if (gtkStrIsAscii(str) || !charset)
        return (char*)str;
      else if (charset)
      {
        if (gktLastConvertUTF8)
          g_free(gktLastConvertUTF8);
        gktLastConvertUTF8 = gtkStrToUTF8(str, charset);
        if (!gktLastConvertUTF8) return (char*)str;
        return gktLastConvertUTF8;
      }
    }
  }
  return (char*)str;
}

gboolean iupgtkMotionNotifyEvent(GtkWidget *widget, GdkEventMotion *evt, Ihandle *ih)
{
  IFniis cb;

  if (evt->is_hint)
  {
    int x, y;
    gdk_window_get_pointer(iupgtkGetWindow(widget), &x, &y, NULL);
    evt->x = x;
    evt->y = y;
  }

  cb = (IFniis)IupGetCallback(ih,"MOTION_CB");
  if (cb)
  {
    char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;
    iupgtkButtonKeySetStatus(evt->state, 0, status, 0);
    cb(ih, (int)evt->x, (int)evt->y, status);
  }

  return FALSE;
}

gboolean iupgtkButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih)
{
  IFniiiis cb = (IFniiiis)IupGetCallback(ih,"BUTTON_CB");
  if (cb)
  {
    int doubleclick = 0, ret, press = 1;
    int b = IUP_BUTTON1+(evt->button-1);
    char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;

    if (evt->type == GDK_BUTTON_RELEASE)
      press = 0;

    if (evt->type == GDK_2BUTTON_PRESS)
      doubleclick = 1;

    iupgtkButtonKeySetStatus(evt->state, evt->button, status, doubleclick);

    if (doubleclick)
    {
      /* Must compensate the fact that in GTK there is an extra button press event 
         when occours a double click, we compensate that completing the event 
         with a button release before the double click. */

      status[5] = ' '; /* clear double click */

      ret = cb(ih, b, 0, (int)evt->x, (int)evt->y, status);  /* release */
      if (ret==IUP_CLOSE)
        IupExitLoop();
      else if (ret==IUP_IGNORE)
        return TRUE;

      status[5] = 'D'; /* restore double click */
    }

    ret = cb(ih, b, press, (int)evt->x, (int)evt->y, status);
    if (ret==IUP_CLOSE)
      IupExitLoop();
    else if (ret==IUP_IGNORE)
      return TRUE;
  }

  (void)widget;
  return FALSE;
}

void iupdrvSendKey(int key, int press)
{
  Ihandle* focus;
  gint nkeys = 0; 
  GdkKeymapKey *keys; 
  GdkEventKey evt;
  memset(&evt, 0, sizeof(GdkEventKey));
  evt.send_event = TRUE;

  focus = IupGetFocus();
  if (!focus)
    return;
  evt.window = iupgtkGetWindow(focus->handle);

  iupdrvKeyEncode(key, &evt.keyval, &evt.state);
  if (!evt.keyval)
    return;

  if (!gdk_keymap_get_entries_for_keyval(gdk_keymap_get_default(), evt.keyval, &keys, &nkeys))
    return;
  evt.hardware_keycode = (guint16)keys[0].keycode;
  evt.group = (guint8)keys[0].group; 

  if (press & 0x01)
  {
    evt.type = GDK_KEY_PRESS;
    gdk_event_put((GdkEvent*)&evt);
  }

  if (press & 0x02)
  {
    evt.type = GDK_KEY_RELEASE;
    gdk_event_put((GdkEvent*)&evt);
  }
}

void iupdrvWarpPointer(int x, int y)
{
  /* VirtualBox does not reproduce the mouse move visually, but it is working. */
#if GTK_CHECK_VERSION(2, 8, 0)
  gdk_display_warp_pointer(gdk_display_get_default(), gdk_screen_get_default(), x, y);
#endif
}

void iupdrvSendMouse(int x, int y, int bt, int status)
{
  /* always update cursor */
  /* must be before sending the message because the cursor position will be used */
  /* this will also send an extra motion event */
  iupdrvWarpPointer(x, y);

  if (status != -1)
  {
    GtkWidget* grab_widget;
    gint origin_x, origin_y;

    GdkEventButton evt;
    memset(&evt, 0, sizeof(GdkEventButton));
    evt.send_event = TRUE;

    evt.x_root = x;
    evt.y_root = y;
    evt.type = (status==0)? GDK_BUTTON_RELEASE: ((status==2)? GDK_2BUTTON_PRESS: GDK_BUTTON_PRESS);
    evt.device = gdk_device_get_core_pointer();

    grab_widget = gtk_grab_get_current();
    if (grab_widget) 
      evt.window = iupgtkGetWindow(grab_widget);
    else
      evt.window = gdk_window_at_pointer(NULL, NULL);

    switch(bt)
    {
    case IUP_BUTTON1:
      evt.state = GDK_BUTTON1_MASK;
      evt.button = 1;
      break;
    case IUP_BUTTON2:
      evt.state = GDK_BUTTON2_MASK;
      evt.button = 2;
      break;
    case IUP_BUTTON3:
      evt.state = GDK_BUTTON3_MASK;
      evt.button = 3;
      break;
    case IUP_BUTTON4:
      evt.state = GDK_BUTTON4_MASK;
      evt.button = 4;
      break;
    case IUP_BUTTON5:
      evt.state = GDK_BUTTON5_MASK;
      evt.button = 5;
      break;
    default:
      return;
    }

    gdk_window_get_origin(evt.window, &origin_x, &origin_y);
    evt.x = x - origin_x;
    evt.y = y - origin_y;

    gdk_event_put((GdkEvent*)&evt);
  }
#if 0 /* kept until code stabilizes */
  else
  {
    GtkWidget* grab_widget;
    gint origin_x, origin_y;

    GdkEventMotion evt;
    memset(&evt, 0, sizeof(GdkEventMotion));
    evt.send_event = TRUE;

    evt.x_root = x;
    evt.y_root = y;
    evt.type = GDK_MOTION_NOTIFY;
    evt.device = gdk_device_get_core_pointer();

    grab_widget = gtk_grab_get_current();
    if (grab_widget) 
      evt.window = iupgtkGetWindow(grab_widget);
    else
      evt.window = gdk_window_at_pointer(NULL, NULL);

    switch(bt)
    {
    case IUP_BUTTON1:
      evt.state = GDK_BUTTON1_MASK;
      break;
    case IUP_BUTTON2:
      evt.state = GDK_BUTTON2_MASK;
      break;
    case IUP_BUTTON3:
      evt.state = GDK_BUTTON3_MASK;
      break;
    case IUP_BUTTON4:
      evt.state = GDK_BUTTON4_MASK;
      break;
    case IUP_BUTTON5:
      evt.state = GDK_BUTTON5_MASK;
      break;
    default:
      return;
    }

    gdk_window_get_origin(evt.window, &origin_x, &origin_y);
    evt.x = x - origin_x;
    evt.y = y - origin_y;

    gdk_event_put((GdkEvent*)&evt);
  }
#endif
}

void iupdrvSleep(int time)
{
  g_usleep(time*1000);  /* mili to micro */
}

GdkWindow* iupgtkGetWindow(GtkWidget *widget)
{
#if GTK_CHECK_VERSION(2, 14, 0)
  return gtk_widget_get_window(widget);
#else
  return widget->window;
#endif
}
