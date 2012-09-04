/** \file
 * \brief OLD mask pattern matching
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>

#include "iup.h"
#include "iupcontrols.h"
#include "iupmask.h"

#include "iup_mask.h"
#include "iup_str.h"


void iupmaskRemove(Ihandle *ih)
{
  IupSetAttribute(ih,"MASK", NULL);
}

void iupmaskMatRemove(Ihandle *ih, int lin, int col)
{
  IupMatSetAttribute(ih,"MASK", lin, col, NULL);
}

int iupmaskSetInt(Ihandle *ih, int autofill, int min, int max)
{
  (void)autofill;
  IupSetfAttribute(ih,"MASKINT", "%d:%d", min, max);
  return 1;
}

int iupmaskMatSetInt(Ihandle *ih, int autofill, int min, int max, int lin, int col)
{
  (void)autofill;
  IupMatSetfAttribute(ih,"MASKINT", lin, col, "%d:%d", min, max);
  return 1;
}

int iupmaskSetFloat(Ihandle* ih, int autofill, float min, float max)
{
  (void)autofill;
  IupSetfAttribute(ih,"MASKFLOAT", "%g:%g", min, max);
  return 1;
}

int iupmaskMatSetFloat(Ihandle* ih, int autofill, float min, float max, int lin, int col)
{
  (void)autofill;
  IupMatSetfAttribute(ih,"MASKFLOAT", lin, col, "%g:%g", min, max);
  return 0;
}

int iupmaskSet(Ihandle* ih, const char* mask_str, int autofill, int casei)
{
  (void)autofill;
  IupSetAttribute(ih,"MASKCASEI", casei?"YES":"NO");
  IupStoreAttribute(ih,"MASK", mask_str);
  if (iupStrEqual(mask_str, IupGetAttribute(ih,"MASK")))
    return 1;
  else
    return 0;
}

int iupmaskMatSet(Ihandle* ih, const char* mask_str, int autofill, int casei, int lin, int col)
{
  (void)autofill;
  IupMatSetAttribute(ih,"MASKCASEI", lin, col, casei?"YES":"NO");
  IupMatSetAttribute(ih,"MASK", lin, col, (char*)mask_str);
  if (iupStrEqual(mask_str, IupMatGetAttribute(ih,"MASK", lin, col)))
    return 1;
  else
    return 0;
}

int iupmaskCheck(Ihandle* ih)
{
  char *val = IupGetAttribute(ih,"VALUE");
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");
  return iupMaskCheck(mask,val)==1;
}

int iupmaskMatCheck(Ihandle *ih, int lin, int col)
{
  char *val = IupMatGetAttribute(ih,"",lin,col);
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");
  return iupMaskCheck(mask,val)==1;
}

int iupmaskGet(Ihandle *ih, char **sval)
{
  char *val = IupGetAttribute(ih,"VALUE");
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");

  if (iupMaskCheck(mask,val)==1)
  {
    *sval = val;
    return 1;
  }
  else
    return 0;
}

int iupmaskMatGet(Ihandle *ih, char **sval, int lin, int col)
{
  char *val = IupMatGetAttribute(ih,"",lin,col);
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");

  if (iupMaskCheck(mask,val)==1)
  {
    *sval = val;
    return 1;
  }
  else
    return 0;
}

int iupmaskGetDouble(Ihandle *ih, double *dval)
{
  char *val = IupGetAttribute(ih,"VALUE");
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");

  if(iupMaskCheck(mask,val)==1)
  {
    *dval = 0.0;
    sscanf(val,"%lf",dval);
    return 1;
  }
  else
    return 0;
}

int iupmaskMatGetDouble(Ihandle *ih, double *dval, int lin, int col)
{
  char *val = IupMatGetAttribute(ih,"",lin,col);
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");

  if(iupMaskCheck(mask,val)==1)
  {
    *dval = 0.0;
    sscanf(val,"%lf",dval);
    return 1;
  }
  else
    return 0;
}

int iupmaskGetFloat(Ihandle *ih, float *fval)
{
  char *val = IupGetAttribute(ih,"VALUE");
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");

  if(iupMaskCheck(mask,val)==1)
  {
    *fval = 0.0F;
    sscanf(val,"%f",fval);
    return 1;
  }
  else
    return 0;
}

int iupmaskMatGetFloat(Ihandle *ih, float *fval, int lin, int col)
{
  char *val = IupMatGetAttribute(ih,"",lin,col);
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");

  if (iupMaskCheck(mask,val)==1)
  {
    *fval = 0.0F;
    sscanf(val,"%f",fval);
    return 1;
  }
  else
    return 0;
}

int iupmaskGetInt(Ihandle *ih, int *ival)
{
  char *val = IupGetAttribute(ih,"VALUE");
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");

  if (iupMaskCheck(mask,val)==1)
  {
    *ival = 0;
    sscanf(val,"%d",ival);
    return 1;
  }
  else
    return 0;
}

int iupmaskMatGetInt(Ihandle *ih, int *ival, int lin, int col)
{
  char *val = IupMatGetAttribute(ih,"",lin,col);
  Imask* mask = (Imask*)IupGetAttribute(ih,"OLD_MASK_DATA");

  if(iupMaskCheck(mask,val)==1)
  {
    *ival = 0;
    sscanf(val,"%d",ival);
    return 1;
  }
  else
    return 0;
}
