/** \file
 * \brief IupImgLib
 *
 * See Copyright Notice in iup.h
 */

#include <stdlib.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_image.h"

#include "iup_imglib.h"

#ifdef IUP_IMGLIB_LARGE
/* source code, included only here */
#include "iup_imglib_logos.h"

void iupImglibLogosOpen(void)
{
  iupImageStockSet("IUP_LogoTecgraf", load_image_LogoTecgraf, 0);
  iupImageStockSet("IUP_LogoPUC-Rio", load_image_LogoPUC_Rio, 0);
  iupImageStockSet("IUP_LogoBR", load_image_LogoBR, 0);
  iupImageStockSet("IUP_LogoLua", load_image_LogoLua, 0);
  iupImageStockSet("IUP_LogoTecgrafPUC-Rio", load_image_LogoTecgrafPUC_Rio, 0);
  iupImageStockSet("IUP_LogoPetrobras", load_image_LogoPetrobras, 0);
}

#ifdef WIN32
void iupImglibLogosWin32MsgOpen(void)
{
  iupImageStockSet("IUP_LogoMessageError", load_image_LogoMessageError, 0);
  iupImageStockSet("IUP_LogoMessageHelp", load_image_LogoMessageHelp, 0);
  iupImageStockSet("IUP_LogoMessageInfo", load_image_LogoMessageInfo, 0);
  iupImageStockSet("IUP_LogoMessageSecurity", load_image_LogoMessageSecurity, 0);
  iupImageStockSet("IUP_LogoMessageWarning", load_image_LogoMessageWarning, 0);
}
#endif

void iupImglibLogosGtkOpen(void)
{
  iupImageStockSet("IUP_LogoMessageError", 0, "gtk-dialog-error");
  iupImageStockSet("IUP_LogoMessageHelp", 0, "gtk-dialog-question");
  iupImageStockSet("IUP_LogoMessageInfo", 0, "gtk-dialog-info");
  iupImageStockSet("IUP_LogoMessageSecurity", 0, "gtk-dialog-authentication");
  iupImageStockSet("IUP_LogoMessageWarning", 0, "gtk-dialog-warning");
}
#endif
