/** \file
 * \brief Mask functions
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_MASK_H 
#define __IUP_MASK_H

#ifdef __cplusplus
extern "C" {
#endif


/** \defgroup mask Text Mask
 * \par
 * Used to filter text input in IupText.
 * \par
 * See \ref iup_mask.h
 * \ingroup util */

typedef struct _Imask Imask;

/** Creates a mask given a string.  \n
 * If casei is true, will turn the mask case insensitive.
 * \ingroup mask */
Imask* iupMaskCreate(const char* mask_str, int casei);

/** Creates an integer mask with limits.
 * \ingroup mask */
Imask* iupMaskCreateInt(int min, int max);

/** Creates a float mask with limits.
 * \ingroup mask */
Imask* iupMaskCreateFloat(float min, float max);

/** Destroys the mask.
 * \ingroup mask */
void iupMaskDestroy(Imask* mask);

/** Check if the value is valid using the mask to filter it.
 * Returns 1 if full match, -1 if partial match, and 0 otherwise.
 * \ingroup mask */
int iupMaskCheck(Imask* mask, const char *value);

/** Returns the mask string.
 * \ingroup mask */
char* iupMaskGetStr(Imask* mask);


#ifdef __cplusplus
}
#endif

#endif
