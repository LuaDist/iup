/** \file
 * \brief IupFileDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>

#include <stdio.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_dialog.h"
#include "iup_drvinfo.h"

#include "iupwin_drv.h"


/* Not defined for Cygwin or MingW */
#ifndef OFN_FORCESHOWHIDDEN
#define OFN_FORCESHOWHIDDEN          0x10000000
#endif

#define IUP_MAX_FILENAME_SIZE 65000
#define IUP_PREVIEWCANVAS 3000

#define IUP_EDIT        0x0480


enum {IUP_DIALOGOPEN, IUP_DIALOGSAVE, IUP_DIALOGDIR};


static void winFileDlgStrReplacePathSlash(char* name)
{
  /* check for "/" */
  int i, len = strlen(name);
  for (i = 0; i < len; i++)
  {
    if (name[i] == '/')
      name[i] = '\\';
  }
}

static INT CALLBACK winFileDlgBrowseCallback(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  (void)lParam;
  if (uMsg == BFFM_INITIALIZED)
  {
    char* value;
    Ihandle* ih = (Ihandle*)lpData;
    ih->handle = hWnd;
    iupDialogUpdatePosition(ih);
    ih->handle = NULL;  /* reset handle */
    value = iupStrDup(iupAttribGet(ih, "DIRECTORY"));
    if (value)
    {
      winFileDlgStrReplacePathSlash(value);
      SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)value);
      free(value);
    }
  }
  else if (uMsg == BFFM_SELCHANGED)
  {
    char buffer[IUP_MAX_FILENAME_SIZE];
    ITEMIDLIST* selecteditem = (ITEMIDLIST*)lParam;
    buffer[0] = 0;
    SHGetPathFromIDList(selecteditem, buffer);
    if (buffer[0] == 0)
      SendMessage(hWnd, BFFM_ENABLEOK, 0, (LPARAM)FALSE);
    else
      SendMessage(hWnd, BFFM_ENABLEOK, 0, (LPARAM)TRUE);
  }
  return 0;
}

static void winFileDlgGetFolder(Ihandle *ih)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  BROWSEINFO browseinfo;
  char buffer[MAX_PATH];
  ITEMIDLIST *selecteditem;

  if (!parent)
    parent = GetActiveWindow();

  ZeroMemory(&browseinfo, sizeof(BROWSEINFO));
  browseinfo.lpszTitle = iupAttribGet(ih, "TITLE");
  browseinfo.pszDisplayName = buffer; 
  browseinfo.lpfn = winFileDlgBrowseCallback;
  browseinfo.lParam = (LPARAM)ih;
  browseinfo.ulFlags = IupGetGlobal("_IUPWIN_COINIT_MULTITHREADED")? 0: BIF_NEWDIALOGSTYLE;
  browseinfo.hwndOwner = parent;

  selecteditem = SHBrowseForFolder(&browseinfo);
  if (!selecteditem)
  {
    iupAttribSetStr(ih, "VALUE", NULL);
    iupAttribSetStr(ih, "STATUS", "-1");
  }
  else
  {
    SHGetPathFromIDList(selecteditem, buffer);
    iupAttribStoreStr(ih, "VALUE", buffer);
    iupAttribSetStr(ih, "STATUS", "0");
  }

  iupAttribSetStr(ih, "FILEEXIST", NULL);
  iupAttribSetStr(ih, "FILTERUSED", NULL);
}

/************************************************************************************************/

static int winFileDlgGetSelectedFile(Ihandle* ih, HWND hWnd, char* filename)
{
  int ret = CommDlg_OpenSave_GetFilePath(GetParent(hWnd), filename, IUP_MAX_FILENAME_SIZE);
  if (ret < 0)
    return 0;

  if (iupAttribGetBoolean(ih, "MULTIPLEFILES"))
  {
    /* check if there are more than 1 files and return only the first one */
    int found = 0;
    while(*filename != 0)
    {            
      if (*filename == '"')
      {
        if (!found)
          found = 1;
        else
        {
          *(filename-1) = 0;
          return 1;
        }
      }
      if (found)
        *filename = *(filename+1);
      filename++;
    }
  }

  return 1;
}

static int winFileDlgWmNotify(HWND hWnd, LPOFNOTIFY pofn)
{
  Ihandle* ih = (Ihandle*)pofn->lpOFN->lCustData;
  switch (pofn->hdr.code)
  {
  case CDN_INITDONE:
    {
      HWND hWndPreview;

      IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
      if (cb) cb(ih, NULL, "INIT");

      hWndPreview = GetDlgItem(hWnd, IUP_PREVIEWCANVAS);
      if (hWndPreview) 
        RedrawWindow(hWndPreview, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW);
      break;
    }
  case CDN_FILEOK:
  case CDN_SELCHANGE:
    {
      HWND hWndPreview;

      IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
      if (cb)
      {
        char filename[IUP_MAX_FILENAME_SIZE];
        if (winFileDlgGetSelectedFile(ih, hWnd, filename))
        {
          int ret;
          char* file_msg;

          if (!iupdrvIsFile(filename))
            file_msg = "OTHER";
          else if (pofn->hdr.code == CDN_FILEOK)
            file_msg = "OK";
          else 
            file_msg = "SELECT";

          ret = cb(ih, filename, file_msg);
          if (pofn->hdr.code == CDN_FILEOK && ret == IUP_IGNORE) 
          {
            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, 1L);
            return 1; /* will refuse the file */
          }
        }
      }

      hWndPreview = GetDlgItem(hWnd, IUP_PREVIEWCANVAS);
      if (pofn->hdr.code == CDN_SELCHANGE && hWndPreview) 
        RedrawWindow(hWndPreview, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW);
      break;
    }
  case CDN_TYPECHANGE:
    {
      IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
      if (cb)
      {
        char filename[IUP_MAX_FILENAME_SIZE];
        if (winFileDlgGetSelectedFile(ih, hWnd, filename))
        {
          iupAttribSetInt(ih, "FILTERUSED", (int)pofn->lpOFN->nFilterIndex);
          if (cb(ih, filename, "FILTER") == IUP_CONTINUE)
          {
            char* value = iupAttribGet(ih, "FILE");
            if (value)
            {
              strncpy(filename, value, IUP_MAX_FILENAME_SIZE);
              winFileDlgStrReplacePathSlash(filename);
              SendMessage(GetParent(hWnd), CDM_SETCONTROLTEXT, (WPARAM)IUP_EDIT, (LPARAM)filename);
            }
          }
        }
      }
      break;
    }
  case CDN_HELP:
    {
      Icallback cb = (Icallback) IupGetCallback(ih, "HELP_CB");
      if (cb && cb(ih) == IUP_CLOSE) 
        EndDialog(GetParent(hWnd), IDCANCEL);
      break;
    }
  }

  return 0;
}

static UINT_PTR CALLBACK winFileDlgSimpleHook(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  (void)wParam;
  switch(uiMsg)
  {
  case WM_INITDIALOG:
    {
      OPENFILENAME* openfilename = (OPENFILENAME*)lParam;
      Ihandle* ih = (Ihandle*)openfilename->lCustData;
      ih->handle = GetParent(hWnd);
      iupDialogUpdatePosition(ih);
      ih->handle = NULL;  /* reset handle */
      SetWindowLongPtr(hWnd, DWLP_USER, (LONG_PTR)ih);
      break;
    }
  case WM_DESTROY:
    {
      Ihandle* ih = (Ihandle*)GetWindowLongPtr(hWnd, DWLP_USER);
      IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
      if (cb) cb(ih, NULL, "FINISH");
      break;
    }
  case WM_NOTIFY:
    return winFileDlgWmNotify(hWnd, (LPOFNOTIFY)lParam);
  }
  return 0;
}

static void winFileDlgSetPreviewCanvasPos(HWND hWnd, HWND hWndPreview)
{
  int height, width, ypos, xpos;
  RECT rect, dlgrect;
  HWND hWndFileList = GetDlgItem(GetParent(hWnd), 0x0471);
  HWND hWndFileCombo = GetDlgItem(GetParent(hWnd), 0x047C);

  /* GetWindowRect return screen coordinates, must convert to parent's client coordinates */
  GetWindowRect(hWnd, &dlgrect);

  GetWindowRect(hWndPreview, &rect);
  ypos = rect.top - dlgrect.top;   /* keep the same vertical position, at first time this is 0 */
  height = rect.bottom-rect.top;   /* keep the same height */

  GetWindowRect(hWndFileList, &rect);
  xpos = rect.left - dlgrect.left;   /* horizontally align with file list at left */

  GetWindowRect(hWndFileCombo, &rect);
  width = (rect.right - dlgrect.left) - xpos;  /* set size to align with file combo at right */

  /* also position the child window that contains the template, must have room for the preview canvas */
  if (ypos) /* first time does nothing */
    SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, (rect.right - dlgrect.left), (dlgrect.bottom - dlgrect.top), SWP_NOMOVE|SWP_NOZORDER);

  SetWindowPos(hWndPreview, HWND_TOP, xpos, ypos, width, height, SWP_NOZORDER);
}

static void winFileDlgUpdatePreviewGLCanvas(Ihandle* ih)
{
  Ihandle* glcanvas = IupGetAttributeHandle(ih, "PREVIEWGLCANVAS");
  if (glcanvas)
  {
    iupAttribSetStr(glcanvas, "HWND", iupAttribGet(ih, "HWND"));
    glcanvas->iclass->Map(glcanvas);
  }
}

static UINT_PTR CALLBACK winFileDlgPreviewHook(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
  /* hWnd here is a handle to the child window that contains the template,
     NOT the file dialog. Only the preview canvas is a child of this window. */

  switch(uiMsg)
  {
  case WM_INITDIALOG:
    {
      OPENFILENAME* openfilename = (OPENFILENAME*)lParam;
      Ihandle* ih = (Ihandle*)openfilename->lCustData;
      HWND hWndPreview = GetDlgItem(hWnd, IUP_PREVIEWCANVAS);

      ih->handle = GetParent(hWnd);
      iupDialogUpdatePosition(ih);
      ih->handle = NULL;  /* reset handle */

      if (hWndPreview)
      {
        RECT rect;

        winFileDlgSetPreviewCanvasPos(hWnd, hWndPreview);

        GetClientRect(hWndPreview, &rect);
        iupAttribSetInt(ih, "PREVIEWWIDTH", rect.right - rect.left);
        iupAttribSetInt(ih, "PREVIEWHEIGHT", rect.bottom - rect.top);
      }
      SetWindowLongPtr(hWnd, DWLP_USER, (LONG_PTR)ih);
      iupAttribSetStr(ih, "WID", (char*)hWndPreview);
      iupAttribSetStr(ih, "HWND", (char*)hWndPreview);
      winFileDlgUpdatePreviewGLCanvas(ih);
      break;
    }
  case WM_DRAWITEM:
    {
      if (wParam == IUP_PREVIEWCANVAS)
      {
        LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
        Ihandle* ih = (Ihandle*)GetWindowLongPtr(hWnd, DWLP_USER);
        /* callback here always exists */
        IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
        char filename[IUP_MAX_FILENAME_SIZE];
        iupAttribSetStr(ih, "PREVIEWDC", (char*)lpDrawItem->hDC);
        if (winFileDlgGetSelectedFile(ih, hWnd, filename))
        {
          if (iupdrvIsFile(filename))
            cb(ih, filename, "PAINT");
          else
            cb(ih, NULL, "PAINT");
        }
        else
          cb(ih, NULL, "PAINT");
        iupAttribSetStr(ih, "PREVIEWDC", NULL);
      }
      break;
    }
  case WM_SIZE:
    {
      HWND hWndPreview = GetDlgItem(hWnd, IUP_PREVIEWCANVAS);
      if (hWndPreview)
      {
        Ihandle* ih = (Ihandle*)GetWindowLongPtr(hWnd, DWLP_USER);
        RECT rect;

        winFileDlgSetPreviewCanvasPos(hWnd, hWndPreview);

        GetClientRect(hWndPreview, &rect);
        iupAttribSetInt(ih, "PREVIEWWIDTH", rect.right-rect.left);

        RedrawWindow(hWndPreview, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW);
      }
      break;
    }
  case WM_DESTROY:
    {
      Ihandle* ih = (Ihandle*)GetWindowLongPtr(hWnd, DWLP_USER);
      /* callback here always exists */
      IFnss cb = (IFnss)IupGetCallback(ih, "FILE_CB");
      cb(ih, NULL, "FINISH");
      break;
    }
  case WM_NOTIFY:
      return winFileDlgWmNotify(hWnd, (LPOFNOTIFY)lParam);
  }
  return 0;
}

static char* winFileDlgStrReplaceSeparator(const char* name)
{
  int i=0;
  char* buffer = (char*)malloc(strlen(name)+2);

  /* replace symbols "|" by terminator "\0" */

  while (*name) 
  {
    buffer[i] = *name;

    if (buffer[i] == '|')
      buffer[i] = 0;

    i++;
    name++;
  }

  buffer[i] = 0;
  buffer[i+1] = 0;      /* additional 0 at the end */
  return buffer;
}

static int winFileDlgPopup(Ihandle *ih, int x, int y)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  OPENFILENAME openfilename;
  int result, dialogtype;
  char *value;

  iupAttribSetInt(ih, "_IUPDLG_X", x);   /* used in iupDialogUpdatePosition */
  iupAttribSetInt(ih, "_IUPDLG_Y", y);

  value = iupAttribGetStr(ih, "DIALOGTYPE");
  if (iupStrEqualNoCase(value, "SAVE"))
    dialogtype = IUP_DIALOGSAVE;
  else if (iupStrEqualNoCase(value, "DIR"))
    dialogtype = IUP_DIALOGDIR;
  else
    dialogtype = IUP_DIALOGOPEN;

  if (dialogtype == IUP_DIALOGDIR)
  {
    winFileDlgGetFolder(ih);
    return IUP_NOERROR;
  }

  if (!parent)
    parent = GetActiveWindow();  /* if NOT set will NOT be Modal */
                                 /* anyway it will be modal only relative to its parent */

  ZeroMemory(&openfilename, sizeof(OPENFILENAME));
  openfilename.lStructSize = sizeof(OPENFILENAME);
  openfilename.hwndOwner = parent;

  value = iupAttribGet(ih, "EXTFILTER");
  if (value)
  {
    int index;
    openfilename.lpstrFilter = winFileDlgStrReplaceSeparator(value);

    value = iupAttribGet(ih, "FILTERUSED");
    if (iupStrToInt(value, &index))
      openfilename.nFilterIndex = index;
    else
      openfilename.nFilterIndex = 1;
  }
  else 
  {
    value = iupAttribGet(ih, "FILTER");
    if (value)
    {
      int sz1, sz2;
      char* info = iupAttribGet(ih, "FILTERINFO");
      if (!info)
        info = value;

      /* concat FILTERINFO+FILTER */
      sz1 = strlen(info)+1;
      sz2 = strlen(value)+1;
      openfilename.lpstrFilter = (char*)malloc(sz1+sz2+1);
      memcpy((char*)openfilename.lpstrFilter, info, sz1);
      memcpy((char*)openfilename.lpstrFilter+sz1, value, sz2);
      ((char*)openfilename.lpstrFilter)[sz1+sz2] = 0; /* additional 0 at the end */

      openfilename.nFilterIndex = 1;
    }
  }

  openfilename.lpstrFile = (char*)malloc(IUP_MAX_FILENAME_SIZE+1);
  value = iupAttribGet(ih, "FILE");
  if (value)
  {
    strncpy(openfilename.lpstrFile, value, IUP_MAX_FILENAME_SIZE);
    winFileDlgStrReplacePathSlash(openfilename.lpstrFile);
  }
  else
    openfilename.lpstrFile[0] = 0;

  openfilename.nMaxFile = IUP_MAX_FILENAME_SIZE;

  openfilename.lpstrInitialDir = iupStrDup(iupAttribGet(ih, "DIRECTORY"));
  if (openfilename.lpstrInitialDir)
    winFileDlgStrReplacePathSlash((char*)openfilename.lpstrInitialDir);

  openfilename.lpstrTitle = iupAttribGet(ih, "TITLE");
  openfilename.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

  if (!iupAttribGetBoolean(ih, "NOOVERWRITEPROMPT"))
    openfilename.Flags |= OFN_OVERWRITEPROMPT;

  if (iupAttribGetBoolean(ih, "SHOWHIDDEN"))
    openfilename.Flags |= OFN_FORCESHOWHIDDEN;

  value = iupAttribGet(ih, "ALLOWNEW");
  if (!value)
  {
    if (dialogtype == IUP_DIALOGSAVE)
      value = "YES";
    else
      value = "NO";
  }
  if (iupStrBoolean(value))
    openfilename.Flags |= OFN_CREATEPROMPT;
  else
    openfilename.Flags |= OFN_FILEMUSTEXIST;

  if (iupAttribGetBoolean(ih, "NOCHANGEDIR"))
    openfilename.Flags |= OFN_NOCHANGEDIR;

  if (iupAttribGetBoolean(ih, "MULTIPLEFILES"))
     openfilename.Flags |= OFN_ALLOWMULTISELECT;

  openfilename.lpfnHook = winFileDlgSimpleHook;
  openfilename.Flags |= OFN_ENABLEHOOK | OFN_EXPLORER | OFN_ENABLESIZING;
  openfilename.lCustData = (LPARAM)ih;

  if (iupAttribGetBoolean(ih, "SHOWPREVIEW") && IupGetCallback(ih, "FILE_CB"))
  {
    openfilename.Flags |= OFN_ENABLETEMPLATE;
    openfilename.hInstance = iupwin_dll_hinstance? iupwin_dll_hinstance: iupwin_hinstance;
    openfilename.lpTemplateName = "iupPreviewDlg";
    openfilename.lpfnHook = winFileDlgPreviewHook;
  }

  if (IupGetCallback(ih, "HELP_CB"))
    openfilename.Flags |= OFN_SHOWHELP;

  if (dialogtype == IUP_DIALOGOPEN)
    result = GetOpenFileName(&openfilename);
  else
    result = GetSaveFileName(&openfilename);

  if (result)
  {
    if (iupAttribGetBoolean(ih, "MULTIPLEFILES"))
    {
      int i = 0;

      char* dir = iupStrFileGetPath(openfilename.lpstrFile);  /* the first part is the directory already */
      iupAttribStoreStr(ih, "DIRECTORY", dir);
      free(dir);
    
      /* If there is more than one file, replace terminator by the separator */
      if (openfilename.lpstrFile[openfilename.nFileOffset-1] == 0 && 
          openfilename.nFileOffset>0) 
      {
        while (openfilename.lpstrFile[i] != 0 || openfilename.lpstrFile[i+1] != 0)
        {
          if (openfilename.lpstrFile[i]==0)
            openfilename.lpstrFile[i] = '|';
          i++;
        }
        openfilename.lpstrFile[i] = '|';
      }

      iupAttribSetStr(ih, "STATUS", "0");
      iupAttribSetStr(ih, "FILEEXIST", "YES");
    }
    else
    {
      if (iupdrvIsFile(openfilename.lpstrFile))  /* check if file exists */
      {
        char* dir = iupStrFileGetPath(openfilename.lpstrFile);
        iupAttribStoreStr(ih, "DIRECTORY", dir);
        free(dir);

        iupAttribSetStr(ih, "FILEEXIST", "YES");
        iupAttribSetStr(ih, "STATUS", "0");
      }
      else
      {
        iupAttribSetStr(ih, "FILEEXIST", "NO");
        iupAttribSetStr(ih, "STATUS", "1");
      }
    }

    iupAttribStoreStr(ih, "VALUE", openfilename.lpstrFile);
    iupAttribSetInt(ih, "FILTERUSED", (int)openfilename.nFilterIndex);
  }
  else
  {
    iupAttribSetStr(ih, "FILTERUSED", NULL);
    iupAttribSetStr(ih, "VALUE", NULL);
    iupAttribSetStr(ih, "FILEEXIST", NULL);
    iupAttribSetStr(ih, "STATUS", "-1");
  }

  if (openfilename.lpstrFilter) free((char*)openfilename.lpstrFilter);
  if (openfilename.lpstrInitialDir) free((char*)openfilename.lpstrInitialDir);
  if (openfilename.lpstrFile) free(openfilename.lpstrFile);

  return IUP_NOERROR;
}

void iupdrvFileDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = winFileDlgPopup;

  /* IupFileDialog Windows and GTK Only */
  iupClassRegisterAttribute(ic, "EXTFILTER", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERINFO", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERUSED", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTIPLEFILES", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
