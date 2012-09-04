/** \file
 * \brief Windows Font mapping
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdlib.h>
#include <stdio.h>

#include <windows.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_array.h"
#include "iup_attrib.h"
#include "iup_object.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_assert.h"

#include "iupwin_drv.h"
#include "iupwin_info.h"


typedef struct IwinFont_
{
  char standardfont[200];
  HFONT hFont;
  int charwidth, charheight;
} IwinFont;

static Iarray* win_fonts = NULL;

static IwinFont* winFindFont(const char *standardfont)
{
  HFONT hFont;
  int height_pixels;
  char typeface[50] = "";
  int height = 8;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  int res = iupwinGetScreenRes();
  int i, count = iupArrayCount(win_fonts);
  const char* mapped_name;

  /* Check if the standardfont already exists in cache */
  IwinFont* fonts = (IwinFont*)iupArrayGetData(win_fonts);
  for (i = 0; i < count; i++)
  {
    if (iupStrEqualNoCase(standardfont, fonts[i].standardfont))
      return &fonts[i];
  }

  if (!iupGetFontInfo(standardfont, typeface, &height, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return NULL;

  /* Map standard names to native names */
  mapped_name = iupFontGetWinName(typeface);
  if (mapped_name)
    strcpy(typeface, mapped_name);

  /* get in pixels */
  if (height < 0)  
    height_pixels = height;    /* already in pixels */
  else
    height_pixels = -iupWIN_PT2PIXEL(height, res);

  if (height_pixels == 0)
    return NULL;

  hFont = CreateFont(height_pixels,
                        0,0,0,
                        (is_bold) ? FW_BOLD : FW_NORMAL,
                        is_italic,
                        is_underline,
                        is_strikeout,
                        DEFAULT_CHARSET,OUT_TT_PRECIS,
                        CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                        FF_DONTCARE|DEFAULT_PITCH,
                        typeface);
  if (!hFont)
    return NULL;

  /* create room in the array */
  fonts = (IwinFont*)iupArrayInc(win_fonts);

  strcpy(fonts[i].standardfont, standardfont);
  fonts[i].hFont = hFont;

  {
    TEXTMETRIC tm;
    HDC hdc = GetDC(NULL);
    HFONT oldfont = SelectObject(hdc, hFont);

    GetTextMetrics(hdc, &tm);

    SelectObject(hdc, oldfont);
    ReleaseDC(NULL, hdc);
    
    fonts[i].charwidth = tm.tmAveCharWidth; 
    fonts[i].charheight = tm.tmHeight;
  }

  return &fonts[i];
}

static void winFontFromLogFont(LOGFONT* logfont, char * font)
{
  int is_bold = (logfont->lfWeight == FW_NORMAL)? 0: 1;
  int is_italic = logfont->lfItalic;
  int is_underline = logfont->lfUnderline;
  int is_strikeout = logfont->lfStrikeOut;
  int height_pixels = logfont->lfHeight;  /* negative value */
  int res = iupwinGetScreenRes();
  int height = iupWIN_PIXEL2PT(-height_pixels, res);  /* return in points */

  sprintf(font, "%s, %s%s%s%s %d", logfont->lfFaceName, 
                                   is_bold?"Bold ":"", 
                                   is_italic?"Italic ":"", 
                                   is_underline?"Underline ":"", 
                                   is_strikeout?"Strikeout ":"", 
                                   height);
}

char* iupdrvGetSystemFont(void)
{
  static char str[200]; /* must return a static string, because it will be used as the default value for the FONT attribute */
  NONCLIENTMETRICS ncm;
  ncm.cbSize = sizeof(NONCLIENTMETRICS);
  if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, FALSE))
    winFontFromLogFont(&ncm.lfMessageFont, str);
  else
    strcpy(str, "Tahoma, 10");
  return str;
}

char* iupwinFindHFont(HFONT hFont)
{
  int i, count = iupArrayCount(win_fonts);

  /* Check if the standardfont already exists in cache */
  IwinFont* fonts = (IwinFont*)iupArrayGetData(win_fonts);
  for (i = 0; i < count; i++)
  {
    if (hFont == fonts[i].hFont)
      return fonts[i].standardfont;
  }

  return NULL;
}

HFONT iupwinGetHFont(const char* value)
{
  IwinFont* winfont = winFindFont(value);
  if (!winfont)
    return NULL;
  else
    return winfont->hFont;
}

static IwinFont* winFontCreateNativeFont(Ihandle *ih, const char* value)
{
  IwinFont* winfont = winFindFont(value);
  if (!winfont)
  {
    iupERROR1("Failed to create Font: %s", value); 
    return NULL;
  }

  iupAttribSetStr(ih, "_IUP_WINFONT", (char*)winfont);
  return winfont;
}

static IwinFont* winFontGet(Ihandle *ih)
{
  IwinFont* winfont = (IwinFont*)iupAttribGet(ih, "_IUP_WINFONT");
  if (!winfont)
    winfont = winFontCreateNativeFont(ih, iupGetFontAttrib(ih));
  return winfont;
}

char* iupwinGetHFontAttrib(Ihandle *ih)
{
  IwinFont* winfont = winFontGet(ih);
  if (!winfont)
    return NULL;
  else
    return (char*)winfont->hFont;
}

int iupdrvSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  IwinFont* winfont = winFontCreateNativeFont(ih, value);
  if (!winfont)
    return 1;

  /* If FONT is changed, must update the SIZE attribute */
  iupBaseUpdateSizeFromFont(ih);

  /* FONT attribute must be able to be set before mapping, 
      so the font is enable for size calculation. */
  if (ih->handle && (ih->iclass->nativetype != IUP_TYPEVOID))
    SendMessage(ih->handle, WM_SETFONT, (WPARAM)winfont->hFont, MAKELPARAM(TRUE,0));

  return 1;
}

static HDC winFontGetDC(Ihandle* ih)
{
  if (ih->iclass->nativetype == IUP_TYPEVOID)
    return GetDC(NULL);
  else
    return GetDC(ih->handle);  /* handle can be NULL here */
}

static void winFontReleaseDC(Ihandle* ih, HDC hdc)
{
  if (ih->iclass->nativetype == IUP_TYPEVOID)
    ReleaseDC(NULL, hdc);
  else
    ReleaseDC(ih->handle, hdc);  /* handle can be NULL here */
}

void iupdrvFontGetMultiLineStringSize(Ihandle* ih, const char* str, int *w, int *h)
{
  int max_w = 0;

  IwinFont* winfont = winFontGet(ih);
  if (!winfont)
  {
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }

  if (!str)
  {
    if (w) *w = 0;
    if (h) *h = winfont->charheight * 1;
    return;
  }

  if (str[0])
  {
    SIZE size;
    int len;
    const char *nextstr;
    const char *curstr = str;

    HDC hdc = winFontGetDC(ih);
    HFONT oldhfont = SelectObject(hdc, winfont->hFont);

    do
    {
      nextstr = iupStrNextLine(curstr, &len);
      if (len)
      {
        size.cx = 0;
        GetTextExtentPoint32(hdc, curstr, len, &size);
        max_w = iupMAX(max_w, size.cx);
      }

      curstr = nextstr;
    } while(*nextstr);

    SelectObject(hdc, oldhfont);
    winFontReleaseDC(ih, hdc);
  }

  if (w) *w = max_w;
  if (h) *h = winfont->charheight * iupStrLineCount(str);
}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
  HDC hdc;
  HFONT oldhfont, hFont;
  SIZE size;
  int len;
  char* line_end;

  if (!str || str[0]==0)
    return 0;

  hFont = (HFONT)iupwinGetHFontAttrib(ih);
  if (!hFont)
    return 0;

  hdc = winFontGetDC(ih);
  oldhfont = SelectObject(hdc, hFont);

  line_end = strchr(str, '\n');
  if (line_end)
    len = line_end-str;
  else
    len = strlen(str);

  GetTextExtentPoint32(hdc, str, len, &size);

  SelectObject(hdc, oldhfont);
  winFontReleaseDC(ih, hdc);

  return size.cx;
}

void iupdrvFontGetCharSize(Ihandle* ih, int *charwidth, int *charheight)
{
  IwinFont* winfont = winFontGet(ih);
  if (!winfont)
  {
    if (charwidth)  *charwidth = 0;
    if (charheight) *charheight = 0;
    return;
  }

  if (charwidth)  *charwidth = winfont->charwidth; 
  if (charheight) *charheight = winfont->charheight;
}

void iupdrvFontInit(void)
{
  win_fonts = iupArrayCreate(50, sizeof(IwinFont));
}

void iupdrvFontFinish(void)
{
  int i, count = iupArrayCount(win_fonts);
  IwinFont* fonts = (IwinFont*)iupArrayGetData(win_fonts);
  for (i = 0; i < count; i++)
  {
    DeleteObject(fonts[i].hFont);
    fonts[i].hFont = NULL;
  }
  iupArrayDestroy(win_fonts);
}
