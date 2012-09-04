/***************************************************************************
 * mgl_main.cpp is part of Math Graphic Library
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
#ifdef WIN32
#include <io.h>
//#include <direct.h>
#else
#include <unistd.h>
#endif
#include <stdarg.h>
#include <wchar.h>
#include "mgl/mgl.h"
#include "mgl/mgl_eval.h"
//-----------------------------------------------------------------------------
const char *mglWarn[mglWarnEnd] = {"%s: data dimension(s) is incompatible",
								"%s: data dimension(s) is too small",
								"%s: minimal data value is negative",
								"No file or wrong data dimensions",
								"Not enough memory",
								"%s: data values are zero",
								"Too many legend entries",
								"No legend entries",
								"%s: slice value is out of range",
								"%s: number of contours is zero or negative",
								"Couldn't open file %s",
								"Light: ID is out of range",
								"Setsize: size(s) is zero or negative",
								"Format %s is not supported for that build"};
//-----------------------------------------------------------------------------
mglColorID mglColorIds[] = {{'k', mglColor(0,0,0)},
	{'r', mglColor(1,0,0)},		{'R', mglColor(0.5,0,0)},
	{'g', mglColor(0,1,0)},		{'G', mglColor(0,0.5,0)},
	{'b', mglColor(0,0,1)},		{'B', mglColor(0,0,0.5)},
	{'w', mglColor(1,1,1)},		{'W', mglColor(0.7,0.7,0.7)},
	{'c', mglColor(0,1,1)},		{'C', mglColor(0,0.5,0.5)},
	{'m', mglColor(1,0,1)},		{'M', mglColor(0.5,0,0.5)},
	{'y', mglColor(1,1,0)},		{'Y', mglColor(0.5,0.5,0)},
	{'h', mglColor(0.5,0.5,0.5)},	{'H', mglColor(0.3,0.3,0.3)},
	{'l', mglColor(0,1,0.5)},	{'L', mglColor(0,0.5,0.25)},
	{'e', mglColor(0.5,1,0)},	{'E', mglColor(0.25,0.5,0)},
	{'n', mglColor(0,0.5,1)},	{'N', mglColor(0,0.25,0.5)},
	{'u', mglColor(0.5,0,1)},	{'U', mglColor(0.25,0,0.5)},
	{'q', mglColor(1,0.5,0)},	{'Q', mglColor(0.5,0.25,0)},
	{'p', mglColor(1,0,0.5)},	{'P', mglColor(0.5,0,0.25)},
	{' ', mglColor(-1,-1,-1)},	{0, mglColor(-1,-1,-1)}	// the last one MUST have id=0
};
//-----------------------------------------------------------------------------
bool mglTestMode=false;
void mglTest(const char *str, ...)
{
	if(mglTestMode)
	{
		char buf[256];
		va_list lst;
		va_start(lst,str);
		vsprintf(buf,str,lst);
		va_end(lst);
		printf("TEST: %s\n",buf);
		fflush(stdout);
	}
}
//---------------------------------------------------------------------------
void mglGraph::RecalcBorder()
{
	if(!fx &&	!fy &&	!fz)
	{	FMin = Min;	FMax = Max;	}
	else
	{
		FMin = mglPoint( 1e30, 1e30, 1e30);
		FMax = mglPoint(-1e30,-1e30,-1e30);
		register int i,j;
		for(i=0;i<51;i++)	for(j=0;j<51;j++)	// x range
		{
			SetFBord(Min.x, Min.y+i*(Max.y-Min.y)/50, Min.z+j*(Max.z-Min.z)/50);
			SetFBord(Max.x, Min.y+i*(Max.y-Min.y)/50, Min.z+j*(Max.z-Min.z)/50);
		}
		for(i=0;i<51;i++)	for(j=0;j<51;j++)	// y range
		{
			SetFBord(Min.x+i*(Max.x-Min.x)/50, Min.y, Min.z+j*(Max.z-Min.z)/50);
			SetFBord(Min.x+i*(Max.x-Min.x)/50, Max.y, Min.z+j*(Max.z-Min.z)/50);
		}
		for(i=0;i<51;i++)	for(j=0;j<51;j++)	// x range
		{
			SetFBord(Min.x+i*(Max.x-Min.x)/50, Min.y+j*(Max.y-Min.y)/50, Min.x);
			SetFBord(Min.x+i*(Max.x-Min.x)/50, Min.y+j*(Max.y-Min.y)/50, Max.z);
		}
		if(!fx)	{	FMin.x = Min.x;	FMax.x = Max.x;	}
		if(!fy)	{	FMin.y = Min.y;	FMax.y = Max.y;	}
		if(!fz)	{	FMin.z = Min.z;	FMax.z = Max.z;	}
	}
}
//-----------------------------------------------------------------------------
void mglGraph::SetFBord(mreal x,mreal y,mreal z)
{
	if(fx)
	{
		mreal v = fx->Calc(x,y,z);
		if(FMax.x < v)	FMax.x = v;
		if(FMin.x > v)	FMin.x = v;
	}
	if(fy)
	{
		mreal v = fy->Calc(x,y,z);
		if(FMax.y < v)	FMax.y = v;
		if(FMin.y > v)	FMin.y = v;
	}
	if(fz)
	{
		mreal v = fz->Calc(x,y,z);
		if(FMax.z < v)	FMax.z = v;
		if(FMin.z > v)	FMin.z = v;
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Axis(mglPoint m1, mglPoint m2, mglPoint org)
{
	if(m1.x<m2.x)	{	Min.x=m1.x;	Max.x = m2.x;	}
	if(m1.x>m2.x)	{	Min.x=m2.x;	Max.x = m1.x;	}
	if(m1.y<m2.y)	{	Min.y=m1.y;	Max.y = m2.y;	}
	if(m1.y>m2.y)	{	Min.y=m2.y;	Max.y = m1.y;	}
	if(m1.z<m2.z)	{	Min.z=m1.z;	Max.z = m2.z;	}
	if(m1.z>m2.z)	{	Min.z=m2.z;	Max.z = m1.z;	}
	Cmin = Min.z;	Cmax = Max.z;	Org = org;
	//OrgT = mglPoint(NAN,NAN,NAN);	// Let user set it up manually !
	if(AutoOrg)
	{
		if(Org.x<Min.x && !isnan(Org.x))	Org.x = Min.x;
		if(Org.x>Max.x && !isnan(Org.x))	Org.x = Max.x;
		if(Org.y<Min.y && !isnan(Org.y))	Org.y = Min.y;
		if(Org.y>Max.y && !isnan(Org.y))	Org.y = Max.y;
		if(Org.z<Min.z && !isnan(Org.z))	Org.z = Min.z;
		if(Org.z>Max.z && !isnan(Org.z))	Org.z = Max.z;
	}
	CutMin = mglPoint(0,0,0);	CutMax = mglPoint(0,0,0);
	RecalcBorder();
}
//-----------------------------------------------------------------------------
void mglGraph::SetFunc(const char *EqX,const char *EqY,const char *EqZ,const char *EqA)
{
	if(fa)	delete fa;	if(fx)	delete fx;
	if(fy)	delete fy;	if(fz)	delete fz;
	if(EqX && EqX[0] && (EqX[0]!='x' || EqX[1]!=0))
		fx = new mglFormula(EqX);
	else	fx = 0;
	if(EqY && EqY[0] && (EqY[0]!='y' || EqY[1]!=0))
		fy = new mglFormula(EqY);
	else	fy = 0;
	if(EqZ && EqZ[0] && (EqZ[0]!='z' || EqZ[1]!=0))
		fz = new mglFormula(EqZ);
	else	fz = 0;
	if(EqA && EqA[0] && ((EqA[0]!='c' && EqA[0]!='a') || EqA[1]!=0))
		fa = new mglFormula(EqA);
	else	fa = 0;
	RecalcBorder();
}
//-----------------------------------------------------------------------------
void mglGraph::CutOff(const char *EqC)
{
	if(fc)	delete fc;
	if(EqC && EqC[0])	fc = new mglFormula(EqC);	else	fc = 0;
}
//-----------------------------------------------------------------------------
//#define FLT_EPS	1.1920928955078125e-07
#define FLT_EPS	(1.+2e-07)
bool mglGraph::ScalePoint(mreal &x,mreal &y,mreal &z)
{
//	mreal x1=x,y1=y,z1=z;
	if(isnan(x) || isnan(y) || isnan(z))	return false;
	mreal x1,y1,z1,x2,y2,z2;
	x1 = x>0?x*FLT_EPS:x/FLT_EPS;	x2 = x<0?x*FLT_EPS:x/FLT_EPS;
	y1 = y>0?y*FLT_EPS:y/FLT_EPS;	y2 = y<0?y*FLT_EPS:y/FLT_EPS;
	z1 = z>0?z*FLT_EPS:z/FLT_EPS;	z2 = z<0?z*FLT_EPS:z/FLT_EPS;
	bool res = true;
	if(x2>CutMin.x && x1<CutMax.x && y2>CutMin.y && y1<CutMax.y &&
		z2>CutMin.z && z1<CutMax.z)	res = false;
	if(fc && fc->Calc(x,y,z))	res = false;

	if(Cut)
	{
//		if(x1<Min.x || x2>Max.x || y1<Min.y || y2>Max.y || z1<Min.z || z2>Max.z)	res = false;
		if((x1-Min.x)*(x1-Max.x)>0 && (x2-Min.x)*(x2-Max.x)>0)	res = false;
		if((y1-Min.y)*(y1-Max.y)>0 && (y2-Min.y)*(y2-Max.y)>0)	res = false;
		if((z1-Min.z)*(z1-Max.z)>0 && (z2-Min.z)*(z2-Max.z)>0)	res = false;
	}
	else
	{
		if(x1<Min.x)	x=Min.x;	if(x2>Max.x)	x=Max.x;
		if(y1<Min.y)	y=Min.y;	if(y2>Max.y)	y=Max.y;
		if(z1<Min.z)	z=Min.z;	if(z2>Max.z)	z=Max.z;
	}

	if(fx)	x1 = fx->Calc(x,y,z);	else	x1=x;
	if(fy)	y1 = fy->Calc(x,y,z);	else	y1=y;
	if(fz)	z1 = fz->Calc(x,y,z);	else	z1=z;
	if(isnan(x1) || isnan(y1) || isnan(z1))	return false;

	x = (2*x1 - FMin.x - FMax.x)/(FMax.x - FMin.x);
	y = (2*y1 - FMin.y - FMax.y)/(FMax.y - FMin.y);
	z = (2*z1 - FMin.z - FMax.z)/(FMax.z - FMin.z);
	if((TernAxis&3)==1)			// usual ternary axis
	{
		if(x+y>0)
		{
			if(Cut)	res = false;
			else	y = -x;
		}
		x += (y+1)/2;
	}
	else if((TernAxis&3)==2)	// quaternary axis
	{
		if(x+y+z>-1)
		{
			if(Cut)	res = false;
			else	z = -1-y-x;
		}
		x += 1+(y+z)/2;		y += (z+1)/3;
	}
/*	if(TernAxis)
	{
		if(x+y>0)
		{
			if(Cut)	res = false;
			else	y = -x;
		}
		x = x + (y+1)/2;
	}*/
	if(fabs(x)>FLT_EPS)	res = false;
	if(fabs(y)>FLT_EPS)	res = false;
	if(fabs(z)>FLT_EPS)	res = false;
	return res;
}
//-----------------------------------------------------------------------------
void mglGraph::Color(mreal a,mreal a1,mreal a2)
{
	mglColor c;
	if(a1==a2)	{	a1 = Cmin;	a2 = Cmax;	}
	mreal z = (2*a-a1-a2)/(a2-a1);
	if(z>1)	z=1;	if(z<-1)	z=-1;
	c = GetC(z,false);
	DefColor(c,AlphaDef);
}
//-----------------------------------------------------------------------------
void mglGraph::SetScheme(const char *s, bool face)
{
	unsigned i;
	mglColor c;
	const char *col = "kwrgbcymhWRGBCYMHlenpquLENPQU";
	if(!s || s[0]==0 || s[0]==':')		return;
	if(face && s[0]=='|' && s[1]==0)	{	SmoothColorbar=false;	return;	}
	for(;*s;s++)	if(strchr(col,*s))	break;
	if(s[0]==0)	return;
	strcpy(last_style, s);
	NumCol = 0;
	for(i=0;i<MGL_CMAP_COLOR;i++)	cmap[i] = NC;
	cmap[0] = mglColor(0,0,0);	cmap[1] = mglColor(1,1,1);
	OnCoord = false;	SmoothColorbar = true;
	for(i=0;i<strlen(s);i++)
	{
		if(face && s[i]=='d')	OnCoord = true;
		if(face && s[i]=='|')	SmoothColorbar = false;
		else if(face && s[i]==':')	break;
		else if(strchr(col,s[i]))
		{
			if(s[i+1]>='1' && s[i+1]<='9')
			{	cmap[NumCol].Set(s[i],(s[i+1]-'0')/5.f);	i++;	}
			else	cmap[NumCol].Set(s[i]);
			NumCol++;
			if(NumCol>=MGL_CMAP_COLOR)	break;
		}
		else
		{
			mglColor c1(s[i]);
			if(c1.r!=-1)
			{
				cmap[NumCol].Set(s[i]);
				NumCol++;
				if(NumCol>=MGL_CMAP_COLOR)	break;
			}
		}
	}
	if(NumCol==0)	NumCol = 2;
}
//-----------------------------------------------------------------------------
mglColor mglGraph::GetC(mreal x,mreal y,mreal z,bool simple)
{
	mglColor c,m;
	mreal n;
	if(OnCoord && !simple && NumCol>2)
	{
		m = cmap[0]+cmap[1]+cmap[2];		n = m.Norm();
		c = cmap[0]*((x+1)/(2*n)) + cmap[1]*((y+1)/(2*n)) + cmap[2]*((z+1)/(2*n));
	}
	else	c = GetC(z);
	return c;
}
//-----------------------------------------------------------------------------
mreal mglGraph::GetA(mreal a)
{
	mreal Amin=Cmin, Amax=Cmax;
	if(fa)
	{
		Amin = fa->Calc(0,0,0,Cmin);
		Amax = fa->Calc(0,0,0,Cmax);
		a = fa->Calc(0,0,0,a);
	}
	a = (2*a-Amin-Amax)/(Amax-Amin);
	a = a<1?(a>-1?a:-1):1;
	return a;
}
//-----------------------------------------------------------------------------
mglColor mglGraph::GetC(mreal z, bool scale)
{
	mglColor c;
	register long n = NumCol-1;
	if(scale)	z = GetA(z);
	z = (z+1.f)/2.f;
	if(SmoothColorbar)
	{
		z *= n;
		long i = long(z);		z -= i;
		if(i<n)	c = cmap[i]*(1.f-z)+cmap[i+1]*z;
		else	c = cmap[n];
	}
	else	c = cmap[z<1 ? long(NumCol*z):NumCol-1];
	return c;
}
//-----------------------------------------------------------------------------
mglColor mglGraph::GetC2(mreal x,mreal y)
{
	mglColor c;
	mreal m=1;
	x = (2*x-Min.x)/(Max.x-Min.x);
	y = (2*y-Min.y)/(Max.y-Min.y);
	if(NumCol<1)		c = mglColor(x-x*y,y-x*y,x*y);
	else if(NumCol==1)	c = cmap[0];
	else if(NumCol==2)
	{
		if((cmap[0].r+cmap[1].r)*m>1)	 m = 1/(cmap[0].r+cmap[1].r);
		if((cmap[0].g+cmap[1].g)*m>1)	 m = 1/(cmap[0].g+cmap[1].g);
		if((cmap[0].b+cmap[1].b)*m>1)	 m = 1/(cmap[0].b+cmap[1].b);
		c = cmap[0]*(x*m) + cmap[1]*(y*m);
	}
	else if(NumCol==3)
		c = cmap[0]*(x*(1-y)) + cmap[1]*(y*(1-x)) + cmap[2]*(x*y);
	else if(NumCol>3)
		c = cmap[0]*(1-x-y+x*y) + cmap[1]*(x*(1-y)) + cmap[2]*(y*(1-x)) + cmap[3]*(x*y);
	if(c.r>1)	c.r=1;	if(c.r<0)	c.r=0;
	if(c.g>1)	c.g=1;	if(c.g<0)	c.g=0;
	if(c.b>1)	c.b=1;	if(c.b<0)	c.b=0;
	return c;
}
//-----------------------------------------------------------------------------
void mglGraph::CRange(const mglData &a,bool add, mreal fact)
{
	long n = a.nx*a.ny*a.nz;
	register long i;
	if(!add)	{	Cmin = 1e20;	Cmax = -1e20;	}
	for(i=0;i<n;i++)
	{
		if(isnan(a.a[i]))	continue;
		Cmin = Cmin<a.a[i] ? Cmin : a.a[i];
		Cmax = Cmax>a.a[i] ? Cmax : a.a[i];
	}
	if(Cmin==Cmax)	Cmax += 1;
	mreal dc = (Cmax-Cmin)*fact;
	Cmax+=dc;	Cmin-=dc;
}
//-----------------------------------------------------------------------------
void mglGraph::XRange(const mglData &a,bool add,mreal fact)
{
	long n = a.nx*a.ny*a.nz;
	register long i;
	if(!add)	{	Min.x = 1e20;	Max.x = -1e20;	}
	for(i=0;i<n;i++)
	{
		if(isnan(a.a[i]))	continue;
		Min.x = Min.x<a.a[i] ? Min.x : a.a[i];
		Max.x = Max.x>a.a[i] ? Max.x : a.a[i];
	}
	mreal dc = (Max.x-Min.x)*fact;
	Max.x+=dc;	Min.x-=dc;
	if(AutoOrg && Org.x<Min.x && !isnan(Org.x))	Org.x = Min.x;
	if(AutoOrg && Org.x>Max.x && !isnan(Org.x))	Org.x = Max.x;
	RecalcBorder();
}
//-----------------------------------------------------------------------------
void mglGraph::YRange(const mglData &a,bool add,mreal fact)
{
	long n = a.nx*a.ny*a.nz;
	register long i;
	if(!add)	{	Min.y = 1e20;	Max.y = -1e20;	}
	for(i=0;i<n;i++)
	{
		if(isnan(a.a[i]))	continue;
		Min.y = Min.y<a.a[i] ? Min.y : a.a[i];
		Max.y = Max.y>a.a[i] ? Max.y : a.a[i];
	}
	mreal dc = (Max.y-Min.y)*fact;
	Max.y+=dc;	Min.y-=dc;
	if(AutoOrg && Org.y<Min.y && !isnan(Org.y))	Org.y = Min.y;
	if(AutoOrg && Org.y>Max.y && !isnan(Org.y))	Org.y = Max.y;
	RecalcBorder();
}
//-----------------------------------------------------------------------------
void mglGraph::ZRange(const mglData &a,bool add,mreal fact)
{
	long n = a.nx*a.ny*a.nz;
	register long i;
	if(!add)	{	Min.z = 1e20;	Max.z = -1e20;	}
	for(i=0;i<n;i++)
	{
		if(isnan(a.a[i]))	continue;
		Min.z = Min.z<a.a[i] ? Min.z : a.a[i];
		Max.z = Max.z>a.a[i] ? Max.z : a.a[i];
	}
	mreal dc = (Max.z-Min.z)*fact;
	Max.z+=dc;	Min.z-=dc;
	if(AutoOrg && Org.z<Min.z && !isnan(Org.z))	Org.z = Min.z;
	if(AutoOrg && Org.z>Max.z && !isnan(Org.z))	Org.z = Max.z;
	RecalcBorder();
}
//-----------------------------------------------------------------------------
char mglGraph::SelectPen(const char *p)
{
	mglColor c = NC;
	int w=1;
	char st='-',mk=0,cc=0;

	Arrow1 = Arrow2 = '-';
	if(p && *p!=0)
	{
//		strcpy(last_style, p);
		const char *col = "wkrgbcymhRGBCYMHWlenuqpLENUQP";
		const char *stl = " -|;:ji=";
		const char *mrk = "*o+xsd.^v<>";
		const char *wdh = "123456789";
		const char *arr = "AKDTVISO_";
		for(unsigned i=0;i<strlen(p);i++)
		{
			if(strchr(stl,p[i]))	st  = p[i];
			else if(strchr(mrk,p[i]))	mk = p[i];
			else if(strchr(wdh,p[i]))	w = p[i]-'0';
			else if(strchr(col,p[i]))	{	cc=p[i];	c.Set(p[i]);	}
			else if(strchr(arr,p[i]))
			{
				if(Arrow1=='-')	Arrow1 = p[i];
				else	Arrow2 = p[i];
			}
			else
			{
				mglColor c1(p[i]);
				if(c1.r!=-1)	c = c1;
			}
		}
		if(strchr(p,'#'))
		{
			if(mk=='.')	mk = 'C';
			if(mk=='+')	mk = 'P';
			if(mk=='x')	mk = 'X';
			if(mk=='o')	mk = 'O';
			if(mk=='d')	mk = 'D';
			if(mk=='s')	mk = 'S';
			if(mk=='^')	mk = 'T';
			if(mk=='v')	mk = 'V';
			if(mk=='<')	mk = 'L';
			if(mk=='>')	mk = 'R';
			if(mk=='*')	mk = 'Y';
		}
	}
	Pen(c, st, BaseLineWidth*w);
	last_style[0]=cc?cc:(NumPal?PalNames[(CurrPal+1)%NumPal]:'k');
	last_style[1]=st;	last_style[2]=w+'0';	last_style[3]=mk;	last_style[4]=0;
	return mk;
}
//-----------------------------------------------------------------------------
void mglGraph::SetPal(const char *colors)
{
#if 0
	if(!colors || !colors[0])	// restore default if only 1 color
	{	if(NumPal<2)	colors = DefPal;	else	return;	}
	memset(Pal,0,100*sizeof(mglColor));
	memset(PalNames,0,101*sizeof(char));
	int i,n = strlen(colors),k;
	n = n<100? n:100;
	for(i=k=0;i<n;i++)
	{
		if(!strchr("wkrgbcymhRGBCYMHWlenuqpLENUQP",colors[i]))	continue;
		PalNames[k] = colors[i];	Pal[k] = mglColor(colors[i]);	k++;
	}
	NumPal = k;	//	CurrPal = 0;
	if(k<1)	SetPal(0);	// if no colors then set default palette
#endif  
}
//-----------------------------------------------------------------------------
mglColor mglGraph::GetPal()
{
//	const char *col = "wkrgbcymhRGBCYMHWlenuqpLENUQP";
	CurrPal = (CurrPal+1)%NumPal;
//	if(*last_style==0)	{	last_style[0] = PalNames[CurrPal];	last_style[1]=0;	}
//	else for
	return 	Pal[CurrPal];
}
//-----------------------------------------------------------------------------
void mglGraph::SubPlot(int nx,int ny,int m, mreal dx, mreal dy)
{
	mreal x1,x2,y1,y2;
	int mx = m%nx, my = m/nx;
	if(AutoPlotFactor)	{	dx /= 1.55;	dy /= 1.55;	}
	else	{	dx /= 2;	dy /= 2;	}
	x1 = (mx+dx)/nx;		x2 = (mx+1+dx)/nx;
	y2 = 1.f-(my+dy)/ny;	y1 = 1.f-(my+1+dy)/ny;
	InPlot(x1,x2,y1,y2,false);
}
//-----------------------------------------------------------------------------
void mglGraph::SubPlot(int nx,int ny,int m, const char *style)
{
	mreal x1,x2,y1,y2;
	int mx = m%nx, my = m/nx;
	x1 = float(mx)/nx;		x2 = float(mx+1)/nx;
	y2 = 1.f-float(my)/ny;	y1 = 1.f-float(my+1)/ny;
	InPlot(x1,x2,y1,y2,style);
}
//-----------------------------------------------------------------------------
void mglGraph::InPlot(mreal x1,mreal x2,mreal y1,mreal y2, const char *st)
{
	if(!st)		{	InPlot(x1,x2,y1,y2,false);	return;	}
	if(strchr(st,'T'))	{	y1*=0.9;	y2*=0.9;	}	// general title
	bool r = !(strchr(st,'r') || strchr(st,'R') || strchr(st,'>') || strchr(st,'g'));
	bool l = !(strchr(st,'l') || strchr(st,'L') || strchr(st,'<') || strchr(st,'g'));
	bool u = !(strchr(st,'u') || strchr(st,'U') || strchr(st,'_') || strchr(st,'g'));
	bool a = !(strchr(st,'a') || strchr(st,'A') || strchr(st,'^') || strchr(st,'g') || strchr(st,'t'));
	// let use simplified scheme -- i.e. no differences between axis, colorbar and/or title
	register mreal xs=(x1+x2)/2, ys=(y1+y2)/2, f1 = 1.3, f2 = 1.1;
	if(r && l)	{	x2=xs+(x2-xs)*f1;	x1=xs+(x1-xs)*f1;	}
	else if(r)	{	x2=xs+(x2-xs)*f1;	x1=xs+(x1-xs)*f2;	}
	else if(l)	{	x2=xs+(x2-xs)*f2;	x1=xs+(x1-xs)*f1;	}
	if(a && u)	{	y2=ys+(y2-ys)*f1;	y1=ys+(y1-ys)*f1;	}
	else if(a)	{	y2=ys+(y2-ys)*f1;	y1=ys+(y1-ys)*f2;	}
	else if(u)	{	y2=ys+(y2-ys)*f2;	y1=ys+(y1-ys)*f1;	}
	InPlot(x1,x2,y1,y2,false);
}
//-----------------------------------------------------------------------------
void mglGraph::Printf(mglPoint p,const char *str,...)
{
	char text[256];
	va_list ap;
	if (!str) return;
	va_start(ap, str);
	vsprintf(text, str, ap);
	va_end(ap);
	Puts(p,text,FontDef);
}
//-----------------------------------------------------------------------------
mreal GetX(const mglData &x, int i, int j, int k)
{
	k = k<x.nz ? k : 0;
	if(x.ny>j && x.nx>i && x.ny>1)	return x.v(i,j,k);
	else if(x.nx>i)	return x.v(i);
	return 0;
}
//-----------------------------------------------------------------------------
mreal GetY(const mglData &y, int i, int j, int k)
{
	k = k<y.nz ? k : 0;
	if(y.ny>j && y.nx>i && y.ny>1)	return y.v(i,j,k);
	else if(y.nx>j)	return y.v(j);
	return 0;
}
//-----------------------------------------------------------------------------
mreal GetZ(const mglData &z, int i, int j, int k)
{
	k = k<z.nz ? k : 0;
	if(z.ny>j && z.nx>i && z.ny>1)	return z.v(i,j,k);
	else if(z.nx>i)	return z.v(i);
	return 0;
}
//-----------------------------------------------------------------------------
void mglColor::Set(mglColor c, mreal br)
{
	if(br<0)	br=0;	if(br>2.f)	br=2.f;
	r= br<=1.f ? c.r*br : 1 - (1-c.r)*(2-br);
	g= br<=1.f ? c.g*br : 1 - (1-c.g)*(2-br);
	b= br<=1.f ? c.b*br : 1 - (1-c.b)*(2-br);
}
//-----------------------------------------------------------------------------
void mglColor::Set(char p, mreal bright)
{
	Set(-1,-1,-1);
	for(long i=0; mglColorIds[i].id; i++)
		if(mglColorIds[i].id==p)
		{	Set(mglColorIds[i].col, bright);	break;	}
}
//-----------------------------------------------------------------------------
void mglGraph::ball(mreal *p, mreal *c)
{	Ball(p[0],p[1],p[2],mglColor(c[0],c[1],c[2]),c[3]);	}
//-----------------------------------------------------------------------------
void mglGraph::ClearEq()
{
	if(fx)	delete fx;	if(fy)	delete fy;	if(fz)	delete fz;
	if(fa)	delete fa;	if(fc)	delete fc;
	fx = fy = fz = fc = fa = 0;
	RecalcBorder();
}
//-----------------------------------------------------------------------------
void mglGraph::DefaultPlotParam()
{
	LegendMarks = 1;
	FontSize = 5;			BaseLineWidth = 1;
	Ambient();				Ternary(0);
	PlotId = "frame";		SelectPen("k-1");
	SetScheme("BbcyrR");	SetPalette(MGL_DEF_PAL);
	SetTicks('x');	SetTicks('y');	SetTicks('z');	SetTicks('c');
	Axis(mglPoint(-1,-1,-1), mglPoint(1,1,1));
	SetFunc(0,0);			CutOff(0);
	SetWarn(mglWarnNone);	Message = 0;
	BarWidth = 0.7;			fit_res[0] = 0;
	MarkSize = 0.02;		ArrowSize = 0.03;
	AlphaDef = 0.5;			Transparent = true;
	FontDef[0]=0;			AxialDir = 'y';
	UseAlpha = false;		TranspType = 0;
	RotatedText = true;		fnt->gr = this;
	CloudFactor = 1;		MeshNum = 0;
	ClearLegend();			LegendBox = true;
	CutMin=mglPoint(0,0,0);	CutMax=mglPoint(0,0,0);
	AutoOrg = true;			//CurFrameId = 0;
	CirclePnts=40;	FitPnts=100;	GridPnts=50;
	DrawFace = true;		_tetx=_tety=_tetz=0;
	TuneTicks= true;		_sx=_sy=_sz = 1;
	Alpha(false);	Fog(0);	FactorPos = 1.15;
	xtt[0]=ytt[0]=ztt[0]=ctt[0]=0;
	AutoPlotFactor = true;	ScalePuts = true;
	TickLen = 0.1;	st_t = 1;	Cut = true;
	TickStl[0] = SubTStl[0] = '-';
	TickStl[1] = SubTStl[1] = 0;
	PlotFactor = AutoPlotFactor ? 1.55f :2.f;
	for(int i=0;i<10;i++)
	{	Light(i, mglPoint(0,0,1));	Light(i,false);	}
	Light(0,true);		Light(false);
	InPlot(0,1,0,1,false);	Zoom(0,0,1,1);
}
//-----------------------------------------------------------------------------
void mglGraph::SetTickStl(const char *stl, const char *sub)
{
	if(!stl || !(*stl))
	{	TickStl[0] = SubTStl[0] = 0;	}
	else if(strlen(stl)<32)
	{
		strcpy(TickStl,stl);
		if(!sub || !(*sub))		strcpy(SubTStl,stl);
		else if(strlen(sub)<32)	strcpy(SubTStl,sub);
	}
}
//-----------------------------------------------------------------------------
void mglGraph::SetTickLen(mreal tlen, mreal stt)
{	TickLen = tlen?tlen:0.1;	st_t=stt>0?stt:1;	}
//-----------------------------------------------------------------------------
void mglGraph::SimplePlot(const mglData &a, int type, const char *stl)
{
	if(a.nx<2)	{	SetWarn(mglWarnLow);	return;	}
	if(a.ny<2)	switch(type)
	{
	case 1:		Area(a,stl);	break;
	case 2:		Step(a,stl);	break;
	case 3:		Stem(a,stl);	break;
	case 4:		Bars(a,stl);	break;
	default:	Plot(a,stl);	break;
	}
	else if(a.nz<2)	switch(type)
	{
	case 1:		Dens(a,stl);	break;
	case 2:		Mesh(a,stl);	break;
	case 3:		Cont(a,stl);	break;
	default:	Surf(a,stl);	break;
	}
	else switch(type)
	{
	case 1:		DensA(a,stl);	break;
	case 2:		ContA(a,stl);	break;
	case 3:		Cloud(a,stl);	break;
	case 4:		CloudP(a,stl);	break;
	default:	Surf3(a,stl);	break;
	}
}
//-----------------------------------------------------------------------------
void mglGraph::Zoom(mreal x1, mreal y1, mreal x2, mreal y2)
{
	if(x1==x2 || y1==y2)	{	x1=y1=0;	x2=y2=1;	}
	//Clf();
	if(x1<x2)	{	zoomx1=x1;	zoomx2=x2-x1;	}
	else		{	zoomx1=x2;	zoomx2=x1-x2;	}
	if(y1<y2)	{	zoomy1=y1;	zoomy2=y2-y1;	}
	else		{	zoomy1=y2;	zoomy2=y1-y2;	}
}
//-----------------------------------------------------------------------------
void mglGraph::font_curve(long n,mreal *pp,bool *,long *nn,const wchar_t *text,
				int pos,mreal size)
{
	if(n<2 || text==0 || wcslen(text)==0)	return;
	if(!pp || !nn || n<2 || nn[0]<0)	return;
	if(size<0)	size = -size*FontSize;
	mglPoint p[25],t;
	mreal del = fnt->Width(text,"rL")*size/8.;
	del = del>1 ? del:1;
	long k=1,i,j;
	bool less;
	p[0] = mglPoint(pp[0],pp[1],pp[2]);
	string_curve(0,n,pp,nn,text,size,pos);
	for(i=1;i<n;i++)	// print it several times (for contours)
	{
		if(nn[i]<0)	continue;
		less = false;
		t = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
		for(j=0;j<k;j++)	if(Norm(t-p[j])<del)	{	less=true;	break;	}
		if(less)	continue;
		p[k] = t;	k++;
		string_curve(i,n,pp,nn,text,size,pos);
		if(k>=25)	break;
	}
}
//-----------------------------------------------------------------------------
bool same_chain(long f,long i,long *nn)
{
	long j=f;
	while(1)
	{
		j = nn[j];
		if(j==f || j<0)	return false;
		if(j==i)	return true;
	}
}
//-----------------------------------------------------------------------------
void mglGraph::string_curve(long f,long n,mreal *pp,long *nn,const wchar_t *text, mreal size, int pos)
{
	wchar_t L[2]=L"a";
	mglPoint p1,n1,p2;

	mreal w, r, ww, wg=fnt->Height("")*size/8.;//*font_factor;
	register long i,k,h;
	ScalePuts = false;

	h=3*f;	k=3*nn[f];	// print string symbol-by-symbol
	mglPoint p0(pp[h],pp[h+1],pp[h+2]),n0(pp[k]-pp[h],pp[k+1]-pp[h+1],pp[k+2]-pp[h+2]);

	for(unsigned j=0;j<wcslen(text);j++)
	{
		L[0] = text[j];		ww = Putsw(p0,n0,L,pos<0?'T':'t',size);
//printf("%lc -> %g\n",L[0], ww);
		p1 = p0+(ww/Norm(n0))*n0;
		// let find closest point
		for(r=1e10,i=0;i<n;i++)
		{
			n1 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
			w = Norm(p1-n1);
			if(w<r)	{	r=w;	k=i;	}
		}
		i=k;	k=nn[i];
		for(h=0;h<n;h++)	if(nn[h]==i)	break;	// h is previous point
		// point not in the same chain (continue by stright line)
		if(k<0 || h>=n || !same_chain(f,i,nn))	{	p0=p1;	continue;	}
		// if last point let change it to previous one
		if(k<0)	{	k=i;	i=h;	}
		p2 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
		n1 = mglPoint(pp[3*k],pp[3*k+1],pp[3*k+2])-p2;
		w = (p1-p2)*n1;
		// go in wrong direction, let turn back
		if((w<0 || w>n1*n1) && i!=h)
		{
			p2 = mglPoint(pp[3*h],pp[3*h+1],pp[3*h+2]);
			n1 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2])-p2;
			k = i;
		}
		// under maximum and should skip some points
		if(n1.x*n0.y>n1.y*n0.x && pos<0)
		{
			p1 = p0+((ww/Norm(n0))*n0) + ((wg/Norm(n0)/3)*mglPoint(n0.y,-n0.x,n0.z));
			for(r=1e10,i=0;i<n;i++)	// let find closest point
			{
				n1 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
				w = Norm(p1-n1);
				if(w<r)	{	r=w;	k=i;	}
			}
			i=k;	k=nn[i];
			for(h=0;h<n;h++)	if(nn[h]==i)	break;
			if(k<0 || h>=n || !same_chain(f,i,nn))
			{	p0 = p0+(ww/Norm(n0))*n0;	continue;	}
			if(k<0)	{	k=i;	i=h;	}
			p2 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
			n1 = mglPoint(pp[3*k],pp[3*k+1],pp[3*k+2])-p2;
			w = (p1-p2)*n1;
			if((w<0 || w>n1*n1) && i!=h)
			{
				p2 = mglPoint(pp[3*h],pp[3*h+1],pp[3*h+2]);
				n1 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2])-p2;
				k = i;
			}
		}
		// above minimum and should skip some points
		if(n1.x*n0.y<n1.y*n0.x && pos>0)
		{
			p1 = p0+((ww/Norm(n0))*n0) - ((wg/Norm(n0)/3)*mglPoint(n0.y,-n0.x,n0.z));
			for(r=1e10,i=0;i<n;i++)	// let find closest point
			{
				n1 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
				w = Norm(p1-n1);
				if(w<r)	{	r=w;	k=i;	}
			}
			i=k;	k=nn[i];
			for(h=0;h<n;h++)	if(nn[h]==i)	break;
			if(k<0 || h>=n || !same_chain(f,i,nn))
			{	p0 = p0+(ww/Norm(n0))*n0;	continue;	}
			if(k<0)	{	k=i;	i=h;	}
			p2 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2]);
			n1 = mglPoint(pp[3*k],pp[3*k+1],pp[3*k+2])-p2;
			w = (p1-p2)*n1;
			if((w<0 || w>n1*n1) && i!=h)
			{
				p2 = mglPoint(pp[3*h],pp[3*h+1],pp[3*h+2]);
				n1 = mglPoint(pp[3*i],pp[3*i+1],pp[3*i+2])-p2;
				k = i;
			}
		}
		// OK, next point will be
		p0 = p2 + n1*(((p1-p2)*n1)/(n1*n1));
		n0 = n1;
	}
	ScalePuts = true;
}
//-----------------------------------------------------------------------------
// Pure virtual functions below are defined here for compatibility with MinGW
void mglGraph::Pen(mglColor , char ,mreal ){}
void mglGraph::Light(int ,bool ){}
void mglGraph::Light(int , mglPoint , mglColor , mreal , bool, mreal ){}
void mglGraph::Clf(mglColor ){}
void mglGraph::InPlot(mreal ,mreal ,mreal ,mreal ,bool ){}
//-----------------------------------------------------------------------------
// These functions can be pure virtual but it should be empty (do nothing)
// in some of inherit classes so they are defined empty here
void mglGraph::SetSize(int ,int ){}
mreal mglGraph::GetRatio(){return 1.;}
void mglGraph::Flush(){}
void mglGraph::Finish(){}
//-----------------------------------------------------------------------------
void mglGraph::Mark(mglPoint p,char t)
{	ScalePoint(p.x,p.y,p.z);	Mark(p.x,p.y,p.z,t);	}
//-----------------------------------------------------------------------------
void mglGraph::View(mreal tetx,mreal tetz,mreal tety)
{	_tetx=tetx;	_tety=tety;	_tetz=tetz;	}
void mglGraph::Identity(bool rel)	{	InPlot(0,1,0,1,rel);	}
//-----------------------------------------------------------------------------
void mglGraph::Rotate(mreal TetX,mreal TetZ,mreal TetY)
{
	RotateN(TetX+_tetx,1.,0.,0.);
	RotateN(TetY+_tety,0.,1.,0.);
	RotateN(TetZ+_tetz,0.,0.,1.);
}
//-----------------------------------------------------------------------------
void mglGraph::Fog(mreal d, mreal dz)	{	FogDist=d;	FogDz = dz;	}
//-----------------------------------------------------------------------------
void mglGraph::Light(int n,mglPoint p, char c, mreal bright, bool infty, mreal ap)
{	Light(n,p,mglColor(c),bright,infty,ap);	}
//-----------------------------------------------------------------------------
void mglGraph::Ambient(mreal bright)	{	AmbBr = bright;	}
//-----------------------------------------------------------------------------
mglGraph::mglGraph()
{
	memset(this,0,sizeof(mglGraph));
//	xnum=ynum=znum=0;
	fit_res = new char[1024];
	fnt = new mglFont;
//	DefaultPlotParam();
}
//-----------------------------------------------------------------------------
mglGraph::~mglGraph()
{
	if(xnum)	delete []xbuf;
	if(ynum)	delete []ybuf;
	if(znum)	delete []zbuf;
	delete []fit_res;
	ClearEq();
	ClearLegend();
	delete fnt;
}
//-----------------------------------------------------------------------------
void mglGraph::SetWarn(int code, const char *who)
{
	WarnCode = code;
	if(Message && code>0 && code<mglWarnEnd)
		sprintf(Message,mglWarn[code-1],who);
	else if(Message)	Message[0]=0;
}
//-----------------------------------------------------------------------------
void mglGraph::SetFont(mglFont *f)
{
	if(f)	fnt->Copy(f);		// set new typeface and delete existed
	else	fnt->Load(MGL_DEF_FONT_NAME,0);	// restore default typeface
}
//-----------------------------------------------------------------------------
void mglGraph::Title(const wchar_t *str,const char *font,mreal size)
{
	Push();	Identity();
	mglFormula *ox=fx, *oy=fy, *oz=fz;
	fx = fy = fz = NULL;
	Text(mglPoint((Min.x+Max.x)*0.5, Max.y+(Max.y-Min.y)*0.15, (Min.z+Max.z)*0.5), str, font, size);
	fx=ox;	fy=oy;	fz=oz;
	Pop();
}
//-----------------------------------------------------------------------------
void mglGraph::Title(const char *str,const char *font,mreal size)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Title(wcs, font, size);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::Labelw(mreal x, mreal y, const wchar_t *text, const char *fnt, mreal size, bool rel)
{
	Push();	Identity(rel);
	mglFormula *ox=fx, *oy=fy, *oz=fz;
	fx = fy = fz = NULL;
	char *f = new char[strlen(fnt)+1];
	strcpy(f,fnt);
	for(int i=0;f[i];i++)	if(f[i]=='a' || f[i]=='A')	f[i]=' ';
	Text(mglPoint((Min.x+Max.x)/2+PlotFactor*(Max.x-Min.x)*(x-0.5),
				(Min.y+Max.y)/2+PlotFactor*(Max.y-Min.y)*(y-0.5),
				Max.z), text, f, size);
	delete []f;
	fx=ox;	fy=oy;	fz=oz;
	Pop();
}
//-----------------------------------------------------------------------------
void mglGraph::Label(mreal x, mreal y, const char *str, const char *fnt, mreal size, bool rel)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Labelw(x,y,wcs, fnt, size, rel);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::Label(char dir, const char *str, mreal pos, mreal size, mreal shift)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Labelw(dir, wcs, pos, size, shift);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::Puts(mglPoint p,const char *str,const char *font,mreal size,char dir,mreal shift)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Putsw(p, wcs, font, size, dir, shift);
	delete []wcs;
}
//-----------------------------------------------------------------------------
mreal mglGraph::Puts(mglPoint p,mglPoint l,const char *str,char font,mreal size)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	mreal res = Putsw(p, l, wcs, font, size);
	delete []wcs;
	return res;
}
//-----------------------------------------------------------------------------
void mglGraph::Text(mglPoint p,const char *text,const char *font,mreal size,char dir)
{
	bool rt = RotatedText;
	RotatedText = false;
	Puts(p,text,font,size,dir);
	RotatedText = rt;
}
//-----------------------------------------------------------------------------
void mglGraph::Text(mglPoint p,const wchar_t *text,const char *font,mreal size,char dir)
{
	bool rt = RotatedText;
	RotatedText = false;
	Putsw(p,text,font,size,dir);
	RotatedText = rt;
}
//-----------------------------------------------------------------------------
void mglGraph::Text(const mglData &y,const char *str,const char *font,mreal size,mreal zVal)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Text(y, wcs, font, size, zVal);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::Text(const mglData &x,const mglData &y,const char *str,const char *font,mreal size,mreal zVal)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Text(x, y, wcs, font, size, zVal);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::Text(const mglData &x,const mglData &y,const mglData &z,const char *str,const char *font,mreal size)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Text(x, y, z, wcs, font, size);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::TextMark(const mglData &x, const mglData &y, const mglData &z, const mglData &r, const char *str, const char *fnt)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	TextMark(x, y, z, r, wcs, fnt);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::TextMark(const mglData &x, const mglData &y, const mglData &r, const char *str, const char *fnt, mreal zVal)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	TextMark(x, y, r, wcs, fnt, zVal);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::TextMark(const mglData &y, const mglData &r, const char *str, const char *fnt, mreal zVal)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	TextMark(y, r, wcs, fnt, zVal);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::TextMark(const mglData &y, const char *str, const char *fnt, mreal zVal)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	TextMark(y, wcs, fnt, zVal);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::AddLegend(const char *str,const char *style)
{
	if(!str)	return;
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	AddLegend(wcs, style);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglGraph::SetFontSizePT(mreal pt, int dpi)
{	FontSize = pt*27.f/dpi;	}
//-----------------------------------------------------------------------------
mreal mglGraph::GetOrgX(char)	{	return isnan(Org.x) ? Min.x : Org.x;	}
mreal mglGraph::GetOrgY(char)	{	return isnan(Org.y) ? Min.y : Org.y;	}
mreal mglGraph::GetOrgZ(char)	{	return isnan(Org.z) ? Min.z : Org.z;	}
//-----------------------------------------------------------------------------
void mglGraph::FaceX(mreal x0, mreal y0, mreal z0, mreal wy, mreal wz, const char *stl, mreal d1, mreal d2)
{
	Face(mglPoint(x0,y0,z0), mglPoint(x0,y0+wy,z0), mglPoint(x0,y0,z0+wz), mglPoint(x0,y0+wy+d1,z0+wz+d2), stl, 2);
}
//-----------------------------------------------------------------------------
void mglGraph::FaceY(mreal x0, mreal y0, mreal z0, mreal wx, mreal wz, const char *stl, mreal d1, mreal d2)
{
	Face(mglPoint(x0,y0,z0), mglPoint(x0+wx,y0,z0), mglPoint(x0,y0,z0+wz), mglPoint(x0+wx+d1,y0,z0+wz+d2), stl, 2);
}
//-----------------------------------------------------------------------------
void mglGraph::FaceZ(mreal x0, mreal y0, mreal z0, mreal wx, mreal wy, const char *stl, mreal d1, mreal d2)
{
	Face(mglPoint(x0,y0,z0), mglPoint(x0,y0+wy,z0), mglPoint(x0+wx,y0,z0), mglPoint(x0+wx+d1,y0+wy+d2,z0), stl, 2);
}
//-----------------------------------------------------------------------------
void mglGraph::ShowImage(const char *viewer, bool keep)
{
	char fname[128], *cmd = new char [128];
	sprintf(fname,"%s.png", tmpnam(NULL));
	WritePNG(fname,"MathGL ShowImage file",false);
	if(!viewer || !viewer[0])
		viewer = MGL_DEF_VIEWER;
	if(keep)
	{
		sprintf(cmd,"%s %s &", viewer,fname);
		if(system(cmd)==-1)	printf("Error to call external viewer\n");
#ifdef WIN32
//		sleep(2);
		sprintf(cmd,"del %s", fname);
	}
	else
		sprintf(cmd,"%s %s; del %s", viewer,fname,fname);
#else
		sleep(2);
		sprintf(cmd,"rm %s", fname);
	}
	else
		sprintf(cmd,"%s %s; rm %s", viewer,fname,fname);
#endif
	if(system(cmd)==-1)	printf("Error to call external viewer\n");
	delete []cmd;
}
//-----------------------------------------------------------------------------
void mglGraph::StartGroup(const char *name, int id)
{
	char buf[128];
	sprintf(buf,"%s_%d",name,id);
	StartAutoGroup(buf);
}
//-----------------------------------------------------------------------------
void mglGraph::SetAutoRanges(mreal x1, mreal x2, mreal y1, mreal y2, mreal z1, mreal z2)
{
	if(x1!=x2)	{	Min.x = x1;	Max.x = x2;	}
	if(y1!=y2)	{	Min.y = y1;	Max.y = y2;	}
	if(z1!=z2)	{	Min.z = z1;	Max.z = z2;	}
}
//-----------------------------------------------------------------------------
void mglGraph::Colorbar(const char *sch,int where)
{
	SetScheme(sch);
	// ‘0’ - right, ‘1’ - left, ‘2’ - above, ‘3’ - under
	if(sch && strchr(sch,'>'))	where = 0;
	if(sch && strchr(sch,'<'))	where = 1;
	if(sch && strchr(sch,'^'))	where = 2;
	if(sch && strchr(sch,'_'))	where = 3;
	if(sch && strchr(sch,'A'))	{	Push();	Identity();	}
	Colorbar(where, where==0?1:0, where==2?1:0, 1, 1);
	if(sch && strchr(sch,'A'))	Pop();
}
//-----------------------------------------------------------------------------
void mglGraph::Colorbar(const mglData &v, const char *sch,int where)
{
	if(sch && strchr(sch,'>'))	where = 0;
	if(sch && strchr(sch,'<'))	where = 1;
	if(sch && strchr(sch,'^'))	where = 2;
	if(sch && strchr(sch,'_'))	where = 3;
	if(sch && strchr(sch,'A'))	{	Push();	Identity();	}
	Colorbar(v,sch,where, where==0?1:0, where==2?1:0, 1, 1);
	if(sch && strchr(sch,'A'))	Pop();
}
//-----------------------------------------------------------------------------
void mglGraph::Colorbar(int where, mreal x, mreal y, mreal w, mreal h)
{
	float d = fabs(Cmax-Cmin);
	d = floor(d*pow(10,-floor(log10(d))));
	long n = 50*(d<4?int(2*d+0.5):int(d+0.5))+1;
	if(d==1.f)	n = 101;
	mglData v(n);
	if(dc)	v.Fill(Cmin,Cmax);
	else if(Cmax>Cmin && Cmin>0)
	{	v.Fill(log(Cmin), log(Cmax));	v.Modify("exp(u)");		}
	else if(Cmin<Cmax && Cmax<0)
	{	v.Fill(log(-Cmin), log(-Cmax));	v.Modify("-exp(u)");	}
	mglColor *c=new mglColor[n];
	for(long i=0;i<n;i++)	c[i] = GetC(v.a[i]);
	colorbar(v, c, where, x, y, w, h);
	delete []c;
}
//-----------------------------------------------------------------------------
void mglGraph::Colorbar(const mglData &v, const char *sch, int where, mreal x, mreal y, mreal w, mreal h)
{
	mglColor *c=new mglColor[v.nx];
	if(!sch || !(*sch))	sch = PalNames;
	for(long i=0;i<v.nx;i++)	c[i] = mglColor(sch[i%strlen(sch)]);
	colorbar(v, c, where, x, y, w, h);
	delete []c;
}
//-----------------------------------------------------------------------------
void mglGraph::ColumnPlot(int num, int i, mreal dd)
{
	mreal d = i/(num+PlotFactor-1);
	mreal w = PlotFactor/(num+PlotFactor-1);
	InPlot(0,1,d,d+w*(1-dd),true);
}
//-----------------------------------------------------------------------------
void mglGraph::SetCoor(int how)
{
	switch(how)
	{
	case mglCartesian:	SetFunc(0,0);	break;
	case mglPolar:
		SetFunc("x*cos(y)","x*sin(y)");	break;
	case mglSpherical:
		SetFunc("x*sin(y)*cos(z)","x*sin(y)*sin(z)","x*cos(y)");	break;
	case mglParabolic:
		SetFunc("x*y","(x*x-y*y)/2");	break;
	case mglParaboloidal:
		SetFunc("(x*x-y*y)*cos(z)/2","(x*x-y*y)*sin(z)/2","x*y");	break;
	case mglOblate:
		SetFunc("cosh(x)*cos(y)*cos(z)","cosh(x)*cos(y)*sin(z)","sinh(x)*sin(y)");	break;
//		SetFunc("x*y*cos(z)","x*y*sin(z)","(x*x-1)*(1-y*y)");	break;
	case mglProlate:
		SetFunc("sinh(x)*sin(y)*cos(z)","sinh(x)*sin(y)*sin(z)","cosh(x)*cos(y)");	break;
	case mglElliptic:
		SetFunc("cosh(x)*cos(y)","sinh(x)*sin(y)");	break;
	case mglToroidal:
		SetFunc("sinh(x)*cos(z)/(cosh(x)-cos(y))","sinh(x)*sin(z)/(cosh(x)-cos(y))",
			"sin(y)/(cosh(x)-cos(y))");	break;
	case mglBispherical:
		SetFunc("sin(y)*cos(z)/(cosh(x)-cos(y))","sin(y)*sin(z)/(cosh(x)-cos(y))",
			"sinh(x)/(cosh(x)-cos(y))");	break;
	case mglBipolar:
		SetFunc("sinh(x)/(cosh(x)-cos(y))","sin(y)/(cosh(x)-cos(y))");	break;
	default:	SetFunc(0,0);	break;
	}
}
//-----------------------------------------------------------------------------
