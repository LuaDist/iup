/** \file
 * \brief mask pattern matching
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup_maskparse.h"
#include "iup_mask.h"
#include "iup_str.h"


#define IUP_MASK_FLOAT      "[+/-]?(/d+/.?/d*|/./d+)"
#define IUP_MASK_UFLOAT     "(/d+/.?/d*|/./d+)"
#define IUP_MASK_INT	      "[+/-]?/d+"
#define IUP_MASK_UINT     	"/d+"

struct _Imask
{
  char* mask_str;
  ImaskParsed* fsm;
  int casei;
  char type;
  float fmin, 
        fmax;
  int   imin,
        imax;
};


int iupMaskCheck(Imask* mask, const char *val)
{
  int ret;

  /* empty text or no mask */
  if (!val || !(*val) || !mask) 
    return 1;

  ret = iupMaskMatch(val,mask->fsm,0,NULL,NULL,NULL,mask->casei);
  if (ret == IMASK_PARTIALMATCH)
    return -1;
  if (ret != (int)strlen(val))
    return 0;

  switch(mask->type)
  {
  case 'I':
    {
      int ival = 0;
      sscanf(val,"%d",&ival);
      if(ival < mask->imin || ival > mask->imax)
        return 0;
      break;
    }
  case 'F':
    {
      float fval = 0;
      sscanf(val,"%f",&fval);
      if(fval < mask->fmin || fval > mask->fmax)
        return 0;
      break;
    }
  }

  return 1;
}

Imask* iupMaskCreate(const char* mask_str, int casei)
{
  ImaskParsed* fsm;
  Imask* mask;
  char* copy_mask_str;

  if (!mask_str)
    return NULL;

  /* Parse the mask first */
  copy_mask_str = iupStrDup(mask_str);
  if (iupMaskParse(copy_mask_str, &fsm) != IMASK_PARSE_OK)
  {
    free(copy_mask_str);
    return NULL;
  }

  mask = (Imask*)malloc(sizeof(Imask));
  memset(mask, 0, sizeof(Imask));

  mask->mask_str = copy_mask_str;
  mask->casei = casei;
  mask->fsm = fsm;

  return mask;
}

Imask* iupMaskCreateInt(int min, int max)
{
  Imask* mask;

  if (min < 0)
    mask = iupMaskCreate(IUP_MASK_INT, 0);
  else
    mask = iupMaskCreate(IUP_MASK_UINT, 0);

  if (mask)
  {
    mask->imin = min;
    mask->imax = max;
    mask->type = 'I';
  }

  return mask;
}

Imask* iupMaskCreateFloat(float min, float max)
{
  Imask* mask;

  if (min < 0)
    mask = iupMaskCreate(IUP_MASK_FLOAT, 0);
  else
    mask = iupMaskCreate(IUP_MASK_UFLOAT, 0);

  if (mask)
  {
    mask->fmin = min;
    mask->fmax = max;
    mask->type = 'F';
  }

  return mask;
}

void iupMaskDestroy(Imask* mask)
{
  free(mask->mask_str); 
  free(mask->fsm); 
  free(mask); 
}

char* iupMaskGetStr(Imask* mask)
{
  return mask->mask_str;
}
