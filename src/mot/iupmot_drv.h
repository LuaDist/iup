/** \file
 * \brief Motif Driver 
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPMOT_DRV_H 
#define __IUPMOT_DRV_H

#ifdef __cplusplus
extern "C" {
#endif


/* global variables, declared in iupmot_open.c */
extern Widget         iupmot_appshell;         
extern Display*       iupmot_display;          
extern int            iupmot_screen;           
extern XtAppContext   iupmot_appcontext;       
extern Visual*        iupmot_visual;
extern Atom           iupmot_wm_deletewindow;

/* dialog */
void iupmotDialogSetVisual(Ihandle* ih, void* visual);
void iupmotDialogResetVisual(Ihandle* ih);

/* focus */
void iupmotFocusChangeEvent(Widget w, Ihandle *ih, XEvent *evt, Boolean *cont);

/* key */
void iupmotCanvasKeyReleaseEvent(Widget w, Ihandle *ih, XEvent *evt, Boolean *cont);
void iupmotKeyPressEvent(Widget w, Ihandle *ih, XEvent *evt, Boolean *cont);
KeySym iupmotKeyCharToKeySym(char c);
void iupmotButtonKeySetStatus(unsigned int state, unsigned int but, char* status, int doubleclick);
int iupmotKeyDecode(XKeyEvent *evt);

/* font */
char* iupmotGetFontListAttrib(Ihandle *ih);
XmFontList iupmotGetFontList(const char* foundry, const char* value);
XFontStruct* iupmotGetFontStruct(const char* value);
char* iupmotFindFontList(XmFontList fontlist);
char* iupmotGetFontStructAttrib(Ihandle *ih);
char* iupmotGetFontIdAttrib(Ihandle *ih);

/* tips */
/* called from Enter/Leave events to check if a TIP is present. */
void iupmotTipEnterNotify(Ihandle* ih);
void iupmotTipLeaveNotify(void);
void iupmotTipsFinish(void);

/* common */
void iupmotPointerMotionEvent(Widget w, Ihandle *ih, XEvent *evt, Boolean *cont);
void iupmotDummyPointerMotionEvent(Widget w, XtPointer *data, XEvent *evt, Boolean *cont);
void iupmotButtonPressReleaseEvent(Widget w, Ihandle* ih, XEvent* evt, Boolean* cont);
void iupmotEnterLeaveWindowEvent(Widget w, Ihandle *ih, XEvent *evt, Boolean *cont);
void iupmotHelpCallback(Widget w, Ihandle *ih, XtPointer call_data);
void iupmotSetString(Widget w, const char *resource, const char* value);
char* iupmotConvertString(XmString str);
void iupmotSetMnemonicTitle(Ihandle *ih, Widget w, const char* value);
void iupmotDisableDragSource(Widget w);
void iupmotSetPixmap(Ihandle* ih, const char* name, const char* prop, int make_inactive);
void iupmotSetGlobalColorAttrib(Widget w, const char* xmname, const char* name);
void iupmotSetBgColor(Widget w, Pixel color);
char* iupmotGetBgColorAttrib(Ihandle* ih);

void iupmotGetWindowSize(Ihandle *ih, int *width, int *height);

char* iupmotGetXWindowAttrib(Ihandle *ih);

#define iupMOT_SETARG(_a, _i, _n, _d) ((_a)[(_i)].name = (_n), (_a)[(_i)].value = (XtArgVal)(_d), (_i)++)


#ifdef __cplusplus
}
#endif

#endif
