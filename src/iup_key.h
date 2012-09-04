/** \file
 * \brief Manage keys encoding and decoding.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_KEY_H 
#define __IUP_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup key Key Coding and Key Callbacks
 * \par
 * See \ref iup_key.h
 * \ingroup cpi */


/** Returns the key name from its code. 
 * Returns NULL if code not found.
 * \ingroup key */
char *iupKeyCodeToName(int code);

/** Returns the key code from its name. 
 * Returns 0 if name not found.
 * \ingroup key */
int iupKeyNameToCode(const char *name);

/** Returns true if the key code can be changed by CAPSLOCK.
 * \ingroup key */
int iupKeyCanCaps(int code);

/** Calls a function for each defined key. 
 * \ingroup key */
void iupKeyForEach(void (*func)(const char *name, int code, void* user_data), void* user_data);

/** Calls the K_ANY or K_* callbacks. Should be called when a keyboard event occoured.
 * \ingroup key */
int iupKeyCallKeyCb(Ihandle *ih, int c);

/** Calls the KEYPRESS_CB callback. Should be called when a keyboard event occoured.
 * \ingroup key */
int iupKeyCallKeyPressCb(Ihandle *ih, int code, int press);

/** Process Tab, DEFAULTENTER and DEFAULTESC in key press events.
 * \ingroup key */
int iupKeyProcessNavigation(Ihandle* ih, int key, int shift);
                             
                             
/* Called only from IupOpen. */
void iupKeyInit(void);
                        
                        
#define IUPKEY_STATUS_SIZE 11 /* 10 chars + null */
#define IUPKEY_STATUS_INIT "          "  /* 10 spaces */
#define iupKEY_SETSHIFT(_s)    (_s[0]='S')
#define iupKEY_SETCONTROL(_s)  (_s[1]='C')
#define iupKEY_SETBUTTON1(_s)  (_s[2]='1')
#define iupKEY_SETBUTTON2(_s)  (_s[3]='2')
#define iupKEY_SETBUTTON3(_s)  (_s[4]='3')
#define iupKEY_SETDOUBLE(_s)   (_s[5]='D')
#define iupKEY_SETALT(_s)      (_s[6]='A')
#define iupKEY_SETSYS(_s)      (_s[7]='Y')
#define iupKEY_SETBUTTON4(_s)  (_s[8]='4')
#define iupKEY_SETBUTTON5(_s)  (_s[9]='5')


#ifdef __cplusplus
}
#endif

#endif
