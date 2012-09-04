/***************************************************************************
 * mgl_zb.h is part of Math Graphic Library
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
#ifndef _MGL_ZB_H_
#define _MGL_ZB_H_
#include "mgl/mgl_ab.h"
//-----------------------------------------------------------------------------
/// Class implement the creation of different mathematical plots using Z-Buffer
class mglGraphZB : public mglGraphAB
{
friend void *mgl_thr_ball(void *p);
friend void *mgl_thr_dfin(void *p);
friend void *mgl_thr_ffin(void *p);
friend void *mgl_thr_quad(void *p);
friend void *mgl_thr_quaa(void *p);
friend void *mgl_thr_trig(void *p);
friend void *mgl_thr_trin(void *p);

public:
	/// Initialize ZBuffer drawing and allocate the memory for image with size [Width x Height].
	mglGraphZB(int w=600, int h=400);
	virtual ~mglGraphZB();
	/// write n-th slice (for testing)
	void WriteSlice(int n);
	void Finish();
	virtual void Clf(mglColor Back=NC);
	virtual void SetSize(int w,int h);
	void Glyph(mreal x, mreal y, mreal f, int style, long icode, char col);
	void PutDrawReg(int m, int n, int k, mglGraphAB *gr);
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bool FastNoFace;	/// Use fastest (but less accurate) scheme for drawing (without faces)
protected:
	mreal *Z;			///< Height for given level in Z-direction
	unsigned char *C;	///< Picture for given level in Z-direction

	/// Plot point \a p with color \a c
	void pnt_plot(long x,long y,mreal z,unsigned char c[4]);
};
//-----------------------------------------------------------------------------
#endif
#endif
//-----------------------------------------------------------------------------
