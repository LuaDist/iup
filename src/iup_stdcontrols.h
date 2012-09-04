/** \file
 * \brief Standard Controls Class Initialization functions.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_STDCONTROLS_H 
#define __IUP_STDCONTROLS_H

#ifdef __cplusplus
extern "C" {
#endif


Iclass* iupDialogNewClass(void);
Iclass* iupMessageDlgNewClass(void);
Iclass* iupColorDlgNewClass(void);
Iclass* iupFontDlgNewClass(void);
Iclass* iupFileDlgNewClass(void);

Iclass* iupLabelNewClass(void);
Iclass* iupButtonNewClass(void);
Iclass* iupToggleNewClass(void);
Iclass* iupRadioNewClass(void);
Iclass* iupCanvasNewClass(void);
Iclass* iupFrameNewClass(void);
Iclass* iupProgressBarNewClass(void);
Iclass* iupTextNewClass(void);
Iclass* iupMultilineNewClass(void);
Iclass* iupValNewClass(void);
Iclass* iupTabsNewClass(void);
Iclass* iupSpinNewClass(void);
Iclass* iupSpinboxNewClass(void);
Iclass* iupListNewClass(void);
Iclass* iupTreeNewClass(void);

Iclass* iupMenuNewClass(void);
Iclass* iupItemNewClass(void);
Iclass* iupSeparatorNewClass(void);
Iclass* iupSubmenuNewClass(void);

Iclass* iupFillNewClass(void);
Iclass* iupHboxNewClass(void);
Iclass* iupVboxNewClass(void);
Iclass* iupZboxNewClass(void);
Iclass* iupCboxNewClass(void);
Iclass* iupSboxNewClass(void);
Iclass* iupNormalizerNewClass(void);
Iclass* iupSplitNewClass(void);

Iclass* iupTimerNewClass(void);
Iclass* iupImageNewClass(void);
Iclass* iupImageRGBNewClass(void);
Iclass* iupImageRGBANewClass(void);
Iclass* iupUserNewClass(void);
Iclass* iupClipboardNewClass(void);

/*************************************************/

void iupdrvMessageDlgInitClass(Iclass* ic);
void iupdrvColorDlgInitClass(Iclass* ic);
void iupdrvFontDlgInitClass(Iclass* ic);
void iupdrvFileDlgInitClass(Iclass* ic);

/************************************************/

/* Common definition of the canvas class */
typedef struct _iupCanvas {
  int sb;    /* scrollbar configuration, valid only after map, use iupBaseGetScrollbar before map */
  float posx, posy;
} iupCanvas;

#ifdef __cplusplus
}
#endif

#endif
