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

/* GTK and Win32 share the same library in Windows */
/* GTK and Motif share the same library in UNIX */

void IupImageLibOpen(void)
{
#ifndef IUP_IMGLIB_DUMMY
  int motif=0, win32=0, gtk=0;
  char* driver;

  if (IupGetGlobal("_IUP_IMAGELIB_OPEN"))
    return;

  driver = IupGetGlobal("DRIVER");
  if (iupStrEqualNoCase(driver, "GTK"))
    gtk = 1;
  else if (iupStrEqualNoCase(driver, "Motif"))
    motif = 1;
  else if (iupStrEqualNoCase(driver, "Win32"))
    win32 = 1;

  IupSetGlobal("_IUP_IMAGELIB_OPEN", "1");

  /**************** Bitmaps *****************/

#ifndef WIN32
  if (motif)
    iupImglibBitmaps8Open();
#endif

#ifdef WIN32
  if (win32)
    iupImglibBitmapsOpen();
#endif  

  if (gtk)
    iupImglibBitmapsGtkOpen();

  /***************** Icons *****************/

#ifndef WIN32
  if (motif)
    iupImglibIcons8Open();
  else
#endif
    iupImglibIconsOpen();

  /***************** Logos *****************/

#ifdef IUP_IMGLIB_LARGE
#ifndef WIN32
  if (motif)
    iupImglibLogos8Open();
  else
#endif
    iupImglibLogosOpen();

#ifdef WIN32
  if (win32)
  {
    iupImglibLogosWin32Open();
    iupImglibLogosWin32MsgOpen();
  }
#endif  

  if (gtk)
    iupImglibLogosGtkOpen();
#endif  
#endif  
}
 
