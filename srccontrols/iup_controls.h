/** \file
 * \brief Additional Controls Class Initialization functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_CONTROLS_H 
#define __IUP_CONTROLS_H


#ifdef __cplusplus
extern "C" {
#endif


Iclass* iupDialNewClass(void);
Iclass* iupCellsNewClass(void);
Iclass* iupColorbarNewClass(void);
Iclass* iupColorBrowserNewClass(void);
Iclass* iupMatrixNewClass(void);
Iclass* iupGaugeNewClass(void);
Iclass* iupTabsNewClass(void);
Iclass* iupColorBrowserDlgNewClass(void);

char *iupControlBaseGetParentBgColor (Ihandle* ih);
char *iupControlBaseGetBgColorAttrib(Ihandle* ih);


#ifdef __cplusplus
}
#endif

#endif
