/** \file
 * \brief iupmatrix. change number of collumns or lines.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPMAT_NUMLC_H 
#define __IUPMAT_NUMLC_H

#ifdef __cplusplus
extern "C" {
#endif

int  iupMatrixSetAddLinAttrib(Ihandle* ih, const char* value);
int  iupMatrixSetDelLinAttrib(Ihandle* ih, const char* value);
int  iupMatrixSetAddColAttrib(Ihandle* ih, const char* value);
int  iupMatrixSetDelColAttrib(Ihandle* ih, const char* value);

int  iupMatrixSetNumLinAttrib(Ihandle* ih, const char* value);
int  iupMatrixSetNumColAttrib(Ihandle* ih, const char* value);


#ifdef __cplusplus
}
#endif

#endif
