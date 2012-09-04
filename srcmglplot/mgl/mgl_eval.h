/***************************************************************************
 * mgl_eval.h is part of Math Graphic Library
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
//---------------------------------------------------------------------------
#ifdef _MGL_W_H_
#warning "MathGL wrapper was enabled. So disable original MathGL classes"
#else
#ifndef _MGL_EVAL_H_
#define _MGL_EVAL_H_
//---------------------------------------------------------------------------
#include <math.h>
#include "mgl/mgl_define.h"
/// types of errors
#define MGL_ERR_LOG		1
#define MGL_ERR_ARC		2
#define MGL_ERR_SQRT	3
/// size of \a var array
const int MGL_VS = 'z'-'a'+1;
//---------------------------------------------------------------------------
/// Class for evaluating formula specified by the string
class mglFormula					// объект для ввода и вычисления формул
{
public:
	/// Evaluates the formula for 'x','r'=\a x, 'y','n'=\a y, 'z','t'=\a z, 'u'=\a u
	mreal Calc(mreal x,mreal y=0,mreal z=0,mreal u=0) const;
	/// Evaluates the formula for 'x, y, z, u, v, w'
	mreal Calc(mreal x,mreal y,mreal z,mreal u,mreal v,mreal w) const;
	/// Evaluates the formula for variables \a var
	mreal Calc(const mreal var[MGL_VS]) const;
	/// Evaluates the derivates of the formula for variables \a var respect to variable \a diff
	mreal CalcD(const mreal var[MGL_VS], char diff) const;
	/// Return error code
	int GetError() const;
	/// Parse the formula \a str and create formula-tree
	mglFormula(const char *str);
	/// Clean up formula-tree
	virtual ~mglFormula();
protected:
	mreal CalcIn(const mreal *a1) const;
	mreal CalcDIn(int id, const mreal *a1) const;
	mglFormula *Left,*Right;	// first and second argument of the function
	int Kod;					// the function ID
	mreal Res;					// the number or the variable ID
	static int Error;
};
//---------------------------------------------------------------------------
char *mgl_strdup(const char *s);
//-----------------------------------------------------------------------------
#endif
#endif
