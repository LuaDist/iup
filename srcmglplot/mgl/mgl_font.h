/***************************************************************************
 * mgl_font.h is part of Math Graphic Library
 * Copyright (C) 2007 Alexey Balakin <balakin@appl.sci-nnov.ru>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//-----------------------------------------------------------------------------
#ifdef _MGL_W_H_
#warning "MathGL wrapper was enabled. So disable original MathGL classes"
#else
#ifndef _MGL_FONT_H_
#define _MGL_FONT_H_

#include <stdio.h>
#include <mgl/mgl_define.h>
//-----------------------------------------------------------------------------
#define MGL_FONT_BOLD		0x01000000	// This value is used binary
#define MGL_FONT_ITAL		0x02000000	// This value is used binary
#define MGL_FONT_BOLD_ITAL	0x03000000
#define MGL_FONT_WIRE		0x04000000
#define MGL_FONT_OLINE		0x08000000	// This value is used binary
#define MGL_FONT_ULINE		0x10000000
#define MGL_FONT_ZEROW		0x20000000	// internal codes
#define MGL_FONT_UPPER		0x40000000
#define MGL_FONT_LOWER		0x80000000
#define MGL_FONT_ROMAN		0xfcffffff
#define MGL_FONT_MASK		0x00ffffff
#define MGL_COLOR_MASK		0xffffff00
#define MGL_FONT_STYLE		0x3f000000
//-----------------------------------------------------------------------------
#ifdef WIN32	// a man ask to use built-in font under Windows
#define MGL_DEF_FONT_NAME	0
#else
#define MGL_DEF_FONT_NAME	"STIX"
#endif
//-----------------------------------------------------------------------------
struct mglTeXsymb	{	unsigned kod;	const wchar_t *tex;	};
class mglGraph;
//-----------------------------------------------------------------------------
/// Class for incapsulating font plotting procedures
class mglFont
{
public:
	mglGraph *gr;	///< mglGraph class used for drawing characters
	mglFont(const char *name=0, const char *path=0);
	virtual ~mglFont();
	bool parse;		///< Parse LaTeX symbols
  
  // Direct Loader for OTF and TTF fonts
  friend class mglMakeFont;

	/// Load font data to memory. Normally used by constructor.
	bool Load(const char *base, const char *path=0);
	/// Free memory
	void Clear();
	/// Get height of text
	mreal Height(int font);
	/// Get height of text
	mreal Height(const char *how);
	/// Print text string for font specified by string
	mreal Puts(const char *str,const char *how, char col);
	/// Get width of text string for font specified by string
	mreal Width(const char *str,const char *how);
	/// Print text string for font specified by integer constant
	mreal Puts(const char *str,int font=0,int align=0, char col='k');
	/// Get width of text string for font specified by integer constant
	mreal Width(const char *str,int font=0);

	/// Print text string for font specified by string
	mreal Puts(const wchar_t *str,const char *how, char col);
	/// Get width of text string for font specified by string
	mreal Width(const wchar_t *str,const char *how);
	/// Print text string for font specified by integer constant
	mreal Puts(const wchar_t *str,int font=0,int align=0, char col='k');
	/// Get width of text string for font specified by integer constant
	mreal Width(const wchar_t *str,int font=0);
	/// Return number of glyphs
	inline unsigned GetNumGlyph()	{	return numg;	};
	/// Copy data from other font
	void Copy(mglFont *);
	/// Restore default font
	void Restore();
	/// Return true if font is loaded
	inline bool Ready()	{	return numg!=0;	};
	/// Return some of pointers
	inline const short *GetTr(int s, long j)	{	return buf+tr[s][j];	};
	inline const short *GetLn(int s, long j)	{	return buf+ln[s][j];	};
	inline int GetNt(int s, long j)	{	return numt[s][j];	};
	inline int GetNl(int s, long j)	{	return numl[s][j];	};
	inline mreal GetFact(int s)		{	return fact[s];	};
protected:
	wchar_t *id;		///< Unicode ID for glyph
	unsigned *tr[4];	///< Shift of glyph description by triangles (for solid font)
	unsigned *ln[4];	///< Shift of glyph description by lines (for wire font)
	short *numt[4];		///< Number of triangles in glyph description (for solid font)
	short *numl[4];		///< Number of lines in glyph description (for wire font)
	short *width[4];	///< Width of glyph for wire font
	float fact[4];		///< Divider for width of glyph
	unsigned numg;		///< Number of glyphs
	short *buf;			///< Buffer for glyph descriptions
	long numb;			///< Buffer size

	/// Draw string recursively
	/* x,y - position, f - factor, style: 0x1 - italic, 0x2 - bold, 0x4 - overline, 0x8 - underline, 0x10 - empty (not draw) */
	mreal Puts(const unsigned *str, mreal x,mreal y,mreal f,int style,char col);
	/// Replace TeX symbols by its UTF code and add font styles
	void Convert(const wchar_t *str, unsigned *res);
	/// Parse LaTeX command
	unsigned Parse(const wchar_t *s);
	/// Get internal code for symbol
	long Internal(unsigned s);

	/// Convert string to internal kod, also recognize TeX command
	void Convert(const wchar_t *str,unsigned *kod,int font);
	/// Get symbol for character \a ch with given \a font style
	unsigned Symbol(char ch);
private:
	mreal get_ptr(long &i,unsigned *str, unsigned **b1, unsigned **b2,mreal &w1,mreal &w2, mreal f1, mreal f2, int st);
	bool read_data(const char *fname, float *ff, short *wdt, short *numl, unsigned *posl, short *numt, unsigned *post, unsigned &cur);
	void main_copy();
	bool read_main(const char *fname, unsigned &cur);
	void mem_alloc();
	bool read_def(unsigned &cur);
	void draw_ouline(mglGraph *gr, int st, mreal x, mreal y, mreal f, mreal g, mreal ww, char ccol);
};
//-----------------------------------------------------------------------------
#endif
#endif
//-----------------------------------------------------------------------------
