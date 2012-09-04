/** \file
 * \brief Keyboard Focus navigation
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_FOCUS_H 
#define __IUP_FOCUS_H

#ifdef __cplusplus
extern "C" {
#endif


/** \defgroup focus Keyboard Focus
 * \par
 * See \ref iup_focus.h
 * \ingroup cpi */


/** Utility to check if a control can have the keyboard input focus.
 * To receive the focus must be interactive, has CANFOCUS=YES, is mapped, is visible and is active.
 * \ingroup focus */
int iupFocusCanAccept(Ihandle *ih);

/** Call GETFOCUS_CB and FOCUS_CB.
 * \ingroup focus */
void iupCallGetFocusCb(Ihandle *ih);

/** Call KILLFOCUS_CB and FOCUS_CB.
 * \ingroup focus */
void iupCallKillFocusCb(Ihandle *ih);

/** Returns the next interactive brother. Independs if it can receive the focus.
 * \ingroup focus */
Ihandle* iupFocusNextInteractive(Ihandle *ih);

/* Used only in iupKeyProcessNavigation */
void iupFocusNext(Ihandle *ih);
void iupFocusPrevious(Ihandle *ih);

void iupSetCurrentFocus(Ihandle *ih);

/* Other functions declared in <iup.h> and implemented here. 
IupPreviousField
IupNextField
*/


#ifdef __cplusplus
}
#endif

#endif
