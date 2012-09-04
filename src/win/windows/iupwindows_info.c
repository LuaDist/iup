/** \file
 * \brief Windows System Information
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h> 

/* This module should depend only on IUP core headers 
   and Windows system headers. */

#include <windows.h>

#include "iup_str.h"
#include "iup_drvinfo.h"


char* iupdrvLocaleInfo(void)
{
  CPINFOEX info;
  GetCPInfoEx(CP_ACP, 0, &info);
  return iupStrGetMemoryCopy(info.CodePageName);
}

int iupdrvMakeDirectory(const char* name) 
{
  if (CreateDirectory(name, NULL))
    return 1;
  else
    return 0;
}

int iupdrvIsFile(const char* name)
{
  DWORD attrib = GetFileAttributes(name);
  if (attrib == INVALID_FILE_ATTRIBUTES)
    return 0;
  if (attrib & FILE_ATTRIBUTE_DIRECTORY)
    return 0;
  return 1;
}

int iupdrvIsDirectory(const char* name)
{
  DWORD attrib = GetFileAttributes(name);
  if (attrib == INVALID_FILE_ATTRIBUTES)
    return 0;
  if (attrib & FILE_ATTRIBUTE_DIRECTORY)
    return 1;
  return 0;
}            

char* iupdrvGetCurrentDirectory(void)
{
  char* cur_dir;
  int len = GetCurrentDirectory(0, NULL);
  if (len == 0) return NULL;

  cur_dir = malloc(len+2);
  GetCurrentDirectory(len+1, cur_dir);
  cur_dir[len] = '\\';
  cur_dir[len+1] = 0;
  return cur_dir;
}

int iupdrvSetCurrentDirectory(const char* dir)
{
  return SetCurrentDirectory(dir);
}

void iupdrvGetScreenSize(int *width, int *height)
{
  RECT area;
  SystemParametersInfo(SPI_GETWORKAREA, 0, &area, 0);
  *width = (int)(area.right - area.left);
  *height =  (int)(area.bottom - area.top);
}

void iupdrvGetFullSize(int *width, int *height)
{
  RECT rect;
  GetWindowRect(GetDesktopWindow(), &rect);
  *width = rect.right - rect.left;
  *height = rect.bottom - rect.top;
}

int iupdrvGetScreenDepth(void)
{
  int bpp;
  HDC hDCDisplay = GetDC(NULL);
  bpp = GetDeviceCaps(hDCDisplay, BITSPIXEL);
  ReleaseDC(NULL, hDCDisplay);
  return bpp;
}

float iupdrvGetScreenDpi(void)
{
  float dpi;
  HDC hDCDisplay = GetDC(NULL);
  dpi = (float)GetDeviceCaps(hDCDisplay, LOGPIXELSY);
  ReleaseDC(NULL, hDCDisplay);
  return dpi;
}

void iupdrvGetCursorPos(int *x, int *y)
{
  POINT CursorPoint;
  GetCursorPos(&CursorPoint);
  *x = (int)CursorPoint.x;
  *y = (int)CursorPoint.y;
}

void iupdrvGetKeyState(char* key)
{
  if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
    key[0] = 'S';
  else
    key[0] = ' ';
  if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
    key[1] = 'C';
  else
    key[1] = ' ';
  if (GetAsyncKeyState(VK_MENU) & 0x8000)
    key[2] = 'A';
  else
    key[2] = ' ';
  if ((GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000))
    key[3] = 'Y';
  else
    key[3] = ' ';

  key[4] = 0;
}

char *iupdrvGetSystemName(void)
{
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);

  if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
  {
    if (osvi.dwMajorVersion <= 4)
      return "WinNT";

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
      return "Win2K";

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion > 0)
      return "WinXP";

    if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
      return "Vista";

    if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion > 0)
      return "Win7";
  }

  return "Windows";
}

char *iupdrvGetSystemVersion(void)
{
  char *str = iupStrGetMemory(256);
  OSVERSIONINFOEX osvi;
  SYSTEM_INFO si;

  ZeroMemory(&si, sizeof(SYSTEM_INFO));
  GetSystemInfo(&si);

  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((OSVERSIONINFO*)&osvi);

  sprintf(str, "%d.%d.%d", (int)osvi.dwMajorVersion, (int)osvi.dwMinorVersion, (int)osvi.dwBuildNumber);

  /* Display service pack (if any). */
  if (osvi.szCSDVersion && osvi.szCSDVersion[0]!=0)
  {
    strcat(str, " ");
    strcat(str, osvi.szCSDVersion);
  }

  if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
    strcat(str, " (IA64)");
  else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
    strcat(str, " (x64)");
  else
    strcat(str, " (x86)");

  return str;
}

char *iupdrvGetComputerName(void)
{
  DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
  char* str = iupStrGetMemory(size);
  GetComputerName((LPTSTR)str, &size);
  return str;
}

char *iupdrvGetUserName(void)
{
  DWORD size = 256;
  char* str = iupStrGetMemory(size);
  GetUserName((LPTSTR)str, &size);
  return (char*)str;
}
