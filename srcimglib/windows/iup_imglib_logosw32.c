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
#include "iup_imglib_logosw32.h"

void iupImglibLogosWin32Open(void)
{
  iupImageStockSet("IUP_DeviceCamera", load_image_DeviceCamera, 0);
  iupImageStockSet("IUP_DeviceCD", load_image_DeviceCD, 0);
  iupImageStockSet("IUP_DeviceCellPhone", load_image_DeviceCellPhone, 0);
  iupImageStockSet("IUP_DeviceComputer", load_image_DeviceComputer, 0);
  iupImageStockSet("IUP_DeviceFax", load_image_DeviceFax, 0);
  iupImageStockSet("IUP_DeviceMP3", load_image_DeviceMP3, 0);
  iupImageStockSet("IUP_DeviceNetwork", load_image_DeviceNetwork, 0);
  iupImageStockSet("IUP_DevicePDA", load_image_DevicePDA, 0);
  iupImageStockSet("IUP_DevicePrinter", load_image_DevicePrinter, 0);
  iupImageStockSet("IUP_DeviceScanner", load_image_DeviceScanner, 0);
  iupImageStockSet("IUP_DeviceSound", load_image_DeviceSound, 0);
  iupImageStockSet("IUP_DeviceVideo", load_image_DeviceVideo, 0);
}
#endif
