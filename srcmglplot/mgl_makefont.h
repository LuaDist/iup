#ifndef _MGL_MAKEFONT_H 
#define _MGL_MAKEFONT_H

#include "mgl/mgl_font.h"

class mglMakeFont
{
private:
  void    *fontFT;
  mglFont *fontMGL;
  double   ww;  /* maximal width */

  void mglMakeFontSort();
  double mglMakeFontCalcFactorDX();
  void mglMakeFontMainCopy(int style);

  void mglMakeFontPrepareSolidFont(unsigned int k, double dx, unsigned long *cur, unsigned *post, short *numt, short *buf);
  void mglMakeFontPrepareWireFont(unsigned int k, double dx, unsigned long *cur, unsigned *ids, unsigned *posl, short *wdt, short *numl, short *buf);
  void mglMakeFontPrepareFont(int style);
  void mglMakeSetFontFT(mglGraph *mgl);

  void mglMakeFontAddGlyph(wchar_t chr);
  bool mglMakeFontSearchSymbols(const wchar_t *str, int size);

public:
  bool mglMakeLoadFontFT(mglGraph *mgl, const char *filename, int pt_size);
  void mglMakeFontSearchGlyph(mglGraph *mgl, const char* str);
};

#endif
