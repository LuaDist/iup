/** \file
* \brief Tabs Control
*
* See Copyright Notice in "iup.h"
*/

#include <Xm/Xm.h>
#include <Xm/Notebook.h>
#include <Xm/BulletinB.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>

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
#include "iup_childtree.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_tabs.h"
#include "iup_image.h"

#include "iupmot_drv.h"
#include "iupmot_color.h"


int iupdrvTabsExtraDecor(Ihandle* ih)
{
  (void)ih;
  return 1;
}

int iupdrvTabsGetLineCountAttrib(Ihandle* ih)
{
  (void)ih;
  return 1;
}

void iupdrvTabsSetCurrentTab(Ihandle* ih, int pos)
{
  Ihandle* child = IupGetChild(ih, pos);
  Ihandle* prev_child = IupGetChild(ih, iupdrvTabsGetCurrentTab(ih));
  Widget child_manager = (Widget)iupAttribGet(child, "_IUPTAB_CONTAINER");
  Widget prev_child_manager = (Widget)iupAttribGet(prev_child, "_IUPTAB_CONTAINER");
  XtMapWidget(child_manager);
  if (prev_child_manager) XtUnmapWidget(prev_child_manager);

  XtVaSetValues(ih->handle, XmNcurrentPageNumber, pos, NULL);
}

int iupdrvTabsGetCurrentTab(Ihandle* ih)
{
  int pos;
  XtVaGetValues(ih->handle, XmNcurrentPageNumber, &pos, NULL);
  return pos;
}

static void motTabsUpdatePageNumber(Ihandle* ih)
{
  int pos, old_pos;
  Ihandle* child;
  for (pos = 0, child = ih->firstchild; child; child = child->brother, pos++)
  {
    Widget child_manager = (Widget)iupAttribGet(child, "_IUPTAB_CONTAINER");
    if (child_manager)
    {
      old_pos = iupAttribGetInt(child, "_IUPMOT_TABNUMBER");  /* did not work when using XtVaGetValues(child_manager, XmNpageNumber) */
      if (pos != old_pos)
      {
        Widget tab_button = (Widget)iupAttribGet(child, "_IUPMOT_TABBUTTON");
        XtVaSetValues(child_manager, XmNpageNumber, pos, NULL);
        XtVaSetValues(tab_button, XmNpageNumber, pos, NULL);
        iupAttribSetInt(child, "_IUPMOT_TABNUMBER", pos);
      }
    }
  }
}

static void motTabsUpdatePageBgPixmap(Ihandle* ih, Pixmap pixmap)
{
  Ihandle* child;
  for (child = ih->firstchild; child; child = child->brother)
  {
    Widget child_manager = (Widget)iupAttribGet(child, "_IUPTAB_CONTAINER");
    if (child_manager)
      XtVaSetValues(child_manager, XmNbackgroundPixmap, pixmap, NULL);
  }
}

static void motTabsUpdatePageBgColor(Ihandle* ih, Pixel color)
{
  Ihandle* child;
  for (child = ih->firstchild; child; child = child->brother)
  {
    Widget child_manager = (Widget)iupAttribGet(child, "_IUPTAB_CONTAINER");
    if (child_manager)
      iupmotSetBgColor(child_manager, color);
  }
}

static void motTabsUpdateButtonsFgColor(Ihandle* ih, Pixel color)
{
  Ihandle* child;
  for (child = ih->firstchild; child; child = child->brother)
  {
    Widget tab_button = (Widget)iupAttribGet(child, "_IUPMOT_TABBUTTON");
    if (tab_button)
      XtVaSetValues(tab_button, XmNforeground, color, NULL);
  }
}

static void motTabsUpdateButtonsBgColor(Ihandle* ih, Pixel color)
{
  Ihandle* child;
  for (child = ih->firstchild; child; child = child->brother)
  {
    Widget tab_button = (Widget)iupAttribGet(child, "_IUPMOT_TABBUTTON");
    if (tab_button)
      iupmotSetBgColor(tab_button, color);
  }
}

static void motTabsUpdateButtonsPadding(Ihandle* ih)
{
  Ihandle* child;
  for (child = ih->firstchild; child; child = child->brother)
  {
    Widget tab_button = (Widget)iupAttribGet(child, "_IUPMOT_TABBUTTON");
    if (tab_button)
      XtVaSetValues(tab_button, XmNmarginHeight, ih->data->vert_padding,
                                XmNmarginWidth, ih->data->horiz_padding, NULL);
  }
}

static void motTabsUpdatePageFont(Ihandle* ih)
{
  Ihandle* child;
  XmFontList fontlist = (XmFontList)iupmotGetFontListAttrib(ih);

  for (child = ih->firstchild; child; child = child->brother)
  {
    Widget tab_button = (Widget)iupAttribGet(child, "_IUPMOT_TABBUTTON");
    if (tab_button)
      XtVaSetValues(tab_button, XmNrenderTable, fontlist, NULL);
  }
}

/* ------------------------------------------------------------------------- */
/* motTabs - Sets and Gets accessors                                         */
/* ------------------------------------------------------------------------- */

static int motTabsSetPaddingAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->horiz_padding, &ih->data->vert_padding, 'x');

  if (ih->handle)
    motTabsUpdateButtonsPadding(ih);
  return 0;
}

static void motTabsUpdateTabType(Ihandle* ih)
{
  if (ih->data->type == ITABS_LEFT)
    XtVaSetValues(ih->handle, XmNbackPagePlacement, XmBOTTOM_LEFT, 
                              XmNorientation, XmHORIZONTAL,
                              NULL);
  else if(ih->data->type == ITABS_RIGHT)
    XtVaSetValues(ih->handle, XmNbackPagePlacement, XmBOTTOM_RIGHT, 
                              XmNorientation, XmHORIZONTAL,
                              NULL);
  else if(ih->data->type == ITABS_BOTTOM)
    XtVaSetValues(ih->handle, XmNbackPagePlacement, XmBOTTOM_RIGHT, 
                              XmNorientation, XmVERTICAL,
                              NULL);
  else /* "TOP" */
    XtVaSetValues(ih->handle, XmNbackPagePlacement, XmTOP_RIGHT, 
                              XmNorientation, XmVERTICAL,
                              NULL);
}

static int motTabsSetTabTypeAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "LEFT"))
    ih->data->type = ITABS_LEFT;
  else if(iupStrEqualNoCase(value, "RIGHT"))
    ih->data->type = ITABS_RIGHT;
  else if(iupStrEqualNoCase(value, "BOTTOM"))
    ih->data->type = ITABS_BOTTOM;
  else /* "TOP" */
    ih->data->type = ITABS_TOP;

  if (ih->handle)
    motTabsUpdateTabType(ih);  /* for this to work must be updated in map */

  return 0;
}

static int motTabsSetBgColorAttrib(Ihandle* ih, const char* value)
{
  Pixel color;

  /* given value is used only for child, to the Tabs must use only from parent */
  char* parent_value = iupBaseNativeParentGetBgColor(ih);

  color = iupmotColorGetPixelStr(parent_value);
  if (color != (Pixel)-1)
  {
    iupmotSetBgColor(ih->handle, color);
    motTabsUpdatePageBgColor(ih, color);
  }

  color = iupmotColorGetPixelStr(value);
  if (color != (Pixel)-1)
    motTabsUpdateButtonsBgColor(ih, color);

  return 1; 
}

static int motTabsSetBackgroundAttrib(Ihandle* ih, const char* value)
{
  /* given value is used only for child, to the Tabs must use only from parent */
  char* parent_value = iupAttribGetInheritNativeParent(ih, "BACKGROUND");

  Pixel color = iupmotColorGetPixelStr(parent_value);
  if (color != (Pixel)-1)
  {
    iupmotSetBgColor(ih->handle, color);
    motTabsUpdatePageBgColor(ih, color);
  }
  else
  {
    Pixmap pixmap = (Pixmap)iupImageGetImage(parent_value, ih, 0);
    if (pixmap)
    {
      XtVaSetValues(ih->handle, XmNbackgroundPixmap, pixmap, NULL);
      motTabsUpdatePageBgPixmap(ih, pixmap);
    }
  }

  (void)value;
  return 1;
}

static int motTabsSetFgColorAttrib(Ihandle* ih, const char* value)
{
  Pixel color = iupmotColorGetPixelStr(value);
  if (color != (Pixel)-1)
  {
    XtVaSetValues(ih->handle, XmNforeground, color, NULL);
    motTabsUpdateButtonsFgColor(ih, color);
    return 1;
  }
  return 0; 
}

static int motTabsSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetStandardFontAttrib(ih, value);
  if (ih->handle)
    motTabsUpdatePageFont(ih);
  return 1;
}

static int motTabsSetTabTitleAttrib(Ihandle* ih, int pos, const char* value)
{
  if (value)
  {
    Ihandle* child = IupGetChild(ih, pos);
    Widget tab_button = (Widget)iupAttribGet(child, "_IUPMOT_TABBUTTON");
    if (tab_button)
      iupmotSetMnemonicTitle(ih, tab_button, value);
  }
  return 1;
}

static int motTabsSetTabImageAttrib(Ihandle* ih, int pos, const char* value)
{
  if (value)
  {
    Ihandle* child = IupGetChild(ih, pos);
    Widget tab_button = (Widget)iupAttribGet(child, "_IUPMOT_TABBUTTON");
    if (tab_button)
    {
      Pixmap pixmap = (Pixmap)iupImageGetImage(value, ih, 0);
      if (pixmap)
        XtVaSetValues(tab_button, XmNlabelPixmap, pixmap, NULL);
    }
  }
  return 1;
}


/* ------------------------------------------------------------------------- */
/* motTabs - Callback                                                        */
/* ------------------------------------------------------------------------- */

void motTabsPageChangedCallback(Widget w, Ihandle* ih, XmNotebookCallbackStruct *nptr)
{
  if (nptr->reason == XmCR_MAJOR_TAB)
  {
    IFnnn cb;
    Ihandle* child = IupGetChild(ih, nptr->page_number);
    Ihandle* prev_child = IupGetChild(ih, nptr->prev_page_number);
    Widget child_manager = (Widget)iupAttribGet(child, "_IUPTAB_CONTAINER");
    Widget prev_child_manager = (Widget)iupAttribGet(prev_child, "_IUPTAB_CONTAINER");

    XtMapWidget(child_manager);
    if (prev_child_manager) XtUnmapWidget(prev_child_manager);

    cb = (IFnnn)IupGetCallback(ih, "TABCHANGE_CB");
    if (cb)
      cb(ih, child, prev_child);
    else
    {
      IFnii cb2 = (IFnii)IupGetCallback(ih, "TABCHANGEPOS_CB");
      if (cb2)
        cb2(ih, nptr->page_number, nptr->prev_page_number);
    }
  }
  (void)w; 
}

static void motTabsConfigureNotify(Widget w, XEvent *evt, String* s, Cardinal *card)
{
  /* Motif does not process the changed of position and/or size of children outside the parent's client area. 
     Since Notebook pages are not resized until they are moved into the visible area,
     we must update the children position and size when a tab page is resize. 
     Since tab pages are not hidden, they are moved outside the visible area,
     a resize occours every time a tab is activated.
  */
  Ihandle *child;
  (void)s;
  (void)card;
  (void)evt;

  XtVaGetValues(w, XmNuserData, &child, NULL);
  if (!child) return;

  iupLayoutUpdate(child);
}

/* ------------------------------------------------------------------------- */
/* motTabs - Methods and Init Class                                          */
/* ------------------------------------------------------------------------- */

static void motTabsChildAddedMethod(Ihandle* ih, Ihandle* child)
{
  if (IupGetName(child) == NULL)
    iupAttribSetHandleName(child);

  if (ih->handle)
  {
    Widget child_manager;
    Widget tab_button;
    int num_args = 0, pos;
    Arg args[30];
    char *tabtitle, *tabimage, *background;
    Pixel color;

    /* open space for new tab number */
    motTabsUpdatePageNumber(ih);

    pos = IupGetChildPos(ih, child);

    /* Create pages */
    child_manager = XtVaCreateManagedWidget(
                    "child_manager",
                    xmBulletinBoardWidgetClass,
                    ih->handle,
                    /* Core */
                    XmNborderWidth, 0,
                    /* Manager */
                    XmNshadowThickness, 0,
                    XmNnavigationType, XmTAB_GROUP,
                    XmNuserData, child, /* used only in motTabsConfigureNotify  */
                    /* BulletinBoard */
                    XmNmarginWidth, 0,
                    XmNmarginHeight, 0,
                    XmNresizePolicy, XmRESIZE_NONE, /* no automatic resize of children */
                    /* Notebook Constraint */
                    XmNnotebookChildType, XmPAGE,
                    XmNpageNumber, pos,
                    XmNresizable, True,
                    NULL);   

    XtOverrideTranslations(child_manager, XtParseTranslationTable("<Configure>: iupTabsConfigure()"));

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

    /* Create tabs */
    /* Label */
    iupMOT_SETARG(args, num_args, XmNlabelType, tabtitle? XmSTRING: XmPIXMAP);
    iupMOT_SETARG(args, num_args, XmNmarginHeight, 0);
    iupMOT_SETARG(args, num_args, XmNmarginWidth, 0);
    /* Notebook Constraint */
    iupMOT_SETARG(args, num_args, XmNnotebookChildType, XmMAJOR_TAB);
    iupMOT_SETARG(args, num_args, XmNpageNumber, pos);
    tab_button = XtCreateManagedWidget("tab_button", xmPushButtonWidgetClass, ih->handle, args, num_args);

    /* Disable Drag Source */
    iupmotDisableDragSource(tab_button);

    XtAddEventHandler(tab_button, EnterWindowMask, False, (XtEventHandler)iupmotEnterLeaveWindowEvent, (XtPointer)ih);
    XtAddEventHandler(tab_button, LeaveWindowMask, False, (XtEventHandler)iupmotEnterLeaveWindowEvent, (XtPointer)ih);
    XtAddEventHandler(tab_button, FocusChangeMask, False, (XtEventHandler)iupmotFocusChangeEvent, (XtPointer)ih);
    XtAddEventHandler(tab_button, KeyPressMask,    False, (XtEventHandler)iupmotKeyPressEvent, (XtPointer)ih);

    if (iupStrBoolean(IupGetGlobal("INPUTCALLBACKS")))
    {
      XtAddEventHandler(tab_button, PointerMotionMask, False, (XtEventHandler)iupmotDummyPointerMotionEvent, NULL);
      XtAddEventHandler(child_manager, PointerMotionMask, False, (XtEventHandler)iupmotDummyPointerMotionEvent, NULL);
    }

    if (tabtitle)
      iupmotSetMnemonicTitle(ih, tab_button, tabtitle);
    else
    {
      Pixmap pixmap = (Pixmap)iupImageGetImage(tabimage, ih, 0);
      if (pixmap)
        XtVaSetValues(tab_button, XmNlabelPixmap, pixmap, NULL);
    }

    background = iupBaseNativeParentGetBgColorAttrib(ih);
    color = iupmotColorGetPixelStr(background);
    if (color != -1)
      iupmotSetBgColor(child_manager, color);
    else
    {
      Pixmap pixmap = (Pixmap)iupImageGetImage(background, ih, 0);
      if (pixmap)
      {
        XtVaSetValues(child_manager, XmNbackgroundPixmap, pixmap, NULL);
      }
    }

    background = iupAttribGetStr(ih, "BGCOLOR");
    color = iupmotColorGetPixelStr(background);
    if (color != -1)
      iupmotSetBgColor(tab_button, color);

    color = iupmotColorGetPixelStr(IupGetAttribute(ih, "FGCOLOR"));
    XtVaSetValues(tab_button, XmNforeground, color, NULL);

    XtRealizeWidget(child_manager);
    XtRealizeWidget(tab_button);

    iupAttribSetStr(child, "_IUPTAB_CONTAINER", (char*)child_manager);
    iupAttribSetStr(child, "_IUPMOT_TABBUTTON", (char*)tab_button);
    iupAttribSetInt(child, "_IUPMOT_TABNUMBER", pos);

    if (pos != iupdrvTabsGetCurrentTab(ih))
      XtUnmapWidget(child_manager);
  }
}

static void motTabsChildRemovedMethod(Ihandle* ih, Ihandle* child)
{
  if (ih->handle)
  {
    Widget child_manager = (Widget)iupAttribGet(child, "_IUPTAB_CONTAINER");
    if (child_manager)
    {
      int pos;
      Widget tab_button = (Widget)iupAttribGet(child, "_IUPMOT_TABBUTTON");

      pos = iupAttribGetInt(child, "_IUPMOT_TABNUMBER");  /* did not work when using XtVaGetValues(child_manager, XmNpageNumber) */
      iupTabsTestRemoveTab(ih, pos);

      XtDestroyWidget(tab_button);
      XtDestroyWidget(child_manager);

      /* compact the tab number usage */
      motTabsUpdatePageNumber(ih);

      iupAttribSetStr(child, "_IUPTAB_CONTAINER", NULL);
      iupAttribSetStr(child, "_IUPMOT_TABBUTTON", NULL);
      iupAttribSetStr(child, "_IUPMOT_TABNUMBER", NULL);
    }
  }
}

static int motTabsMapMethod(Ihandle* ih)
{
  int num_args = 0;
  Arg args[30];

  if (!ih->parent)
    return IUP_ERROR;

  /* Core */
  iupMOT_SETARG(args, num_args, XmNmappedWhenManaged, False);  /* not visible when managed */
  iupMOT_SETARG(args, num_args, XmNx, 0);  /* x-position */
  iupMOT_SETARG(args, num_args, XmNy, 0);  /* y-position */
  iupMOT_SETARG(args, num_args, XmNwidth, 10);  /* default width to avoid 0 */
  iupMOT_SETARG(args, num_args, XmNheight, 10); /* default height to avoid 0 */
  /* Manager */
  iupMOT_SETARG(args, num_args, XmNshadowThickness, 0);
  iupMOT_SETARG(args, num_args, XmNtraversalOn, True);
  iupMOT_SETARG(args, num_args, XmNhighlightThickness, 0);
  /* Notebook */
  iupMOT_SETARG(args, num_args, XmNbindingType, XmNONE);
  iupMOT_SETARG(args, num_args, XmNbindingWidth, 0);
  iupMOT_SETARG(args, num_args, XmNfirstPageNumber, 0);  /* IupTabs index always starts with zero */
  iupMOT_SETARG(args, num_args, XmNbackPageSize, 0);
  iupMOT_SETARG(args, num_args, XmNbackPageNumber, 1);
  iupMOT_SETARG(args, num_args, XmNframeShadowThickness, 2);

  ih->handle = XtCreateManagedWidget(
    iupDialogGetChildIdStr(ih),  /* child identifier */
    xmNotebookWidgetClass,       /* widget class */
    iupChildTreeGetNativeParentHandle(ih), /* widget parent */
    args, num_args);

  if (!ih->handle)
    return IUP_ERROR;
 
  ih->serial = iupDialogGetChildId(ih); /* must be after using the string */

  /* Disable page scroller */
  {
    Widget scroller;
    scroller = XtNameToWidget(ih->handle, "*PageScroller");
    XtUnmanageChild(scroller);
  }

  /* Callbacks */
  XtAddCallback(ih->handle, XmNpageChangedCallback, (XtCallbackProc)motTabsPageChangedCallback, (XtPointer)ih);
  XtAddCallback(ih->handle, XmNhelpCallback, (XtCallbackProc)iupmotHelpCallback, (XtPointer)ih);

  /* update Tab position */
  motTabsUpdateTabType(ih);

  /* current value is now given by the native system */
  iupAttribSetStr(ih, "_IUPTABS_VALUE_HANDLE", NULL);

  /* initialize the widget */
  XtRealizeWidget(ih->handle);

  /* Create pages and tabs */
  if (ih->firstchild)
  {
    Ihandle* child;
    for (child = ih->firstchild; child; child = child->brother)
      motTabsChildAddedMethod(ih, child);
  }

  return IUP_NOERROR;
}

void iupdrvTabsInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = motTabsMapMethod;
  ic->ChildAdded     = motTabsChildAddedMethod;
  ic->ChildRemoved   = motTabsChildRemovedMethod;

  {
    /* Set up a translation table that captures "Resize" events
       (also called ConfigureNotify or Configure events). */
    XtActionsRec rec = {"iupTabsConfigure", motTabsConfigureNotify};
    XtAppAddActions(iupmot_appcontext, &rec, 1);
  }

  /* Driver Dependent Attribute functions */

  /* Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, motTabsSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, motTabsSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, motTabsSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BACKGROUND", NULL, motTabsSetBackgroundAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  
  /* IupTabs only */
  iupClassRegisterAttribute(ic, "TABTYPE", iupTabsGetTabTypeAttrib, motTabsSetTabTypeAttrib, IUPAF_SAMEASSYSTEM, "TOP", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);  
  iupClassRegisterAttribute(ic, "TABORIENTATION", iupTabsGetTabOrientationAttrib, NULL, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);  /* can not be set, always HORIZONTAL in Motif */
  iupClassRegisterAttributeId(ic, "TABTITLE", NULL, motTabsSetTabTitleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TABIMAGE", NULL, motTabsSetTabImageAttrib, IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PADDING", iupTabsGetPaddingAttrib, motTabsSetPaddingAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* NOT supported */
  iupClassRegisterAttribute(ic, "MULTILINE", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_DEFAULT);
}
