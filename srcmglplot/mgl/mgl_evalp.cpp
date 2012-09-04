/***************************************************************************
 * mgl_evalp.cpp is part of Math Graphic Library
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
#include <wchar.h>
#include "mgl/mgl_parse.h"
#ifndef NO_GSL
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>
#endif
//-----------------------------------------------------------------------------
double mgl_ipow(double x,int n);
void mgl_wcstrim(wchar_t *str);
void mgl_wcslwr(wchar_t *str);
void mgl_wcstombs(char *dst, const wchar_t *src, int size);
int mglFormulaError;
mglData mglFormulaCalc(const wchar_t *string, mglParse *arg);
//-----------------------------------------------------------------------------
mglData mglApplyOper(const wchar_t *a1, const wchar_t *a2, mglParse *arg, double (*func)(double,double))
{
	const mglData &a = mglFormulaCalc(a1,arg), &b = mglFormulaCalc(a2,arg);
	long n = mgl_max(a.nx,b.nx), m = mgl_max(a.ny,b.ny), l = mgl_max(a.nz,b.nz);
	mglData r(n, m, l);
	register int i,j,k;
	if(b.nx*b.ny*b.nz==1)	for(i=0;i<a.nx*a.ny*a.nz;i++)
		r.a[i] = func(a.a[i],b.a[0]);
	else if(a.nx*a.ny*a.nz==1)	for(i=0;i<b.nx*b.ny*b.nz;i++)
		r.a[i] = func(a.a[0],b.a[i]);
	else if(a.nx==b.nx && b.ny==a.ny && b.nz==a.nz)	for(i=0;i<n*m*l;i++)
		r.a[i] = func(a.a[i], b.a[i]);
	else if(a.nx==b.nx && b.ny*b.nz==1)	for(i=0;i<n;i++)	for(j=0;j<a.ny*a.nz;j++)
		r.a[i+n*j] = func(a.a[i+n*j], b.a[i]);
	else if(a.nx==b.nx && a.ny*a.nz==1)	for(i=0;i<n;i++)	for(j=0;j<b.ny*b.nz;j++)
		r.a[i+n*j] = func(a.a[i], b.a[i+n*j]);
	else if(a.nx==b.nx && b.ny==a.ny && b.nz==1)
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	for(k=0;k<a.nz;k++)
			r.a[i+n*(j+m*k)] = func(a.a[i+n*(j+m*k)], b.a[i+n*j]);
	else if(a.nx==b.nx && b.ny==a.ny && a.nz==1)
		for(i=0;i<n;i++)	for(j=0;j<m;j++)	for(k=0;k<b.nz;k++)
			r.a[i+n*(j+m*k)] = func(a.a[i+n*j], b.a[i+n*(j+m*k)]);
	return r;
}
//-----------------------------------------------------------------------------
bool mglCheck(wchar_t *str,int n)
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
int mglFindInText(wchar_t *str,const char *lst)
{
	register long l=0,r=0,i;//,j,len=strlen(lst);
	for(i=wcslen(str)-1;i>=0;i--)
	{
		if(str[i]=='(') l++;
		if(str[i]==')') r++;
		if(l==r && strchr(lst,str[i]))	return i;
	}
	return -1;
}
//-----------------------------------------------------------------------------
double cand(double a,double b);//	{return a&&b?1:0;}
double cor(double a,double b);//	{return a||b?1:0;}
double ceq(double a,double b);//	{return a==b?1:0;}
double clt(double a,double b);//	{return a<b?1:0;}
double cgt(double a,double b);//	{return a>b?1:0;}
double add(double a,double b);//	{return a+b;}
double sub(double a,double b);//	{return a-b;}
double mul(double a,double b);//	{return a&&b?a*b:0;}
double div(double a,double b);//	{return b?a/b:NAN;}
double ipw(double a,double b);//	{return mgl_ipow(a,int(b));}
double llg(double a,double b);//	{return log(a)/log(b);}
double asinh(double x);//	{	return log(x+sqrt(x*x+1));	}
double acosh(double x);//	{	return x>1 ? log(x+sqrt(x*x-1)) : NAN;	}
double atanh(double x);//	{	return fabs(x)<1 ? log((1+x)/(1-x))/2 : NAN;	}
double gslEllE(double a,double b);//	{return gsl_sf_ellint_E(a,b,GSL_PREC_SINGLE);}
double gslEllF(double a,double b);//	{return gsl_sf_ellint_F(a,b,GSL_PREC_SINGLE);}
double gslLegP(double a,double b);//	{return gsl_sf_legendre_Pl(int(a),b);}
//-----------------------------------------------------------------------------
/// Parse string and substitute the script argument
// All numbers are presented as mglData(1). Do boundary checking.
// NOTE: In any case where number is required the mglData::a[0] is used.
// String flag is binary 0x1 -> 'x', 0x2 -> 'y', 0x4 -> 'z'
// NOTE: the speed is not a goal (mglFormula is faster). It is true interpreter!
mglData mglFormulaCalc(const wchar_t *string, mglParse *arg)
{
#ifndef NO_GSL
	gsl_set_error_handler_off();
#endif
	mglData res;
	if(!string || !(*string) || mglFormulaError)	return res;	// nothing to parse
	wchar_t *str = new wchar_t[wcslen(string)+1];
	wcscpy(str,string);
	static wchar_t Buf[2048];
	long n,len;
	mgl_wcstrim(str);	//	mgl_wcslwr(str);
	len=wcslen(str);
	if(str[0]=='(' && mglCheck(&(str[1]),len-2))	// remove braces
	{
		wcscpy(Buf,str+1);
		len-=2;	Buf[len]=0;
		wcscpy(str,Buf);
	}
	len=wcslen(str);
	if(str[0]=='[')	// this is manual subdata
	{
		mglData a1;
		long i, j, br=0,k;
		bool ar=true,mt=false;
		for(i=1,j=1;i<len-1;i++)
		{
			if(str[i]=='[')	br++;
			if(str[i]==']' && br>0)	br--;
			if(str[i]==',' && !br)
			{
				wcscpy(Buf,str+j);	Buf[i-j]=0;
				a1=mglFormulaCalc(Buf, arg);
				if(j==1)
				{	res = a1;	ar = (a1.nx==1);	mt = (a1.nx>1 && a1.ny==1);	}
				else
				{
					if(ar)		// res 1d array
					{	k = res.nx;	res.InsertColumns(k);	res.Put(a1,k);	}
					else if(mt)	// res 2d array
					{	k = res.ny;	res.InsertRows(k);	res.Put(a1,-1,k);	}
					else		// res 3d array
					{	k = res.nz;	res.InsertSlices(k);	res.Put(a1,-1,-1,k);	}
				}
				j=i+1;
			}
		}
		wcscpy(Buf,str+j);	Buf[i-j]=0;
		a1=mglFormulaCalc(Buf, arg);
		if(j==1)
		{	res = a1;	ar = (a1.nx==1);	mt = (a1.nx>1 && a1.ny==1);	}
		else
		{
			if(ar)		// res 1d array
			{	k = res.nx;	res.InsertColumns(k);	res.Put(a1,k);	}
			else if(mt)	// res 2d array
			{	k = res.ny;	res.InsertRows(k);	res.Put(a1,-1,k);	}
			else		// res 3d array
			{	k = res.nz;	res.InsertSlices(k);	res.Put(a1,-1,-1,k);	}
		}
		return res;
	}

	n=mglFindInText(str,"&|");				// lowest priority -- logical
	if(n>=0)
	{
		wcscpy(Buf,str);	Buf[n]=0;
		res = mglApplyOper(Buf,Buf+n+1,arg, str[n]=='|'?cor:cand);
		delete []str;		return res;
	}
	n=mglFindInText(str,"<>=");				// low priority -- conditions
	if(n>=0)
	{
		wcscpy(Buf,str);	Buf[n]=0;
		if(str[n]=='<')			res = mglApplyOper(Buf,Buf+n+1,arg, clt);
		else if(str[n]=='>')	res = mglApplyOper(Buf,Buf+n+1,arg, cgt);
		else	res = mglApplyOper(Buf,Buf+n+1,arg, ceq);
		delete []str;		return res;
	}
	n=mglFindInText(str,"+-");				// normal priority -- additions
	if(n>=0 && (n<2 || str[n-1]!='e' || str[n-2]<'0' || str[n-2]>'9'))
	{
		wcscpy(Buf,str);	Buf[n]=0;
		res = mglApplyOper(Buf,Buf+n+1,arg, str[n]=='+'?add:sub);
		delete []str;		return res;
	}
	n=mglFindInText(str,"*/");				// high priority -- multiplications
	if(n>=0)
	{
		wcscpy(Buf,str);	Buf[n]=0;
		if(str[n]=='*')	res = mglApplyOper(Buf,Buf+n+1,arg, mul);
		else			res = mglApplyOper(Buf,Buf+n+1,arg, div);
		delete []str;		return res;
	}
	n=mglFindInText(str,"^");				// highest priority -- power
	if(n>=0)
	{
		wcscpy(Buf,str);	Buf[n]=0;
		res = mglApplyOper(Buf,Buf+n+1,arg, ipw);
		delete []str;		return res;
	}
	n=mglFindInText(str,":");				// highest priority -- array
	if(n>=0 && wcscmp(str,L":"))
	{
		wcscpy(Buf,str);	Buf[n]=0;
		mglData a1=mglFormulaCalc(Buf, arg);
		mglData a2=mglFormulaCalc(Buf+n+1, arg);
		res.Create(abs(int(a2.a[0]+0.5)-int(a1.a[0]+0.5))+1);
		res.Fill(a1.a[0], a2.a[0]);
		delete []str;		return res;
	}
	n=mglFindInText(str,".");				// highest priority -- suffixes
	if(n>=0)
	{
		wcscpy(Buf,str);	Buf[n]=0;
		mreal x,y,z,k,v=NAN;
		mglData d = mglFormulaCalc(Buf, arg);
		const wchar_t *p=Buf+n+1;
		if(!wcscmp(p,L"a"))			v = d.a[0];
		else if(!wcscmp(p,L"fst"))	{	int i=-1,j=-1,k=-1;	v = d.Find(0,i,j,k);	}
		else if(!wcscmp(p,L"lst"))	{	int i=-1,j=-1,k=-1;	v = d.Last(0,i,j,k);	}
		else if(!wcscmp(p,L"nx"))	v=d.nx;
		else if(!wcscmp(p,L"ny"))	v=d.ny;
		else if(!wcscmp(p,L"nz"))	v=d.nz;
		else if(!wcscmp(p,L"max"))	v=d.Maximal();
		else if(!wcscmp(p,L"min"))	v=d.Minimal();
		else if(!wcscmp(p,L"sum"))	v=d.Momentum('x',x,y);
		else if(!wcscmp(p,L"mx"))	{	d.Maximal(x,y,z);	v=x/d.nx;	}
		else if(!wcscmp(p,L"my"))	{	d.Maximal(x,y,z);	v=y/d.ny;	}
		else if(!wcscmp(p,L"mz"))	{	d.Maximal(x,y,z);	v=z/d.nz;	}
		else if(!wcscmp(p,L"ax"))	{	d.Momentum('x',x,y);	v=x/d.nx;	}
		else if(!wcscmp(p,L"ay"))	{	d.Momentum('y',x,y);	v=x/d.ny;	}
		else if(!wcscmp(p,L"az"))	{	d.Momentum('z',x,y);	v=x/d.nz;	}
		else if(!wcscmp(p,L"wx"))	{	d.Momentum('x',x,y);	v=y/d.nx;	}
		else if(!wcscmp(p,L"wy"))	{	d.Momentum('y',x,y);	v=y/d.ny;	}
		else if(!wcscmp(p,L"wz"))	{	d.Momentum('z',x,y);	v=y/d.nz;	}
		else if(!wcscmp(p,L"sx"))	{	d.Momentum('x',x,y,z,k);	v=z/d.nx;	}
		else if(!wcscmp(p,L"sy"))	{	d.Momentum('y',x,y,z,k);	v=z/d.ny;	}
		else if(!wcscmp(p,L"sz"))	{	d.Momentum('z',x,y,z,k);	v=z/d.nz;	}
		else if(!wcscmp(p,L"kx"))	{	d.Momentum('x',x,y,z,k);	v=k/d.nx;	}
		else if(!wcscmp(p,L"ky"))	{	d.Momentum('y',x,y,z,k);	v=k/d.ny;	}
		else if(!wcscmp(p,L"kz"))	{	d.Momentum('z',x,y,z,k);	v=k/d.nz;	}
		else if(!wcscmp(p,L"aa"))	{	d.Momentum('a',x,y);	v=x;	}
		else if(!wcscmp(p,L"wa"))	{	d.Momentum('a',x,y);	v=y;	}
		else if(!wcscmp(p,L"sa"))	{	d.Momentum('a',x,y,z,k);v=z;	}
		else if(!wcscmp(p,L"ka"))	{	d.Momentum('a',x,y,z,k);v=k;	}
		// if this is valid suffix when finish parsing (it can be float number)
		if(!isnan(v))	{	res.a[0] = v;	delete []str;	return res;	}
	}
	for(n=0;n<len;n++)	if(str[n]=='(')	break;
	if(n>=len)		// this is number or variable
	{
		mglVar *v = arg->FindVar(str);
		mglNum *f = arg->FindNum(str);
		if(v)	res = v->d;
		else if(f)	res.a[0] = f->d;
		else if(!wcscmp(str,L"rnd"))	res.a[0] = mgl_rnd();
		else if(!wcscmp(str,L"nan"))	res.a[0] = NAN;
		else if(!wcscmp(str,L"pi"))		res.a[0] = M_PI;
		else if(!wcscmp(str,L"on"))		res.a[0] = 1;
		else if(!wcscmp(str,L"off"))	res.a[0] = 0;
		else if(!wcscmp(str,L":"))	res.a[0] = -1;
		else res.a[0] = wcstod(str,0);		// this is number
		delete []str;	return res;
	}
	else
	{
		register long i;
		wchar_t name[128];
		wcscpy(name,str);	name[n]=0;
		wcscpy(Buf,str+n+1);
		len=wcslen(Buf);	Buf[--len]=0;
		mglVar *v = arg->FindVar(name);
		if(!v)
		{
			if(!wcsncmp(name,L"jacobi_",7))
				memmove(name,name+7,(wcslen(name+7)+1)*sizeof(wchar_t));
		}
		if(v)	// subdata
		{
			if(Buf[0]=='\'' && Buf[len-1]=='\'')	// this is column call
			{
				char *buf = new char[len];
				Buf[len-1]=0;	mgl_wcstombs(buf, Buf+1, len-1);
				res=v->d.Column(buf);	delete []buf;
			}
			else
			{
				long m;
				mglData a1, a2, a3;
				a1.a[0] = a2.a[0] = a3.a[0] = -1;
				n=mglFindInText(Buf,",");
				if(n>0)
				{
					Buf[n]=0;	m=mglFindInText(Buf,",");
					if(m>0)
					{
						Buf[m]=0;
						a1 = mglFormulaCalc(Buf, arg);
						a2 = mglFormulaCalc(Buf+m+1, arg);
						a3 = mglFormulaCalc(Buf+n+1, arg);
					}
					else
					{
						a1 = mglFormulaCalc(Buf, arg);
						a2 = mglFormulaCalc(Buf+n+1, arg);
					}
				}
				else	a1 = mglFormulaCalc(Buf, arg);
				res = v->d.SubData(a1,a2,a3);
			}
		}
		else if(name[0]=='a')	// function
		{
			if(!wcscmp(name+1,L"sin"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = asin(res.a[i]);	}
			else if(!wcscmp(name+1,L"cos"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = acos(res.a[i]);	}
			else if(!wcscmp(name+1,L"tan"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = atan(res.a[i]);	}
			else if(!wcscmp(name+1,L"rg"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf+n+1,Buf,arg, atan2);	}
			}
			else if(!wcscmp(name+1,L"bs"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = fabs(res.a[i]);	}
#ifndef NO_GSL
			else if(!wcscmp(name+1,L"i"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_airy_Ai(res.a[i],GSL_PREC_SINGLE);	}
			else if(!wcscmp(name+1,L"iry_ai"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_airy_Ai(res.a[i],GSL_PREC_SINGLE);	}
			else if(!wcscmp(name+1,L"iry_dai"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_airy_Ai_deriv(res.a[i],GSL_PREC_SINGLE);	}
			else if(!wcscmp(name+1,L"iry_bi"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_airy_Bi(res.a[i],GSL_PREC_SINGLE);	}
			else if(!wcscmp(name+1,L"iry_dbi"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_airy_Bi_deriv(res.a[i],GSL_PREC_SINGLE);	}
		}
		else if(name[0]=='b')
		{
			if(!wcscmp(name+1,L"eta"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_beta);	}
			}
			else if(!wcscmp(name+1,L"i"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_airy_Bi(res.a[i],GSL_PREC_SINGLE);	}
#endif
		}
		else if(name[0]=='c')
		{
			if(!wcscmp(name+1,L"os"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = cos(res.a[i]);	}
			else if(!wcscmp(name+1,L"osh") || !wcscmp(name+1,L"h"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = cosh(res.a[i]);	}
#ifndef NO_GSL
			else if(!wcscmp(name+1,L"i"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_Ci(res.a[i]);	}
			else if(!wcscmp(name+1,L"essel_i"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_bessel_Inu);	}
			}
			else if(!wcscmp(name+1,L"essel_j"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_bessel_Jnu);	}
			}
			else if(!wcscmp(name+1,L"essel_k"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_bessel_Knu);	}
			}
			else if(!wcscmp(name+1,L"essel_y"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_bessel_Ynu);	}
			}
#endif
		}
		else if(name[0]=='e')
		{
			if(!wcscmp(name+1,L"xp"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = exp(res.a[i]);	}
#ifndef NO_GSL
			else if(!wcscmp(name+1,L"rf"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_erf(res.a[i]);	}
//			else if(!wcscmp(name+1,L"n"))	Kod=EQ_EN;	// NOTE: not supported
			else if(!wcscmp(name+1,L"e") || !wcscmp(name+1,L"lliptic_ec"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_ellint_Ecomp(res.a[i],GSL_PREC_SINGLE);	}
			else if(!wcscmp(name+1,L"k") || !wcscmp(name+1,L"lliptic_kc"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_ellint_Kcomp(res.a[i],GSL_PREC_SINGLE);	}
			else if(name[0]==0 || !wcscmp(name+1,L"lliptic_e"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gslEllE);	}
			}
			else if(!wcscmp(name+1,L"lliptic_f"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gslEllF);	}
			}

			else if(!wcscmp(name+1,L"i"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_expint_Ei(res.a[i]);	}
			else if(!wcscmp(name+1,L"1"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_expint_E1(res.a[i]);	}
			else if(!wcscmp(name+1,L"2"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_expint_E2(res.a[i]);	}
			else if(!wcscmp(name+1,L"ta"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_eta(res.a[i]);	}
			else if(!wcscmp(name+1,L"i3"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_expint_3(res.a[i]);	}
#endif
		}
		else if(name[0]=='l')
		{
			if(!wcscmp(name+1,L"og"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, llg);	}
			}
			else if(!wcscmp(name+1,L"g"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = log10(res.a[i]);	}
			else if(!wcscmp(name+1,L"n"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = log(res.a[i]);	}
#ifndef NO_GSL
			else if(!wcscmp(name+1,L"i2"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_dilog(res.a[i]);	}
			else if(!wcscmp(name+1,L"egendre"))
			{
				n=mglFindInText(Buf,",");
				if(n<=0)	mglFormulaError=true;
				else
				{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gslLegP);	}
			}
#endif
		}
		else if(name[0]=='s')
		{
			if(!wcscmp(name+1,L"qrt"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = sqrt(res.a[i]);	}
			else if(!wcscmp(name+1,L"in"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = sin(res.a[i]);	}
			else if(!wcscmp(name+1,L"tep"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = res.a[i]>0?1:0;	}
			else if(!wcscmp(name+1,L"ign"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = res.a[i]>0?1:(res.a[i]<0?-1:0);	}
			else if(!wcscmp(name+1,L"inh") || !wcscmp(name+1,L"h"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = sinh(res.a[i]);	}
#ifndef NO_GSL
			else if(!wcscmp(name+1,L"i"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_Si(res.a[i]);	}
			else if(!wcscmp(name+1,L"inc"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)
					res.a[i] = gsl_sf_sinc(res.a[i]);	}
#endif
		}
		else if(name[0]=='t')
		{
			if(!wcscmp(name+1,L"g") || !wcscmp(name+1,L"an"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = tan(res.a[i]);	}
			else if(!wcscmp(name+1,L"anh") || !wcscmp(name+1,L"h"))
			{	res=mglFormulaCalc(Buf, arg);
				for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = tanh(res.a[i]);	}
		}
		else if(!wcscmp(name,L"pow"))
		{
			n=mglFindInText(Buf,",");
			if(n<=0)	mglFormulaError=true;
			else
			{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, pow);	}
		}
		else if(!wcscmp(name,L"mod"))
		{
			n=mglFindInText(Buf,",");
			if(n<=0)	mglFormulaError=true;
			else
			{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, fmod);	}
		}
		else if(!wcscmp(name+1,L"int"))
		{	res=mglFormulaCalc(Buf, arg);
			for(i=0;i<res.nx*res.ny*res.nz;i++)	res.a[i] = floor(res.a[i]);	}
#ifndef NO_GSL
		else if(!wcscmp(name,L"i"))
		{
			n=mglFindInText(Buf,",");
			if(n<=0)	mglFormulaError=true;
			else
			{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_bessel_Inu);	}
		}
		else if(!wcscmp(name,L"j"))
		{
			n=mglFindInText(Buf,",");
			if(n<=0)	mglFormulaError=true;
			else
			{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_bessel_Jnu);	}
		}
		else if(!wcscmp(name,L"k"))
		{
			n=mglFindInText(Buf,",");
			if(n<=0)	mglFormulaError=true;
			else
			{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_bessel_Knu);	}
		}
		else if(!wcscmp(name,L"y"))
		{
			n=mglFindInText(Buf,",");
			if(n<=0)	mglFormulaError=true;
			else
			{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gsl_sf_bessel_Ynu);	}
		}
		else if(!wcscmp(name,L"f"))
		{
			n=mglFindInText(Buf,",");
			if(n<=0)	mglFormulaError=true;
			else
			{	Buf[n]=0;	res = mglApplyOper(Buf,Buf+n+1,arg, gslEllF);	}
		}
		else if(!wcscmp(name,L"gamma"))
		{	res=mglFormulaCalc(Buf, arg);
			for(i=0;i<res.nx*res.ny*res.nz;i++)
				res.a[i] = gsl_sf_gamma(res.a[i]);	}
		else if(!wcscmp(name,L"w0"))
		{	res=mglFormulaCalc(Buf, arg);
			for(i=0;i<res.nx*res.ny*res.nz;i++)
				res.a[i] = gsl_sf_lambert_W0(res.a[i]);	}
		else if(!wcscmp(name,L"w1"))
		{	res=mglFormulaCalc(Buf, arg);
			for(i=0;i<res.nx*res.ny*res.nz;i++)
				res.a[i] = gsl_sf_lambert_Wm1(res.a[i]);	}
		else if(!wcscmp(name,L"psi"))
		{	res=mglFormulaCalc(Buf, arg);
			for(i=0;i<res.nx*res.ny*res.nz;i++)
				res.a[i] = gsl_sf_psi(res.a[i]);	}
		else if(!wcscmp(name,L"zeta"))
		{	res=mglFormulaCalc(Buf, arg);
			for(i=0;i<res.nx*res.ny*res.nz;i++)
				res.a[i] = gsl_sf_zeta(res.a[i]);	}
		else if(!wcscmp(name,L"z"))
		{	res=mglFormulaCalc(Buf, arg);
			for(i=0;i<res.nx*res.ny*res.nz;i++)
				res.a[i] = gsl_sf_dawson(res.a[i]);	}
#endif
	}
	delete []str;	return res;
}
//-----------------------------------------------------------------------------
void mgl_wcslwr(wchar_t *str)
{
	for(long k=0;k<(long)wcslen(str);k++)	// ������� ��������� �������
		str[k] = (str[k]>='A' && str[k]<='Z') ? str[k]+'a'-'A' : str[k];
}
//-----------------------------------------------------------------------------
