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

/* source code, included only here */
#include "iup_imglib_icons8.h"

void iupImglibIcons8Open(void)
{
  iupImageStockSet("IUP_Webcam", load_image_Webcam8, 0);

  iupImageStockSet("IUP_Tecgraf", load_image_Tecgraf8, 0);
  iupImageStockSet("IUP_PUC-Rio", load_image_PUC_Rio8, 0);
  iupImageStockSet("IUP_BR", load_image_BR8, 0);
  iupImageStockSet("IUP_Lua", load_image_Lua8, 0);
#ifdef IUP_IMGLIB_LARGE
  iupImageStockSet("IUP_TecgrafPUC-Rio", load_image_TecgrafPUC_Rio8, 0);
  iupImageStockSet("IUP_Petrobras", load_image_Petrobras8, 0);
#endif
}
