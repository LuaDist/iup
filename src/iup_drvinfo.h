/** \file
 * \brief Driver Information Functions
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_DRVINFO_H 
#define __IUP_DRVINFO_H

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup drvinfo Driver Information Interface 
 * \par
 * Each driver must export the symbols defined here. 
 * But in this case the functions are shared by different drivers in the same system.
 * \par
 * For example, the GTK driver and the Windows driver share the same implementation 
 * of these functions when the GTK driver is compiled in Windows. 
 * The GTK driver and the Motif driver share the same implementation 
 * of these functions when the GTK driver is compiled in UNIX. 
 * \par
 * See \ref iup_drvinfo.h 
 * \ingroup drv */


/** Retrieve the main desktop full size.
 * \ingroup drvinfo */
void iupdrvGetFullSize(int *width, int *height);

/** Retrieve the main desktop available size.
 * \ingroup drvinfo */
void iupdrvGetScreenSize(int *width, int *height);

/** Retrieve the main desktop size when there are multiple monitors.
 * Useful only when in GTK.
 * \ingroup drvinfo */
int iupdrvCheckMainScreen(int *width, int *height);

/** Retrieve the default desktop bits per pixel.
 * \ingroup drvinfo */
int iupdrvGetScreenDepth(void);

/** Retrieve the default desktop resolution in dpi (dots or pixels per inch).
 * \ingroup drvinfo */
float iupdrvGetScreenDpi(void);

/** Returns a string with the system version number.
 * \ingroup drvinfo */
char *iupdrvGetSystemVersion(void);

/** Returns a string with the system name.
 * \ingroup drvinfo */
char* iupdrvGetSystemName(void);

/** Returns a string with the computer name.
 * \ingroup drvinfo */
char* iupdrvGetComputerName(void);

/** Returns a string with the user name.
 * \ingroup drvinfo */
char* iupdrvGetUserName(void);

/** Returns the key state for Shift, Ctrl, Alt and sYs, in this order.
 * Left and right keys are considered.
 * Should declare "char key[5]".
 * Values could be space (" ") or "SCAY".
 * \ingroup drvinfo */
void iupdrvGetKeyState(char* key);

/** Returns the current position of the mouse cursor.
 * \ingroup drvinfo */
void iupdrvGetCursorPos(int *x, int *y);

/** Returns the driver "Display" in UNIX and NULL in Windows.
 * Must be implemented somewhere else.
 * \ingroup drvinfo */
void* iupdrvGetDisplay(void);

/** Returns the current directory.
 * \ingroup drvinfo */
char* iupdrvGetCurrentDirectory(void);

/** Changes the current directory.
 * \ingroup drvinfo */
int iupdrvSetCurrentDirectory(const char* dir);

/** Returns true if the given name is an existant file.
 * \ingroup drvinfo */
int iupdrvIsFile(const char* name);

/** Returns true if the given name is an existant directory.
 * \ingroup drvinfo */
int iupdrvIsDirectory(const char* name);

/** Creates a new direcotry.
 * \ingroup drvinfo */
int iupdrvMakeDirectory(const char* name);

/** Returns the current locale name.
 * \ingroup drvinfo */
char* iupdrvLocaleInfo(void);


#ifdef __cplusplus
}
#endif

#endif
