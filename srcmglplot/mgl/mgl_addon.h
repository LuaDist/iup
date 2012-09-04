/***************************************************************************
 * mgl_addon.h is part of Math Graphic Library
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
#ifndef _MGL_ADDON_H_
#define _MGL_ADDON_H_
//---------------------------------------------------------------------------
#include <stdio.h>
#include <mgl/mgl_evalc.h>
//---------------------------------------------------------------------------
/// Integer power of the \a x
double mgl_ipow(double x,int n);
/// Integer power of the \a x
dual mgl_ipowc(dual x,int n);
void mgl_srnd(long seed);
double mgl_rnd();
double mgl_gauss_rnd();
dual expi(dual a);
dual expi(double a);
void mgl_fft_freq(double *freq,unsigned nn);
//---------------------------------------------------------------------------
/// Duplicate string (returned pointer must be free() after usage)
char *mgl_strdup(const char *s);
wchar_t *mgl_wcsdup(const wchar_t *s);
/// Remove spaces at begining and at the end of the string
void mgl_strtrim(char *str);
void mgl_wcstrim(wchar_t *str);
/// Change register to lowercase (only for ANSI symbols)
void mgl_strlwr(char *str);
/// Remove double spaces from the string
void mgl_strcls(char *str);
/// Get position of substring or return -1 if not found
int mgl_strpos(const char *str,char *fnd);
/// Get position of symbol or return -1 if not found
int mgl_strpos(const char *str,char fnd);
//---------------------------------------------------------------------------
/// Get uncommented string from file (NOTE: it is not thread safe!!!)
char *mgl_fgetstr(FILE *fp);
/// Check if symbol denote true
bool mgl_istrue(char ch);
/// Print test message
void mgl_test(const char *str, ...);
/// Print info message
void mgl_info(const char *str, ...);
/// Locate next data block (block started by -----)
FILE *mgl_next_data(const char *fname,int p);
//---------------------------------------------------------------------------
/// Explicit scheme for 1 step of axial diffraction
bool mglDifrAxial(dual *a, int n, dual q, int Border,dual *b, dual *d, int kk, double di);
/// Explicit scheme for 1 step of plane diffraction
bool mglDifrGrid(dual *a,int n,dual q,int Border,dual *b,dual *d,int kk);
//---------------------------------------------------------------------------
#endif
#endif
