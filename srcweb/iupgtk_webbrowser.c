/** \file
 * \brief Web Browser Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>

#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>

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
#include "iup_webbrowser.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_key.h"
#include "iup_register.h"

#include "iupgtk_drv.h"


#ifndef WEBKIT_LOAD_FAILED
#define WEBKIT_LOAD_FAILED 4
#endif

struct _IcontrolData 
{
  int sb;    /* scrollbar configuration, valid only after map, use iupBaseGetScrollbar before map */
};

static char* gtkWebBrowserGetItemHistoryAttrib(Ihandle* ih, int id)
{
  WebKitWebBackForwardList *back_forward_list = webkit_web_view_get_back_forward_list ((WebKitWebView*)ih->handle);
  WebKitWebHistoryItem* item = webkit_web_back_forward_list_get_nth_item(back_forward_list, id);
  if (item)
  {
    char* str = iupStrGetMemory(1024);
    sprintf(str, "%s", webkit_web_history_item_get_uri(item));
    return str;
  }
  else
    return NULL;
}

static char* gtkWebBrowserGetForwardCountAttrib(Ihandle* ih)
{
  WebKitWebBackForwardList *back_forward_list = webkit_web_view_get_back_forward_list ((WebKitWebView*)ih->handle);
  char* str = iupStrGetMemory(30);
  sprintf(str, "%d", webkit_web_back_forward_list_get_forward_length(back_forward_list));
  return str;
}

static char* gtkWebBrowserGetBackCountAttrib(Ihandle* ih)
{
  WebKitWebBackForwardList *back_forward_list = webkit_web_view_get_back_forward_list ((WebKitWebView*)ih->handle);
  char* str = iupStrGetMemory(30);
  sprintf(str, "%d", webkit_web_back_forward_list_get_back_length(back_forward_list));
  return str;
}

static int gtkWebBrowserSetHTMLAttrib(Ihandle* ih, const char* value)
{
  if (value)
    webkit_web_view_load_string((WebKitWebView*)ih->handle, iupgtkStrConvertToUTF8(value), "text/html", "UTF-8", "");
  return 0; /* do not store value in hash table */
}

static char* gtkWebBrowserGetStatusAttrib(Ihandle* ih)
{
  WebKitLoadStatus status = webkit_web_view_get_load_status((WebKitWebView*)ih->handle);
  if (status == WEBKIT_LOAD_FAILED)
    return "FAILED";
  else if (status == WEBKIT_LOAD_FINISHED)
    return "COMPLETED";
  else
    return "LOADING";
}

static int gtkWebBrowserSetReloadAttrib(Ihandle* ih, const char* value)
{
  webkit_web_view_reload((WebKitWebView*)ih->handle);
  (void)value;
  return 0; /* do not store value in hash table */
}

static int gtkWebBrowserSetStopAttrib(Ihandle* ih, const char* value)
{
  webkit_web_view_stop_loading((WebKitWebView*)ih->handle);
  (void)value;
  return 0; /* do not store value in hash table */
}

static int gtkWebBrowserSetBackForwardAttrib(Ihandle* ih, const char* value)
{
  int val;
  if (iupStrToInt(value, &val))
  {
    /* Negative values represent steps backward while positive values represent steps forward. */
    webkit_web_view_go_back_or_forward((WebKitWebView*)ih->handle, val);
  }
  return 0; /* do not store value in hash table */
}

static int gtkWebBrowserSetValueAttrib(Ihandle* ih, const char* value)
{
  if (value)
    webkit_web_view_load_uri((WebKitWebView*)ih->handle, value);
  return 0; /* do not store value in hash table */
}

static char* gtkWebBrowserGetValueAttrib(Ihandle* ih)
{
  const gchar* value = webkit_web_view_get_uri((WebKitWebView*)ih->handle);
  return iupStrGetMemoryCopy(value);
}

/*********************************************************************************************/

static void gtkWebBrowserDocumentLoadFinished(WebKitWebView *web_view, WebKitWebFrame *frame, Ihandle *ih)
{
  IFns cb = (IFns)IupGetCallback(ih, "COMPLETED_CB");
  if (cb)
    cb(ih, (char*)webkit_web_frame_get_uri(frame));
}

static gboolean gtkWebBrowserLoadError(WebKitWebView *web_view, WebKitWebFrame *frame,
                                       gchar *uri, gpointer web_error, Ihandle *ih)
{
  IFns cb = (IFns)IupGetCallback(ih, "ERROR_CB");
  if (cb)
    cb(ih, uri);

  return FALSE;
}

static int gtkWebBrowserNavigate(WebKitWebView *web_view, WebKitWebFrame *frame, WebKitNetworkRequest *request,
                                 WebKitWebNavigationAction *navigation_action, WebKitWebPolicyDecision *policy_decision, Ihandle *ih)
{
  /*
  char *strReason = iupStrGetMemory(50);
  WebKitWebNavigationReason reason = webkit_web_navigation_action_get_reason(navigation_action);

  switch(reason)
  {
    case WEBKIT_WEB_NAVIGATION_REASON_LINK_CLICKED:
      sprintf(strReason, "%s", "LINK_CLICKED");
      break;
    case WEBKIT_WEB_NAVIGATION_REASON_FORM_SUBMITTED:
      sprintf(strReason, "%s", "FORM_SUBMITTED");
      break;
    case WEBKIT_WEB_NAVIGATION_REASON_BACK_FORWARD:
      sprintf(strReason, "%s", "BACK_FORWARD");
      break;
    case WEBKIT_WEB_NAVIGATION_REASON_RELOAD:
      sprintf(strReason, "%s", "RELOAD");
      break;
    case WEBKIT_WEB_NAVIGATION_REASON_FORM_RESUBMITTED:
      sprintf(strReason, "%s", "FORM_RESUBMITTED");
      break;
    case WEBKIT_WEB_NAVIGATION_REASON_OTHER:
      sprintf(strReason, "%s", "OTHER");
      break;
  }
  */

  IFns cb = (IFns)IupGetCallback(ih, "NAVIGATE_CB");
  if (cb)
  {
    if (cb(ih, (char*)webkit_network_request_get_uri(request)) == IUP_IGNORE)
      return FALSE;
  }

  return FALSE;
}

static WebKitWebView* gtkWebBrowserNewWindow(WebKitWebView  *web_view, WebKitWebFrame *frame, Ihandle *ih)
{
  IFns cb = (IFns)IupGetCallback(ih, "NEWWINDOW_CB");
  if (cb)
    cb(ih, (char*)webkit_web_frame_get_uri(frame));

  return web_view;
}

/*********************************************************************************************/

static void gtkWebBrowserDummyLogFunc(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
  /* does nothing */
  (void)log_domain;
  (void)log_level;
  (void)message;
  (void)user_data;
}

static int gtkWebBrowserMapMethod(Ihandle* ih)
{
  GtkScrolledWindow* scrolled_window;

  ih->handle = (GtkWidget*)webkit_web_view_new();
  if (!ih->handle)
    return IUP_ERROR;

  scrolled_window = (GtkScrolledWindow*)gtk_scrolled_window_new(NULL, NULL);
  if (!scrolled_window)
    return IUP_ERROR;

  {
    /* to avoid the "cannot add non scrollable widget" warning */
#if GTK_CHECK_VERSION(2, 6, 0)
    GLogFunc def_func = g_log_set_default_handler(gtkWebBrowserDummyLogFunc, NULL);
#endif
    gtk_container_add((GtkContainer*)scrolled_window, ih->handle);
#if GTK_CHECK_VERSION(2, 6, 0)
    g_log_set_default_handler(def_func, NULL);
#endif
  }

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

  gtk_widget_show((GtkWidget*)scrolled_window);

  iupAttribSetStr(ih, "_IUP_EXTRAPARENT", (char*)scrolled_window);

  /* add to the parent, all GTK controls must call this. */
  iupgtkBaseAddToParent(ih);

  g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event", G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-in-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-out-event",    G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "show-help",          G_CALLBACK(iupgtkShowHelp),        ih);

  g_signal_connect(G_OBJECT(ih->handle), "create-web-view", G_CALLBACK(gtkWebBrowserNewWindow), ih);
  g_signal_connect(G_OBJECT(ih->handle), "navigation-policy-decision-requested", G_CALLBACK(gtkWebBrowserNavigate), ih);
  g_signal_connect(G_OBJECT(ih->handle), "load-error", G_CALLBACK(gtkWebBrowserLoadError), ih);
  g_signal_connect(G_OBJECT(ih->handle), "document-load-finished", G_CALLBACK(gtkWebBrowserDocumentLoadFinished), ih);

  gtk_widget_realize((GtkWidget*)scrolled_window);
  gtk_widget_realize(ih->handle);

  return IUP_NOERROR;
}

static void gtkWebBrowserInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkWebBrowserMapMethod;
  
  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT); 

  /* IupWebBrowser GTK only */
  iupClassRegisterAttribute(ic, "VALUE", gtkWebBrowserGetValueAttrib, gtkWebBrowserSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKFORWARD", NULL, gtkWebBrowserSetBackForwardAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STOP", NULL, gtkWebBrowserSetStopAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RELOAD", NULL, gtkWebBrowserSetReloadAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HTML", NULL, gtkWebBrowserSetHTMLAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STATUS", gtkWebBrowserGetStatusAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKCOUNT", gtkWebBrowserGetBackCountAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORWARDCOUNT", gtkWebBrowserGetForwardCountAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "ITEMHISTORY",  gtkWebBrowserGetItemHistoryAttrib,  NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
}

static void gtkWebBrowserComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int natural_w = 0, natural_h = 0;
  int sb_size = iupdrvGetScrollbarSize();
  (void)expand; /* unset if not a container */

  iupdrvFontGetCharSize(ih, &natural_w, &natural_h);

  if (ih->data->sb & IUP_SB_HORIZ)
    natural_h += sb_size;  /* sb horizontal affects vertical size */
  if (ih->data->sb & IUP_SB_VERT)
    natural_w += sb_size;  /* sb vertical affects horizontal size */

  *w = natural_w;
  *h = natural_h;
}

static int gtkWebBrowserCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  ih->data = iupALLOCCTRLDATA();

  /* default EXPAND is YES */
  ih->expand = IUP_EXPAND_BOTH;
  ih->data->sb = IUP_SB_HORIZ | IUP_SB_VERT;  /* default is YES */

  return IUP_NOERROR; 
}

Iclass* iupWebBrowserNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "webbrowser";
  ic->format = NULL; /* no parameters */
  ic->nativetype  = IUP_TYPECONTROL;
  ic->childtype   = IUP_CHILDNONE;
  ic->is_interactive = 0;
  ic->has_attrib_id = 1;   /* has attributes with IDs that must be parsed */

  /* Class functions */
  ic->New = iupWebBrowserNewClass;
  ic->Create = gtkWebBrowserCreateMethod;
  ic->ComputeNaturalSize = gtkWebBrowserComputeNaturalSizeMethod;

  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "NEWWINDOW_CB", "s");
  iupClassRegisterCallback(ic, "NAVIGATE_CB", "s");
  iupClassRegisterCallback(ic, "ERROR_CB", "s");

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* IupWebBrowser only */
  gtkWebBrowserInitClass(ic);

  return ic;
}
