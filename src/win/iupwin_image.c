/** \file
 * \brief Image Resource.
 *
 * See Copyright Notice in "iup.h"
 */

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"

#include "iupwin_drv.h"

/* RGB in RGBA DIBs are pre-multiplied by alpha to AlphaBlend usage. */
#define iupALPHAPRE(_src, _alpha) (((_src)*(_alpha))/255)

static int winDibNumColors(BITMAPINFOHEADER* bmih)
{
  if (bmih->biBitCount > 8)
  {
    if (bmih->biCompression == BI_BITFIELDS)
      return 3;
    else
      return 0;
  }
  else
  {
    if (bmih->biClrUsed != 0)
      return bmih->biClrUsed;
    else
      return 1 << bmih->biBitCount;
  }
}

void iupdrvImageGetRawData(void* handle, unsigned char* imgdata)
{
  int x, y, w, h, bpp, bmp_line_size, bits_size;
  BYTE* bits;
  HANDLE hHandle = (HANDLE)handle;
  void* dib = GlobalLock(hHandle);
  BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)dib;

  w = bmih->biWidth;
  h = abs(bmih->biHeight);
  bpp = iupImageNormBpp(bmih->biBitCount);
  bmp_line_size = ((w * bmih->biBitCount + 31) / 32) * 4;    /* DWORD aligned, 4 bytes boundary in a N bpp image */
  bits_size = bmp_line_size*h;

  bits = ((BYTE*)dib) + sizeof(BITMAPINFOHEADER) + winDibNumColors(bmih)*sizeof(RGBQUAD);

  /* windows bitmaps are bottom up */
  /* imgdata is bottom up */

  if (bmih->biHeight < 0)
    bits = bits + (bits_size - bmp_line_size); /* start of last line */

  if (bpp == 8)
  {
    for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x++)
      {
        switch (bmih->biBitCount)
        {
        case 1:
          *imgdata++ = (unsigned char)((bits[x / 8] >> (7 - x % 8)) & 0x01);
          break;
        case 4:
          *imgdata++ = (unsigned char)((bits[x / 2] >> ((1 - x % 2) * 4)) & 0x0F);
          break;
        case 8:
          *imgdata++ = bits[x];
          break;
        }
      }
    
      if (bmih->biHeight < 0)
        bits -= bmp_line_size;
      else
        bits += bmp_line_size;
    }
  }
  else
  {
    int offset, planesize;
    unsigned short color;
    unsigned int rmask = 0, gmask = 0, bmask = 0, 
                  roff = 0, goff = 0, boff = 0; /* pixel bit mask control when reading 16 and 32 bpp images */
    unsigned char *red, *green, *blue, *alpha;

    planesize = w*h;
    red = imgdata;
    green = imgdata+planesize;
    blue = imgdata+2*planesize;
    alpha = imgdata+3*planesize;
    
    if (bmih->biBitCount == 16)
      offset = bmp_line_size;  /* do not increment for each pixel, jump line */
    else
      offset = bmp_line_size - (w*bmih->biBitCount)/8;   /* increment for each pixel, jump pad */
    
    if (bmih->biCompression == BI_BITFIELDS)
    {
      unsigned int Mask;
      unsigned int* palette = (unsigned int*)(((BYTE*)bmih) + sizeof(BITMAPINFOHEADER));
      
      rmask = Mask = palette[0];
      while (!(Mask & 0x01))
      {Mask >>= 1; roff++;}
      
      gmask = Mask = palette[1];
      while (!(Mask & 0x01))
      {Mask >>= 1; goff++;}
      
      bmask = Mask = palette[2];
      while (!(Mask & 0x01))
      {Mask >>= 1; boff++;}
    }
    else if (bmih->biBitCount == 16)
    {
      bmask = 0x001F;
      gmask = 0x03E0;
      rmask = 0x7C00;
      boff = 0;
      goff = 5;
      roff = 10;
    }
    
    for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x++)
      {
        if (bmih->biBitCount == 16)
        {
          color = ((unsigned short*)bits)[x];
          *red++ = (unsigned char)((((rmask & color) >> roff) * 255) / (rmask >> roff));
          *green++ = (unsigned char)((((gmask & color) >> goff) * 255) / (gmask >> goff));
          *blue++ = (unsigned char)((((bmask & color) >> boff) * 255) / (bmask >> boff));
        }
        else
        {
          *blue++ = *bits++;
          *green++ = *bits++;
          *red++ = *bits++;
          
          if (bmih->biBitCount == 32)
          {
            if (alpha)
              *alpha++ = *bits++;
            else
              bits++;
          }
        }
      }
      
      bits += offset;

      if (bmih->biHeight < 0)
        bits -= 2*bmp_line_size;
    }
  }

  GlobalUnlock(hHandle);
}

void* iupdrvImageCreateImageRaw(int width, int height, int bpp, iupColor* colors, int colors_count, unsigned char *imgdata)
{
  int y,x,bmp_line_size,channels,bits_size,header_size;
  HANDLE hHandle;
  BYTE* bits;   /* DIB bitmap bits, created in CreateDIBSection and filled here */
  void* dib;
  BITMAPINFOHEADER* bmih;

  bmp_line_size = ((width * bpp + 31) / 32) * 4;    /* DWORD aligned, 4 bytes boundary in a N bpp image */
  bits_size = bmp_line_size*height;
  header_size = sizeof(BITMAPINFOHEADER) + colors_count*sizeof(RGBQUAD);

  hHandle = GlobalAlloc(GMEM_MOVEABLE, header_size+bits_size);
  if (!hHandle)
    return NULL;

  dib = GlobalLock(hHandle);
  bits = ((BYTE*)dib) + header_size;

  bmih = (BITMAPINFOHEADER*)dib;

  memset(bmih, 0, sizeof(BITMAPINFOHEADER));
  bmih->biSize = sizeof(BITMAPINFOHEADER);
  bmih->biWidth  = width;
  bmih->biHeight = height;
  bmih->biPlanes = 1; /* not the same as PLANES */
  bmih->biBitCount = (WORD)bpp;
  bmih->biCompression = BI_RGB;
  bmih->biClrUsed = colors_count;

  if (colors_count)
  {
    RGBQUAD* bitmap_colors = (RGBQUAD*)(((BYTE*)dib) + sizeof(BITMAPINFOHEADER));
    int i;
    for (i=0;i<colors_count;i++)
    {
      bitmap_colors[i].rgbRed   = colors[i].r;
      bitmap_colors[i].rgbGreen = colors[i].g;
      bitmap_colors[i].rgbBlue  = colors[i].b;
      bitmap_colors[i].rgbReserved = 0;
    }
  }

  channels = 1;
  if (bpp == 24)
    channels = 3;
  else if (bpp == 32)
    channels = 4;

  /* windows bitmaps are bottom up */
  /* imgdata is bottom up */

  if (bpp != 8) /* (bpp == 32 || bpp == 24) */
  {
    unsigned char *r, *g, *b, *a;

    int planesize = width*height;
    r = imgdata;
    g = imgdata+planesize;
    b = imgdata+2*planesize;
    a = imgdata+3*planesize;

    for (y=0; y<height; y++)
    {
      int lineoffset = y*width;
      for (x=0; x<width; x++)
      {
        int offset = channels*x;
        /* Windows Bitmap order is BGRA */
        BYTE *bmp_b = &bits[offset],
             *bmp_g = bmp_b+1,
             *bmp_r = bmp_g+1,
             *bmp_a = bmp_r+1;

        *bmp_r = r[lineoffset+x];
        *bmp_g = g[lineoffset+x];
        *bmp_b = b[lineoffset+x];

        if (channels == 4)  /* bpp==32 */
        {
          *bmp_a = a[lineoffset+x];

          /* RGB in RGBA DIBs are pre-multiplied by alpha to AlphaBlend usage. */
          *bmp_r = iupALPHAPRE(*bmp_r,*bmp_a);
          *bmp_g = iupALPHAPRE(*bmp_g,*bmp_a);
          *bmp_b = iupALPHAPRE(*bmp_b,*bmp_a);
        }
      }

      bits += bmp_line_size;
    }
  }
  else
  {
    for (y=0; y<height; y++)
    {
      int lineoffset = y*width;
      for (x=0; x<width; x++)
      {
        bits[x] = imgdata[lineoffset+x];
      }

      bits += bmp_line_size;
    }
  }

  GlobalUnlock(hHandle);
  return hHandle;
}

int iupdrvImageGetRawInfo(void* handle, int *w, int *h, int *bpp, iupColor* colors, int *colors_count)
{
  HANDLE hHandle = (HANDLE)handle;
  void* dib = GlobalLock(hHandle);
  BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)dib;
  
  if (w) *w = bmih->biWidth;
  if (h) *h = abs(bmih->biHeight);
  if (bpp) *bpp = iupImageNormBpp(bmih->biBitCount);

  if (bmih->biBitCount <= 8)
  {
    RGBQUAD* bitmap_colors = (RGBQUAD*)(((BYTE*)bmih) + sizeof(BITMAPINFOHEADER));
    int i;

    if (bmih->biClrUsed != 0)
      *colors_count = bmih->biClrUsed;
    else
      *colors_count = 1 << bmih->biBitCount;

    for (i=0;i<*colors_count;i++)
    {
      colors[i].r = bitmap_colors[i].rgbRed;
      colors[i].g = bitmap_colors[i].rgbGreen;
      colors[i].b = bitmap_colors[i].rgbBlue;
    }
  }

  GlobalUnlock(hHandle);
  return 1;
}

static int winImageInitDibColors(iupColor* colors, RGBQUAD* bmpcolors, int colors_count, 
                                 unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, int make_inactive)
{
  int i, ret = 0;
  for (i=0;i<colors_count;i++)
  {
    bmpcolors[i].rgbRed   = colors[i].r;
    bmpcolors[i].rgbGreen = colors[i].g;
    bmpcolors[i].rgbBlue  = colors[i].b;
    bmpcolors[i].rgbReserved = colors[i].a;

    if (bmpcolors[i].rgbReserved == 0) /* full transparent alpha */
    {
      bmpcolors[i].rgbBlue = bg_b; 
      bmpcolors[i].rgbGreen = bg_g; 
      bmpcolors[i].rgbRed = bg_r;
      ret = 1;
    }
    else
      bmpcolors[i].rgbReserved = 0; /* clear non transparent mark */

    if (make_inactive)
      iupImageColorMakeInactive(&(bmpcolors[i].rgbRed), &(bmpcolors[i].rgbGreen), &(bmpcolors[i].rgbBlue), 
                                bg_r, bg_g, bg_b);
  }

  return ret;
}

static HBITMAP winImageCreateBitmap(Ihandle *ih, int width, int height, int bpp, BYTE** bits, 
                                    unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, int make_inactive)
{
  HDC hDC;
  int colors_count;
  HBITMAP hBitmap;
  BITMAPINFOHEADER* bmih;  /* bitmap info header */
  iupColor colors[256];

  if (bpp == 32 || bpp == 24)
    colors_count = 0;
  else /* bpp == 8 */
    iupImageInitColorTable(ih, colors, &colors_count);

  bmih = malloc(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*colors_count);
  if (!bmih)
    return NULL;

  memset(bmih, 0, sizeof(BITMAPINFOHEADER));
  bmih->biSize = sizeof(BITMAPINFOHEADER);
  bmih->biWidth  = width;
  bmih->biHeight = height;
  bmih->biPlanes = 1; /* not the same as PLANES */
  bmih->biBitCount = (WORD)bpp;
  bmih->biCompression = BI_RGB;
  bmih->biClrUsed = colors_count;

  if (colors_count)
  {
    /* since colors are only passed to the CreateDIBSection here, must update BGCOLOR and inactive here */
    RGBQUAD* bitmap_colors = (RGBQUAD*)(((BYTE*)bmih) + sizeof(BITMAPINFOHEADER));
    if (winImageInitDibColors(colors, bitmap_colors, colors_count, bg_r, bg_g, bg_b, make_inactive))
      iupAttribSetStr(ih, "_IUP_BGCOLOR_DEPEND", "1");
  }

  hDC = GetDC(NULL);
  hBitmap = CreateDIBSection(hDC, (BITMAPINFO*)bmih, DIB_RGB_COLORS, (void**)bits, NULL, 0x0);
  ReleaseDC(NULL, hDC);
  free(bmih);

  return hBitmap;
}

void* iupdrvImageCreateImage(Ihandle *ih, const char* bgcolor, int make_inactive)
{
  unsigned char bg_r = 0, bg_g = 0, bg_b = 0;
  int y,x,bmp_line_size,data_line_size,
      width = ih->currentwidth,
      height = ih->currentheight,
      channels = iupAttribGetInt(ih, "CHANNELS"),
      flat_alpha = iupAttribGetBoolean(ih, "FLAT_ALPHA"),
      bpp = iupAttribGetInt(ih, "BPP");
  unsigned char *imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");
  HBITMAP hBitmap;
  BYTE* bits;   /* DIB bitmap bits, created in CreateDIBSection and filled here */

  iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);

  hBitmap = winImageCreateBitmap(ih, width, height, bpp, &bits, bg_r, bg_g, bg_b, make_inactive);
  if (!hBitmap)
    return NULL;

  bmp_line_size = ((width * bpp + 31) / 32) * 4;    /* DWORD aligned, 4 bytes boundary in a N bpp image */
  data_line_size = width*channels;

  /* windows bitmaps are bottom up */
  imgdata += (height-1)*data_line_size;  /* iupimage is top down */

  for (y=0; y<height; y++)
  {
    for (x=0; x<width; x++)
    {
      if (bpp != 8) /* (bpp == 32 || bpp == 24) */
      {
        int offset = channels*x;
        /* Windows Bitmap order is BGRA */
        BYTE *b = &bits[offset],
             *g = b+1,
             *r = g+1,
             *a = r+1,
             *dat = &imgdata[offset];

        *r = *(dat);
        *g = *(dat+1);
        *b = *(dat+2);

        if (channels == 4)  /* bpp==32 */
        {
          if (flat_alpha)
          {
            *a = *(dat+3);
            *r = iupALPHABLEND(*r, bg_r, *a);
            *g = iupALPHABLEND(*g, bg_g, *a);
            *b = iupALPHABLEND(*b, bg_b, *a);
            *a = 255;
          }

          if (make_inactive)
            iupImageColorMakeInactive(r, g, b, bg_r, bg_g, bg_b);

          if (!flat_alpha)
          {
            *a = *(dat+3);

            /* RGB in RGBA DIBs are pre-multiplied by alpha to AlphaBlend usage. */
            *r = iupALPHAPRE(*r,*a);
            *g = iupALPHAPRE(*g,*a);
            *b = iupALPHAPRE(*b,*a);
          }
        }
      }
      else /* bpp == 8 */
      {
        bits[x] = imgdata[x];
      }
    }

    bits += bmp_line_size;
    imgdata -= data_line_size;    /* iupimage is top down */
  }

  if (make_inactive || (channels == 4 && flat_alpha))
    iupAttribSetStr(ih, "_IUP_BGCOLOR_DEPEND", "1");

  return hBitmap;
}

static HBITMAP winImageCreateBitmask(Ihandle *ih, int invert)
{
  int y, x, mask_line_size,data_line_size, colors_count, set,
      width = ih->currentwidth,
      height = ih->currentheight,
      channels = iupAttribGetInt(ih, "CHANNELS"),
      bpp = iupAttribGetInt(ih, "BPP");
  unsigned char *imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");
  HBITMAP hBitmap;
  BYTE* bitmask, *bitmask_ptr;
  iupColor colors[256];

  if (bpp == 8)
    iupImageInitColorTable(ih, colors, &colors_count);

  mask_line_size = ((width + 15) / 16) * 2;      /* WORD aligned, 2 bytes boundary in a 1 bpp image */
  data_line_size = width*channels;

  bitmask = malloc(height * mask_line_size);
  memset(bitmask, 0, height * mask_line_size); /* opaque */

  /* mask and iupimage are top down */

  bitmask_ptr = bitmask;
  for (y=0; y<height; y++)
  {
    for (x=0; x<width; x++)
    {
      set = 0;

      if (bpp == 32)
      {
        BYTE* dat = &imgdata[channels*x];
        if (*(dat+3) == 0) /* full transparent alpha */
          set = 1;
      }
      else if (bpp == 8)
      {
        unsigned char index = imgdata[x];
        if (colors[index].a == 0) /* full transparent alpha */
          set = 1;
      }

      if (set)
        bitmask_ptr[x/8] |= 1 << (7 - (x % 8)); /* set transparent mask bit */
    }

    bitmask_ptr += mask_line_size;
    imgdata += data_line_size;
  }

  if (invert)
  {
    int k, size = height * mask_line_size;
    for (k = 0; k < size; k++)
      bitmask[k] = ~bitmask[k];
  }
  
  hBitmap = CreateBitmap(width, height, 1, 1, bitmask);
  free(bitmask);
  return hBitmap;
}

static HICON winImageCreateIcon(Ihandle *ih, int is_cursor)
{
  HBITMAP hBitmap, hBitmapMask;
  ICONINFO iconinfo;
  HICON icon;
  char* color0 = NULL;

  /* If cursor and no transparency defined, assume 0 if transparent. 
     We do this only in Windows and because of backward compatibility. */
  if (is_cursor)
  {
    int bpp = iupAttribGetInt(ih, "BPP");
    if (bpp == 8)
    {
      if (!iupStrEqual(iupAttribGet(ih, "0"), "BGCOLOR") &&
          !iupStrEqual(iupAttribGet(ih, "1"), "BGCOLOR") &&
          !iupStrEqual(iupAttribGet(ih, "2"), "BGCOLOR"))
      {
        color0 = iupStrDup(iupAttribGet(ih, "0"));
        iupAttribSetStr(ih, "0", "BGCOLOR");
      }
    }
  }
   
  hBitmap = iupdrvImageCreateImage(ih, NULL, 0);
  if (!hBitmap) 
  {
    if (color0) free(color0);
    return NULL;
  }

  /* Transparency mask */
  hBitmapMask = winImageCreateBitmask(ih, 0);
  if (!hBitmapMask)
  {
    DeleteObject(hBitmap);
    if (color0) free(color0);
    return NULL;
  }
 
  /* destination = (destination AND bitmask) XOR bitmap */
  iconinfo.hbmMask = hBitmapMask;   /* AND */
  iconinfo.hbmColor = hBitmap;      /* XOR */

  if (is_cursor)
  {
    int x=0,y=0;
    iupStrToIntInt(iupAttribGet(ih, "HOTSPOT"), &x, &y, ':');

    iconinfo.xHotspot = x;
    iconinfo.yHotspot = y;
    iconinfo.fIcon = FALSE;
  }
  else
    iconinfo.fIcon = TRUE;
  
  icon = CreateIconIndirect(&iconinfo);

  DeleteObject(hBitmap);
  DeleteObject(hBitmapMask);

  if (color0)
  {
    iupAttribStoreStr(ih, "0", color0);
    free(color0);
  }
  
  return icon;
}

void* iupdrvImageCreateIcon(Ihandle *ih)
{
  return winImageCreateIcon(ih, 0);
}

void* iupdrvImageCreateCursor(Ihandle *ih)
{
  return winImageCreateIcon(ih, 1);
}

void* iupdrvImageCreateMask(Ihandle *ih)
{
  int invert = 1;
  if (!ih) return NULL;
  if (iupAttribGet(ih, "_IUPIMG_NO_INVERT")) invert = 0;
  return (void*)winImageCreateBitmask(ih, invert);
}

void* iupdrvImageLoad(const char* name, int type)
{
  int iup2win[3] = {IMAGE_BITMAP, IMAGE_ICON, IMAGE_CURSOR};
  HANDLE hImage = LoadImage(iupwin_hinstance, (LPCTSTR)name, iup2win[type], 0, 0, type==0?LR_CREATEDIBSECTION:0);
  if (!hImage && iupwin_dll_hinstance)
    hImage = LoadImage(iupwin_dll_hinstance, (LPCTSTR)name, iup2win[type], 0, 0, type==0?LR_CREATEDIBSECTION:0);
  if (!hImage)
    hImage = LoadImage(NULL, (LPCTSTR)name, iup2win[type], 0, 0, LR_LOADFROMFILE|(type==0?LR_CREATEDIBSECTION:0));
  return hImage;
}

int iupdrvImageGetInfo(void* handle, int *w, int *h, int *bpp)
{
  BITMAP bm;
  if (!GetObject((HBITMAP)handle, sizeof(BITMAP), (LPSTR)&bm))
  {
    if (w) *w = 0;
    if (h) *h = 0;
    if (bpp) *bpp = 0;
    return 0;
  }
  if (w) *w = bm.bmWidth;
  if (h) *h = abs(bm.bmHeight);
  if (bpp) *bpp = iupImageNormBpp(bm.bmBitsPixel*bm.bmPlanes);
  return 1;
}

void iupdrvImageDestroy(void* handle, int type)
{
  switch (type)
  {
  case IUPIMAGE_IMAGE:
    if (GetObjectType((HBITMAP)handle)==OBJ_BITMAP)
      DeleteObject((HBITMAP)handle);
    else
      GlobalFree((HANDLE)handle);
    break;
  case IUPIMAGE_ICON:
    DestroyIcon((HICON)handle);
    break;
  case IUPIMAGE_CURSOR:
    DestroyCursor((HCURSOR)handle);
    break;
  }
}

