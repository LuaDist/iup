/** \file
 * \brief GTK Driver iupdrvSetGlobal
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <string.h>

#include <gtk/gtk.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_key.h"

#include "iupgtk_drv.h"


int iupgtk_utf8autoconvert = 1;
int iupgtk_globalmenu = 0;


static void iGdkEventFunc(GdkEvent *evt, gpointer	data)
{
  switch(evt->type)
  {
  case GDK_BUTTON_PRESS:
  case GDK_2BUTTON_PRESS:
  case GDK_3BUTTON_PRESS:
  case GDK_BUTTON_RELEASE:
    {
      IFiiiis cb = (IFiiiis)IupGetFunction("GLOBALBUTTON_CB");
      if (cb)
      {
        GdkEventButton* evt_button = (GdkEventButton*)evt;
        gint win_x = 0, win_y = 0;
        int doubleclick = 0, press = 1;
        int b = IUP_BUTTON1+(evt_button->button-1);
        char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;
        int x = (int)evt_button->x;
        int y = (int)evt_button->y;

        if (evt_button->type == GDK_BUTTON_RELEASE)
          press = 0;

        if (evt_button->type == GDK_2BUTTON_PRESS)
          doubleclick = 1;

        iupgtkButtonKeySetStatus(evt_button->state, evt_button->button, status, doubleclick);

        gdk_window_get_origin(evt_button->window, &win_x, &win_y);  /* GDK window relative to screen */
        x += win_x;
        y += win_y;

        if (doubleclick)
        {
          /* Must compensate the fact that in GTK there is an extra button press event 
             when occours a double click, we compensate that completing the event 
             with a button release before the double click. */
          status[5] = ' '; /* clear double click */
          cb(b, 0, x, y, status);  /* release */
          status[5] = 'D'; /* restore double click */
        }

        cb(b, press, x, y, status);
      }
      break;
    }
  case GDK_MOTION_NOTIFY:
    {
      IFiis cb = (IFiis)IupGetFunction("GLOBALMOTION_CB");
      if (cb)
      {
        GdkEventMotion* evt_motion = (GdkEventMotion*)evt;
        gint win_x = 0, win_y = 0;
        int x = (int)evt_motion->x;
        int y = (int)evt_motion->y;
        char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;

        iupgtkButtonKeySetStatus(evt_motion->state, 0, status, 0);

        if (evt_motion->is_hint)
          gdk_window_get_pointer(evt_motion->window, &x, &y, NULL);

        gdk_window_get_origin(evt_motion->window, &win_x, &win_y);  /* GDK window relative to screen */
        x += win_x;
        y += win_y;

        cb(x, y, status);
      }
      break;
    }
  case GDK_KEY_PRESS:
  case GDK_KEY_RELEASE:
    {
      IFii cb = (IFii)IupGetFunction("GLOBALKEYPRESS_CB");
      if (cb)
      {
        int pressed = (evt->type==GDK_KEY_PRESS)? 1: 0;
        int code = iupgtkKeyDecode((GdkEventKey*)evt);
        if (code != 0)
          cb(code, pressed);
      }
      break;
    }
  default:
    break;
  }

  (void)data;
  gtk_main_do_event(evt);
}

int iupdrvSetGlobal(const char *name, const char *value)
{
  if (iupStrEqual(name, "INPUTCALLBACKS"))
  {
    if (iupStrBoolean(value))
      gdk_event_handler_set(iGdkEventFunc, NULL, NULL);
    else 
      gdk_event_handler_set((GdkEventFunc)gtk_main_do_event, NULL, NULL);
    return 1;
  }
  if (iupStrEqual(name, "UTF8AUTOCONVERT"))
  {
    if (!value || iupStrBoolean(value))
      iupgtk_utf8autoconvert = 1;
    else
      iupgtk_utf8autoconvert = 0;
    return 0;
  }
  if (iupStrEqual(name, "SHOWMENUIMAGES"))
  {
    /* make sure the type is realized */
    g_type_class_unref (g_type_class_ref (GTK_TYPE_IMAGE_MENU_ITEM));

    if (iupStrBoolean(value))
      g_object_set (gtk_settings_get_default (), "gtk-menu-images", TRUE, NULL);
    else
      g_object_set (gtk_settings_get_default (), "gtk-menu-images", FALSE, NULL);
  }
  if (iupStrEqual(name, "GLOBALMENU"))
  {
    if (iupStrBoolean(value))
      iupgtk_globalmenu = 1;
    else
      iupgtk_globalmenu = 0;
    return 0;
  }
  return 1;
}

int iupdrvCheckMainScreen(int *w, int *h)
{
  GdkScreen *screen = gdk_screen_get_default();
  int monitors_count = gdk_screen_get_n_monitors(screen);
  if (monitors_count > 1)
  {
    GdkRectangle rect;
    gdk_screen_get_monitor_geometry(screen, gdk_screen_get_monitor_at_point(screen, 0, 0), &rect);
    *w = rect.width;
    *h = rect.height;
    return 1;
  }
  return 0;
}

char *iupdrvGetGlobal(const char *name)
{
  if (iupStrEqual(name, "VIRTUALSCREEN"))
  {
    char *str = iupStrGetMemory(50);
    GdkScreen *screen = gdk_screen_get_default();
    GdkWindow *root = gdk_screen_get_root_window(gdk_screen_get_default());
    int x = 0;
    int y = 0;
    int w = gdk_screen_get_width(screen); 
    int h = gdk_screen_get_height(screen);
    gdk_window_get_root_origin(root, &x, &y);
    sprintf(str, "%d %d %d %d", x, y, w, h);
    return str;
  }
  if (iupStrEqual(name, "MONITORSINFO"))
  {
    int i;
    GdkScreen *screen = gdk_screen_get_default();
    int monitors_count = gdk_screen_get_n_monitors(screen);
    char *str = iupStrGetMemory(monitors_count*50);
    char* pstr = str;
    GdkRectangle rect;

    for (i=0; i < monitors_count; i++)
    {
      gdk_screen_get_monitor_geometry(screen, i, &rect);
      pstr += sprintf(pstr, "%d %d %d %d\n", rect.x, rect.y, rect.width, rect.height);
    }

    return str;
  }
  if (iupStrEqual(name, "TRUECOLORCANVAS"))
  {
    if (gdk_visual_get_best_depth() > 8)
      return "YES";
    else
      return "NO";
  }
  if (iupStrEqual(name, "UTF8AUTOCONVERT"))
  {
    if (iupgtk_utf8autoconvert)
      return "YES";
    else
      return "NO";
  }
  if (iupStrEqual(name, "SHOWMENUIMAGES"))
  {
    gboolean menu_images;
    g_object_get (gtk_settings_get_default (), "gtk-menu-images", &menu_images, NULL);
    if (menu_images)
      return "YES";
    else
      return "NO";
  }
  if (iupStrEqual(name, "GLOBALMENU"))
  {
    if (iupgtk_globalmenu)
      return "YES";
    else
      return "NO";
  }
  return NULL;
}
