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
#include "iup_imglib_bitmaps.h"

void iupImglibBitmapsOpen(void)
{
  iupImageStockSet("IUP_ActionCancel", load_image_ActionCancel, 0);
  iupImageStockSet("IUP_ActionOk", load_image_ActionOk, 0);
  iupImageStockSet("IUP_ArrowDown", load_image_ArrowDown, 0);
  iupImageStockSet("IUP_ArrowLeft", load_image_ArrowLeft, 0);
  iupImageStockSet("IUP_ArrowRight", load_image_ArrowRight, 0);
  iupImageStockSet("IUP_ArrowUp", load_image_ArrowUp, 0);
  iupImageStockSet("IUP_EditCopy", load_image_EditCopy, 0);
  iupImageStockSet("IUP_EditCut", load_image_EditCut, 0);
  iupImageStockSet("IUP_EditErase", load_image_EditErase, 0);
  iupImageStockSet("IUP_EditFind", load_image_EditFind, 0);
  iupImageStockSet("IUP_EditPaste", load_image_EditPaste, 0);
  iupImageStockSet("IUP_EditRedo", load_image_EditRedo, 0);
  iupImageStockSet("IUP_EditUndo", load_image_EditUndo, 0);
  iupImageStockSet("IUP_FileClose", load_image_FileClose, 0);
  iupImageStockSet("IUP_FileCloseAll", load_image_FileCloseAll, 0);
  iupImageStockSet("IUP_FileNew", load_image_FileNew, 0);
  iupImageStockSet("IUP_FileOpen", load_image_FileOpen, 0);
  iupImageStockSet("IUP_FileProperties", load_image_FileProperties, 0);
  iupImageStockSet("IUP_FileSave", load_image_FileSave, 0);
  iupImageStockSet("IUP_FileSaveAll", load_image_FileSaveAll, 0);
  iupImageStockSet("IUP_FileText", load_image_FileText, 0);
  iupImageStockSet("IUP_FontBold", load_image_FontBold, 0);
  iupImageStockSet("IUP_FontDialog", load_image_FontDialog, 0);
  iupImageStockSet("IUP_FontItalic", load_image_FontItalic, 0);
  iupImageStockSet("IUP_MediaForward", load_image_MediaForward, 0);
  iupImageStockSet("IUP_MediaGotoBegin", load_image_MediaGoToBegin, 0);
  iupImageStockSet("IUP_MediaGoToEnd", load_image_MediaGoToEnd, 0);
  iupImageStockSet("IUP_MediaPause", load_image_MediaPause, 0);
  iupImageStockSet("IUP_MediaPlay", load_image_MediaPlay, 0);
  iupImageStockSet("IUP_MediaRecord", load_image_MediaRecord, 0);
  iupImageStockSet("IUP_MediaReverse", load_image_MediaReverse, 0);
  iupImageStockSet("IUP_MediaRewind", load_image_MediaRewind, 0);
  iupImageStockSet("IUP_MediaStop", load_image_MediaStop, 0);
  iupImageStockSet("IUP_MessageError", load_image_MessageError, 0);
  iupImageStockSet("IUP_MessageHelp", load_image_MessageHelp, 0);
  iupImageStockSet("IUP_MessageInfo", load_image_MessageInfo, 0);
  iupImageStockSet("IUP_NavigateHome", load_image_NavigateHome, 0);
  iupImageStockSet("IUP_NavigateRefresh", load_image_NavigateRefresh, 0);
  iupImageStockSet("IUP_Print", load_image_Print, 0);
  iupImageStockSet("IUP_PrintPreview", load_image_PrintPreview, 0);
  iupImageStockSet("IUP_ToolsColor", load_image_ToolsColor, 0);
  iupImageStockSet("IUP_ToolsSettings", load_image_ToolsSettings, 0);
  iupImageStockSet("IUP_ToolsSortAscend", load_image_ToolsSortAscend, 0);
  iupImageStockSet("IUP_ToolsSortDescend", load_image_ToolsSortDescend, 0);
  iupImageStockSet("IUP_ViewFullScreen", load_image_ViewFullScreen, 0);
  iupImageStockSet("IUP_WindowsCascade", load_image_WindowsCascade, 0);
  iupImageStockSet("IUP_WindowsTile", load_image_WindowsTile, 0);
  iupImageStockSet("IUP_Zoom", load_image_Zoom, 0);
  iupImageStockSet("IUP_ZoomActualSize", load_image_ZoomActualSize, 0);
  iupImageStockSet("IUP_ZoomIn", load_image_ZoomIn, 0);
  iupImageStockSet("IUP_ZoomOut", load_image_ZoomOut, 0);
  iupImageStockSet("IUP_ZoomSelection", load_image_ZoomSelection, 0);
}
