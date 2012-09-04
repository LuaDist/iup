/** \file
 * \brief Abstract Layout Management
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_LAYOUT_H 
#define __IUP_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

/* called from IupMap and IupRefresh */
void iupLayoutCompute(Ihandle* ih);  /* can be called before map */
void iupLayoutUpdate(Ihandle* ih);   /* called only after map */

void iupLayoutSetMinMaxSize(Ihandle* ih, int *w, int *h);

/* Other functions declared in <iup.h> and implemented here. 
IupRefresh
*/


#ifdef __cplusplus
}
#endif

#endif
