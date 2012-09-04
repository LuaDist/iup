/** \file
 * \brief Windows Driver
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPWIN_DRV_H 
#define __IUPWIN_DRV_H

#ifdef __cplusplus
extern "C" {
#endif


/* global variables */
/* declared where they are initialized */
extern HINSTANCE iupwin_hinstance;      /* winopen.c */
extern HINSTANCE iupwin_dll_hinstance;  /* winmain.c */
extern int       iupwin_comctl32ver6;   /* winopen.c */

void iupwinShowLastError(void);

/* focus */
void iupwinWmSetFocus(Ihandle *ih);

/* key */
int iupwinKeyEvent(Ihandle* ih, int wincode, int press);
void iupwinButtonKeySetStatus(WORD keys, char* status, int doubleclick);
int iupwinKeyDecode(int wincode);

/* tips */
void iupwinTipsGetDispInfo(LPARAM lp);
void iupwinTipsUpdateInfo(Ihandle* ih, HWND tips_hwnd);
void iupwinTipsDestroy(Ihandle* ih);

/* touch */
void iupwinTouchInit(void);
void iupwinTouchRegisterAttrib(Iclass* ic);
void iupwinTouchProcessInput(Ihandle* ih, int count, void* lp);

/* font */
char* iupwinGetHFontAttrib(Ihandle *ih);
HFONT iupwinGetHFont(const char* value);
char* iupwinFindHFont(HFONT hFont);

/* DnD */
int iupwinDragStart(Ihandle* ih);
void iupwinDropFiles(HDROP hDrop, Ihandle *ih);
void iupwinDestroyDragDrop(Ihandle* ih);

/* menu */
void iupwinMenuDialogProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp);
Ihandle* iupwinMenuGetItemHandle(HMENU hmenu, int menuId);
Ihandle* iupwinMenuGetHandle(HMENU hMenu);

/* common */

/* Definition of a callback used to return the background brush of controls called "_IUPWIN_CTLCOLOR_CB". */
typedef int (*IFctlColor)(Ihandle* ih, HDC hdc, LRESULT *result);

/* Definition of a callback used to draw custom controls called "_IUPWIN_DRAWITEM_CB". 
  drawitem is a pointer to a DRAWITEMSTRUCT struct. */
typedef void (*IFdrawItem)(Ihandle* ih, void* drawitem);

/* Definition of a callback used to notify custom controls called "_IUPWIN_NOTIFY_CB". 
  msg_info is a pointer to a NMHDR struct. */
typedef int (*IFnotify)(Ihandle* ih, void* msg_info, int *result);

/* Definition of a callback used to process WM_COMMAND messages called "_IUPWIN_COMMAND_CB". */
typedef int (*IFwmCommand)(Ihandle* ih, WPARAM wp, LPARAM lp);

/* Definition of callback used for custom WinProc. Can return 0 or 1.
   0 = do default processing. 
   1 = ABORT default processing and the result value should be returned.
*/
typedef int (*IwinProc)(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result);

/* Base WinProc used by all native elements. Configure base message handling 
   and custom IwinProc using "_IUPWIN_CTRLPROC_CB" callback. */
LRESULT CALLBACK iupwinBaseWinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

/* Base IwinProc callback used by native controls. */
int iupwinBaseProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result);

/* Base IwinProc callback used by native containers. 
   Handle messages that are sent to the parent Window.  */
int iupwinBaseContainerProc(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp, LRESULT *result);

void iupwinChangeProc(Ihandle *ih, WNDPROC new_proc);

/* Creates the Window with native parent and child ID, associate HWND with Ihandle*, 
   and replace the WinProc by iupwinBaseWinProc */
int iupwinCreateWindowEx(Ihandle* ih, LPCSTR lpClassName, DWORD dwExStyle, DWORD dwStyle);

void iupwinGetNativeParentStyle(Ihandle* ih, DWORD *dwExStyle, DWORD *dwStyle);
void iupwinMergeStyle(Ihandle* ih, DWORD old_mask, DWORD value);
void iupwinSetStyle(Ihandle* ih, DWORD value, int set);

int iupwinClassExist(const char* name);

int iupwinGetColorRef(Ihandle *ih, char *name, COLORREF *color);
int iupwinGetParentBgColor(Ihandle* ih, COLORREF* cr);

WCHAR* iupwinStrChar2Wide(const char* str);
char* iupwinStrWide2Char(const WCHAR* wstr);

int iupwinButtonUp(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp);
int iupwinButtonDown(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp);
int iupwinMouseMove(Ihandle* ih, UINT msg, WPARAM wp, LPARAM lp);

char* iupwinGetClipboardText(Ihandle* ih);
int iupwinSetAutoRedrawAttrib(Ihandle* ih, const char* value);

int iupwinGetScreenRes(void);
/* 1 point = 1/72 inch */
/* pixel = (point/72)*(pixel/inch) */
#define iupWIN_PT2PIXEL(_pt, _res)    MulDiv(_pt, _res, 72)     /* (((_pt)*(_res))/72)    */
#define iupWIN_PIXEL2PT(_pixel, _res) MulDiv(_pixel, 72, _res)  /* (((_pixel)*72)/(_res)) */


/* child window identifier of the first MDI child window created,
   should not conflict with any other command identifiers. */
#define IUP_MDI_FIRSTCHILD 100000000


#ifdef __cplusplus
}
#endif

#endif
