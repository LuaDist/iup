/** \file
 * \brief iupmatrix. CD help macros.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPMAT_CD_H 
#define __IUPMAT_CD_H

#ifdef __cplusplus
extern "C" {
#endif

#define iupMATRIX_LINE(_ih,_x1,_y1,_x2,_y2)             cdCanvasLine((_ih)->data->cddbuffer, (_x1), iupMATRIX_INVERTYAXIS(_ih, _y1), (_x2), iupMATRIX_INVERTYAXIS(_ih, _y2))
#define iupMATRIX_VERTEX(_ih,_x,_y)                     cdCanvasVertex((_ih)->data->cddbuffer, (_x), iupMATRIX_INVERTYAXIS(_ih, _y))
#define iupMATRIX_BOX(_ih,_xmin,_xmax,_ymin,_ymax)      cdCanvasBox((_ih)->data->cddbuffer, (_xmin), (_xmax), iupMATRIX_INVERTYAXIS(_ih, _ymin), iupMATRIX_INVERTYAXIS(_ih, _ymax))
#define iupMATRIX_RECT(_ih,_xmin,_xmax,_ymin,_ymax)     cdCanvasRect((_ih)->data->cddbuffer, (_xmin), (_xmax), iupMATRIX_INVERTYAXIS(_ih, _ymin), iupMATRIX_INVERTYAXIS(_ih, _ymax))
#define iupMATRIX_CLIPAREA(_ih,_xmin,_xmax,_ymin,_ymax) cdCanvasClipArea((_ih)->data->cddbuffer, (_xmin), (_xmax), iupMATRIX_INVERTYAXIS(_ih, _ymin), iupMATRIX_INVERTYAXIS(_ih, _ymax))
#define iupMATRIX_TEXT(_ih,_x,_y,_text)                 cdCanvasText((_ih)->data->cddbuffer, (_x), iupMATRIX_INVERTYAXIS(_ih, _y), (_text))

#ifdef __cplusplus
}
#endif

#endif
