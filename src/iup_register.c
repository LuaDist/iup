/** \file
* \brief Register the Internal Controls
*
* See Copyright Notice in "iup.h"
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_register.h"
#include "iup_stdcontrols.h"


static Itable *iregister_table = NULL;   /* table indexed by name containing Iclass* address */

void iupRegisterInit(void)
{
  iregister_table = iupTableCreate(IUPTABLE_STRINGINDEXED);
}

void iupRegisterFinish(void)
{
  char* name = iupTableFirst(iregister_table);
  while (name)
  {
    Iclass* ic = (Iclass*)iupTableGetCurr(iregister_table);
    iupClassRelease(ic);
    name = iupTableNext(iregister_table);
  }

  iupTableDestroy(iregister_table);
  iregister_table = NULL;
}

int IupGetAllClasses(char** list, int n)
{
  int i = 0;
  char* name = iupTableFirst(iregister_table);

  if (!list || !n)
    return iupTableCount(iregister_table);

  while (name)
  {
    list[i] = name;
    i++;
    if (i == n)
      break;

    name = iupTableNext(iregister_table);
  }

  return i;
}

Iclass* iupRegisterFindClass(const char* name)
{
  return (Iclass*)iupTableGet(iregister_table, name);
}

void iupRegisterClass(Iclass* ic)
{
  Iclass* old_ic = (Iclass*)iupTableGet(iregister_table, ic->name);
  if (old_ic)
    iupClassRelease(old_ic);

  iupTableSet(iregister_table, ic->name, (void*)ic, IUPTABLE_POINTER);
}


/***************************************************************/

void iupRegisterInternalClasses(void)
{
  iupRegisterClass(iupDialogNewClass());
  iupRegisterClass(iupMessageDlgNewClass());
  iupRegisterClass(iupColorDlgNewClass());
  iupRegisterClass(iupFontDlgNewClass());
  iupRegisterClass(iupFileDlgNewClass());

  iupRegisterClass(iupTimerNewClass());
  iupRegisterClass(iupImageNewClass());
  iupRegisterClass(iupImageRGBNewClass());
  iupRegisterClass(iupImageRGBANewClass());
  iupRegisterClass(iupUserNewClass());
  iupRegisterClass(iupClipboardNewClass());

  iupRegisterClass(iupRadioNewClass());
  iupRegisterClass(iupFillNewClass());
  iupRegisterClass(iupHboxNewClass());
  iupRegisterClass(iupVboxNewClass());
  iupRegisterClass(iupZboxNewClass());
  iupRegisterClass(iupCboxNewClass());
  iupRegisterClass(iupSboxNewClass());
  iupRegisterClass(iupNormalizerNewClass());
  iupRegisterClass(iupSplitNewClass());

  iupRegisterClass(iupMenuNewClass());
  iupRegisterClass(iupItemNewClass());
  iupRegisterClass(iupSeparatorNewClass());
  iupRegisterClass(iupSubmenuNewClass());

  iupRegisterClass(iupLabelNewClass());
  iupRegisterClass(iupButtonNewClass());
  iupRegisterClass(iupToggleNewClass());
  iupRegisterClass(iupCanvasNewClass());
  iupRegisterClass(iupFrameNewClass());
  iupRegisterClass(iupTextNewClass());
  iupRegisterClass(iupMultilineNewClass());
  iupRegisterClass(iupListNewClass());

  iupRegisterClass(iupProgressBarNewClass());
  iupRegisterClass(iupValNewClass());
  iupRegisterClass(iupTabsNewClass());
  iupRegisterClass(iupSpinNewClass());
  iupRegisterClass(iupSpinboxNewClass());
  iupRegisterClass(iupTreeNewClass());
}
