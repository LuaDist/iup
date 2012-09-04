/** \file
 * \brief Canvas Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_dialog.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_drvfont.h"
#include "iup_canvas.h"
#include "iup_key.h"

#include "iupgtk_drv.h"


static int gtkCanvasScroll2Iup(GtkScrollType scroll, int vert)
{
  switch(scroll)
  {
    case GTK_SCROLL_STEP_UP:
      return IUP_SBUP;
    case GTK_SCROLL_STEP_DOWN:
      return IUP_SBDN;
    case GTK_SCROLL_PAGE_UP:
      return IUP_SBPGUP;
    case GTK_SCROLL_PAGE_DOWN:
      return IUP_SBPGDN;
    case GTK_SCROLL_STEP_LEFT:
      return IUP_SBLEFT;
    case GTK_SCROLL_STEP_RIGHT:
      return IUP_SBRIGHT;
    case GTK_SCROLL_PAGE_LEFT:
      return IUP_SBPGLEFT;
    case GTK_SCROLL_PAGE_RIGHT:
      return IUP_SBPGRIGHT;
    case GTK_SCROLL_STEP_BACKWARD:
      return vert? IUP_SBUP: IUP_SBLEFT;
    case GTK_SCROLL_STEP_FORWARD:
      return vert? IUP_SBDN: IUP_SBRIGHT;
    case GTK_SCROLL_PAGE_BACKWARD:
      return vert? IUP_SBPGUP: IUP_SBPGLEFT;
    case GTK_SCROLL_PAGE_FORWARD:
      return vert? IUP_SBPGDN: IUP_SBPGRIGHT;
    case GTK_SCROLL_JUMP:
    case GTK_SCROLL_START:
    case GTK_SCROLL_END:
      return vert? IUP_SBPOSV: IUP_SBPOSH;
    case GTK_SCROLL_NONE:
      return -1;
  }

  /* No IUP_SBDRAGV or IUP_SBDRAGH support in GTK */

  return -1;
}

static gboolean gtkCanvasHChangeValue(GtkRange *range, GtkScrollType scroll, double value, Ihandle *ih)
{
  double posx, posy;
  IFniff cb;

  double xmin = iupAttribGetFloat(ih, "XMIN");
  double xmax = iupAttribGetFloat(ih, "XMAX");
  double dx = iupAttribGetFloat(ih, "DX");
  if (value < xmin) value = xmin;
  if (value > xmax-dx) value = xmax-dx;

  posx = value;
  ih->data->posx = (float)posx;
  posy = ih->data->posy;

  cb = (IFniff)IupGetCallback(ih,"SCROLL_CB");
  if (cb)
  {
    int op = gtkCanvasScroll2Iup(scroll, 0);
    if (op == -1)
      return FALSE;

    cb(ih, op, (float)posx, (float)posy);
  }
  else
  {
    IFnff cb = (IFnff)IupGetCallback(ih,"ACTION");
    if (cb)
      cb (ih, (float)posx, (float)posy);
  }

  (void)range;
  return FALSE;
}

static gboolean gtkCanvasVChangeValue(GtkRange *range, GtkScrollType scroll, double value, Ihandle *ih)
{
  double posx, posy;
  IFniff cb;

  double ymin = iupAttribGetFloat(ih, "YMIN");
  double ymax = iupAttribGetFloat(ih, "YMAX");
  double dy = iupAttribGetFloat(ih, "DY");
  if (value < ymin) value = ymin;
  if (value > ymax-dy) value = ymax-dy;

  posy = value;
  ih->data->posy = (float)posy;
  posx = ih->data->posx;

  cb = (IFniff)IupGetCallback(ih,"SCROLL_CB");
  if (cb)
  {
    int op = gtkCanvasScroll2Iup(scroll, 1);
    if (op == -1)
      return FALSE;

    cb(ih, op, (float)posx, (float)posy);
  }
  else
  {
    IFnff cb = (IFnff)IupGetCallback(ih,"ACTION");
    if (cb)
      cb (ih, (float)posx, (float)posy);
  }

  (void)range;
  return FALSE;
}

static gboolean gtkCanvasScrollEvent(GtkWidget *widget, GdkEventScroll *evt, Ihandle *ih)
{    
  IFnfiis wcb = (IFnfiis)IupGetCallback(ih, "WHEEL_CB");
  if (wcb)
  {
    int delta = evt->direction==GDK_SCROLL_UP||evt->direction==GDK_SCROLL_LEFT? 1: -1;
    char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;
    int button = evt->direction==GDK_SCROLL_UP||evt->direction==GDK_SCROLL_LEFT? 4: 5;
    iupgtkButtonKeySetStatus(evt->state, button, status, 0);

    wcb(ih, (float)delta, (int)evt->x, (int)evt->y, status);
  }
  else
  {
    IFniff scb = (IFniff)IupGetCallback(ih,"SCROLL_CB");
    int delta = evt->direction==GDK_SCROLL_UP||evt->direction==GDK_SCROLL_LEFT? 1: -1;

    if (evt->direction==GDK_SCROLL_UP || evt->direction==GDK_SCROLL_DOWN)
    {
      float posy = ih->data->posy;
      posy -= delta*iupAttribGetFloat(ih, "DY")/10.0f;
      IupSetfAttribute(ih, "POSY", "%g", posy);
    }
    else
    {
      float posx = ih->data->posx;
      posx -= delta*iupAttribGetFloat(ih, "DX")/10.0f;
      IupSetfAttribute(ih, "POSX", "%g", posx);
    }

    if (scb)
    {
      int scroll_gtk2iup[4] = {IUP_SBUP, IUP_SBDN, IUP_SBLEFT, IUP_SBRIGHT};
      int op = scroll_gtk2iup[evt->direction];
      scb(ih,op,ih->data->posx,ih->data->posy);
    }
  }
  (void)widget;
  return TRUE;
}

static gboolean gtkCanvasButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih)
{
  if (evt->type == GDK_BUTTON_PRESS)
  {
    /* Force focus on canvas click */
    if (iupAttribGetBoolean(ih, "CANFOCUS"))
      gtk_widget_grab_focus(ih->handle);
  }

  return iupgtkButtonEvent(widget, evt, ih);
}

static int gtkCanvasSetBgColorAttrib(Ihandle* ih, const char* value);

static gboolean gtkCanvasExposeEvent(GtkWidget *widget, GdkEventExpose *evt, Ihandle *ih)
{
  IFnff cb = (IFnff)IupGetCallback(ih,"ACTION");
  if (cb)
  {
    if (!iupAttribGet(ih, "_IUPGTK_NO_BGCOLOR"))
      gtkCanvasSetBgColorAttrib(ih, iupAttribGetStr(ih, "BGCOLOR"));  /* reset to update window attributes */

    iupAttribSetStrf(ih, "CLIPRECT", "%d %d %d %d", evt->area.x, evt->area.y, evt->area.x+evt->area.width-1, evt->area.y+evt->area.height-1);
    cb(ih,ih->data->posx,ih->data->posy);
    iupAttribSetStr(ih, "CLIPRECT", NULL);
  }

  (void)widget;
  return TRUE;  /* stop other handlers */
}

static void gtkCanvasLayoutUpdateMethod(Ihandle *ih)
{
  GdkWindow* window = iupgtkGetWindow(ih->handle);

  iupdrvBaseLayoutUpdateMethod(ih);

  /* Force GdkWindow size update when not visible,
     so when mapped before show the function gdk_drawable_get_size returns the correct value. */
  if (!iupdrvIsVisible(ih))
    gdk_window_resize(window, ih->currentwidth, ih->currentheight);

  if (iupAttribGetStr(ih, "_IUP_GTK_FIRST_RESIZE"))
  {
    /* GTK is nor calling gtkCanvasConfigureEvent on the first resize */
    IFnii cb = (IFnii)IupGetCallback(ih,"RESIZE_CB");
    iupAttribSetStr(ih, "_IUP_GTK_FIRST_RESIZE", NULL);
    if (cb)
    {
      int sb_w = 0, sb_h = 0;

      if (ih->data->sb)
      {
        int sb_size = iupdrvGetScrollbarSize();
        if (ih->data->sb & IUP_SB_HORIZ)
          sb_h += sb_size;  /* sb horizontal affects vertical size */
        if (ih->data->sb & IUP_SB_VERT)
          sb_w += sb_size;  /* sb vertical affects horizontal size */
      }

      if (iupAttribGetBoolean(ih, "BORDER"))
      {
        sb_w += 4;
        sb_h += 4;
      }

      cb(ih, ih->currentwidth-sb_w, ih->currentheight-sb_h);
    }
  }
}

static gboolean gtkCanvasConfigureEvent(GtkWidget *widget, GdkEventConfigure *evt, Ihandle *ih)
{
  IFnii cb = (IFnii)IupGetCallback(ih,"RESIZE_CB");
  if (cb)
    cb(ih,evt->width,evt->height);

  (void)widget;
  return FALSE;
}

static GtkScrolledWindow* gtkCanvasGetScrolledWindow(Ihandle* ih)
{
  return (GtkScrolledWindow*)iupAttribGet(ih, "_IUP_EXTRAPARENT");
}

static int gtkCanvasSetXAutoHideAttrib(Ihandle* ih, const char *value)
{
  GtkPolicyType vscrollbar_policy;
  gtk_scrolled_window_get_policy(gtkCanvasGetScrolledWindow(ih), NULL, &vscrollbar_policy);

  if (ih->data->sb & IUP_SB_HORIZ)
  {
    GtkPolicyType hscrollbar_policy;

    if (iupStrBoolean(value))
      hscrollbar_policy = GTK_POLICY_AUTOMATIC;
    else
      hscrollbar_policy = GTK_POLICY_ALWAYS;

    gtk_scrolled_window_set_policy(gtkCanvasGetScrolledWindow(ih), hscrollbar_policy, vscrollbar_policy);
  }
  else
    gtk_scrolled_window_set_policy(gtkCanvasGetScrolledWindow(ih), GTK_POLICY_NEVER, vscrollbar_policy);

  return 1;
}

static int gtkCanvasSetYAutoHideAttrib(Ihandle* ih, const char *value)
{
  GtkPolicyType hscrollbar_policy;
  gtk_scrolled_window_get_policy(gtkCanvasGetScrolledWindow(ih), &hscrollbar_policy, NULL);

  if (ih->data->sb & IUP_SB_VERT)
  {
    GtkPolicyType vscrollbar_policy;

    if (iupStrBoolean(value))
      vscrollbar_policy = GTK_POLICY_AUTOMATIC;
    else
      vscrollbar_policy = GTK_POLICY_ALWAYS;

    gtk_scrolled_window_set_policy(gtkCanvasGetScrolledWindow(ih), hscrollbar_policy, vscrollbar_policy);
  }
  else
    gtk_scrolled_window_set_policy(gtkCanvasGetScrolledWindow(ih), hscrollbar_policy, GTK_POLICY_NEVER);

  return 1;
}

static int gtkCanvasCheckScroll(double min, double max, double *page, double *pos)
{
  double old_pos = *pos;
  double range = max-min;
  if (*page > range) *page = range;
  if (*page <= 0) *page = range/10.;

  if (*pos < min) *pos = min;
  if (*pos > (max - *page)) *pos = max - *page;

  if (old_pos == *pos)
    return 0;
  else
    return 1;
}

static int gtkCanvasSetDXAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_HORIZ)
  {
    double xmin, xmax, linex;
    float dx;
    int value_changed;
    GtkAdjustment* sb_horiz = gtk_scrolled_window_get_hadjustment(gtkCanvasGetScrolledWindow(ih));
    if (!sb_horiz) return 1;

    if (!iupStrToFloat(value, &dx))
      return 1;

    xmin = iupAttribGetFloat(ih, "XMIN");
    xmax = iupAttribGetFloat(ih, "XMAX");

    if (!iupAttribGet(ih,"LINEX"))
    {
      linex = dx/10;
      if (linex==0)
        linex = 1;
    }
    else
      linex = iupAttribGetFloat(ih,"LINEX");

#if GTK_CHECK_VERSION(2, 14, 0)
    {
      double page_size = dx;
      double dvalue = gtk_adjustment_get_value(sb_horiz);
      value_changed = gtkCanvasCheckScroll(xmin, xmax, &page_size, &dvalue);
      gtk_adjustment_configure(sb_horiz, dvalue, xmin, xmax, linex, page_size, dx);
    }
#else
    sb_horiz->lower = xmin;
    sb_horiz->upper = xmax;
    sb_horiz->step_increment = linex;
    sb_horiz->page_size = dx;

    value_changed = gtkCanvasCheckScroll(xmin, xmax, &sb_horiz->page_size, &sb_horiz->value);
    sb_horiz->page_increment = sb_horiz->page_size;

    gtk_adjustment_changed(sb_horiz);
#endif

    if (value_changed)
      gtk_adjustment_value_changed(sb_horiz);
  }
  return 1;
}

static int gtkCanvasSetDYAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_VERT)
  {
    double ymin, ymax, liney;
    float dy;
    int value_changed;
    GtkAdjustment* sb_vert = gtk_scrolled_window_get_vadjustment(gtkCanvasGetScrolledWindow(ih));
    if (!sb_vert) return 1;

    if (!iupStrToFloat(value, &dy))
      return 1;

    ymin = iupAttribGetFloat(ih, "YMIN");
    ymax = iupAttribGetFloat(ih, "YMAX");

    if (!iupAttribGet(ih,"LINEY"))
    {
      liney = dy/10;
      if (liney==0)
        liney = 1;
    }
    else
      liney = iupAttribGetFloat(ih,"LINEY");

#if GTK_CHECK_VERSION(2, 14, 0)
    {
      double page_size = dy;
      double dvalue = gtk_adjustment_get_value(sb_vert);
      value_changed = gtkCanvasCheckScroll(ymin, ymax, &page_size, &dvalue);
      gtk_adjustment_configure(sb_vert, dvalue, ymin, ymax, liney, page_size, dy);
    }
#else
    sb_vert->lower = ymin;
    sb_vert->upper = ymax;
    sb_vert->step_increment = liney;
    sb_vert->page_size = dy;

    value_changed = gtkCanvasCheckScroll(ymin, ymax, &sb_vert->page_size, &sb_vert->value);
    sb_vert->page_increment = sb_vert->page_size;

    gtk_adjustment_changed(sb_vert);
#endif

    if (value_changed)
      gtk_adjustment_value_changed(sb_vert);
  }
  return 1;
}

static int gtkCanvasSetPosXAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_HORIZ)
  {
    float posx, xmin, xmax, dx;
    GtkAdjustment* sb_horiz = gtk_scrolled_window_get_hadjustment(gtkCanvasGetScrolledWindow(ih));
    if (!sb_horiz) return 1;

    if (!iupStrToFloat(value, &posx))
      return 1;

    xmin = iupAttribGetFloat(ih, "XMIN");
    xmax = iupAttribGetFloat(ih, "XMAX");
    dx = iupAttribGetFloat(ih, "DX");

    if (posx < xmin) posx = xmin;
    if (posx > (xmax - dx)) posx = xmax - dx;
    ih->data->posx = posx;

    gtk_adjustment_set_value(sb_horiz, posx);
  }
  return 1;
}

static int gtkCanvasSetPosYAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_VERT)
  {
    float posy, ymin, ymax, dy;
    GtkAdjustment* sb_vert = gtk_scrolled_window_get_vadjustment(gtkCanvasGetScrolledWindow(ih));
    if (!sb_vert) return 1;

    if (!iupStrToFloat(value, &posy))
      return 1;

    ymin = iupAttribGetFloat(ih, "YMIN");
    ymax = iupAttribGetFloat(ih, "YMAX");
    dy = iupAttribGetFloat(ih, "DY");

    if (posy < ymin) posy = ymin;
    if (posy > (ymax - dy)) posy = ymax - dy;
    ih->data->posy = posy;

    gtk_adjustment_set_value(sb_vert, posy);
  }
  return 1;
}

static int gtkCanvasSetBgColorAttrib(Ihandle* ih, const char* value)
{
  GtkScrolledWindow* scrolled_window = gtkCanvasGetScrolledWindow(ih);
  unsigned char r, g, b;

  /* ignore given value, must use only from parent for the scrollbars */
  char* parent_value = iupBaseNativeParentGetBgColor(ih);

  if (iupStrToRGB(parent_value, &r, &g, &b))
  {
    GtkWidget* sb;

    iupgtkBaseSetBgColor((GtkWidget*)scrolled_window, r, g, b);

#if GTK_CHECK_VERSION(2, 8, 0)
    sb = gtk_scrolled_window_get_hscrollbar(scrolled_window);
    if (sb) iupgtkBaseSetBgColor(sb, r, g, b);
    sb = gtk_scrolled_window_get_vscrollbar(scrolled_window);
    if (sb) iupgtkBaseSetBgColor(sb, r, g, b);
#endif
  }

  if (!IupGetCallback(ih, "ACTION")) 
  {
    /* enable automatic double buffering */
    gtk_widget_set_double_buffered(ih->handle, TRUE);
    gtk_widget_set_double_buffered((GtkWidget*)scrolled_window, TRUE);
    return iupdrvBaseSetBgColorAttrib(ih, value);
  }
  else
  {
    /* disable automatic double buffering */
    gtk_widget_set_double_buffered(ih->handle, FALSE);
    gtk_widget_set_double_buffered((GtkWidget*)scrolled_window, FALSE);
    gdk_window_set_back_pixmap(iupgtkGetWindow(ih->handle), NULL, FALSE);
    iupAttribSetStr(ih, "_IUPGTK_NO_BGCOLOR", "1");
    return 1;
  }
}

static char* gtkCanvasGetDrawSizeAttrib(Ihandle *ih)
{
  char* str = iupStrGetMemory(20);
  int w, h;
  GdkWindow* window = iupgtkGetWindow(ih->handle);

  if (window)
    gdk_drawable_get_size(window, &w, &h);
  else
    return NULL;

  sprintf(str, "%dx%d", w, h);
  return str;
}

static char* gtkCanvasGetDrawableAttrib(Ihandle* ih)
{
  return (char*)iupgtkGetWindow(ih->handle);
}

static void gtkCanvasDummyLogFunc(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
  /* does nothing */
  (void)log_domain;
  (void)log_level;
  (void)message;
  (void)user_data;
}

static int gtkCanvasMapMethod(Ihandle* ih)
{
  GtkScrolledWindow* scrolled_window;
  void* visual;

  if (!ih->parent)
    return IUP_ERROR;

  ih->data->sb = iupBaseGetScrollbar(ih);

  visual = (void*)IupGetAttribute(ih, "VISUAL");   /* defined by the OpenGL Canvas in X11 or NULL */
  if (visual)
    iupgtkPushVisualAndColormap(visual, (void*)iupAttribGet(ih, "COLORMAP"));

  ih->handle = gtk_drawing_area_new();

  if (visual)
    gtk_widget_pop_colormap();

  if (!ih->handle)
      return IUP_ERROR;

#if GTK_CHECK_VERSION(2, 18, 0)
  /* CD will NOT work properly without this, must use always the CD-GDK driver */
  gtk_widget_set_has_window(ih->handle, TRUE);  
#endif

  scrolled_window = (GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
  if (!scrolled_window)
    return IUP_ERROR;

  {
    /* to avoid the "cannot add non scrollable widget" warning */
#if GTK_CHECK_VERSION(2, 6, 0)
    GLogFunc def_func = g_log_set_default_handler(gtkCanvasDummyLogFunc, NULL);
#endif
    gtk_container_add((GtkContainer*)scrolled_window, ih->handle);
#if GTK_CHECK_VERSION(2, 6, 0)
    g_log_set_default_handler(def_func, NULL);
#endif
  }

  gtk_widget_show((GtkWidget*)scrolled_window);

  iupAttribSetStr(ih, "_IUP_EXTRAPARENT", (char*)scrolled_window);

  /* add to the parent, all GTK controls must call this. */
  iupgtkBaseAddToParent(ih);

  g_signal_connect(G_OBJECT(ih->handle), "focus-in-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-out-event",    G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "key-press-event",    G_CALLBACK(iupgtkKeyPressEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "key-release-event",  G_CALLBACK(iupgtkKeyReleaseEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "show-help",          G_CALLBACK(iupgtkShowHelp), ih);

  g_signal_connect(G_OBJECT(ih->handle), "expose-event",       G_CALLBACK(gtkCanvasExposeEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "button-press-event", G_CALLBACK(gtkCanvasButtonEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "button-release-event",G_CALLBACK(gtkCanvasButtonEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "motion-notify-event",G_CALLBACK(iupgtkMotionNotifyEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "scroll-event",G_CALLBACK(gtkCanvasScrollEvent), ih);

#if GTK_CHECK_VERSION(2, 8, 0)
  g_signal_connect(G_OBJECT(gtk_scrolled_window_get_hscrollbar(scrolled_window)), "change-value",G_CALLBACK(gtkCanvasHChangeValue), ih);
  g_signal_connect(G_OBJECT(gtk_scrolled_window_get_vscrollbar(scrolled_window)), "change-value",G_CALLBACK(gtkCanvasVChangeValue), ih);
#endif

  /* To receive mouse events on a drawing area, you will need to enable them. */
  gtk_widget_add_events(ih->handle, GDK_EXPOSURE_MASK|
    GDK_POINTER_MOTION_MASK|GDK_POINTER_MOTION_HINT_MASK|
    GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK|GDK_BUTTON_MOTION_MASK|
    GDK_KEY_PRESS_MASK|GDK_KEY_RELEASE_MASK|
    GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK|
    GDK_FOCUS_CHANGE_MASK|GDK_STRUCTURE_MASK);

  /* To receive keyboard events, you will need to set the GTK_CAN_FOCUS flag on the drawing area. */
  if (ih->iclass->is_interactive)
  {
    if (iupAttribGetBoolean(ih, "CANFOCUS"))
      iupgtkSetCanFocus(ih->handle, 1);
  }

  if (iupAttribGetBoolean(ih, "BORDER"))
    gtk_scrolled_window_set_shadow_type(scrolled_window, GTK_SHADOW_IN); 
  else
    gtk_scrolled_window_set_shadow_type(scrolled_window, GTK_SHADOW_NONE);

  gtk_widget_realize((GtkWidget*)scrolled_window);
  gtk_widget_realize(ih->handle);

  /* must be connected after realize or a RESIZE_CB will happen before MAP_CB
    works only for the GtkDrawingArea. */
  g_signal_connect(G_OBJECT(ih->handle), "configure-event", G_CALLBACK(gtkCanvasConfigureEvent), ih);
  iupAttribSetStr(ih, "_IUP_GTK_FIRST_RESIZE", "1");

  /* configure for DRAG&DROP */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSetStr(ih, "DROPFILESTARGET", "YES");

  /* update a mnemonic in a label if necessary */
  iupgtkUpdateMnemonic(ih);

  /* configure scrollbar */
  if (ih->data->sb)
  {
    GtkPolicyType hscrollbar_policy = GTK_POLICY_NEVER, vscrollbar_policy = GTK_POLICY_NEVER;
    if (ih->data->sb & IUP_SB_HORIZ)
      hscrollbar_policy = GTK_POLICY_AUTOMATIC;
    if (ih->data->sb & IUP_SB_VERT)
      vscrollbar_policy = GTK_POLICY_AUTOMATIC;
    gtk_scrolled_window_set_policy(scrolled_window, hscrollbar_policy, vscrollbar_policy);
  }
  else
    gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_NEVER);

  /* force the update of BGCOLOR here, to let derived classes ignore it if ACTION is defined */
  gtkCanvasSetBgColorAttrib(ih, iupAttribGetStr(ih, "BGCOLOR"));
    
  return IUP_NOERROR;
}

void iupdrvCanvasInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkCanvasMapMethod;
  ic->LayoutUpdate = gtkCanvasLayoutUpdateMethod;

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkCanvasSetBgColorAttrib, "255 255 255", NULL, IUPAF_DEFAULT);  /* force new default value */

  /* IupCanvas only */
  iupClassRegisterAttribute(ic, "CURSOR", NULL, iupdrvBaseSetCursorAttrib, IUPAF_SAMEASSYSTEM, "ARROW", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAWSIZE", gtkCanvasGetDrawSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DX", NULL, gtkCanvasSetDXAttrib, "0.1", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "DY", NULL, gtkCanvasSetDYAttrib, "0.1", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "POSX", iupCanvasGetPosXAttrib, gtkCanvasSetPosXAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "POSY", iupCanvasGetPosYAttrib, gtkCanvasSetPosYAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "XAUTOHIDE", NULL, gtkCanvasSetXAutoHideAttrib, "YES", NULL, IUPAF_DEFAULT);  /* force new default value */
  iupClassRegisterAttribute(ic, "YAUTOHIDE", NULL, gtkCanvasSetYAutoHideAttrib, "YES", NULL, IUPAF_DEFAULT);  /* force new default value */

  iupClassRegisterAttribute(ic, "DRAWABLE", gtkCanvasGetDrawableAttrib, NULL, NULL, NULL, IUPAF_NO_STRING);

  /* IupCanvas Windows or X only */
#ifndef GTK_MAC
  #ifdef WIN32                                 
    iupClassRegisterAttribute(ic, "HWND", iupgtkGetNativeWindowHandle, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_NO_INHERIT);
  #else
    iupClassRegisterAttribute(ic, "XWINDOW", iupgtkGetNativeWindowHandle, NULL, NULL, NULL, IUPAF_NO_INHERIT|IUPAF_NO_STRING);
    iupClassRegisterAttribute(ic, "XDISPLAY", (IattribGetFunc)iupdrvGetDisplay, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  #endif
#endif

  /* Not Supported */
  iupClassRegisterAttribute(ic, "BACKINGSTORE", NULL, NULL, "YES", NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TOUCH", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}
