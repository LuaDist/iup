/** \file
 * \brief IupColorDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <Xm/Xm.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drvinfo.h"
#include "iup_dialog.h"

#include "iupmot_drv.h"


static int motColorDlgPopup(Ihandle* ih, int x, int y)
{
  (void)ih;
  (void)x;
  (void)y;
  return IUP_ERROR;
}

void iupdrvColorDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = motColorDlgPopup;
}
