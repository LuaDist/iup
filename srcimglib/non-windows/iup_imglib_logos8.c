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
#include "iup_imglib_logos8.h"

void iupImglibLogos8Open(void)
{
  iupImageStockSet("IUP_LogoTecgraf", load_image_LogoTecgraf8, 0);
  iupImageStockSet("IUP_LogoPUC-Rio", load_image_LogoPUC_Rio8, 0);
  iupImageStockSet("IUP_LogoBR", load_image_LogoBR8, 0);
  iupImageStockSet("IUP_LogoLua", load_image_LogoLua8, 0);
  iupImageStockSet("IUP_LogoTecgrafPUC-Rio", load_image_LogoTecgrafPUC_Rio8, 0);
  iupImageStockSet("IUP_LogoPetrobras", load_image_LogoPetrobras8, 0);
}
#endif
