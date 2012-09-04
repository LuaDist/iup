/*
* MGL_MAKEFONT.CPP
*
* Description.: module to load a Free Type font, and to define its mglFont
*
* Based on....: simple.cpp - simple demo for FTGL, the OpenGL font library
*
* Copyright (c) 2008 Sam Hocevar <sam@zoy.org>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <FTGL/ftgl.h>

#include "mgl/mgl.h"

#include "FTVector.h"
#include "FTVectoriser.h"

#include "mgl_makefont.h"

typedef struct
{
  size_t first;
  size_t second;
  size_t third;
} FTTriangle;

typedef FTVector<FTTriangle> FTTriangleVector;
typedef FTVector<FTPoint> FTPointVector;

class FTScreenBuffer
{
public:
  FTScreenBuffer() : width(0), bBox(1048576.0f, 1048576.0f, 1048576.0f, -1048576.0f, -1048576.0f, -1048576.0f) {}

  void clear()
  {
    contourpoints.clear();
    points.clear();
    triangles.clear();
    width = 0;
    bBox = FTBBox(1048576.0f, 1048576.0f, 1048576.0f, -1048576.0f, -1048576.0f, -1048576.0f);
  }
  
  double width;
  FTBBox bBox;
  FTTriangleVector triangles;
  FTPointVector points;
  FTPointVector contourpoints;
};

class TriangleGlyph
{
public:
  unsigned int code;
  double width;
  FTBBox bBox;
  FTTriangleVector triangles;
  FTPointVector points;
  FTPointVector contourpoints;
};

typedef FTVector<TriangleGlyph> TriangleGlyphVector;

/**
 * FTvtkGlyph is a specialization of FTGlyph for creating tessellated
 * polygon glyphs in memory.
 *
 */

class FTVectoriser;

class FTVtkGlyph : public FTGlyph
{
public:
  /**
   * Constructor. Sets the Error to Invalid_Outline if the glyphs
   * isn't an outline.
   *
   * @param glyph The Freetype glyph to be processed
   */
  FTVtkGlyph(FT_GlyphSlot glyph, FTScreenBuffer* sb);
  
  /**
   * Render this glyph at the current pen position.
   *
   * @param pen  The current pen position.
   * @param renderMode  Render mode to display
   * @return  The advance distance for this glyph.
   */
  virtual const FTPoint& Render(const FTPoint& pen, int renderMode);
  
  /**
   * Return the advance width for this glyph.
   *
   * @return  advance width.
   */
  virtual float Advance() const;
  
  /**
   * Return the bounding box for this glyph.
   *
   * @return  bounding box.
   */
  virtual const FTBBox& BBox() const;
  
  /**
   * Queries for errors.
   *
   * @return  The current error code.
   */
  virtual FT_Error Error() const;
  
private:
  /**
   * The advance distance for this glyph
   */
  FTPoint advance;
  
  /**
   * The bounding box of this glyph.
   */
  FTBBox bBox;
  
  /**
   * Current error code. Zero means no error.
   */
  FT_Error err;
  
  /**
   * Private rendering variables.
   */
  FTTriangleVector triangles;
  FTPointVector points;
  FTPointVector contourpoints;
  FTScreenBuffer* screenBuffer;

  size_t AddPoint(const FTPoint& Point);
  void AddTriangle(const FTPoint& Point0, const FTPoint& Point1, const FTPoint& Point2);
};

FTVtkGlyph::FTVtkGlyph(FT_GlyphSlot glyph, FTScreenBuffer* sb) 
:FTGlyph(glyph), err(0), screenBuffer(sb)
{
  if(glyph)
  {
    bBox = FTBBox(glyph);
    advance = FTPoint(glyph->advance.x / 64.0f,
                      glyph->advance.y / 64.0f);
  }
  
  if(ft_glyph_format_outline != glyph->format)
  {
    err = 0x14;  /* Invalid_Outline */
    return;
  }
  
  FTVectoriser vectoriser(glyph);
  
  if((vectoriser.ContourCount() < 1) || (vectoriser.PointCount() < 3))
    return;
  
  for(size_t c = 0; c < vectoriser.ContourCount(); ++c)
  {
    const FTContour* contour = vectoriser.Contour(c);
      
    for(size_t pointIndex = 0; pointIndex < contour->PointCount(); ++pointIndex)
    {
      FTPoint point = contour->Point(pointIndex);
      contourpoints.push_back((1/64.0) * point);
    }
    contourpoints.push_back(FTPoint(-1024.0, -1024.0, -1024.0));
  }
  
  vectoriser.MakeMesh(1.0, 0, 0);
  
  const FTMesh *mesh = vectoriser.GetMesh();
  
  for(size_t index = 0; index < mesh->TesselationCount(); ++index)
  {
    const FTTesselation* subMesh = mesh->Tesselation(index);
    GLenum polygonType = subMesh->PolygonType();
    
    if(polygonType == GL_TRIANGLE_STRIP)
    {
      size_t PointCount = subMesh->PointCount();
      
      for(size_t x = 2; x < PointCount; ++x)
      {
        if(x % 2)
          AddTriangle(subMesh->Point(x - 1), subMesh->Point(x - 2), subMesh->Point(x));
        else
          AddTriangle(subMesh->Point(x - 2), subMesh->Point(x - 1), subMesh->Point(x));
      }
    }
    else if(polygonType == GL_TRIANGLE_FAN)
    {
      size_t PointCount = subMesh->PointCount();
        
      for(size_t x = 2; x < PointCount; ++x)
        AddTriangle(subMesh->Point(0), subMesh->Point(x - 1), subMesh->Point(x));
    }
    else if(polygonType == GL_TRIANGLES)
    {
      size_t PointCount = subMesh->PointCount();
        
      for(size_t x = 0; x < PointCount; x += 3)
        AddTriangle(subMesh->Point(x + 0), subMesh->Point(x + 1), subMesh->Point(x + 2));
    }
  }
}

float FTVtkGlyph::Advance() const
{
  return advance.Xf();
}

const FTBBox& FTVtkGlyph::BBox() const
{
  return bBox;
}

FT_Error FTVtkGlyph::Error() const
{
  return err;
}

const FTPoint& FTVtkGlyph::Render(const FTPoint& pen, int renderMode)
{
  (void)renderMode;

  if(triangles.size() != 0)
  {
    screenBuffer->width += advance.X();
    FTBBox tmpBox = bBox;
    tmpBox += pen;
    screenBuffer->bBox |= tmpBox;
    
    size_t pointsNumber = screenBuffer->points.size();
    
    for(size_t i = 0; i < points.size(); i++)
      screenBuffer->points.push_back(points[i] + pen);

    for(size_t i = 0; i < triangles.size(); i++)
    {
      FTTriangle triangle;
      triangle.first  = triangles[i].first  + pointsNumber;
      triangle.second = triangles[i].second + pointsNumber;
      triangle.third  = triangles[i].third  + pointsNumber;
      screenBuffer->triangles.push_back(triangle);
    }
  }

  for(size_t i = 0; i < contourpoints.size(); i++)
    screenBuffer->contourpoints.push_back(contourpoints[i] + pen);
  
  return advance;
}

size_t FTVtkGlyph::AddPoint(const FTPoint& Point)
{
  FTPoint NewPoint = Point;
  NewPoint = (1/64.0) * NewPoint;
  
  for(size_t i = 0; i < points.size(); i++)
  {
    if(points[i] == NewPoint)
      return ( i );
  }
 
  points.push_back( NewPoint );
  return (points.size() - 1);
}

void FTVtkGlyph::AddTriangle(const FTPoint& Point0, const FTPoint& Point1, const FTPoint& Point2)
{
  if((Point0.X() == Point1.X() &&
     Point0.X() == Point2.X()) ||
     (Point0.Y() == Point1.Y() &&
     Point0.Y() == Point2.Y()))
  {
    /* degenerate triangle */
  }
  else
  {
    FTTriangle triangle;
    triangle.first  = AddPoint(Point0);
    triangle.second = AddPoint(Point1);
    triangle.third  = AddPoint(Point2);
    triangles.push_back(triangle);
  }
}

class FTVtkFont : public FTFont
{
public:
  FTVtkFont(char const *fontFilePath) 
    :FTFont(fontFilePath) 
    { GlyphLoadFlags(FT_LOAD_NO_HINTING); }

  TriangleGlyphVector TriangleFont;
  FTScreenBuffer screenBuffer;
    
private:
  virtual FTGlyph* MakeGlyph(FT_GlyphSlot slot)
  {
    return new FTVtkGlyph(slot, &screenBuffer);
  }
};


/*******************************/
/********** mglMakeFont ********/
/*******************************/

void mglMakeFont::mglMakeFontSort()
{
  FTVtkFont* fnt = (FTVtkFont*)this->fontFT;
  unsigned int k, aux;
  unsigned int numg = fnt->TriangleFont.size();  
  
  // TODO: implement a better sort algorithm

  /* Sort glyph vector */
  for(k = 0; k < numg; k++)
  {
    for(aux = 0; aux < numg; aux++)
    {
      if(fnt->TriangleFont[k].code < fnt->TriangleFont[aux].code)
      {
        TriangleGlyph glyph;

        glyph.bBox = fnt->TriangleFont[k].bBox;
        glyph.code = fnt->TriangleFont[k].code;
        glyph.contourpoints = fnt->TriangleFont[k].contourpoints;
        glyph.points = fnt->TriangleFont[k].points;
        glyph.triangles = fnt->TriangleFont[k].triangles;
        glyph.width = fnt->TriangleFont[k].width;

        fnt->TriangleFont[k] = fnt->TriangleFont[aux];
        fnt->TriangleFont[aux] = glyph;
      }
    }
  }
}

double mglMakeFont::mglMakeFontCalcFactorDX()
{
  double d1, d2, d3, dx;
  FTVtkFont* fnt = (FTVtkFont*)this->fontFT;

  d1 =  500.0 / fnt->Ascender();
  d2 = -500.0 / fnt->Descender();
  d3 = 1000.0 / this->ww;
  dx = (d1 < d2) ? d1 : d2;
  dx = (dx < d3) ? dx : d3;

  return dx;
}

void mglMakeFont::mglMakeFontMainCopy(int style)
{
  int idxA, idxB, idxC;

  if(style == 3)  /* current style: bold-italic */
  {
    idxA = 0;  idxB = 1;  idxC = 2;
  }
  else if(style == 2)  /* current style: italic */
  {
    idxA = 0;  idxB = 1;  idxC = 3;
  }
  else if(style == 1)  /* current style: bold */
  {
    idxA = 0;  idxB = 2;  idxC = 3;
  }
  else  /* current style: regular */
  {
    idxA = 1;  idxB = 2;  idxC = 3;
  }

  /* copy default factor for other font styles */
  this->fontMGL->fact[idxA] = this->fontMGL->fact[idxB] = this->fontMGL->fact[idxC] = this->fontMGL->fact[style];

  /* copy current style as default for other styles */
  memcpy(this->fontMGL->numl[idxA],  this->fontMGL->numl[style],  this->fontMGL->numg*sizeof(short));
  memcpy(this->fontMGL->numl[idxB],  this->fontMGL->numl[style],  this->fontMGL->numg*sizeof(short));
  memcpy(this->fontMGL->numl[idxC],  this->fontMGL->numl[style],  this->fontMGL->numg*sizeof(short));
  memcpy(this->fontMGL->ln[idxA],    this->fontMGL->ln[style],    this->fontMGL->numg*sizeof(unsigned));
  memcpy(this->fontMGL->ln[idxB],    this->fontMGL->ln[style],    this->fontMGL->numg*sizeof(unsigned));
  memcpy(this->fontMGL->ln[idxC],    this->fontMGL->ln[style],    this->fontMGL->numg*sizeof(unsigned));
  memcpy(this->fontMGL->numt[idxA],  this->fontMGL->numt[style],  this->fontMGL->numg*sizeof(short));
  memcpy(this->fontMGL->numt[idxB],  this->fontMGL->numt[style],  this->fontMGL->numg*sizeof(short));
  memcpy(this->fontMGL->numt[idxC],  this->fontMGL->numt[style],  this->fontMGL->numg*sizeof(short));
  memcpy(this->fontMGL->tr[idxA],    this->fontMGL->tr[style],    this->fontMGL->numg*sizeof(unsigned));
  memcpy(this->fontMGL->tr[idxB],    this->fontMGL->tr[style],    this->fontMGL->numg*sizeof(unsigned));
  memcpy(this->fontMGL->tr[idxC],    this->fontMGL->tr[style],    this->fontMGL->numg*sizeof(unsigned));
  memcpy(this->fontMGL->width[idxA], this->fontMGL->width[style], this->fontMGL->numg*sizeof(short));
  memcpy(this->fontMGL->width[idxB], this->fontMGL->width[style], this->fontMGL->numg*sizeof(short));
  memcpy(this->fontMGL->width[idxC], this->fontMGL->width[style], this->fontMGL->numg*sizeof(short));
}

/********** Prepare solid font data **********/
void mglMakeFont::mglMakeFontPrepareSolidFont(unsigned int k, double dx, unsigned long *cur, unsigned *post, short *numt, short *buf)
{
  unsigned int i, ii, ll, last = 0, j = *cur;
  short *xx, *yy, *tt;
  FTVtkFont* fnt = (FTVtkFont*)this->fontFT;
  const TriangleGlyph &glyph = fnt->TriangleFont[k];

  /* Removing the points in the same line */
  xx = new short[glyph.points.size()];
  yy = new short[glyph.points.size()];
  tt = new short[6*glyph.triangles.size()];

  for(i = 0; i < glyph.points.size(); i++)
  {
    xx[i] = short(dx * glyph.points[i].X() + 0.5);
    yy[i] = short(dx * glyph.points[i].Y() + 0.5);
  }

  for(i = 0; i < glyph.triangles.size(); i++)
  {
    ii = 6 * i;
    ll = 6 * last;

    tt[ii]   = xx[glyph.triangles[i].first];
    tt[ii+1] = yy[glyph.triangles[i].first];
    tt[ii+2] = xx[glyph.triangles[i].second];
    tt[ii+3] = yy[glyph.triangles[i].second];
    tt[ii+4] = xx[glyph.triangles[i].third];
    tt[ii+5] = yy[glyph.triangles[i].third];

    if((i > 0) && (i<glyph.triangles.size()))
    {
      if((tt[ii] == tt[ii+2]) && (tt[ii+1] == tt[ii+3]))
        continue;

      if((tt[ii] == tt[ii+4]) && (tt[ii+1] == tt[ii+5]))
        continue;

      if((tt[ii+2] == tt[ii+4]) && (tt[ii+3] == tt[ii+5]))
        continue;
    }

    memcpy(tt+ll, tt+ii, 6*sizeof(int));
    last++;
  }

  /* Write glyph to tables */
  *numt = (short)last;
  *post = *cur;

  for(i = 0; i < last; i++)
  {
    ii = 6 * i;

    buf[j++] = tt[ii];
    buf[j++] = tt[ii+1];
    buf[j++] = tt[ii+2];
    buf[j++] = tt[ii+3];
    buf[j++] = tt[ii+4];
    buf[j++] = tt[ii+5];
  }

  *cur = j;

  delete []xx;
  delete []yy;
  delete []tt;
}

/********** Prepare wire font data **********/
void mglMakeFont::mglMakeFontPrepareWireFont(unsigned int k, double dx, unsigned long *cur, unsigned *ids, unsigned *posl, short *wdt, short *numl, short *buf)
{
  unsigned int i, last = 0, j = *cur;
  short *xx, *yy;
  FTVtkFont* fnt = (FTVtkFont*)this->fontFT;
  const TriangleGlyph &glyph = fnt->TriangleFont[k];

  /* Removing the points in the same line */
  xx = new short[glyph.contourpoints.size()];
  yy = new short[glyph.contourpoints.size()];

  for(i = 0; i < glyph.contourpoints.size(); i++)
  {
    xx[i] = (short)((glyph.contourpoints[i].X() > -500.) ? int(dx * glyph.contourpoints[i].X() + 0.5) : 0x3fff);
    yy[i] = (short)((glyph.contourpoints[i].Y() > -500.) ? int(dx * glyph.contourpoints[i].Y() + 0.5) : 0x3fff);
  }

  for(i = 0; i < glyph.contourpoints.size(); i++)
  {
    if((i > 0) && (i < glyph.contourpoints.size()))
    {
      if(((xx[i] - xx[last-1]) * (yy[i+1] - yy[last-1]) == (yy[i] - yy[last-1]) * (xx[i+1] - xx[last-1])) &&
        (xx[i] != 0x3fff) && (yy[i] != 0x3fff) && (xx[i+1] != 0x3fff) && (yy[i+1] != 0x3fff))
        continue;
    }
    xx[last] = xx[i];
    yy[last] = yy[i];
    last++;
  }

  if((xx[last-1] == 0x3fff) || (yy[last-1]==0x3fff))
    last--;

  if((xx[last-1] == xx[0]) && (yy[last-1]==yy[0]))
    last--;

  /* Write glyph to tables */
  *ids  = glyph.code;
  *numl = (short)last;
  *posl = *cur;
  *wdt  = short(glyph.width * dx);

  for(i = 0; i < last; i++)
  {
    buf[j++] = xx[i];
    buf[j++] = yy[i];
  }

  *cur = j;

  delete []xx;
  delete []yy;
}

void mglMakeFont::mglMakeFontPrepareFont(int style)
{
  unsigned long i, cur = 0;
  unsigned int k, len = 0;
  unsigned int *ids, *posl, *post;
  short *wdt, *numl, *numt, *buf;
  const mreal mgl_fgen = 56;  /*  (4 * 14)  */
  double dx = this->mglMakeFontCalcFactorDX();
  FTVtkFont* fnt = (FTVtkFont*)this->fontFT;
  unsigned int newSize = this->fontMGL->numg;

  for(k = 0; k < newSize; k++)
    len += fnt->TriangleFont[k].contourpoints.size();

  for(k = 0; k < newSize; k++)
    len += 3 * fnt->TriangleFont[k].points.size();

  ids  = new unsigned[newSize];
  wdt  = new    short[newSize];
  numl = new    short[newSize];
  posl = new unsigned[newSize];
  numt = new    short[newSize];
  post = new unsigned[newSize];
  buf  = new    short[2*len];

  /* Prepare wire font data */
  for(k = 0; k < newSize; k++)
    this->mglMakeFontPrepareWireFont(k, dx, &cur, &ids[k], &posl[k], &wdt[k], &numl[k], buf);

  /* Prepare solid font data */
  for(k = 0; k < newSize; k++)
    this->mglMakeFontPrepareSolidFont(k, dx, &cur, &post[k], &numt[k], buf);

  /* Font style factor */
  this->fontMGL->fact[style] = (float)(dx * mgl_fgen);

  /* Reallocate buffer size */
  this->fontMGL->buf = (short*)realloc(this->fontMGL->buf, (cur + this->fontMGL->numb)*sizeof(short));

  /* Load symbols itself */
  for(k = 0; k < newSize; k++)
  {
    this->fontMGL->id[k] = (wchar_t)ids[k];
    this->fontMGL->width[style][k] = wdt[k];
    this->fontMGL->numl[style][k] = numl[k];
    this->fontMGL->ln[style][k] = posl[k];
    this->fontMGL->numt[style][k] = numt[k];
    this->fontMGL->tr[style][k] = post[k];
  }

  /* copy buffer */
  for(i = this->fontMGL->numb; i < cur; i++)
    this->fontMGL->buf[i] = buf[i];
  
  /* update buffer size */
  this->fontMGL->numb = cur;

  delete []ids;    delete []numl;    delete []numt;    delete []wdt;
  delete []buf;    delete []posl;    delete []post;
}

void mglMakeFont::mglMakeFontAddGlyph(wchar_t chr)
{
  wchar_t wstring[2];
  FTVtkFont *fnt = (FTVtkFont*)this->fontFT;

  /* Print one character string */
  wstring[0] = chr;
  /* Set up some strings with the characters to draw. */
  wstring[1] = '\0';

  /* Clear the screen buffer */
  fnt->screenBuffer.clear();
  fnt->Render(wstring);

  /* If something gets printed, copy output to glyph structure and add it to font */
  if(fnt->screenBuffer.triangles.size() > 0)
  {
    TriangleGlyph glyph;

    glyph.code = chr;
    glyph.width = fnt->screenBuffer.width;
    this->ww = (this->ww > glyph.width) ? this->ww : glyph.width;
    glyph.bBox = fnt->screenBuffer.bBox;
    glyph.points = fnt->screenBuffer.points;
    glyph.triangles = fnt->screenBuffer.triangles;
    glyph.contourpoints = fnt->screenBuffer.contourpoints;
    fnt->TriangleFont.push_back(glyph);
  }
}

/* mglMakeFontSearchSymbols is based on "mglFont::Convert" */
bool mglMakeFont::mglMakeFontSearchSymbols(const wchar_t *str, int size)
{
  bool newGlyph = false;
  unsigned r, k;
  int i0, i;
  wchar_t texCommand[128], ch;  /* TeX command and current char */
 
  for(i = 0; i < size; i++)
  {
    ch = str[i];
    if(ch == '\\')	/* It can be a TeX command */
    {
      if(wcschr(L"{}_^\\@# ", str[i+1]))
      {
        /* No, it is an usual symbol, and these symbols already were
           added during the default load font (mglMakeLoadFontFT) */
        continue;
      }
      else  /* Yes, it is a TeX command */
      {
        i0 = i+1;
        for(k = 0; isalnum(str[++i]) && k < 127; k++)
          texCommand[k] = str[i];
        texCommand[k] = 0;

        r = this->fontMGL->Parse(texCommand);
        if(r == unsigned(-2))
        {
          /* command not found, so use next symbol itself */
          i = i0;
        }
        else if(r && !wcschr(this->fontMGL->id, (wchar_t)r))
        {
          this->mglMakeFontAddGlyph((wchar_t)r);
          newGlyph = true;

          if(str[i]>' ')
            i--;
          if(str[i]==0)
            break;
        }
      }
    }
    else if(!wcschr(this->fontMGL->id, ch))
    {
      this->mglMakeFontAddGlyph(ch);  /* Adding new char */
      newGlyph = true;
    }
  }

  return newGlyph;
}

void mglMakeFont::mglMakeSetFontFT(mglGraph *mgl)
{
  FTVtkFont* fnt = (FTVtkFont*)this->fontFT;

  /* Free memory for all fonts */
  this->fontMGL->Clear();

  this->fontMGL->numg = fnt->TriangleFont.size();  /* Number of glyphs */
  this->fontMGL->parse = true;  /* TeX formats */
  this->fontMGL->numb = 0;

  /* Allocate memory for all fonts */
  this->fontMGL->mem_alloc();

  /* Preallocate buffer */
  this->fontMGL->buf = (short*)malloc(sizeof(short));

  /* Keeping the original vector sorted... */
  this->mglMakeFontSort();

  /* Prepare and load font (Style: 0 - Regular; 1 - Bold; 2 - Italic; 3 - Bold-Italic */
  this->mglMakeFontPrepareFont(0);

  /* TODO: Workaround, copy loaded style as default for other styles */
  this->mglMakeFontMainCopy(0);

  /* Set font to current mglGraph */
  mgl->SetFont(this->fontMGL);
}

void mglMakeFont::mglMakeFontSearchGlyph(mglGraph *mgl, const char* str)
{
  unsigned s = strlen(str)+1;
  wchar_t *wstr = new wchar_t[s];

  mbstowcs(wstr, str, s);  /* Convert const char* to wide-character string */

  /* Verifying new glyphs and TeX symbols... */
  if(this->mglMakeFontSearchSymbols(wstr, s))
    this->mglMakeSetFontFT(mgl);  /* Updating mglFont w/ new glyphs */

  delete []wstr;
}

/* Loading FTFont and mglFont */
bool mglMakeFont::mglMakeLoadFontFT(mglGraph *mgl, const char *filename, int pt_size)
{
  unsigned char chr;
  FTVtkFont *fnt;

  if (!filename)
    return false;

  fnt = new FTVtkFont(filename);
  if (fnt->Error())
    return false;

  fnt->FaceSize(pt_size);

  this->fontFT = (FTFont*)fnt;
  this->fontMGL = new mglFont;
  
  this->fontMGL->Clear();
  fnt->TriangleFont.clear();

  /* Rendering all characters (0xFFFF) are too slow... */
  for(chr = ' '; chr < 0x7F; chr++)  /* TINY SET: 0x7F --- SMALL SET: 0xFF */
    this->mglMakeFontAddGlyph(chr);

  this->mglMakeSetFontFT(mgl);

  return true;
}

/* TODO:
Load all font files for Bold, Italic and BoldItalic styles.
*/
