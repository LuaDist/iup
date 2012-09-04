/** \file
 * \brief Tabs Control
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_TABS_H 
#define __IUP_TABS_H

#ifdef __cplusplus
extern "C" {
#endif


char* iupTabsGetTabOrientationAttrib(Ihandle* ih);
char* iupTabsGetTabTypeAttrib(Ihandle* ih);
char* iupTabsAttribGetStrId(Ihandle* ih, const char* name, int pos);
void iupTabsAttribSetStrId(Ihandle* ih, const char* name, int pos, const char* value);
char* iupTabsGetPaddingAttrib(Ihandle* ih);
void iupTabsTestRemoveTab(Ihandle* ih, int pos);

int iupdrvTabsExtraDecor(Ihandle* ih);
int iupdrvTabsGetLineCountAttrib(Ihandle* ih);
void iupdrvTabsSetCurrentTab(Ihandle* ih, int pos);
int iupdrvTabsGetCurrentTab(Ihandle* ih);
void iupdrvTabsInitClass(Iclass* ic);

typedef enum
{
  ITABS_TOP, ITABS_BOTTOM, ITABS_LEFT, ITABS_RIGHT
} ItabsType;

typedef enum
{
  ITABS_HORIZONTAL, ITABS_VERTICAL
} ItabsOrientation;

/* Control context */
struct _IcontrolData
{
  ItabsType type;
  ItabsOrientation orientation;
  int horiz_padding, vert_padding;  /* tab title margin */
  int is_multiline; /* used only in Windows */
};


#ifdef __cplusplus
}
#endif

#endif
