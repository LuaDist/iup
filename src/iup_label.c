/** \file
 * \brief Label Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_label.h"
#include "iup_image.h"


static int iLabelSetSeparatorAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (value)
  {
    if (iupStrEqualNoCase(value, "HORIZONTAL"))
      ih->expand = IUP_EXPAND_WIDTH;
    else if (iupStrEqualNoCase(value, "VERTICAL"))
      ih->expand = IUP_EXPAND_HEIGHT;
    else
      return 0;
  }

  return 1;
}

static char* iLabelGetSeparatorAttrib(Ihandle* ih)
{
  if (ih->handle)
  {
    if (ih->data->type == IUP_LABEL_SEP_HORIZ)
      return "HORIZONTAL";
    else if (ih->data->type == IUP_LABEL_SEP_VERT)
      return "VERTICAL";
  }
  return NULL;
}

char* iupLabelGetPaddingAttrib(Ihandle* ih)
{
  if (ih->data->type != IUP_LABEL_SEP_HORIZ && ih->data->type != IUP_LABEL_SEP_VERT)
  {
    char *str = iupStrGetMemory(50);
    sprintf(str, "%dx%d", ih->data->horiz_padding, ih->data->vert_padding);
    return str;
  }
  else
    return NULL;
}


/**************************************************************************************/


static int iLabelCreateMethod(Ihandle* ih, void** params)
{
  if (params && params[0])
    iupAttribStoreStr(ih, "TITLE", (char*)(params[0]));
  
  ih->data = iupALLOCCTRLDATA();

  /* used only by the Windows driver */
  ih->data->vert_alignment = IUP_ALIGN_ACENTER;
  return IUP_NOERROR;
}

static void iLabelComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int natural_w = 0, 
      natural_h = 0, 
      type = ih->data->type;
  (void)expand; /* unset if not a container */

  if (!ih->handle)
  {
    /* if not mapped must initialize the internal values */
    char* value = iupAttribGet(ih, "SEPARATOR");
    if (value)
    {
      if (iupStrEqualNoCase(value, "HORIZONTAL"))
        type = IUP_LABEL_SEP_HORIZ;
      else /* "VERTICAL" */
        type = IUP_LABEL_SEP_VERT;
    }
    else
    {
      value = iupAttribGet(ih, "IMAGE");
      if (value)
        type = IUP_LABEL_IMAGE;
      else
        type = IUP_LABEL_TEXT;
    }
  }

  if (type == IUP_LABEL_SEP_HORIZ)
    natural_h = 2;
  else if (type == IUP_LABEL_SEP_VERT)
    natural_w = 2;
  else if (type == IUP_LABEL_IMAGE)
  {
    iupImageGetInfo(iupAttribGet(ih, "IMAGE"), &natural_w, &natural_h, NULL);

    natural_w += 2*ih->data->horiz_padding;
    natural_h += 2*ih->data->vert_padding;
  }
  else /* IUP_LABEL_TEXT */
  {
    /* must use IupGetAttribute to check from the native implementation */
    char* title = IupGetAttribute(ih, "TITLE");
    char* str = iupStrProcessMnemonic(title, NULL, 0);   /* remove & */
    iupdrvFontGetMultiLineStringSize(ih, str, &natural_w, &natural_h);
    if (str && str!=title) free(str);

    natural_w += 2*ih->data->horiz_padding;
    natural_h += 2*ih->data->vert_padding;
  }

  *w = natural_w;
  *h = natural_h;
}


/******************************************************************************/


Ihandle* IupLabel(const char* title)
{
  void *params[2];
  params[0] = (void*)title;
  params[1] = NULL;
  return IupCreatev("label", params);
}

Iclass* iupLabelNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "label";
  ic->format = "s"; /* one string */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupLabelNewClass;
  ic->Create = iLabelCreateMethod;
  ic->ComputeNaturalSize = iLabelComputeNaturalSizeMethod;

  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Manually register the Common Callbacks */
  iupClassRegisterCallback(ic, "MAP_CB", "");
  iupClassRegisterCallback(ic, "UNMAP_CB", "");

  iupClassRegisterCallback(ic, "DROPFILES_CB", "siii");
  iupClassRegisterCallback(ic, "BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "ENTERWINDOW_CB", "");
  iupClassRegisterCallback(ic, "LEAVEWINDOW_CB", "");

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* IupLabel only */
  iupClassRegisterAttribute(ic, "SEPARATOR", iLabelGetSeparatorAttrib, iLabelSetSeparatorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupdrvLabelInitClass(ic);

  return ic;
}
