/** \file
 * \brief String Utilities
 *
 * See Copyright Notice in "iup.h"
 */

 
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <limits.h>  
#include <stdarg.h>

#include "iup.h"

#include "iup_assert.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_strmessage.h"

/* from iup_open, but it is not exported, used only here */
int iupIsOpened(void);

void iupError(const char* format, ...)
{
  int size;
  char* msg = iupStrGetLargeMem(&size);
  va_list arglist;
  va_start(arglist, format);
  vsnprintf(msg, size, format, arglist);
  va_end(arglist);
#if IUP_ASSERT_CONSOLE 
  fprintf(stderr, msg);
#else
  if (iupIsOpened())
    iupStrMessageShowError(NULL, iupStrGetMemoryCopy(msg));
  else
    fprintf(stderr, msg);
#endif
}

void iupAssert(const char* expr, const char* file, int line, const char* func)
{
  if (func)
    iupError("File: %s\n"
             "Line: %d\n"
             "Function: %s\n"
             "Assertive: (%s)", 
             file, line, func, expr);
  else
    iupError("File: %s\n"
             "Line: %d\n"
             "Assertive: (%s)", 
             file, line, expr);
}
