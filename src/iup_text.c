/** \file
 * \brief Text Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_register.h"
#include "iup_layout.h"
#include "iup_mask.h"
#include "iup_array.h"
#include "iup_text.h"
#include "iup_assert.h"


char* iupTextGetFormattingAttrib(Ihandle* ih)
{
  if (ih->data->has_formatting)
    return "YES";
  else
    return "NO";
}

int iupTextSetFormattingAttrib(Ihandle* ih, const char* value)
{
  if (ih->handle)  /* only before map */
    return 0;

  ih->data->has_formatting = iupStrBoolean(value);

  return 0;
}

static void iTextDestroyFormatTags(Ihandle* ih)
{
  /* called if the element was destroyed before it was mapped */
  int i, count = iupArrayCount(ih->data->formattags);
  Ihandle** tag_array = (Ihandle**)iupArrayGetData(ih->data->formattags);
  for (i = 0; i < count; i++)
    IupDestroy(tag_array[i]);
  iupArrayDestroy(ih->data->formattags);
  ih->data->formattags = NULL;
}

static void iTextUpdateValueAttrib(Ihandle* ih)
{
  char* value = iupAttribGet(ih, "VALUE");
  if (value)
  {
    int inherit;
    iupClassObjectSetAttribute(ih, "VALUE", value, &inherit);

    iupAttribSetStr(ih, "VALUE", NULL); /* clear hash table */
  }
}

char* iupTextGetNCAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(100);
  sprintf(str, "%d", ih->data->nc);
  return str;
}

void iupTextUpdateFormatTags(Ihandle* ih)
{
  /* called when the element is mapped */
  int i, count = iupArrayCount(ih->data->formattags);
  Ihandle** tag_array = (Ihandle**)iupArrayGetData(ih->data->formattags);

  /* must update VALUE before updating the format */
  iTextUpdateValueAttrib(ih);

  for (i = 0; i < count; i++)
  {
    char* bulk = iupAttribGet(tag_array[i], "BULK");
    if (bulk && iupStrBoolean(bulk))
    {
      Ihandle* child;
      void* state = iupdrvTextAddFormatTagStartBulk(ih);

      char* cleanout = iupAttribGet(tag_array[i], "CLEANOUT");
      if (cleanout && iupStrBoolean(cleanout))
        IupSetAttribute(ih, "REMOVEFORMATTING", "ALL");

      for (child = tag_array[i]->firstchild; child; child = child->brother)
        iupdrvTextAddFormatTag(ih, child, 1);

      iupdrvTextAddFormatTagStopBulk(ih, state);
    }
    else
      iupdrvTextAddFormatTag(ih, tag_array[i], 0);
    IupDestroy(tag_array[i]);
  }
  iupArrayDestroy(ih->data->formattags);
  ih->data->formattags = NULL;
}

int iupTextSetAddFormatTagHandleAttrib(Ihandle* ih, const char* value)
{
  Ihandle* formattag = (Ihandle*)value;
  if (!iupObjectCheck(formattag))
    return 0;

  if (ih->handle)
  {
    char* bulk;

    /* must update VALUE before updating the format */
    iTextUpdateValueAttrib(ih);

    bulk = iupAttribGet(formattag, "BULK");
    if (bulk && iupStrBoolean(bulk))
    {
      Ihandle* child;
      void* state = iupdrvTextAddFormatTagStartBulk(ih);

      char* cleanout = iupAttribGet(formattag, "CLEANOUT");
      if (cleanout && iupStrBoolean(cleanout))
        IupSetAttribute(ih, "REMOVEFORMATTING", "ALL");

      for (child = formattag->firstchild; child; child = child->brother)
        iupdrvTextAddFormatTag(ih, child, 1);

      iupdrvTextAddFormatTagStopBulk(ih, state);
    }
    else
      iupdrvTextAddFormatTag(ih, formattag, 0);

    IupDestroy(formattag);
  }
  else
  {
    Ihandle** tag_array;
    int i;

    if (!ih->data->formattags)
      ih->data->formattags = iupArrayCreate(10, sizeof(Ihandle*));

    i = iupArrayCount(ih->data->formattags);
    tag_array = (Ihandle**)iupArrayInc(ih->data->formattags);
    tag_array[i] = formattag;
  }
  return 0;
}

int iupTextSetAddFormatTagAttrib(Ihandle* ih, const char* value)
{
  return iupTextSetAddFormatTagHandleAttrib(ih, (char*)IupGetHandle(value));
}

static char* iTextGetMaskDataAttrib(Ihandle* ih)
{
  /* Used only by the OLD iupmask API */
  return (char*)ih->data->mask;
}

static char* iTextGetMaskAttrib(Ihandle* ih)
{
  if (ih->data->mask)
    return iupMaskGetStr(ih->data->mask);
  else
    return NULL;
}

static int iTextSetValueMaskedAttrib(Ihandle* ih, const char* value)
{
  if (value)
  {
    if (ih->data->mask && iupMaskCheck(ih->data->mask, value)==0)
      return 0; /* abort */
    IupStoreAttribute(ih, "VALUE", value);
  }
  return 0;
}

static int iTextSetMaskAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    if (ih->data->mask)
    {
      iupMaskDestroy(ih->data->mask);
      ih->data->mask = NULL;
    }
  }
  else
  {
    int casei = iupAttribGetInt(ih, "MASKCASEI");
    Imask* mask = iupMaskCreate(value,casei);
    if (mask)
    {
      if (ih->data->mask)
        iupMaskDestroy(ih->data->mask);

      ih->data->mask = mask;
      return 0;
    }
  }

  return 0;
}

static int iTextSetMaskIntAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    if (ih->data->mask)
    {
      iupMaskDestroy(ih->data->mask);
      ih->data->mask = NULL;
    }
  }
  else
  {
    Imask* mask;
    int min, max;

    if (iupStrToIntInt(value, &min, &max, ':')!=2)
      return 0;

    mask = iupMaskCreateInt(min,max);

    if (ih->data->mask)
      iupMaskDestroy(ih->data->mask);

    ih->data->mask = mask;
  }

  return 0;
}

static int iTextSetMaskFloatAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    if (ih->data->mask)
    {
      iupMaskDestroy(ih->data->mask);
      ih->data->mask = NULL;
    }
  }
  else
  {
    Imask* mask;
    float min, max;

    if (iupStrToFloatFloat(value, &min, &max, ':')!=2)
      return 0;

    mask = iupMaskCreateFloat(min,max);

    if (ih->data->mask)
      iupMaskDestroy(ih->data->mask);

    ih->data->mask = mask;
  }

  return 0;
}

static int iTextSetMultilineAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (iupStrBoolean(value))
  {
    ih->data->is_multiline = 1;
    ih->data->sb = IUP_SB_HORIZ | IUP_SB_VERT;  /* reset SCROLLBAR to YES */
  }
  else
    ih->data->is_multiline = 0;

  return 0;
}

static char* iTextGetMultilineAttrib(Ihandle* ih)
{
  if (ih->data->is_multiline)
    return "YES";
  else
    return "NO";
}

static int iTextSetAppendNewlineAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->append_newline = 1;
  else
    ih->data->append_newline = 0;
  return 0;
}

static char* iTextGetAppendNewlineAttrib(Ihandle* ih)
{
  if (ih->data->append_newline)
    return "YES";
  else
    return "NO";
}

static int iTextSetScrollbarAttrib(Ihandle* ih, const char* value)
{
  /* valid only before map */
  if (ih->handle || !ih->data->is_multiline)
    return 0;

  if (!value)
    value = "YES";    /* default, if multiline, is YES */

  if (iupStrEqualNoCase(value, "YES"))
    ih->data->sb = IUP_SB_HORIZ | IUP_SB_VERT;
  else if (iupStrEqualNoCase(value, "HORIZONTAL"))
    ih->data->sb = IUP_SB_HORIZ;
  else if (iupStrEqualNoCase(value, "VERTICAL"))
    ih->data->sb = IUP_SB_VERT;
  else
    ih->data->sb = IUP_SB_NONE;

  return 0;
}

static char* iTextGetScrollbarAttrib(Ihandle* ih)
{
  if (!ih->data->is_multiline)
    return NULL;
  if (ih->data->sb == (IUP_SB_HORIZ | IUP_SB_VERT))
    return "YES";
  if (ih->data->sb & IUP_SB_HORIZ)
    return "HORIZONTAL";
  if (ih->data->sb & IUP_SB_VERT)
    return "VERTICAL";
  return "NO";   /* IUP_SB_NONE */
}

char* iupTextGetPaddingAttrib(Ihandle* ih)
{
  char *str = iupStrGetMemory(50);
  sprintf(str, "%dx%d", ih->data->horiz_padding, ih->data->vert_padding);
  return str;
}


/********************************************************************/


static int iTextCreateMethod(Ihandle* ih, void** params)
{
  if (params)
  {
    if (params[0]) iupAttribStoreStr(ih, "ACTION", (char*)(params[0]));
  }
  ih->data = iupALLOCCTRLDATA();
  ih->data->append_newline = 1;
  return IUP_NOERROR;
}

static int iMultilineCreateMethod(Ihandle* ih, void** params)
{
  (void)params;
  ih->data->is_multiline = 1;
  ih->data->sb = IUP_SB_HORIZ | IUP_SB_VERT;  /* default is YES */
  return IUP_NOERROR;
}

static void iTextComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *expand)
{
  int natural_w = 0, 
      natural_h = 0,
      visiblecolumns = iupAttribGetInt(ih, "VISIBLECOLUMNS"),
      visiblelines = iupAttribGetInt(ih, "VISIBLELINES");
  (void)expand; /* unset if not a container */

  /* Since the contents can be changed by the user, the size can not be dependent on it. */
  if (ih->data->is_multiline)
  {
    iupdrvFontGetCharSize(ih, NULL, &natural_h);  /* one line height */
    natural_w = iupdrvFontGetStringWidth(ih, "WWWWWWWWWW");
    natural_w = (visiblecolumns*natural_w)/10;
    natural_h = visiblelines*natural_h;
  }
  else
  {
    iupdrvFontGetCharSize(ih, NULL, &natural_h);  /* one line height */
    natural_w = iupdrvFontGetStringWidth(ih, "WWWWWWWWWW");
    natural_w = (visiblecolumns*natural_w)/10;
  }

  /* compute the borders space */
  if (iupAttribGetBoolean(ih, "BORDER"))
    iupdrvTextAddBorders(&natural_w, &natural_h);

  if (iupAttribGetBoolean(ih, "SPIN"))
    iupdrvTextAddSpin(&natural_w, natural_h);

  natural_w += 2*ih->data->horiz_padding;
  natural_h += 2*ih->data->vert_padding;

  /* add scrollbar */
  if (ih->data->is_multiline && ih->data->sb)
  {
    int sb_size = iupdrvGetScrollbarSize();
    if (ih->data->sb & IUP_SB_HORIZ)
      natural_h += sb_size;  /* sb horizontal affects vertical size */
    if (ih->data->sb & IUP_SB_VERT)
      natural_w += sb_size;  /* sb vertical affects horizontal size */
  }

  *w = natural_w;
  *h = natural_h;
}

static void iTextDestroyMethod(Ihandle* ih)
{
  if (ih->data->formattags)
    iTextDestroyFormatTags(ih);
  if (ih->data->mask)
    iupMaskDestroy(ih->data->mask);
}


/******************************************************************************/


void IupTextConvertLinColToPos(Ihandle* ih, int lin, int col, int *pos)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (!ih->handle)
    return;
    
  if (IupClassMatch(ih, "text"))
  {
    if (ih->data->is_multiline)
      iupdrvTextConvertLinColToPos(ih, lin, col, pos);
    else
      *pos = col - 1; /* IUP starts at 1 */
  }
}

void IupTextConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (!ih->handle)
    return;
    
  if (IupClassMatch(ih, "text"))
  {
    if (ih->data->is_multiline)
      iupdrvTextConvertPosToLinCol(ih, pos, lin, col);
    else
    {
      *col = pos + 1; /* IUP starts at 1 */
      *lin = 1;
    }
  }
}

Ihandle* IupText(const char* action)
{
  void *params[2];
  params[0] = (void*)action;
  params[1] = NULL;
  return IupCreatev("text", params);
}

Ihandle* IupMultiLine(const char* action)
{
  void *params[2];
  params[0] = (void*)action;
  params[1] = NULL;
  return IupCreatev("multiline", params);
}

Iclass* iupTextNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "text";
  ic->format = "a"; /* one ACTION callback name */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New = iupTextNewClass;
  ic->Create = iTextCreateMethod;
  ic->Destroy = iTextDestroyMethod;
  ic->ComputeNaturalSize = iTextComputeNaturalSizeMethod;
  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;
  ic->UnMap = iupdrvBaseUnMapMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "CARET_CB", "iii");
  iupClassRegisterCallback(ic, "ACTION", "is");
  iupClassRegisterCallback(ic, "DROPFILES_CB", "siii");
  iupClassRegisterCallback(ic, "BUTTON_CB", "iiiis");
  iupClassRegisterCallback(ic, "MOTION_CB", "iis");
  iupClassRegisterCallback(ic, "SPIN_CB", "i");
  iupClassRegisterCallback(ic, "VALUECHANGED_CB", "");
  

  /* Common Callbacks */
  iupBaseRegisterCommonCallbacks(ic);

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* Drag&Drop */
  iupdrvRegisterDragDropAttrib(ic);

  /* IupText only */
  iupClassRegisterAttribute(ic, "SCROLLBAR", iTextGetScrollbarAttrib, iTextSetScrollbarAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTILINE", iTextGetMultilineAttrib, iTextSetMultilineAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "APPENDNEWLINE", iTextGetAppendNewlineAttrib, iTextSetAppendNewlineAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "VALUEMASKED", NULL, iTextSetValueMaskedAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MASKCASEI", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MASK", iTextGetMaskAttrib, iTextSetMaskAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MASKINT", NULL, iTextSetMaskIntAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MASKFLOAT", NULL, iTextSetMaskFloatAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OLD_MASK_DATA", iTextGetMaskDataAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BORDER", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPIN", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINALIGN", NULL, NULL, IUPAF_SAMEASSYSTEM, "RIGHT", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINAUTO", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SPINWRAP", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VISIBLECOLUMNS", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VISIBLELINES", NULL, NULL, IUPAF_SAMEASSYSTEM, "1", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WORDWRAP", NULL, NULL, NULL, NULL, IUPAF_DEFAULT);

  iupdrvTextInitClass(ic);

  return ic;
}

Iclass* iupMultilineNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("text"));

  ic->name = "multiline";   /* register the multiline name, so LED will work */
  ic->format = "a"; /* one ACTION callback name */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  ic->Create = iMultilineCreateMethod;

  return ic;
}
