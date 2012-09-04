/** \file
 * \brief IupImgLib
 *
 * See Copyright Notice in iup.h
 */

#include <stdlib.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_image.h"

#include "iup_imglib.h"

/* source code, included only here */
#include "iup_imglib_bitmaps8.h"

void iupImglibBitmaps8Open(void)
{
  iupImageStockSet("IUP_ActionCancel", load_image_ActionCancel8, 0);
  iupImageStockSet("IUP_ActionOk", load_image_ActionOk8, 0);
  iupImageStockSet("IUP_ArrowDown", load_image_ArrowDown8, 0);
  iupImageStockSet("IUP_ArrowLeft", load_image_ArrowLeft8, 0);
  iupImageStockSet("IUP_ArrowRight", load_image_ArrowRight8, 0);
  iupImageStockSet("IUP_ArrowUp", load_image_ArrowUp8, 0);
  iupImageStockSet("IUP_EditCopy", load_image_EditCopy8, 0);
  iupImageStockSet("IUP_EditCut", load_image_EditCut8, 0);
  iupImageStockSet("IUP_EditErase", load_image_EditErase8, 0);
  iupImageStockSet("IUP_EditFind", load_image_EditFind8, 0);
  iupImageStockSet("IUP_EditPaste", load_image_EditPaste8, 0);
  iupImageStockSet("IUP_EditRedo", load_image_EditRedo8, 0);
  iupImageStockSet("IUP_EditUndo", load_image_EditUndo8, 0);
  iupImageStockSet("IUP_FileClose", load_image_FileClose8, 0);
  iupImageStockSet("IUP_FileCloseAll", load_image_FileCloseAll8, 0);
  iupImageStockSet("IUP_FileNew", load_image_FileNew8, 0);
  iupImageStockSet("IUP_FileOpen", load_image_FileOpen8, 0);
  iupImageStockSet("IUP_FileProperties", load_image_FileProperties8, 0);
  iupImageStockSet("IUP_FileSave", load_image_FileSave8, 0);
  iupImageStockSet("IUP_FileSaveAll", load_image_FileSaveAll8, 0);
  iupImageStockSet("IUP_FileText", load_image_FileText8, 0);
  iupImageStockSet("IUP_FontBold", load_image_FontBold8, 0);
  iupImageStockSet("IUP_FontDialog", load_image_FontDialog8, 0);
  iupImageStockSet("IUP_FontItalic", load_image_FontItalic8, 0);
  iupImageStockSet("IUP_MediaForward", load_image_MediaForward8, 0);
  iupImageStockSet("IUP_MediaGotoBegin", load_image_MediaGoToBegin8, 0);
  iupImageStockSet("IUP_MediaGoToEnd", load_image_MediaGoToEnd8, 0);
  iupImageStockSet("IUP_MediaPause", load_image_MediaPause8, 0);
  iupImageStockSet("IUP_MediaPlay", load_image_MediaPlay8, 0);
  iupImageStockSet("IUP_MediaRecord", load_image_MediaRecord8, 0);
  iupImageStockSet("IUP_MediaReverse", load_image_MediaReverse8, 0);
  iupImageStockSet("IUP_MediaRewind", load_image_MediaRewind8, 0);
  iupImageStockSet("IUP_MediaStop", load_image_MediaStop8, 0);
  iupImageStockSet("IUP_MessageError", load_image_MessageError8, 0);
  iupImageStockSet("IUP_MessageHelp", load_image_MessageHelp8, 0);
  iupImageStockSet("IUP_MessageInfo", load_image_MessageInfo8, 0);
  iupImageStockSet("IUP_NavigateHome", load_image_NavigateHome8, 0);
  iupImageStockSet("IUP_NavigateRefresh", load_image_NavigateRefresh8, 0);
  iupImageStockSet("IUP_Print", load_image_Print8, 0);
  iupImageStockSet("IUP_PrintPreview", load_image_PrintPreview8, 0);
  iupImageStockSet("IUP_ToolsColor", load_image_ToolsColor8, 0);
  iupImageStockSet("IUP_ToolsSettings", load_image_ToolsSettings8, 0);
  iupImageStockSet("IUP_ToolsSortAscend", load_image_ToolsSortAscend8, 0);
  iupImageStockSet("IUP_ToolsSortDescend", load_image_ToolsSortDescend8, 0);
  iupImageStockSet("IUP_ViewFullScreen", load_image_ViewFullScreen8, 0);
  iupImageStockSet("IUP_WindowsCascade", load_image_WindowsCascade8, 0);
  iupImageStockSet("IUP_WindowsTile", load_image_WindowsTile8, 0);
  iupImageStockSet("IUP_Zoom", load_image_Zoom8, 0);
  iupImageStockSet("IUP_ZoomActualSize", load_image_ZoomActualSize8, 0);
  iupImageStockSet("IUP_ZoomIn", load_image_ZoomIn8, 0);
  iupImageStockSet("IUP_ZoomOut", load_image_ZoomOut8, 0);
  iupImageStockSet("IUP_ZoomSelection", load_image_ZoomSelection8, 0);
}
