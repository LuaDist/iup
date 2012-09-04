/** \file
 * \brief Base for box Controls.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_BOX_H 
#define __IUP_BOX_H

#ifdef __cplusplus
extern "C" {
#endif


struct _IcontrolData 
{
  int alignment,
      expand_children,
      is_homogeneous,
      normalize_size,
      margin_x,
      margin_y,
      gap;
  int children_naturalsize,   /* calculated in ComputeNaturalSize, used in SetChildrenCurrentSize */
      homogeneous_size;       /* calculated in SetChildrenCurrentSize, used in SetChildrenPosition */
};

Iclass* iupBoxNewClassBase(void);

/* Implemented in iup_normalizer.c */
void iupNormalizeSizeBoxChild(Ihandle *ih, int normalize, int children_natural_maxwidth, int children_natural_maxheight);
int iupNormalizeGetNormalizeSize(const char* value);
char* iupNormalizeGetNormalizeSizeStr(int normalize);


#ifdef __cplusplus
}
#endif

#endif
