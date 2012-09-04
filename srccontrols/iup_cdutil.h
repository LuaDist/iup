/** \file
 * \brief cdiuputil. CD and IUP utilities for the IupControls
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_CDUTIL_H 
#define __IUP_CDUTIL_H 

#ifdef __cplusplus
extern "C" {
#endif

void cdIupCalcShadows(long bgcolor, long *light_shadow, long *mid_shadow, long *dark_shadow);
long cdIupConvertColor(const char *color);
void cdIupDrawSunkenRect(cdCanvas *canvas, int x1, int y1, int x2, int y2,  
                         long light_shadow, long mid_shadow, long dark_shadow);
void cdIupDrawRaisenRect(cdCanvas *canvas, int x1, int y1, int x2, int y2,  
                         long light_shadow, long mid_shadow, long dark_shadow);
void cdIupDrawVertSunkenMark(cdCanvas *canvas, int x, int y1, int y2, long light_shadow, long dark_shadow);
void cdIupDrawHorizSunkenMark(cdCanvas *canvas, int x1, int x2, int y, long light_shadow, long dark_shadow);
void cdIupDrawFocusRect(Ihandle* ih, cdCanvas *canvas, int x1, int y1, int x2, int y2);
void cdIupSetFont(Ihandle* ih, cdCanvas *canvas, const char* font);

void cdDrawFocusRect(cdCanvas *canvas, int x1, int y1, int x2, int y2);

#define cdIupInvertYAxis(_y, _h) ((_h) - (_y) - 1);

#define cdIupLIGTHER(_x)    ((unsigned char)(((_x)+ 192)/2))


#ifdef __cplusplus
}
#endif

#endif
