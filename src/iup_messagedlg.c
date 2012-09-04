/** \file
 * \brief IupMessageDlg class
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_stdcontrols.h"
#include "iup_register.h"
#include "iup_str.h"


Ihandle* IupMessageDlg(void)
{
  return IupCreate("messagedlg");
}

Iclass* iupMessageDlgNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("dialog"));

  ic->name = "messagedlg";
  ic->nativetype = IUP_TYPEDIALOG;
  ic->is_interactive = 1;

  ic->New = iupMessageDlgNewClass;

  /* reset not used native dialog methods */
  ic->parent->LayoutUpdate = NULL;
  ic->parent->SetChildrenPosition = NULL;
  ic->parent->Map = NULL;
  ic->parent->UnMap = NULL;

  iupdrvMessageDlgInitClass(ic);

  /* only the default values */
  iupClassRegisterAttribute(ic, "DIALOGTYPE", NULL, NULL, IUPAF_SAMEASSYSTEM, "MESSAGE", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BUTTONS", NULL, NULL, IUPAF_SAMEASSYSTEM, "OK", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BUTTONDEFAULT", NULL, NULL, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BUTTONRESPONSE", NULL, NULL, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);

  return ic;
}

void IupMessage(const char* title, const char* message)
{
  Ihandle* dlg = IupCreate("messagedlg");

  IupSetAttribute(dlg, "TITLE", (char*)title);
  IupSetAttribute(dlg, "VALUE", (char*)message);
  IupSetAttribute(dlg, "PARENTDIALOG", IupGetGlobal("PARENTDIALOG"));

  IupPopup(dlg, IUP_CENTER, IUP_CENTER);
  IupDestroy(dlg);
}

void IupMessagef(const char *title, const char *format, ...)
{
  int size;
  char* str = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, format);
  vsnprintf(str, size, format, arglist);
  va_end (arglist);
  IupMessage(title, iupStrGetMemoryCopy(str));
}
