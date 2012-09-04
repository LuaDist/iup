/** \file
 * \brief Timer Resource Private Declarations
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_TIMER_H 
#define __IUP_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif


void iupdrvTimerStop(Ihandle* ih);
void iupdrvTimerRun(Ihandle* ih);
void iupdrvTimerInitClass(Iclass* ic);


#ifdef __cplusplus
}
#endif

#endif
