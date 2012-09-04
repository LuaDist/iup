/** \file
 * \brief GTK Driver Core
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>          
#include <stdlib.h>
#include <string.h>          

#include <gtk/gtk.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_object.h"
#include "iup_globalattrib.h"

#include "iupgtk_drv.h"

#ifdef GTK_MAC
#include <gdk/gdk.h>

char* iupgtkGetNativeWindowHandle(Ihandle* ih)
{
  GdkWindow* window = iupgtkGetWindow(ih->handle);
  if (window)
    return (char*)window;
  else
    return NULL;
}

void* iupgtkGetNativeGraphicsContext(GtkWidget* widget)
{
  return (void*)gdk_gc_new((GdkDrawable*)iupgtkGetWindow(widget));
}

void iupgtkReleaseNativeGraphicsContext(GtkWidget* widget, void* gc)
{
  g_object_unref(gc);
  (void)widget;
}

void* iupdrvGetDisplay(void)
{
  GdkDisplay* display = gdk_display_get_default();
  return display;
}

void iupgtkPushVisualAndColormap(void* visual, void* colormap)
{
  GdkColormap* gdk_colormap;
  GdkVisual *gdk_visual = gdk_visual_get_best();

  gdk_colormap = gdk_colormap_new(gdk_visual, FALSE);

  gtk_widget_push_colormap(gdk_colormap);

  /* gtk_widget_push_visual is now deprecated */
}

static void gtkSetDrvGlobalAttrib(void)
{
}

#else
#ifdef WIN32   /******************************** WIN32 ************************************/
#include <gdk/gdkwin32.h>

char* iupgtkGetNativeWindowHandle(Ihandle* ih)
{
  GdkWindow* window = iupgtkGetWindow(ih->handle);
  if (window)
    return (char*)GDK_WINDOW_HWND(window);
  else
    return NULL;
}

void* iupgtkGetNativeGraphicsContext(GtkWidget* widget)
{
  return GetDC(GDK_WINDOW_HWND(iupgtkGetWindow(widget)));
}

void iupgtkReleaseNativeGraphicsContext(GtkWidget* widget, void* gc)
{
  ReleaseDC(GDK_WINDOW_HWND(iupgtkGetWindow(widget)), (HDC)gc);
}

void* iupdrvGetDisplay(void)
{
  return NULL;
}

void iupgtkPushVisualAndColormap(void* visual, void* colormap)
{
  (void)visual;
  (void)colormap;
}

static void gtkSetDrvGlobalAttrib(void)
{
}

#else          /******************************** X11 ************************************/
#include <gdk/gdkx.h>

char* iupgtkGetNativeWindowHandle(Ihandle* ih)
{
  GdkWindow* window = iupgtkGetWindow(ih->handle);
  if (window)
    return (char*)GDK_WINDOW_XID(window);
  else
    return NULL;
}

void* iupgtkGetNativeGraphicsContext(GtkWidget* widget)
{
  GdkDisplay* display = gdk_display_get_default();
  return (void*)XCreateGC(GDK_DISPLAY_XDISPLAY(display), GDK_WINDOW_XID(iupgtkGetWindow(widget)), 0, NULL);
}

void iupgtkReleaseNativeGraphicsContext(GtkWidget* widget, void* gc)
{
  GdkDisplay* display = gdk_display_get_default();
  XFreeGC(GDK_DISPLAY_XDISPLAY(display), (GC)gc);
  (void)widget;
}

void* iupdrvGetDisplay(void)
{
  GdkDisplay* display = gdk_display_get_default();
  return GDK_DISPLAY_XDISPLAY(display);
}

void iupgtkPushVisualAndColormap(void* visual, void* colormap)
{
  GdkColormap* gdk_colormap;
  GdkVisual *gdk_visual = gdkx_visual_get(XVisualIDFromVisual((Visual*)visual));
  if (colormap)
    gdk_colormap = gdk_x11_colormap_foreign_new(gdk_visual, (Colormap)colormap);
  else
    gdk_colormap = gdk_colormap_new(gdk_visual, FALSE);

  gtk_widget_push_colormap(gdk_colormap);

  /* gtk_widget_push_visual is now deprecated */
}

static void gtkSetDrvGlobalAttrib(void)
{
  GdkDisplay* display = gdk_display_get_default();
  Display* xdisplay = GDK_DISPLAY_XDISPLAY(display);
  IupSetGlobal("XDISPLAY", (char*)xdisplay);
  IupSetGlobal("XSCREEN", (char*)XDefaultScreen(xdisplay));
  IupSetGlobal("XSERVERVENDOR", ServerVendor(xdisplay));
  IupSetfAttribute(NULL, "XVENDORRELEASE", "%d", VendorRelease(xdisplay));
}

#endif

#endif

static void gtkSetGlobalColorAttrib(const char* name, GdkColor *color)
{
  iupGlobalSetDefaultColorAttrib(name, (int)iupCOLOR16TO8(color->red), 
                                       (int)iupCOLOR16TO8(color->green), 
                                       (int)iupCOLOR16TO8(color->blue));
}

void iupgtkUpdateGlobalColors(GtkStyle* style)
{
  GdkColor color = style->bg[GTK_STATE_NORMAL];
  gtkSetGlobalColorAttrib("DLGBGCOLOR", &color);

  color = style->fg[GTK_STATE_NORMAL];
  gtkSetGlobalColorAttrib("DLGFGCOLOR", &color);

  color = style->base[GTK_STATE_NORMAL];
  gtkSetGlobalColorAttrib("TXTBGCOLOR", &color);

  color = style->text[GTK_STATE_NORMAL];
  gtkSetGlobalColorAttrib("TXTFGCOLOR", &color);
}

int iupdrvOpen(int *argc, char ***argv)
{
  char* value;
  GtkStyle* style;

  if (!gtk_init_check(argc, argv))
    return IUP_ERROR;
  
  IupSetGlobal("DRIVER", "GTK");

  IupStoreGlobal("SYSTEMLANGUAGE", pango_language_to_string(gtk_get_default_language()));

  /* driver system version */
  IupSetfAttribute(NULL, "GTKVERSION", "%d.%d.%d", gtk_major_version, 
                                                   gtk_minor_version, 
                                                   gtk_micro_version);
  IupSetfAttribute(NULL, "GTKDEVVERSION", "%d.%d.%d", GTK_MAJOR_VERSION, 
                                                      GTK_MINOR_VERSION, 
                                                      GTK_MICRO_VERSION);

  gtkSetDrvGlobalAttrib();

  style = gtk_style_new();
  iupgtkUpdateGlobalColors(style);
  IupSetGlobal("_IUP_RESET_GLOBALCOLORS", "YES");  /* will update the global colors when the first dialog is mapped */
  g_object_unref(style);

  IupSetGlobal("SHOWMENUIMAGES", "YES");

  value = getenv("UBUNTU_MENUPROXY");  /* for now only in Ubuntu */
  if (value && iupStrEqualNoCase(value, "libappmenu.so"))
    iupgtk_globalmenu = 1;
  
  return IUP_NOERROR;
}

void iupdrvClose(void)
{
  iupgtkReleaseConvertUTF8();
}
