/** \file
 * \brief miscelaneous functions
 *
 * See Copyright Notice in "iup.h"
 */

/*! \mainpage IUP
 *
 * \section intro Introduction
 *
 * Internal SDK documentation of the IUP library, automatically generated using Doxygen (<A HREF="http://www.doxygen.org/">http://www.doxygen.org/</A>).
 *
 * \section codestd Code Standards
 *
 * \subsection func Function Names (prefix format)
 *  - IupFunc - User API, implemented in the core
 *  - iupFunc - Internal Core API, implemented in the core, used in the core or in driver
 *  - iupxxxFunc - Windows Internal API, implemented in driver xxx, used in driver xxx
 *  - iupdrvFunc - Driver API, implemented in driver, used in the core or driver
 *  - xxxFunc - Driver xxx local functions
 * 
 * \subsection glob Globais Variables (lower case format)
 *  - iupxxx_var
 *                    
 * \subsection loc Local Variables (lower case format, using module name)
 *  - iyyy_var
 *                    
 * \subsection fil File Names
 *  - iupyyy.h - public headers
 *  - iup_yyy.h/c - core
 *  - iupxxx_yyy.h/c - driver
 *   
 * \subsection strc Structures
 *  - Iyyy
 *   
 * \subsection com File Comments (at start)
 *  - Check an existant file for example.
 *    
 * \subsection def Defines
 *  - __IUPXXX_H   (for include file, same file name, upper case, "__" preffix and replace "." by "_")
 *  - IUP_XXX      (for global enumerations)
 *  - IXXX_YYY     (for local enumerations)
 *  - iupXXX       (for macros, complement with Function Names rules)
 *   
 * \subsection doc Documentation
 *  - In the header, using Doxygen commands.
 *  - Check an existant header for example.
 *
 */

/** \defgroup util Utilities
 */

/** \defgroup cpi Control SDK
 * \par
 * <H3><A HREF="../en/cpi.html">Control Creation Guide</A></H3>
 */

#include <stdlib.h>

#include "iup.h"

/* This appears only here to avoid changing the iup.h header fo bug fixes */
#define IUP_VERSION_FIX ""
#define IUP_VERSION_FIX_NUMBER 0
/*#define IUP_VERSION_FIX_DATE "" */

const char iup_ident[] = 
  "$IUP: " IUP_VERSION IUP_VERSION_FIX " " IUP_COPYRIGHT " $\n"
  "$URL: www.tecgraf.puc-rio.br/iup $\n";

/* Using this, if you look for the string TECVER, you will find also the library version. */
const char *iup_tecver = "TECVERID.str:Iup:LIB:" IUP_VERSION IUP_VERSION_FIX;

char* IupVersion(void)
{
  (void)iup_tecver;
  (void)iup_ident;
  return IUP_VERSION IUP_VERSION_FIX;
}

char* IupVersionDate(void)
{
#ifdef IUP_VERSION_FIX_DATE
  return IUP_VERSION_FIX_DATE;
#else
  return IUP_VERSION_DATE;
#endif
}
 
int IupVersionNumber(void)
{
  return IUP_VERSION_NUMBER+IUP_VERSION_FIX_NUMBER;
}
