/** \file
 * \brief Draw Functions
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPWIN_DRAW_H 
#define __IUPWIN_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif


void iupwinDrawInit(void);

void iupwinDrawBitmap(HDC hDC, HBITMAP hBitmap, HBITMAP hMask, int x, int y, int width, int height, int bpp);
void iupwinDrawText(HDC hDC, const char* text, int x, int y, int width, int height, HFONT hFont, COLORREF fgcolor, int style);

void iupwinDrawParentBackground(Ihandle* ih, HDC hDC, RECT* rect);
void iupwinDrawButtonBorder(HWND hWnd, HDC hDC, RECT *rect, UINT itemState);
void iupwinDraw3StateButton(HWND hWnd, HDC hDC, RECT *rect);

void iupwinDrawThemeFrameBorder(HWND hWnd, HDC hDC, RECT *rect, UINT itemState);
int  iupwinDrawGetThemeTabsBgColor(HWND hWnd, COLORREF *color);
int  iupwinDrawGetThemeButtonBgColor(HWND hWnd, COLORREF *color);
int  iupwinDrawGetThemeFrameFgColor(HWND hWnd, COLORREF *color);
void iupwinDrawRemoveTheme(HWND hWnd);

typedef struct _iupwinBitmapDC
{
  HBITMAP hBitmap, hOldBitmap;
  HDC hBitmapDC, hDC;
  int x, y, w, h;
} iupwinBitmapDC;

HDC iupwinDrawCreateBitmapDC(iupwinBitmapDC *bmpDC, HDC hDC, int x, int y, int w, int h);
void iupwinDrawDestroyBitmapDC(iupwinBitmapDC *bmpDC);

#ifndef ODS_HOTLIGHT     /* Not defined for MingW and Cygwin */
#define ODS_HOTLIGHT        0x0040
#endif

#ifdef __cplusplus
}
#endif

#endif
