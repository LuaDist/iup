/** \file
 * \brief iupim utilities
 *
 * See Copyright Notice in "iup.h"
 */

#include <im.h>
#include <im_convert.h>
#include <im_counter.h>
#include <im_util.h>
#include <im_image.h>

#include "iup.h"
#include "iupim.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup_object.h"
#include "iup_assert.h"
#include "iup_str.h"
#include "iup_image.h"


static void iSaveErrorMsg(int error)
{
  char* lang = IupGetLanguage();
  char* msg;
  if (iupStrEqualNoCase(lang, "ENGLISH"))
  {
    switch (error)
    {
    case IM_ERR_NONE:
      msg = NULL;
    case IM_ERR_OPEN:
      msg = "Error Opening Image File.\n";
      break;
    case IM_ERR_MEM:
      msg = "Insuficient memory.\n";
      break;
    case IM_ERR_ACCESS:
      msg = "Error Accessing Image File.\n";
      break;
    case IM_ERR_DATA:
      msg = "Image type not Suported.\n";
      break;
    case IM_ERR_FORMAT:
      msg = "Invalid Image File Format.\n";
      break;
    case IM_ERR_COMPRESS:
      msg = "Invalid or unsupported compression.\n";
      break;
    default:
      msg = "Unknown Error.\n";
    }
  }
  else
  {
    switch (error)
    {
    case IM_ERR_NONE:
      msg = NULL;
    case IM_ERR_OPEN:
      msg = "Erro Abrindo Arquivo de Imagem.\n";
      break;
    case IM_ERR_MEM:
      msg = "Memória Insuficiente.\n";
      break;
    case IM_ERR_ACCESS:
      msg = "Erro Acessando Arquivo de Imagem.\n";
      break;
    case IM_ERR_DATA:
      msg = "Tipo de Imagem não Suportado.\n";
      break;
    case IM_ERR_FORMAT:
      msg = "Formato de Arquivo de Imagem Inválido.\n";
      break;
    case IM_ERR_COMPRESS:
      msg = "Compressão Inválida ou não Suportada.\n";
      break;
    default:
      msg = "Erro Desconhecido.\n";
    }
  }

  IupSetGlobal("IUPIM_LASTERROR", msg);
}

Ihandle* IupLoadImage(const char* file_name)
{
  long palette[256];
  int i, error, width, height, color_mode, flags,
      data_type, palette_count, has_alpha = 0;
  Ihandle* iup_image = NULL;
  const unsigned char* transp_index;
  void* image_data = NULL;
  imCounterCallback old_callback;
  imFile* ifile;

  iupASSERT(file_name);
  if (!file_name)
    return NULL;

  old_callback = imCounterSetCallback(NULL, NULL);

  ifile = imFileOpen(file_name, &error);
  if (error)
    goto load_finish;

  error = imFileReadImageInfo(ifile, 0, &width, &height, &color_mode, &data_type);
  if (error)
    goto load_finish;

  flags = IM_TOPDOWN;
  flags |= IM_PACKED;
  if (imColorModeHasAlpha(color_mode))
  {
    has_alpha = 1;
    flags |= IM_ALPHA;
  }

  color_mode = imColorModeToBitmap(color_mode);
  data_type = IM_BYTE;

  image_data = malloc(imImageDataSize(width, height, flags|color_mode, data_type));
  if (!image_data)
    goto load_finish;

  error = imFileReadImageData(ifile, image_data, 1, flags);
  if (error)
    goto load_finish;

  if (color_mode == IM_RGB)
  {
    if (has_alpha)
      iup_image = IupImageRGBA(width, height, (unsigned char*)image_data);
    else
      iup_image = IupImageRGB(width, height, (unsigned char*)image_data);
    palette_count = 0;
  }
  else
  {
    imFileGetPalette(ifile, palette, &palette_count);
    iup_image = IupImage(width, height, (unsigned char*)image_data);
  }

  for (i = 0; i < palette_count; i++)
  {
    char attr[6], color[30];
    unsigned char r, g, b;

    sprintf(attr, "%d", i);
    imColorDecode(&r, &g, &b, palette[i]);
    sprintf(color, "%d %d %d", (int)r, (int)g, (int)b);

    IupStoreAttribute(iup_image, attr, color); 
  }

  transp_index = imFileGetAttribute(ifile, "TransparencyIndex", NULL, NULL);
  if (transp_index)
  {
    char attr[6];
    sprintf(attr, "%d", (int)(*transp_index));
    IupSetAttribute(iup_image, attr, "BGCOLOR"); 
  }

load_finish:
  imCounterSetCallback(NULL, old_callback);
  if (ifile) imFileClose(ifile);
  if (image_data) free(image_data);
  iSaveErrorMsg(error);
  return iup_image;
}

int IupSaveImage(Ihandle* ih, const char* file_name, const char* format)
{
  int width, height, i, bpp;
  unsigned char* data;
  int error;
  long palette[256];
  imFile* ifile;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return 0;

  iupASSERT(file_name);
  if (!file_name)
    return 0;

  ifile = imFileNew(file_name, format, &error);
  if (!ifile)
  {
    iSaveErrorMsg(error);
    return 0;
  }

  data = (unsigned char*)IupGetAttribute(ih, "WID");

  width = IupGetInt(ih, "WIDTH");
  height = IupGetInt(ih, "HEIGHT");
  bpp = IupGetInt(ih, "BPP");

  if (bpp == 24)
    error = imFileWriteImageInfo(ifile, width, height, IM_RGB|IM_TOPDOWN|IM_PACKED, IM_BYTE);
  else if (bpp == 32)
    error = imFileWriteImageInfo(ifile, width, height, IM_RGB|IM_TOPDOWN|IM_PACKED|IM_ALPHA, IM_BYTE);
  else /* bpp == 8 */
  {
    for(i = 0; i < 256; i++)
    {
      unsigned int r, g, b;
      char str[20];
      char* color;

      sprintf(str, "%d", i);
      color = IupGetAttribute(ih, str);
      if (!color)
        break;

      if (strcmp(color, "BGCOLOR") == 0)
      {
        unsigned char transp_index = (unsigned char)i;
        imFileSetAttribute(ifile, "TransparencyIndex", IM_BYTE, 1, &transp_index);
        palette[i] = imColorEncode(0, 0, 0);
      }
      else
      {
        sscanf(color, "%d %d %d", &r, &g, &b);
        palette[i] = imColorEncode((unsigned char)r, (unsigned char)g, (unsigned char)b);
      }
    }

    imFileSetPalette(ifile, palette, i);

    error = imFileWriteImageInfo(ifile, width, height, IM_MAP|IM_TOPDOWN, IM_BYTE);
  }

  if (error == IM_ERR_NONE)
    error = imFileWriteImageData(ifile, data);

  imFileClose(ifile); 

  iSaveErrorMsg(error);

  return error == IM_ERR_NONE? 1: 0;
}

/******************************************************************************/

static void iInitColors(iupColor* colors, int count, long* palette)
{
  int i;
  for (i=0; i< count; i++)
  {
    imColorDecode(&(colors[i].r), &(colors[i].g), &(colors[i].b), palette[i]);
    colors[i].a = 0;
  }
}

static void iInitPalette(long* palette, int count, iupColor* colors)
{
  int i;
  for (i=0; i< count; i++)
  {
    palette[i] = imColorEncode(colors[i].r, colors[i].g, colors[i].b);
  }
}

void* IupGetImageNativeHandle(imImage* image)
{
  int bpp = image->depth*8;
  iupColor colors[256];
  iInitColors(colors, image->palette_count, image->palette);
  if (image->has_alpha && bpp == 24)
    bpp = 32;
  return iupdrvImageCreateImageRaw(image->width, image->height, bpp, colors, image->palette_count, image->data[0]);
}

imImage* IupGetNativeHandleImage(void* handle)
{
  int width, height, bpp;
  iupColor colors[256];
  int colors_count = 0;
  if (iupdrvImageGetRawInfo(handle, &width, &height, &bpp, colors, &colors_count))
  {
    imImage* image = imImageCreate(width, height, bpp>8? IM_RGB: IM_MAP, IM_BYTE);
    if (image)
    {
      if (bpp==32)
        imImageAddAlpha(image);

      if (bpp<=8 && colors_count)
      {
        long* palette = (long*)malloc(256*sizeof(long));
        iInitPalette(palette, colors_count, colors);
        imImageSetPalette(image, palette, colors_count);
      }

      iupdrvImageGetRawData(handle, image->data[0]);
        
      return image;
    }

    iupdrvImageDestroy(handle, IUPIMAGE_IMAGE);
  }
  return NULL;
}
