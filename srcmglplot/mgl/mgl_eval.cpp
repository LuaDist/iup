/***************************************************************************
 * mgl_eval.cpp is part of Math Graphic Library
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
#ifndef NO_GSL
#include <gsl/gsl_sf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_errno.h>
#endif
#include "mgl/mgl_eval.h"
//-----------------------------------------------------------------------------
//	константы для распознования выражения
enum{
EQ_NUM=0,	// a variable substitution
EQ_RND,		// random number
EQ_A,		// numeric constant
// normal functions of 2 arguments
EQ_LT,		// comparison x<y			!!! MUST BE FIRST 2-PLACE FUNCTION
EQ_GT,		// comparison x>y
EQ_EQ,		// comparison x=y
EQ_OR,		// comparison x|y
EQ_AND,		// comparison x&y
EQ_ADD,		// addition x+y
EQ_SUB,		// substraction x-y
EQ_MUL,		// multiplication x*y
EQ_DIV,		// division x/y
EQ_IPOW,	// power x^n for integer n
EQ_POW,		// power x^y
EQ_MOD,		// x modulo y
EQ_LOG,		// logarithm of x on base a, log_a(x) = ln(x)/ln(a)
EQ_ARG,		// argument of complex number arg(x,y) = atan2(x,y)
// special functions of 2 arguments
EQ_BESJ,	// regular cylindrical Bessel function of fractional order
EQ_BESY,	// irregular cylindrical Bessel function of fractional order
EQ_BESI,	// regular modified Bessel function of fractional order
EQ_BESK,	// irregular modified Bessel function of fractional order
EQ_ELE,		// elliptic integral E(\phi,k) = \int_0^\phi dt   \sqrt((1 - k^2 \sin^2(t)))
EQ_ELF,		// elliptic integral F(\phi,k) = \int_0^\phi dt 1/\sqrt((1 - k^2 \sin^2(t)))
EQ_LP,		// Legendre polynomial P_l(x), (|x|<=1, l>=0)
EQ_BETA,	// beta function B(x,y) = Gamma(x)*Gamma(y)/Gamma(x+y)
// normal functions of 1 argument
EQ_SIN,		// sine function \sin(x).			!!! MUST BE FIRST 1-PLACE FUNCTION
EQ_COS,		// cosine function \cos(x).
EQ_TAN,		// tangent function \tan(x).
EQ_ASIN,	// inverse sine function \sin(x).
EQ_ACOS,	// inverse cosine function \sin(x).
EQ_ATAN,	// inverse tangent function \tan(x).
EQ_SINH,	// hyperbolic sine function \sin(x).
EQ_COSH,	// hyperbolic cosine function \sin(x).
EQ_TANH,	// hyperbolic tangent function \tan(x).
EQ_ASINH,	// inverse hyperbolic sine function \sin(x).
EQ_ACOSH,	// inverse hyperbolic cosine function \sin(x).
EQ_ATANH,	// inverse hyperbolic tangent function \tan(x).
EQ_SQRT,	// square root function \sqrt(x)
EQ_EXP,		// exponential function \exp(x)
EQ_LN,		// logarithm of x, ln(x)
EQ_LG,		// decimal logarithm of x, lg(x) = ln(x)/ln(10)
EQ_SIGN,	// sign of number
EQ_STEP,	// step function
EQ_INT,		// integer part [x]
EQ_ABS,		// absolute value of x
// special functions of 1 argument
EQ_LI2,		// dilogarithm for a real argument Li2(x) = - \Re \int_0^x ds \log(1-s)/s.
EQ_ELLE,	// complete elliptic integral is denoted by E(k) = E(\pi/2, k).
EQ_ELLK,	// complete elliptic integral is denoted by K(k) = F(\pi/2, k).
EQ_AI,		// Airy function Ai(x)
EQ_BI,		// Airy function Bi(x)
EQ_ERF,		// error function erf(x) = (2/\sqrt(\pi)) \int_0^x dt \exp(-t^2).
EQ_EI3,		// exponential integral Ei_3(x) = \int_0^x dt \exp(-t^3) for x >= 0.
EQ_EI,		// exponential integral Ei(x),  Ei(x) := - PV(\int_{-x}^\infty dt \exp(-t)/t), where PV denotes the principal value of the integral.
EQ_E1,		// exponential integral E_1(x), E_1(x) := Re \int_1^\infty dt \exp(-xt)/t.
EQ_E2,		// exponential integral E_2(x), E_2(x) := Re \int_1^\infty dt \exp(-xt)/t^2.
EQ_SI,		// Sine integral Si(x) = \int_0^x dt \sin(t)/t.
EQ_CI,		// Cosine integral Ci(x) = \int_0^x dt \cos(t)/t.
EQ_GAMMA,	// Gamma function \Gamma(x) = \int_0^\infty dt  t^{x-1} \exp(-t)
EQ_PSI,		// digamma function \psi(x) = \Gamma'(x)/\Gamma(x) for general x, x \ne 0.
EQ_W0,		// principal branch of the Lambert W function, W_0(x). Functions W(x), are defined to be solutions of the equation W\exp(W) = x.
EQ_W1,		// secondary mreal-valued branch of the Lambert W function, W_{-1}(x). Functions W(x), are defined to be solutions of the equation W\exp(W) = x.
EQ_SINC,		// compute \sinc(x) = \sin(\pi x) / (\pi x) for any value of x.
EQ_ZETA,		// Riemann zeta function \zeta(s) = \sum_{k=1}^\infty k^{-s}for arbitrary s, s \ne 1.
EQ_ETA,		// eta function \eta(s) = (1-2^{1-s}) \zeta(s) for arbitrary s.
EQ_AID,		// Derivative of Airy function Ai(x)
EQ_BID,		// Derivative of Airy function Bi(x)
EQ_Z,		// Dawson function \exp(-x^2) \int_0^x dt \exp(t^2)
// Jacoby functions of 2 arguments
EQ_SN,		// Jacobian elliptic function sn(u|m)	// !!! MUST BE FIRST NON 1-PLACE FUNCTION
EQ_SC,		// Jacobian elliptic function sn(u|m)/cn(u|m)
EQ_SD,		// Jacobian elliptic function sn(u|m)/dn(u|m)
EQ_NS,		// Jacobian elliptic function 1/sn(u|m)
EQ_NC,		// Jacobian elliptic function 1/cn(u|m)
EQ_ND,		// Jacobian elliptic function 1/dn(u|m)
EQ_CN,		// Jacobian elliptic function cn(u|m)
EQ_CS,		// Jacobian elliptic function cn(u|m)/sn(u|m)
EQ_CD,		// Jacobian elliptic function cn(u|m)/dn(u|m)
EQ_DN,		// Jacobian elliptic function dn(u|m)
EQ_DS,		// Jacobian elliptic function dn(u|m)/sn(u|m)
EQ_DC,		// Jacobian elliptic function dn(u|m)/cn(u|m)
			// MUST BE LAST ELLIPTIC FUNCTION
// non-ready
EQ_EN,
EQ_CL,		// Clausen function
};
//-----------------------------------------------------------------------------
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
//-----------------------------------------------------------------------------
#ifndef NO_GSL
gsl_rng *mgl_rng=0;
#endif
void mgl_srnd(long seed)
{
#ifndef NO_GSL
	if(mgl_rng==0)
	{
		gsl_rng_env_setup();
		mgl_rng = gsl_rng_alloc(gsl_rng_default);
	}
	gsl_rng_set(mgl_rng, seed);
#else
	srand(seed);
#endif
}
double mgl_rnd()
{
#ifndef NO_GSL
	if(mgl_rng==0)
	{
		gsl_rng_env_setup();
		mgl_rng = gsl_rng_alloc(gsl_rng_default);
		gsl_rng_set(mgl_rng, time(0));
	}
	return gsl_rng_uniform(mgl_rng);
//	gsl_rng_free(r);
#else
	return rand()/(RAND_MAX-1.);
#endif
}
//-----------------------------------------------------------------------------
double mgl_ipow(double x,int n);
int mglFormula::Error=0;
bool mglCheck(char *str,int n);
int mglFindInText(char *str,const char *lst);
//-----------------------------------------------------------------------------
void mgl_strtrim(char *str)
{
	char *c = mgl_strdup(str);
	unsigned long n=strlen(str);
	long k;
	for(k=0;k<long(strlen(str));k++)	// удаляем начальные пробелы
		if(str[k]>' ')	break;
	strcpy(c,&(str[k]));
	n = strlen(c);
	for(k=n-1;k>=0;k--)	// удаляем начальные пробелы
		if(c[k]>' ')		break;
	c[k+1] = 0;
	strcpy(str,c);	free(c);
}
//-----------------------------------------------------------------------------
void mgl_strlwr(char *str)
{
	for(long k=0;k<(long)strlen(str);k++)	// удаляем начальные пробелы
		str[k] = (str[k]>='A' && str[k]<='Z') ? str[k]+'a'-'A' : str[k];
}
//-----------------------------------------------------------------------------
// деструктор формулы
mglFormula::~mglFormula()
{
	if(Left) delete Left;
	if(Right) delete Right;
}
//-----------------------------------------------------------------------------
// конструктор формулы (автоматически распознает и "компилирует" формулу)
mglFormula::mglFormula(const char *string)
{
#ifndef NO_GSL
	gsl_set_error_handler_off();
#endif
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
	n=mglFindInText(str,"&|");				// меньший приоритет - сложение, вычитание
	if(n>=0)
	{
		if(str[n]=='|') Kod=EQ_OR;	else Kod=EQ_AND;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"<>=");				// меньший приоритет - сложение, вычитание
	if(n>=0)
	{
		if(str[n]=='<') Kod=EQ_LT;
		else if(str[n]=='>') Kod=EQ_GT;
		else Kod=EQ_EQ;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"+-");				// меньший приоритет - сложение, вычитание
	if(n>=0)
	{
		if(str[n]=='+') Kod=EQ_ADD; else Kod=EQ_SUB;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"*/");				// средний приоритет - умножение, деление
	if(n>=0)
	{
		if(str[n]=='*') Kod=EQ_MUL; else Kod=EQ_DIV;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"^");				// высокий приоритет - возведение в степень
	if(n>=0)
	{
		Kod=EQ_IPOW;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
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
		if(!strncmp(name,"jacobi_",7))
			memmove(name,name+7,(strlen(name+7)+1)*sizeof(char));
		if(name[0]=='a')
		{
			if(!strcmp(name+1,"sin"))		Kod=EQ_ASIN;
			else if(!strcmp(name+1,"cos"))	Kod=EQ_ACOS;
			else if(!strcmp(name+1,"tan"))	Kod=EQ_ATAN;
			else if(!strcmp(name+1,"sinh"))	Kod=EQ_ASINH;
			else if(!strcmp(name+1,"cosh"))	Kod=EQ_ACOSH;
			else if(!strcmp(name+1,"tanh"))	Kod=EQ_ATANH;
			else if(!strcmp(name+1,"rg"))	Kod=EQ_ARG;
			else if(!strcmp(name+1,"bs"))	Kod=EQ_ABS;
			else if(!strcmp(name+1,"i"))	Kod=EQ_AI;
			else if(!strcmp(name+1,"iry_ai"))	Kod=EQ_AI;
			else if(!strcmp(name+1,"iry_bi"))	Kod=EQ_BI;
			else if(!strcmp(name+1,"iry_dai"))	Kod=EQ_AID;
			else if(!strcmp(name+1,"iry_dbi"))	Kod=EQ_BID;
		}
		else if(name[0]=='b')
		{
			if(!strcmp(name+1,"i"))		Kod=EQ_BI;
			else if(!strcmp(name+1,"essel_j"))	Kod=EQ_BESJ;
			else if(!strcmp(name+1,"essel_i"))	Kod=EQ_BESI;
			else if(!strcmp(name+1,"essel_k"))	Kod=EQ_BESK;
			else if(!strcmp(name+1,"essel_y"))	Kod=EQ_BESY;
			else if(!strcmp(name+1,"eta"))	Kod=EQ_BETA;
		}
		else if(name[0]=='c')
		{
			if(!strcmp(name+1,"os"))		Kod=EQ_COS;
			else if(!strcmp(name+1,"osh"))	Kod=EQ_COSH;
			else if(!strcmp(name+1,"h"))	Kod=EQ_COSH;
			else if(!strcmp(name+1,"i"))	Kod=EQ_CI;
			else if(!strcmp(name+1,"n"))	Kod=EQ_CN;
			else if(!strcmp(name+1,"s"))	Kod=EQ_CS;
			else if(!strcmp(name+1,"d"))	Kod=EQ_CD;
			else if(!strcmp(name+1,"l"))	Kod=EQ_CL;
		}
		else if(name[0]=='d')
		{
			if(!strcmp(name+1,"n"))			Kod=EQ_DN;
			else if(!strcmp(name+1,"s"))	Kod=EQ_DS;
			else if(!strcmp(name+1,"c"))	Kod=EQ_DC;
			else if(!strcmp(name+1,"ilog"))	Kod=EQ_LI2;
		}
		else if(name[0]=='e')
		{
			if(!strcmp(name+1,"xp"))		Kod=EQ_EXP;
			else if(!strcmp(name+1,"rf"))	Kod=EQ_ERF;
			else if(!strcmp(name+1,"n"))	Kod=EQ_EN;
			else if(!strcmp(name+1,"e"))	Kod=EQ_ELLE;
			else if(!strcmp(name+1,"k"))	Kod=EQ_ELLK;
			else if(name[0]==0)				Kod=EQ_ELE;
			else if(!strcmp(name+1,"i"))	Kod=EQ_EI;
			else if(!strcmp(name+1,"1"))	Kod=EQ_E1;
			else if(!strcmp(name+1,"2"))	Kod=EQ_E2;
			else if(!strcmp(name+1,"ta"))	Kod=EQ_ETA;
			else if(!strcmp(name+1,"i3"))	Kod=EQ_EI3;
			else if(!strcmp(name+1,"lliptic_e"))	Kod=EQ_ELE;
			else if(!strcmp(name+1,"lliptic_f"))	Kod=EQ_ELF;
			else if(!strcmp(name+1,"lliptic_ec"))	Kod=EQ_ELLE;
			else if(!strcmp(name+1,"lliptic_kc"))	Kod=EQ_ELLK;
		}
		else if(name[0]=='l')
		{
			if(!strcmp(name+1,"og"))		Kod=EQ_LOG;
			else if(!strcmp(name+1,"g"))	Kod=EQ_LG;
			else if(!strcmp(name+1,"n"))	Kod=EQ_LN;
			else if(!strcmp(name+1,"i2"))	Kod=EQ_LI2;
			else if(!strcmp(name+1,"egendre"))	Kod=EQ_LP;
		}
		else if(name[0]=='s')
		{
			if(!strcmp(name+1,"qrt"))		Kod=EQ_SQRT;
			else if(!strcmp(name+1,"in"))	Kod=EQ_SIN;
			else if(!strcmp(name+1,"tep"))	Kod=EQ_STEP;
			else if(!strcmp(name+1,"ign"))	Kod=EQ_SIGN;
			else if(!strcmp(name+1,"inh"))	Kod=EQ_SINH;
			else if(!strcmp(name+1,"h"))	Kod=EQ_SINH;
			else if(!strcmp(name+1,"i"))	Kod=EQ_SI;
			else if(!strcmp(name+1,"n"))	Kod=EQ_SN;
			else if(!strcmp(name+1,"c"))	Kod=EQ_SC;
			else if(!strcmp(name+1,"d"))	Kod=EQ_SD;
			else if(!strcmp(name+1,"inc"))	Kod=EQ_SINC;
		}
		else if(name[0]=='t')
		{
			if(!strcmp(name+1,"g"))			Kod=EQ_TAN;
			else if(!strcmp(name+1,"an"))	Kod=EQ_TAN;
			else if(!strcmp(name+1,"anh"))	Kod=EQ_TANH;
			else if(!strcmp(name+1,"h"))	Kod=EQ_TANH;
		}
		else if(!strcmp(name,"pow"))	Kod=EQ_POW;
		else if(!strcmp(name,"mod"))	Kod=EQ_MOD;
		else if(!strcmp(name,"i"))		Kod=EQ_BESI;
		else if(!strcmp(name,"int"))	Kod=EQ_INT;
		else if(!strcmp(name,"j"))		Kod=EQ_BESJ;
		else if(!strcmp(name,"k"))		Kod=EQ_BESK;
		else if(!strcmp(name,"y"))		Kod=EQ_BESY;
		else if(!strcmp(name,"f"))		Kod=EQ_ELF;
		else if(!strcmp(name,"gamma"))	Kod=EQ_GAMMA;
		else if(!strcmp(name,"ns"))		Kod=EQ_NS;
		else if(!strcmp(name,"nc"))		Kod=EQ_NC;
		else if(!strcmp(name,"nd"))		Kod=EQ_ND;
		else if(!strcmp(name,"w0"))		Kod=EQ_W0;
		else if(!strcmp(name,"w1"))		Kod=EQ_W1;
		else if(!strcmp(name,"psi"))	Kod=EQ_PSI;
		else if(!strcmp(name,"zeta"))	Kod=EQ_ZETA;
		else if(!strcmp(name,"z"))		Kod=EQ_Z;
		else {	delete []str;	return;	}	// unknown function
		n=mglFindInText(Buf,",");
		if(n>=0)
		{
			Buf[n]=0;
			Left=new mglFormula(Buf);
			Right=new mglFormula(&(Buf[n+1]));
		}
		else	Left=new mglFormula(Buf);
	}
	delete []str;
}
//-----------------------------------------------------------------------------
// evaluate formula for 'x'='r', 'y'='n'='v', 't'='z', 'u'='a' variables
mreal mglFormula::Calc(mreal x,mreal y,mreal t,mreal u) const
{
	Error=0;
	mreal a1[MGL_VS];	memset(a1,0,MGL_VS*sizeof(mreal));
	a1['a'-'a'] = a1['c'-'a'] = a1['u'-'a'] = u;
	a1['x'-'a'] = a1['r'-'a'] = x;
	a1['y'-'a'] = a1['n'-'a'] = a1['v'-'a'] = y;
	a1['z'-'a'] = a1['t'-'a'] = t;
	mreal a = CalcIn(a1);
	return isfinite(a) ? a : NAN;
}
//-----------------------------------------------------------------------------
// evaluate formula for 'x'='r', 'y'='n', 't'='z', 'u'='a', 'v'='b', 'w'='c' variables
mreal mglFormula::Calc(mreal x,mreal y,mreal t,mreal u,mreal v,mreal w) const
{
	Error=0;
	mreal a1[MGL_VS];	memset(a1,0,MGL_VS*sizeof(mreal));
	a1['c'-'a'] = a1['w'-'a'] = w;
	a1['b'-'a'] = a1['v'-'a'] = v;
	a1['a'-'a'] = a1['u'-'a'] = u;
	a1['x'-'a'] = a1['r'-'a'] = x;
	a1['y'-'a'] = a1['n'-'a'] = y;
	a1['z'-'a'] = a1['t'-'a'] = t;
	mreal a = CalcIn(a1);
	return isfinite(a) ? a : NAN;
}
//-----------------------------------------------------------------------------
// evaluate formula for arbitrary set of variables
mreal mglFormula::Calc(const mreal var[MGL_VS]) const
{
	Error=0;
	mreal a = CalcIn(var);
	return isfinite(a) ? a : NAN;
}
//-----------------------------------------------------------------------------
// evaluate derivate of formula respect to 'diff' variable for arbitrary set of other variables
mreal mglFormula::CalcD(const mreal var[MGL_VS], char diff) const
{
	Error=0;
	mreal a = CalcDIn(diff-'a', var);
	return isfinite(a) ? a : NAN;
}
//-----------------------------------------------------------------------------
double cand(double a,double b)	{return a&&b?1:0;}
double cor(double a,double b)	{return a||b?1:0;}
double ceq(double a,double b)	{return a==b?1:0;}
double clt(double a,double b)	{return a<b?1:0;}
double cgt(double a,double b)	{return a>b?1:0;}
double add(double a,double b)	{return a+b;}
double sub(double a,double b)	{return a-b;}
double mul(double a,double b)	{return a&&b?a*b:0;}
double div(double a,double b)	{return b?a/b:NAN;}
double ipw(double a,double b)	{return mgl_ipow(a,int(b));}
double llg(double a,double b)	{return log(a)/log(b);}
#ifndef NO_GSL
double gslEllE(double a,double b)	{return gsl_sf_ellint_E(a,b,GSL_PREC_SINGLE);}
double gslEllF(double a,double b)	{return gsl_sf_ellint_F(a,b,GSL_PREC_SINGLE);}
double gslLegP(double a,double b)	{return gsl_sf_legendre_Pl(int(a),b);}
double gslEllEc(double a)	{return gsl_sf_ellint_Ecomp(a,GSL_PREC_SINGLE);}
double gslEllFc(double a)	{return gsl_sf_ellint_Kcomp(a,GSL_PREC_SINGLE);}
double gslAi(double a)	{return gsl_sf_airy_Ai(a,GSL_PREC_SINGLE);}
double gslBi(double a)	{return gsl_sf_airy_Bi(a,GSL_PREC_SINGLE);}
double gslAi_d(double a)	{return gsl_sf_airy_Ai_deriv(a,GSL_PREC_SINGLE);}
double gslBi_d(double a)	{return gsl_sf_airy_Bi_deriv(a,GSL_PREC_SINGLE);}
#endif
double sgn(double a)	{return a<0 ? -1:(a>0?1:0);}
double stp(double a)	{return a>0 ? 1:0;}
double arg(double a,double b)	{	return atan2(b,a);	}
double mgz1(double)	{return 0;}
double mgz2(double,double)	{return 0;}
#ifdef WIN32
double asinh(double x)	{	return log(x+sqrt(x*x+1));	}
double acosh(double x)	{	return x>1 ? log(x+sqrt(x*x-1)) : NAN;	}
double atanh(double x)	{	return fabs(x)<1 ? log((1+x)/(1-x))/2 : NAN;	}
#endif
//-----------------------------------------------------------------------------
typedef double (*func_1)(double);
typedef double (*func_2)(double, double);
// evaluation of embedded (included) expressions
mreal mglFormula::CalcIn(const mreal *a1) const
{
	func_2 f2[22] = {clt,cgt,ceq,cor,cand,add,sub,mul,div,ipw,pow,fmod,llg,arg
#ifndef NO_GSL
			,gsl_sf_bessel_Jnu,gsl_sf_bessel_Ynu,
			gsl_sf_bessel_Inu,gsl_sf_bessel_Knu,
			gslEllE,gslEllF,gslLegP,gsl_sf_beta
#else
			,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2
#endif
		};
	func_1 f1[42] = {sin,cos,tan,asin,acos,atan,sinh,cosh,tanh,
					asinh,acosh,atanh,sqrt,exp,log,log10,sgn,stp,floor,fabs
#ifndef NO_GSL
			,gsl_sf_dilog,gslEllEc,gslEllFc,gslAi,gslBi,gsl_sf_erf,
			gsl_sf_expint_3,gsl_sf_expint_Ei,gsl_sf_expint_E1,gsl_sf_expint_E2,
			gsl_sf_Si,gsl_sf_Ci,gsl_sf_gamma,gsl_sf_psi,gsl_sf_lambert_W0,
			gsl_sf_lambert_Wm1,gsl_sf_sinc,gsl_sf_zeta,gsl_sf_eta,gslAi_d,gslBi_d,
			gsl_sf_dawson
#else
			,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,
			mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1
#endif
		};
//	if(Error)	return 0;
	if(Kod<EQ_LT)
	{
		if(Kod==EQ_RND)	return mgl_rnd();
		else	return (Kod==EQ_A) ? a1[int(Res)] : Res;
	}
	double a = Left->CalcIn(a1);
	if(!isnan(a))
	{
		if(Kod<EQ_SIN)
		{
			double b = Right->CalcIn(a1);
			return !isnan(b) ? f2[Kod-EQ_LT](a,b) : NAN;
		}
		else if(Kod<EQ_SN)	return f1[Kod-EQ_SIN](a);
#ifndef NO_GSL
		else if(Kod<=EQ_DC)
		{
			double sn=0,cn=0,dn=0,b = Right->CalcIn(a1);
			if(isnan(b))	return NAN;
			gsl_sf_elljac_e(a,b, &sn, &cn, &dn);
			switch(Kod)
			{
			case EQ_SN:		return sn;
			case EQ_SC:		return sn/cn;
			case EQ_SD:		return sn/dn;
			case EQ_CN:		return cn;
			case EQ_CS:		return cn/sn;
			case EQ_CD:		return cn/dn;
			case EQ_DN:		return dn;
			case EQ_DS:		return dn/sn;
			case EQ_DC:		return dn/cn;
			case EQ_NS:		return 1./sn;
			case EQ_NC:		return 1./cn;
			case EQ_ND:		return 1./dn;
			}
		}
#endif
	}
	return NAN;
}
//-----------------------------------------------------------------------------
double mgp(double ,double )	{return 1;}
double mgm(double ,double )	{return -1;}
double mul1(double ,double b)	{return b;}
double mul2(double a,double )	{return a;}
double div1(double ,double b)	{return b?1/b:NAN;}
double div2(double a,double b)	{return b?-a/(b*b):NAN;}
double ipw1(double a,double b)	{return int(b)*mgl_ipow(a,int(b-1));}
double pow1(double a,double b)	{return b*pow(a,b-1);}
double pow2(double a,double b)	{return log(a)*pow(a,b);}
double llg1(double a,double b)	{return 1/(a*log(b));}
double llg2(double a,double b)	{return -log(a)/(b*log(b)*log(b));}
double cos_d(double a)	{return -sin(a);}
double tan_d(double a)	{return 1./(cos(a)*cos(a));}
double asin_d(double a)	{return 1./sqrt(1-a*a);}
double acos_d(double a)	{return -1./sqrt(1-a*a);}
double atan_d(double a)	{return 1./(1+a*a);}
double tanh_d(double a)	{return 1./(cosh(a)*cosh(a));}
double atanh_d(double a){return 1./(1-a*a);}
double asinh_d(double a){return 1./sqrt(1+a*a);}
double acosh_d(double a){return 1./sqrt(a*a-1);}
double sqrt_d(double a)	{return 0.5/sqrt(a);}
double log10_d(double a){return M_LN10/a;}
double log_d(double a)	{return 1/a;}
double erf_d(double a)	{return 2*exp(-a*a)/sqrt(M_PI);}
double dilog_d(double a){return log(a)/(1-a);}
double ei_d(double a)	{return exp(a)/a;}
double si_d(double a)	{return a?sin(a)/a:1;}
double ci_d(double a)	{return cos(a)/a;}
double exp3_d(double a)	{return exp(-a*a*a);}
double e1_d(double a)	{return exp(-a)/a;}
double sinc_d(double a)	{return a ? (cos(M_PI*a)/a-sin(M_PI*a)/(M_PI*a*a)) : 0;}
#ifndef NO_GSL
double e2_d(double a)	{return -gsl_sf_expint_E1(a);}
double gslJnuD(double a,double b)	{return 0.5*(gsl_sf_bessel_Jnu(a-1,b)-gsl_sf_bessel_Jnu(a+1,b));}
double gslYnuD(double a,double b)	{return 0.5*(gsl_sf_bessel_Ynu(a-1,b)-gsl_sf_bessel_Ynu(a+1,b));}
double gslKnuD(double a,double b)	{return -(a*gsl_sf_bessel_Knu(a,b)/b +gsl_sf_bessel_Knu(a-1,b));}
double gslInuD(double a,double b)	{return -(a*gsl_sf_bessel_Inu(a,b)/b -gsl_sf_bessel_Inu(a-1,b));}
double gslEllE1(double a,double b)	{return sqrt(1-sin(a)*sin(a)*b);}
double gslEllE2(double a,double b)	{return (gsl_sf_ellint_E(a,b,GSL_PREC_SINGLE) - gsl_sf_ellint_F(a,b,GSL_PREC_SINGLE))/(2*b);}
double gslEllF1(double a,double b)	{return 1./sqrt(1-sin(a)*sin(a)*b);}
double gslEllF2(double a,double b)	{return (gsl_sf_ellint_E(a,b,GSL_PREC_SINGLE) - gsl_sf_ellint_F(a,b,GSL_PREC_SINGLE)*(1-b))/(2*b*(1-b)) - sin(2*a)/(sqrt(1-sin(a)*sin(a)*b)*2*(1-b));}
double gslE_d(double a)	{return (gsl_sf_ellint_Ecomp(a,GSL_PREC_SINGLE) - gsl_sf_ellint_Kcomp(a,GSL_PREC_SINGLE))/(2*a);}
double gslK_d(double a)	{return (gsl_sf_ellint_Ecomp(a,GSL_PREC_SINGLE) - (1-a)*gsl_sf_ellint_Kcomp(a,GSL_PREC_SINGLE))/(2*a*(1-a));}
double gamma_d(double a)	{return gsl_sf_psi(a)*gsl_sf_gamma(a);}
#endif
//-----------------------------------------------------------------------------
// evaluation of derivative of embedded (included) expressions
mreal mglFormula::CalcDIn(int id, const mreal *a1) const
{
	func_2 f21[22] = {mgz2,mgz2,mgz2, mgz2,mgz2,mgp, mgp,mul1,div1, ipw1,pow1,mgp,llg1, mgz2// TODO deriv of arg!
#ifndef NO_GSL
			,mgz2,mgz2,mgz2, mgz2,gslEllE1,gslEllF2, mgz2,mgz2
#else
			,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2
#endif
		};
	func_2 f22[22] = {mgz2,mgz2,mgz2,mgz2,mgz2,mgp,mgm,mul2,div2,mgz2,pow2,mgz2,llg2, mgz2 // TODO deriv of arg!
#ifndef NO_GSL
			,gslJnuD,gslYnuD,gslInuD,gslKnuD,gslEllE2,gslEllF2,mgz2/*gslLegP*/,mgz2
#else
			,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2
#endif
		};
	func_1 f11[42] = {cos,cos_d,tan_d,asin_d,acos_d,atan_d,cosh,sinh,tanh_d,
					asinh_d,acosh_d,atanh_d,sqrt_d,exp,log_d,log10_d,mgz1,mgz1,mgz1,sgn
#ifndef NO_GSL
			,dilog_d,gslE_d,gslK_d,gslAi_d,gslBi_d,erf_d,exp3_d,ei_d,e1_d,e2_d,
			si_d,ci_d,gamma_d,gsl_sf_psi_1,mgz1,mgz1,sinc_d,mgz1,mgz1,mgz1,mgz1,mgz1
#else
			,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,
			mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1
#endif
		};
//	if(Error)	return 0;
	if(Kod<EQ_LT)	return (Kod==EQ_A && id==(int)Res)?1:0;

	double a = Left->CalcIn(a1), d = Left->CalcDIn(id,a1);
	if(!isnan(a) && !isnan(d))
	{
		if(Kod<EQ_SIN)
		{
			double b = Right->CalcIn(a1);
			return !isnan(b) ? f21[Kod-EQ_LT](a,b)*d + f22[Kod-EQ_LT](a,b)*Right->CalcDIn(id,a1) : NAN;
		}
		else if(Kod<EQ_SN)	return f11[Kod-EQ_SIN](a)*d;
#ifndef NO_GSL
		else if(Kod<=EQ_DC)
		{
			double sn=0,cn=0,dn=0,b = Right->CalcIn(a1);
			if(isnan(b))	return NAN;
			gsl_sf_elljac_e(a,b, &sn, &cn, &dn);
			switch(Kod)	// At this moment parse only differentiation or argument NOT mu !!!
			{
			case EQ_SN:		return cn*dn*d;
			case EQ_SC:		return dn*d/(cn*cn);
			case EQ_SD:		return cn*d/(dn*dn);
			case EQ_CN:		return -dn*sn*d;
			case EQ_CS:		return dn*d/(sn*sn);
			case EQ_CD:		return (b-1)*d*sn/(dn*dn);
			case EQ_DN:		return -b*d*cn*sn;
			case EQ_DS:		return -cn*d/(sn*sn);
			case EQ_DC:		return (1-b)*sn*d/(cn*cn);
			case EQ_NS:		return -cn*dn*d/(sn*sn);
			case EQ_NC:		return dn*sn*d/(cn*cn);
			case EQ_ND:		return b*cn*sn*d/(dn*dn);
			}
		}
#endif
	}
	return NAN;
}
//-----------------------------------------------------------------------------
// проверка корректности скобок
bool mglCheck(char *str,int n)
{
	register long s = 0,i;
	for(i=0;i<n;i++)
	{
		if(str[i]=='(')	s++;
		if(str[i]==')') s--;
		if(s<0)	return false;
	}
	return (s==0) ? true : false;
}
//-----------------------------------------------------------------------------
// поиск одного из символов lst в строке str
int mglFindInText(char *str,const char *lst)
{
	register long l=0,r=0,i;//,j,len=strlen(lst);
	for(i=strlen(str)-1;i>=0;i--)
	{
		if(str[i]=='(') l++;
		if(str[i]==')') r++;
		if(l==r && strchr(lst,str[i]))	return i;
	}
	return -1;
}
//-----------------------------------------------------------------------------
