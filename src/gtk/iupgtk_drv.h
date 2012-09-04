/** \file
 * \brief GTK Driver 
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPGTK_DRV_H 
#define __IUPGTK_DRV_H

#ifdef __cplusplus
extern "C" {
#endif


/* global variables, declared in iupgtk_globalattrib.c */
extern int iupgtk_utf8autoconvert;         
extern int iupgtk_globalmenu;


/* common */
gboolean iupgtkEnterLeaveEvent(GtkWidget *widget, GdkEventCrossing *evt, Ihandle* ih);
gboolean iupgtkShowHelp(GtkWidget *widget, GtkWidgetHelpType *arg1, Ihandle* ih);
GtkFixed* iupgtkBaseGetFixed(Ihandle* ih);
void iupgtkBaseAddToParent(Ihandle* ih);
void iupgdkColorSet(GdkColor* color, unsigned char r, unsigned char g, unsigned char b);
int iupgtkSetMnemonicTitle(Ihandle* ih, GtkLabel* label, const char* value);
char* iupgtkStrConvertToUTF8(const char* str);
char* iupgtkStrConvertFromUTF8(const char* str);
void iupgtkReleaseConvertUTF8(void);
char* iupgtkStrConvertFromFilename(const char* str);
char* iupgtkStrConvertToFilename(const char* str);
void iupgtkUpdateMnemonic(Ihandle* ih);
gboolean iupgtkMotionNotifyEvent(GtkWidget *widget, GdkEventMotion *evt, Ihandle *ih);
gboolean iupgtkButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih);
void iupgtkBaseSetBgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b);
void iupgtkBaseSetFgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b);
void iupgtkBaseSetFgGdkColor(InativeHandle* handle, GdkColor *color);
const char* iupgtkGetWidgetClassName(GtkWidget* widget);
GdkWindow* iupgtkGetWindow(GtkWidget *widget);


/* focus */
gboolean iupgtkFocusInOutEvent(GtkWidget *widget, GdkEventFocus *evt, Ihandle* ih);
void iupgtkSetCanFocus(GtkWidget *widget, int can);


/* key */
gboolean iupgtkKeyPressEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle* ih);
gboolean iupgtkKeyReleaseEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle* ih);
void iupgtkButtonKeySetStatus(guint state, unsigned int but, char* status, int doubleclick);
int iupgtkKeyDecode(GdkEventKey *evt);


/* font */
char* iupgtkGetPangoFontDescAttrib(Ihandle *ih);
char* iupgtkGetPangoLayoutAttrib(Ihandle *ih);
char* iupgtkGetFontIdAttrib(Ihandle *ih);
PangoFontDescription* iupgtkGetPangoFontDesc(const char* value);
PangoLayout* iupgtkGetPangoLayout(const char* value);
char* iupgtkFindPangoFontDesc(PangoFontDescription* fontdesc);
void iupgtkFontUpdatePangoLayout(Ihandle* ih, PangoLayout* layout);
void iupgtkFontUpdateObjectPangoLayout(Ihandle* ih, gpointer object);

/* There are PANGO_SCALE Pango units in one device unit. 
  For an output backend where a device unit is a pixel, 
  a size value of 10 * PANGO_SCALE gives 10 pixels. */
#define iupGTK_PANGOUNITS2PIXELS(_x) (((_x) + PANGO_SCALE/2) / PANGO_SCALE)
#define iupGTK_PIXELS2PANGOUNITS(_x) ((_x) * PANGO_SCALE)


/* open */
char* iupgtkGetNativeWindowHandle(Ihandle* ih);
void iupgtkPushVisualAndColormap(void* visual, void* colormap);
void* iupgtkGetNativeGraphicsContext(GtkWidget* widget);
void iupgtkReleaseNativeGraphicsContext(GtkWidget* widget, void* gc);
void iupgtkUpdateGlobalColors(GtkStyle* style);


/* dialog */
gboolean iupgtkDialogDeleteEvent(GtkWidget *widget, GdkEvent *evt, Ihandle *ih);


#ifdef __cplusplus
}
#endif

#endif
