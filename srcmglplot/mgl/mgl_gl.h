/***************************************************************************
 * mgl_gl.h is part of Math Graphic Library
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
#ifndef _MGL_GL_H_
#define _MGL_GL_H_
#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "mgl/mgl_ab.h"
//-----------------------------------------------------------------------------
/// Class implements the creation of different mathematical plots under OpenGL
class mglGraphGL : public mglGraphAB
{
public:
	mglGraphGL();
	virtual ~mglGraphGL();
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void Flush() {glFlush();};
	void Finish(){glFinish();};
	bool Alpha(bool enable);
	bool Light(bool enable);
	void Light(int n, bool enable);
	void Fog(mreal d, mreal dz=0.25);
	void Light(int n,mglPoint p, mglColor c=NC, mreal br=0.5, bool infty=true);
	void View(mreal tetX,mreal tetY,mreal tetZ);
	void Clf(mglColor Back=NC);
	int NewFrame();
	void EndFrame();
	void InPlot(mreal x1,mreal x2,mreal y1,mreal y2,bool rel=false);
	void SetSize(int ,int ){};

	void Ball(mreal x,mreal y,mreal z,mglColor col=RC,mreal alpha=1);
	void Glyph(mreal x, mreal y, mreal f, int style, long icode, char col);
//	void Glyph(mreal x,mreal y, mreal f, int nt, const short *trig, int nl, const short *line);
protected:
	mglColor def_col;

//	void DefColor(mglColor c, mreal alpha=-1);
	void Pen(mglColor col, char style,mreal width);
	unsigned char **GetRGBLines(long &w, long &h, unsigned char *&f, bool solid=true);

	void ball(mreal *p,mreal *c);

	void LightScale();
	void line_plot(mreal *p1,mreal *p2,mreal *c1,mreal *c2,bool all=false);
	void line_plot_s(mreal *p1,mreal *p2,mreal *c1,mreal *c2,bool all=false);
	void trig_plot(mreal *p0,mreal *p1,mreal *p2,
					mreal *c0,mreal *c1,mreal *c2);
	void trig_plot_n(mreal *p0,mreal *p1,mreal *p2,
					mreal *c0,mreal *c1,mreal *c2,
					mreal *n0,mreal *n1,mreal *n2);
	void quad_plot(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
					mreal *c0,mreal *c1,mreal *c2,mreal *c3);
	void quad_plot_a(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
					mreal a0,mreal a1,mreal a2,mreal a3,mreal alpha);
	void quad_plot_n(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
					mreal *c0,mreal *c1,mreal *c2,mreal *c3,
					mreal *n0,mreal *n1,mreal *n2,mreal *n3);
	void mark_plot(mreal *pp, char type);
	void font_line(mreal *p, unsigned char *r,bool thin=true);
};
//-----------------------------------------------------------------------------
#endif
#endif
//-----------------------------------------------------------------------------
