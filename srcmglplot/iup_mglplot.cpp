/*
* IupMglPlot component
*
* Description : A component, derived from MathGL and IupGLCanvas
*      Remark : Depend on libs IUP, IUPGL, OpenGL
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"
#include "iupgl.h"

#include "iup_mglplot.h"

#include "iup_assert.h"
#include "iup_class.h"
#include "iup_register.h"
#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_array.h"

#include "mgl/mgl.h"
#include "mgl/mgl_ab.h"
#include "mgl/mgl_eps.h"
#include "mgl/mgl_zb.h"
#include "mgl/mgl_eval.h"
#include "mgl/mgl_gl.h"

#include "mgl_makefont.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif


enum {IUP_MGLPLOT_BOTTOMLEFT, IUP_MGLPLOT_BOTTOMRIGHT, IUP_MGLPLOT_TOPLEFT, IUP_MGLPLOT_TOPRIGHT};
enum {IUP_MGLPLOT_INHERIT, IUP_MGLPLOT_PLAIN, IUP_MGLPLOT_BOLD, IUP_MGLPLOT_ITALIC, IUP_MGLPLOT_BOLD_ITALIC};


typedef struct _IdataSet
{
  char dsLineStyle;
  float dsLineWidth;
  bool dsShowMarks;
  char dsMarkStyle;
  float dsMarkSize;
  bool dsShowValues;
  char* dsMode;
  char* dsLegend;
  mglColor dsColor;  // should never has NAN values

  int dsDim;        /* Dimension of the data: 1D, 2D or 3D */
  Iarray* dsNames;  /* optional names used in ticks when in 1D */
  mglData* dsX;
  mglData* dsY;
  mglData* dsZ;
  int dsCount;
} IdataSet;

typedef struct _Iaxis
{
  mglColor axColor;
  float axOrigin;
  const char* axScale;
  bool axShow, axShowArrow;

  int axLabelPos;
  int axLabelFontStyle;
  float axLabelFontSizeFactor;
  bool axLabelRotation;

  int axTickFontStyle;
  float axTickFontSizeFactor;
  bool axTickShowValues, axTickShow, 
       axTickAutoSpace, axTickAutoSize;
  float axTickMinorSizeFactor, axTickMajorSize;          
  int axTickMinorDivision;
  float axTickMajorSpan;
  bool axTickValuesRotation;

  bool axReverse;
  float axMax, axMin;
  bool axAutoScaleMax, axAutoScaleMin;
} Iaxis;

struct _IcontrolData
{
  iupCanvas canvas;     /* from IupCanvas (must reserve it)  */

  mglGraphAB* mgl;

  /* Control */
  int w, h;
  float dpi;
  bool redraw;
  bool opengl;
  char ErrorMessage[1024];

  /* Obtained from FONT */
  char FontDef[32];     
  float FontSizeDef;
  int FontStyleDef;
  
  bool useMakeFont;
  mglMakeFont* makeFont;

  /* Global */
  bool transparent;
  float alpha;
  mglColor bgColor, fgColor;

  /* Title */
  mglColor titleColor;
  int titleFontStyle;
  float titleFontSizeFactor;

  /* Axes */
  Iaxis axisX, axisY, axisZ;

  /* Interaction */
  float last_x, last_y;  // Last Mouse Position
  float x1, x2, y1, y2;  // Zoom+Pan
  float rotX, rotZ, rotY;  // Rotate (angle in degrees)

  /* Box */
  bool Box, boxTicks;
  mglColor boxColor;

  /* Legend */
  bool legendShow, legendBox;
  mglColor legendColor;
  int legendPosition;
  int legendFontStyle;
  float legendFontSizeFactor;

  /* Grid */
  mglColor gridColor;
  const char* gridShow;
  char gridLineStyle;

  /* Dataset */
  int dataSetCurrent;
  int dataSetCount, dataSetMaxCount;
  IdataSet* dataSet;
};

/* Callbacks function pointer typedefs. */
// TODO
// typedef int (*IFniiff)(Ihandle*, int, int, float, float); /* delete_cb */
// typedef int (*IFniiffi)(Ihandle*, int, int, float, float, int); /* select_cb */
// typedef int (*IFniiffff)(Ihandle*, int, int, float, float, float*, float*); /* edit_cb */

/******************************************************************************
 Useful Functions
******************************************************************************/

// Quantize 0-1 values into 0-255.
inline unsigned char iQuant(const float& value)
{
  if (value >= 1.0f) return 255;
  if (value <= 0.0f) return 0;
  return (unsigned char)(value*256.0f);
}                               

// Reconstruct 0-255 values into 0-1.
inline float iRecon(const unsigned char& value)
{
  if (value <= 0) return 0.0f;
  if (value >= 255) return 1.0f;
  return ((float)value + 0.5f)/256.0f;
}

inline void iSwap(float& a, float& b)
{
  float tmp = a;
  a = b;
  b = tmp;
}

static char* iMglPlotDefaultLegend(int ds)
{
  char legend[50];
  sprintf(legend, "plot %d", ds);
  return iupStrDup(legend);
}

static void iMglPlotSetDsColorDefault(mglColor &color, int ds_index)
{
  switch(ds_index)
  {
    case 0: color.Set(1, 0, 0); break;
    case 1: color.Set(0, 0, 1); break;
    case 2: color.Set(0, 1, 0); break;
    case 3: color.Set(0, 1, 1); break;
    case 4: color.Set(1, 0, 1); break;
    case 5: color.Set(1, 1, 0); break;
    default: color.Set(0, 0, 0); break;
  }
}

static void iMglPlotResetDataSet(IdataSet* ds, int ds_index)
{
  // Can NOT use memset here

  ds->dsLineStyle  = '-';  // SOLID/CONTINUOUS
  ds->dsLineWidth  = 1;
  ds->dsMarkStyle  = 'x';  // "X"
  ds->dsMarkSize   = 0.02f;
  ds->dsShowMarks = false;
  ds->dsShowValues = false;
  ds->dsMode = iupStrDup("LINE");
  ds->dsLegend = iMglPlotDefaultLegend(ds_index);

  iMglPlotSetDsColorDefault(ds->dsColor, ds_index);
}

static void iMglPlotResetInteraction(Ihandle *ih)
{
  ih->data->x1 = 0.0f;  ih->data->y1 = 0.0f;
  ih->data->x2 = 1.0f;  ih->data->y2 = 1.0f;

  ih->data->last_x = 0.0f;
  ih->data->last_y = 0.0f;

  ih->data->rotX = 0.0f;   ih->data->rotZ = 0.0f;   ih->data->rotY = 0.0f;
}

static void iMglPlotResetAxis(Iaxis& axis)
{
  memset(&axis, 0, sizeof(Iaxis));

  axis.axLabelFontSizeFactor = 1;
  axis.axTickFontSizeFactor = 0.8f;

  axis.axColor.Set(NAN, NAN, NAN);

  axis.axAutoScaleMin = true;
  axis.axAutoScaleMax = true;
  axis.axMax = 1;
  axis.axShow = true;
  axis.axShowArrow = true;
  axis.axLabelRotation = true;
  axis.axOrigin = NAN;

  axis.axTickShow = true;
  axis.axTickShowValues = true;
  axis.axTickValuesRotation = true;
  axis.axTickAutoSpace = true;
  axis.axTickAutoSize = true;
  axis.axTickMinorSizeFactor = 0.6f;
  axis.axTickMajorSize = 0.1f;
  axis.axTickMinorDivision = 5;
  axis.axTickMajorSpan = -5;
}

static void iMglPlotReset(Ihandle* ih)
{
  // Can NOT use memset here

  ih->data->legendPosition = IUP_MGLPLOT_TOPRIGHT;
  ih->data->legendBox = true;
  ih->data->legendShow = false;
  ih->data->Box = false;
  ih->data->boxTicks = true;
  ih->data->gridLineStyle = '-';
  ih->data->gridShow = NULL;

  ih->data->alpha = 0.5f;
  ih->data->transparent = false;

  ih->data->titleFontStyle = IUP_MGLPLOT_INHERIT;
  ih->data->legendFontStyle = IUP_MGLPLOT_INHERIT;
  ih->data->legendFontSizeFactor = 0.8f;
  ih->data->titleFontSizeFactor = 1.4f;

  ih->data->bgColor.Set(1, 1, 1);
  ih->data->fgColor.Set(0, 0, 0);
  ih->data->gridColor.Set(iRecon(200), iRecon(200), iRecon(200));
  ih->data->titleColor.Set(NAN, NAN, NAN);
  ih->data->legendColor.Set(NAN, NAN, NAN);
  ih->data->boxColor.Set(NAN, NAN, NAN);

  iMglPlotResetAxis(ih->data->axisX);
  iMglPlotResetAxis(ih->data->axisY);
  iMglPlotResetAxis(ih->data->axisZ);

  /* Interaction default values */
  iMglPlotResetInteraction(ih);
}

static bool iMglPlotIsPlanarOrVolumetricData(IdataSet* ds)
{
  return (ds->dsDim == 1 && (ds->dsX->ny>1 || ds->dsX->nz>1));
}

static bool iMglPlotIsPlanarData(IdataSet* ds)
{
  return (ds->dsDim == 1 && ds->dsX->ny>1 && ds->dsX->nz==1);
}

static bool iMglPlotIsVolumetricData(IdataSet* ds)
{
  return (ds->dsDim == 1 && ds->dsX->nz>1);
}

static bool iMglPlotIsView3D(Ihandle* ih)
{
  int i;
  for (i=0; i< ih->data->dataSetCount; i++)
  {
    IdataSet* ds = &ih->data->dataSet[i];
    if ((ds->dsDim == 1 && (ds->dsX->ny>1 || ds->dsX->nz>1)) ||  // Planar or Volumetric
        ds->dsDim == 3)  // 3D data
    {
      if (!iupStrEqualNoCase(ds->dsMode, "PLANAR_DENSITY") &&
          !iupStrEqualNoCase(ds->dsMode, "PLANAR_GRADIENTLINES"))
        return true;
    }
  }

  return false;
}

#ifdef WIN32
static LONG winGetNextNameValue(HKEY key, LPCTSTR subkey, LPTSTR szName, LPTSTR szData)
{
  static HKEY hkey = NULL;
  static DWORD dwIndex = 0;
  LONG retval;

  if (subkey == NULL && szName == NULL && szData == NULL)
  {
    if (hkey)
      RegCloseKey(hkey);
  
    hkey = NULL;
    dwIndex = 0;
    return ERROR_SUCCESS;
  }

  if (subkey && subkey[0] != 0)
  {
    retval = RegOpenKeyEx(key, subkey, 0, KEY_READ, &hkey);
    if (retval != ERROR_SUCCESS)
      return retval;

    dwIndex = 0;
  }
  else
    dwIndex++;

  *szName = 0;
  *szData = 0;

  {
    char szValueName[MAX_PATH];
    DWORD dwValueNameSize = sizeof(szValueName)-1;
    BYTE szValueData[MAX_PATH];
    DWORD dwValueDataSize = sizeof(szValueData)-1;
    DWORD dwType = 0;

    retval = RegEnumValue(hkey, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, szValueData, &dwValueDataSize);
    if (retval == ERROR_SUCCESS)
    {
      lstrcpy(szName, (char *)szValueName);
      lstrcpy(szData, (char *)szValueData);
    }
  }

  return retval;
}

static int winReadStringKey(HKEY base_key, char* key_name, char* value_name, char* value)
{
	HKEY key;
	DWORD max_size = 512;

	if (RegOpenKeyEx(base_key, key_name, 0, KEY_READ, &key) != ERROR_SUCCESS)
		return 0;

  if (RegQueryValueEx(key, value_name, NULL, NULL, (LPBYTE)value, &max_size) != ERROR_SUCCESS)
  {
    RegCloseKey(key);
		return 0;
  }

	RegCloseKey(key);
	return 1;
}

static char* winGetFontDir(void)
{
  static char font_dir[1024];
  if (!winReadStringKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", "Fonts", font_dir))
    return "";
  else
    return font_dir;
}

static int sysGetFontFileName(const char *font_name, int is_bold, int is_italic, char* fileName)
{
  TCHAR szName[2 * MAX_PATH];
  TCHAR szData[2 * MAX_PATH];
  LPCTSTR strFont = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
  char localFontName[256];
  int bResult = 0;

  if (iupStrEqualNoCase(font_name, "Courier") || iupStrEqualNoCase(font_name, "Monospace"))
    font_name = "Courier New";
  else if (iupStrEqualNoCase(font_name, "Times") || iupStrEqualNoCase(font_name, "Serif"))
    font_name = "Times New Roman";
  else if (iupStrEqualNoCase(font_name, "Helvetica") || iupStrEqualNoCase(font_name, "Sans"))
    font_name = "Arial";

  strcpy(localFontName, font_name);

  if (is_bold)
    strcat(localFontName, " Bold");

  if (is_italic)
    strcat(localFontName, " Italic");

  while (winGetNextNameValue(HKEY_LOCAL_MACHINE, strFont, szName, szData) == ERROR_SUCCESS)
  {
    if (iupStrEqualNoCasePartial(szName, localFontName))
    {
      sprintf(fileName, "%s\\%s", winGetFontDir(), szData);  // szData already includes file extension
      bResult = 1;
      break;
    }
    strFont = NULL;
  }

  /* close the registry key */
  winGetNextNameValue(HKEY_LOCAL_MACHINE, NULL, NULL, NULL);

  return bResult;
}
#else
#ifndef NO_FONTCONFIG
#include <fontconfig/fontconfig.h>

static int sysGetFontFileName(const char *font_name, int is_bold, int is_italic, char* fileName)
{
  char styles[4][20];
  int style_size;
  FcObjectSet *os = 0;
  FcFontSet *fs;
  FcPattern *pat;
  int bResult = 0;

  if (iupStrEqualNoCase(font_name, "Courier") || iupStrEqualNoCase(font_name, "Courier New") || iupStrEqualNoCase(font_name, "Monospace"))
    font_name = "freemono";
  else if (iupStrEqualNoCase(font_name, "Times") || iupStrEqualNoCase(font_name, "Times New Roman")|| iupStrEqualNoCase(font_name, "Serif"))
    font_name = "freeserif";
  else if (iupStrEqualNoCase(font_name, "Helvetica") || iupStrEqualNoCase(font_name, "Arial") || iupStrEqualNoCase(font_name, "Sans"))
    font_name = "freesans";

  if( is_bold && is_italic )
  {
    strcpy(styles[0], "BoldItalic");
    strcpy(styles[1], "Bold Italic");
    strcpy(styles[2], "Bold Oblique");
    strcpy(styles[3], "BoldOblique");
    style_size = 4;
  }
  else if( is_bold )
  {
    strcpy(styles[0], "Bold");
    style_size = 1;
  }
  else if( is_italic )
  {
    strcpy(styles[0], "Italic");
    strcpy(styles[1], "Oblique");
    style_size = 2;
  }
  else
  {
    strcpy(styles[0], "Regular");
    strcpy(styles[1], "Normal");
    strcpy(styles[2], "Medium");
    style_size = 3;
  }

  pat = FcPatternCreate();
  os = FcObjectSetBuild(FC_FAMILY, FC_FILE, FC_STYLE, NULL);
  fs = FcFontList(NULL, pat, os);
  if (pat)
    FcPatternDestroy(pat);

  if(fs)
  {
    int j, s;

    for (j = 0; j < fs->nfont; j++)
    {
      FcChar8 *file;
      FcChar8 *style;
      FcChar8 *family;

      FcPatternGetString(fs->fonts[j], FC_FILE, 0, &file); 
      FcPatternGetString(fs->fonts[j], FC_STYLE, 0, &style );
      FcPatternGetString(fs->fonts[j], FC_FAMILY, 0, &family );

      if (iupStrEqualNoCasePartial((char*)family, font_name))
      {
        /* check if the font is of the correct type. */
        for(s = 0; s < style_size; s++ )
        {
          if (iupStrEqualNoCase(styles[s], (char*)style))
          {
            strcpy(fileName, (char*)file);
            bResult = 1;
            FcFontSetDestroy (fs);
            return bResult;
          }

          /* set value to use if no more correct font of same family is found. */
          strcpy(fileName, (char*)file);
          bResult = 1;
        }
      }
    }
    FcFontSetDestroy (fs);
  }

  return bResult;
}
#else
static int sysGetFontFileName(const char *font_name, int is_bold, int is_italic, char* fileName)
{
  (void)font_name;
  (void)is_bold;
  (void)is_italic;
  (void)fileName;
  return 0;
}
#endif
#endif

static int iupGetFontFileName(const char* path, const char* typeface, const char* ext, char* filename)
{
  FILE *file;

  /* current directory */
  sprintf(filename, "%s.%s", typeface, ext);
  file = fopen(filename, "r");

  if (file)
    fclose(file);
  else
  {
    /* path from the environment */
    if (path)
    {
      sprintf(filename, "%s/%s.%s", path, typeface, ext);
      file = fopen(filename, "r");
    }

    if (file)
      fclose(file);
    else
    {
#ifdef WIN32
      /* Windows Font folder */
      sprintf(filename, "%s\\%s.%s", winGetFontDir(), typeface, ext);
      file = fopen(filename, "r");

      if (file)
        fclose(file);
      else
        return 0;
#else
      return 0;
#endif
    }
  }

  return 1;
}

static char* iMglPlotGetTexFontName(const char* typeface)
{
  if (iupStrEqualNoCase(typeface, "sans") ||
      iupStrEqualNoCase(typeface, "helvetica") ||
      iupStrEqualNoCase(typeface, "arial"))
    return "heros";
  if (iupStrEqualNoCase(typeface, "monospace") ||
      iupStrEqualNoCase(typeface, "courier") ||
      iupStrEqualNoCase(typeface, "courier new"))
    return "cursor";
  if (iupStrEqualNoCase(typeface, "serif") ||
      iupStrEqualNoCase(typeface, "times") ||
      iupStrEqualNoCase(typeface, "times new roman"))
    return "termes";
  return NULL;
}

static char* iMglPlotGetTTFFontName(const char* typeface)
{
  if (iupStrEqualNoCase(typeface, "sans") ||
      iupStrEqualNoCase(typeface, "helvetica") ||
      iupStrEqualNoCase(typeface, "arial"))
    return "arial";
  if (iupStrEqualNoCase(typeface, "monospace") ||
      iupStrEqualNoCase(typeface, "courier") ||
      iupStrEqualNoCase(typeface, "courier new"))
    return "cour";
  if (iupStrEqualNoCase(typeface, "serif") ||
      iupStrEqualNoCase(typeface, "times") ||
      iupStrEqualNoCase(typeface, "times new roman"))
    return "times";
  return NULL;
}

static int iMglPlotGetFontNameOTF(const char* path, const char* typeface, char* filename, int is_bold, int is_italic)
{
  char fontname[100];
  char* face = iMglPlotGetTexFontName(typeface);
  if (face)
    sprintf(fontname, "texgyre%s-", face);
  else
    sprintf(fontname, "texgyre%s-", typeface);

  if (is_bold && is_italic)
    strcat(fontname, "bolditalic");
  else if (is_italic)
    strcat(fontname, "italic");
  else if (is_bold)
    strcat(fontname, "bold");
  else
    strcat(fontname, "regular");

  return iupGetFontFileName(path, fontname, "otf", filename);
}

static int iMglPlotGetFontNameTTF(const char* path, const char* typeface, char* filename, int is_bold, int is_italic)
{
  char fontname[100];
  char* face = iMglPlotGetTTFFontName(typeface);
  if (face)
    strcpy(fontname, face);
  else
    strcpy(fontname, typeface);

  if (is_bold && is_italic)
    strcat(fontname, "bi");
  else if (is_italic)
    strcat(fontname, "i");
  else if (is_bold)
    strcat(fontname, "bd");

  return iupGetFontFileName(path, fontname, "ttf", filename);
}

static void iMglPlotGetFontFilename(char* filename, const char* path, const char *typeface, int is_bold, int is_italic)
{
  /* search for the font in the system */
  // "Helvetica" "Courier" "Times" "Segoe UI" "Tahoma" etc
  if (sysGetFontFileName(typeface, is_bold, is_italic, filename))
    return;

  /* try typeface as a file title, compose with path to get a filename */
  // "ariali" "courbd" "texgyrecursor-bold"
  if (iupGetFontFileName(path, typeface, "ttf", filename))
    return;
  if (iupGetFontFileName(path, typeface, "otf", filename))
    return;

  /* check for the pre-defined names, and use style to compose the filename */
  // "cursor"
  if (iMglPlotGetFontNameTTF(path, typeface, filename, is_bold, is_italic))
    return;
  if (iMglPlotGetFontNameOTF(path, typeface, filename, is_bold, is_italic))
    return;

  /* try the typeface as file name */
  strcpy(filename, typeface);
}

static void iMglPlotConfigFontDef(Ihandle* ih, mglGraph *gr)
{
  int size = 0, i = 0;
  int is_bold = 0,
    is_italic = 0, 
    is_underline = 0,
    is_strikeout = 0;
  char typeface[1024];
  
  const char* font = iupGetFontAttrib(ih);
  if (!iupGetFontInfo(font, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    return;

  if (is_bold && is_italic)
  {
    ih->data->FontStyleDef = IUP_MGLPLOT_BOLD_ITALIC;
    ih->data->FontDef[i++] = 'b';
    ih->data->FontDef[i++] = 'i';
  }
  else if (is_bold)
  {
    ih->data->FontStyleDef = IUP_MGLPLOT_BOLD;
    ih->data->FontDef[i++] = 'b';
  }
  else if (is_italic)
  {
    ih->data->FontStyleDef = IUP_MGLPLOT_ITALIC;
    ih->data->FontDef[i++] = 'i';
  }
  else
  {
    ih->data->FontStyleDef = IUP_MGLPLOT_PLAIN;
    ih->data->FontDef[i++] = 'r';
  }

  if (is_underline)
    ih->data->FontDef[i++] = 'u';

  if (is_strikeout)
    ih->data->FontDef[i++] = 'o';

  ih->data->FontDef[i] = 0;

  if (size < 0) 
    size = -size;
  else 
    size = (int)((size*ih->data->dpi)/72.0f);   //from points to pixels

  //IMPORTANT: 
  //  Magic factor for acceptable size. 
  //  Don't know why it works, but we obtain good results.
  ih->data->FontSizeDef = ((float)size/(float)ih->data->h)*ih->data->dpi;

  char *path = getenv("IUP_MGLFONTS");
  if (!path) 
    path = IupGetGlobal("MGLFONTS");

  if (!iupStrEqualNoCase(typeface, iupAttribGetStr(ih, "_IUP_MGL_FONTNAME")) ||
      !iupStrEqualNoCase(path, iupAttribGetStr(ih, "_IUP_MGL_FONTPATH")))
  {
    // Try to Load OTF and TTF fonts using Freetype and FTGL
    char filename[10240];
    iMglPlotGetFontFilename(filename, path, typeface, is_bold, is_italic);
    ih->data->useMakeFont = true;
    if (!ih->data->makeFont->mglMakeLoadFontFT(gr, filename, size)) 
    {
      ih->data->useMakeFont = false;
      const char* fontname = iMglPlotGetTexFontName(typeface);
      if (!fontname)
        fontname = typeface;
      gr->LoadFont(fontname, path);  // Load MathGL VFM fonts
    }

    iupAttribStoreStr(ih, "_IUP_MGL_FONTNAME", typeface);
    iupAttribStoreStr(ih, "_IUP_MGL_FONTPATH", path);
  }

  //IMPORTANT:
  //  BOLD and ITALIC does not work for the internal font, only for loaded fonts. 
  //  Some TeX features too.
  //  BOLD and ITALIC inside TeX formatting does not work for TTF or OTF fonts.
}

static void iMglPlotConfigFont(Ihandle* ih, mglGraph *gr, int fontstyle, float fontsizefactor)
{
  int i=0;
  char fnt[32];

  if (fontstyle==IUP_MGLPLOT_INHERIT)
    fontstyle = ih->data->FontStyleDef;

  if (fontstyle==IUP_MGLPLOT_PLAIN)
    fnt[i++] = 'r';
  else if (fontstyle==IUP_MGLPLOT_BOLD)
    fnt[i++] = 'b';
  else if (fontstyle==IUP_MGLPLOT_ITALIC)
    fnt[i++] = 'i';
  else if (fontstyle==IUP_MGLPLOT_BOLD_ITALIC)
  {
    fnt[i++] = 'b';
    fnt[i++] = 'i';
  }

  fnt[i] = 0;

  if (i!=0)
    gr->SetFontDef(fnt);
  else
    gr->SetFontDef(ih->data->FontDef);

  gr->SetFontSize(fontsizefactor*ih->data->FontSizeDef);
}


static mglColor iMglPlotGetColorDefault(Ihandle* ih, mglColor color)
{
  if (isnan(color.r) || isnan(color.g) || isnan(color.b))
    color = ih->data->fgColor;
  return color;
}

static void iMglPlotConfigColor(Ihandle* ih, mglGraph *gr, mglColor color)
{
  gr->DefColor(iMglPlotGetColorDefault(ih, color), 1);
}

static char* iMglPlotConfigPen(mglGraph *gr, char* pen, char line_style, float line_width)
{
  *pen++ = line_style;

  gr->SetBaseLineWidth(line_width);
  *pen++ = '1'; // 1*BaseLineWidth

  *pen = 0;
  return pen;
}

static char iMglPlotFindColor(const mglColor& c1)
{
  int i;
  char id = 0;
  mreal old_diff = -1, diff, rr, bb, gg;

  /* Not the best solution, 
     but the only one possible witht he current MglPlot limitations */

	for(i=0; mglColorIds[i].id; i++)
  {
    const mglColor& c2 = mglColorIds[i].col;
    rr = c1.r-c2.r;
    gg = c1.g-c2.g;
    bb = c1.b-c2.b;
    diff = rr*rr + gg*gg + bb*bb;
    if (old_diff==-1 ||
        diff < old_diff)
    {
      id = mglColorIds[i].id;
      old_diff = diff;
    }
  }

  return id;
}

static void iMglPlotConfigDataSetColor(IdataSet* ds, char* style)
{
  style += strlen(style); // Skip previous configuration

  char id = iMglPlotFindColor(ds->dsColor);
  if (id)
    *style++ = id;

  *style = 0;
}

static void iMglPlotConfigDataSetLineMark(IdataSet* ds, mglGraph *gr, char* style)
{
  style += strlen(style); // Skip previous configuration
  style = iMglPlotConfigPen(gr, style, ds->dsLineStyle, ds->dsLineWidth);

  if (ds->dsShowMarks)
  {
    if (ds->dsMarkStyle == 'O' || ds->dsMarkStyle == 'S' || ds->dsMarkStyle == 'D')
    {
      *style++ = '#';   // Conflict with datagrid, can not use this marks
      *style++ = (char)tolower(ds->dsMarkStyle);
    }
    else
      *style++ = ds->dsMarkStyle;

    gr->SetMarkSize(ds->dsMarkSize);
  }

  *style = 0;
}

static void iMglPlotSetFunc(mglFormula* *func, const char* scale)
{
  if(*func) delete *func;
  *func = new mglFormula(scale);
}

static void iMglPlotConfigAxisTicks(Ihandle* ih, mglGraph *gr, char dir, const Iaxis& axis, float min, float max)
{
  if (!axis.axTickShow)
  {
    gr->SetTicks(dir, 0, 0);
    return;
  }

  //Set Scale only if ticks are enabled
  if (axis.axScale)  // Logarithm Scale
  {
    if (dir == 'x')
      iMglPlotSetFunc(&(gr->fx), axis.axScale);
    if (dir == 'y')
      iMglPlotSetFunc(&(gr->fy), axis.axScale);
    if (dir == 'z')
      iMglPlotSetFunc(&(gr->fz), axis.axScale);

    gr->SetTicks(dir, 0, 0);  // Disable normal ticks, logarithm ticks will be done
  }
  else  
  {
    // step for axis ticks (if positive) or 
    // it’s number on the axis range (if negative).
    float step = -5;
    int numsub = 0;  // Number of axis submarks

    if (axis.axTickAutoSpace)
    {
      // Copied from MathGL AdjustTicks internal function
      // TODO: improve autoticks computation
      mreal n, d = fabs(max-min);
      n = floor(log10(d));  
      d = floor(d*pow(10,-n));
      step = pow(10,n);
      if (d<4)
      {
        step *= 0.5f;
        numsub = 4;
      }
      else if(d<7)
        numsub = 4;
      else
      {
        step *= 2;
        numsub = 3;
      }
    }
    else
    {
      numsub = axis.axTickMinorDivision-1;
      step = axis.axTickMajorSpan;
    }

    gr->SetTicks(dir, step, numsub);
  }

  // Setting the template, switches off automatic ticks tuning
  char attrib[16] = "AXS_?TICKFORMAT";
  attrib[4] = (char)toupper(dir);
  char* format = iupAttribGetStr(ih, attrib);
  if (format && axis.axTickShowValues)
  {
    if (dir == 'x') gr->SetXTT(format);
    if (dir == 'y') gr->SetYTT(format);
    if (dir == 'z') gr->SetZTT(format);
    if (dir == 'c') gr->SetCTT(format);
  }
}

static void iMglPlotConfigScale(Iaxis& axis, float& min, float& max)
{
  if (!axis.axAutoScaleMax)
    max = axis.axMax;
  else
    axis.axMax = max;  // Update the attribute value
  if (!axis.axAutoScaleMin)
    min = axis.axMin;
  else
    axis.axMin = min;  // Update the attribute value

  if (axis.axScale)  // Logarithm Scale
  {
    if (axis.axMin < 0)
    {
      axis.axMin = 0;
      min = 0;
    }
    if (axis.axMax < 0)
    {
      axis.axMax = 0;
      max = 0;
    }
  }

  if (axis.axReverse)
    iSwap(min, max);
}

static void iMglPlotFindMinMaxValues(mglData& ds_data, bool add, float& min, float& max)
{
  int i, count = ds_data.nx*ds_data.ny*ds_data.nz;
  float* data = ds_data.a;

  for (i = 1; i < count; i++, data++)
  {
    if (isnan(*data))
      continue;

    if (!add)  // first valid found
    {
      min = *data;
      max = *data;
      add = true;
    }
    else
    {
      min = min<*data ? min: *data;
      max = max>*data ? max: *data;
    }
  }
}

static void iMglPlotConfigAxesRange(Ihandle* ih, mglGraph *gr)
{
  int i;

  if (ih->data->axisX.axAutoScaleMax || ih->data->axisX.axAutoScaleMin ||
      ih->data->axisY.axAutoScaleMax || ih->data->axisY.axAutoScaleMin ||
      ih->data->axisZ.axAutoScaleMax || ih->data->axisZ.axAutoScaleMin)
  {
    for(i = 0; i < ih->data->dataSetCount; i++)
    {
      IdataSet* ds = &ih->data->dataSet[i];
      bool add = i==0? false: true;
      mglPoint oldMin = gr->Min;
      mglPoint oldMax = gr->Max;

      if (iupStrEqualNoCase(ds->dsMode, "RADAR"))
        add = false;

      if (ds->dsDim == 1)
      {
        if (ds->dsX->ny>1 || ds->dsX->nz>1)  /* Planar or Volumetric data */
        {
          if (ih->data->axisX.axAutoScaleMax || ih->data->axisX.axAutoScaleMin)
          {
            gr->Min.x = -1.0f;
            gr->Max.x = +1.0f;
          }
          if (ih->data->axisY.axAutoScaleMax || ih->data->axisY.axAutoScaleMin)
          {
            gr->Min.y = -1.0f;
            gr->Max.y = +1.0f;
          }
          if (ih->data->axisZ.axAutoScaleMax || ih->data->axisZ.axAutoScaleMin)
          {
            gr->Min.z = -1.0f;
            gr->Max.z = +1.0f;
          }
        }
        else /* 1D Linear data */
        {
          /* the data will be plotted as Y, X will be 0,1,2,3,... */
          if (ih->data->axisX.axAutoScaleMax || ih->data->axisX.axAutoScaleMin)
          {
            gr->Min.x = 0;
            float ds_max = (mreal)(ds->dsCount-1);
            gr->Max.x = i==0? ds_max: (ds_max>gr->Max.x? ds_max: gr->Max.x);
          }
          if (ih->data->axisY.axAutoScaleMax || ih->data->axisY.axAutoScaleMin)
            iMglPlotFindMinMaxValues(*ds->dsX, add, gr->Min.y, gr->Max.y);

          if (iupStrEqualNoCase(ds->dsMode, "BARHORIZONTAL"))
          {
            iSwap(gr->Min.x, gr->Min.y);
            iSwap(gr->Max.x, gr->Max.y);
          }
          else if (iupStrEqualNoCase(ds->dsMode, "RADAR"))
          {
            float r = iupAttribGetFloat(ih, "RADARSHIFT");   // Default -1
          	if(r<0)	r = (gr->Min.y<0)? -gr->Min.y:0;
            float rmax = fabs(gr->Max.y)>fabs(gr->Min.y)?fabs(gr->Max.y):fabs(gr->Min.y);
            float min = -(r + rmax);
            float max =  (r + rmax);
            if (i==0)
            {
              gr->Min.y = gr->Min.x = min;
              gr->Max.y = gr->Max.x = max;
            }
            else
            {
              if (min < oldMin.x)
                gr->Min.y = gr->Min.x = min;
              else
                gr->Min.y = gr->Min.x = oldMin.x;
              if (max > oldMax.x)
                gr->Max.y = gr->Max.x = max;
              else
                gr->Max.y = gr->Max.x = oldMax.x;
            }
            oldMin = gr->Min;
            oldMax = gr->Max;
          }
          else if (iupStrEqualNoCase(ds->dsMode, "CHART") && iupAttribGetBoolean(ih, "PIECHART"))
          {
            float rmax = fabs(gr->Max.y)>fabs(gr->Min.y)?fabs(gr->Max.y):fabs(gr->Min.y);
            gr->Min.y = gr->Min.x = -rmax;
            gr->Max.y = gr->Max.x =  rmax;
          }
        }
      }
      else if (ds->dsDim == 2)  /* 2D Linear data */
      {
        if (ih->data->axisX.axAutoScaleMax || ih->data->axisX.axAutoScaleMin)
          iMglPlotFindMinMaxValues(*ds->dsX, add, gr->Min.x, gr->Max.x);
        if (ih->data->axisY.axAutoScaleMax || ih->data->axisY.axAutoScaleMin)
          iMglPlotFindMinMaxValues(*ds->dsY, add, gr->Min.y, gr->Max.y);
      }
      else if (ds->dsDim == 3)  /* 2D Linear data */
      {
        if (ih->data->axisX.axAutoScaleMax || ih->data->axisX.axAutoScaleMin)
          iMglPlotFindMinMaxValues(*ds->dsX, add, gr->Min.x, gr->Max.x);
        if (ih->data->axisY.axAutoScaleMax || ih->data->axisY.axAutoScaleMin)
          iMglPlotFindMinMaxValues(*ds->dsY, add, gr->Min.y, gr->Max.y);
        if (ih->data->axisZ.axAutoScaleMax || ih->data->axisZ.axAutoScaleMin)
          iMglPlotFindMinMaxValues(*ds->dsZ, add, gr->Min.z, gr->Max.z);
      }
    }
  }

  iMglPlotConfigScale(ih->data->axisX, gr->Min.x, gr->Max.x);
  iMglPlotConfigScale(ih->data->axisY, gr->Min.y, gr->Max.y);
  iMglPlotConfigScale(ih->data->axisZ, gr->Min.z, gr->Max.z);

  iMglPlotConfigAxisTicks(ih, gr, 'x', ih->data->axisX, gr->Min.x, gr->Max.x);
  iMglPlotConfigAxisTicks(ih, gr, 'y', ih->data->axisY, gr->Min.y, gr->Max.y);
  iMglPlotConfigAxisTicks(ih, gr, 'z', ih->data->axisZ, gr->Min.z, gr->Max.z);

  // By default set CMin-Max to Z Min-Max
  char* value = iupAttribGetStr(ih, "COLORBARAXISTICKS");
  if (value && tolower(*value) == 'x') { gr->Cmin = gr->Min.x;	gr->Cmax = gr->Max.x; }
  else if (value && tolower(*value) == 'y') { gr->Cmin = gr->Min.y;	gr->Cmax = gr->Max.y; }
  else { gr->Cmin = gr->Min.z;	gr->Cmax = gr->Max.z; }

  value = iupAttribGetStr(ih, "COLORBARRANGE");
  if (value) iupStrToFloatFloat(value, &(gr->Cmin), &(gr->Cmax), ':');

  value = iupAttribGetStr(ih, "COLORBARAXISTICKS");
  if (value && tolower(*value) == 'x') iMglPlotConfigAxisTicks(ih, gr, 'c', ih->data->axisX, gr->Cmin, gr->Cmax);
  else if (value && tolower(*value) == 'y') iMglPlotConfigAxisTicks(ih, gr, 'c', ih->data->axisY, gr->Cmin, gr->Cmax);
  else iMglPlotConfigAxisTicks(ih, gr, 'c', ih->data->axisZ, gr->Cmin, gr->Cmax);

  // Do NOT automatically set Org, we will set Origin
  gr->AutoOrg = false;
  gr->SetOrigin(ih->data->axisX.axOrigin, ih->data->axisY.axOrigin, ih->data->axisZ.axOrigin);

  gr->RecalcBorder();
}

static void iMglPlotDrawAxisLabel(Ihandle* ih, mglGraph *gr, char dir, Iaxis& axis)
{
  char attrib[11] = "AXS_?LABEL";
  attrib[4] = (char)toupper(dir);
  char* label = iupAttribGetStr(ih, attrib);
  if (label)
  {
    gr->SetRotatedText(axis.axLabelRotation);

    iMglPlotConfigFont(ih, gr, axis.axLabelFontStyle, axis.axLabelFontSizeFactor);
    
    // Check if all characters are loaded
    if (ih->data->useMakeFont)
      ih->data->makeFont->mglMakeFontSearchGlyph(gr, label);

    // TODO sometimes the label gets too close to the ticks
    gr->Label(dir, label, (mreal)axis.axLabelPos, -1);  

    gr->SetRotatedText(false);
  }
}

static void iMglPlotConfigAxisTicksVal(Ihandle* ih, mglGraph *gr, bool set)
{
  IdataSet* ds = &ih->data->dataSet[0];  // Allow names only for the first dataset
  if (ds->dsNames)
  {
    // TODO: TicksVal should follow ticks spacing configuration
    if (set)
    {
      char** dsNames = (char**)iupArrayGetData(ds->dsNames);
      int count = iupArrayCount(ds->dsNames);
      mreal *val = new mreal[count];
      for(int i=0; i< count; i++)
        val[i] = (mreal)i;
      gr->SetTicksVal('x', count, val, (const char**)dsNames);
      delete val;
    }
    else
      gr->SetTicksVal('x', 0, (mreal*)NULL, (const char **)NULL);
  }
}

static void iMglPlotConfigAxisTicksLen(mglGraph *gr, Iaxis& axis)
{
  if (!axis.axTickAutoSize)
  {
    //TODO: documentation says negative len puts tic outside the bounding box, 
    //      but it is NOT working
    mreal stt = 1.0f/(axis.axTickMinorSizeFactor*axis.axTickMinorSizeFactor) - 1.0f;
    gr->SetTickLen(axis.axTickMajorSize, stt);
  }
  else
  {
    mreal stt = 1.0f/(0.6f*0.6f) - 1.0f;
    gr->SetTickLen(0.1f, stt);
  }
}

static void iMglPlotDrawAxis(Ihandle* ih, mglGraph *gr, char dir, Iaxis& axis)
{
  iMglPlotConfigColor(ih, gr, axis.axColor);
  iMglPlotConfigFont(ih, gr, axis.axTickFontStyle, axis.axTickFontSizeFactor);

  // Must be set here, because there is only one for all the axis.
  iMglPlotConfigAxisTicksLen(gr, axis);

  // Must be after configuring the ticks
  if (dir == 'x')
    iMglPlotConfigAxisTicksVal(ih, gr, true);

  // Configure ticks values rotation along axis
  gr->SetRotatedText(axis.axTickValuesRotation);

  // Draw the axis, ticks and ticks values
  int i = 0;
  char style[10];
  style[i++]=dir;
  if (!axis.axTickShowValues)
    style[i++]='_';
  if (axis.axShowArrow)
    style[i++]='T';
  gr->Axis(style);  

  iMglPlotDrawAxisLabel(ih, gr, dir, axis);

  // Reset to default
  if (dir == 'x')
    iMglPlotConfigAxisTicksVal(ih, gr, false);

  gr->SetRotatedText(false);
}

static void iMglPlotDrawAxes(Ihandle* ih, mglGraph *gr)
{
  // Draw axes lines, ticks and ticks labels
  if(ih->data->axisX.axShow)  
    iMglPlotDrawAxis(ih, gr, 'x', ih->data->axisX);
  if(ih->data->axisY.axShow)  
    iMglPlotDrawAxis(ih, gr, 'y', ih->data->axisY);
  if(ih->data->axisZ.axShow && iMglPlotIsView3D(ih))
    iMglPlotDrawAxis(ih, gr, 'z', ih->data->axisZ);

  // Reset to default
  gr->SetFunc(NULL, NULL, NULL);

  if (iupAttribGetBoolean(ih, "COLORBAR"))
  {
    int pos = 0; // RIGHT
    char* value = iupAttribGetStr(ih, "COLORBARPOS");
	  if (iupStrEqualNoCase(value,"LEFT"))	pos = 1;
	  else if(iupStrEqualNoCase(value,"TOP"))	pos = 2;
	  else if(iupStrEqualNoCase(value,"BOTTOM"))	pos = 3;

    float w = 1.0f, h = 1.0f;
    float x = pos==0?1.0f:0;
    float y = pos==2?1.0f:0;
    if (pos==1 || pos==0)
    {
      iMglPlotConfigColor(ih, gr, ih->data->axisX.axColor);
      iMglPlotConfigFont(ih, gr, ih->data->axisX.axTickFontStyle, ih->data->axisX.axTickFontSizeFactor);
      y = 0.15f;
      h = 0.7f;
    }
    else
    {
      iMglPlotConfigColor(ih, gr, ih->data->axisY.axColor);
      iMglPlotConfigFont(ih, gr, ih->data->axisY.axTickFontStyle, ih->data->axisY.axTickFontSizeFactor);
      x = 0.15f;
      w = 0.7f;
    }

    gr->Colorbar(pos, x, y, w, h);
  }
}

static void iMglPlotDrawGrid(Ihandle* ih, mglGraph *gr)
{
  char pen[10], grid[10];
  iMglPlotConfigColor(ih, gr, ih->data->gridColor);
  iMglPlotConfigPen(gr, pen, ih->data->gridLineStyle, 1);
  iupStrLower(grid, ih->data->gridShow);
  gr->Grid(grid, pen);
}

static void iMglPlotDrawBox(Ihandle* ih, mglGraph *gr)
{
  iMglPlotConfigColor(ih, gr, ih->data->boxColor);
  gr->Box(iMglPlotGetColorDefault(ih, ih->data->boxColor), ih->data->boxTicks);
}

static char* iMglPlotMakeFormatString(float inValue, float range) 
{
  if (inValue<0)
    inValue = range/(-inValue);

  if (inValue > (float)1.0e4 || inValue < (float)1.0e-3)
    return "%.1e";
  else 
  {
    int thePrecision = 0;
    if (inValue<1) 
    {
      float theSpan = inValue;
      while (theSpan<1 && thePrecision<10) 
      {
        thePrecision++;
        theSpan *= 10;
      }
      if (thePrecision<10)
        thePrecision++;
    }

    switch (thePrecision)
    {
    case 1: return "%.1f";
    case 2: return "%.2f";
    case 3: return "%.3f";
    case 4: return "%.4f";
    case 5: return "%.5f";
    case 6: return "%.6f";
    case 7: return "%.7f";
    case 8: return "%.8f";
    case 9: return "%.9f";
    default: return "%.0f";
    }
  }
}

static void iMglPlotDrawValues(Ihandle* ih, IdataSet* ds, mglGraph *gr)
{
  int i;
  char text[256];
  char format[256];
  mglPoint p;
  char* xformat = iupAttribGetStr(ih, "AXS_XTICKFORMAT");
  char* yformat = iupAttribGetStr(ih, "AXS_YTICKFORMAT");
  char* zformat = iupAttribGetStr(ih, "AXS_ZTICKFORMAT");
  if (!xformat) xformat = iMglPlotMakeFormatString(gr->dx, gr->Max.x-gr->Min.x);
  if (!yformat) yformat = iMglPlotMakeFormatString(gr->dy, gr->Max.y-gr->Min.y);
  if (!zformat) zformat = iMglPlotMakeFormatString(gr->dz, gr->Max.z-gr->Min.z);

  iMglPlotConfigColor(ih, gr, ds->dsColor);
  iMglPlotConfigFont(ih, gr, ih->data->legendFontStyle, ih->data->legendFontSizeFactor);

  float* dsXPoints = ds->dsX->a;
  if (ds->dsDim == 3)
  {
    float* dsYPoints = ds->dsY->a;
    float* dsZPoints = ds->dsZ->a;
    mglPoint d = mglPoint(gr->Max.x-gr->Min.x, gr->Max.y-gr->Min.y, gr->Max.z-gr->Min.z);

    sprintf(format, "(%s, %s, %s)", xformat, yformat, zformat);

    for(i = 0; i < ds->dsCount; i++)
    {
      sprintf(text, format, dsXPoints[i], dsYPoints[i], dsZPoints[i]);
      p = mglPoint(dsXPoints[i], dsYPoints[i], dsZPoints[i]);
      
      // Check if all characters are loaded
      if (ih->data->useMakeFont)
        ih->data->makeFont->mglMakeFontSearchGlyph(gr, text);

      gr->Puts(p, d, text, 0, -1);
    }
  }
  else if (ds->dsDim == 2)
  {
    float* dsYPoints = ds->dsY->a;
    mglPoint d = mglPoint(gr->Max.x-gr->Min.x, gr->Max.y-gr->Min.y);

    sprintf(format, "(%s, %s)", xformat, yformat);

    for(i = 0; i < ds->dsCount; i++)
    {
      sprintf(text, format, dsXPoints[i], dsYPoints[i]);
      p = mglPoint(dsXPoints[i], dsYPoints[i]);
      
      // Check if all characters are loaded
      if (ih->data->useMakeFont)
        ih->data->makeFont->mglMakeFontSearchGlyph(gr, text);
      
      gr->Puts(p, d, text, 0, -1);
    }
  }
  else
  {
    sprintf(format, "(%%d, %s)", xformat);
    mglPoint d = mglPoint(gr->Max.x-gr->Min.x, gr->Max.y-gr->Min.y);

    for(i = 0; i < ds->dsCount; i++)
    {
      sprintf(text, format, i, dsXPoints[i]);
      p = mglPoint((float)i, dsXPoints[i]);
      
      // Check if all characters are loaded
      if (ih->data->useMakeFont)
        ih->data->makeFont->mglMakeFontSearchGlyph(gr, text);
      
      gr->Puts(p, d, text, 0, -1);
    }
  }
}

static float iMglPlotGetAttribFloatNAN(Ihandle* ih, const char* name)
{
  float val = NAN;
  iupStrToFloat(iupAttribGet(ih, name), &val);
  return val;
}

static bool iMglPlotHasx10(float min, float max)
{
	if(fabs(max-min)<0.01*fabs(min))
	{
		float v = fabs(max-min);
		if (v>10000.f || v<1e-4f)
      return true;
  }
	else
	{
		float v = fabs(max)>fabs(min)?fabs(max):fabs(min);
		if(v>10000.f || v<1e-4f)
      return true;
  }
  return false;
}

static void iMglPlotConfigPlotArea(Ihandle* ih, mglGraph *gr)
{
  // The default adds spaces at left, top, right and bottom
  float x1=0, x2=1.0f, y1=0, y2=1.0f;

  // So we remove the spaces if not used

  // Left
  if (!isnan(ih->data->axisY.axOrigin))
    x1 = -0.16f;  // Axis is crossed
  else if (!iupAttribGetStr(ih, "AXS_YLABEL"))  // no label
  {
    if (!ih->data->axisY.axTickShowValues)
      x1 = -0.20f;  // not crossed, no label, no ticks values
    else if (ih->data->axisY.axTickValuesRotation)
      x1 = -0.15f;  // not crossed, no label, with vertical ticks values
    else
      x1 = -0.10f;  // not crossed, no label, with horizontal ticks values
  }
  else if (ih->data->axisY.axLabelRotation)
    x1 = -0.10f; // not crossed and with vertical label
  else
    x1 = -0.01f; // not crossed and with horizontal label

  // Right
  if (iMglPlotHasx10(ih->data->axisX.axMin, ih->data->axisX.axMax))  // X Axis Min-Max
    x2 = 1.05f;  // with x10 notation
  else
    x2 = 1.15f;  // no notation

  // Bottom
  if (!isnan(ih->data->axisX.axOrigin))  
    y1 = -0.20f;  // Axis is crossed
  else if (!iupAttribGetStr(ih, "AXS_XLABEL"))  
  {
    if (!ih->data->axisX.axTickShowValues)
      y1 = -0.20f;  // not crossed, no label, no ticks values
    else
      y1 = -0.15f;  // not crossed, no label, with ticks values
  }
  else
    y1 = -0.10f;  // not crossed and with label

  // Top
  if (!iupAttribGetStr(ih, "TITLE"))
    y2 = 1.15f;  // no title
  else
    y2 = 1.05f;  // with title

  // Colorbar
  if (iupAttribGetBoolean(ih, "COLORBAR"))
  {
    char* value = iupAttribGetStr(ih, "COLORBARPOS");
    if (iupStrEqualNoCase(value,"LEFT"))
      x1 = 0.05f;  // includes also space for axis
    else if(iupStrEqualNoCase(value,"TOP"))
      y2 = 0.9f;   // includes also space for title
    else if(iupStrEqualNoCase(value,"BOTTOM"))
      y1 = 0.05f;  // includes also space for axis
    else // RIGHT
      x2 = 0.9f;   // includes also space for notation
  }

  // Only one plot using all viewport
	gr->InPlot(x1, x2, y1, y2, false);
}

static void iMglPlotConfigView(Ihandle* ih, mglGraph *gr)
{
  iMglPlotConfigPlotArea(ih, gr);

  // Transparency
  gr->Alpha(true);  // Necessary so Anti-alias can work.
  if (ih->data->transparent)
  {
    gr->SetTransparent(true);
    gr->SetAlphaDef(ih->data->alpha);  // Should be used only if transparent is enabled
  }
  else
  {
    gr->SetTransparent(false);
    gr->SetAlphaDef(1.0f);
  }

  // Zoom and Pan
  gr->Zoom(ih->data->x1, ih->data->y1, ih->data->x2, ih->data->y2);

  if (iMglPlotIsView3D(ih))
  {
    if (iupAttribGetBoolean(ih, "LIGHT"))  /* Default: FALSE */
      gr->Light(true);
    else
      gr->Light(false);

    gr->Rotate(ih->data->rotX, ih->data->rotZ, ih->data->rotY);  // Notice that rotZ is the second parameter

    if (ih->data->rotX!=0 || ih->data->rotY!=0 || ih->data->rotZ)
      gr->PlotFactor += 0.3f;  // Add more space when rotating a 3D graph
  }
}

static void iMglPlotConfigDataGrid(mglGraph *gr, IdataSet* ds, char* style)
{ 
  // Plot affected by SelectPen
  iMglPlotConfigDataSetLineMark(ds, gr, style);

  strcat(style, "#"); // grid
}

static void iMglPlotConfigColorScheme(Ihandle* ih, char* style)
{
  char* value = iupAttribGetStr(ih, "COLORSCHEME");
  if (value)
  {
    strcpy(style, value);
    strcat(style, ":");
  }
}

static void iMglPlotDrawVolumetricData(Ihandle* ih, mglGraph *gr, IdataSet* ds)
{               
  char style[64] = "";
  char* value;

  // All plots here are affected by SetScheme
  iMglPlotConfigColorScheme(ih, style);

  int nx = (int)ds->dsX->nx;
  int ny = (int)ds->dsX->ny;
  int nz = (int)ds->dsX->nz;
  mglData xx(nx), yy(ny), zz(nz);
  xx.Fill(-1.0f, +1.0f);
  yy.Fill(-1.0f, +1.0f);
  zz.Fill(-1.0f, +1.0f);

  if (iupStrEqualNoCase(ds->dsMode, "VOLUME_ISOSURFACE"))
  {
    if (iupAttribGetBoolean(ih, "DATAGRID"))   // Default false
      iMglPlotConfigDataGrid(gr, ds, style);  // Here means wire

    value = iupAttribGet(ih, "ISOVALUE");
    if (value)
    {
      float isovalue;
      if (iupStrToFloat(value, &isovalue))
        gr->Surf3(isovalue, xx, yy, zz, *ds->dsX, style);   // only 1 isosurface
    }
    else
    {
      int isocount = iupAttribGetInt(ih, "ISOCOUNT");  //Default 3
      gr->Surf3(xx, yy, zz, *ds->dsX, style, isocount); // plots N isosurfaces, from Cmin to Cmax
    }
  }
  else if (iupStrEqualNoCase(ds->dsMode, "VOLUME_DENSITY"))
  {
    if (iupAttribGetBoolean(ih, "DATAGRID"))  // Default false
      iMglPlotConfigDataGrid(gr, ds, style);  // Here means wire

    char* slicedir = iupAttribGetStr(ih, "SLICEDIR"); //Default "XYZ"
    int project = iupAttribGetBoolean(ih, "PROJECT");  //Default false
    if (project)
    {
      float valx = iMglPlotGetAttribFloatNAN(ih, "PROJECTVALUEX");
      float valy = iMglPlotGetAttribFloatNAN(ih, "PROJECTVALUEY");
      float valz = iMglPlotGetAttribFloatNAN(ih, "PROJECTVALUEZ");
      if (tolower(*slicedir)=='x') { gr->DensX(ds->dsX->Sum("x"), style, valx); slicedir++; }
      if (tolower(*slicedir)=='y') { gr->DensY(ds->dsX->Sum("y"), style, valy); slicedir++; }
      if (tolower(*slicedir)=='z') { gr->DensZ(ds->dsX->Sum("z"), style, valz); slicedir++; }
    }
    else
    {
      int slicex = iupAttribGetInt(ih, "SLICEX");  //Default -1 (central)
      int slicey = iupAttribGetInt(ih, "SLICEY");  //Default -1 (central)
      int slicez = iupAttribGetInt(ih, "SLICEZ");  //Default -1 (central)
      if (tolower(*slicedir)=='x') { gr->Dens3(xx, yy, zz, *ds->dsX, 'x', slicex, style); slicedir++; }
      if (tolower(*slicedir)=='y') { gr->Dens3(xx, yy, zz, *ds->dsX, 'y', slicey, style); slicedir++; }
      if (tolower(*slicedir)=='z') { gr->Dens3(xx, yy, zz, *ds->dsX, 'z', slicez, style); slicedir++; }
    }
  }
  else if (iupStrEqualNoCase(ds->dsMode, "VOLUME_CONTOUR"))
  {
    if (iupAttribGetBoolean(ih, "DATAGRID"))
      iMglPlotConfigDataGrid(gr, ds, style);  // Here means wire

    int contourcount = iupAttribGetInt(ih, "CONTOURCOUNT");  //Default 7, plots N countours, from Cmin to Cmax
    int countourfilled = iupAttribGetBoolean(ih, "CONTOURFILLED");  //Default false
    char* slicedir = iupAttribGetStr(ih, "SLICEDIR"); //Default "XYZ"
    int project = iupAttribGetBoolean(ih, "PROJECT");  //Default false
    if (project)
    {
      float valx = iMglPlotGetAttribFloatNAN(ih, "PROJECTVALUEX");
      float valy = iMglPlotGetAttribFloatNAN(ih, "PROJECTVALUEY");
      float valz = iMglPlotGetAttribFloatNAN(ih, "PROJECTVALUEZ");
      if (countourfilled)
      {
        if (tolower(*slicedir)=='x') { gr->ContFX(ds->dsX->Sum("x"), style, valx, contourcount); slicedir++; }
        if (tolower(*slicedir)=='y') { gr->ContFY(ds->dsX->Sum("y"), style, valy, contourcount); slicedir++; }
        if (tolower(*slicedir)=='z') { gr->ContFZ(ds->dsX->Sum("z"), style, valz, contourcount); slicedir++; }
      }
      else
      {
        if (tolower(*slicedir)=='x') { gr->ContX(ds->dsX->Sum("x"), style, valx, contourcount); slicedir++; }
        if (tolower(*slicedir)=='y') { gr->ContY(ds->dsX->Sum("y"), style, valy, contourcount); slicedir++; }
        if (tolower(*slicedir)=='z') { gr->ContZ(ds->dsX->Sum("z"), style, valz, contourcount); slicedir++; }
      }
    }
    else
    {
      int slicex = iupAttribGetInt(ih, "SLICEX");  //Default -1 (central)
      int slicey = iupAttribGetInt(ih, "SLICEY");  //Default -1 (central)
      int slicez = iupAttribGetInt(ih, "SLICEZ");  //Default -1 (central)

      if (countourfilled)
      {
        if (tolower(*slicedir)=='x') { gr->ContF3(xx, yy, zz, *ds->dsX, 'x', slicex, style, contourcount); slicedir++; }
        if (tolower(*slicedir)=='y') { gr->ContF3(xx, yy, zz, *ds->dsX, 'y', slicey, style, contourcount); slicedir++; }
        if (tolower(*slicedir)=='z') { gr->ContF3(xx, yy, zz, *ds->dsX, 'z', slicez, style, contourcount); slicedir++; }
      }
      else
      {
        // Plot affected by SelectPen
        iMglPlotConfigDataSetLineMark(ds, gr, style);

        if (tolower(*slicedir)=='x') { gr->Cont3(xx, yy, zz, *ds->dsX, 'x', slicex, style, contourcount); slicedir++; }
        if (tolower(*slicedir)=='y') { gr->Cont3(xx, yy, zz, *ds->dsX, 'y', slicey, style, contourcount); slicedir++; }
        if (tolower(*slicedir)=='z') { gr->Cont3(xx, yy, zz, *ds->dsX, 'z', slicez, style, contourcount); slicedir++; }
      }
    }
  }
  else if (iupStrEqualNoCase(ds->dsMode, "VOLUME_CLOUD"))
  {
    int cubes = iupAttribGetBoolean(ih, "CLOUDCUBES");  //Default true
    if (cubes)
      gr->Cloud(xx, yy, zz, *ds->dsX, style, -1);   // Use AlphaDef
    else
      gr->CloudP(xx, yy, zz, *ds->dsX, style, -1);  // Use AlphaDef
  }
}

static void iMglPlotDrawPlanarData(Ihandle* ih, mglGraph *gr, IdataSet* ds)
{               
  char style[64] = "";

  // All plots here are affected by SetScheme
  iMglPlotConfigColorScheme(ih, style);

  int nx = (int)ds->dsX->nx;
  int ny = (int)ds->dsX->ny;
  mglData xx(nx), yy(ny);
  xx.Fill(-1.0f, +1.0f);
  yy.Fill(-1.0f, +1.0f);

  if (iupStrEqualNoCase(ds->dsMode, "PLANAR_MESH"))
  {
    // Plot affected by SelectPen
    iMglPlotConfigDataSetLineMark(ds, gr, style);

    gr->Mesh(xx, yy, *ds->dsX, style);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_FALL"))
  {
    // Plot affected by SelectPen
    iMglPlotConfigDataSetLineMark(ds, gr, style);

    char* falldir = iupAttribGetStr(ih, "DIR"); //Default "Y"
    if (tolower(*falldir) == 'x')
      { style[0] = 'x'; style[1] = 0; }

    gr->Fall(xx, yy, *ds->dsX, style);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_BELT"))
  {
    char* beltdir = iupAttribGetStr(ih, "DIR"); //Default "Y"
    if (tolower(*beltdir) == 'x')
      { style[0] = 'x'; style[1] = 0; }

    gr->Belt(xx, yy, *ds->dsX, style);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_SURFACE"))
  {
    if (iupAttribGetBoolean(ih, "DATAGRID"))   // Default false
      iMglPlotConfigDataGrid(gr, ds, style);

    gr->Surf(xx, yy, *ds->dsX, style);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_BOXES"))
  {
    if (iupAttribGetBoolean(ih, "DATAGRID"))   // Default false
      iMglPlotConfigDataGrid(gr, ds, style);  // Here means box lines

    gr->Boxs(xx, yy, *ds->dsX, style);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_TILE"))
  {
    gr->Tile(xx, yy, *ds->dsX, style);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_DENSITY"))
  {
    float val = iMglPlotGetAttribFloatNAN(ih, "PLANARVALUE");   // Default NAN
    gr->Dens(xx, yy, *ds->dsX, style, val);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_CONTOUR"))
  {
    int contourcount = iupAttribGetInt(ih, "CONTOURCOUNT");  //Default 7, plots N countours, from Cmin to Cmax
    int countourfilled = iupAttribGetBoolean(ih, "CONTOURFILLED");  //Default false
    float val = iMglPlotGetAttribFloatNAN(ih, "PLANARVALUE");  // Default NAN
    if (countourfilled)
      gr->ContF(xx, yy, *ds->dsX, style, contourcount, val);
    else
    {
      // Plot affected by SelectPen
      iMglPlotConfigDataSetLineMark(ds, gr, style);

      char* countourlabels = iupAttribGetStr(ih, "CONTOURLABELS");  //Default NULL
      if (iupStrEqualNoCase(countourlabels, "BELLOW"))
        strcat(style, "t");
      else if (iupStrEqualNoCase(countourlabels, "ABOVE"))
        strcat(style, "T");

      gr->Cont(xx, yy, *ds->dsX, style, contourcount, val);
    }
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_AXIALCONTOUR"))
  {
    int axialcount = iupAttribGetInt(ih, "AXIALCOUNT");  //Default 3, plots N countours, from Cmin to Cmax
    gr->Axial(xx, yy, *ds->dsX, style, axialcount);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "PLANAR_GRADIENTLINES"))
  {
    int gradlinescount = iupAttribGetInt(ih, "GRADLINESCOUNT");  //Default 5
    float val = iMglPlotGetAttribFloatNAN(ih, "PLANARVALUE");  // Default NAN
    gr->Grad(xx, yy, *ds->dsX, style, gradlinescount, val);
  }
}

static void iMglPlotDrawLinearData(Ihandle* ih, mglGraph *gr, IdataSet* ds)
{               
  char style[64] = "";

  iMglPlotConfigColor(ih, gr, ds->dsColor);

  int nx = (int)ds->dsX->nx;
  mglData xx(nx);
  xx.Fill(0,(float)nx-1);

  // All plots here are affected by SelectPen
  // All plots here are affected by SetPalette

  // TODO Workaround, we changed SetPal for this to work
  gr->SetPalNum(1);
  gr->SetPalColor(0, ds->dsColor.r, ds->dsColor.g, ds->dsColor.b); // No problem using it here because 
                                                                   // dsColor should never has NAN values

  if (iupStrEqualNoCase(ds->dsMode, "LINE") ||
      iupStrEqualNoCase(ds->dsMode, "MARKLINE") ||
      iupStrEqualNoCase(ds->dsMode, "MARK"))
  {
    if (!iupStrEqualNoCase(ds->dsMode, "LINE"))  // same as setting marks=yes
      ds->dsShowMarks = true;
    if (iupStrEqualNoCase(ds->dsMode, "MARK"))   // Same as setting linestyle=none
      ds->dsLineStyle = ' '; // No line

    // Plot affected by SelectPen
    iMglPlotConfigDataSetLineMark(ds, gr, style);

    if (ds->dsDim == 3)
      gr->Plot(*ds->dsX, *ds->dsY, *ds->dsZ, style);
    else if (ds->dsDim == 2)
      gr->Plot(*ds->dsX, *ds->dsY, style, 0);
    else
      gr->Plot(xx, *ds->dsX, style, 0);  // At Z=0
  }
  else if (iupStrEqualNoCase(ds->dsMode, "RADAR"))
  {
    // Plot affected by SelectPen
    iMglPlotConfigDataSetLineMark(ds, gr, style);

    float radarshift = iupAttribGetFloat(ih, "RADARSHIFT");   // Default -1

    if (iupAttribGetBoolean(ih, "DATAGRID"))  //Default false
      iMglPlotConfigDataGrid(gr, ds, style);

    gr->Radar(*ds->dsX, style, radarshift);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "AREA"))
  {
    // NOT affected by SelectPen

    if (ds->dsDim == 3)
      gr->Area(*ds->dsX, *ds->dsY, *ds->dsZ, style);
    else if (ds->dsDim == 2)
      gr->Area(*ds->dsX, *ds->dsY, style, 0);
    else
      gr->Area(xx, *ds->dsX, style, 0);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "BAR"))
  {
    // NOT affected by SelectPen
    float barwidth = iupAttribGetFloat(ih, "BARWIDTH");   // Default 0.7
    gr->SetBarWidth(barwidth);

    if (iupAttribGetBoolean(ih, "DATAGRID"))  //Default false
      iMglPlotConfigDataGrid(gr, ds, style);

    // Each bar could has a different color using a scheme
    // But we can NOT use scheme here because we changed SetPal
    //iMglPlotConfigColorScheme(ih, style);

    // To avoid hole bars clipped when touching the bounding box
    gr->SetCut(false); 

    if (ds->dsDim == 3)
      gr->Bars(*ds->dsX, *ds->dsY, *ds->dsZ, style);
    else if (ds->dsDim == 2)
      gr->Bars(*ds->dsX, *ds->dsY, style, 0);
    else
      gr->Bars(xx, *ds->dsX, style, 0);

    gr->SetCut(true); 
  }
  else if (iupStrEqualNoCase(ds->dsMode, "BARHORIZONTAL"))
  {
    // NOT affected by SelectPen
    float barwidth = iupAttribGetFloat(ih, "BARWIDTH");   // Default 0.7
    gr->SetBarWidth(barwidth);

    // Each bar could has a different color using a scheme
    // But we can NOT use scheme here because we changed SetPal
    //iMglPlotConfigColorScheme(ih, style);

    if (iupAttribGetBoolean(ih, "DATAGRID"))  //Default false
      iMglPlotConfigDataGrid(gr, ds, style);

    // To avoid hole bars clipped when touching the bounding box
    gr->SetCut(false); 

    if (ds->dsDim == 2)
      gr->Barh(*ds->dsY, *ds->dsX, style, 0);
    else
      gr->Barh(xx, *ds->dsX, style, 0);

    gr->SetCut(true); 
  }
  else if (iupStrEqualNoCase(ds->dsMode, "STEP"))
  {
    // Plot affected by SelectPen
    iMglPlotConfigDataSetLineMark(ds, gr, style);

    if (ds->dsDim == 3)
      gr->Step(*ds->dsX, *ds->dsY, *ds->dsZ, style);
    else if (ds->dsDim == 2)
      gr->Step(*ds->dsX, *ds->dsY, style, 0);
    else
      gr->Step(xx, *ds->dsX, style, 0);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "STEM"))
  {
    // Plot affected by SelectPen
    iMglPlotConfigDataSetLineMark(ds, gr, style);

    if (ds->dsDim == 3)
      gr->Stem(*ds->dsX, *ds->dsY, *ds->dsZ, style);
    else if (ds->dsDim == 2)
      gr->Stem(*ds->dsX, *ds->dsY, style, 0);
    else
      gr->Stem(xx, *ds->dsX, style, 0);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "CHART"))
  {
    // NOT affected by SelectPen

    // Chart does not use SetPalette,
    // so we use the scheme attribute as a list of colors.
    iMglPlotConfigColorScheme(ih, style);

    if (iupAttribGetBoolean(ih, "DATAGRID"))  //Default false
      iMglPlotConfigDataGrid(gr, ds, style);

    int piechart = iupAttribGetBoolean(ih, "PIECHART");  //Default false
    if (piechart)
    {
      //TODO: this equation works only if x in the interval [-1, 1]
      gr->SetFunc("(y+1)/2*cos(pi*x)", "(y+1)/2*sin(pi*x)");

      // This box has to be here, so it will be affected by the transformation
      if (ih->data->Box)
        iMglPlotDrawBox(ih, gr);

      gr->Chart(*ds->dsX, style);
      gr->SetFunc(NULL, NULL);
    }
    else
      gr->Chart(*ds->dsX, style);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "CRUST"))
  {
    // NOT affected by SelectPen
    // NOT affected by SetPalette

    // Affected by SetScheme
    iMglPlotConfigColorScheme(ih, style);

    float radius = iupAttribGetFloat(ih, "CRUSTRADIUS");   // Default 0

    if (iupAttribGetBoolean(ih, "DATAGRID"))  //Default false
      iMglPlotConfigDataGrid(gr, ds, style);

    if (ds->dsDim == 3)
      gr->Crust(*ds->dsX, *ds->dsY, *ds->dsZ, style, radius);
  }
  else if (iupStrEqualNoCase(ds->dsMode, "DOTS"))
  {
    // NOT affected by SelectPen
    // NOT affected by SetPalette

    // Affected by SetScheme
    iMglPlotConfigColorScheme(ih, style);

    if (ds->dsDim == 3)
      gr->Dots(*ds->dsX, *ds->dsY, *ds->dsZ, style);
  }
}

static void iMglPlotDrawData(Ihandle* ih, mglGraph *gr)
{
  int i;
  for(i = 0; i < ih->data->dataSetCount; i++)
  {
    IdataSet* ds = &ih->data->dataSet[i];

    if (iMglPlotIsVolumetricData(ds))
      iMglPlotDrawVolumetricData(ih, gr, ds);
    else if (iMglPlotIsPlanarData(ds))
      iMglPlotDrawPlanarData(ih, gr, ds);
    else
      iMglPlotDrawLinearData(ih, gr, ds);

    if(ds->dsShowValues && !iMglPlotIsPlanarOrVolumetricData(ds))
      iMglPlotDrawValues(ih, ds, gr);  /* Print values near the samples */
  }
}

static void iMglPlotDrawLegend(Ihandle* ih, mglGraph *gr)
{
  int i;
  char style[64] = "";

  gr->ClearLegend();

  for(i = 0; i < ih->data->dataSetCount; i++)
  {
    IdataSet* ds = &ih->data->dataSet[i];

    style[0] = 0;
    iMglPlotConfigDataSetLineMark(ds, gr, style);
    iMglPlotConfigDataSetColor(ds, style);
    gr->AddLegend(ds->dsLegend, style);
  }

  gr->SetLegendBox(ih->data->legendBox);

  // Draw legend of accumulated strings
  iMglPlotConfigColor(ih, gr, ih->data->legendColor);
  iMglPlotConfigFont(ih, gr, ih->data->legendFontStyle, ih->data->legendFontSizeFactor);
  gr->Legend(ih->data->legendPosition, NULL, -1, 0.08f);
}

static void iMglPlotDrawTitle(Ihandle* ih, mglGraph *gr, const char* title)
{
  iMglPlotConfigColor(ih, gr, ih->data->titleColor);
  iMglPlotConfigFont(ih, gr, ih->data->titleFontStyle, ih->data->titleFontSizeFactor);
      
  // Check if all characters are loaded
  if (ih->data->useMakeFont)
    ih->data->makeFont->mglMakeFontSearchGlyph(gr, title);
  
  gr->Title(title, NULL, -1);
}

static void iMglPlotDrawPlot(Ihandle* ih, mglGraph *gr)
{
  // Since this function will be used to draw on screen and
  // on metafile and bitmaps, all mglGraph control must be done here
  // and can NOT be done inside the attribute methods

  gr->Message = &(ih->data->ErrorMessage[0]);
  ih->data->ErrorMessage[0] = 0;

  gr->DefaultPlotParam();

  gr->Clf(ih->data->bgColor); /* Clear */

  iMglPlotConfigView(ih, gr);

  iMglPlotConfigFontDef(ih, gr);

  iMglPlotConfigAxesRange(ih, gr);

  iupAttribSetStr(ih, "_IUP_MGLPLOT_GRAPH", (char*)gr);

  IFn cb = IupGetCallback(ih, "PREDRAW_CB");
  if (cb)
    cb(ih);

  iMglPlotDrawAxes(ih, gr);

  if(ih->data->gridShow)
    iMglPlotDrawGrid(ih, gr);

  int piechart = iupAttribGetBoolean(ih, "PIECHART");  //Default false
  if (ih->data->Box && !piechart)
    iMglPlotDrawBox(ih, gr);

  iMglPlotDrawData(ih, gr);

  if(ih->data->legendShow)
    iMglPlotDrawLegend(ih, gr);

  char* value = iupAttribGetStr(ih, "TITLE");
  if (value)
    iMglPlotDrawTitle(ih, gr, value);

  cb = IupGetCallback(ih, "POSTDRAW_CB");
  if (cb)
    cb(ih);

  iupAttribSetStr(ih, "_IUP_MGLPLOT_GRAPH", NULL);
}

static void iMglPlotRepaint(Ihandle* ih, int force, int flush)
{
  if (!IupGLIsCurrent(ih))
    force = 1;

  IupGLMakeCurrent(ih);

  if (force || ih->data->redraw)
  {
    /* update render */
    iMglPlotDrawPlot(ih, ih->data->mgl);  /* Draw the graphics plot */

    ih->data->mgl->Finish();
    ih->data->redraw = false;
  }

  if (flush)
  {
    if (!ih->data->opengl)
    {
	    const unsigned char *rgb = ih->data->mgl->GetBits();
	    if (rgb)
        glDrawPixels(ih->data->w, ih->data->h, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    }

    IupGLSwapBuffers(ih);
  }
}


/******************************************************************************
 Attribute Methods
******************************************************************************/

static int iMglPlotSetColor(Ihandle* ih, const char* value, mglColor& color)
{
  unsigned char rr, gg, bb;

  if (!value)
  {
    color.Set(NAN, NAN, NAN);
    ih->data->redraw = true;
  }
  else if (iupStrToRGB(value, &rr, &gg, &bb))
  {
    color.Set(iRecon(rr), iRecon(gg), iRecon(bb));
    ih->data->redraw = true;
  }

  return 0;
}

static char* iMglPlotGetColorAttrib(const mglColor& color)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d %d %d", iQuant(color.r), iQuant(color.g), iQuant(color.b));
  return buffer;
}

static int iMglPlotSetBoolean(Ihandle* ih, const char* value, bool& num)
{
  bool old_num = num;

  if (iupStrBoolean(value))
    num = true;
  else 
    num = false;

  if (old_num != num)
    ih->data->redraw = true;

  return 0;
}

static int iMglPlotSetInt(Ihandle* ih, const char* value, int& num)
{
  int old_num = num;

  iupStrToInt(value, &num);

  if (old_num != num)
    ih->data->redraw = true;

  return 0;
}

static int iMglPlotSetFloat(Ihandle* ih, const char* value, float& num)
{
  float old_num = num;

  iupStrToFloat(value, &num);

  if (old_num != num)
    ih->data->redraw = true;

  return 0;
}

static char* iMglPlotGetFloat(float num)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%g", num);
  return buffer;
}

static char* iMglPlotGetInt(int num)
{
  char* buffer = iupStrGetMemory(30);
  sprintf(buffer, "%d", num);
  return buffer;
}

static char* iMglPlotGetBoolean(bool num)
{
  if (num)
    return "YES";
  else
    return "NO";
}

static int iMglPlotSetResetAttrib(Ihandle* ih, const char* value)
{
  (void)value;  /* not used */
  iMglPlotReset(ih);

  // Some attributes are stored in the hash table
  iupAttribSetStr(ih, "TITLE", NULL);

  iupAttribSetStr(ih, "DATAGRID", NULL);
  iupAttribSetStr(ih, "PLANARVALUE", NULL);
  iupAttribSetStr(ih, "GRADLINESCOUNT", NULL);
  iupAttribSetStr(ih, "AXIALCOUNT", NULL);
  iupAttribSetStr(ih, "CONTOURFILLED", NULL);
  iupAttribSetStr(ih, "CONTOURLABELS", NULL);
  iupAttribSetStr(ih, "CONTOURCOUNT", NULL);
  iupAttribSetStr(ih, "DIR", NULL);
  iupAttribSetStr(ih, "CLOUDCUBES", NULL);
  iupAttribSetStr(ih, "SLICEX", NULL);
  iupAttribSetStr(ih, "SLICEY", NULL);
  iupAttribSetStr(ih, "SLICEZ", NULL);
  iupAttribSetStr(ih, "SLICEDIR", NULL);
  iupAttribSetStr(ih, "PROJECTVALUEX", NULL);
  iupAttribSetStr(ih, "PROJECTVALUEY", NULL);
  iupAttribSetStr(ih, "PROJECTVALUEZ", NULL);
  iupAttribSetStr(ih, "PROJECT", NULL);
  iupAttribSetStr(ih, "ISOCOUNT", NULL);
  iupAttribSetStr(ih, "BARWIDTH", NULL);
  iupAttribSetStr(ih, "RADARSHIFT", NULL);
  iupAttribSetStr(ih, "PIECHART", NULL);
  iupAttribSetStr(ih, "CRUSTRADIUS", NULL);

  iupAttribSetStr(ih, "LIGHT", NULL);
  iupAttribSetStr(ih, "COLORSCHEME", NULL);

  iupAttribSetStr(ih, "COLORBAR", NULL);
  iupAttribSetStr(ih, "COLORBARPOS", NULL);
  iupAttribSetStr(ih, "COLORBARRANGE", NULL);
  iupAttribSetStr(ih, "COLORBARAXISTICKS", NULL);

  iupAttribSetStr(ih, "AXS_XLABEL", NULL);
  iupAttribSetStr(ih, "AXS_YLABEL", NULL);
  iupAttribSetStr(ih, "AXS_ZLABEL", NULL);
  iupAttribSetStr(ih, "AXS_XTICKFORMAT", NULL);
  iupAttribSetStr(ih, "AXS_YTICKFORMAT", NULL);
  iupAttribSetStr(ih, "AXS_ZTICKFORMAT", NULL);

  ih->data->redraw = true;
  return 0;
}

static int iMglPlotSetRedrawAttrib(Ihandle* ih, const char* value)
{
  (void)value;  /* not used */
  iMglPlotRepaint(ih, 1, 1);    /* force a full redraw here */
  return 0;
}

static int iMglPlotSetBGColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->bgColor);
}

static char* iMglPlotGetBGColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->bgColor);
}

static int iMglPlotSetFGColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->fgColor);
}

static char* iMglPlotGetFGColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->fgColor);
}

static int iMglPlotSetTitleFontSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->titleFontSizeFactor);
}

static char* iMglPlotGetTitleFontSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->titleFontSizeFactor);
}

static int iMglPlotSetFontStyle(Ihandle* ih, const char* value, int& fontstyle)
{
  int old_fontstyle = fontstyle;

  if (!value)
    fontstyle = IUP_MGLPLOT_INHERIT;
  else if (iupStrEqualNoCase(value, "PLAIN"))
    fontstyle = IUP_MGLPLOT_PLAIN;
  else if (iupStrEqualNoCase(value, "BOLD"))
    fontstyle = IUP_MGLPLOT_BOLD;
  else if (iupStrEqualNoCase(value, "ITALIC"))
    fontstyle = IUP_MGLPLOT_ITALIC;
  else if (iupStrEqualNoCase(value, "BOLDITALIC"))
    fontstyle = IUP_MGLPLOT_BOLD_ITALIC;

  if (old_fontstyle != fontstyle)
    ih->data->redraw = true;

  return 0;
}

static char* iMglPlotGetFontStyle(int fontstyle)
{
  if (fontstyle != IUP_MGLPLOT_INHERIT)
    return iMglPlotGetInt(fontstyle);
  else
    return NULL;
}

static int iMglPlotSetTitleFontStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFontStyle(ih, value, ih->data->titleFontStyle);
}

static char* iMglPlotGetTitleFontStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetFontStyle(ih->data->titleFontStyle);
}

static int iMglPlotSetTitleColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->titleColor);
}

static char* iMglPlotGetTitleColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->titleColor);
}

static int iMglPlotSetLegendFontSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->legendFontSizeFactor);
}

static char* iMglPlotGetLegendFontStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetFontStyle(ih->data->legendFontStyle);
}

static char* iMglPlotGetLegendFontSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->legendFontSizeFactor);
}

static int iMglPlotSetLegendFontStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFontStyle(ih, value, ih->data->legendFontStyle);
}

static int iMglPlotSetLegendShowAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->legendShow);
}

static char* iMglPlotGetLegendShowAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->legendShow);
}

static int iMglPlotSetLegendBoxAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->legendBox);
}

static char* iMglPlotGetLegendBoxAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->legendBox);
}

static int iMglPlotSetLegendPosAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "TOPLEFT"))
    ih->data->legendPosition = IUP_MGLPLOT_TOPLEFT;
  else if (iupStrEqualNoCase(value, "BOTTOMLEFT"))
    ih->data->legendPosition = IUP_MGLPLOT_BOTTOMLEFT;
  else if (iupStrEqualNoCase(value, "BOTTOMRIGHT"))
    ih->data->legendPosition = IUP_MGLPLOT_BOTTOMRIGHT;
  else
    ih->data->legendPosition = IUP_MGLPLOT_TOPRIGHT;

  ih->data->redraw = true;
  return 0;
}

static char* iMglPlotGetLegendPosAttrib(Ihandle* ih)
{
  char* legendpos_str[4] = {"BOTTOMLEFT", "BOTTOMRIGHT", "TOPLEFT", "TOPRIGHT"};
  return legendpos_str[ih->data->legendPosition];
}

static int iMglPlotSetLegendColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->legendColor);
}

static char* iMglPlotGetLegendColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->legendColor);
}

static int iMglPlotSetGridColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->gridColor);
}

static char* iMglPlotGetGridColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->gridColor);
}

static int iMglPlotSetBoxColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->boxColor);
}

static char* iMglPlotGetBoxColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->boxColor);
}

static int iMglPlotSetLineStyle(Ihandle* ih, const char* value, char &linestyle)
{
  char old_linestyle = linestyle;

  if (!value || iupStrEqualNoCase(value, "CONTINUOUS"))
    linestyle = '-';
  else if (iupStrEqualNoCase(value, "LONGDASHED"))
    linestyle = '|';
  else if (iupStrEqualNoCase(value, "DASHED"))
    linestyle = ';';
  else if (iupStrEqualNoCase(value, "SMALLDASHED"))
    linestyle = '=';
  else if (iupStrEqualNoCase(value, "DOTTED"))
    linestyle = ':';
  else if (iupStrEqualNoCase(value, "DASH_DOT"))
    linestyle = 'j';
  else if (iupStrEqualNoCase(value, "SMALLDASH_DOT") || 
           iupStrEqualNoCase(value, "DASH_DOT_DOT")) // for compatibility
    linestyle = 'i';
  else if (iupStrEqualNoCase(value, "NONE"))
    linestyle = ' ';
  else
    linestyle = '-';  /* reset to default */

  if (old_linestyle != linestyle)
    ih->data->redraw = true;

  return 0;
}

static char* iMglPlotGetLineStyle(char linestyle)
{
  switch(linestyle)
  {
    case ' ': return "NONE";
    case '|': return "LONGDASHED";
    case ';': return "DASHED";
    case '=': return "SMALLDASHED";
    case ':': return "DOTTED";
    case 'j': return "DASH_DOT";
    case 'i': return "SMALLDASH_DOT";
    default:  return "CONTINUOUS";
  }
}

static int iMglPlotSetGridLineStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetLineStyle(ih, value, ih->data->gridLineStyle);
}

static char* iMglPlotGetGridLineStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetLineStyle(ih->data->gridLineStyle);
}

static int iMglPlotSetGridAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "VERTICAL") || iupStrEqualNoCase(value, "X"))
    ih->data->gridShow = "X";
  else if (iupStrEqualNoCase(value, "HORIZONTAL") || iupStrEqualNoCase(value, "Y"))
    ih->data->gridShow = "Y";
  else if (iupStrEqualNoCase(value, "Z"))
    ih->data->gridShow = "Z";
  else if (iupStrEqualNoCase(value, "XY"))
    ih->data->gridShow = "XY";
  else if (iupStrEqualNoCase(value, "XZ"))
    ih->data->gridShow = "XZ";
  else if (iupStrEqualNoCase(value, "YZ"))
    ih->data->gridShow = "YZ";
  else if (iupStrEqualNoCase(value, "YES") || iupStrEqualNoCase(value, "XYZ"))
    ih->data->gridShow = "XYZ";
  else                                
    ih->data->gridShow = NULL;

  ih->data->redraw = true;
  return 0;
}

static char* iMglPlotGetGridAttrib(Ihandle* ih)
{
  if (ih->data->gridShow)
    return (char*)ih->data->gridShow;
  else
    return "NO";
}

static char* iMglPlotGetCountAttrib(Ihandle* ih)
{
  return iMglPlotGetInt(ih->data->dataSetCount);
}

static int iMglPlotSetCurrentAttrib(Ihandle* ih, const char* value)
{
  int ii;

  ih->data->dataSetCurrent = -1;

  if (iupStrToInt(value, &ii))
  {
    if (ii>=0 && ii<ih->data->dataSetCount)
    {
      ih->data->dataSetCurrent = ii;
      ih->data->redraw = true;
    }
  }

  return 0;
}

static char* iMglPlotGetCurrentAttrib(Ihandle* ih)
{
  return iMglPlotGetInt(ih->data->dataSetCurrent);
}

static void iMglPlotRemoveDataSet(IdataSet* ds)
{
  free(ds->dsLegend);
  free(ds->dsMode);

  if (ds->dsNames)
  {
    int j;
    char** dsNames = (char**)iupArrayGetData(ds->dsNames);
    int count = iupArrayCount(ds->dsNames);
    for (j=0; j<count; j++)
      free(dsNames[j]);

    iupArrayDestroy(ds->dsNames);
  }

  if (ds->dsX)
    delete ds->dsX;
  if (ds->dsY)
    delete ds->dsY;
  if (ds->dsZ)
    delete ds->dsZ;

  memset(ds, 0, sizeof(IdataSet));
}

static int iMglPlotSetRemoveAttrib(Ihandle* ih, const char* value)
{
  int ii;
  if (iupStrToInt(value, &ii))
  {
    int i;

    if(ii > ih->data->dataSetCount || ii < 0)
      return 0;

    iMglPlotRemoveDataSet(&ih->data->dataSet[ii]);

    for(i = ii; i < ih->data->dataSetCount-1; i++)
      ih->data->dataSet[i] = ih->data->dataSet[i+1];

    memset(&ih->data->dataSet[ih->data->dataSetCount-1], 0, sizeof(IdataSet));

    ih->data->dataSetCount--;

    if (ih->data->dataSetCurrent > ih->data->dataSetCount-1)
      ih->data->dataSetCurrent = ih->data->dataSetCount-1;

    ih->data->redraw = true;
  }
  return 0;
}

static int iMglPlotSetClearAttrib(Ihandle* ih, const char* value)
{
  int i;
  for(i = 0; i < ih->data->dataSetCount; i++)
    iMglPlotRemoveDataSet(&ih->data->dataSet[i]);
  ih->data->dataSetCount = 0;
  ih->data->dataSetCurrent = -1;
  ih->data->redraw = true;
  (void)value;
  return 0;
}

static int iMglPlotSetDSRearrangeAttrib(Ihandle* ih, const char* value)
{
  int i;
  IdataSet* ds;

  if (ih->data->dataSetCurrent==-1)
    return 0;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];

  // Must be Planar data
  if (ds->dsY || ds->dsZ || ds->dsX->nz != 1)
    return 0;
  if (ds->dsX->ny == 1)
    return 0;

  // Now nx!=1 and ny!=1, so rearrange data from ds->dsX to ds->dsY
  if (ds->dsX->ny == 2 || ds->dsX->ny == 3)
  {
    ds->dsCount = ds->dsX->nx;
    ds->dsDim = 2;
    ds->dsY = new mglData(ds->dsCount);
    mglData* dsX = new mglData(ds->dsCount);
    float* ax = ds->dsX->a;
    float* ay = ax + ds->dsCount;
    float* az = NULL;
    float* x = dsX->a;
    float* y = ds->dsY->a;
    float* z = NULL;
    if (ds->dsX->ny == 3)
    {
      ds->dsZ = new mglData(ds->dsCount);
      z = ds->dsZ->a;
      az = ay + ds->dsCount;
      ds->dsDim = 3;
    }

    memcpy(x, ax, ds->dsCount*sizeof(float));
    memcpy(y, ay, ds->dsCount*sizeof(float));
    if (z) memcpy(z, az, ds->dsCount*sizeof(float));

    delete ds->dsX;
    ds->dsX = dsX;
  }
  else if (ds->dsX->nx == 2 || ds->dsX->nx == 3)
  {
    ds->dsCount = ds->dsX->ny;
    ds->dsDim = 2;
    ds->dsY = new mglData(ds->dsCount);
    mglData* dsX = new mglData(ds->dsCount);
    float* a = ds->dsX->a;
    float* x = dsX->a;
    float* y = ds->dsY->a;
    float* z = NULL;
    if (ds->dsX->nx == 3)
    {
      ds->dsZ = new mglData(ds->dsCount);
      z = ds->dsZ->a;
      ds->dsDim = 3;
    }

    for (i=0; i<ds->dsCount; i++)
    {
      *x++ = *a++;
      *y++ = *a++;
      if (z) *z++ = *a++;
    }

    delete ds->dsX;
    ds->dsX = dsX;
  }
  else if (ds->dsX->nx == 1)
  {
    ds->dsCount = ds->dsX->ny;
    ds->dsX->nx = ds->dsCount;
    ds->dsX->ny = 1;
  }

  (void)value;
  return 0;
}

static int iMglPlotSetDSSplitAttrib(Ihandle* ih, const char* value)
{
  int old_current;
  IdataSet* ds;

  if (ih->data->dataSetCurrent==-1)
    return 0;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  
  // Must be Planar data
  if (ds->dsY || ds->dsZ || ds->dsX->nz != 1)
    return 0;
  if (ds->dsX->ny == 1)
    return 0;

  old_current = ih->data->dataSetCurrent;

  // Now nx!=1 and ny!=1, so rearrange data from ds->dsX to ds->dsY
  if (ds->dsX->ny == 2 || ds->dsX->ny == 3)
  {
    ds->dsCount = ds->dsX->nx;

    float* ax = ds->dsX->a;
    float* ay = ax + ds->dsCount;

    int dsy = IupMglPlotNewDataSet(ih, 1);
    IupMglPlotSet1D(ih, dsy, NULL, ay, ds->dsCount);

    if (ds->dsX->ny == 3)
    {
      float* az = ay + ds->dsCount;
      int dsz = IupMglPlotNewDataSet(ih, 1);
      IupMglPlotSet1D(ih, dsz, NULL, az, ds->dsCount);
    }

    ds->dsDim = 1;
    ds->dsX->ny = 1;
  }

  ih->data->dataSetCurrent = old_current;

  (void)value;
  return 0;
}

static int iMglPlotSetDSLineStyleAttrib(Ihandle* ih, const char* value)
{
  IdataSet* ds;

  if (ih->data->dataSetCurrent==-1)
    return 0;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotSetLineStyle(ih, value, ds->dsLineStyle);
}

static char* iMglPlotGetDSLineStyleAttrib(Ihandle* ih)
{
  IdataSet* ds;
  if (ih->data->dataSetCurrent==-1)
    return NULL;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotGetLineStyle(ds->dsLineStyle);
}

static int iMglPlotSetDSLineWidthAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->dataSetCurrent==-1)
    return 0;

  IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotSetFloat(ih, value, ds->dsLineWidth);
}

static char* iMglPlotGetDSLineWidthAttrib(Ihandle* ih)
{
  if (ih->data->dataSetCurrent==-1)
    return 0;
  else
  {
    IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
    return iMglPlotGetFloat(ds->dsLineWidth);
  }
}

static int iMglPlotSetMarkStyle(Ihandle* ih, const char* value, char& markstyle)
{
  char old_markstyle = markstyle;

  if (!value || iupStrEqualNoCase(value, "X"))
    markstyle = 'x';
  else if (iupStrEqualNoCase(value, "STAR"))
    markstyle = '*';
  else if (iupStrEqualNoCase(value, "CIRCLE"))
    markstyle = 'O';  // In fact "#o"
  else if (iupStrEqualNoCase(value, "PLUS"))
    markstyle = '+';
  else if (iupStrEqualNoCase(value, "BOX"))
    markstyle = 'S';  // In fact "#s"
  else if (iupStrEqualNoCase(value, "DIAMOND"))
    markstyle = 'D';  // In fact "#d"
  else if (iupStrEqualNoCase(value, "HOLLOW_CIRCLE"))
    markstyle = 'o';
  else if (iupStrEqualNoCase(value, "HOLLOW_BOX"))
    markstyle = 's';
  else if (iupStrEqualNoCase(value, "HOLLOW_DIAMOND"))
    markstyle = 'd';
  else
    markstyle = 'x';  /* reset to default */
  
  if (old_markstyle != markstyle)
    ih->data->redraw = true;

  return 0;
}

static int iMglPlotSetDSMarkStyleAttrib(Ihandle* ih, const char* value)
{
  IdataSet* ds;

  if (ih->data->dataSetCurrent==-1)
    return 0;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotSetMarkStyle(ih, value, ds->dsMarkStyle);
}

static char* iMglPlotGetDSMarkStyleAttrib(Ihandle* ih)
{
  IdataSet* ds;

  if (ih->data->dataSetCurrent==-1)
    return NULL;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];

  switch(ds->dsMarkStyle)
  {
    case '*': return "STAR";
    case 'O': return "CIRCLE";
    case 'x': return "X";
    case 'S': return "BOX";
    case 'D': return "DIAMOND";
    case 'o': return "HOLLOW_CIRCLE";
    case 's': return "HOLLOW_BOX";
    case 'd': return "HOLLOW_DIAMOND";
    default: return "PLUS";
  }
}

static int iMglPlotSetDSMarkSizeAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->dataSetCurrent==-1)
    return 0;

  IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotSetFloat(ih, value, ds->dsMarkSize);
}

static char* iMglPlotGetDSMarkSizeAttrib(Ihandle* ih)
{
  if (ih->data->dataSetCurrent==-1)
    return NULL;
  else
  {
    IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
    return iMglPlotGetFloat(ds->dsMarkSize);
  }
}

static int iMglPlotSetDSColorAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->dataSetCurrent==-1)
    return 0;

  IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  iMglPlotSetColor(ih, value, ds->dsColor);

  if (isnan(ds->dsColor.r) || isnan(ds->dsColor.g) || isnan(ds->dsColor.b))
    iMglPlotSetDsColorDefault(ds->dsColor, ih->data->dataSetCurrent);

  return 0;
}

static char* iMglPlotGetDSColorAttrib(Ihandle* ih)
{
  if (ih->data->dataSetCurrent==-1)
    return NULL;

  IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotGetColorAttrib(ds->dsColor);
}

static int iMglPlotSetDSModeAttrib(Ihandle* ih, const char* value)
{
  IdataSet* ds;

  if (ih->data->dataSetCurrent==-1)
    return 0;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];

  free(ds->dsMode);
  if (value)
    ds->dsMode = iupStrDup(value);
  else
    ds->dsMode = iupStrDup("LINE");

  ih->data->redraw = true;
  return 0;
}

static char* iMglPlotGetDSModeAttrib(Ihandle* ih)
{
  IdataSet* ds;
  if (ih->data->dataSetCurrent==-1)
    return NULL;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return ds->dsMode;
}

static int iMglPlotSetDSLegendAttrib(Ihandle* ih, const char* value)
{
  IdataSet* ds;

  if (ih->data->dataSetCurrent==-1)
    return 0;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];

  free(ds->dsLegend);

  if (value)
    ds->dsLegend = iupStrDup(value);
  else
    ds->dsLegend = iMglPlotDefaultLegend(ih->data->dataSetCurrent);

  ih->data->redraw = true;
  return 0;
}

static char* iMglPlotGetDSLegendAttrib(Ihandle* ih)
{
  IdataSet* ds;

  if (ih->data->dataSetCurrent==-1)
    return NULL;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return ds->dsLegend;
}

static int iMglPlotSetDSShowValuesAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->dataSetCurrent==-1)
    return 0;

  IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotSetBoolean(ih, value, ds->dsShowValues);
}

static char* iMglPlotGetDSShowValuesAttrib(Ihandle* ih)
{
  if (ih->data->dataSetCurrent==-1)
    return NULL;

  IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotGetBoolean(ds->dsShowValues);
}

static int iMglPlotSetDSShowMarksAttrib(Ihandle* ih, const char* value)
{
  if (ih->data->dataSetCurrent==-1)
    return 0;

  IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotSetBoolean(ih, value, ds->dsShowMarks);
}

static char* iMglPlotGetDSShowMarksAttrib(Ihandle* ih)
{
  if (ih->data->dataSetCurrent==-1)
    return NULL;

  IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  return iMglPlotGetBoolean(ds->dsShowMarks);
}

static int iMglPlotSetDSRemoveAttrib(Ihandle* ih, const char* value)
{
  IdataSet* ds;
  int index, remove_count=1;

  if (ih->data->dataSetCurrent==-1)
    return 0;

  ds = &ih->data->dataSet[ih->data->dataSetCurrent];
  if (iMglPlotIsPlanarOrVolumetricData(ds))
    return 0;

  if (iupStrToIntInt(value, &index, &remove_count, ':'))
  {
    if(index < 0 || index+remove_count > ds->dsCount)
      return 0;

    if (index+remove_count < ds->dsCount)  /* if equal, remove at the end, no need to move data */
    {
      memmove(ds->dsX->a + index, ds->dsX->a + (index + remove_count), sizeof(float)*(ds->dsCount - (index + remove_count)));
      if (ds->dsY)
        memmove(ds->dsY->a + index, ds->dsY->a + (index + remove_count), sizeof(float)*(ds->dsCount - (index + remove_count)));
      if (ds->dsZ)
        memmove(ds->dsZ->a + index, ds->dsZ->a + (index + remove_count), sizeof(float)*(ds->dsCount - (index + remove_count)));
    }
    ds->dsX->Crop(ds->dsCount, 1, 'x');
    if (ds->dsY)
      ds->dsY->Crop(ds->dsCount, 1, 'y');
    if (ds->dsZ)
      ds->dsZ->Crop(ds->dsCount, 1, 'z');

    ds->dsCount -= remove_count;

    if (ds->dsNames)
    {
      char** dsNames = (char**)iupArrayGetData(ds->dsNames);
      free(dsNames[index]);
      iupArrayRemove(ds->dsNames, index, remove_count);
    }

    ih->data->redraw = true;
  }
  return 0;
}

static char* iMglPlotGetDSCountAttrib(Ihandle* ih)
{
  if (ih->data->dataSetCurrent==-1)
    return NULL;
  else
  {
    IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
    return iMglPlotGetInt(ds->dsCount);
  }
}

static char* iMglPlotGetDSDimAttrib(Ihandle* ih)
{
  if (ih->data->dataSetCurrent==-1)
    return NULL;
  else
  {
    IdataSet* ds = &ih->data->dataSet[ih->data->dataSetCurrent];
    if (iMglPlotIsPlanarOrVolumetricData(ds))
    {
      char* buffer = iupStrGetMemory(30);
      sprintf(buffer, "%dx%dx%d", (int)ds->dsX->nx, (int)ds->dsX->ny, (int)ds->dsX->nz);
      return buffer;
    }
    else
      return iMglPlotGetInt(ds->dsDim);
  }
}

// TODO
//static int iMglPlotSetDSEditAttrib(Ihandle* ih, const char* value)
//{
//  IdataSet* ds;
//
//  if (ih->data->dataSetCurrent==-1)
//    return 0;
//
//  ds = &ih->data->dataSet[ih->data->dataSetCurrent];
//  if (iMglPlotIsPlanarOrVolumetricData(ds))
//    return 0;
//
//  if (iupStrBoolean(value))
//    ;
//  else
//    ;
//
//  return 0;
//}
//
//static char* iMglPlotGetDSEditAttrib(Ihandle* ih)
//{
//  (void)ih;
//  return "NO";
//}

static int iMglPlotSetBoxAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->Box);
}

static char* iMglPlotGetBoxAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->Box);
}

static int iMglPlotSetBoxTicksAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->boxTicks);
}

static char* iMglPlotGetBoxTicksAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->boxTicks);
}

static int iMglPlotSetAxisLabelPosition(Ihandle* ih, const char* value, int& pos)
{
  int old_pos = pos;

  if (iupStrEqualNoCase(value, "CENTER"))
    pos = 0;
  else if (iupStrEqualNoCase(value, "MIN"))
    pos = -1;
  else if (iupStrEqualNoCase(value, "MAX"))
    pos = 1;

  if (old_pos != pos)
    ih->data->redraw = true;

  return 0;
}

static char* iMglPlotGetAxisLabelPosition(int pos)
{
  if (pos == 0)
    return "CENTER";
  if (pos == 1)
    return "MAX";
  if (pos == -1)
    return "MIN";
  return NULL;
}

static int iMglPlotSetAxisXLabelPositionAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisLabelPosition(ih, value, ih->data->axisX.axLabelPos);
}

static int iMglPlotSetAxisYLabelPositionAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisLabelPosition(ih, value, ih->data->axisY.axLabelPos);
}

static int iMglPlotSetAxisZLabelPositionAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisLabelPosition(ih, value, ih->data->axisZ.axLabelPos);
}

static char* iMglPlotGetAxisXLabelPositionAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisLabelPosition(ih->data->axisX.axLabelPos);
}

static char* iMglPlotGetAxisYLabelPositionAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisLabelPosition(ih->data->axisY.axLabelPos);
}

static char* iMglPlotGetAxisZLabelPositionAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisLabelPosition(ih->data->axisZ.axLabelPos);
}

static int iMglPlotSetAxisXLabelRotationAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axLabelRotation);
}

static int iMglPlotSetAxisYLabelRotationAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axLabelRotation);
}

static int iMglPlotSetAxisZLabelRotationAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axLabelRotation);
}

static char* iMglPlotGetAxisXLabelRotationAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axLabelRotation);
}

static char* iMglPlotGetAxisYLabelRotationAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axLabelRotation);
}

static char* iMglPlotGetAxisZLabelRotationAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axLabelRotation);
}

static int iMglPlotSetAxisXLabelCenteredAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    return iMglPlotSetAxisLabelPosition(ih, "CENTER", ih->data->axisX.axLabelPos);
  else 
    return iMglPlotSetAxisLabelPosition(ih, "MAX", ih->data->axisX.axLabelPos);
}

static int iMglPlotSetAxisYLabelCenteredAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    return iMglPlotSetAxisLabelPosition(ih, "CENTER", ih->data->axisY.axLabelPos);
  else 
    return iMglPlotSetAxisLabelPosition(ih, "MAX", ih->data->axisY.axLabelPos);
}

static int iMglPlotSetAxisZLabelCenteredAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    return iMglPlotSetAxisLabelPosition(ih, "CENTER", ih->data->axisZ.axLabelPos);
  else 
    return iMglPlotSetAxisLabelPosition(ih, "MAX", ih->data->axisZ.axLabelPos);
}

static char* iMglPlotGetAxisXLabelCenteredAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axLabelPos==0? true: false);
}

static char* iMglPlotGetAxisYLabelCenteredAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axLabelPos==0? true: false);
}

static char* iMglPlotGetAxisZLabelCenteredAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axLabelPos==0? true: false);
}

static int iMglPlotSetAxisXColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->axisX.axColor);
}

static int iMglPlotSetAxisYColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->axisY.axColor);
}

static int iMglPlotSetAxisZColorAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetColor(ih, value, ih->data->axisZ.axColor);
}

static char* iMglPlotGetAxisXColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->axisX.axColor);
}

static char* iMglPlotGetAxisYColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->axisY.axColor);
}

static char* iMglPlotGetAxisZColorAttrib(Ihandle* ih)
{
  return iMglPlotGetColorAttrib(ih->data->axisZ.axColor);
}

static int iMglPlotSetAxisXFontSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisX.axLabelFontSizeFactor);
}

static int iMglPlotSetAxisYFontSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisY.axLabelFontSizeFactor);
}

static int iMglPlotSetAxisZFontSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisZ.axLabelFontSizeFactor);
}

static char* iMglPlotGetAxisXFontSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisX.axLabelFontSizeFactor);
}

static char* iMglPlotGetAxisYFontSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisY.axLabelFontSizeFactor);
}

static char* iMglPlotGetAxisZFontSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisZ.axLabelFontSizeFactor);
}

static int iMglPlotSetAxisXFontStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFontStyle(ih, value, ih->data->axisX.axLabelFontStyle);
}

static int iMglPlotSetAxisYFontStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFontStyle(ih, value, ih->data->axisY.axLabelFontStyle);
}

static int iMglPlotSetAxisZFontStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFontStyle(ih, value, ih->data->axisZ.axLabelFontStyle);
}

static char* iMglPlotGetAxisXFontStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetFontStyle(ih->data->axisX.axLabelFontStyle);
}

static char* iMglPlotGetAxisYFontStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetFontStyle(ih->data->axisY.axLabelFontStyle);
}

static char* iMglPlotGetAxisZFontStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetFontStyle(ih->data->axisZ.axLabelFontStyle);
}

static int iMglPlotSetAxisXTickFontSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisX.axTickFontSizeFactor);
}

static int iMglPlotSetAxisYTickFontSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisY.axTickFontSizeFactor);
}

static int iMglPlotSetAxisZTickFontSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisZ.axTickFontSizeFactor);
}

static char* iMglPlotGetAxisXTickFontSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisX.axTickFontSizeFactor);
}

static char* iMglPlotGetAxisYTickFontSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisY.axTickFontSizeFactor);
}

static char* iMglPlotGetAxisZTickFontSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisZ.axTickFontSizeFactor);
}

static int iMglPlotSetAxisXTickFontStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFontStyle(ih, value, ih->data->axisX.axTickFontStyle);
}

static int iMglPlotSetAxisYTickFontStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFontStyle(ih, value, ih->data->axisY.axTickFontStyle);
}

static int iMglPlotSetAxisZTickFontStyleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFontStyle(ih, value, ih->data->axisZ.axTickFontStyle);
}

static char* iMglPlotGetAxisXTickFontStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetFontStyle(ih->data->axisX.axTickFontStyle);
}

static char* iMglPlotGetAxisYTickFontStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetFontStyle(ih->data->axisY.axTickFontStyle);
}

static char* iMglPlotGetAxisZTickFontStyleAttrib(Ihandle* ih)
{
  return iMglPlotGetFontStyle(ih->data->axisZ.axTickFontStyle);
}

static int iMglPlotSetAxisXTickMinorSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisX.axTickMinorSizeFactor);
}

static int iMglPlotSetAxisYTickMinorSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisY.axTickMinorSizeFactor);
}

static int iMglPlotSetAxisZTickMinorSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisZ.axTickMinorSizeFactor);
}

static char* iMglPlotGetAxisXTickMinorSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisX.axTickMinorSizeFactor);
}

static char* iMglPlotGetAxisYTickMinorSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisY.axTickMinorSizeFactor);
}

static char* iMglPlotGetAxisZTickMinorSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisZ.axTickMinorSizeFactor);
}

static int iMglPlotSetAxisXTickMajorSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisX.axTickMajorSize);
}

static int iMglPlotSetAxisYTickMajorSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisY.axTickMajorSize);
}

static int iMglPlotSetAxisZTickMajorSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisZ.axTickMajorSize);
}

static char* iMglPlotGetAxisXTickMajorSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisX.axTickMajorSize);
}

static char* iMglPlotGetAxisYTickMajorSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisY.axTickMajorSize);
}

static char* iMglPlotGetAxisZTickMajorSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisZ.axTickMajorSize);
}

static int iMglPlotSetAxisXTickMajorSpanAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisX.axTickMajorSpan);
}

static int iMglPlotSetAxisYTickMajorSpanAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisY.axTickMajorSpan);
}

static int iMglPlotSetAxisZTickMajorSpanAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisZ.axTickMajorSpan);
}

static char* iMglPlotGetAxisXTickMajorSpanAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisX.axTickMajorSpan);
}

static char* iMglPlotGetAxisYTickMajorSpanAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisY.axTickMajorSpan);
}

static char* iMglPlotGetAxisZTickMajorSpanAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisZ.axTickMajorSpan);
}

static int iMglPlotSetAxisScale(Ihandle* ih, const char* value, const char** scale, char dir)
{
  if(iupStrEqualNoCase(value, "LIN"))
    *scale = NULL;
  else if(iupStrEqualNoCase(value, "LOG10"))
    *scale = (dir=='x')? "lg(x)": (dir=='y'? "lg(y)": "lg(z)");
/*  else if(iupStrEqualNoCase(value, "LOG2"))   // NOT supported yet
    *scale = (dir=='x')? "log(x, 2)": (dir=='y'? "log(y, 2)": "log(z, 2)");
  else if(iupStrEqualNoCase(value, "LOGN"))
    *scale = (dir=='x')? "ln(x)": (dir=='y'? "ln(y)": "ln(z)"); */

  ih->data->redraw = true;
  return 0;
}

static int iMglPlotSetAxisXScaleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisScale(ih, value, &(ih->data->axisX.axScale), 'x');
}

static int iMglPlotSetAxisYScaleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisScale(ih, value, &(ih->data->axisY.axScale), 'y');
}

static int iMglPlotSetAxisZScaleAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisScale(ih, value, &(ih->data->axisZ.axScale), 'z');
}

static char* iMglPlotGetAxisScale(const char* scale)
{
  if (!scale)
    return "LIN";
  else if (strstr(scale, "lg"))
    return "LOG10";
/*  else if (strstr(scale, "log"))  // NOT supported yet
    return "LOG2";
  else if (strstr(scale, "ln"))
    return "LOGN"; */
  return NULL;
}

static char* iMglPlotGetAxisXScaleAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisScale(ih->data->axisX.axScale);
}

static char* iMglPlotGetAxisYScaleAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisScale(ih->data->axisY.axScale);
}

static char* iMglPlotGetAxisZScaleAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisScale(ih->data->axisZ.axScale);
}

static int iMglPlotSetAxisXReverseAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axReverse);
}

static int iMglPlotSetAxisYReverseAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axReverse);
}

static int iMglPlotSetAxisZReverseAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axReverse);
}

static char* iMglPlotGetAxisXReverseAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axReverse);
}

static char* iMglPlotGetAxisYReverseAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axReverse);
}

static char* iMglPlotGetAxisZReverseAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axReverse);
}

static int iMglPlotSetAxisXShowAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axShow);
}

static int iMglPlotSetAxisYShowAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axShow);
}

static int iMglPlotSetAxisZShowAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axShow);
}

static char* iMglPlotGetAxisXShowAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axShow);
}

static char* iMglPlotGetAxisYShowAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axShow);
}

static char* iMglPlotGetAxisZShowAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axShow);
}

static int iMglPlotSetAxisXShowArrowAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axShowArrow);
}

static int iMglPlotSetAxisYShowArrowAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axShowArrow);
}

static int iMglPlotSetAxisZShowArrowAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axShowArrow);
}

static char* iMglPlotGetAxisXShowArrowAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axShowArrow);
}

static char* iMglPlotGetAxisYShowArrowAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axShowArrow);
}

static char* iMglPlotGetAxisZShowArrowAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axShowArrow);
}

static int iMglPlotSetAxisXTickShowValuesAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axTickShowValues);
}

static int iMglPlotSetAxisYTickShowValuesAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axTickShowValues);
}

static int iMglPlotSetAxisZTickShowValuesAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axTickShowValues);
}

static char* iMglPlotGetAxisXTickShowValuesAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axTickShowValues);
}

static char* iMglPlotGetAxisYTickShowValuesAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axTickShowValues);
}

static char* iMglPlotGetAxisZTickShowValuesAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axTickShowValues);
}

static int iMglPlotSetAxisXTickAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axTickShow);
}

static int iMglPlotSetAxisYTickAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axTickShow);
}

static int iMglPlotSetAxisZTickAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axTickShow);
}

static char* iMglPlotGetAxisXTickAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axTickShow);
}

static char* iMglPlotGetAxisYTickAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axTickShow);
}

static char* iMglPlotGetAxisZTickAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axTickShow);
}

static int iMglPlotSetAxisXTickAutoSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axTickAutoSize);
}

static int iMglPlotSetAxisYTickAutoSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axTickAutoSize);
}

static int iMglPlotSetAxisZTickAutoSizeAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axTickAutoSize);
}

static char* iMglPlotGetAxisXTickAutoSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axTickAutoSize);
}

static char* iMglPlotGetAxisYTickAutoSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axTickAutoSize);
}

static char* iMglPlotGetAxisZTickAutoSizeAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axTickAutoSize);
}

static int iMglPlotSetAxisXTickAutoAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axTickAutoSpace);
}

static int iMglPlotSetAxisYTickAutoAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axTickAutoSpace);
}

static int iMglPlotSetAxisZTickAutoAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axTickAutoSpace);
}

static char* iMglPlotGetAxisXTickAutoAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axTickAutoSpace);
}

static char* iMglPlotGetAxisYTickAutoAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axTickAutoSpace);
}

static char* iMglPlotGetAxisZTickAutoAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axTickAutoSpace);
}

//////////////////////////
static int iMglPlotSetAxisXTickValuesRotationAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axTickValuesRotation);
}

static int iMglPlotSetAxisYTickValuesRotationAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axTickValuesRotation);
}

static int iMglPlotSetAxisZTickValuesRotationAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axTickValuesRotation);
}

static char* iMglPlotGetAxisXTickValuesRotationAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axTickValuesRotation);
}

static char* iMglPlotGetAxisYTickValuesRotationAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axTickValuesRotation);
}

static char* iMglPlotGetAxisZTickValuesRotationAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axTickValuesRotation);
}


static int iMglPlotSetAxisXTickMinorDivisionAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetInt(ih, value, ih->data->axisX.axTickMinorDivision);
}

static int iMglPlotSetAxisYTickMinorDivisionAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetInt(ih, value, ih->data->axisY.axTickMinorDivision);
}

static int iMglPlotSetAxisZTickMinorDivisionAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetInt(ih, value, ih->data->axisZ.axTickMinorDivision);
}

static char* iMglPlotGetAxisXTickMinorDivisionAttrib(Ihandle* ih)
{
  return iMglPlotGetInt(ih->data->axisX.axTickMinorDivision);
}

static char* iMglPlotGetAxisYTickMinorDivisionAttrib(Ihandle* ih)
{
  return iMglPlotGetInt(ih->data->axisY.axTickMinorDivision);
}

static char* iMglPlotGetAxisZTickMinorDivisionAttrib(Ihandle* ih)
{
  return iMglPlotGetInt(ih->data->axisZ.axTickMinorDivision);
}


static int iMglPlotSetAxisXAutoMinAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axAutoScaleMin);
}

static int iMglPlotSetAxisYAutoMinAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axAutoScaleMin);
}

static int iMglPlotSetAxisZAutoMinAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axAutoScaleMin);
}

static char* iMglPlotGetAxisXAutoMinAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axAutoScaleMin);
}

static char* iMglPlotGetAxisYAutoMinAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axAutoScaleMin);
}

static char* iMglPlotGetAxisZAutoMinAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axAutoScaleMin);
}

static int iMglPlotSetAxisXAutoMaxAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisX.axAutoScaleMax);
}

static int iMglPlotSetAxisYAutoMaxAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisY.axAutoScaleMax);
}

static int iMglPlotSetAxisZAutoMaxAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->axisZ.axAutoScaleMax);
}

static char* iMglPlotGetAxisXAutoMaxAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisX.axAutoScaleMax);
}

static char* iMglPlotGetAxisYAutoMaxAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisY.axAutoScaleMax);
}

static char* iMglPlotGetAxisZAutoMaxAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->axisZ.axAutoScaleMax);
}

static int iMglPlotSetAxisXMinAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisX.axMin);
}

static int iMglPlotSetAxisYMinAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisY.axMin);
}

static int iMglPlotSetAxisZMinAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisZ.axMin);
}

static char* iMglPlotGetAxisXMinAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisX.axMin);
}

static char* iMglPlotGetAxisYMinAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisY.axMin);
}

static char* iMglPlotGetAxisZMinAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisZ.axMin);
}

static int iMglPlotSetAxisXMaxAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisX.axMax);
}

static int iMglPlotSetAxisYMaxAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisY.axMax);
}

static int iMglPlotSetAxisZMaxAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->axisZ.axMax);
}

static char* iMglPlotGetAxisXMaxAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisX.axMax);
}

static char* iMglPlotGetAxisYMaxAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisY.axMax);
}

static char* iMglPlotGetAxisZMaxAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->axisZ.axMax);
}

static int iMglPlotSetAxisCrossOrigin(Ihandle* ih, const char* value, float& origin)
{
  float old_origin = origin;

  if (iupStrBoolean(value))
    origin = 0;
  else
    origin = NAN;

  if (old_origin != origin)
    ih->data->redraw = true;

  return 0;
}

static int iMglPlotSetAxisXCrossOriginAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisCrossOrigin(ih, value, ih->data->axisX.axOrigin);
}

static int iMglPlotSetAxisYCrossOriginAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisCrossOrigin(ih, value, ih->data->axisY.axOrigin);
}

static int iMglPlotSetAxisZCrossOriginAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisCrossOrigin(ih, value, ih->data->axisZ.axOrigin);
}

static char* iMglPlotGetAxisXCrossOrigin(float origin)
{
  if (origin==0)
    return "YES";
  else if (isnan(origin))
    return "NO";
  else
    return NULL;
}

static char* iMglPlotGetAxisXCrossOriginAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisXCrossOrigin(ih->data->axisX.axOrigin);
}

static char* iMglPlotGetAxisYCrossOriginAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisXCrossOrigin(ih->data->axisY.axOrigin);
}

static char* iMglPlotGetAxisZCrossOriginAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisXCrossOrigin(ih->data->axisZ.axOrigin);
}

static int iMglPlotSetAxisOrigin(Ihandle* ih, const char* value, float& num)
{
  float old_num = num;

  if (!value)
    num = NAN;
  else
    iupStrToFloat(value, &num);

  if (old_num != num)
    ih->data->redraw = true;

  return 0;
}

static int iMglPlotSetAxisXOriginAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisOrigin(ih, value, ih->data->axisX.axOrigin);
}

static int iMglPlotSetAxisYOriginAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisOrigin(ih, value, ih->data->axisY.axOrigin);
}

static int iMglPlotSetAxisZOriginAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetAxisOrigin(ih, value, ih->data->axisZ.axOrigin);
}

static char* iMglPlotGetAxisOrigin(float Origin)
{
  if (!isnan(Origin))
    return iMglPlotGetFloat(Origin);
  else
    return NULL;
}

static char* iMglPlotGetAxisXOriginAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisOrigin(ih->data->axisX.axOrigin);
}

static char* iMglPlotGetAxisYOriginAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisOrigin(ih->data->axisY.axOrigin);
}

static char* iMglPlotGetAxisZOriginAttrib(Ihandle* ih)
{
  return iMglPlotGetAxisOrigin(ih->data->axisZ.axOrigin);
}

static int iMglPlotSetAlphaAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetFloat(ih, value, ih->data->alpha);
}

static char* iMglPlotGetAlphaAttrib(Ihandle* ih)
{
  return iMglPlotGetFloat(ih->data->alpha);
}

static int iMglPlotSetTransparentAttrib(Ihandle* ih, const char* value)
{
  return iMglPlotSetBoolean(ih, value, ih->data->transparent);
}

static char* iMglPlotGetTransparentAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->transparent);
}

static void iMglPlotInitOpenGL2D(void)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1.0f, 0, 0);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  /* data alignment is 1 */
  glPixelZoom(1.0f, -1.0f);  // vertical flip image
  glRasterPos2f(0, 1.0f);
}

static int iMglPlotSetOpenGLAttrib(Ihandle* ih, const char* value)
{
  bool old_opengl = ih->data->opengl;
  iMglPlotSetBoolean(ih, value, ih->data->opengl);

  if (old_opengl != ih->data->opengl)
  {
    delete ih->data->mgl;
    ih->data->redraw = 1;

    if (ih->data->opengl)
      ih->data->mgl = new mglGraphGL();
    else
    {
      if (ih->handle)
      {
        IupGLMakeCurrent(ih);
        iMglPlotInitOpenGL2D();
      }
      ih->data->mgl = new mglGraphZB(ih->data->w, ih->data->h);
    }
  }

  return 0;
}

static char* iMglPlotGetOpenGLAttrib(Ihandle* ih)
{
  return iMglPlotGetBoolean(ih->data->opengl);
}

static int iMglPlotSetAntialiasAttrib(Ihandle* ih, const char* value)
{
  if (!ih->data->opengl)
    return 0;

  IupGLMakeCurrent(ih);

  if (iupStrBoolean(value))
  {
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  }
  else
  {
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
  }

  return 0;
}

static char* iMglPlotGetAntialiasAttrib(Ihandle* ih)
{
  if (!ih->data->opengl)
    return "NO";

  IupGLMakeCurrent(ih);
  return iMglPlotGetBoolean(glIsEnabled(GL_LINE_SMOOTH)==GL_TRUE);
}

static char* iMglPlotGetErrorMessageAttrib(Ihandle* ih)
{
  if (ih->data->ErrorMessage[0])
    return ih->data->ErrorMessage;
  else
    return NULL;
}

static int iMglPlotSetZoomAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->data->x1 = 0;
    ih->data->y1 = 0;
    ih->data->x2 = 1.0f;
    ih->data->y2 = 1.0f;
  }
  else
  {
    char value1[50]="", value2[50]="";

    iupStrToStrStr(value, value1, value2, ':');

    iupStrToFloatFloat(value1, &ih->data->x1, &ih->data->y1, ',');
    iupStrToFloatFloat(value2, &ih->data->x2, &ih->data->y2, ',');

    if (ih->data->x1 < 0) ih->data->x1 = 0;
    if (ih->data->y1 < 0) ih->data->y1 = 0;
    if (ih->data->x2 > 1.0f) ih->data->x2 = 1.0f;
    if (ih->data->y2 > 1.0f) ih->data->y2 = 1.0f;
  }

  ih->data->redraw = true;

  return 0;
}

static char* iMglPlotGetZoomAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(50);
  sprintf(str, "%g,%g:%g,%g", ih->data->x1, ih->data->y1, ih->data->x2, ih->data->y2);
  return str;
}

static int iMglPlotSetRotateAttrib(Ihandle* ih, const char* value)
{
  if (!value)
  {
    ih->data->rotX = 0;
    ih->data->rotY = 0;
    ih->data->rotZ = 0;
  }
  else
  {
    char value1[50]="", value2[100]="";
    iupStrToStrStr(value, value1, value2, ':');
    iupStrToFloat(value1, &ih->data->rotX);
    iupStrToFloatFloat(value2, &ih->data->rotY, &ih->data->rotZ, ':');
  }

  ih->data->redraw = true;

  return 0;
}

static char* iMglPlotGetRotateAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(50);
  sprintf(str, "%g:%g:%g", ih->data->rotX, ih->data->rotY, ih->data->rotZ);
  return str;
}


/******************************************************************************
Additional Functions
******************************************************************************/


int IupMglPlotNewDataSet(Ihandle *ih, int dim)
{
  int ds_index;
  IdataSet* ds;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return -1;

  /* Increment the number of datasets */
  ih->data->dataSetCount++;

  // Allocate memory if necessary
  if(ih->data->dataSetCount == ih->data->dataSetMaxCount)
  {
    ih->data->dataSetMaxCount += ih->data->dataSetMaxCount;
    ih->data->dataSet = (IdataSet*)realloc(ih->data->dataSet, ih->data->dataSetMaxCount*sizeof(IdataSet));
    memset(ih->data->dataSet+ih->data->dataSetCount-1, 0, sizeof(IdataSet)*(ih->data->dataSetMaxCount - (ih->data->dataSetCount-1)));
  }

  ds_index = ih->data->dataSetCount-1;
  ds = &ih->data->dataSet[ds_index];

  ds->dsDim = dim; 
  ds->dsX = new mglData();
  if (dim > 1)
    ds->dsY = new mglData();
  if (dim > 2)
    ds->dsZ = new mglData();
  ds->dsCount = 0;

  /* Initialize the default values */
  iMglPlotResetDataSet(ds, ds_index);

  ih->data->redraw = true;

  ih->data->dataSetCurrent = ds_index;
  return ih->data->dataSetCurrent;
}

void IupMglPlotBegin(Ihandle* ih, int dim)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  Iarray* inXData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_XDATA");
  Iarray* inYData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_YDATA");
  Iarray* inZData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_ZDATA");
  Iarray* inNames = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_NAMES");

  if (inXData) { iupArrayDestroy(inXData); inXData = NULL;}
  if (inYData) { iupArrayDestroy(inYData); inYData = NULL;}
  if (inZData) { iupArrayDestroy(inZData); inZData = NULL;}
  if (inNames) iupArrayDestroy(inNames);

  inXData =  iupArrayCreate(10, sizeof(float));
  if (dim>1)
    inYData =  iupArrayCreate(10, sizeof(float));
  if (dim>2)
    inZData =  iupArrayCreate(10, sizeof(float));

  iupAttribSetStr(ih, "_IUP_MGLPLOT_XDATA", (char*)inXData);
  iupAttribSetStr(ih, "_IUP_MGLPLOT_YDATA", (char*)inYData);
  iupAttribSetStr(ih, "_IUP_MGLPLOT_ZDATA", (char*)inZData);
  iupAttribSetStr(ih, "_IUP_MGLPLOT_NAMES", NULL);
}

int IupMglPlotEnd(Ihandle* ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "mglplot"))
    return -1;

  Iarray* inXData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_XDATA");
  Iarray* inYData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_YDATA");
  Iarray* inZData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_ZDATA");
  Iarray* inNames = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_NAMES");
  int dim = 0;
  if (inXData) dim = 1;
  if (inYData) dim = 2;
  if (inZData) dim = 3;
  if (dim == 0)
    return -1;

  // Actually add the dataset only at the End
  int ds_index = IupMglPlotNewDataSet(ih, dim);
  if (dim==1)
  {
    char** names = NULL;
    if (inNames)
      names = (char**)iupArrayGetData(inNames);
    float* x = (float*)iupArrayGetData(inXData);
    int count = iupArrayCount(inXData);
    IupMglPlotSet1D(ih, ds_index, (const char**)names, x, count);
  }
  else if (dim==2)
  {
    float* x = (float*)iupArrayGetData(inXData);
    float* y = (float*)iupArrayGetData(inYData);
    int count = iupArrayCount(inXData);
    IupMglPlotSet2D(ih, ds_index, x, y, count);
  }
  else if (dim==3)
  {
    float* x = (float*)iupArrayGetData(inXData);
    float* y = (float*)iupArrayGetData(inYData);
    float* z = (float*)iupArrayGetData(inZData);
    int count = iupArrayCount(inXData);
    IupMglPlotSet3D(ih, ds_index, x, y, z, count);
  }

  if (inXData) iupArrayDestroy(inXData);
  if (inYData) iupArrayDestroy(inYData);
  if (inZData) iupArrayDestroy(inZData);
  if (inNames) iupArrayDestroy(inNames);
  iupAttribSetStr(ih, "_IUP_MGLPLOT_XDATA", NULL);
  iupAttribSetStr(ih, "_IUP_MGLPLOT_YDATA", NULL);
  iupAttribSetStr(ih, "_IUP_MGLPLOT_ZDATA", NULL);
  iupAttribSetStr(ih, "_IUP_MGLPLOT_NAMES", NULL);

  return ds_index;
}

void IupMglPlotAdd1D(Ihandle* ih, const char* inName, float inX)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  Iarray* inXData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_XDATA");
  Iarray* inYData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_YDATA");
  Iarray* inZData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_ZDATA");
  int dim = 0;
  if (inXData) dim = 1;
  if (inYData) dim = 2;
  if (inZData) dim = 3;
  if (dim < 1)
    return;

  int sample_index = iupArrayCount(inXData);  /* get before incrementing the array */

  if (inName)
  {
    Iarray* inNames = NULL;
    if (sample_index == 0)
    {
      if (inNames) iupArrayDestroy(inNames);
      inNames =  iupArrayCreate(10, sizeof(char*));
      iupAttribSetStr(ih, "_IUP_MGLPLOT_NAMES", (char*)inNames);
    }
    else
      inNames = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_NAMES");

    if (inNames)
    {
      char** names = (char**)iupArrayInc(inNames);
      names[sample_index] = iupStrDup(inName);
    }
  }

  float* x = (float*)iupArrayInc(inXData);
  x[sample_index] = inX;
}

void IupMglPlotAdd2D(Ihandle* ih, float inX, float inY)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  Iarray* inXData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_XDATA");
  Iarray* inYData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_YDATA");
  Iarray* inZData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_ZDATA");
  int dim = 0;
  if (inXData) dim = 1;
  if (inYData) dim = 2;
  if (inZData) dim = 3;
  if (dim < 2)
    return;

  int sample_index = iupArrayCount(inXData);  /* get before incrementing the array */

  float* x = (float*)iupArrayInc(inXData);
  float* y = (float*)iupArrayInc(inYData);
  x[sample_index] = inX;
  y[sample_index] = inY;
}

void IupMglPlotAdd3D(Ihandle *ih, float inX, float inY, float inZ)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  Iarray* inXData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_XDATA");
  Iarray* inYData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_YDATA");
  Iarray* inZData = (Iarray*)iupAttribGet(ih, "_IUP_MGLPLOT_ZDATA");
  int dim = 0;
  if (inXData) dim = 1;
  if (inYData) dim = 2;
  if (inZData) dim = 3;
  if (dim < 3)
    return;

  int sample_index = iupArrayCount(inXData);  /* get before incrementing the array */

  float* x = (float*)iupArrayInc(inXData);
  float* y = (float*)iupArrayInc(inYData);
  float* z = (float*)iupArrayInc(inZData);
  x[sample_index] = inX;
  y[sample_index] = inY;
  z[sample_index] = inZ;
}

void IupMglPlotInsert1D(Ihandle* ih, int inIndex, int inSampleIndex, const char** inNames, const float* inX, int inCount)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 || inCount<=0)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];

  if(inSampleIndex > ds->dsCount || inSampleIndex < 0)
    return;

  ds->dsCount += inCount;
  ds->dsX->Extend(ds->dsCount);
  if (inSampleIndex < ds->dsCount-1)  // insert in the middle, open space first
    memmove(ds->dsX->a + inSampleIndex + inCount, ds->dsX->a + inSampleIndex, inCount*sizeof(float));
  memcpy(ds->dsX->a + inSampleIndex, inX, inCount*sizeof(float));

  if (inNames && ds->dsNames && inIndex==0)  // Allow names only for the first dataset
  {
    char** dsNames = (char**)iupArrayInsert(ds->dsNames, inSampleIndex, inCount);
    for (int i = 0; i < inCount; i++)
    {
      dsNames[inSampleIndex] = iupStrDup(inNames[i]!=NULL? inNames[i]: "");  
      inSampleIndex++;
    }
  }
}

void IupMglPlotInsert2D(Ihandle* ih, int inIndex, int inSampleIndex, const float *inX, const float *inY, int inCount)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 || inCount<=0)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];
  if (!ds->dsY)
    return;

  if(inSampleIndex > ds->dsCount || inSampleIndex < 0)
    return;

  ds->dsCount += inCount;
  ds->dsX->Extend(ds->dsCount);
  ds->dsY->Extend(ds->dsCount);
  if (inSampleIndex < ds->dsCount-1)  // insert in the middle, open space first
  {
    memmove(ds->dsX->a + inSampleIndex + inCount, ds->dsX->a + inSampleIndex, inCount*sizeof(float));
    memmove(ds->dsY->a + inSampleIndex + inCount, ds->dsY->a + inSampleIndex, inCount*sizeof(float));
  }
  memcpy(ds->dsX->a + inSampleIndex, inX, inCount*sizeof(float));
  memcpy(ds->dsY->a + inSampleIndex, inY, inCount*sizeof(float));
}

void IupMglPlotInsert3D(Ihandle* ih, int inIndex, int inSampleIndex, const float* inX, const float* inY, const float* inZ, int inCount)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 || inCount<=0)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];
  if (!ds->dsY || !ds->dsZ)
    return;

  if(inSampleIndex > ds->dsCount || inSampleIndex < 0)
    return;

  ds->dsCount += inCount;
  ds->dsX->Extend(ds->dsCount);
  ds->dsY->Extend(ds->dsCount);
  ds->dsZ->Extend(ds->dsCount);
  if (inSampleIndex < ds->dsCount-1)  // insert in the middle, open space first
  {
    memmove(ds->dsX->a + inSampleIndex + inCount, ds->dsX->a + inSampleIndex, inCount*sizeof(float));
    memmove(ds->dsY->a + inSampleIndex + inCount, ds->dsY->a + inSampleIndex, inCount*sizeof(float));
    memmove(ds->dsZ->a + inSampleIndex + inCount, ds->dsZ->a + inSampleIndex, inCount*sizeof(float));
  }
  memcpy(ds->dsX->a + inSampleIndex, inX, inCount*sizeof(float));
  memcpy(ds->dsY->a + inSampleIndex, inY, inCount*sizeof(float));
  memcpy(ds->dsZ->a + inSampleIndex, inZ, inCount*sizeof(float));
}

void IupMglPlotSet1D(Ihandle* ih, int inIndex, const char** inNames, const float* inX, int inCount)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 || inCount<=0)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];

  if (inNames && inIndex==0)   // Allow names only for the first dataset
  {
    char** dsNames;
    int j;

    if (!ds->dsNames)
    {
      ds->dsNames = iupArrayCreate(10, sizeof(char*));
      dsNames = (char**)iupArrayAdd(ds->dsNames, inCount);
    }
    else
    {
      dsNames = (char**)iupArrayGetData(ds->dsNames);
      int count = iupArrayCount(ds->dsNames);
      for (j=0; j<count; j++)
        free(dsNames[j]);

      if (count < inCount)
        dsNames = (char**)iupArrayAdd(ds->dsNames, inCount - count);
      else if (count > inCount)
        iupArrayRemove(ds->dsNames, inCount, count - inCount);
    }

    for (j = 0; j < inCount; j++)
    {
      dsNames[j] = iupStrDup(inNames[j]!=NULL? inNames[j]: "");  
    }
  }

  ds->dsX->Set(inX, inCount);
  ds->dsCount = inCount;

  ih->data->redraw = true;
}

void IupMglPlotSet2D(Ihandle* ih, int inIndex, const float *inX, const float *inY, int inCount)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 || inCount<=0)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];
  if (!ds->dsY)
    return;

  ds->dsX->Set(inX, inCount);
  ds->dsY->Set(inY, inCount);
  ds->dsCount = inCount;

  ih->data->redraw = true;
}

void IupMglPlotSet3D(Ihandle* ih, int inIndex, const float* inX, const float* inY, const float* inZ, int inCount)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 || inCount<=0)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];
  if (!ds->dsY || !ds->dsZ)
    return;

  ds->dsX->Set(inX, inCount);
  ds->dsY->Set(inY, inCount);
  ds->dsZ->Set(inZ, inCount);
  ds->dsCount = inCount;

  ih->data->redraw = true;
}

void IupMglPlotSetData(Ihandle* ih, int inIndex, const float* data, int count_x, int count_y, int count_z)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 ||
     count_x<=0 || count_y<=0 || count_z<=0)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];
  ds->dsX->Set(data, count_x, count_y, count_z);
  ds->dsCount = count_x*count_y*count_z;

  ih->data->redraw = true;
}

void IupMglPlotLoadData(Ihandle* ih, int inIndex, const char* filename, int count_x, int count_y, int count_z)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 ||
     !filename)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];
  if (count_x==0 || count_y==0 || count_z==0)
    ds->dsX->Read(filename);
  else
    ds->dsX->Read(filename, count_x, count_y, count_z);
  ds->dsCount = ds->dsX->nx*ds->dsX->ny*ds->dsX->nz;

  ih->data->redraw = true;
}

void IupMglPlotSetFormula(Ihandle* ih, int inIndex, const char* formulaX, const char* formulaY, const char* formulaZ, int count)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 || !formulaX)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];

  if (ds->dsDim >= 2 && !formulaY)
    return;
  if (ds->dsDim == 3 && !formulaZ)
    return;

  count = count>0? count: ds->dsCount;
  if (count != ds->dsCount)
  {
    ds->dsCount = count;
    ds->dsX->Create(count);
    if (ds->dsY) ds->dsY->Create(count);
    if (ds->dsZ) ds->dsZ->Create(count);
  }

  ds->dsX->Modify(formulaX);
  if (ds->dsY) ds->dsY->Modify(formulaY);
  if (ds->dsZ) ds->dsZ->Modify(formulaZ);

  ih->data->redraw = true;
}

void IupMglPlotSetFromFormula(Ihandle* ih, int inIndex, const char* formula, int count_x, int count_y, int count_z)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if(inIndex > (ih->data->dataSetCount-1) || inIndex < 0 ||
     !formula)
    return;

  IdataSet* ds = &ih->data->dataSet[inIndex];

  int nx = count_x>0? count_x: ds->dsX->nx;
  int ny = count_y>0? count_y: ds->dsX->ny;
  int nz = count_z>0? count_z: ds->dsX->nz;
  if (nx != ds->dsX->nx || ny != ds->dsX->ny || nz != ds->dsX->nz)
    ds->dsX->Create(nx, ny, nz);

  ds->dsX->Modify(formula);
  ds->dsCount = ds->dsX->nx*ds->dsX->ny*ds->dsX->nz;

  ih->data->redraw = true;
}

void IupMglPlotTransform(Ihandle* ih, float x, float y, float z, int *ix, int *iy)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "mglplot"))
    return;

  ih->data->mgl->CalcScr(mglPoint(x, y, z), ix, iy);
}

void IupMglPlotTransformXYZ(Ihandle* ih, int ix, int iy, float *x, float *y, float *z)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
      !IupClassMatch(ih, "mglplot"))
    return;

  mglPoint p = ih->data->mgl->CalcXYZ(ix, iy);
  if (x) *x = p.x;
  if (y) *y = p.y;
  if (z) *z = p.z;
}

void IupMglPlotPaintTo(Ihandle* ih, const char* format, int w, int h, float dpi, void *data)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  if (!format || !data)
    return;

  int old_w = ih->data->w;
  int old_h = ih->data->h;
  float old_dpi = ih->data->dpi;

  if (dpi == 0) dpi = old_dpi;
  if (w == 0) w = old_w;
  if (h == 0) h = old_h;

  ih->data->w = w;
  ih->data->h = h;
  ih->data->dpi = dpi;

  if (*format == 'E') //EPS
  {
    char* filename = (char*)data;

    mglGraphPS* gr = new mglGraphPS(w, h);
    iMglPlotDrawPlot(ih, gr);
    gr->WriteEPS(filename, "IupMglPlot");

    delete gr;
  }
  else if (*format == 'S') //SVG
  {
    char* filename = (char*)data;

    mglGraphPS* gr = new mglGraphPS(w, h);
    iMglPlotDrawPlot(ih, gr);
    gr->WriteSVG(filename, "IupMglPlot");

    delete gr;
  }
  else if (*format == 'R') //RGB
  {
    int alpha = 0;
    const unsigned char *input_bits;
    unsigned char* output_bits = (unsigned char*)data;
    if (*(format+3) == 'A')
      alpha = 1;

    mglGraphZB* gr = new mglGraphZB(w, h);
    iMglPlotDrawPlot(ih, gr);
    if (alpha)
      input_bits = gr->GetRGBA();
    else
      input_bits = gr->GetBits();
    memcpy(output_bits, input_bits, w*h*(alpha? 4:3));

    delete gr;
  }

  ih->data->w = old_w;
  ih->data->h = old_h;
  ih->data->dpi = old_dpi;
}

void IupMglPlotDrawMark(Ihandle* ih, float x, float y, float z)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  mglGraph* gr = (mglGraph*)iupAttribGet(ih, "_IUP_MGLPLOT_GRAPH");
  if (!gr)
    return;

  char* value = iupAttribGetStr(ih, "DRAWCOLOR");
  mglColor markColor;
  if (!value) markColor = ih->data->fgColor;
  else iMglPlotSetColor(ih, value, markColor);
  iMglPlotConfigColor(ih, gr, markColor);

  value = iupAttribGetStr(ih, "DRAWMARKSTYLE");
  char markstyle = 'x';
  iMglPlotSetMarkStyle(ih, value, markstyle);

  value = iupAttribGetStr(ih, "DRAWMARKSIZE");
  float marksize = 0.02f;
  iMglPlotSetFloat(ih, value, marksize);
  gr->SetMarkSize(marksize);

  gr->Mark(mglPoint(x, y, z), markstyle);
}

void IupMglPlotDrawLine(Ihandle* ih, float x1, float y1, float z1, float x2, float y2, float z2)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  mglGraph* gr = (mglGraph*)iupAttribGet(ih, "_IUP_MGLPLOT_GRAPH");
  if (!gr)
    return;

  char* value = iupAttribGetStr(ih, "DRAWCOLOR");
  mglColor lineColor;
  if (!value) lineColor = ih->data->fgColor;
  else iMglPlotSetColor(ih, value, lineColor);

  value = iupAttribGetStr(ih, "DRAWLINESTYLE");
  char linestyle = '-';
  iMglPlotSetLineStyle(ih, value, linestyle);

  value = iupAttribGetStr(ih, "DRAWLINEWIDTH");
  float linewidth = 0;
  iupStrToFloat(value, &linewidth);
  if (linewidth<=0) linewidth = 1.0f;

  char pen[10];
  iMglPlotConfigColor(ih, gr, lineColor);
  iMglPlotConfigPen(gr, pen, linestyle, linewidth);
  gr->Line(mglPoint(x1, y1, z1), mglPoint(x2, y2, z2), pen);
}

void IupMglPlotDrawText(Ihandle* ih, const char* text, float x, float y, float z)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return;

  if (ih->iclass->nativetype != IUP_TYPECANVAS || 
    !IupClassMatch(ih, "mglplot"))
    return;

  mglGraph* gr = (mglGraph*)iupAttribGet(ih, "_IUP_MGLPLOT_GRAPH");
  if (!gr)
    return;

  char* value = iupAttribGetStr(ih, "DRAWCOLOR");
  mglColor textColor;
  if (!value) textColor = ih->data->fgColor;
  else iMglPlotSetColor(ih, value, textColor);

  value = iupAttribGetStr(ih, "DRAWFONTSTYLE");
  int fontstyle = IUP_MGLPLOT_INHERIT;
  iMglPlotSetFontStyle(ih, value, fontstyle);

  value = iupAttribGetStr(ih, "DRAWFONTSIZE");
  float fontsize = 1.0f;
  iMglPlotSetFloat(ih, value, fontsize);

  char* style = iupAttribGetStr(ih, "DRAWTEXTALIGN");
  if (style)
  {
    if (iupStrEqualNoCase(style, "LEFT"))
      style = "L";
    else if (iupStrEqualNoCase(style, "CENTER"))
      style = "C";
    else if (iupStrEqualNoCase(style, "RIGHT"))
      style = "R";
    else
      style = NULL;
  }

  iMglPlotConfigColor(ih, gr, textColor);
  iMglPlotConfigFont(ih, gr, fontstyle, fontsize);
  
  // Check if all characters are loaded
  if (ih->data->useMakeFont)
    ih->data->makeFont->mglMakeFontSearchGlyph(gr, text);

  gr->Puts(mglPoint(x, y, z), text, style, -1);
}


/******************************************************************************
 Canvas Callbacks
******************************************************************************/


static void iMglPlotZoom(Ihandle *ih, float factor)
{
  float rh = ih->data->y2 - ih->data->y1;
  float ry = (rh * factor) / (float)ih->data->h;
  float rw = ih->data->x2 - ih->data->x1;
  float rx = (rw * factor) / (float)ih->data->w;
  ih->data->y1 += ry;
  ih->data->y2 -= ry;
  ih->data->x1 += rx;
  ih->data->x2 -= rx;

  //TODO: temporary-commented while adjusting sizes
  //if (ih->data->x1 < 0) ih->data->x1 = 0;
  //if (ih->data->y1 < 0) ih->data->y1 = 0;
  //if (ih->data->x2 > 1.0f) ih->data->x2 = 1.0f;
  //if (ih->data->y2 > 1.0f) ih->data->y2 = 1.0f;
}

static void iMglPlotPanY(Ihandle *ih, float yoffset)
{
  float rh = ih->data->y2 - ih->data->y1;
  float ry = (rh * yoffset) / (float)ih->data->h;
  if (ih->data->y1 + ry < 0) ry = -ih->data->y1;
  if (ih->data->y2 + ry > 1.0f) ry = 1.0f - ih->data->y2;
  ih->data->y1 += ry;
  ih->data->y2 += ry;
}

static void iMglPlotPanX(Ihandle *ih, float xoffset)
{
  float rw = ih->data->x2 - ih->data->x1;
  float rx = (rw * xoffset) / (float)ih->data->w;
  if (ih->data->x1 + rx < 0) rx = -ih->data->x1;
  if (ih->data->x2 + rx > 1.0f) rx = 1.0f - ih->data->x2;
  ih->data->x1 += rx;
  ih->data->x2 += rx;
}

static void iMglPlotRotate(float &angle, float delta)
{
  angle += delta;

  // Keep between -360:+360 just to return a nice value in the getattrib method
  angle = fmod(angle, 360.0f);
}

static int iMglPlotResize_CB(Ihandle* ih, int width, int height)
{
  IupGLMakeCurrent(ih);
  glViewport(0, 0, width, height);

  if (!ih->data->opengl)
  {
    iMglPlotInitOpenGL2D();
    ih->data->mgl->SetSize(width, height);
  }

  ih->data->redraw = true;
  ih->data->w = width;
  ih->data->h = height;
  ih->data->dpi = IupGetFloat(NULL, "SCREENDPI");

  return IUP_DEFAULT;
}

static int iMglPlotRedraw_CB(Ihandle* ih)
{
  iMglPlotRepaint(ih, 0, 1);  /* full redraw only if nothing changed */
  return IUP_DEFAULT;
}

static int iMglPlotMouseButton_CB(Ihandle* ih, int b, int press, int x, int y, char* status)
{
  if (press)  /* Any Button pressed */
  {
    /* Initial (x,y) */
    ih->data->last_x = (float)x;
    ih->data->last_y = (float)y;
  }

  if (iup_isdouble(status))  /* Double-click: restore interaction default values */
  {
    iMglPlotResetInteraction(ih);
    iMglPlotRepaint(ih, 1, 1);
  }

  (void)b;
  return IUP_DEFAULT;
}

static int iMglPlotMouseMove_CB(Ihandle* ih, int x, int y, char *status)
{
  float cur_x = (float)x;
  float cur_y = (float)y;

  if (iup_isbutton3(status))
  {
    // This is the same computation done in MathGL widgets
    float ff = 240.0f / sqrt(float(ih->data->w * ih->data->h));
    float deltaZ = (ih->data->last_x - cur_x) * ff;
    float deltaX = (ih->data->last_y - cur_y) * ff;

    iMglPlotRotate(ih->data->rotX, deltaX);
    iMglPlotRotate(ih->data->rotZ, deltaZ);

    iMglPlotRepaint(ih, 1, 1);
  }
  else if(iup_isbutton1(status))
  {
    if (iup_iscontrol(status))
    {
      /* Zoom with vertical movement */
      float factor = 10.0f * (ih->data->last_y - cur_y);
      iMglPlotZoom(ih, factor);
    }
    else
    {
      /* Pan */
      float xoffset = ih->data->last_x - cur_x;
      float yoffset = cur_y - ih->data->last_y;  /* Inverted because Y in IUP is top-down */

      iMglPlotPanX(ih, xoffset);
      iMglPlotPanY(ih, yoffset);
    }

    iMglPlotRepaint(ih, 1, 1);
  }

  ih->data->last_x = cur_x;
  ih->data->last_y = cur_y;

  return IUP_DEFAULT;
}

static int iMglPlotWheel_CB(Ihandle* ih, float delta)
{
  if(delta > 0)  /* Zoom In */
  {
    iMglPlotZoom(ih, 50.0f);
    iMglPlotRepaint(ih, 1, 1);
  }
  else if(delta < 0)  /* Zoom Out */
  {
    iMglPlotZoom(ih, -50.0f);
    iMglPlotRepaint(ih, 1, 1);
  }

  return IUP_DEFAULT;
}

static int iMglPlotKeyPress_CB(Ihandle* ih, int c, int press)
{
  if(!press)
    return IUP_DEFAULT;

  switch(c)
  {
  /* Restore interaction default values */
  case K_HOME:
    iMglPlotResetInteraction(ih);
    break;
  /* Pan */
  case K_cUP:
  case K_UP:
    if (c == K_cUP)
      iMglPlotPanY(ih, -10.0f);
    else
      iMglPlotPanY(ih, -1.0f);
    break;
  case K_cDOWN:
  case K_DOWN:
    if (c == K_cDOWN)
      iMglPlotPanY(ih, 10.0f);
    else
      iMglPlotPanY(ih, 1.0f);
    break;
  case K_cLEFT:
  case K_LEFT:
    if (c == K_cLEFT)
      iMglPlotPanX(ih, 10.0f);
    else
      iMglPlotPanX(ih, 1.0f);
    break;
  case K_cRIGHT:
  case K_RIGHT:
    if (c == K_cRIGHT)
      iMglPlotPanX(ih, -10.0f);
    else
      iMglPlotPanX(ih, -1.0f);
    break;
  /* Zoom */
  case K_plus:
    iMglPlotZoom(ih, 10.0f);
    break;
  case K_minus:
    iMglPlotZoom(ih, -10.0f);
    break;
  /* Rotation */
  case K_A: case K_a:
    iMglPlotRotate(ih->data->rotY, +1.0f);     // 1 degree
    break;
  case K_D: case K_d:
    iMglPlotRotate(ih->data->rotY, -1.0f);
    break;
  case K_W: case K_w:
    iMglPlotRotate(ih->data->rotX, +1.0f);
    break;
  case K_S: case K_s:
    iMglPlotRotate(ih->data->rotX, -1.0f);
    break;
  case K_E: case K_e:
    iMglPlotRotate(ih->data->rotZ, +1.0f);
    break;
  case K_Q: case K_q:
    iMglPlotRotate(ih->data->rotZ, -1.0f);
    break;
  default:
    return IUP_DEFAULT;
  }

  iMglPlotRepaint(ih, 1, 1);
  return IUP_DEFAULT;
} 

/******************************************************************************
  Class Methods
******************************************************************************/

static void iMglPlotDestroyMethod(Ihandle* ih)
{
  int i;
  
  if(ih->data->makeFont)
    free(ih->data->makeFont);

  /* PLOT End for the current stream */
  for(i = 0; i < ih->data->dataSetCount; i++)
    iMglPlotRemoveDataSet(&ih->data->dataSet[i]);

  delete ih->data->mgl;
}

static int iMglPlotCreateMethod(Ihandle* ih, void **params)
{
  (void)params;

  /* free the data allocated by IupCanvas */
  free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  ih->data->dataSetMaxCount = 5;
  ih->data->dataSet = (IdataSet*)malloc(sizeof(IdataSet)*ih->data->dataSetMaxCount);
  memset(ih->data->dataSet, 0, sizeof(IdataSet)*ih->data->dataSetMaxCount);
  ih->data->dataSetCurrent = -1;

  /* IupCanvas callbacks */
  IupSetCallback(ih, "RESIZE_CB",   (Icallback)iMglPlotResize_CB);
  IupSetCallback(ih, "ACTION",      (Icallback)iMglPlotRedraw_CB);
  IupSetCallback(ih, "BUTTON_CB",   (Icallback)iMglPlotMouseButton_CB);
  IupSetCallback(ih, "WHEEL_CB",    (Icallback)iMglPlotWheel_CB);
  IupSetCallback(ih, "KEYPRESS_CB", (Icallback)iMglPlotKeyPress_CB);
  IupSetCallback(ih, "MOTION_CB",   (Icallback)iMglPlotMouseMove_CB);

  IupSetAttribute(ih, "BUFFER", "DOUBLE");

  ih->data->redraw = true;
  ih->data->w = 1;
  ih->data->h = 1;
  ih->data->mgl = new mglGraphZB(ih->data->w, ih->data->h);

  ih->data->makeFont = (mglMakeFont*)malloc(sizeof(mglMakeFont));
  
  // Default values
  iMglPlotReset(ih);

  return IUP_NOERROR;
}

static Iclass* iMglPlotNewClass(void)
{
  Iclass* ic = iupClassNew(iupRegisterFindClass("glcanvas"));

  ic->name = "mglplot";
  ic->format = NULL;  /* none */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->New     = iMglPlotNewClass;
  ic->Create  = iMglPlotCreateMethod;
  ic->Destroy = iMglPlotDestroyMethod;

   /* IupPPlot Callbacks */
   iupClassRegisterCallback(ic, "POSTDRAW_CB", "");
   iupClassRegisterCallback(ic, "PREDRAW_CB", "");
  // TODO
//   iupClassRegisterCallback(ic, "DELETE_CB", "iiff");
//   iupClassRegisterCallback(ic, "DELETEBEGIN_CB", "");
//   iupClassRegisterCallback(ic, "DELETEEND_CB", "");
//   iupClassRegisterCallback(ic, "SELECT_CB", "iiffi");
//   iupClassRegisterCallback(ic, "SELECTBEGIN_CB", "");
//   iupClassRegisterCallback(ic, "SELECTEND_CB", "");
//   iupClassRegisterCallback(ic, "EDIT_CB", "iiffvv");
//   iupClassRegisterCallback(ic, "EDITBEGIN_CB", "");
//   iupClassRegisterCallback(ic, "EDITEND_CB", "");

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", iMglPlotGetBGColorAttrib, iMglPlotSetBGColorAttrib, "255 255 255", NULL, IUPAF_NOT_MAPPED);   /* overwrite canvas implementation, set a system default to force a new default */
  iupClassRegisterAttribute(ic, "FGCOLOR", iMglPlotGetFGColorAttrib, iMglPlotSetFGColorAttrib, IUPAF_SAMEASSYSTEM, "0 0 0", IUPAF_NOT_MAPPED);

  /* IupMglPlot only */
  iupClassRegisterAttribute(ic, "REDRAW", NULL, iMglPlotSetRedrawAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALPHA", iMglPlotGetAlphaAttrib, iMglPlotSetAlphaAttrib, IUPAF_SAMEASSYSTEM, "0.5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TRANSPARENT", iMglPlotGetTransparentAttrib, iMglPlotSetTransparentAttrib, IUPAF_SAMEASSYSTEM, "No", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "OPENGL", iMglPlotGetOpenGLAttrib, iMglPlotSetOpenGLAttrib, IUPAF_SAMEASSYSTEM, "No", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ANTIALIAS", iMglPlotGetAntialiasAttrib, iMglPlotSetAntialiasAttrib, "Yes", NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RESET", NULL, iMglPlotSetResetAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ERRORMESSAGE", iMglPlotGetErrorMessageAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "TITLE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLECOLOR", iMglPlotGetTitleColorAttrib, iMglPlotSetTitleColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TITLEFONTSIZE", iMglPlotGetTitleFontSizeAttrib, iMglPlotSetTitleFontSizeAttrib, IUPAF_SAMEASSYSTEM, "1.6", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TITLEFONTSTYLE", iMglPlotGetTitleFontStyleAttrib, iMglPlotSetTitleFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "LEGEND", iMglPlotGetLegendShowAttrib, iMglPlotSetLegendShowAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDSHOW", iMglPlotGetLegendShowAttrib, iMglPlotSetLegendShowAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT); // for IupPPlot compatibility
  iupClassRegisterAttribute(ic, "LEGENDBOX", iMglPlotGetLegendBoxAttrib, iMglPlotSetLegendBoxAttrib, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDPOS", iMglPlotGetLegendPosAttrib, iMglPlotSetLegendPosAttrib, IUPAF_SAMEASSYSTEM, "TOPRIGHT", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDFONTSIZE", iMglPlotGetLegendFontSizeAttrib, iMglPlotSetLegendFontSizeAttrib, IUPAF_SAMEASSYSTEM, "0.8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDFONTSTYLE", iMglPlotGetLegendFontStyleAttrib, iMglPlotSetLegendFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "LEGENDCOLOR", iMglPlotGetLegendColorAttrib, iMglPlotSetLegendColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "GRIDLINESTYLE", iMglPlotGetGridLineStyleAttrib, iMglPlotSetGridLineStyleAttrib, IUPAF_SAMEASSYSTEM, "CONTINUOUS", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRIDCOLOR", iMglPlotGetGridColorAttrib, iMglPlotSetGridColorAttrib, IUPAF_SAMEASSYSTEM, "200 200 200", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRID", iMglPlotGetGridAttrib, iMglPlotSetGridAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "REMOVE", NULL, iMglPlotSetRemoveAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLEAR", NULL, iMglPlotSetClearAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COUNT", iMglPlotGetCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CURRENT", iMglPlotGetCurrentAttrib, iMglPlotSetCurrentAttrib, IUPAF_SAMEASSYSTEM, "-1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DS_LINESTYLE", iMglPlotGetDSLineStyleAttrib, iMglPlotSetDSLineStyleAttrib, IUPAF_SAMEASSYSTEM, "CONTINUOUS", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_LINEWIDTH", iMglPlotGetDSLineWidthAttrib, iMglPlotSetDSLineWidthAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MARKSTYLE", iMglPlotGetDSMarkStyleAttrib, iMglPlotSetDSMarkStyleAttrib, IUPAF_SAMEASSYSTEM, "X", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MARKSIZE", iMglPlotGetDSMarkSizeAttrib, iMglPlotSetDSMarkSizeAttrib, IUPAF_SAMEASSYSTEM, "0.02", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_SHOWMARKS", iMglPlotGetDSShowMarksAttrib, iMglPlotSetDSShowMarksAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_LEGEND", iMglPlotGetDSLegendAttrib, iMglPlotSetDSLegendAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_COLOR", iMglPlotGetDSColorAttrib, iMglPlotSetDSColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_SHOWVALUES", iMglPlotGetDSShowValuesAttrib, iMglPlotSetDSShowValuesAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_MODE", iMglPlotGetDSModeAttrib, iMglPlotSetDSModeAttrib, IUPAF_SAMEASSYSTEM, "LINE", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  //iupClassRegisterAttribute(ic, "DS_EDIT", iMglPlotGetDSEditAttrib, iMglPlotSetDSEditAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_REMOVE", NULL, iMglPlotSetDSRemoveAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_COUNT", iMglPlotGetDSCountAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_DIMENSION", iMglPlotGetDSDimAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_REARRANGE", NULL, iMglPlotSetDSRearrangeAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DS_SPLIT", NULL, iMglPlotSetDSSplitAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DATAGRID", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PLANARVALUE", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GRADLINESCOUNT", NULL, NULL, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXIALCOUNT", NULL, NULL, IUPAF_SAMEASSYSTEM, "3", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CONTOURFILLED", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CONTOURLABELS", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CONTOURCOUNT", NULL, NULL, IUPAF_SAMEASSYSTEM, "7", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DIR", NULL, NULL, IUPAF_SAMEASSYSTEM, "Y", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLOUDCUBES", NULL, NULL, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SLICEX", NULL, NULL, IUPAF_SAMEASSYSTEM, "-1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SLICEY", NULL, NULL, IUPAF_SAMEASSYSTEM, "-1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SLICEZ", NULL, NULL, IUPAF_SAMEASSYSTEM, "-1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SLICEDIR", NULL, NULL, IUPAF_SAMEASSYSTEM, "XYZ", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PROJECTVALUEX", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PROJECTVALUEY", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PROJECTVALUEZ", NULL, NULL, IUPAF_SAMEASSYSTEM, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PROJECT", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ISOCOUNT", NULL, NULL, IUPAF_SAMEASSYSTEM, "3", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BARWIDTH", NULL, NULL, IUPAF_SAMEASSYSTEM, "0.7", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RADARSHIFT", NULL, NULL, IUPAF_SAMEASSYSTEM, "-1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "PIECHART", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CRUSTRADIUS", NULL, NULL, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "LIGHT", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COLORSCHEME", NULL, NULL, IUPAF_SAMEASSYSTEM, "BbcyrR", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "COLORBAR", NULL, NULL, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COLORBARPOS", NULL, NULL, IUPAF_SAMEASSYSTEM, "RIGHT", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COLORBARRANGE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COLORBARAXISTICKS", NULL, NULL, IUPAF_SAMEASSYSTEM, "Z", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BOX", iMglPlotGetBoxAttrib, iMglPlotSetBoxAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BOXTICKS", iMglPlotGetBoxTicksAttrib, iMglPlotSetBoxTicksAttrib, IUPAF_SAMEASSYSTEM, "Yes", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BOXCOLOR", iMglPlotGetBoxColorAttrib, iMglPlotSetBoxColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "AXS_XLABEL", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YLABEL", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZLABEL", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XLABELCENTERED", iMglPlotGetAxisXLabelCenteredAttrib, iMglPlotSetAxisXLabelCenteredAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YLABELCENTERED", iMglPlotGetAxisYLabelCenteredAttrib, iMglPlotSetAxisYLabelCenteredAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZLABELCENTERED", iMglPlotGetAxisZLabelCenteredAttrib, iMglPlotSetAxisZLabelCenteredAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XLABELPOSITION", iMglPlotGetAxisXLabelPositionAttrib, iMglPlotSetAxisXLabelPositionAttrib, IUPAF_SAMEASSYSTEM, "CENTER", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YLABELPOSITION", iMglPlotGetAxisYLabelPositionAttrib, iMglPlotSetAxisYLabelPositionAttrib, IUPAF_SAMEASSYSTEM, "CENTER", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZLABELPOSITION", iMglPlotGetAxisZLabelPositionAttrib, iMglPlotSetAxisZLabelPositionAttrib, IUPAF_SAMEASSYSTEM, "CENTER", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XLABELROTATION", iMglPlotGetAxisXLabelRotationAttrib, iMglPlotSetAxisXLabelRotationAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YLABELROTATION", iMglPlotGetAxisYLabelRotationAttrib, iMglPlotSetAxisYLabelRotationAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZLABELROTATION", iMglPlotGetAxisZLabelRotationAttrib, iMglPlotSetAxisZLabelRotationAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XCOLOR", iMglPlotGetAxisXColorAttrib, iMglPlotSetAxisXColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YCOLOR", iMglPlotGetAxisYColorAttrib, iMglPlotSetAxisYColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZCOLOR", iMglPlotGetAxisZColorAttrib, iMglPlotSetAxisZColorAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOMIN", iMglPlotGetAxisXAutoMinAttrib, iMglPlotSetAxisXAutoMinAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOMIN", iMglPlotGetAxisYAutoMinAttrib, iMglPlotSetAxisYAutoMinAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZAUTOMIN", iMglPlotGetAxisZAutoMinAttrib, iMglPlotSetAxisZAutoMinAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOMAX", iMglPlotGetAxisXAutoMaxAttrib, iMglPlotSetAxisXAutoMaxAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOMAX", iMglPlotGetAxisYAutoMaxAttrib, iMglPlotSetAxisYAutoMaxAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZAUTOMAX", iMglPlotGetAxisZAutoMaxAttrib, iMglPlotSetAxisZAutoMaxAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XMIN", iMglPlotGetAxisXMinAttrib, iMglPlotSetAxisXMinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YMIN", iMglPlotGetAxisYMinAttrib, iMglPlotSetAxisYMinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZMIN", iMglPlotGetAxisZMinAttrib, iMglPlotSetAxisZMinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XMAX", iMglPlotGetAxisXMaxAttrib, iMglPlotSetAxisXMaxAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YMAX", iMglPlotGetAxisYMaxAttrib, iMglPlotSetAxisYMaxAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZMAX", iMglPlotGetAxisZMaxAttrib, iMglPlotSetAxisZMaxAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XREVERSE", iMglPlotGetAxisXReverseAttrib, iMglPlotSetAxisXReverseAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YREVERSE", iMglPlotGetAxisYReverseAttrib, iMglPlotSetAxisYReverseAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZREVERSE", iMglPlotGetAxisZReverseAttrib, iMglPlotSetAxisZReverseAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_X", iMglPlotGetAxisXShowAttrib, iMglPlotSetAxisXShowAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_Y", iMglPlotGetAxisYShowAttrib, iMglPlotSetAxisYShowAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_Z", iMglPlotGetAxisZShowAttrib, iMglPlotSetAxisZShowAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XARROW", iMglPlotGetAxisXShowArrowAttrib, iMglPlotSetAxisXShowArrowAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YARROW", iMglPlotGetAxisYShowArrowAttrib, iMglPlotSetAxisYShowArrowAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZARROW", iMglPlotGetAxisZShowArrowAttrib, iMglPlotSetAxisZShowArrowAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XCROSSORIGIN", iMglPlotGetAxisXCrossOriginAttrib, iMglPlotSetAxisXCrossOriginAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YCROSSORIGIN", iMglPlotGetAxisYCrossOriginAttrib, iMglPlotSetAxisYCrossOriginAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZCROSSORIGIN", iMglPlotGetAxisZCrossOriginAttrib, iMglPlotSetAxisZCrossOriginAttrib, IUPAF_SAMEASSYSTEM, "NO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XORIGIN", iMglPlotGetAxisXOriginAttrib, iMglPlotSetAxisXOriginAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YORIGIN", iMglPlotGetAxisYOriginAttrib, iMglPlotSetAxisYOriginAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZORIGIN", iMglPlotGetAxisZOriginAttrib, iMglPlotSetAxisZOriginAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XSCALE", iMglPlotGetAxisXScaleAttrib, iMglPlotSetAxisXScaleAttrib, IUPAF_SAMEASSYSTEM, "LIN", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YSCALE", iMglPlotGetAxisYScaleAttrib, iMglPlotSetAxisYScaleAttrib, IUPAF_SAMEASSYSTEM, "LIN", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZSCALE", iMglPlotGetAxisZScaleAttrib, iMglPlotSetAxisZScaleAttrib, IUPAF_SAMEASSYSTEM, "LIN", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XFONTSIZE", iMglPlotGetAxisXFontSizeAttrib, iMglPlotSetAxisXFontSizeAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YFONTSIZE", iMglPlotGetAxisYFontSizeAttrib, iMglPlotSetAxisYFontSizeAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YFONTSIZE", iMglPlotGetAxisZFontSizeAttrib, iMglPlotSetAxisZFontSizeAttrib, IUPAF_SAMEASSYSTEM, "1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XFONTSTYLE", iMglPlotGetAxisXFontStyleAttrib, iMglPlotSetAxisXFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YFONTSTYLE", iMglPlotGetAxisYFontStyleAttrib, iMglPlotSetAxisYFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZFONTSTYLE", iMglPlotGetAxisZFontStyleAttrib, iMglPlotSetAxisZFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "AXS_XTICK", iMglPlotGetAxisXTickAttrib, iMglPlotSetAxisXTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICK", iMglPlotGetAxisYTickAttrib, iMglPlotSetAxisYTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICK", iMglPlotGetAxisZTickAttrib, iMglPlotSetAxisZTickAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKVALUES", iMglPlotGetAxisXTickShowValuesAttrib, iMglPlotSetAxisXTickShowValuesAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKVALUES", iMglPlotGetAxisYTickShowValuesAttrib, iMglPlotSetAxisYTickShowValuesAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKVALUES", iMglPlotGetAxisZTickShowValuesAttrib, iMglPlotSetAxisZTickShowValuesAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKVALUESROTATION", iMglPlotGetAxisXTickValuesRotationAttrib, iMglPlotSetAxisXTickValuesRotationAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKVALUESROTATION", iMglPlotGetAxisYTickValuesRotationAttrib, iMglPlotSetAxisYTickValuesRotationAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKVALUESROTATION", iMglPlotGetAxisZTickValuesRotationAttrib, iMglPlotSetAxisZTickValuesRotationAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFORMAT", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFORMAT", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKFORMAT", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFONTSIZE", iMglPlotGetAxisXTickFontSizeAttrib, iMglPlotSetAxisXTickFontSizeAttrib, IUPAF_SAMEASSYSTEM, "0.8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFONTSIZE", iMglPlotGetAxisYTickFontSizeAttrib, iMglPlotSetAxisYTickFontSizeAttrib, IUPAF_SAMEASSYSTEM, "0.8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKFONTSIZE", iMglPlotGetAxisZTickFontSizeAttrib, iMglPlotSetAxisZTickFontSizeAttrib, IUPAF_SAMEASSYSTEM, "0.8", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKFONTSTYLE", iMglPlotGetAxisXTickFontStyleAttrib, iMglPlotSetAxisXTickFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKFONTSTYLE", iMglPlotGetAxisYTickFontStyleAttrib, iMglPlotSetAxisYTickFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKFONTSTYLE", iMglPlotGetAxisZTickFontStyleAttrib, iMglPlotSetAxisZTickFontStyleAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOTICK", iMglPlotGetAxisXTickAutoAttrib, iMglPlotSetAxisXTickAutoAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOTICK", iMglPlotGetAxisYTickAutoAttrib, iMglPlotSetAxisYTickAutoAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZAUTOTICK", iMglPlotGetAxisZTickAutoAttrib, iMglPlotSetAxisZTickAutoAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKAUTO", iMglPlotGetAxisXTickAutoAttrib, iMglPlotSetAxisXTickAutoAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKAUTO", iMglPlotGetAxisYTickAutoAttrib, iMglPlotSetAxisYTickAutoAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKAUTO", iMglPlotGetAxisZTickAutoAttrib, iMglPlotSetAxisZTickAutoAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKMAJORSPAN", iMglPlotGetAxisXTickMajorSpanAttrib, iMglPlotSetAxisXTickMajorSpanAttrib, IUPAF_SAMEASSYSTEM, "-5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKMAJORSPAN", iMglPlotGetAxisYTickMajorSpanAttrib, iMglPlotSetAxisYTickMajorSpanAttrib, IUPAF_SAMEASSYSTEM, "-5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKMAJORSPAN", iMglPlotGetAxisZTickMajorSpanAttrib, iMglPlotSetAxisZTickMajorSpanAttrib, IUPAF_SAMEASSYSTEM, "-5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKDIVISION", iMglPlotGetAxisXTickMinorDivisionAttrib, iMglPlotSetAxisXTickMinorDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKDIVISION", iMglPlotGetAxisYTickMinorDivisionAttrib, iMglPlotSetAxisYTickMinorDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKDIVISION", iMglPlotGetAxisZTickMinorDivisionAttrib, iMglPlotSetAxisZTickMinorDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKMINORDIVISION", iMglPlotGetAxisXTickMinorDivisionAttrib, iMglPlotSetAxisXTickMinorDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKMINORDIVISION", iMglPlotGetAxisYTickMinorDivisionAttrib, iMglPlotSetAxisYTickMinorDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKMINORDIVISION", iMglPlotGetAxisZTickMinorDivisionAttrib, iMglPlotSetAxisZTickMinorDivisionAttrib, IUPAF_SAMEASSYSTEM, "5", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XAUTOTICKSIZE", iMglPlotGetAxisXTickAutoSizeAttrib, iMglPlotSetAxisXTickAutoSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YAUTOTICKSIZE", iMglPlotGetAxisYTickAutoSizeAttrib, iMglPlotSetAxisYTickAutoSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZAUTOTICKSIZE", iMglPlotGetAxisZTickAutoSizeAttrib, iMglPlotSetAxisZTickAutoSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKAUTOSIZE", iMglPlotGetAxisXTickAutoSizeAttrib, iMglPlotSetAxisXTickAutoSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKAUTOSIZE", iMglPlotGetAxisYTickAutoSizeAttrib, iMglPlotSetAxisYTickAutoSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKAUTOSIZE", iMglPlotGetAxisZTickAutoSizeAttrib, iMglPlotSetAxisZTickAutoSizeAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKMAJORSIZE", iMglPlotGetAxisXTickMajorSizeAttrib, iMglPlotSetAxisXTickMajorSizeAttrib, IUPAF_SAMEASSYSTEM, "0.1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKMAJORSIZE", iMglPlotGetAxisYTickMajorSizeAttrib, iMglPlotSetAxisYTickMajorSizeAttrib, IUPAF_SAMEASSYSTEM, "0.1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKMAJORSIZE", iMglPlotGetAxisZTickMajorSizeAttrib, iMglPlotSetAxisZTickMajorSizeAttrib, IUPAF_SAMEASSYSTEM, "0.1", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_XTICKMINORSIZE", iMglPlotGetAxisXTickMinorSizeAttrib, iMglPlotSetAxisXTickMinorSizeAttrib, IUPAF_SAMEASSYSTEM, "0.6", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_YTICKMINORSIZE", iMglPlotGetAxisYTickMinorSizeAttrib, iMglPlotSetAxisYTickMinorSizeAttrib, IUPAF_SAMEASSYSTEM, "0.6", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "AXS_ZTICKMINORSIZE", iMglPlotGetAxisZTickMinorSizeAttrib, iMglPlotSetAxisZTickMinorSizeAttrib, IUPAF_SAMEASSYSTEM, "0.6", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "DRAWCOLOR", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAWMARKSTYLE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAWLINESTYLE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAWLINEWIDTH", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAWFONTSTYLE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "DRAWFONTSIZE", NULL, NULL, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "ZOOM", iMglPlotGetZoomAttrib, iMglPlotSetZoomAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ROTATE", iMglPlotGetRotateAttrib, iMglPlotSetRotateAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  return ic;
}

Ihandle* IupMglPlot(void)
{
  return IupCreate("mglplot");
}

void IupMglPlotOpen(void)
{
  IupGLCanvasOpen();

  if (!IupGetGlobal("_IUP_MGLPLOT_OPEN"))
  {
    iupRegisterClass(iMglPlotNewClass());
    IupSetGlobal("_IUP_MGLPLOT_OPEN", "1");
  }
}

/* TODO

Next Version:
  Binary internal fonts
  DS_EDIT+Selection+Callbacks
  Properties dialog (see IupGraph)
  IDTF+PDF 3D

Maybe:
  curvilinear coordinates
  plots that need two datasets: BoxPlot, Region, Tens, Mark, Error, Flow, Pipe, Ring
     chart and bars can be combined in one plot (bars then can include above and fall)
  reference datasets
     dataset can be a pointer to the previous data, 
     so the same data can be displayed using different modes using the same memory
  Ternary
  IupMglPlotDrawCurve and IupMglPlotDrawFace

MathGL:
  evaluate interval, [-1,1] x [0,1] x [0,n-1]
  ***improve autoticks computation
  ***Legend does not work in OpenGL
  ***Legend background is always white regardless of plot background
  ***gr->Box AND gr->ContFA have different results in OpenGL. 
     It seems to have an invalid depth. Without OpenGL works fine.
     SOLVED BY CALLING glEnable(GL_DEPTH_TEST), but this affected anti-aliasing.
  gr->Axial is changing something that affects other graphs in OpenGL. Without OpenGL works fine.
  ***graph disapear during zoom in, only in OpenGL, depth clipping?
  ***bars at 0 and n-1
  ***Axis color is fixed in black
  Ticks
     SetTickLen - documentation says negative len puts ticks outside the bounding box, but it is NOT working
     ***TicksVal should follow ticks spacing configuration 
  Fonts
     ***font aspect ratio not being mantained in OpenGL
     option or function to draw an opaque background for text
     BOLD and ITALIC inside TeX formatting does not work for TTF or OTF fonts.
*/
