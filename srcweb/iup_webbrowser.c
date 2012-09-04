/** \file
 * \brief Web Browser Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "iup.h"
#include "iupweb.h"
#ifdef WIN32
#include "iupole.h"
#endif

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_register.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_webbrowser.h"


int IupWebBrowserOpen(void)
{
#ifdef WIN32
  IupOleControlOpen();
#endif

  if (IupGetGlobal("_IUP_WEBBROWSER_OPEN"))
    return IUP_OPENED;

  iupRegisterClass(iupWebBrowserNewClass());

  IupSetGlobal("_IUP_WEBBROWSER_OPEN", "1");
  return IUP_NOERROR;
}

Ihandle *IupWebBrowser(void)
{
  return IupCreate("webbrowser");
}
