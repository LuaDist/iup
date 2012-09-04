/** \file
 * \brief Windows System Information
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPWIN_INFO_H 
#define __IUPWIN_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

/* system */
int   iupwinGetSystemMajorVersion(void);
int   iupwinGetComCtl32Version(void);
char* iupwinGetSystemLanguage(void);
int   iupwinIsAppThemed(void);
int   iupwinIsVistaOrNew(void);
int   iupwinIs7OrNew(void);

/* color */
void iupwinGetSysColor(char* color, int wincolor);

#ifdef __cplusplus
}
#endif

#endif
