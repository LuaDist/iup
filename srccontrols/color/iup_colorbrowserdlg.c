/** \file
 * \brief IupColorDlg pre-defined dialog control
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"
#include "iupcontrols.h"

#include <cd.h>
#include <cdiup.h>
#include <cddbuf.h>
#include <cdirgb.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_strmessage.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_controls.h"
#include "iup_cdutil.h"
#include "iup_register.h"
#include "iup_register.h"
#include "iup_image.h"
#include "iup_colorhsi.h"
#include "iup_childtree.h"

       
const char* default_colortable_cells[20] = 
{
  "0 0 0", "64 64 64", "128 128 128", "144 144 144", "0 128 128", "128 0 128", "128 128 0", "128 0 0", "0 128 0", "0 0 128",
  "255 255 255", "240 240 240", "224 224 224", "192 192 192", "0 255 255", "255 0 255", "255 255 0", "255 0 0", "0 255 0", "0 0 255"
};

typedef struct _IcolorDlgData
{
  int status;

  long previous_color, color;

  float hue, saturation, intensity;
  unsigned char red, green, blue, alpha;

  Ihandle *red_txt, *green_txt, *blue_txt, *alpha_txt;
  Ihandle *hue_txt, *intensity_txt, *saturation_txt;
  Ihandle *color_browser, *color_cnv, *colorhex_txt;
  Ihandle *colortable_cbar, *alpha_val;
  Ihandle *help_bt;

  cdCanvas* color_cdcanvas, *color_cddbuffer;
} IcolorDlgData;


static void iColorBrowserDlgColorCnvRepaint(IcolorDlgData* colordlg_data)
{
  int x, y, w, h, width, height, box_size = 10;

  if (!colordlg_data->color_cddbuffer)
    return;

  cdCanvasGetSize(colordlg_data->color_cddbuffer, &width, &height, NULL, NULL);

  cdCanvasBackground(colordlg_data->color_cddbuffer, CD_WHITE);
  cdCanvasClear(colordlg_data->color_cddbuffer);

  w = (width+box_size-1)/box_size;
  h = (height+box_size-1)/box_size;

  cdCanvasForeground(colordlg_data->color_cddbuffer, CD_GRAY);

  for (y = 0; y < h; y++)
  {                              
    for (x = 0; x < w; x++)
    {
      if (((x%2) && (y%2)) || (((x+1)%2) && ((y+1)%2)))
      {
        int xmin, xmax, ymin, ymax;

        xmin = x*box_size;
        xmax = xmin+box_size;
        ymin = y*box_size;
        ymax = ymin+box_size;

        cdCanvasBox(colordlg_data->color_cddbuffer, xmin, xmax, ymin, ymax);
      }
    }
  }

  cdCanvasForeground(colordlg_data->color_cddbuffer, colordlg_data->previous_color);
  cdCanvasBox(colordlg_data->color_cddbuffer, 0, width/2, 0, height);

  cdCanvasForeground(colordlg_data->color_cddbuffer, colordlg_data->color);
  cdCanvasBox(colordlg_data->color_cddbuffer, width/2+1, width, 0, height);

  cdCanvasFlush(colordlg_data->color_cddbuffer);
}

static void iColorBrowserDlgHSI2RGB(IcolorDlgData* colordlg_data)
{
  iupColorHSI2RGB(colordlg_data->hue, colordlg_data->saturation, colordlg_data->intensity,
                  &colordlg_data->red, &colordlg_data->green, &colordlg_data->blue);
}

static void iColorBrowserDlgRGB2HSI(IcolorDlgData* colordlg_data)
{
  iupColorRGB2HSI(colordlg_data->red, colordlg_data->green, colordlg_data->blue,
                  &(colordlg_data->hue), &(colordlg_data->saturation), &(colordlg_data->intensity));
}

static void iColorBrowserDlgHex_TXT_Update(IcolorDlgData* colordlg_data)
{
  IupSetfAttribute(colordlg_data->colorhex_txt, "VALUE", "#%02X%02X%02X", (int)colordlg_data->red, (int)colordlg_data->green, (int)colordlg_data->blue);
}

static int iupStrHexToRGB(const char *str, unsigned char *r, unsigned char *g, unsigned char *b)
{
  unsigned int ri = 0, gi = 0, bi = 0;
  if (!str) return 0;
  if (sscanf(str, "#%2X%2X%2X", &ri, &gi, &bi) != 3) return 0;
  if (ri > 255 || gi > 255 || bi > 255) return 0;
  *r = (unsigned char)ri;
  *g = (unsigned char)gi;
  *b = (unsigned char)bi;
  return 1;
}

/*************************************************\
* Updates text fields with the current HSI values *
\*************************************************/
static void iColorBrowserDlgHSI_TXT_Update(IcolorDlgData* colordlg_data)
{
  IupSetfAttribute(colordlg_data->hue_txt, "VALUE", "%d", iupROUND(colordlg_data->hue));
  IupSetfAttribute(colordlg_data->saturation_txt, "VALUE", "%d", iupROUND(colordlg_data->saturation * 100));
  IupSetfAttribute(colordlg_data->intensity_txt, "VALUE", "%d", iupROUND(colordlg_data->intensity * 100));
}

/*************************************************\
* Updates text fields with the current RGB values *
\*************************************************/
static void iColorBrowserDlgRGB_TXT_Update(IcolorDlgData* colordlg_data)
{
  IupSetfAttribute(colordlg_data->red_txt, "VALUE", "%d", (int) colordlg_data->red);
  IupSetfAttribute(colordlg_data->green_txt, "VALUE", "%d", (int) colordlg_data->green);
  IupSetfAttribute(colordlg_data->blue_txt, "VALUE", "%d", (int) colordlg_data->blue);
}

static void iColorBrowserDlgBrowserRGB_Update(IcolorDlgData* colordlg_data)
{
  IupSetfAttribute(colordlg_data->color_browser, "RGB", "%d %d %d", colordlg_data->red, colordlg_data->green, colordlg_data->blue);
}

static void iColorBrowserDlgBrowserHSI_Update(IcolorDlgData* colordlg_data)
{
  IupSetfAttribute(colordlg_data->color_browser, "HSI", "%g %g %g", (double)colordlg_data->hue, (double)colordlg_data->saturation, (double)colordlg_data->intensity);
}

/*****************************************\
* Sets the RGB color in the Color Canvas *
\*****************************************/
static void iColorBrowserDlgColor_Update(IcolorDlgData* colordlg_data)
{
  colordlg_data->color = cdEncodeColor(colordlg_data->red, colordlg_data->green, colordlg_data->blue);
  colordlg_data->color = cdEncodeAlpha(colordlg_data->color, colordlg_data->alpha);
  iColorBrowserDlgColorCnvRepaint(colordlg_data);
}

static void iColorBrowserDlgHSIChanged(IcolorDlgData* colordlg_data) 
{
  iColorBrowserDlgHSI2RGB(colordlg_data);
  iColorBrowserDlgBrowserHSI_Update(colordlg_data);
  iColorBrowserDlgHex_TXT_Update(colordlg_data);
  iColorBrowserDlgRGB_TXT_Update(colordlg_data);
  iColorBrowserDlgColor_Update(colordlg_data);
}

static void iColorBrowserDlgRGBChanged(IcolorDlgData* colordlg_data) 
{
  iColorBrowserDlgRGB2HSI(colordlg_data);
  iColorBrowserDlgBrowserRGB_Update(colordlg_data);
  iColorBrowserDlgHex_TXT_Update(colordlg_data);
  iColorBrowserDlgHSI_TXT_Update(colordlg_data);
  iColorBrowserDlgColor_Update(colordlg_data);
}

/***********************************************\
* Initializes the default values to the element *
\***********************************************/
static void iColorBrowserDlgInit_Defaults(IcolorDlgData* colordlg_data)
{
  char* str = iupStrGetMemory(100);
  Ihandle* box;
  int i;

  IupSetAttribute(colordlg_data->color_browser, "RGB", "0 0 0");

  IupSetAttribute(colordlg_data->red_txt,   "VALUE", "0");
  IupSetAttribute(colordlg_data->green_txt, "VALUE", "0");
  IupSetAttribute(colordlg_data->blue_txt,  "VALUE", "0");

  IupSetAttribute(colordlg_data->hue_txt,        "VALUE", "0");
  IupSetAttribute(colordlg_data->saturation_txt, "VALUE", "0");
  IupSetAttribute(colordlg_data->intensity_txt,  "VALUE", "0");
  
  IupSetAttribute(colordlg_data->colorhex_txt, "VALUE", "#000000");

  colordlg_data->alpha = 255;
  IupSetAttribute(colordlg_data->alpha_val, "VALUE", "255");
  IupSetAttribute(colordlg_data->alpha_txt, "VALUE", "255");

  box = IupGetParent(colordlg_data->alpha_val);
  IupSetAttribute(box, "FLOATING", "YES");
  IupSetAttribute(box, "VISIBLE", "NO");

  box = IupGetParent(colordlg_data->colortable_cbar);
  IupSetAttribute(box, "FLOATING", "YES");
  IupSetAttribute(box, "VISIBLE", "NO");

  box = IupGetParent(colordlg_data->colorhex_txt);
  IupSetAttribute(box, "FLOATING", "YES");
  IupSetAttribute(box, "VISIBLE", "NO");

  for(i = 0; i < 20; i++)
  {
    sprintf(str, "CELL%d", i);
    IupSetAttribute(colordlg_data->colortable_cbar, str, default_colortable_cells[i]);
  }
}


/**************************************************************************************************************/
/*                                 Internal Callbacks                                                         */
/**************************************************************************************************************/


static int iColorBrowserDlgButtonOK_CB(Ihandle* ih)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  colordlg_data->status = 1;
  return IUP_CLOSE;
}

static int iColorBrowserDlgButtonCancel_CB(Ihandle* ih)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  colordlg_data->status = 0;
  return IUP_CLOSE;
}

static int iColorBrowserDlgButtonHelp_CB(Ihandle* ih)
{
  Icallback cb = IupGetCallback(IupGetDialog(ih), "HELP_CB");
  if (cb && cb(ih) == IUP_CLOSE)
  {
    IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
    colordlg_data->status = 0;
    return IUP_CLOSE;
  }
  return IUP_DEFAULT;
}

static int iColorBrowserDlgColorCnvRedraw_CB(Ihandle* ih)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  if (!colordlg_data->color_cddbuffer)
    return IUP_DEFAULT;

  cdCanvasActivate(colordlg_data->color_cddbuffer);

  iColorBrowserDlgColorCnvRepaint(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgRedAction_CB(Ihandle* ih, int c, char *value) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int vi;

  if (iupStrToInt(value, &vi))
  {
    colordlg_data->red = (unsigned char)vi;
    iColorBrowserDlgRGBChanged(colordlg_data);
  }

  (void)c;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgRedSpin_CB(Ihandle* ih, int vi) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->red = (unsigned char)vi;
  iColorBrowserDlgRGBChanged(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgGreenAction_CB(Ihandle* ih, int c, char *value) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int vi;

  if (iupStrToInt(value, &vi))
  {
    colordlg_data->green = (unsigned char)vi;
    iColorBrowserDlgRGBChanged(colordlg_data);
  }

  (void)c;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgGreenSpin_CB(Ihandle* ih, int vi) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->green = (unsigned char)vi;
  iColorBrowserDlgRGBChanged(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgBlueAction_CB(Ihandle* ih, int c, char *value) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int vi;

  if (iupStrToInt(value, &vi))
  {
    colordlg_data->blue = (unsigned char)vi;
    iColorBrowserDlgRGBChanged(colordlg_data);
  }

  (void)c;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgBlueSpin_CB(Ihandle* ih, int vi) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->blue = (unsigned char)vi;
  iColorBrowserDlgRGBChanged(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgHueAction_CB(Ihandle* ih, int c, char *value) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int vi;

  if (iupStrToInt(value, &vi))
  {
    colordlg_data->hue = (float)vi;
    iColorBrowserDlgHSIChanged(colordlg_data);
  }

  (void)c;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgHueSpin_CB(Ihandle* ih, int vi) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->hue = (float)vi;
  iColorBrowserDlgHSIChanged(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgSaturationAction_CB(Ihandle* ih, int c, char *value) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int vi;

  if (iupStrToInt(value, &vi))
  {
    colordlg_data->saturation = (float)vi/100.0f;
    iColorBrowserDlgHSIChanged(colordlg_data);
  }

  (void)c;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgSaturationSpin_CB(Ihandle* ih, int vi) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->saturation = (float)vi/100.0f;
  iColorBrowserDlgHSIChanged(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgIntensityAction_CB(Ihandle* ih, int c, char *value) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int vi;

  if (iupStrToInt(value, &vi))
  {
    colordlg_data->intensity = (float)vi/100.0f;
    iColorBrowserDlgHSIChanged(colordlg_data);
  }

  (void)c;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgIntensitySpin_CB(Ihandle* ih, int vi) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->intensity = (float)vi/100.0f;
  iColorBrowserDlgHSIChanged(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgHexAction_CB(Ihandle* ih, int c, char* value) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  if (iupStrHexToRGB(value, &colordlg_data->red, &colordlg_data->green, &colordlg_data->blue))
  {
    iColorBrowserDlgRGB2HSI(colordlg_data);
    iColorBrowserDlgBrowserRGB_Update(colordlg_data);
    iColorBrowserDlgHSI_TXT_Update(colordlg_data);
    iColorBrowserDlgRGB_TXT_Update(colordlg_data);
    iColorBrowserDlgColor_Update(colordlg_data);
  }

  (void)c;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgColorSelDrag_CB(Ihandle* ih, unsigned char r, unsigned char g, unsigned char b)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->red   = r;
  colordlg_data->green = g;
  colordlg_data->blue  = b;

  iColorBrowserDlgRGB2HSI(colordlg_data);
  iColorBrowserDlgHex_TXT_Update(colordlg_data);
  iColorBrowserDlgHSI_TXT_Update(colordlg_data);
  iColorBrowserDlgRGB_TXT_Update(colordlg_data);

  colordlg_data->color = cdEncodeColor(colordlg_data->red,colordlg_data->green,colordlg_data->blue);
  colordlg_data->color = cdEncodeAlpha(colordlg_data->color, colordlg_data->alpha);
  iColorBrowserDlgColorCnvRepaint(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgAlphaVal_CB(Ihandle* ih, double val)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->alpha = (unsigned char)val;
  IupSetfAttribute(colordlg_data->alpha_txt, "VALUE", "%d", (int)colordlg_data->alpha);

  colordlg_data->color = cdEncodeAlpha(colordlg_data->color, colordlg_data->alpha);
  iColorBrowserDlgColorCnvRepaint(colordlg_data);

  return IUP_DEFAULT;  
}

static int iColorBrowserDlgAlphaAction_CB(Ihandle* ih, int c, char* value) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int vi;

  if (iupStrToInt(value, &vi))
  {
    colordlg_data->alpha = (unsigned char)vi;
    IupSetfAttribute(colordlg_data->alpha_val, "VALUE", "%d", (int)colordlg_data->alpha);

    colordlg_data->color = cdEncodeAlpha(colordlg_data->color, colordlg_data->alpha);
    iColorBrowserDlgColorCnvRepaint(colordlg_data);
  }

  (void)c;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgAlphaSpin_CB(Ihandle* ih, int vi) 
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  colordlg_data->alpha = (unsigned char)vi;
  IupSetfAttribute(colordlg_data->alpha_val, "VALUE", "%d", (int)colordlg_data->alpha);

  colordlg_data->color = cdEncodeAlpha(colordlg_data->color, colordlg_data->alpha);
  iColorBrowserDlgColorCnvRepaint(colordlg_data);

  return IUP_DEFAULT;
}

static int iColorBrowserDlgColorTableSelect_CB(Ihandle* ih, int cell, int type)
{
  char* str = iupStrGetMemory(30);

  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  sprintf(str, "CELL%d", cell);
  iupStrToRGB(IupGetAttribute(ih, str), &colordlg_data->red, &colordlg_data->green, &colordlg_data->blue);

  iColorBrowserDlgRGB_TXT_Update(colordlg_data);
  iColorBrowserDlgRGBChanged(colordlg_data);

  (void)type;
  return IUP_DEFAULT;
}

static int iColorBrowserDlgColorCnvButton_CB(Ihandle* ih, int b, int press, int x, int y)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int width;
  (void)y;

  if (b != IUP_BUTTON1 || !press || !colordlg_data->color_cddbuffer)
    return IUP_DEFAULT;

  cdCanvasGetSize(colordlg_data->color_cddbuffer, &width, NULL, NULL, NULL);

  if (x < width/2)
  {
    /* reset color to previous */
    colordlg_data->red = cdRed(colordlg_data->previous_color);
    colordlg_data->green = cdGreen(colordlg_data->previous_color);
    colordlg_data->blue = cdBlue(colordlg_data->previous_color);
    colordlg_data->alpha = cdAlpha(colordlg_data->previous_color);

    IupSetfAttribute(colordlg_data->alpha_txt, "VALUE", "%d", (int)colordlg_data->alpha);
    IupSetfAttribute(colordlg_data->alpha_val, "VALUE", "%d", (int)colordlg_data->alpha);

    iColorBrowserDlgRGB_TXT_Update(colordlg_data);
    iColorBrowserDlgRGBChanged(colordlg_data);
  }

  return IUP_DEFAULT;
}

static int iColorBrowserDlgColorCnvMap_CB(Ihandle* ih)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  /* Create Canvas */
  colordlg_data->color_cdcanvas = cdCreateCanvas(CD_IUP, colordlg_data->color_cnv);

  if (!colordlg_data->color_cdcanvas)
    return IUP_DEFAULT;

  /* this can fail if canvas size is zero */
  colordlg_data->color_cddbuffer = cdCreateCanvas(CD_DBUFFERRGB, colordlg_data->color_cdcanvas);
  return IUP_DEFAULT;
}

static int iColorBrowserDlgColorCnvUnMap_CB(Ihandle* ih)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  if (colordlg_data->color_cddbuffer)
  {
    cdKillCanvas(colordlg_data->color_cddbuffer);
    colordlg_data->color_cddbuffer = NULL;
  }

  if (colordlg_data->color_cdcanvas)
  {
    cdKillCanvas(colordlg_data->color_cdcanvas);
    colordlg_data->color_cdcanvas = NULL;
  }

  return IUP_DEFAULT;
}


/**************************************************************************************************************/
/*                                     Attributes                                                             */
/**************************************************************************************************************/


static char* iColorBrowserDlgGetStatusAttrib(Ihandle* ih)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  if (colordlg_data->status) 
    return "1";
  else 
    return NULL;
}

static int iColorBrowserDlgSetShowHelpAttrib(Ihandle* ih, const char* value)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  IupSetAttribute(colordlg_data->help_bt, "VISIBLE", iupStrBoolean(value)? "YES": "NO");
  return 1;
}

static int iColorBrowserDlgSetShowHexAttrib(Ihandle* ih, const char* value)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  /* valid only before map */
  if (ih->handle)
    return 1;

  if (iupStrBoolean(value))
  {
    Ihandle* box = IupGetParent(colordlg_data->colorhex_txt);
    IupSetAttribute(box, "FLOATING", NULL);
    IupSetAttribute(box, "VISIBLE", "YES");
  }

  return 1;
}

static int iColorBrowserDlgSetShowColorTableAttrib(Ihandle* ih, const char* value)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  /* valid only before map */
  if (ih->handle)
    return 1;

  if (iupStrBoolean(value))
  {
    Ihandle* box = IupGetParent(colordlg_data->colortable_cbar);
    IupSetAttribute(box, "FLOATING", NULL);
    IupSetAttribute(box, "VISIBLE", "YES");
  }

  return 1;
}

static int iColorBrowserDlgSetShowAlphaAttrib(Ihandle* ih, const char* value)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  /* valid only before map */
  if (ih->handle)
    return 1;

  if (iupStrBoolean(value))
  {
    Ihandle* box = IupGetParent(colordlg_data->alpha_val);
    IupSetAttribute(box, "FLOATING", NULL);
    IupSetAttribute(box, "VISIBLE", "YES");
  }

  return 1;
}

static int iColorBrowserDlgSetAlphaAttrib(Ihandle* ih, const char* value)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int alpha;
  if (iupStrToInt(value, &alpha))
  {
    colordlg_data->alpha = (unsigned char)alpha;
    IupSetfAttribute(colordlg_data->alpha_txt, "VALUE", "%d", (int)colordlg_data->alpha);
    IupSetfAttribute(colordlg_data->alpha_val, "VALUE", "%d", (int)colordlg_data->alpha);

    colordlg_data->color = cdEncodeAlpha(colordlg_data->color, colordlg_data->alpha);
    colordlg_data->previous_color = cdEncodeAlpha(colordlg_data->previous_color, colordlg_data->alpha);
    iColorBrowserDlgColorCnvRepaint(colordlg_data);

    if (!ih->handle)  /* do it only before map */
      IupSetAttribute(ih, "SHOWALPHA", "YES");
  }
 
  return 1;
}

static char* iColorBrowserDlgGetAlphaAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  sprintf(buffer, "%d", (int)colordlg_data->alpha);
  return buffer;
}

static int iColorBrowserDlgSetValueAttrib(Ihandle* ih, const char* value)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int ret = iupStrToRGBA(value, &colordlg_data->red, &colordlg_data->green, &colordlg_data->blue, &colordlg_data->alpha);
  if (!ret)
    return 0;
  
  colordlg_data->previous_color = cdEncodeColor(colordlg_data->red, colordlg_data->green, colordlg_data->blue);
  colordlg_data->previous_color = cdEncodeAlpha(colordlg_data->previous_color, colordlg_data->alpha);

  if (ret == 4)
  {
    IupSetfAttribute(colordlg_data->alpha_txt, "VALUE", "%d", (int)colordlg_data->alpha);
    IupSetfAttribute(colordlg_data->alpha_val, "VALUE", "%d", (int)colordlg_data->alpha);

    if (!ih->handle)  /* do it only before map */
      IupSetAttribute(ih, "SHOWALPHA", "YES");
  }

  iColorBrowserDlgRGB_TXT_Update(colordlg_data);
  iColorBrowserDlgRGBChanged(colordlg_data);

  return 0;
}

static char* iColorBrowserDlgGetValueAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  if (iupAttribGetBoolean(ih, "SHOWALPHA"))
    sprintf(buffer, "%d %d %d %d", (int)colordlg_data->red, (int)colordlg_data->green, (int)colordlg_data->blue, (int)colordlg_data->alpha);
  else
    sprintf(buffer, "%d %d %d", (int)colordlg_data->red, (int)colordlg_data->green, (int)colordlg_data->blue);
  return buffer;
}

static int iupStrToHSI_Int(const char *str, int *h, int *s, int *i)
{
  int fh, fs, fi;
  if (!str) return 0;
  if (sscanf(str, "%d %d %d", &fh, &fs, &fi) != 3) return 0;
  if (fh > 359 || fs > 100 || fi > 100) return 0;
  if (fh < 0 || fs < 0 || fi < 0) return 0;
  *h = fh;
  *s = fs;
  *i = fi;
  return 1;
}

static int iColorBrowserDlgSetValueHSIAttrib(Ihandle* ih, const char* value)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  int hue, saturation, intensity;

  if (!iupStrToHSI_Int(value, &hue, &saturation, &intensity))
    return 0;
  
  colordlg_data->hue = (float)hue;
  colordlg_data->saturation = (float)saturation/100.0f;
  colordlg_data->intensity = (float)intensity/100.0f;

  iColorBrowserDlgHSI2RGB(colordlg_data);
  colordlg_data->previous_color = cdEncodeColor(colordlg_data->red, colordlg_data->green, colordlg_data->blue);
  colordlg_data->previous_color = cdEncodeAlpha(colordlg_data->previous_color, colordlg_data->alpha);
 
  iColorBrowserDlgHSIChanged(colordlg_data);
  return 0;
}

static char* iColorBrowserDlgGetValueHSIAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  sprintf(buffer, "%d %d %d", (int)colordlg_data->hue, (int)(colordlg_data->saturation*100), (int)(colordlg_data->intensity*100));
  return buffer;
}

static int iColorBrowserDlgSetValueHexAttrib(Ihandle* ih, const char* value)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  if (!iupStrHexToRGB(value, &colordlg_data->red, &colordlg_data->green, &colordlg_data->blue))
    return 0;

  colordlg_data->previous_color = cdEncodeColor(colordlg_data->red, colordlg_data->green, colordlg_data->blue);
  colordlg_data->previous_color = cdEncodeAlpha(colordlg_data->previous_color, colordlg_data->alpha);

  iColorBrowserDlgRGB2HSI(colordlg_data);
  iColorBrowserDlgBrowserRGB_Update(colordlg_data);
  iColorBrowserDlgHSI_TXT_Update(colordlg_data);
  iColorBrowserDlgRGB_TXT_Update(colordlg_data);
  iColorBrowserDlgColor_Update(colordlg_data);
  return 0;
}

static char* iColorBrowserDlgGetValueHexAttrib(Ihandle* ih)
{
  char* buffer = iupStrGetMemory(100);
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  sprintf(buffer, "#%02X%02X%02X", (int)colordlg_data->red, (int)colordlg_data->green, (int)colordlg_data->blue);
  return buffer;
}

static char* iColorBrowserDlgGetColorTableAttrib(Ihandle* ih)
{
  int i, inc, off = 0;
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  char* color_str, attrib_str[30];
  char* str = iupStrGetMemory(300);
  for (i=0; i < 20; i++)
  {
    sprintf(attrib_str, "CELL%d", i);
    color_str = IupGetAttribute(colordlg_data->colortable_cbar, attrib_str);
    inc = strlen(color_str);
    memcpy(str+off, color_str, inc);
    memcpy(str+off+inc, ";", 1);
    off += inc+1;
  }
  str[off-1] = 0; /* remove last separator */
  return str;
}

static int iColorBrowserDlgSetColorTableAttrib(Ihandle* ih, const char* value)
{
  int i = 0;
  unsigned char r, g, b;
  char str[30];
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");

  if (!ih->handle)    /* do it only before map */
    iColorBrowserDlgSetShowColorTableAttrib(ih, "YES");

  while (value && *value && i < 20)
  {
    if (iupStrToRGB(value, &r, &g, &b))
    {
      sprintf(str, "CELL%d", i);
      IupSetfAttribute(colordlg_data->colortable_cbar, str, "%d %d %d", (int)r, (int)g, (int)b);
    }

    value = strchr(value, ';');
    if (value) value++;
    i++;
  }

  return 1;
}


/**************************************************************************************************************/
/*                                     Methods                                                                */
/**************************************************************************************************************/

static int iColorBrowserDlgMapMethod(Ihandle* ih)
{
  if (!IupGetCallback(ih, "HELP_CB"))
  {
    IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
    IupSetAttribute(colordlg_data->help_bt, "VISIBLE", "NO");
  }

  return IUP_NOERROR;
}

static void iColorBrowserDlgDestroyMethod(Ihandle* ih)
{
  IcolorDlgData* colordlg_data = (IcolorDlgData*)iupAttribGetInherit(ih, "_IUP_GC_DATA");
  free(colordlg_data);
}

static int iColorBrowserDlgCreateMethod(Ihandle* ih, void** params)
{
  Ihandle *ok_bt, *cancel_bt;
  Ihandle *rgb_vb, *hsi_vb, *clr_vb;
  Ihandle *lin1, *lin2, *col1, *col2;

  IcolorDlgData* colordlg_data = (IcolorDlgData*)malloc(sizeof(IcolorDlgData));
  memset(colordlg_data, 0, sizeof(IcolorDlgData));
  iupAttribSetStr(ih, "_IUP_GC_DATA", (char*)colordlg_data);

  /* ======================================================================= */
  /* BUTTONS   ============================================================= */
  /* ======================================================================= */
  ok_bt = IupButton(iupStrMessageGet("IUP_OK"), NULL);                      /* Ok Button */
  IupSetAttribute(ok_bt, "PADDING", "20x0");
  IupSetCallback (ok_bt, "ACTION", (Icallback)iColorBrowserDlgButtonOK_CB);
  IupSetAttributeHandle(ih, "DEFAULTENTER", ok_bt);

  cancel_bt = IupButton(iupStrMessageGet("IUP_CANCEL"), NULL);          /* Cancel Button */
  IupSetAttribute(cancel_bt, "PADDING", "20x0");
  IupSetCallback (cancel_bt, "ACTION", (Icallback)iColorBrowserDlgButtonCancel_CB);
  IupSetAttributeHandle(ih, "DEFAULTESC", cancel_bt);

  colordlg_data->help_bt = IupButton(iupStrMessageGet("IUP_HELP"), NULL);            /* Help Button */
  IupSetAttribute(colordlg_data->help_bt, "PADDING", "20x0");
  IupSetCallback (colordlg_data->help_bt, "ACTION", (Icallback)iColorBrowserDlgButtonHelp_CB);

  /* ======================================================================= */
  /* COLOR   =============================================================== */
  /* ======================================================================= */
  colordlg_data->color_browser = IupColorBrowser();
  IupSetAttribute(colordlg_data->color_browser, "EXPAND", "YES");  
  IupSetCallback(colordlg_data->color_browser, "DRAG_CB",   (Icallback)iColorBrowserDlgColorSelDrag_CB);
  IupSetCallback(colordlg_data->color_browser, "CHANGE_CB", (Icallback)iColorBrowserDlgColorSelDrag_CB);

  colordlg_data->color_cnv = IupCanvas(NULL);  /* Canvas of the color */
  IupSetAttribute(colordlg_data->color_cnv, "SIZE", "x12");
  IupSetAttribute(colordlg_data->color_cnv, "CANFOCUS", "NO");
  IupSetAttribute(colordlg_data->color_cnv, "EXPAND", "HORIZONTAL");
  IupSetCallback (colordlg_data->color_cnv, "ACTION", (Icallback)iColorBrowserDlgColorCnvRedraw_CB);
  IupSetCallback (colordlg_data->color_cnv, "MAP_CB", (Icallback)iColorBrowserDlgColorCnvMap_CB);
  IupSetCallback (colordlg_data->color_cnv, "UNMAP_CB", (Icallback)iColorBrowserDlgColorCnvUnMap_CB);
  IupSetCallback (colordlg_data->color_cnv, "BUTTON_CB", (Icallback)iColorBrowserDlgColorCnvButton_CB);

  colordlg_data->colorhex_txt = IupText(NULL);      /* Hex of the color */
  IupSetAttribute(colordlg_data->colorhex_txt, "VISIBLECOLUMNS", "7");
  IupSetCallback (colordlg_data->colorhex_txt, "ACTION", (Icallback)iColorBrowserDlgHexAction_CB);
  IupSetAttribute(colordlg_data->colorhex_txt, "MASK", "#[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]");

  /* ======================================================================= */
  /* ALPHA TRANSPARENCY   ================================================== */
  /* ======================================================================= */
  colordlg_data->alpha_val = IupVal("HORIZONTAL");
  IupSetAttribute(colordlg_data->alpha_val, "EXPAND", "HORIZONTAL");
  IupSetAttribute(colordlg_data->alpha_val, "MIN", "0");
  IupSetAttribute(colordlg_data->alpha_val, "MAX", "255");
  IupSetAttribute(colordlg_data->alpha_val, "VALUE", "255");
  IupSetAttribute(colordlg_data->alpha_val, "SIZE", "80x12");
  IupSetCallback (colordlg_data->alpha_val, "MOUSEMOVE_CB", (Icallback)iColorBrowserDlgAlphaVal_CB);
  IupSetCallback (colordlg_data->alpha_val, "BUTTON_PRESS_CB", (Icallback)iColorBrowserDlgAlphaVal_CB);
  IupSetCallback (colordlg_data->alpha_val, "BUTTON_RELEASE_CB", (Icallback)iColorBrowserDlgAlphaVal_CB);

  colordlg_data->alpha_txt = IupText(NULL);                        /* Alpha value */
  IupSetAttribute(colordlg_data->alpha_txt, "VISIBLECOLUMNS", "3");
  IupSetAttribute(colordlg_data->alpha_txt, "SPIN", "YES");
  IupSetAttribute(colordlg_data->alpha_txt, "SPINMIN", "0");
  IupSetAttribute(colordlg_data->alpha_txt, "SPINMAX", "255");
  IupSetAttribute(colordlg_data->alpha_txt, "SPININC", "1");
  IupSetCallback (colordlg_data->alpha_txt, "ACTION", (Icallback)iColorBrowserDlgAlphaAction_CB);
  IupSetCallback (colordlg_data->alpha_txt, "SPIN_CB", (Icallback)iColorBrowserDlgAlphaSpin_CB);
  IupSetAttribute(colordlg_data->alpha_txt, "MASKINT", "0:255");

  /* ======================================================================= */
  /* COLOR TABLE   ========================================================= */
  /* ======================================================================= */
  colordlg_data->colortable_cbar = IupColorbar();
  IupSetAttribute(colordlg_data->colortable_cbar, "ORIENTATION", "HORIZONTAL");
  IupSetAttribute(colordlg_data->colortable_cbar, "NUM_PARTS", "2");  
  IupSetAttribute(colordlg_data->colortable_cbar, "NUM_CELLS", "20");
  IupSetAttribute(colordlg_data->colortable_cbar, "SHOW_PREVIEW", "NO");
  IupSetAttribute(colordlg_data->colortable_cbar, "SIZE", "138x22");
  IupSetAttribute(colordlg_data->colortable_cbar, "SQUARED", "NO");
  IupSetCallback (colordlg_data->colortable_cbar, "SELECT_CB",   (Icallback)iColorBrowserDlgColorTableSelect_CB);

  /* ======================================================================= */
  /* RGB TEXT FIELDS   ===================================================== */
  /* ======================================================================= */
  colordlg_data->red_txt = IupText(NULL);                            /* Red value */
  IupSetAttribute(colordlg_data->red_txt, "VISIBLECOLUMNS", "3");
  IupSetAttribute(colordlg_data->red_txt, "SPIN", "YES");
  IupSetAttribute(colordlg_data->red_txt, "SPINMIN", "0");
  IupSetAttribute(colordlg_data->red_txt, "SPINMAX", "255");
  IupSetAttribute(colordlg_data->red_txt, "SPININC", "1");
  IupSetCallback (colordlg_data->red_txt, "ACTION", (Icallback)iColorBrowserDlgRedAction_CB);
  IupSetCallback (colordlg_data->red_txt, "SPIN_CB", (Icallback)iColorBrowserDlgRedSpin_CB);
  IupSetAttribute(colordlg_data->red_txt, "MASKINT", "0:255");

  colordlg_data->green_txt = IupText(NULL);                        /* Green value */
  IupSetAttribute(colordlg_data->green_txt, "VISIBLECOLUMNS", "3");
  IupSetAttribute(colordlg_data->green_txt, "SPIN", "YES");
  IupSetAttribute(colordlg_data->green_txt, "SPINMIN", "0");
  IupSetAttribute(colordlg_data->green_txt, "SPINMAX", "255");
  IupSetAttribute(colordlg_data->green_txt, "SPININC", "1");
  IupSetCallback (colordlg_data->green_txt, "ACTION", (Icallback)iColorBrowserDlgGreenAction_CB);
  IupSetCallback (colordlg_data->green_txt, "SPIN_CB", (Icallback)iColorBrowserDlgGreenSpin_CB);
  IupSetAttribute(colordlg_data->green_txt, "MASKINT", "0:255");

  colordlg_data->blue_txt = IupText(NULL);                          /* Blue value */
  IupSetAttribute(colordlg_data->blue_txt, "VISIBLECOLUMNS", "3");
  IupSetAttribute(colordlg_data->blue_txt, "SPIN", "YES");
  IupSetAttribute(colordlg_data->blue_txt, "SPINMIN", "0");
  IupSetAttribute(colordlg_data->blue_txt, "SPINMAX", "255");
  IupSetAttribute(colordlg_data->blue_txt, "SPININC", "1");
  IupSetCallback (colordlg_data->blue_txt, "ACTION", (Icallback)iColorBrowserDlgBlueAction_CB);
  IupSetCallback (colordlg_data->blue_txt, "SPIN_CB", (Icallback)iColorBrowserDlgBlueSpin_CB);
  IupSetAttribute(colordlg_data->blue_txt, "MASKINT", "0:255");

  /* ======================================================================= */
  /* HSI TEXT FIELDS   ===================================================== */
  /* ======================================================================= */
  colordlg_data->hue_txt = IupText(NULL);                            /* Hue value */
  IupSetAttribute(colordlg_data->hue_txt, "VISIBLECOLUMNS", "3");
  IupSetAttribute(colordlg_data->hue_txt, "SPIN", "YES");
  IupSetAttribute(colordlg_data->hue_txt, "SPINMIN", "0");
  IupSetAttribute(colordlg_data->hue_txt, "SPINMAX", "359");
  IupSetAttribute(colordlg_data->hue_txt, "SPINWRAP", "YES");
  IupSetAttribute(colordlg_data->hue_txt, "SPININC", "1");
  IupSetCallback(colordlg_data->hue_txt, "ACTION", (Icallback)iColorBrowserDlgHueAction_CB);
  IupSetCallback(colordlg_data->hue_txt, "SPIN_CB", (Icallback)iColorBrowserDlgHueSpin_CB);
  IupSetAttribute(colordlg_data->hue_txt, "MASKINT", "0:359");

  colordlg_data->saturation_txt = IupText(NULL);              /* Saturation value */
  IupSetAttribute(colordlg_data->saturation_txt, "VISIBLECOLUMNS", "3");
  IupSetAttribute(colordlg_data->saturation_txt, "SPIN", "YES");
  IupSetAttribute(colordlg_data->saturation_txt, "SPINMIN", "0");
  IupSetAttribute(colordlg_data->saturation_txt, "SPINMAX", "100");
  IupSetAttribute(colordlg_data->saturation_txt, "SPININC", "1");
  IupSetCallback(colordlg_data->saturation_txt, "ACTION", (Icallback)iColorBrowserDlgSaturationAction_CB);
  IupSetCallback(colordlg_data->saturation_txt, "SPIN_CB", (Icallback)iColorBrowserDlgSaturationSpin_CB);
  IupSetAttribute(colordlg_data->saturation_txt, "MASKINT", "0:100");

  colordlg_data->intensity_txt = IupText(NULL);                /* Intensity value */
  IupSetAttribute(colordlg_data->intensity_txt, "VISIBLECOLUMNS", "3");
  IupSetAttribute(colordlg_data->intensity_txt, "SPIN", "YES");
  IupSetAttribute(colordlg_data->intensity_txt, "SPINMIN", "0");
  IupSetAttribute(colordlg_data->intensity_txt, "SPINMAX", "100");
  IupSetAttribute(colordlg_data->intensity_txt, "SPININC", "1");
  IupSetCallback(colordlg_data->intensity_txt, "ACTION", (Icallback)iColorBrowserDlgIntensityAction_CB);
  IupSetCallback(colordlg_data->intensity_txt, "SPIN_CB", (Icallback)iColorBrowserDlgIntensitySpin_CB);
  IupSetAttribute(colordlg_data->intensity_txt, "MASKINT", "0:100");

  /* =================== */
  /* 1st line = Controls */
  /* =================== */

  col1 = IupVbox(colordlg_data->color_browser, IupSetAttributes(IupHbox(colordlg_data->color_cnv, NULL), "MARGIN=30x0"),NULL);

  hsi_vb = IupVbox(IupSetAttributes(IupHbox(IupLabel(iupStrMessageGet("IUP_HUE")), 
                                            colordlg_data->hue_txt, 
                                            NULL), "ALIGNMENT=ACENTER"),
                   IupSetAttributes(IupHbox(IupLabel(iupStrMessageGet("IUP_SATURATION")), 
                                            colordlg_data->saturation_txt, 
                                            NULL), "ALIGNMENT=ACENTER"),
                   IupSetAttributes(IupHbox(IupLabel(iupStrMessageGet("IUP_INTENSITY")), 
                                            colordlg_data->intensity_txt, 
                                            NULL), "ALIGNMENT=ACENTER"),
                   NULL);
  IupSetAttribute(hsi_vb, "GAP", "5");
  
  rgb_vb = IupVbox(IupSetAttributes(IupHbox(IupLabel(iupStrMessageGet("IUP_RED")), 
                                            colordlg_data->red_txt, 
                                            NULL), "ALIGNMENT=ACENTER"),
                   IupSetAttributes(IupHbox(IupLabel(iupStrMessageGet("IUP_GREEN")), 
                                            colordlg_data->green_txt, 
                                            NULL), "ALIGNMENT=ACENTER"),
                   IupSetAttributes(IupHbox(IupLabel(iupStrMessageGet("IUP_BLUE")), 
                                            colordlg_data->blue_txt, 
                                            NULL), "ALIGNMENT=ACENTER"),
                   NULL);
  IupSetAttribute(rgb_vb, "GAP", "5");
  
  clr_vb = IupVbox(IupSetAttributes(IupHbox(IupLabel(iupStrMessageGet("IUP_OPACITY")), 
                                            colordlg_data->alpha_txt, colordlg_data->alpha_val, 
                                            NULL), "ALIGNMENT=ACENTER"),
                   IupSetAttributes(IupHbox(IupLabel("He&xa:"), 
                                            colordlg_data->colorhex_txt, 
                                            NULL), "ALIGNMENT=ACENTER"),
                   IupSetAttributes(IupVbox(IupLabel(iupStrMessageGet("IUP_PALETTE")), 
                                            colordlg_data->colortable_cbar,
                                            NULL), "GAP=3"),
                   NULL);
  IupSetAttribute(clr_vb, "GAP", "5");
  IupSetAttribute(clr_vb, "EXPAND", "YES");

  IupDestroy(IupSetAttributes(IupNormalizer(IupGetChild(IupGetChild(hsi_vb, 0), 0),  /* Hue Label */
                                            IupGetChild(IupGetChild(hsi_vb, 1), 0),  /* Saturation Label */
                                            IupGetChild(IupGetChild(hsi_vb, 2), 0),  /* Intensity Label */
                                            IupGetChild(IupGetChild(clr_vb, 0), 0),  /* Opacity Label */
                                            IupGetChild(IupGetChild(clr_vb, 1), 0),  /* Hexa Label */
                                            NULL), "NORMALIZE=HORIZONTAL"));

  IupDestroy(IupSetAttributes(IupNormalizer(IupGetChild(IupGetChild(rgb_vb, 0), 0),  /* Red Label */
                                            IupGetChild(IupGetChild(rgb_vb, 1), 0),  /* Green Label */
                                            IupGetChild(IupGetChild(rgb_vb, 2), 0),  /* Blue Label */
                                            NULL), "NORMALIZE=HORIZONTAL"));

  col2 = IupVbox(IupSetAttributes(IupHbox(hsi_vb, IupFill(), rgb_vb, NULL), "EXPAND=YES"), 
                 IupSetAttributes(IupLabel(NULL), "SEPARATOR=HORIZONTAL"), 
                 clr_vb,
                 NULL);
  IupSetAttributes(col2, "EXPAND=NO, GAP=10");

  lin1 = IupHbox(col1, col2, NULL);
  IupSetAttribute(lin1, "GAP", "10");
  IupSetAttribute(lin1, "MARGIN", "0x0");

  /* ================== */
  /* 2nd line = Buttons */
  /* ================== */

  lin2 = IupHbox(IupFill(), ok_bt, cancel_bt, colordlg_data->help_bt, NULL);
  IupSetAttribute(lin2, "GAP", "5");
  IupSetAttribute(lin2, "MARGIN", "0x0");
  IupSetAttribute(lin2, "NORMALIZESIZE", "HORIZONTAL");

  /* Do not use IupAppend because we set childtype=IUP_CHILDNONE */
  iupChildTreeAppend(ih, IupSetAttributes(IupVbox(lin1, IupSetAttributes(IupLabel(NULL), "SEPARATOR=HORIZONTAL"), lin2, NULL), "MARGIN=10x10, GAP=10"));

  iColorBrowserDlgInit_Defaults(colordlg_data);

  (void)params;
  return IUP_NOERROR;
}

Iclass* iupColorBrowserDlgNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("dialog"));

  ic->New = iupColorBrowserDlgNewClass;
  ic->Create = iColorBrowserDlgCreateMethod;
  ic->Destroy = iColorBrowserDlgDestroyMethod;
  ic->Map = iColorBrowserDlgMapMethod;

  ic->name = "colordlg";   /* this will hide the GTK and Windows implementations */
  ic->nativetype = IUP_TYPEDIALOG;
  ic->is_interactive = 1;
  ic->childtype = IUP_CHILDNONE;

  iupClassRegisterAttribute(ic, "COLORTABLE", iColorBrowserDlgGetColorTableAttrib, iColorBrowserDlgSetColorTableAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STATUS", iColorBrowserDlgGetStatusAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUE", iColorBrowserDlgGetValueAttrib, iColorBrowserDlgSetValueAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALPHA", iColorBrowserDlgGetAlphaAttrib, iColorBrowserDlgSetAlphaAttrib, IUPAF_SAMEASSYSTEM, "255", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUEHSI", iColorBrowserDlgGetValueHSIAttrib, iColorBrowserDlgSetValueHSIAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "VALUEHEX", iColorBrowserDlgGetValueHexAttrib, iColorBrowserDlgSetValueHexAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWALPHA", NULL, iColorBrowserDlgSetShowAlphaAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWCOLORTABLE", NULL, iColorBrowserDlgSetShowColorTableAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWHEX", NULL, iColorBrowserDlgSetShowHexAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SHOWHELP", NULL, iColorBrowserDlgSetShowHelpAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}
