/** \file
 * \brief Timer Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_str.h"
#include "iup_stdcontrols.h"
#include "iup_timer.h"


static int iTimerSetRunAttrib(Ihandle *ih, const char *value)
{
  if (iupStrBoolean(value))
    iupdrvTimerRun(ih);
  else
    iupdrvTimerStop(ih);

  return 0;
}

static char* iTimerGetRunAttrib(Ihandle *ih)
{
  if (ih->serial > 0)
    return "YES";
  else
    return "NO";
}

static char* iTimerGetWidAttrib(Ihandle *ih)
{
  char* str = iupStrGetMemory(50);
  sprintf(str, "%d", ih->serial);
  return str;
}

static void iTimerDestroyMethod(Ihandle* ih)
{
  iupdrvTimerStop(ih);
}

/******************************************************************************/

Ihandle* IupTimer(void)
{
  return IupCreate("timer");
}

Iclass* iupTimerNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "timer";
  ic->format = NULL;  /* no parameters */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupTimerNewClass;
  ic->Destroy = iTimerDestroyMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "ACTION_CB", "");

  /* Attribute functions */
  iupClassRegisterAttribute(ic, "WID", iTimerGetWidAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
  iupClassRegisterAttribute(ic, "RUN", iTimerGetRunAttrib, iTimerSetRunAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TIME", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupdrvTimerInitClass(ic);

  return ic;
}
