/** \file
 * \brief Canvas Controls Private Declarations
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_CANVAS_H 
#define __IUP_CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif


void iupdrvCanvasInitClass(Iclass* ic);
void iupCanvasCalcScrollIntPos(double min, double max, double page, double pos, 
                                 int imin,   int imax,  int *ipage,  int *ipos);
void iupCanvasCalcScrollRealPos(double min, double max, double *pos, 
                                 int imin,   int imax,  int ipage,  int *ipos);
char* iupCanvasGetPosXAttrib(Ihandle* ih);
char* iupCanvasGetPosYAttrib(Ihandle* ih);

#define IUP_SB_MIN 0
#define IUP_SB_MAX INT_MAX-1

struct _IcontrolData 
{
  int sb;    /* scrollbar configuration, valid only after map, use iupBaseGetScrollbar before map */
  float posx, posy;
};


#ifdef __cplusplus
}
#endif

#endif
