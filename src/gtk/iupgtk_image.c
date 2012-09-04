/** \file
 * \brief Image Resource.
 *
 * See Copyright Notice in "iup.h"
 */

#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_image.h"
#include "iup_drvinfo.h"

#include "iupgtk_drv.h"


void iupdrvImageGetRawData(void* handle, unsigned char* imgdata)
{
  GdkPixbuf* pixbuf = (GdkPixbuf*)handle;
  int w, h, y, x, bpp;
  guchar *pixdata, *pixline_data;
  int rowstride, channels, planesize;
  unsigned char *r, *g, *b, *a;

  if (!iupdrvImageGetInfo(handle, &w, &h, &bpp))
    return;

  if (bpp==8)
    return;

  pixdata = gdk_pixbuf_get_pixels(pixbuf);
  rowstride = gdk_pixbuf_get_rowstride(pixbuf);
  channels = gdk_pixbuf_get_n_channels(pixbuf);

  /* planes are separated in imgdata */
  planesize = w*h;
  r = imgdata;
  g = imgdata+planesize;
  b = imgdata+2*planesize;
  a = imgdata+3*planesize;
  for (y=0; y<h; y++)
  {
    int lineoffset = (h-1 - y)*w;  /* imgdata is bottom up */
    pixline_data = pixdata + y * rowstride;
    for(x=0;x<w;x++)
    {
      int pos = x*channels;
      r[lineoffset+x] = pixline_data[pos];
      g[lineoffset+x] = pixline_data[pos+1];
      b[lineoffset+x] = pixline_data[pos+2];

      if (bpp == 32)
        a[lineoffset+x] = pixline_data[pos+3];
    }
  }
}

void* iupdrvImageCreateImageRaw(int width, int height, int bpp, iupColor* colors, int colors_count, unsigned char *imgdata)
{
  GdkPixbuf* pixbuf;
  guchar *pixdata, *pixline_data;
  int rowstride, channels;
  unsigned char *line_data;
  int x, y, has_alpha = (bpp==32);
  (void)colors_count;

  pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, has_alpha, 8, width, height);
  if (!pixbuf)
    return NULL;

  pixdata = gdk_pixbuf_get_pixels(pixbuf);
  rowstride = gdk_pixbuf_get_rowstride(pixbuf);
  channels = gdk_pixbuf_get_n_channels(pixbuf);

  /* GdkPixbuf is top-bottom */
  /* imgdata is bottom up */

  if (bpp == 8)
  {
    for (y=0; y<height; y++)
    {
      pixline_data = pixdata + y * rowstride;
      line_data = imgdata + (height-1 - y) * width;  /* imgdata is bottom up */

      for (x=0; x<width; x++)
      {
        unsigned char index = line_data[x];
        iupColor* c = &colors[index];
        guchar *r = &pixline_data[channels*x],
               *g = r+1,
               *b = g+1;

        *r = c->r;
        *g = c->g;
        *b = c->b;
      }
    }
  }
  else /* bpp == 32 or bpp == 24 */
  {
    /* planes are separated in imgdata */
    int planesize = width*height;
    unsigned char *r = imgdata,
                  *g = imgdata+planesize,
                  *b = imgdata+2*planesize,
                  *a = imgdata+3*planesize;
    for (y=0; y<height; y++)
    {
      int lineoffset = (height-1 - y)*width;  /* imgdata is bottom up */
      pixline_data = pixdata + y * rowstride;
      for(x=0;x<width;x++)
      {
        int pos = x*channels;
        pixline_data[pos] = r[lineoffset+x];
        pixline_data[pos+1] = g[lineoffset+x];
        pixline_data[pos+2] = b[lineoffset+x];

        if (bpp == 32)
          pixline_data[pos+3] = a[lineoffset+x];
      }
    }
  }

  return pixbuf;
}

void* iupdrvImageCreateImage(Ihandle *ih, const char* bgcolor, int make_inactive)
{
  GdkPixbuf* pixbuf;
  guchar *pixdata, *pixline_data;
  int rowstride, channels;
  unsigned char *imgdata, *line_data, bg_r=0, bg_g=0, bg_b=0;
  int x, y, i, bpp, colors_count = 0, has_alpha = 0;
  iupColor colors[256];

  bpp = iupAttribGetInt(ih, "BPP");

  if (bpp == 8)
    has_alpha = iupImageInitColorTable(ih, colors, &colors_count);
  else if (bpp == 32)
    has_alpha = 1;

  pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, has_alpha, 8, ih->currentwidth, ih->currentheight);
  if (!pixbuf)
    return NULL;

  pixdata = gdk_pixbuf_get_pixels(pixbuf);
  rowstride = gdk_pixbuf_get_rowstride(pixbuf);
  channels = gdk_pixbuf_get_n_channels(pixbuf);
  imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");

  if (make_inactive)  
    iupStrToRGB(bgcolor, &bg_r, &bg_g, &bg_b);

  if (bpp == 8)
  {
    if (make_inactive)
    {
      for (i=0;i<colors_count;i++)
      {
        if (colors[i].a == 0)
        {
          colors[i].r = bg_r;
          colors[i].g = bg_g;
          colors[i].b = bg_b;
          colors[i].a = 255;
        }

        iupImageColorMakeInactive(&(colors[i].r), &(colors[i].g), &(colors[i].b), 
                                  bg_r, bg_g, bg_b);
      }
    }

    for (y=0; y<ih->currentheight; y++)
    {
      pixline_data = pixdata + y * rowstride;
      line_data = imgdata + y * ih->currentwidth;

      for (x=0; x<ih->currentwidth; x++)
      {
        unsigned char index = line_data[x];
        iupColor* c = &colors[index];
        guchar *r = &pixline_data[channels*x],
               *g = r+1,
               *b = g+1,
               *a = b+1;

        *r = c->r;
        *g = c->g;
        *b = c->b;

        if (has_alpha)
          *a = c->a;
      }
    }
  }
  else /* bpp == 32 or bpp == 24 */
  {
    for (y=0; y<ih->currentheight; y++)
    {
      pixline_data = pixdata + y * rowstride;
      line_data = imgdata + y * ih->currentwidth*channels;

      memcpy(pixline_data, line_data, ih->currentwidth*channels);

      if (make_inactive)
      {
        for (x=0; x<ih->currentwidth; x++)
        {
          guchar *r = &pixline_data[channels*x],
                 *g = r+1,
                 *b = g+1,
                 *a = b+1;

          if (has_alpha)
          {
            if (*a != 255)
            {
              *r = iupALPHABLEND(*r, bg_r, *a);
              *g = iupALPHABLEND(*g, bg_g, *a);
              *b = iupALPHABLEND(*b, bg_b, *a);
            }
            else
              *a = 255;
          }

          iupImageColorMakeInactive(r, g, b, 
                                    bg_r, bg_g, bg_b);
        }
      }
    }
  }

  return pixbuf;
}

void* iupdrvImageCreateIcon(Ihandle *ih)
{
  return iupdrvImageCreateImage(ih, NULL, 0);
}

void* iupdrvImageCreateCursor(Ihandle *ih)
{
  GdkCursor *cursor;
  int hx, hy, bpp;

  hx=0; hy=0;
  iupStrToIntInt(iupAttribGet(ih, "HOTSPOT"), &hx, &hy, ':');

  bpp = iupAttribGetInt(ih, "BPP");

  if (bpp == 8 && !iupAttribGet(ih, "3"))
  {
    GdkPixmap *source, *mask;
    GdkColor fg, bg;
    unsigned char r, g, b;
    char *sbits, *mbits, *sb, *mb;
    int y, x, line_size = (ih->currentwidth+7)/8;
    int size_bytes = line_size*ih->currentheight;
    unsigned char* imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");

    r = 255; g = 255; b = 255;
    iupStrToRGB(iupAttribGet(ih, "1"), &r, &g, &b );
    iupgdkColorSet(&fg, r, g, b);

    r = 0; g = 0; b = 0;
    iupStrToRGB(iupAttribGet(ih, "2"), &r, &g, &b );
    iupgdkColorSet(&bg, r, g, b);

    sbits = (char*)malloc(2*size_bytes);
    if (!sbits) return NULL;
    memset(sbits, 0, 2*size_bytes);
    mbits = sbits + size_bytes;

    sb = sbits;
    mb = mbits;
    for (y=0; y<ih->currentheight; y++)
    {
      for (x=0; x<ih->currentwidth; x++)
      {
        int byte = x/8;
        int bit = x%8;
        int index = (int)imgdata[y*ih->currentwidth+x];
        /* index==0 is transparent */
        if (index == 1)
          sb[byte] = (char)(sb[byte] | (1<<bit));
        if (index != 0)
          mb[byte] = (char)(mb[byte] | (1<<bit));
      }

      sb += line_size;
      mb += line_size;
    }
  
    source = gdk_bitmap_create_from_data(NULL, sbits, ih->currentwidth, ih->currentheight);
    mask = gdk_bitmap_create_from_data(NULL, mbits, ih->currentwidth, ih->currentheight);

    cursor = gdk_cursor_new_from_pixmap(source, mask, &fg, &bg, hx, hy);

    gdk_pixmap_unref(source);
    gdk_pixmap_unref(mask);
    free(sbits);
  }
  else
  {
    GdkPixbuf* pixbuf = iupdrvImageCreateImage(ih, NULL, 0);
    cursor = gdk_cursor_new_from_pixbuf(gdk_display_get_default(), pixbuf, hx, hy);
    g_object_unref(pixbuf);
  }

  return cursor;
}

void* iupdrvImageCreateMask(Ihandle *ih)
{
  int bpp;
  GdkPixmap *mask;
  char *bits, *sb;
  int y, x, line_size = (ih->currentwidth+7)/8;
  int size_bytes = line_size*ih->currentheight;
  unsigned char* imgdata = (unsigned char*)iupAttribGetStr(ih, "WID");
  unsigned char colors[256];

  bpp = iupAttribGetInt(ih, "BPP");
  if (bpp > 8)
    return NULL;

  bits = (char*)malloc(size_bytes);
  if (!bits) return NULL;
  memset(bits, 0, size_bytes);

  iupImageInitNonBgColors(ih, colors);

  sb = bits;
  for (y=0; y<ih->currentheight; y++)
  {
    for (x=0; x<ih->currentwidth; x++)
    {
      int byte = x/8;
      int bit = x%8;
      int index = (int)imgdata[y*ih->currentwidth+x];
      if (colors[index])
        sb[byte] = (char)(sb[byte] | (1<<bit));
    }

    sb += line_size;
  }

  mask = gdk_bitmap_create_from_data(NULL, bits, ih->currentwidth, ih->currentheight);

  free(bits);

  return mask;
}

void* iupdrvImageLoad(const char* name, int type)
{
  if (type == IUPIMAGE_CURSOR)
#if GTK_CHECK_VERSION(2, 8, 0)
    return gdk_cursor_new_from_name(gdk_display_get_default(), name);
#else
    return NULL;
#endif
  else
  {
    GdkPixbuf *pixbuf = NULL;

    GtkIconTheme* icon_theme = gtk_icon_theme_get_default();
    if (icon_theme)
    {
      GError *error = NULL;
      pixbuf = gtk_icon_theme_load_icon(icon_theme, name,
                                        24, /* size */
                                        0,  /* flags */
                                        &error);
      if (error)
        g_error_free(error);
    }

    if (!pixbuf)
    {
      GtkIconSet* icon_set = gtk_icon_factory_lookup_default(name);
      if (icon_set)
      {
        GtkStyle* style = gtk_style_new();
        pixbuf = gtk_icon_set_render_icon(icon_set, style, GTK_TEXT_DIR_NONE, GTK_STATE_NORMAL,
                                          GTK_ICON_SIZE_LARGE_TOOLBAR, NULL, NULL);
        g_object_unref(style);
      }
    }

    if (!pixbuf)
    {
      GError *error = NULL;
      pixbuf = gdk_pixbuf_new_from_file(name, &error);    
      if (error)
        g_error_free(error);
    }

    return pixbuf;
  }
}

int iupdrvImageGetInfo(void* handle, int *w, int *h, int *bpp)
{
  GdkPixbuf* pixbuf = (GdkPixbuf*)handle;
  if (!GDK_IS_PIXBUF(pixbuf)) 
  {
    if (w) *w = 0;
    if (h) *h = 0;
    if (bpp) *bpp = 0;
    return 0;
  }
  if (w) *w = gdk_pixbuf_get_width(pixbuf);
  if (h) *h = gdk_pixbuf_get_height(pixbuf);
  if (bpp) *bpp = iupImageNormBpp(gdk_pixbuf_get_bits_per_sample(pixbuf)*gdk_pixbuf_get_n_channels(pixbuf));
  return 1;
}

int iupdrvImageGetRawInfo(void* handle, int *w, int *h, int *bpp, iupColor* colors, int *colors_count)
{
  /* GdkPixbuf are only 24 bpp or 32 bpp */
  (void)colors;
  (void)colors_count;
  return iupdrvImageGetInfo(handle, w, h, bpp);
}

void iupdrvImageDestroy(void* handle, int type)
{
  if (type == IUPIMAGE_CURSOR)
    gdk_cursor_unref((GdkCursor*)handle);
  else
    g_object_unref(handle);
}
