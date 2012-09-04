/** \file
 * \brief GTK Font mapping
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gtk/gtk.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_attrib.h"
#include "iup_array.h"
#include "iup_object.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_assert.h"

#include "iupgtk_drv.h"


typedef struct _IgtkFont
{
  char standardfont[200];
  PangoFontDescription* fontdesc;
  PangoAttribute* strikethrough;
  PangoAttribute* underline;
  PangoLayout* layout;
  int charwidth, charheight;
} IgtkFont;

static Iarray* gtk_fonts = NULL;
static PangoContext *gtk_fonts_context = NULL;

static void gtkFontUpdate(IgtkFont* gtkfont)
{
  PangoAttrList *attrs;

  pango_layout_set_font_description(gtkfont->layout, gtkfont->fontdesc);

  attrs = pango_layout_get_attributes(gtkfont->layout);
  if (!attrs) 
  {
    attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attribute_copy(gtkfont->strikethrough));
    pango_attr_list_insert(attrs, pango_attribute_copy(gtkfont->underline));
    pango_layout_set_attributes(gtkfont->layout, attrs);
  }
  else
  {
    pango_attr_list_change(attrs, pango_attribute_copy(gtkfont->strikethrough));
    pango_attr_list_change(attrs, pango_attribute_copy(gtkfont->underline));
  }
}

static IgtkFont* gtkFindFont(const char *standardfont)
{
  PangoFontMetrics* metrics;
  PangoFontDescription* fontdesc;
  int i, 
      is_underline = 0,
      is_strikeout = 0,
      count = iupArrayCount(gtk_fonts);

  IgtkFont* fonts = (IgtkFont*)iupArrayGetData(gtk_fonts);

  /* Check if the standardfont already exists in cache */
  for (i = 0; i < count; i++)
  {
    if (iupStrEqualNoCase(standardfont, fonts[i].standardfont))
      return &fonts[i];
  }

  /* not found, create a new one */
  {
    int size = 0, is_pango = 0;
    int is_bold = 0,
      is_italic = 0;
    char typeface[1024];
    const char* mapped_name;

    /* parse the old Windows format first */
    if (!iupFontParseWin(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
    {
      if (!iupFontParseX(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
      {
        if (!iupFontParsePango(standardfont, typeface, &size, &is_bold, &is_italic, &is_underline, &is_strikeout))
          return NULL;
        else
          is_pango = 1;
      }
    }

    /* Map standard names to native names */
    mapped_name = iupFontGetPangoName(typeface);
    if (mapped_name)
    {
      strcpy(typeface, mapped_name);
      is_pango = 0;
    }

    if (is_pango && !is_underline && !is_strikeout && size>0)
      fontdesc = pango_font_description_from_string(standardfont);
    else
    {
      char new_standardfont[200];
      if (size<0)
      {
        double res = ((double)gdk_screen_get_width(gdk_screen_get_default()) / (double)gdk_screen_get_width_mm(gdk_screen_get_default())); /* pixels/mm */
        /* 1 point = 1/72 inch     1 inch = 25.4 mm */
        /* pixel = ((point/72)*25.4)*pixel/mm */
        size = (int)((-size/res)*2.83464567 + 0.5); /* from pixels to points */
      }

      sprintf(new_standardfont, "%s, %s%s%d", typeface, is_bold?"Bold ":"", is_italic?"Italic ":"", size);

      fontdesc = pango_font_description_from_string(new_standardfont);
    }
  }

  if (!fontdesc) 
    return NULL;

  /* create room in the array */
  fonts = (IgtkFont*)iupArrayInc(gtk_fonts);

  strcpy(fonts[i].standardfont, standardfont);
  fonts[i].fontdesc = fontdesc;
  fonts[i].strikethrough = pango_attr_strikethrough_new(is_strikeout? TRUE: FALSE);
  fonts[i].underline = pango_attr_underline_new(is_underline? PANGO_UNDERLINE_SINGLE: PANGO_UNDERLINE_NONE);
  fonts[i].layout = pango_layout_new(gtk_fonts_context);

  metrics = pango_context_get_metrics(gtk_fonts_context, fontdesc, pango_context_get_language(gtk_fonts_context));
  fonts[i].charheight = pango_font_metrics_get_ascent(metrics) + pango_font_metrics_get_descent(metrics);
  fonts[i].charheight = iupGTK_PANGOUNITS2PIXELS(fonts[i].charheight);
  fonts[i].charwidth = pango_font_metrics_get_approximate_char_width(metrics);
  fonts[i].charwidth = iupGTK_PANGOUNITS2PIXELS(fonts[i].charwidth);
  pango_font_metrics_unref(metrics); 

  gtkFontUpdate(&(fonts[i]));

  return &fonts[i];
}

static PangoLayout* gtkFontGetWidgetPangoLayout(Ihandle *ih)
{
  int inherit;
  char *def_value;
  /* only check the  native implementation */
  return (PangoLayout*)iupClassObjectGetAttribute(ih, "WIDGETPANGOLAYOUT", &def_value, &inherit);
}

static IgtkFont* gtkFontCreateNativeFont(Ihandle* ih, const char* value)
{
  IgtkFont *gtkfont = gtkFindFont(value);
  if (!gtkfont)
  {
    iupERROR1("Failed to create Font: %s", value); 
    return NULL;
  }

  iupAttribSetStr(ih, "_IUP_GTKFONT", (char*)gtkfont);
  return gtkfont;
}

static IgtkFont* gtkFontGet(Ihandle *ih)
{
  IgtkFont* gtkfont = (IgtkFont*)iupAttribGet(ih, "_IUP_GTKFONT");
  if (!gtkfont)
    gtkfont = gtkFontCreateNativeFont(ih, iupGetFontAttrib(ih));
  return gtkfont;
}

void iupgtkFontUpdatePangoLayout(Ihandle* ih, PangoLayout* layout)
{
  IgtkFont* gtkfont;
  PangoAttrList *attrs;

  if (!layout)
    return;

  gtkfont = gtkFontGet(ih);
  if (!gtkfont)
    return;

  attrs = pango_layout_get_attributes(layout);
  if (!attrs) 
  {
    attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attribute_copy(gtkfont->strikethrough));
    pango_attr_list_insert(attrs, pango_attribute_copy(gtkfont->underline));
    pango_layout_set_attributes(layout, attrs);
  }
  else
  {
    pango_attr_list_change(attrs, pango_attribute_copy(gtkfont->strikethrough));
    pango_attr_list_change(attrs, pango_attribute_copy(gtkfont->underline));
  }
}

void iupgtkFontUpdateObjectPangoLayout(Ihandle* ih, gpointer object)
{
  PangoAttrList *attrs;

  IgtkFont* gtkfont = gtkFontGet(ih);
  if (!gtkfont)
    return;

  g_object_get(object, "attributes", &attrs, NULL);
  if (!attrs) 
  {
    attrs = pango_attr_list_new();
    pango_attr_list_insert(attrs, pango_attribute_copy(gtkfont->strikethrough));
    pango_attr_list_insert(attrs, pango_attribute_copy(gtkfont->underline));
    g_object_set(object, "attributes", attrs, NULL);
  }
  else
  {
    pango_attr_list_change(attrs, pango_attribute_copy(gtkfont->strikethrough));
    pango_attr_list_change(attrs, pango_attribute_copy(gtkfont->underline));
  }
}

char* iupdrvGetSystemFont(void)
{
  static char str[200]; /* must return a static string, because it will be used as the default value for the FONT attribute */
  GtkStyle* style;
  GtkWidget* widget = gtk_invisible_new();
  gtk_widget_realize(widget);
  style = gtk_widget_get_style(widget);
  if (!style || !style->font_desc)
    strcpy(str, "Sans, 10");
  else
  {
    char* desc = pango_font_description_to_string(style->font_desc);
    strcpy(str, desc);
    g_free(desc);
  }
  gtk_widget_unrealize(widget);
  gtk_widget_destroy(widget);
  return str;
}

char* iupgtkFindPangoFontDesc(PangoFontDescription* fontdesc)
{
  int i, count = iupArrayCount(gtk_fonts);
  IgtkFont* fonts = (IgtkFont*)iupArrayGetData(gtk_fonts);

  /* Check if the standardfont already exists in cache */
  for (i = 0; i < count; i++)
  {
    if (pango_font_description_equal(fontdesc, fonts[i].fontdesc))
      return fonts[i].standardfont;
  }

  return NULL;
}

PangoFontDescription* iupgtkGetPangoFontDesc(const char* value)
{
  IgtkFont *gtkfont = gtkFindFont(value);
  if (!gtkfont)
  {
    iupERROR1("Failed to create Font: %s", value); 
    return NULL;
  }
  return gtkfont->fontdesc;
}

PangoLayout* iupgtkGetPangoLayout(const char* value)
{
  IgtkFont *gtkfont = gtkFindFont(value);
  if (!gtkfont)
  {
    iupERROR1("Failed to create Font: %s", value); 
    return NULL;
  }
  return gtkfont->layout;
}

char* iupgtkGetPangoFontDescAttrib(Ihandle *ih)
{
  IgtkFont* gtkfont = gtkFontGet(ih);
  if (!gtkfont)
    return NULL;
  else
    return (char*)gtkfont->fontdesc;
}

char* iupgtkGetPangoLayoutAttrib(Ihandle *ih)
{
  IgtkFont* gtkfont = gtkFontGet(ih);
  if (!gtkfont)
    return NULL;
  else
    return (char*)gtkfont->layout;
}

char* iupgtkGetFontIdAttrib(Ihandle *ih)
{
  IgtkFont* gtkfont = gtkFontGet(ih);
  if (!gtkfont)
    return NULL;
  else
  {
    GdkFont* gdk_font = gdk_font_from_description(gtkfont->fontdesc);
    return (char*)gdk_font_id(gdk_font);  /* In UNIX will return an X Font ID, in Win32 will return an HFONT */
  }
}

int iupdrvSetStandardFontAttrib(Ihandle* ih, const char* value)
{
  IgtkFont* gtkfont = gtkFontCreateNativeFont(ih, value);
  if (!gtkfont)
    return 1;

  /* If FONT is changed, must update the SIZE attribute */
  iupBaseUpdateSizeFromFont(ih);

  /* FONT attribute must be able to be set before mapping, 
    so the font is enable for size calculation. */
  if (ih->handle && (ih->iclass->nativetype != IUP_TYPEVOID))
  {
    gtk_widget_modify_font(ih->handle, gtkfont->fontdesc);
    iupgtkFontUpdatePangoLayout(ih, gtkFontGetWidgetPangoLayout(ih));
  }

  return 1;
}

void iupdrvFontGetMultiLineStringSize(Ihandle* ih, const char* str, int *w, int *h)
{
  int max_w = 0;

  IgtkFont* gtkfont = gtkFontGet(ih);
  if (!gtkfont)
  {
    if (w) *w = 0;
    if (h) *h = 0;
    return;
  }

  if (!str)
  {
    if (w) *w = 0;
    if (h) *h = gtkfont->charheight * 1;
    return;
  }

  if (str[0])
  {
    int dummy_h;

    pango_layout_set_attributes(gtkfont->layout, NULL);

    if (iupAttribGetBoolean(ih, "MARKUP"))
      pango_layout_set_markup(gtkfont->layout, iupgtkStrConvertToUTF8(str), -1);
    else
      pango_layout_set_text(gtkfont->layout, iupgtkStrConvertToUTF8(str), -1);

    pango_layout_get_pixel_size(gtkfont->layout, &max_w, &dummy_h);
  }

  if (w) *w = max_w;
  if (h) *h = gtkfont->charheight * iupStrLineCount(str);
}

int iupdrvFontGetStringWidth(Ihandle* ih, const char* str)
{
  IgtkFont* gtkfont;
  int len, w;
  char* line_end;

  if (!str || str[0]==0)
    return 0;

  gtkfont = gtkFontGet(ih);
  if (!gtkfont)
    return 0;

  line_end = strchr(str, '\n');
  if (line_end)
    len = line_end-str;
  else
    len = strlen(str);

  if (iupAttribGetBoolean(ih, "MARKUP"))
    pango_layout_set_markup(gtkfont->layout, iupgtkStrConvertToUTF8(str), len);
  else
    pango_layout_set_text(gtkfont->layout, iupgtkStrConvertToUTF8(str), len);

  pango_layout_get_pixel_size(gtkfont->layout, &w, NULL);
  return w;
}

void iupdrvFontGetCharSize(Ihandle* ih, int *charwidth, int *charheight)
{
  IgtkFont* gtkfont = gtkFontGet(ih);
  if (!gtkfont)
  {
    if (charwidth)  *charwidth = 0;
    if (charheight) *charheight = 0;
    return;
  }

  if (charheight) 
    *charheight = gtkfont->charheight;

  if (charwidth)
    *charwidth = gtkfont->charwidth;
}

void iupdrvFontInit(void)
{
  gtk_fonts = iupArrayCreate(50, sizeof(IgtkFont));
  gtk_fonts_context = gdk_pango_context_get();
  pango_context_set_language(gtk_fonts_context, gtk_get_default_language());
}

void iupdrvFontFinish(void)
{
  int i, count = iupArrayCount(gtk_fonts);
  IgtkFont* fonts = (IgtkFont*)iupArrayGetData(gtk_fonts);
  for (i = 0; i < count; i++)
  {
    pango_font_description_free(fonts[i].fontdesc);
    pango_attribute_destroy(fonts[i].strikethrough);
    pango_attribute_destroy(fonts[i].underline);
  }
  iupArrayDestroy(gtk_fonts);
  g_object_unref(gtk_fonts_context);
}
