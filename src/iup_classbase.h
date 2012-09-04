/** \file
 * \brief Base Class
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_CLASSBASE_H 
#define __IUP_CLASSBASE_H

#ifdef __cplusplus
extern "C" {
#endif


/** \defgroup iclassbase Base Class
 * \par
 * See \ref iup_classbase.h
 * \ingroup iclass
 */


/** Register all common base attributes: \n
 * WID                                   \n
 * SIZE, RASTERSIZE, POSITION            \n
 * FONT (and derived)                    \n\n
 * All controls that are positioned inside a dialog must register all common base attributes.
 * \ingroup iclassbase */
void iupBaseRegisterCommonAttrib(Iclass* ic);

/** Register all visual base attributes: \n
 * VISIBLE, ACTIVE                       \n
 * ZORDER, X, Y                          \n
 * TIP (and derived)                     \n\n
 * All controls that are positioned inside a dialog must register all visual base attributes.
 * \ingroup iclassbase */
void iupBaseRegisterVisualAttrib(Iclass* ic);

/** Register all common callbacks: \n
* MAP_CB, UNMAP_CB, GETFOCUS_CB, KILLFOCUS_CB, ENTERWINDOW_CB, LEAVEWINDOW_CB, K_ANY, HELP_CB.
* \ingroup iclassbase */
void iupBaseRegisterCommonCallbacks(Iclass* ic);

/* Register driver dependent common attributes. 
   Used only from iupBaseRegisterCommonAttrib */
void iupdrvBaseRegisterCommonAttrib(Iclass* ic);

/** Updates the expand member of the IUP object from the EXPAND attribute.
 * Should be called in the beginning of the ComputeNaturalSize for a container.
 * \ingroup iclassbase */
void iupBaseContainerUpdateExpand(Ihandle* ih);

/** Initializes the natural size using the user size, then
 * if a container then update the "expand" member from the EXPAND attribute, then
 * call \ref iupClassObjectComputeNaturalSize for containers if they have children or
 * call \ref iupClassObjectComputeNaturalSize for non-containers if user size is not defined.
 * Must be called for each children in the container. \n
 * First call is in iupLayoutCompute.
 * \ingroup iclassbase */
void iupBaseComputeNaturalSize(Ihandle* ih);

/** Update the current size from the available size, the natural size, expand and shrink.
 * Call \ref iupClassObjectSetChildrenCurrentSize for containers if they have children.
 * Must be called for each children in the container. \n
 * First call is in iupLayoutCompute.
 * \ingroup iclassbase */
void iupBaseSetCurrentSize(Ihandle* ih, int w, int h, int shrink);

/** Set the current position and update children position for containers.
 * Call \ref iupClassObjectSetChildrenPosition for containers if they have children.
 * Must be called for each children in the container. \n
 * First call is in iupLayoutCompute.
 * \ingroup iclassbase */
void iupBaseSetPosition(Ihandle* ih, int x, int y);

/* Updates the SIZE attribute if defined. 
   Called only from iupdrvSetStandardFontAttrib. */
void iupBaseUpdateSizeFromFont(Ihandle* ih);


/** \defgroup iclassbasemethod Base Class Methods
 * \par
 * See \ref iup_classbase.h
 * \ingroup iclassbase
 */

/** Driver dependent \ref Iclass::LayoutUpdate method.
 * \ingroup iclassbasemethod */
void iupdrvBaseLayoutUpdateMethod(Ihandle *ih);

/** Driver dependent \ref Iclass::UnMap method.
 * \ingroup iclassbasemethod */
void iupdrvBaseUnMapMethod(Ihandle* ih);

/** Native type void \ref Iclass::Map method.
 * \ingroup iclassbasemethod */
int iupBaseTypeVoidMapMethod(Ihandle* ih);


/** \defgroup iclassbaseattribfunc Base Class Attribute Functions
 * \par
 * Used by the controls for iupClassRegisterAttribute. 
 * \par
 * See \ref iup_classbase.h
 * \ingroup iclassbase
 * @{
 */

/* common */
char* iupBaseGetWidAttrib(Ihandle* ih);
int iupBaseSetNameAttrib(Ihandle* ih, const char* value);
int iupBaseSetRasterSizeAttrib(Ihandle* ih, const char* value);
int iupBaseSetSizeAttrib(Ihandle* ih, const char* value);
char* iupBaseGetSizeAttrib(Ihandle* ih);
char* iupBaseGetRasterSizeAttrib(Ihandle* ih);
char* iupBaseGetClientOffsetAttrib(Ihandle* ih);
int iupBaseSetMaxSizeAttrib(Ihandle* ih, const char* value);
int iupBaseSetMinSizeAttrib(Ihandle* ih, const char* value);

/* visual */
char* iupBaseGetVisibleAttrib(Ihandle* ih);
int iupBaseSetVisibleAttrib(Ihandle* ih, const char* value);
char* iupBaseGetActiveAttrib(Ihandle *ih);
int iupBaseSetActiveAttrib(Ihandle* ih, const char* value);
int iupdrvBaseSetZorderAttrib(Ihandle* ih, const char* value);
int iupdrvBaseSetTipAttrib(Ihandle* ih, const char* value);
int iupdrvBaseSetTipVisibleAttrib(Ihandle* ih, const char* value);
char* iupdrvBaseGetTipVisibleAttrib(Ihandle* ih);
int iupdrvBaseSetBgColorAttrib(Ihandle* ih, const char* value);
int iupdrvBaseSetFgColorAttrib(Ihandle* ih, const char* value);
char* iupBaseNativeParentGetBgColorAttrib(Ihandle* ih);

/* other */
char* iupBaseContainerGetExpandAttrib(Ihandle* ih);
int iupdrvBaseSetCursorAttrib(Ihandle* ih, const char* value);
int iupBaseNoSaveCheck(Ihandle* ih, const char* name);

/* drag&drop */
void iupdrvRegisterDragDropAttrib(Iclass* ic);

/* Windows Only */
char* iupdrvBaseGetTitleAttrib(Ihandle* ih);
int iupdrvBaseSetTitleAttrib(Ihandle* ih, const char* value);

/** @} */



/** \defgroup iclassbaseutil Base Class Utilities
 * \par
 * See \ref iup_classbase.h
 * \ingroup iclassbase
 * @{
 */

#define iupMAX(_a,_b) ((_a)>(_b)?(_a):(_b))
#define iupROUND(_x) ((int)((_x)>0? (_x)+0.5: (_x)-0.5))

#define iupCOLOR8TO16(_x) ((unsigned short)(_x*257))  /* 65535/255 = 257 */
#define iupCOLOR16TO8(_x) ((unsigned char)(_x/257))

#define iupBYTECROP(_x)   ((unsigned char)((_x)<0?0:((_x)>255)?255:(_x)))

enum{IUP_ALIGN_ALEFT, IUP_ALIGN_ACENTER, IUP_ALIGN_ARIGHT};
#define IUP_ALIGN_ABOTTOM IUP_ALIGN_ARIGHT
#define IUP_ALIGN_ATOP IUP_ALIGN_ALEFT

enum{IUP_SB_NONE, IUP_SB_HORIZ, IUP_SB_VERT};
int iupBaseGetScrollbar(Ihandle* ih);

char* iupBaseNativeParentGetBgColor(Ihandle* ih);
void iupBaseCallValueChangedCb(Ihandle* ih);

/** @} */


#ifdef __cplusplus
}
#endif

#endif
