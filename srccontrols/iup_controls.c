/** \file
 * \brief initializes the additional controls.
 *
 * See Copyright Notice in "iup.h"
 */


#include "iup.h"
#include "iupcontrols.h"

#include "iup_object.h"
#include "iup_childtree.h"
#include "iup_register.h"
#include "iup_controls.h"
#include "iup_attrib.h"


int IupControlsOpen(void)
{
  if (IupGetGlobal("_IUP_CONTROLS_OPEN"))
    return IUP_OPENED;

  iupRegisterClass(iupDialNewClass());
  iupRegisterClass(iupCellsNewClass());
  iupRegisterClass(iupColorbarNewClass());
  iupRegisterClass(iupColorBrowserNewClass());
  iupRegisterClass(iupMatrixNewClass());
  iupRegisterClass(iupGaugeNewClass());
  iupRegisterClass(iupColorBrowserDlgNewClass());

  IupSetGlobal("_IUP_CONTROLS_OPEN", "1");

  return IUP_NOERROR;
}

void IupControlsClose(void)  /* for backward compatibility */
{
}

char *iupControlBaseGetParentBgColor(Ihandle* ih)
{
  return IupGetAttribute(iupChildTreeGetNativeParent(ih), "BGCOLOR");
}

char *iupControlBaseGetBgColorAttrib(Ihandle* ih)
{
  /* check the hash table */
  char *color = iupAttribGet(ih, "BGCOLOR");

  /* If not defined check native definition from parent */
  if (!color)
    color = iupControlBaseGetParentBgColor(ih);

  return color;
}

