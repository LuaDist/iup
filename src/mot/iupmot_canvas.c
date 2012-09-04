/** \file
 * \brief Canvas Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <Xm/Xm.h>
#include <Xm/ScrolledW.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrollBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_attrib.h"
#include "iup_dialog.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_canvas.h"
#include "iup_key.h"

#include "iupmot_drv.h"
#include "iupmot_color.h"



static void motDialogScrollbarCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
  int op = (int)client_data, ipage, ipos;
  Ihandle *ih;
  IFniff cb;
  double posx, posy;
  (void)call_data;

  XtVaGetValues(w, XmNuserData, &ih, NULL);
  if (!ih) return;

  XtVaGetValues(w, 
    XmNvalue, &ipos, 
    XmNpageIncrement, &ipage,
    NULL);

  if (op > IUP_SBDRAGV)
  {
    iupCanvasCalcScrollRealPos(iupAttribGetFloat(ih,"XMIN"), iupAttribGetFloat(ih,"XMAX"), &posx, 
                               IUP_SB_MIN, IUP_SB_MAX, ipage, &ipos);
    ih->data->posx = (float)posx;
    posy = ih->data->posy;
  }
  else
  {
    iupCanvasCalcScrollRealPos(iupAttribGetFloat(ih,"YMIN"), iupAttribGetFloat(ih,"YMAX"), &posy, 
                               IUP_SB_MIN, IUP_SB_MAX, ipage, &ipos);
    ih->data->posy = (float)posy;
    posx = ih->data->posx;
  }

  cb = (IFniff)IupGetCallback(ih,"SCROLL_CB");
  if (cb)
    cb(ih, op, (float)posx, (float)posy);
  else
  {
    IFnff cb = (IFnff)IupGetCallback(ih,"ACTION");
    if (cb)
      cb (ih, (float)posx, (float)posy);
  }
}

static void motCanvasResizeCallback(Widget w, Ihandle *ih, XtPointer call_data)
{
  Dimension width, height;
  IFnii cb;
  (void)call_data;

  if (!XtWindow(w) || !ih) return;

  /* client size */
  XtVaGetValues(w, XmNwidth, &width,
                   XmNheight, &height,
                   NULL);

  cb = (IFnii)IupGetCallback(ih,"RESIZE_CB");
  if (cb)
    cb(ih,width,height);
}

static int motCanvasSetBgColorAttrib(Ihandle* ih, const char* value);

static void motCanvasExposeCallback(Widget w, Ihandle *ih, XtPointer call_data)
{
  IFnff cb;
  (void)call_data;

  if (!XtWindow(w) || !ih) return;

  cb = (IFnff)IupGetCallback(ih,"ACTION");
  if (cb)
  {
    if (!iupAttribGet(ih, "_IUPMOT_NO_BGCOLOR"))
      motCanvasSetBgColorAttrib(ih, iupAttribGetStr(ih, "BGCOLOR"));  /* reset to update window attributes */

    cb (ih, ih->data->posx, ih->data->posy);
  }
}

static void motCanvasInputCallback(Widget w, Ihandle *ih, XtPointer call_data)
{
  XEvent *evt = ((XmDrawingAreaCallbackStruct*)call_data)->event;

  if (!XtWindow(w) || !ih) return;

  switch (evt->type)
  {
  case ButtonPress:
    /* Force focus on canvas click */
    if (iupAttribGetBoolean(ih, "CANFOCUS"))
      XmProcessTraversal(w, XmTRAVERSE_CURRENT);
    /* break missing on purpose... */
  case ButtonRelease:
    {
      XButtonEvent *but_evt = (XButtonEvent*)evt;
      Boolean cont = True;
      iupmotButtonPressReleaseEvent(w, ih, evt, &cont);
      if (cont == False)
        return;

      if ((evt->type==ButtonPress) && (but_evt->button==Button4 || but_evt->button==Button5))
      {                                             
        IFnfiis wcb = (IFnfiis)IupGetCallback(ih, "WHEEL_CB");
        if (wcb)
        {
          char status[IUPKEY_STATUS_SIZE] = IUPKEY_STATUS_INIT;
          int delta = but_evt->button==Button4? 1: -1;
          iupmotButtonKeySetStatus(but_evt->state, but_evt->button, status, 0);

          wcb(ih, (float)delta, but_evt->x, but_evt->y, status);
        }
        else
        {
          IFniff scb = (IFniff)IupGetCallback(ih,"SCROLL_CB");
          float posy = ih->data->posy;
          int delta = but_evt->button==Button4? 1: -1;
          int op = but_evt->button==Button4? IUP_SBUP: IUP_SBDN;
          posy -= delta*iupAttribGetFloat(ih, "DY")/10.0f;
          IupSetfAttribute(ih, "POSY", "%g", posy);
          if (scb)
            scb(ih,op,ih->data->posx,ih->data->posy);
        }
      }
    }
    break;
  }
}

static void motCanvasSetScrollInfo(Widget sb, int imin, int imax, int ipos, int ipage, int iline)
{
  XtVaSetValues(sb,
                XmNminimum,       imin,
                XmNmaximum,       imax,
                XmNvalue,         ipos,
                XmNincrement,     iline,
                XmNpageIncrement, ipage,
                XmNsliderSize,    ipage,  /* to make the thumb proportional */
                NULL);
}

static int motCanvasSetDXAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_HORIZ)
  {
    double posx, xmin, xmax, linex;
    float dx;
    int iposx, ipagex, ilinex;
    Widget sb_horiz, sb_win = (Widget)iupAttribGet(ih, "_IUP_EXTRAPARENT");
    XtVaGetValues(sb_win, XmNhorizontalScrollBar, &sb_horiz, NULL);
    if (!sb_horiz) return 1;

    if (!iupStrToFloat(value, &dx))
      return 1;

    xmin = iupAttribGetFloat(ih, "XMIN");
    xmax = iupAttribGetFloat(ih, "XMAX");
    posx = ih->data->posx;

    iupCanvasCalcScrollIntPos(xmin, xmax, dx, posx, 
                              IUP_SB_MIN, IUP_SB_MAX, &ipagex, &iposx);

    if (!iupAttribGet(ih,"LINEX"))
    {
      ilinex = ipagex/10;
      if (!ilinex)
        ilinex = 1;
    }
    else
    {
      /* line and page convertions are the same */
      linex = iupAttribGetFloat(ih,"LINEX");
      iupCanvasCalcScrollIntPos(xmin, xmax, linex, 0, 
                                IUP_SB_MIN, IUP_SB_MAX, &ilinex,  NULL);
    }

    if (dx >= (xmax-xmin))
    {
      if (iupAttribGetBoolean(ih, "XAUTOHIDE"))
        XtUnmanageChild(sb_horiz);
      else
        XtSetSensitive(sb_horiz, 0);
      return 1;
    }
    else
    {
      if (!XtIsManaged(sb_horiz))
        XtManageChild(sb_horiz);
      XtSetSensitive(sb_horiz, 1);
    }

    motCanvasSetScrollInfo(sb_horiz, IUP_SB_MIN, IUP_SB_MAX, iposx, ipagex, ilinex);

    /* update position because it could be corrected */
    iupCanvasCalcScrollRealPos(xmin, xmax, &posx, 
                               IUP_SB_MIN, IUP_SB_MAX, ipagex, &iposx);

    ih->data->posx = (float)posx;
  }
  return 1;
}

static int motCanvasSetPosXAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_HORIZ)
  {
    double xmin, xmax, dx;
    float posx;
    int iposx, ipagex;
    Widget sb_horiz, sb_win = (Widget)iupAttribGet(ih, "_IUP_EXTRAPARENT");
    XtVaGetValues(sb_win, XmNhorizontalScrollBar, &sb_horiz, NULL);
    if (!sb_horiz) return 1;

    if (!iupStrToFloat(value, &posx))
      return 1;

    xmin = iupAttribGetFloat(ih, "XMIN");
    xmax = iupAttribGetFloat(ih, "XMAX");
    dx = iupAttribGetFloat(ih, "DX");

    if (posx < xmin) posx = (float)xmin;
    if (posx > (xmax - dx)) posx = (float)(xmax - dx);
    ih->data->posx = posx;

    iupCanvasCalcScrollIntPos(xmin, xmax, dx, posx, 
                              IUP_SB_MIN, IUP_SB_MAX, &ipagex, &iposx);

    XtVaSetValues(sb_horiz, XmNvalue, iposx, NULL);
  }
  return 1;
}

static int motCanvasSetDYAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_VERT)
  {
    double posy, ymin, ymax, liney;
    float dy;
    int iposy, ipagey, iliney;
    Widget sb_vert, sb_win = (Widget)iupAttribGet(ih, "_IUP_EXTRAPARENT");
    XtVaGetValues(sb_win, XmNverticalScrollBar, &sb_vert, NULL);
    if (!sb_vert) return 1;

    if (!iupStrToFloat(value, &dy))
      return 1;

    ymin = iupAttribGetFloat(ih, "YMIN");
    ymax = iupAttribGetFloat(ih, "YMAX");
    posy = ih->data->posy;

    iupCanvasCalcScrollIntPos(ymin, ymax, dy, posy, 
                              IUP_SB_MIN, IUP_SB_MAX, &ipagey, &iposy);

    if (!iupAttribGet(ih,"LINEY"))
    {
      iliney = ipagey/10;
      if (!iliney)
        iliney = 1;
    }
    else
    {
      /* line and page convertions are the same */
      liney = iupAttribGetFloat(ih,"LINEY");
      iupCanvasCalcScrollIntPos(ymin, ymax, liney, 0, 
                                IUP_SB_MIN, IUP_SB_MAX, &iliney,  NULL);
    }

    if (dy >= (ymax-ymin))
    {
      if (iupAttribGetBoolean(ih, "YAUTOHIDE"))
        XtUnmanageChild(sb_vert);
      else
        XtSetSensitive(sb_vert, 0);
      return 1;
    }
    else
    {
      if (!XtIsManaged(sb_vert))
        XtManageChild(sb_vert);
      XtSetSensitive(sb_vert, 1);
    }

    motCanvasSetScrollInfo(sb_vert, IUP_SB_MIN, IUP_SB_MAX, iposy, ipagey, iliney);

    /* update position because it could be corrected */
    iupCanvasCalcScrollRealPos(ymin, ymax, &posy, 
                               IUP_SB_MIN, IUP_SB_MAX, ipagey, &iposy);

    ih->data->posy = (float)posy;
  }
  return 1;
}

static int motCanvasSetPosYAttrib(Ihandle* ih, const char *value)
{
  if (ih->data->sb & IUP_SB_VERT)
  {
    double ymin, ymax, dy;
    float posy;
    int iposy, ipagey;
    Widget sb_vert, sb_win = (Widget)iupAttribGet(ih, "_IUP_EXTRAPARENT");
    XtVaGetValues(sb_win, XmNverticalScrollBar, &sb_vert, NULL);
    if (!sb_vert) return 1;

    if (!iupStrToFloat(value, &posy))
      return 1;

    ymin = iupAttribGetFloat(ih, "YMIN");
    ymax = iupAttribGetFloat(ih, "YMAX");
    dy = iupAttribGetFloat(ih, "DY");

    if (posy < ymin) posy = (float)ymin;
    if (posy > (ymax - dy)) posy = (float)(ymax - dy);
    ih->data->posy = posy;

    iupCanvasCalcScrollIntPos(ymin, ymax, dy, posy, 
                              IUP_SB_MIN, IUP_SB_MAX, &ipagey, &iposy);

    XtVaSetValues(sb_vert, XmNvalue, iposy, NULL);
  }
  return 1;
}

static char* motCanvasGetXDisplayAttrib(Ihandle *ih)
{
  (void)ih;
  return (char*)iupmot_display;
}

static char* motCanvasGetXScreenAttrib(Ihandle *ih)
{
  (void)ih;
  return (char*)iupmot_screen;
}

static char* motCanvasGetXWindowAttrib(Ihandle *ih)
{
  return (char*)XtWindow(ih->handle);
}

static char* motCanvasGetDrawSizeAttrib(Ihandle *ih)
{
  char* str = iupStrGetMemory(20);
 
  Dimension width, height;
  XtVaGetValues(ih->handle, XmNwidth,  &width,
                            XmNheight, &height, 
                            NULL);

  sprintf(str, "%dx%d", (int)width, (int)height);
  return str;
}

static int motCanvasSetBgColorAttrib(Ihandle* ih, const char* value)
{
  Pixel color;

  /* ignore given value, must use only from parent for the scrollbars */
  char* parent_value = iupBaseNativeParentGetBgColor(ih);

  color = iupmotColorGetPixelStr(parent_value);
  if (color != (Pixel)-1)
  {
    Widget sb;
    Widget sb_win = (Widget)iupAttribGet(ih, "_IUP_EXTRAPARENT");

    iupmotSetBgColor(sb_win, color);

    XtVaGetValues(sb_win, XmNverticalScrollBar, &sb, NULL);
    if (sb) iupmotSetBgColor(sb, color);

    XtVaGetValues(sb_win, XmNhorizontalScrollBar, &sb, NULL);
    if (sb) iupmotSetBgColor(sb, color);
  }

  if (!IupGetCallback(ih, "ACTION")) 
    iupdrvBaseSetBgColorAttrib(ih, value);  /* Use the given value only here */
  else
  {
    XSetWindowAttributes attrs;
    attrs.background_pixmap = None;
    XChangeWindowAttributes(iupmot_display, XtWindow(ih->handle), CWBackPixmap, &attrs);
    iupAttribSetStr(ih, "_IUPMOT_NO_BGCOLOR", "1");
  }

  return 1;
}

static void motCanvasLayoutUpdateMethod(Ihandle *ih)
{
  Widget sb_win = (Widget)iupAttribGet(ih, "_IUP_EXTRAPARENT");
  Dimension border;

  /* IMPORTANT:
   The ScrolledWindow border, added by the Core, is NOT included in the Motif size.
   So when setting the size, we must compensate the border, 
   so the actual size will be the size we expect.
  */

  XtVaGetValues(sb_win, XmNborderWidth, &border, NULL);

  /* avoid abort in X */
  if (ih->currentwidth <= 2*border) ih->currentwidth = 2*border+1;
  if (ih->currentheight <= 2*border) ih->currentheight = 2*border+1;

  XtVaSetValues(sb_win,
    XmNx, (XtArgVal)ih->x,
    XmNy, (XtArgVal)ih->y,
    XmNwidth, (XtArgVal)(ih->currentwidth-2*border),
    XmNheight, (XtArgVal)(ih->currentheight-2*border),
    NULL);
}

static int motCanvasMapMethod(Ihandle* ih)
{
  Widget sb_win;
  char *visual;
  int num_args = 0;
  Arg args[20];

  if (!ih->parent)
    return IUP_ERROR;

  ih->data->sb = iupBaseGetScrollbar(ih);

  /******************************/
  /* Create the scrolled window */
  /******************************/

  iupMOT_SETARG(args, num_args, XmNmappedWhenManaged, False);  /* not visible when managed */
  iupMOT_SETARG(args, num_args, XmNscrollingPolicy, XmAPPLICATION_DEFINED);
  iupMOT_SETARG(args, num_args, XmNvisualPolicy, XmVARIABLE);
  iupMOT_SETARG(args, num_args, XmNspacing, 0); /* no space between scrollbars and draw area */
  iupMOT_SETARG(args, num_args, XmNshadowThickness, 0);

  if (iupAttribGetBoolean(ih, "BORDER"))
  {
    iupMOT_SETARG(args, num_args, XmNborderWidth, 1);
    iupMOT_SETARG(args, num_args, XmNborderColor, iupmotColorGetPixelStr("0 0 0"));
  }
  else
    iupMOT_SETARG(args, num_args, XmNborderWidth, 0);
  
  sb_win = XtCreateManagedWidget(
    iupDialogGetChildIdStr(ih),  /* child identifier */
    xmScrolledWindowWidgetClass,     /* widget class */
    iupChildTreeGetNativeParentHandle(ih), /* widget parent */
    args, num_args);

  if (!sb_win)
    return IUP_ERROR;

  ih->serial = iupDialogGetChildId(ih); /* must be after using the string */

  /****************************/
  /* Create the drawing area  */
  /****************************/

  num_args = 0;
  iupMOT_SETARG(args, num_args, XmNmarginHeight, 0);  /* no shadow margins */
  iupMOT_SETARG(args, num_args, XmNmarginWidth, 0);  /* no shadow margins */
  iupMOT_SETARG(args, num_args, XmNshadowThickness, 0);
  iupMOT_SETARG(args, num_args, XmNresizePolicy, XmRESIZE_NONE); /* no automatic resize of children */
  if (ih->iclass->is_interactive)
  {
    iupMOT_SETARG(args, num_args, XmNnavigationType, XmTAB_GROUP); /* include in navigation */

    if (iupAttribGetBoolean(ih, "CANFOCUS"))
      iupMOT_SETARG(args, num_args, XmNtraversalOn, True);
    else
      iupMOT_SETARG(args, num_args, XmNtraversalOn, False);
  }
  else
  {
    iupMOT_SETARG(args, num_args, XmNnavigationType, XmNONE);
    iupMOT_SETARG(args, num_args, XmNtraversalOn, False);
  }

  iupMOT_SETARG(args, num_args, XmNx, 0);  /* x-position */
  iupMOT_SETARG(args, num_args, XmNy, 0);  /* y-position */
  iupMOT_SETARG(args, num_args, XmNwidth, 10);  /* default width to avoid 0 */
  iupMOT_SETARG(args, num_args, XmNheight, 10); /* default height to avoid 0 */

  visual = IupGetAttribute(ih, "VISUAL");   /* defined by the OpenGL Canvas or NULL */
  if (visual)
  {
    Colormap colormap = (Colormap)iupAttribGet(ih, "COLORMAP");
    if (colormap)
      iupMOT_SETARG(args, num_args, XmNcolormap,colormap);

    iupmotDialogSetVisual(ih, visual);
  }

  ih->handle = XtCreateManagedWidget(
    "draw_area",                 /* child identifier */
    xmDrawingAreaWidgetClass,    /* widget class */
    sb_win,                  /* widget parent */
    args, num_args);

  if (!ih->handle)
  {
    XtDestroyWidget(sb_win);
    return IUP_ERROR;
  }

  if (visual)
    iupmotDialogResetVisual(ih);

  iupAttribSetStr(ih, "_IUP_EXTRAPARENT", (char*)sb_win);
  XtVaSetValues(sb_win, XmNworkWindow, ih->handle, NULL);

  {
    XSetWindowAttributes attrs;
    attrs.bit_gravity = ForgetGravity; /* For the DrawingArea widget gets Expose events when you resize it to be smaller. */

    if (iupAttribGetBoolean(ih, "BACKINGSTORE"))
      attrs.backing_store = WhenMapped;
    else
      attrs.backing_store = NotUseful;

    XChangeWindowAttributes(iupmot_display, XtWindow(ih->handle), CWBitGravity|CWBackingStore, &attrs);
  }

  if (ih->data->sb & IUP_SB_HORIZ)
  {
    Widget sb_horiz = XtVaCreateManagedWidget("sb_horiz",
      xmScrollBarWidgetClass, sb_win,
      XmNorientation, XmHORIZONTAL,
      XmNsliderMark, XmTHUMB_MARK,
      XmNuserData, ih,
      NULL);

    XtAddCallback(sb_horiz, XmNvalueChangedCallback, motDialogScrollbarCallback, (void*)IUP_SBPOSH);
    XtAddCallback(sb_horiz, XmNdragCallback, motDialogScrollbarCallback, (void*)IUP_SBDRAGH);
    XtAddCallback(sb_horiz, XmNdecrementCallback, motDialogScrollbarCallback, (void*)IUP_SBLEFT);
    XtAddCallback(sb_horiz, XmNincrementCallback, motDialogScrollbarCallback, (void*)IUP_SBRIGHT);
    XtAddCallback(sb_horiz, XmNpageDecrementCallback, motDialogScrollbarCallback, (void*)IUP_SBPGLEFT);
    XtAddCallback(sb_horiz, XmNpageIncrementCallback, motDialogScrollbarCallback, (void*)IUP_SBPGRIGHT);

    XtVaSetValues(sb_win, XmNhorizontalScrollBar, sb_horiz, NULL);
  }

  if (ih->data->sb & IUP_SB_VERT)
  {
    Widget sb_vert = XtVaCreateManagedWidget("sb_vert",
      xmScrollBarWidgetClass, sb_win,
      XmNorientation, XmVERTICAL,
      XmNsliderMark, XmTHUMB_MARK,
      XmNuserData, ih,
      NULL);

    XtAddCallback(sb_vert, XmNvalueChangedCallback, motDialogScrollbarCallback, (void*)IUP_SBPOSV);
    XtAddCallback(sb_vert, XmNdragCallback, motDialogScrollbarCallback, (void*)IUP_SBDRAGV);
    XtAddCallback(sb_vert, XmNdecrementCallback, motDialogScrollbarCallback, (void*)IUP_SBUP);
    XtAddCallback(sb_vert, XmNincrementCallback, motDialogScrollbarCallback, (void*)IUP_SBDN);
    XtAddCallback(sb_vert, XmNpageDecrementCallback, motDialogScrollbarCallback, (void*)IUP_SBPGUP);
    XtAddCallback(sb_vert, XmNpageIncrementCallback, motDialogScrollbarCallback, (void*)IUP_SBPGDN);

    XtVaSetValues(sb_win, XmNverticalScrollBar, sb_vert, NULL);
  }

  XtAddCallback(ih->handle, XmNhelpCallback, (XtCallbackProc)iupmotHelpCallback, (XtPointer)ih);
  XtAddCallback(ih->handle, XmNexposeCallback, (XtCallbackProc)motCanvasExposeCallback, (XtPointer)ih);
  XtAddCallback(ih->handle, XmNresizeCallback, (XtCallbackProc)motCanvasResizeCallback,  (XtPointer)ih);
  XtAddCallback(ih->handle, XmNinputCallback,  (XtCallbackProc)motCanvasInputCallback,   (XtPointer)ih);

  XtAddEventHandler(ih->handle, EnterWindowMask, False,(XtEventHandler)iupmotEnterLeaveWindowEvent, (XtPointer)ih);
  XtAddEventHandler(ih->handle, LeaveWindowMask, False,(XtEventHandler)iupmotEnterLeaveWindowEvent, (XtPointer)ih);

  XtAddEventHandler(ih->handle, FocusChangeMask, False,(XtEventHandler)iupmotFocusChangeEvent,        (XtPointer)ih);
  XtAddEventHandler(ih->handle, KeyPressMask,    False, (XtEventHandler)iupmotKeyPressEvent,    (XtPointer)ih);
  XtAddEventHandler(ih->handle, KeyReleaseMask,  False, (XtEventHandler)iupmotCanvasKeyReleaseEvent,  (XtPointer)ih);
  XtAddEventHandler(ih->handle, PointerMotionMask, False, (XtEventHandler)iupmotPointerMotionEvent, (XtPointer)ih);

  /* initialize the widget */
  XtRealizeWidget(sb_win);

  return IUP_NOERROR;
}

void iupdrvCanvasInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = motCanvasMapMethod;
  ic->LayoutUpdate = motCanvasLayoutUpdateMethod;

  /* Driver Dependent Attribute functions */

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, motCanvasSetBgColorAttrib, "255 255 255", NULL, IUPAF_DEFAULT);  /* force new default value */
  
  /* IupCanvas only */
  iupClassRegisterAttribute(ic, "DRAWSIZE", motCanvasGetDrawSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CURSOR", NULL, iupdrvBaseSetCursorAttrib, IUPAF_SAMEASSYSTEM, "ARROW", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DX", NULL, motCanvasSetDXAttrib, "0.1", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "DY", NULL, motCanvasSetDYAttrib, "0.1", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "POSX", iupCanvasGetPosXAttrib, motCanvasSetPosXAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "POSY", iupCanvasGetPosYAttrib, motCanvasSetPosYAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
  iupClassRegisterAttribute(ic, "YAUTOHIDE", NULL, NULL, "YES", NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "XAUTOHIDE", NULL, NULL, "YES", NULL, IUPAF_NOT_MAPPED);

  /* IupCanvas X only */
  iupClassRegisterAttribute(ic, "XWINDOW", motCanvasGetXWindowAttrib, NULL, NULL, NULL, IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "XDISPLAY", motCanvasGetXDisplayAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "XSCREEN", motCanvasGetXScreenAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "BACKINGSTORE", NULL, NULL, "YES", NULL, IUPAF_NOT_MAPPED);

  /* Not Supported */
  iupClassRegisterAttribute(ic, "TOUCH", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
}
