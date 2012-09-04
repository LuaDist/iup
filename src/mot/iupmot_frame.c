/** \file
 * \brief Frame Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <Xm/Xm.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/BulletinB.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"
#include "iup_image.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"

#include "iupmot_drv.h"
#include "iupmot_color.h"


void iupdrvFrameGetDecorOffset(int *x, int *y)
{
  *x = 2;
  *y = 2;
}

int iupdrvFrameHasClientOffset(void)
{
  return 0;
}

static int motFrameSetBgColorAttrib(Ihandle* ih, const char* value)
{
  Pixel color;

  if (!iupAttribGet(ih, "_IUPFRAME_HAS_BGCOLOR"))
  {
    /* ignore given value, must use only from parent */
    value = iupBaseNativeParentGetBgColor(ih);
  }

  color = iupmotColorGetPixelStr(value);
  if (color != (Pixel)-1)
  {
    Widget title_label, child_manager;

    if (!iupAttribGet(ih, "_IUPFRAME_HAS_BGCOLOR"))
    {
      iupmotSetBgColor(ih->handle, color);

      child_manager = XtNameToWidget(ih->handle, "*child_manager");
      iupmotSetBgColor(child_manager, color);

      title_label = XtNameToWidget(ih->handle, "*title_label");
      if (!title_label) return 1;
      iupmotSetBgColor(title_label, color);
    }
    else
    {
      child_manager = XtNameToWidget(ih->handle, "*child_manager");
      iupmotSetBgColor(child_manager, color);
    }

    return 1;
  }
  return 0; 
}

static int motFrameSetBackgroundAttrib(Ihandle* ih, const char* value)
{
  Pixel color;

  /* ignore given value, must use only from parent */
  value = iupAttribGetInheritNativeParent(ih, "BACKGROUND");

  color = iupmotColorGetPixelStr(value);
  if (color != (Pixel)-1)
  {
    Widget title_label, child_manager;

    iupmotSetBgColor(ih->handle, color);

    child_manager = XtNameToWidget(ih->handle, "*child_manager");
    iupmotSetBgColor(child_manager, color);

    title_label = XtNameToWidget(ih->handle, "*title_label");
    if (!title_label) return 1;
    iupmotSetBgColor(title_label, color);

    return 1;
  }
  else
  {
    Pixmap pixmap = (Pixmap)iupImageGetImage(value, ih, 0);
    if (pixmap)
    {
      Widget child_manager = XtNameToWidget(ih->handle, "*child_manager");
      Widget title_label = XtNameToWidget(ih->handle, "*title_label");

      XtVaSetValues(child_manager, XmNbackgroundPixmap, pixmap, NULL);
      if (title_label)
        XtVaSetValues(title_label, XmNbackgroundPixmap, pixmap, NULL);

      return 1;
    }
  }
  return 0;
}

static int motFrameSetFgColorAttrib(Ihandle* ih, const char* value)
{
  Pixel color = iupmotColorGetPixelStr(value);
  if (color != (Pixel)-1)
  {
    Widget title_label = XtNameToWidget(ih->handle, "*title_label");
    if (!title_label) return 0;
    XtVaSetValues(title_label, XmNforeground, color, NULL);
    return 1;
  }
  return 0; 
}

static int motFrameSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  iupdrvSetStandardFontAttrib(ih, value);

  if (ih->handle)
  {
    XmFontList fontlist;
    Widget title_label = XtNameToWidget(ih->handle, "*title_label");
    if (!title_label) return 1;

    fontlist = (XmFontList)iupmotGetFontListAttrib(ih);
    XtVaSetValues(title_label, XmNrenderTable, fontlist, NULL);
  }

  return 1;
}

static int motFrameSetTitleAttrib(Ihandle* ih, const char* value)
{
  Widget title_label = XtNameToWidget(ih->handle, "*title_label");
  if (title_label)
  {
    if (!value) value = "";
    iupmotSetString(title_label, XmNlabelString, value);
    return 1;
  }
  return 0;
}

static void* motFrameGetInnerNativeContainerHandleMethod(Ihandle* ih, Ihandle* child)
{
  (void)child;
  return XtNameToWidget(ih->handle, "*child_manager");
}

static int motFrameMapMethod(Ihandle* ih)
{
  char *title;
  int num_args = 0;
  Arg args[20];
  Widget child_manager;

  if (!ih->parent)
    return IUP_ERROR;

  title = iupAttribGet(ih, "TITLE");

  if (title)
    iupAttribSetStr(ih, "_IUPFRAME_HAS_TITLE", "1");
  else
  {
    char* value = iupAttribGetStr(ih, "SUNKEN");
    if (iupStrBoolean(value))
      iupMOT_SETARG(args, num_args, XmNshadowType, XmSHADOW_IN); 
    else
      iupMOT_SETARG(args, num_args, XmNshadowType, XmSHADOW_ETCHED_IN); 

    if (iupAttribGet(ih, "BGCOLOR"))
      iupAttribSetStr(ih, "_IUPFRAME_HAS_BGCOLOR", "1");
  }

  /* Core */
  iupMOT_SETARG(args, num_args, XmNmappedWhenManaged, False);  /* not visible when managed */
  iupMOT_SETARG(args, num_args, XmNx, 0);  /* x-position */
  iupMOT_SETARG(args, num_args, XmNy, 0);  /* y-position */
  iupMOT_SETARG(args, num_args, XmNwidth, 10);  /* default width to avoid 0 */
  iupMOT_SETARG(args, num_args, XmNheight, 10); /* default height to avoid 0 */
  /* Manager */
  iupMOT_SETARG(args, num_args, XmNshadowThickness, 2);
  /* Frame */
  iupMOT_SETARG(args, num_args, XmNmarginHeight, 0);  /* no shadow margins */
  iupMOT_SETARG(args, num_args, XmNmarginWidth, 0);  /* no shadow margins */
  
  ih->handle = XtCreateManagedWidget(
    iupDialogGetChildIdStr(ih),  /* child identifier */
    xmFrameWidgetClass,          /* widget class */
    iupChildTreeGetNativeParentHandle(ih), /* widget parent */
    args, num_args);

  if (!ih->handle)
    return IUP_ERROR;

  ih->serial = iupDialogGetChildId(ih); /* must be after using the string */

  if (title)
  {
    Widget title_label;
    num_args = 0;
    /* Label */
    iupMOT_SETARG(args, num_args, XmNlabelType, XmSTRING); 
    iupMOT_SETARG(args, num_args, XmNmarginHeight, 0);  /* default padding */
    iupMOT_SETARG(args, num_args, XmNmarginWidth, 0);
    /* Frame Constraint */
    iupMOT_SETARG(args, num_args, XmNchildType, XmFRAME_TITLE_CHILD);
    title_label = XtCreateManagedWidget("title_label", xmLabelWidgetClass, ih->handle, args, num_args);
    iupmotSetString(title_label, XmNlabelString, title);
  }

  child_manager = XtVaCreateManagedWidget(
              "child_manager",
              xmBulletinBoardWidgetClass,
              ih->handle,
              /* Core */
              XmNborderWidth, 0,
              /* Manager */
              XmNshadowThickness, 0,
              XmNnavigationType, XmTAB_GROUP,
              /* BulletinBoard */
              XmNmarginWidth, 0,
              XmNmarginHeight, 0,
              XmNresizePolicy, XmRESIZE_NONE, /* no automatic resize of children */
              /* Frame Constraint */
              XmNchildType, XmFRAME_WORKAREA_CHILD,
              NULL);

  if (iupStrBoolean(IupGetGlobal("INPUTCALLBACKS")))
    XtAddEventHandler(child_manager, PointerMotionMask, False, (XtEventHandler)iupmotDummyPointerMotionEvent, NULL);

  /* initialize the widget */
  XtRealizeWidget(ih->handle);

  return IUP_NOERROR;
}

void iupdrvFrameInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = motFrameMapMethod;
  ic->GetInnerNativeContainerHandle = motFrameGetInnerNativeContainerHandleMethod;

  /* Driver Dependent Attribute functions */

  /* Overwrite Common */
  iupClassRegisterAttribute(ic, "STANDARDFONT", NULL, motFrameSetStandardFontAttrib, IUPAF_SAMEASSYSTEM, "DEFAULTFONT", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, motFrameSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BACKGROUND", NULL, motFrameSetBackgroundAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);

  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, motFrameSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGFGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "TITLE", NULL, motFrameSetTitleAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
}
