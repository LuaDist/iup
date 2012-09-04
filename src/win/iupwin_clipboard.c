/** \file
 * \brief Clipboard for the Windows Driver.
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>
 
#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"

#include "iupwin_drv.h"


typedef struct _APMFILEHEADER
{
  WORD  key1,
        key2,
        hmf,
        bleft, btop, bright, bbottom,
        inch,
        reserved1,
        reserved2,
        checksum;
} APMFILEHEADER;

static WORD winAPMChecksum(APMFILEHEADER* papm)
{
  WORD* pw = (WORD*)papm;
  WORD  wSum = 0;
  int   i;
  
  /* The checksum in a Placeable Metafile header is calculated */
  /* by XOR-ing the first 10 words of the header.              */
  
  for (i = 0; i < 10; i++)
    wSum ^= *pw++;
  
  return wSum;
}

static void winWritePlacebleFile(HANDLE hFile, unsigned char* buffer, DWORD dwSize, LONG mm, LONG xExt, LONG yExt)
{
  DWORD nBytesWrite;
  APMFILEHEADER APMHeader;
  int w = xExt, h = yExt;
  
  if (mm == MM_ANISOTROPIC || mm == MM_ISOTROPIC)
  {
    int res = 30;
    w = xExt / res;
    h = yExt / res;
  }
  
  APMHeader.key1 = 0xCDD7;
  APMHeader.key2 = 0x9AC6;
  APMHeader.hmf = 0;
  APMHeader.bleft = 0;
  APMHeader.btop = 0;
  APMHeader.bright = (short)w;
  APMHeader.bbottom = (short)h;
  APMHeader.inch = 100;  /* this number works fine in Word, etc.. */
  APMHeader.reserved1 = 0;
  APMHeader.reserved2 = 0;
  APMHeader.checksum = winAPMChecksum(&APMHeader);
  
  WriteFile(hFile, (LPSTR)&APMHeader, sizeof(APMFILEHEADER), &nBytesWrite, NULL);
  WriteFile(hFile, buffer, dwSize, &nBytesWrite, NULL);
}

static int winClipboardSetSaveEMFAttrib(Ihandle *ih, const char *value)
{
  HENHMETAFILE Handle;
  DWORD dwSize, nBytesWrite;
  unsigned char* buffer;
  HANDLE hFile;
  (void)ih;
  
  OpenClipboard(NULL);
  Handle = (HENHMETAFILE)GetClipboardData(CF_ENHMETAFILE);
  if (Handle == NULL)
  {
    CloseClipboard();
    return 0;
  }
  
  dwSize = GetEnhMetaFileBits(Handle, 0, NULL);
  
  buffer = (unsigned char*)malloc(dwSize);
  
  GetEnhMetaFileBits(Handle, dwSize, buffer);
  
  hFile = CreateFile(value, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
  if (hFile)
  {
    WriteFile(hFile, buffer, dwSize, &nBytesWrite, NULL);
    CloseHandle(hFile);
  }
  
  free(buffer);
  
  CloseClipboard();
  return 0;
}

static int winClipboardSetSaveWMFAttrib(Ihandle *ih, const char *value)
{
  DWORD dwSize;
  unsigned char* buffer;
  METAFILEPICT* lpMFP;
  HANDLE Handle;
  HANDLE hFile;
  (void)ih;
  
  OpenClipboard(NULL);
  Handle = (HENHMETAFILE)GetClipboardData(CF_METAFILEPICT);
  if (Handle == NULL)
  {
    CloseClipboard();
    return 0;
  }
  
  lpMFP = (METAFILEPICT*) GlobalLock(Handle);
  
  dwSize = GetMetaFileBitsEx(lpMFP->hMF, 0, NULL);
  
  buffer = (unsigned char*)malloc(dwSize);
  
  GetMetaFileBitsEx(lpMFP->hMF, dwSize, buffer);
  
  hFile = CreateFile(value, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
  if (hFile)
  {
    winWritePlacebleFile(hFile, buffer, dwSize, lpMFP->mm, lpMFP->xExt, lpMFP->yExt);
    CloseHandle(hFile);
  }
  
  GlobalUnlock(Handle);
  free(buffer);
  
  CloseClipboard();
  return 0;
}

static int winClipboardSetTextAttrib(Ihandle *ih, const char *value)
{
  HANDLE hHandle;
  void* clip_str;
  int size;
  (void)ih;

  if (!OpenClipboard(NULL))
    return 0;

  size = strlen(value)+1;
  hHandle = GlobalAlloc(GMEM_MOVEABLE, size); 
  if (!hHandle)
    return 0;

  clip_str = GlobalLock(hHandle);
  CopyMemory(clip_str, value, size);
  GlobalUnlock(hHandle);

  EmptyClipboard();
  SetClipboardData(CF_TEXT, hHandle);
  CloseClipboard();

  return 0;
}

static char* winClipboardGetTextAttrib(Ihandle *ih)
{
  HANDLE hHandle;
  char* str;
  (void)ih;

  if (!OpenClipboard(NULL))
    return NULL;

  hHandle = GetClipboardData(CF_TEXT);
  if (!hHandle)
  {
    CloseClipboard();
    return NULL;
  }
  
  str = iupStrGetMemoryCopy((char*)GlobalLock(hHandle));
 
  GlobalUnlock(hHandle);
  CloseClipboard();
  return str;
}

static int winClipboardSetImageAttrib(Ihandle *ih, const char *value)
{
  HBITMAP hBitmap;

  if (!OpenClipboard(NULL))
    return 0;

  if (!value)
  {
    EmptyClipboard();
    CloseClipboard();
    return 0;
  }

  hBitmap = (HBITMAP)iupImageGetImage(value, ih, 0);
  iupImageClearCache(ih, hBitmap);

  EmptyClipboard();
  SetClipboardData(CF_BITMAP, (HANDLE)hBitmap);
  CloseClipboard();

  return 0;
}

static int winClipboardSetNativeImageAttrib(Ihandle *ih, const char *value)
{
  if (!OpenClipboard(NULL))
    return 0;

  EmptyClipboard();
  if (value)
    SetClipboardData(CF_DIB, (HANDLE)value);
  CloseClipboard();

  (void)ih;
  return 0;
}

static HANDLE winCopyHandle(HANDLE hHandle)
{
  void *src_data, *dst_data;
  SIZE_T size = GlobalSize(hHandle);
  HANDLE hNewHandle = GlobalAlloc(GMEM_MOVEABLE, size); 
  if (!hNewHandle)
    return NULL;

  src_data = GlobalLock(hHandle);
  dst_data = GlobalLock(hNewHandle);
  CopyMemory(dst_data, src_data, size);
  GlobalUnlock(hHandle);
  GlobalUnlock(hNewHandle);

  return hNewHandle;
}

static char* winClipboardGetNativeImageAttrib(Ihandle *ih)
{
  HANDLE hHandle;

  if (!OpenClipboard(NULL))
    return 0;

  hHandle = GetClipboardData(CF_DIB);
  if (!hHandle)
  {
    CloseClipboard();
    return NULL;
  }

  hHandle = winCopyHandle(hHandle);   /* must duplicate because CloseClipboard will invalidate the handle */
  CloseClipboard();
  
  (void)ih;
  return (char*)hHandle;
}

static char* winClipboardGetTextAvailableAttrib(Ihandle *ih)
{
  int check;
  (void)ih;
  OpenClipboard(NULL);
  check = IsClipboardFormatAvailable(CF_TEXT);
  CloseClipboard();
  if (check)
    return "YES";
  else
    return "NO";
}

static char* winClipboardGetImageAvailableAttrib(Ihandle *ih)
{
  int check;
  (void)ih;
  OpenClipboard(NULL);
  check = IsClipboardFormatAvailable(CF_DIB);
  CloseClipboard();
  if (check)
    return "YES";
  else
    return "NO";
}

static char* winClipboardGetWMFAvailableAttrib(Ihandle *ih)
{
  int check;
  (void)ih;
  OpenClipboard(NULL);
  check = IsClipboardFormatAvailable(CF_METAFILEPICT);
  CloseClipboard();
  if (check)
    return "YES";
  else
    return "NO";
}

static char* winClipboardGetEMFAvailableAttrib(Ihandle *ih)
{
  int check;
  (void)ih;
  OpenClipboard(NULL);
  check = IsClipboardFormatAvailable(CF_ENHMETAFILE);
  CloseClipboard();
  if (check)
    return "YES";
  else
    return "NO";
}

/******************************************************************************/

Ihandle* IupClipboard(void)
{
  return IupCreate("clipboard");
}

Iclass* iupClipboardNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "clipboard";
  ic->format = NULL;  /* no parameters */
  ic->nativetype = IUP_TYPECONTROL;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 0;

  ic->New = iupClipboardNewClass;

  /* Attribute functions */
  iupClassRegisterAttribute(ic, "TEXT", winClipboardGetTextAttrib, winClipboardSetTextAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NATIVEIMAGE", winClipboardGetNativeImageAttrib, winClipboardSetNativeImageAttrib, NULL, NULL, IUPAF_NO_STRING|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGE", NULL, winClipboardSetImageAttrib, NULL, NULL, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TEXTAVAILABLE", winClipboardGetTextAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEAVAILABLE", winClipboardGetImageAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "WMFAVAILABLE", winClipboardGetWMFAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "EMFAVAILABLE", winClipboardGetEMFAvailableAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SAVEEMF", NULL, winClipboardSetSaveEMFAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SAVEWMF", NULL, winClipboardSetSaveWMFAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}
