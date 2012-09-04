/** \file
* \brief Tabs Control
*
* See Copyright Notice in "iup.h"
*/

#include <gtk/gtk.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_image.h"
#include "iup_tabs.h"

#include "iupgtk_drv.h"


int iupdrvTabsExtraDecor(Ihandle* ih)
{
  (void)ih;
  return 0;
}

int iupdrvTabsGetLineCountAttrib(Ihandle* ih)
{
  (void)ih;
  return 1;
}

void iupdrvTabsSetCurrentTab(Ihandle* ih, int pos)
{
  iupAttribSetStr(ih, "_IUPGTK_IGNORE_CHANGE", "1");
  gtk_notebook_set_current_page((GtkNotebook*)ih->handle, pos);
  iupAttribSetStr(ih, "_IUPGTK_IGNORE_CHANGE", NULL);
}

int iupdrvTabsGetCurrentTab(Ihandle* ih)
{
  return gtk_notebook_get_current_page((GtkNotebook*)ih->handle);
}

static void gtkTabsUpdatePageFont(Ihandle* ih)
{
  Ihandle* child;
  PangoFontDescription* fontdesc = (PangoFontDescription*)iupgtkGetPangoFontDescAttrib(ih);

  for (child = ih->firstchild; child; child = child->brother)
  {
    GtkWidget* tab_label = (GtkWidget*)iupAttribGet(child, "_IUPGTK_TABLABEL");
    if (tab_label)
    {
      gtk_widget_modify_font(tab_label, fontdesc);
      iupgtkFontUpdatePangoLayout(ih, gtk_label_get_layout((GtkLabel*)tab_label));
    }
  }
}

static void gtkTabsUpdatePageBgColor(Ihandle* ih, unsigned char r, unsigned char g, unsigned char b)
{
  Ihandle* child;

  for (child = ih->firstchild; child; child = child->brother)
  {
    GtkWidget* tab_container = (GtkWidget*)iupAttribGet(child, "_IUPTAB_CONTAINER");
    if (tab_container)
    {
      GtkWidget* tab_label = (GtkWidget*)iupAttribGet(child, "_IUPGTK_TABLABEL");
      if (tab_label)
        iupgtkBaseSetBgColor(tab_label, r, g, b);
      iupgtkBaseSetBgColor(tab_container, r, g, b);
    }
  }
}

static void gtkTabsUpdatePageFgColor(Ihandle* ih, unsigned char r, unsigned char g, unsigned char b)
{
  Ihandle* child;

  for (child = ih->firstchild; child; child = child->brother)
  {
    GtkWidget* tab_label = (GtkWidget*)iupAttribGet(child, "_IUPGTK_TABLABEL");
    if (tab_label)
      iupgtkBaseSetFgColor(tab_label, r, g, b);
  }
}

static void gtkTabsUpdatePagePadding(Ihandle* ih)
{
  Ihandle* child;

  for (child = ih->firstchild; child; child = child->brother)
  {
    GtkWidget* tab_label = (GtkWidget*)iupAttribGet(child, "_IUPGTK_TABLABEL");
    if (tab_label)
      gtk_misc_set_padding((GtkMisc*)tab_label, ih->data->horiz_padding, ih->data->vert_padding);
  }
}

/* ------------------------------------------------------------------------- */
/* gtkTabs - Sets and Gets accessors                                         */
/* ------------------------------------------------------------------------- */

  
static int gtkTabsSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');

  if (ih->handle)
  {
    gtkTabsUpdatePagePadding(ih);
    return 0;
  }
  else
    return 1; /* store until not mapped, when mapped will be set again */
}

static void gtkTabsUpdateTabType(Ihandle* ih)
{
  int iup2gtk[4] = {GTK_POS_TOP, GTK_POS_BOTTOM, GTK_POS_LEFT, GTK_POS_RIGHT};
  gtk_notebook_set_tab_pos((GtkNotebook*)ih->handle, iup2gtk[ih->data->type]);
}

static int gtkTabsSetTabTypeAttrib(Ihandle* ih, const char* value)
{
  if(iupStrEqualNoCase(value, "BOTTOM"))
    ih->data->type = ITABS_BOTTOM;
  else if(iupStrEqualNoCase(value, "LEFT"))
    ih->data->type = ITABS_LEFT;
  else if(iupStrEqualNoCase(value, "RIGHT"))
    ih->data->type = ITABS_RIGHT;
  else /* "TOP" */
    ih->data->type = ITABS_TOP;

  if (ih->handle)
    gtkTabsUpdateTabType(ih);  /* for this to work must be updated in map */

  return 0;
}

static int gtkTabsSetTabOrientationAttrib(Ihandle* ih, const char* value)
{
  if (ih->handle) /* allow to set only before mapping */
    return 0;

  if(iupStrEqualNoCase(value, "VERTICAL"))
    ih->data->orientation = ITABS_VERTICAL;
  else  /* HORIZONTAL */
    ih->data->orientation = ITABS_HORIZONTAL;

  return 0;
}

static int gtkTabsSetTabTitleAttrib(Ihandle* ih, int pos, const char* value)
{
  if (value)
  {
    Ihandle* child = IupGetChild(ih, pos);
    GtkWidget* tab_label = (GtkWidget*)iupAttribGet(child, "_IUPGTK_TABLABEL");
    if (tab_label)
    {
      GtkWidget* tab_page = (GtkWidget*)iupAttribGet(child, "_IUPTAB_PAGE");
      iupgtkSetMnemonicTitle(ih, (GtkLabel*)tab_label, value);
      gtk_notebook_set_menu_label_text((GtkNotebook*)ih->handle, tab_page, gtk_label_get_text((GtkLabel*)tab_label));
    }
  }
  return 1;
}

static int gtkTabsSetTabImageAttrib(Ihandle* ih, int pos, const char* value)
{
  if (value)
  {
    Ihandle* child = IupGetChild(ih, pos);
    GtkWidget* tab_image = (GtkWidget*)iupAttribGet(child, "_IUPGTK_TABIMAGE");
    if (tab_image)
    {
      GdkPixbuf* pixbuf = iupImageGetImage(value, ih, 0);
      if (pixbuf)
        gtk_image_set_from_pixbuf((GtkImage*)tab_image, pixbuf);
    }
  }
  return 1;
}

static int gtkTabsSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetStandardFontAttrib(ih, value);
  if (ih->handle)
    gtkTabsUpdatePageFont(ih);
  return 1;
}

static int gtkTabsSetFgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iupgtkBaseSetFgColor(ih->handle, r, g, b);
  gtkTabsUpdatePageFgColor(ih, r, g, b);

  return 1;
}

static int gtkTabsSetBgColorAttrib(Ihandle* ih, const char* value)
{
  unsigned char r, g, b;
  if (!iupStrToRGB(value, &r, &g, &b))
    return 0;

  iupgtkBaseSetBgColor(ih->handle, r, g, b);
  gtkTabsUpdatePageBgColor(ih, r, g, b);

  return 1;
}


/* ------------------------------------------------------------------------- */
/* gtkTabs - Callbacks                                                       */
/* ------------------------------------------------------------------------- */

static void gtkTabSwitchPage(GtkNotebook* notebook, void* page, int pos, Ihandle* ih)
{
  IFnnn cb;
  Ihandle* child = IupGetChild(ih, pos);
  int prev_pos = iupdrvTabsGetCurrentTab(ih);
  Ihandle* prev_child = IupGetChild(ih, prev_pos);
  GtkWidget* tab_container = (GtkWidget*)iupAttribGet(child, "_IUPTAB_CONTAINER");
  GtkWidget* prev_tab_container = (GtkWidget*)iupAttribGet(prev_child, "_IUPTAB_CONTAINER");
  if (tab_container) gtk_widget_show(tab_container);
  if (prev_tab_container) gtk_widget_hide(prev_tab_container);

  if (iupAttribGet(ih, "_IUPGTK_IGNORE_CHANGE"))
    return;

  cb = (IFnnn)IupGetCallback(ih, "TABCHANGE_CB");
  if (cb)
    cb(ih, child, prev_child);
  else
  {
    IFnii cb2 = (IFnii)IupGetCallback(ih, "TABCHANGEPOS_CB");
    if (cb2)
      cb2(ih, pos, prev_pos);
  }

  (void)notebook;
  (void)page;
}

/* ------------------------------------------------------------------------- */
/* gtkTabs - Methods and Init Class                                          */
/* ------------------------------------------------------------------------- */

static void gtkTabsChildAddedMethod(Ihandle* ih, Ihandle* child)
{
  if (IupGetName(child) == NULL)
    iupAttribSetHandleName(child);

  if (ih->handle)
  {
    GtkWidget *tab_page, *tab_container;
    GtkWidget *tab_label = NULL, *tab_image = NULL;
    char *tabtitle, *tabimage;
    int pos;
    unsigned char r, g, b;

    pos = IupGetChildPos(ih, child);

    tab_page = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(tab_page);

    tab_container = gtk_fixed_new();
    gtk_widget_show(tab_container);
    gtk_container_add((GtkContainer*)tab_page, tab_container);

    tabtitle = iupTabsAttribGetStrId(ih, "TABTITLE", pos);
    if (!tabtitle) 
    {
      tabtitle = iupAttribGet(child, "TABTITLE");
      if (tabtitle)
        iupTabsAttribSetStrId(ih, "TABTITLE", pos, tabtitle);
    }
    tabimage = iupTabsAttribGetStrId(ih, "TABIMAGE", pos);
    if (!tabimage) 
    {
      tabimage = iupAttribGet(child, "TABIMAGE");
      if (tabimage)
        iupTabsAttribSetStrId(ih, "TABIMAGE", pos, tabimage);
    }
    if (!tabtitle && !tabimage)
      tabtitle = "     ";

    if (tabtitle)
    {
      tab_label = gtk_label_new(NULL);
      iupgtkSetMnemonicTitle(ih, (GtkLabel*)tab_label, tabtitle);

#if GTK_CHECK_VERSION(2, 6, 0)
      if (ih->data->orientation == ITABS_VERTICAL)
        gtk_label_set_angle((GtkLabel*)tab_label, 90);
#endif
    }

    if (tabimage)
    {
      GdkPixbuf* pixbuf = iupImageGetImage(tabimage, ih, 0);

      tab_image = gtk_image_new();

      if (pixbuf)
        gtk_image_set_from_pixbuf((GtkImage*)tab_image, pixbuf);
    }

    iupAttribSetStr(ih, "_IUPGTK_IGNORE_CHANGE", "1");

    if (tabimage && tabtitle)
    {
      GtkWidget* box;
      if (ih->data->orientation == ITABS_VERTICAL)
        box = gtk_vbox_new(FALSE, 2);
      else
        box = gtk_hbox_new(FALSE, 2);
      gtk_widget_show(box);

      gtk_container_add((GtkContainer*)box, tab_image);
      gtk_container_add((GtkContainer*)box, tab_label);

      gtk_notebook_insert_page((GtkNotebook*)ih->handle, tab_page, box, pos);
      gtk_notebook_set_menu_label_text((GtkNotebook*)ih->handle, tab_page, gtk_label_get_text((GtkLabel*)tab_label));
    }
    else if (tabimage)
      gtk_notebook_insert_page((GtkNotebook*)ih->handle, tab_page, tab_image, pos);
    else
      gtk_notebook_insert_page((GtkNotebook*)ih->handle, tab_page, tab_label, pos);

    gtk_widget_realize(tab_page);

    iupAttribSetStr(child, "_IUPGTK_TABIMAGE", (char*)tab_image);  /* store it even if its NULL */
    iupAttribSetStr(child, "_IUPGTK_TABLABEL", (char*)tab_label);
    iupAttribSetStr(child, "_IUPTAB_CONTAINER", (char*)tab_container);
    iupAttribSetStr(child, "_IUPTAB_PAGE", (char*)tab_page);
    iupStrToRGB(IupGetAttribute(ih, "BGCOLOR"), &r, &g, &b);
    iupgtkBaseSetBgColor(tab_container, r, g, b);

    if (tabtitle)
    {
      PangoFontDescription* fontdesc = (PangoFontDescription*)iupgtkGetPangoFontDescAttrib(ih);
      gtk_widget_modify_font(tab_label, fontdesc);
      iupgtkFontUpdatePangoLayout(ih, gtk_label_get_layout((GtkLabel*)tab_label));

      iupgtkBaseSetBgColor(tab_label, r, g, b);

      iupStrToRGB(IupGetAttribute(ih, "FGCOLOR"), &r, &g, &b);
      iupgtkBaseSetFgColor(tab_label, r, g, b);

      gtk_widget_show(tab_label);
      gtk_widget_realize(tab_label);
    }

    if (tabimage)
    {
      gtk_widget_show(tab_image);
      gtk_widget_realize(tab_image);
    }

    iupAttribSetStr(ih, "_IUPGTK_IGNORE_CHANGE", NULL);

    if (pos != iupdrvTabsGetCurrentTab(ih))
      gtk_widget_hide(tab_container);
  }
}

static void gtkTabsChildRemovedMethod(Ihandle* ih, Ihandle* child)
{
  if (ih->handle)
  {
    GtkWidget* tab_page = (GtkWidget*)iupAttribGet(child, "_IUPTAB_PAGE");
    if (tab_page)
    {
      int pos = gtk_notebook_page_num((GtkNotebook*)ih->handle, tab_page);
      iupTabsTestRemoveTab(ih, pos);

      iupAttribSetStr(ih, "_IUPGTK_IGNORE_CHANGE", "1");
      gtk_notebook_remove_page((GtkNotebook*)ih->handle, pos);
      iupAttribSetStr(ih, "_IUPGTK_IGNORE_CHANGE", NULL);

      iupAttribSetStr(child, "_IUPGTK_TABIMAGE", NULL);
      iupAttribSetStr(child, "_IUPGTK_TABLABEL", NULL);
      iupAttribSetStr(child, "_IUPTAB_CONTAINER", NULL);
      iupAttribSetStr(child, "_IUPTAB_PAGE", NULL);
    }
  }
}

static int gtkTabsMapMethod(Ihandle* ih)
{
  ih->handle = gtk_notebook_new();
  if (!ih->handle)
    return IUP_ERROR;

  gtk_notebook_set_scrollable((GtkNotebook*)ih->handle, TRUE);
  gtk_notebook_popup_enable((GtkNotebook*)ih->handle);

  gtkTabsUpdateTabType(ih);

  /* add to the parent, all GTK controls must call this. */
  iupgtkBaseAddToParent(ih);

  gtk_widget_add_events(ih->handle, GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK);

  g_signal_connect(G_OBJECT(ih->handle), "enter-notify-event",  G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "leave-notify-event",  G_CALLBACK(iupgtkEnterLeaveEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-in-event",      G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "focus-out-event",     G_CALLBACK(iupgtkFocusInOutEvent), ih);
  g_signal_connect(G_OBJECT(ih->handle), "key-press-event",     G_CALLBACK(iupgtkKeyPressEvent),   ih);
  g_signal_connect(G_OBJECT(ih->handle), "show-help",           G_CALLBACK(iupgtkShowHelp),        ih);

  g_signal_connect(G_OBJECT(ih->handle), "switch-page",         G_CALLBACK(gtkTabSwitchPage), ih);

  gtk_widget_realize(ih->handle);

  /* Create pages and tabs */
  if (ih->firstchild)
  {
    Ihandle* child;
    for (child = ih->firstchild; child; child = child->brother)
      gtkTabsChildAddedMethod(ih, child);
  }

  return IUP_NOERROR;
}

void iupdrvTabsInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkTabsMapMethod;
  ic->ChildAdded     = gtkTabsChildAddedMethod;
  ic->ChildRemoved   = gtkTabsChildRemovedMethod;

  /* Driver Dependent Attribute functions */

  /* Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, gtkTabsSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkTabsSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkTabsSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);

  /* IupTabs only */
  iupClassRegisterAttribute(ic, "TABTYPE", iupTabsGetTabTypeAttrib, gtkTabsSetTabTypeAttrib, IUPAF_SAMEASSYSTEM, "TOP", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TABORIENTATION", iupTabsGetTabOrientationAttrib, gtkTabsSetTabOrientationAttrib, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABTITLE", NULL, gtkTabsSetTabTitleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABIMAGE", NULL, gtkTabsSetTabImageAttrib, IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iupTabsGetPaddingAttrib, gtkTabsSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* NOT supported */
  iupClassRegisterAttribute(ic, "MULTILINE", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_DEFAULT);
}
