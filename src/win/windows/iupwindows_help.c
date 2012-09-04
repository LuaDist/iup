/** \file
 * \brief Windows Driver IupHelp
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <shellapi.h>

#include "iup.h"

int IupHelp(const char* url)
{
  int err = (int)ShellExecute(GetDesktopWindow(), "open", url, NULL, NULL, SW_SHOWNORMAL);
  if (err <= 32) 
  {
    switch (err) 
    {
      case ERROR_FILE_NOT_FOUND:
      case ERROR_PATH_NOT_FOUND:
        return -2; /* File not found */
        break;
      default:
        return -1; /* Generic error */
        break;
    }
  }
  return 1;
}
