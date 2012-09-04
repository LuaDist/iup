/** \file
* \brief Progress bar Control
*
* See Copyright Notice in "iup.h"
*/

#undef GTK_DISABLE_DEPRECATED
#include <gtk/gtk.h>

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
#include "iup_progressbar.h"
#include "iup_drv.h"

#include "iupgtk_drv.h"


static int gtkProgressBarTimeCb(Ihandle* timer)
{
  Ihandle* ih = (Ihandle*)iupAttribGet(timer, "_IUP_PROGRESSBAR");
  gtk_progress_bar_pulse((GtkProgressBar*)ih->handle);
  return IUP_DEFAULT;
}

static int gtkProgressBarSetMarqueeAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->marquee)
    return 0;

  if (iupStrBoolean(value))
    IupSetAttribute(ih->data->timer, "RUN", "YES");
  else
    IupSetAttribute(ih->data->timer, "RUN", "NO");

  return 1;
}

static int gtkProgressBarSetValueAttrib(Ihandle* ih, const char* value)
{
  GtkProgressBar* pbar = (GtkProgressBar*)ih->handle;

  if (ih->data->marquee)
    return 0;

  if (!value)
    ih->data->value = 0;
  else
    ih->data->value = atof(value);
  iProgressBarCropValue(ih);

  gtk_progress_bar_set_fraction(pbar, (ih->data->value - ih->data->vmin) / (ih->data->vmax - ih->data->vmin));

  return 0;
}

static int gtkProgressBarSetDashedAttrib(Ihandle* ih, const char* value)
{
  GtkProgressBar* pbar = (GtkProgressBar*)ih->handle;

  if (ih->data->marquee)
    return 0;

  /* gtk_progress_bar_set_bar_style is deprecated */
  if (iupStrBoolean(value))
  {
    ih->data->dashed = 1;
    gtk_progress_bar_set_bar_style(pbar, GTK_PROGRESS_DISCRETE);
  }
  else /* Default */
  {
    ih->data->dashed = 0;
    gtk_progress_bar_set_bar_style(pbar, GTK_PROGRESS_CONTINUOUS);
  }

  return 0;
}

static int gtkProgressBarMapMethod(Ihandle* ih)
{
  ih->handle = gtk_progress_bar_new();
  if (!ih->handle)
    return IUP_ERROR;

  /* add to the parent, all GTK controls must call this. */
  iupgtkBaseAddToParent(ih);

  gtk_widget_realize(ih->handle);

  if (iupStrEqualNoCase(iupAttribGetStr(ih, "ORIENTATION"), "VERTICAL"))
  {
    gtk_progress_bar_set_orientation((GtkProgressBar*)ih->handle, GTK_PROGRESS_BOTTOM_TO_TOP);

    if (ih->currentheight < ih->currentwidth)
    {
      int tmp = ih->currentheight;
      ih->currentheight = ih->currentwidth;
      ih->currentwidth = tmp;
    }
  }
  else
    gtk_progress_bar_set_orientation((GtkProgressBar*)ih->handle, GTK_PROGRESS_LEFT_TO_RIGHT);

  if (iupAttribGetBoolean(ih, "MARQUEE"))
  {
    ih->data->marquee = 1;
    gtk_progress_set_activity_mode((GtkProgress*)ih->handle, TRUE);
    ih->data->timer = IupTimer();
    IupSetCallback(ih->data->timer, "ACTION_CB", (Icallback)gtkProgressBarTimeCb);
    IupSetAttribute(ih->data->timer, "TIME", "100");
    iupAttribSetStr(ih->data->timer, "_IUP_PROGRESSBAR", (char*)ih);
    gtk_progress_bar_set_pulse_step((GtkProgressBar*)ih->handle, 0.02);
  }
  else
  {
    gtk_progress_set_activity_mode((GtkProgress*)ih->handle, FALSE);
    ih->data->marquee = 0;
  }

  return IUP_NOERROR;
}

void iupdrvProgressBarInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = gtkProgressBarMapMethod;

  /* Driver Dependent Attribute functions */
  
  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT);
  
  /* Special */
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);

  /* IupProgressBar only */
  iupClassRegisterAttribute(ic, "VALUE",  iProgressBarGetValueAttrib,  gtkProgressBarSetValueAttrib,  NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DASHED", iProgressBarGetDashedAttrib, gtkProgressBarSetDashedAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ORIENTATION", NULL, NULL, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MARQUEE",     NULL, gtkProgressBarSetMarqueeAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DASHED",      NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
