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
#include "iup_imglib_bitmapsgtk.h"

void iupImglibBitmapsGtkOpen(void)
{
  iupImageStockSet("IUP_ActionCancel", 0, "gtk-cancel");
  iupImageStockSet("IUP_ActionOk", 0, "gtk-apply");
  iupImageStockSet("IUP_ArrowDown", 0, "gtk-go-down");
  iupImageStockSet("IUP_ArrowLeft", 0, "gtk-go-back-ltr");
  iupImageStockSet("IUP_ArrowRight", 0, "gtk-go-forward-ltr");  
  iupImageStockSet("IUP_ArrowUp", 0, "gtk-go-up");       
  iupImageStockSet("IUP_EditCopy", 0, "gtk-copy");
  iupImageStockSet("IUP_EditCut", 0, "gtk-cut");
  iupImageStockSet("IUP_EditErase", 0, "gtk-close");
  iupImageStockSet("IUP_EditFind", 0, "gtk-find");
  iupImageStockSet("IUP_EditPaste", 0, "gtk-paste");
  iupImageStockSet("IUP_EditRedo", 0, "gtk-redo-ltr");
  iupImageStockSet("IUP_EditUndo", 0, "gtk-undo-ltr");
  iupImageStockSet("IUP_FileClose", load_image_iupgtk_close, 0);
  iupImageStockSet("IUP_FileCloseAll", load_image_iupgtk_close_all, 0);
  iupImageStockSet("IUP_FileNew", 0, "gtk-new");
  iupImageStockSet("IUP_FileOpen", 0, "gtk-open");
  iupImageStockSet("IUP_FileProperties", 0, "gtk-properties");
  iupImageStockSet("IUP_FileSave", 0, "gtk-save");
  iupImageStockSet("IUP_FileSaveAll", load_image_iupgtk_save_all, 0);
  iupImageStockSet("IUP_FileText", load_image_iupgtk_text, 0);
  iupImageStockSet("IUP_FontBold", 0, "gtk-bold");
  iupImageStockSet("IUP_FontDialog", 0, "gtk-font");
  iupImageStockSet("IUP_FontItalic", 0, "gtk-italic");
  iupImageStockSet("IUP_MediaForward", 0, "gtk-media-forward-ltr");
  iupImageStockSet("IUP_MediaGotoBegin", 0, "gtk-media-previous-ltr");
  iupImageStockSet("IUP_MediaGoToEnd", 0, "gtk-media-next-ltr");
  iupImageStockSet("IUP_MediaPause", 0, "gtk-media-pause");
  iupImageStockSet("IUP_MediaPlay", 0, "gtk-media-play-ltr");
  iupImageStockSet("IUP_MediaRecord", 0, "gtk-media-record");
  iupImageStockSet("IUP_MediaReverse", 0, "gtk-media-play-rtl");
  iupImageStockSet("IUP_MediaRewind", 0, "gtk-media-rewind-ltr");
  iupImageStockSet("IUP_MediaStop", 0, "gtk-media-stop");
  iupImageStockSet("IUP_MessageError", 0, "gtk-stop");
  iupImageStockSet("IUP_MessageHelp", 0, "gtk-help");
  iupImageStockSet("IUP_MessageInfo", 0, "gtk-info");
  iupImageStockSet("IUP_NavigateHome", 0, "gtk-home");
  iupImageStockSet("IUP_NavigateRefresh", 0, "gtk-refresh");
  iupImageStockSet("IUP_Print", 0, "gtk-print");
  iupImageStockSet("IUP_PrintPreview", 0, "gtk-print-preview");
  iupImageStockSet("IUP_ToolsColor", 0, "gtk-select-color");
  iupImageStockSet("IUP_ToolsSettings", 0, "gtk-preferences");
  iupImageStockSet("IUP_ToolsSortAscend", 0, "gtk-sort-ascending");
  iupImageStockSet("IUP_ToolsSortDescend", 0, "gtk-sort-descending");
  iupImageStockSet("IUP_ViewFullScreen", 0, "gtk-fullscreen");
  iupImageStockSet("IUP_WindowsCascade", load_image_iupgtk_cascade, 0);
  iupImageStockSet("IUP_WindowsTile", load_image_iupgtk_tile, 0);
  iupImageStockSet("IUP_Zoom", load_image_iupgtk_zoom, 0);
  iupImageStockSet("IUP_ZoomActualSize", 0, "gtk-zoom-100");
  iupImageStockSet("IUP_ZoomIn", 0, "gtk-zoom-in");
  iupImageStockSet("IUP_ZoomOut", 0, "gtk-zoom-out");
  iupImageStockSet("IUP_ZoomSelection", 0, "gtk-zoom-fit");
}
