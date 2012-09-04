/***************************************************************************
 * mgl_evalc.cpp is part of Math Graphic Library
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
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "mgl/mgl_evalc.h"
#include "mgl/mgl_addon.h"
#include "mgl/mgl_define.h"
#ifndef NO_GSL
#include <gsl/gsl_sf.h>
#endif
//-----------------------------------------------------------------------------
//	константы для распознования выражения
enum{
EQ_NUM=0,	// a variable substitution
EQ_RND,		// random number
EQ_A,		// numeric constant
// двуместные функции
EQ_ADD,		// addition x+y
EQ_SUB,		// substraction x-y
EQ_MUL,		// multiplication x*y
EQ_DIV,		// division x/y
EQ_IPOW,	// power x^n for integer n
EQ_POW,		// power x^y
EQ_LOG,		// logarithm of x on base a, log_a(x) = ln(x)/ln(a)
// одноместные функции
EQ_SIN,		// sine function \sin(x).			!!! MUST BE FIRST 1-PLACE FUNCTION
EQ_COS,		// cosine function \cos(x).
EQ_TAN,		// tangent function \tan(x).
EQ_ASIN,	// inverse sine function \asin(x).
EQ_ACOS,	// inverse cosine function \acos(x).
EQ_ATAN,	// inverse tangent function \atan(x).
EQ_SINH,	// hyperbolic sine function \sinh(x).
EQ_COSH,	// hyperbolic cosine function \cosh(x).
EQ_TANH,	// hyperbolic tangent function \tanh(x).
EQ_ASINH,	// inverse hyperbolic sine function \asinh(x).
EQ_ACOSH,	// inverse hyperbolic cosine function \acosh(x).
EQ_ATANH,	// inverse hyperbolic tangent function \atanh(x).
EQ_SQRT,	// square root function \sqrt(x)
EQ_EXP,		// exponential function \exp(x)
EQ_EXPI,	// exponential function \exp(i*x)
EQ_LN,		// logarithm of x, ln(x)
EQ_LG,		// decimal logarithm of x, lg(x) = ln(x)/ln(10)
EQ_ABS		// absolute value
};
//-----------------------------------------------------------------------------
int mglFormulaC::Error=0;
bool mglCheck(char *str,int n);
int mglFindInText(char *str,const char *lst);
//-----------------------------------------------------------------------------
// деструктор формулы
mglFormulaC::~mglFormulaC()
{
	if(Left) delete Left;
	if(Right) delete Right;
}
//-----------------------------------------------------------------------------
// конструктор формулы (автоматически распознает и "компилирует" формулу)
mglFormulaC::mglFormulaC(const char *string)
{
	Error=0;
	Left=Right=0;
	Res=0; Kod=0;
	if(!string)	{	Kod = EQ_NUM;	Res = 0;	return;	}
//printf("%s\n",string);	fflush(stdout);
	char *str = new char[strlen(string)+1];
	strcpy(str,string);
	static char Buf[2048];
	long n,len;
	mgl_strtrim(str);
	mgl_strlwr(str);
	len=strlen(str);
	if(str[0]==0) {	delete []str;	return;	}
	if(str[0]=='(' && mglCheck(&(str[1]),len-2))	// если все выражение в скобах, то убираем  их
	{
		strcpy(Buf,str+1);
		len-=2;	Buf[len]=0;
		strcpy(str,Buf);
	}
	len=strlen(str);
	n=mglFindInText(str,"+-");				// меньший приоритет - сложение, вычитание
	if(n>=0)
	{
		if(str[n]=='+') Kod=EQ_ADD; else Kod=EQ_SUB;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormulaC(Buf);
		Right=new mglFormulaC(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"*/");				// средний приоритет - умножение, деление
	if(n>=0)
	{
		if(str[n]=='*') Kod=EQ_MUL; else Kod=EQ_DIV;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormulaC(Buf);
		Right=new mglFormulaC(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"^");				// высокий приоритет - возведение в степень
	if(n>=0)
	{
		Kod=EQ_IPOW;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormulaC(Buf);
		Right=new mglFormulaC(Buf+n+1);
		delete []str;
		return;
	}

	for(n=0;n<len;n++)	if(str[n]=='(')	break;
	if(n>=len)							// это число или переменная
	{
		Kod = EQ_NUM;
//		Left = Right = 0;
		if(str[1]==0 && str[0]>='a' && str[0]<='z')	// доступные перемнные
		{	Kod=EQ_A;	Res = str[0]-'a';	}
		else if(!strcmp(str,"rnd")) Kod=EQ_RND;
		else if(!strcmp(str,"pi")) Res=M_PI;
		else if(str[0]=='i')	Res = dual(0,atof(str+1));
		else Res=atof(str);				// это число
	}
	else
	{
		char name[128];
		strcpy(name,str);
//		strcpy(Buf,str);
		name[n]=0;
//		len-=n;
		memcpy(Buf,&(str[n+1]),len-n);
		len=strlen(Buf);
		Buf[--len]=0;
		if(!strcmp(name,"sin")) Kod=EQ_SIN;
		else if(!strcmp(name,"cos")) Kod=EQ_COS;
		else if(!strcmp(name,"tg")) Kod=EQ_TAN;
		else if(!strcmp(name,"tan")) Kod=EQ_TAN;
		else if(!strcmp(name,"asin")) Kod=EQ_ASIN;
		else if(!strcmp(name,"acos")) Kod=EQ_ACOS;
		else if(!strcmp(name,"atan")) Kod=EQ_ATAN;
		else if(!strcmp(name,"sinh")) Kod=EQ_SINH;
		else if(!strcmp(name,"cosh")) Kod=EQ_COSH;
		else if(!strcmp(name,"tanh")) Kod=EQ_TANH;
		else if(!strcmp(name,"sh")) Kod=EQ_SINH;
		else if(!strcmp(name,"ch")) Kod=EQ_COSH;
		else if(!strcmp(name,"th")) Kod=EQ_TANH;
		else if(!strcmp(name,"sqrt")) Kod=EQ_SQRT;
		else if(!strcmp(name,"log")) Kod=EQ_LOG;
		else if(!strcmp(name,"pow")) Kod=EQ_POW;
		else if(!strcmp(name,"exp")) Kod=EQ_EXP;
		else if(!strcmp(name,"lg")) Kod=EQ_LG;
		else if(!strcmp(name,"ln")) Kod=EQ_LN;
		else if(!strcmp(name,"abs")) Kod=EQ_ABS;
		else {	delete []str;	return;	}	// unknown function
		n=mglFindInText(Buf,",");
		if(n>=0)
		{
			Buf[n]=0;
			Left=new mglFormulaC(Buf);
			Right=new mglFormulaC(&(Buf[n+1]));
		}
		else
			Left=new mglFormulaC(Buf);
	}
	delete []str;
}
//-----------------------------------------------------------------------------
// evaluate formula for 'x'='r', 'y'='n'='v', 't'='z', 'u'='a' variables
dual mglFormulaC::Calc(dual x,dual y,dual t,dual u) const
{
	Error=0;
	dual a1[MGL_VS];	memset(a1,0,MGL_VS*sizeof(dual));
	a1['a'-'a'] = a1['u'-'a'] = u;
	a1['x'-'a'] = a1['r'-'a'] = x;
	a1['y'-'a'] = a1['n'-'a'] = a1['v'-'a'] = y;
	a1['z'-'a'] = a1['t'-'a'] = t;
	return CalcIn(a1);
}
//-----------------------------------------------------------------------------
// evaluate formula for 'x'='r', 'y'='n', 't'='z', 'u'='a', 'v'='b', 'w'='c' variables
dual mglFormulaC::Calc(dual x,dual y,dual t,dual u,dual v,dual w) const
{
	Error=0;
	dual a1[MGL_VS];	memset(a1,0,MGL_VS*sizeof(dual));
	a1['c'-'a'] = a1['w'-'a'] = w;
	a1['b'-'a'] = a1['v'-'a'] = v;
	a1['a'-'a'] = a1['u'-'a'] = u;
	a1['x'-'a'] = a1['r'-'a'] = x;
	a1['y'-'a'] = a1['n'-'a'] = y;
	a1['z'-'a'] = a1['t'-'a'] = t;
	return CalcIn(a1);
}
//-----------------------------------------------------------------------------
// evaluate formula for arbitrary set of variables
dual mglFormulaC::Calc(const dual var[MGL_VS]) const
{
	Error=0;
	return CalcIn(var);
}
//-----------------------------------------------------------------------------
dual addc(dual a,dual b)	{return a+b;}
dual subc(dual a,dual b)	{return a-b;}
dual mulc(dual a,dual b)	{return a*b;}
dual divc(dual a,dual b)	{return a/b;}
dual ipwc(dual a,dual b)	{return mgl_ipowc(a,int(b.real()));}
dual powc(dual a,dual b)	{return exp(b*log(a));	}
dual llgc(dual a,dual b)	{return log(a)/log(b);	}
dual expi(dual a)	{	return exp(dual(0,1)*a);	}
dual expi(double a)	{	return dual(cos(a),sin(a));	}
//-----------------------------------------------------------------------------
dual ic = dual(0,1);
dual asinhc(dual x)	{	return log(x+sqrt(x*x+1.));	}
dual acoshc(dual x)	{	return log(x+sqrt(x*x-1.));	}
dual atanhc(dual x)	{	return log((1.+x)/(1.-x))/2.;	}
dual sinc(dual x)	{	return sin(x);	}
dual cosc(dual x)	{	return cos(x);	}
dual tanc(dual x)	{	return tan(x);	}
dual sinhc(dual x)	{	return sinh(x);	}
dual coshc(dual x)	{	return cosh(x);	}
dual tanhc(dual x)	{	return tanh(x);	}
dual asinc(dual x)	{	return log(ic*x+sqrt(1.-x*x))/ic;	}
dual acosc(dual x)	{	return log(x+sqrt(x*x-1.))/ic;	}
dual atanc(dual x)	{	return log((ic-x)/(ic+x))/(2.*ic);	}
dual expc(dual x)	{	return exp(x);	}
dual sqrtc(dual x)	{	return sqrt(x);	}
dual logc(dual x)	{	return log(x);	}
dual absc(dual x)	{	return abs(x);	}
dual lgc(dual x)	{	return log10(x);}
//-----------------------------------------------------------------------------
typedef dual (*func_1)(dual);
typedef dual (*func_2)(dual, dual);
// evaluation of embedded (included) expressions
dual mglFormulaC::CalcIn(const dual *a1) const
{
	func_2 f2[7] = {addc,subc,mulc,divc,ipwc,powc,llgc};
	func_1 f1[18] = {sinc,cosc,tanc,asinc,acosc,atanc,sinhc,coshc,tanhc,
					asinhc,acoshc,atanhc,sqrtc,expc,expi,logc,lgc,absc};
//	if(Error)	return 0;
	if(Kod==EQ_A)	return a1[(int)Res.real()];
	if(Kod==EQ_RND)	return mgl_rnd();
	if(Kod==EQ_NUM) return Res;

	dual a = Left->CalcIn(a1);
	if(isnan(a.real()) || isnan(a.imag()))	return NAN;

	if(Kod<EQ_SIN)
	{
		dual b = Right->CalcIn(a1);
		if(isnan(b.real()) || isnan(b.imag()))	return NAN;
		return f2[Kod-EQ_ADD](a,b);
	}
	else	return f1[Kod-EQ_SIN](a);
	return Res;
}
//-----------------------------------------------------------------------------
dual mgl_ipowc(dual x,int n)
{
	dual t;
	if(n==2)	return x*x;
	if(n==1)	return x;
	if(n<0)		return 1./mgl_ipowc(x,-n);
	if(n==0)	return 1.;
	t = mgl_ipowc(x,n/2);	t = t*t;
	if(n%2==1)	t *= x;
	return t;
}
//-----------------------------------------------------------------------------
