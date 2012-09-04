/** \file
 * \brief Global Function table.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_FUNC_H 
#define __IUP_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

/* called only in IupOpen and IupClose */
void iupFuncInit(void);
void iupFuncFinish(void);

char* iupGetCallbackName(Ihandle *ih, const char *name);

/* Other functions declared in <iup.h> and implemented here. 
IupGetActionName
IupGetFunction
IupSetFunction
*/

#ifdef __cplusplus
}
#endif

#endif
